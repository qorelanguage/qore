/*
  Context.cc

  Qore programming language

  Copyright (C) 2003, 2004, 2005, 2006 David Nichols

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <qore/config.h>
#include <qore/Context.h>
#include <qore/QoreNode.h>
#include <qore/support.h>
#include <qore/Operator.h>
#include <qore/Exception.h>
#include <qore/qore_thread.h>
#include <qore/Variable.h>
#include <qore/LockedObject.h>

#include <stdlib.h>
#include <stdio.h>

#include <algorithm>
using namespace std;

Context::~Context()
{
   tracein("Context::~Context()");

#ifdef DEBUG
   if (!get_context_stack())
      run_time_error("Context::~Context(): no stack! aborting");
#endif
   update_context_stack(get_context_stack()->next);

   if (name)
      free(name);
   if (master_row_list)
   {
      free(master_row_list);
      if (group_values)
      {
	 int i;
	 
	 for (i = 0; i < max_group_pos; i++)
	 {
	    printd(5, "%d/%d: ", i, max_group_pos);
	    group_values[i].node->deref(NULL);
	    printd(5, "row_list=%08p (num_rows=%d, allocated=%d): ",
		   group_values[i].row_list,
		   group_values[i].num_rows,
		   group_values[i].allocated);
	    free(group_values[i].row_list);
	    printd(5, "done\n");
	 }
	 free(group_values);
      }
   }
   else if (row_list)
      free(row_list);

   traceout("Context::~Context()");
}

class QoreNode *Context::evalValue(char *field, class ExceptionSink *xsink)
{
   if (!value)
      return NULL;

   class QoreNode *v = value->val.hash->evalKeyExistence(field, xsink);
   if (v == (QoreNode *)-1)
   {
      xsink->raiseException("CONTEXT-EXCEPTION", "\"%s\" is not a valid key for this context", field);
      return NULL;
   }
   if (!v || v->type != NT_LIST)
   {
      if (v) v->deref(xsink);
      return NULL;
   }

   QoreNode *rv = v->val.list->eval_entry(row_list[pos], xsink);
   v->deref(xsink);
   return rv;
}

class QoreNode *Context::getRow(class ExceptionSink *xsink)
{
   printd(5, "Context::getRow() value=%08p %s\n", value, value ? value->type->name : "NULL");
   if (!value)
      return NULL;

   class Hash *h = new Hash();

   class HashIterator hi(value->val.hash);
   while (hi.next())
   {
      char *key = hi.getKey();
      //char *key = l->retrieve_entry(i)->val.String->getBuffer();
      printd(5, "Context::getRow() key=%s\n", key);
      // get list from hash
      class QoreNode *v = hi.eval(xsink);
      if (xsink->isEvent())
      {
	 h->derefAndDelete(xsink);
	 return NULL;
      }
      // set key value to list entry
      h->setKeyValue(key, v->val.list->eval_entry(row_list[pos], xsink), NULL);
      v->deref(xsink);
   }
   
   return new QoreNode(h);
}

#define ROW_BLOCK 40
static inline int in_list(class QoreNode *node, struct node_row_list_s *nlist,
			  int max, int row, ExceptionSink *xsink)
{
   int i;

   for (i = 0; i < max; i++)
      if (!compareSoft(node, nlist[i].node, xsink))
      {
	 if (xsink->isEvent()) return 0;
	 // resize array if necessary
	 if (nlist[i].num_rows == nlist[i].allocated)
	 {
	    printd(5, "%d: old row_list: %08p\n", i, nlist[i].row_list);
	    nlist[i].allocated += ROW_BLOCK;
	    nlist[i].row_list = (int *)
	       realloc(nlist[i].row_list, sizeof(int) * nlist[i].allocated);
	    printd(5, "%d: new row_list: %08p\n", i, nlist[i].row_list);
	 }
	 printd(5, "in_list() row %d added to list for unique value %d (%d)\n", row, i, nlist[i].num_rows);
	 nlist[i].row_list[nlist[i].num_rows++] = row;
	 return 1;
      }
   return 0;
}

// to sort non-existing values last
static inline int compare_templist(class Templist t1, class Templist t2)
{
   class QoreNode *rv;
   int rc;
   ExceptionSink xsink;

   //printd(5, "t1.node=%08p pos=%d t2.node=%08p pos=%d\n", t1.node, t1.pos, t2.node, t2.pos);

   if (is_nothing(t1.node))
      return 0;
   if (is_nothing(t2.node))
      return 1;

   rv = OP_LOG_LT->eval(t1.node, t2.node, &xsink);
   rc = rv->getAsInt();
   rv->deref(NULL);

   //printd(5, "t1.node->type=%s t2.node->type=%s\n", t1.node->type->name, t2.node->type->name);
   //   print_node(stderr, t1.node); printd(1," == "); print_node(stderr, t2.node);
   //   printd(5, " result = %d\n", rc);

   return rc;
}

void Context::Sort(class QoreNode *snode, int sort_type)
{
   int sense = 1, i;

   tracein("Context::Sort()");
      
   printd(5, "sorting context (%d row(s)) (type=%d)\n", 
	  //query->name,
	  max_pos, sort_type);
   Templist *list = new Templist[max_pos];
   // NOTE: Solaris CC doesn't allow non-constant array sizes
   //Templist list[max_pos];
   // get list of results to be sorted
   for (pos = 0; pos < max_pos; pos++)
   {
      list[pos].node = snode->eval(sort_xsink);
      if (sort_xsink->isEvent())
      {
	 delete [] list;
	 return;
      }
      printd(5, "Context::Sort() eval(): max=%d list[%d].node = %08p (refs=%d) pos=%d\n",
	     max_pos, pos, list[pos].node ? list[pos].node : NULL,
	     list[pos].node ? list[pos].node->reference_count() : 0,
	     row_list[pos]);
      list[pos].pos = row_list[pos];
   }

   // sort the list with STL sort
   sort(list, list + max_pos, compare_templist);

   // assign sorted row list and delete temporary results
   if (sort_type == CM_SORT_DESCENDING)
   {
      i = max_pos - 1;
      sense = -1;
   }   
   else
      i = 0;
   for (pos = 0; pos < max_pos; pos++)
   {
      row_list[pos] = list[i].pos;
      printd(5, "Context::Sort() deref(): max=%d list[%d].node = %08p (refs=%d)\n",
	     max_pos, i, list[i].node ? list[pos].node : NULL, 
	     list[pos].node ? list[i].node->reference_count() : 0);
      discard(list[i].node, sort_xsink);
      i += sense;
   }

   delete [] list;
   traceout("Context::Sort()");
}

/*
 * if exp == NULL, then it is a subcontext.  Calling with exp == NULL 
 * should only be possible if there is a parent context, so no checks are
 * needed to see if there really is a parent context.
 * The code in summary contexts is only executed once for each discrete value,
 * therefore subcontexts of summary contexts compare all values in the
 * current parent context to create the subcontext row list
 *
 * ROW_BLOCK will be used for normal row lists and summarized value row lists
 * (for now)
 */

Context::Context(char *nme, ExceptionSink *xsink, class QoreNode *exp, class QoreNode *cond, 
		 int sort_type, class QoreNode *sort, class QoreNode *summary,
		 int ignore_key)
{
   int allocated = 0;
   //int sense, lcolumn = -1, fcolumn = -1
   //class Key *key = NULL;

   tracein("Context::Context()");
   //e = ex;
   group_pos = max_group_pos = max_pos = pos = master_max_pos = 0;
   row_list = NULL;
   master_row_list = NULL;
   group_values = NULL;

   sub = !exp;
   // set up initial row list and parameters
   if (sub) // copy subcontext
   {
      // push context on stack
      next = get_context_stack();
      update_context_stack(this);

      name = next->name ? strdup(next->name) : NULL;
      value = next->value;
      max_pos = next->max_pos;
      if (max_pos)
      {
	 row_list = (int *)malloc(sizeof(int) * max_pos);
	 if (!row_list)
	 {
	    xsink->outOfMemory();
	    return;
	 }
	 memcpy(row_list, next->row_list, sizeof(int) * max_pos);
	 printd(5, "Context::Context() subcontext: max_pos=%d row_list=%08p\n", max_pos, row_list);
      }
   }
   else // copy object (query) list
   {
      name = nme ? strdup(nme) : NULL;
      value = exp->eval(xsink);

      // push context on stack
      next = get_context_stack();
      update_context_stack(this);

      if (!value)
	 return;
      if (xsink->isEvent() || value->type != NT_HASH)
      {
	 value->deref(xsink);
	 value = NULL;
	 return;
      }

      class QoreNode *fkv = value->val.hash->evalFirstKeyValue(xsink);

      if (fkv && fkv->type == NT_LIST)
      {
	 max_pos = fkv->val.list->size();
	 row_list = (int *)malloc(sizeof(int) * max_pos);
	 if (!row_list)
	 {
	    xsink->outOfMemory();
	    if (fkv)
	       fkv->deref(xsink);
	    return;
	 }

	 for (int i = 0; i < max_pos; i++)
	    row_list[i] = i;
	 printd(5, "Context::Context() object: max_pos=%d row_list=%08p\n", max_pos, row_list);
      }
      else
	 max_pos = 0;

      if (fkv)
	 fkv->deref(xsink);
   }

   printd(5, "Context::Context() %s max_pos=%d row_list=%08p\n", 
	  sub ? "<SUBCONTEXT>" : "<NORMAL>", max_pos, row_list);
   // check for nested contexts
/*
   if (!ignore_key && next && !sub &&
       (key = find_key(query, NULL, next->query, NULL, &sense)))
   {
      key->check(xsink);
      if (xsink->isEvent()) return;
      if (sense == K_DIRECT)
      {
	 lcolumn = key->column1;
	 fcolumn = key->column2;
      }
      else
      {
	 lcolumn = key->column2;
	 fcolumn = key->column1;
      }
   }
*/
   //printd(2, "Context::Context() %08p (%s) cond=%08p\n", key, key ? sense == K_DIRECT ? "direct" : "reverse" : "none", cond);
   // if there are restrictions, then evaluate each row
   if (//key || 
       cond)
   {
      // use master_row_list to hold the new row_list
      master_row_list = (int *)malloc(sizeof(int) * max_pos);
      if (!master_row_list)
      {
	 xsink->outOfMemory();
	 return;
      }

      // iterate each row in results
      for (pos = 0; pos < max_pos; pos++)
      {
	 //printd(5, "Context::Context() row iteration: %d/%d (%08p)\n", pos, max_pos, key);
	 
	 // if query is in nested context
/*
	 if (key)
	 {
	    int j, rc = 1;
	    
	    // if the parent context is a summary context
	    if (next->master_row_list)
	    {
	       // compare all rows of current context to key value in
	       // current row list of summary context
	       for (j = 0; j < next->max_pos && !*e; j++)
		  if (!(rc = compareSoft(&query->row_val[row_list[pos]][lcolumn],
		        &next->query->row_val[next->row_list[j]][fcolumn], xsink)))
	             break;	       
	    }
	    else // otherwise it is a "regular" subcontext
	       rc = compareSoft(&query->row_val[row_list[pos]][lcolumn],
			        &next->query->row_val[next->row_list[next->pos]][fcolumn], xsink);
	    if (xsink->isEvent())
	       break;
	    // if no match was found, then continue
	    if (rc)
	    {
	       printd(5, "Context::Context() row %d REJECTED (ne key match)\n", pos);
	       master_row_list[pos] = 0;
	       continue;
	    } 
	 }
*/
	 // if there are constraints to check
	 if (cond && !check_condition(cond, xsink))
	 {
	    master_row_list[pos] = 0;
	    printd(5, "Context::Context() row %d REJECTED (!cond)\n", pos);
	    continue;
	 }
	 // add row to row list
	 printd(5, "Context::Context() row %d ACCEPTED\n", pos);
	 master_row_list[pos] = 1;
      }
      // copy the list over
      pos = 0;
      for (int i = 0; i < max_pos; i++)
	 if (master_row_list[i])
	 {
	    if (i != pos)
	       row_list[pos] = row_list[i];
	    pos++;
	 }
      if (max_pos != pos)
      {
	 max_pos = pos;
	 if (max_pos)
	    row_list = (int *)realloc(row_list, sizeof(int) * max_pos);
	 else
	 {
	    free(row_list);
	    row_list = NULL;
	 }
      }
      if (master_row_list)
      {
	 free(master_row_list);
	 master_row_list = NULL;
      }
   }

   // sort if applicable
   if (sort)
   {
      sort_xsink = xsink;
      Sort(sort, sort_type);
   }
   if (xsink->isEvent())
      return;

   if (summary)
   {
      printd(4, "Context::Context() finding unique values for summary context\n");
      master_max_pos = max_pos;
      master_row_list = row_list;
      allocated = 0;
      // find unique values in summary node
      for (pos = 0; pos < master_max_pos; pos++)
      {
	 class QoreNode *node;

	 printd(5, "Context::Context() summary value %d/%d\n",
		pos, master_max_pos);
	 node = summary->eval(xsink);
	 if (xsink->isEvent())
	 {
	    if (node) node->deref(xsink);
	    break;
	 }
	 if (in_list(node, group_values, max_group_pos,
		     master_row_list[pos], xsink))
	 {
	    node->deref(xsink);
	    if (xsink->isEvent()) break;
	    continue;
	 }
	 if (xsink->isEvent()) break;
	 // resize array if necessary
	 if (max_group_pos == allocated)
	 {
	    allocated += ROW_BLOCK;
	    group_values = (struct node_row_list_s *)
	       realloc(group_values,
		       sizeof(struct node_row_list_s) * allocated);
	 }
	 // insert new value in list
	 group_values[max_group_pos].node = node;
	 group_values[max_group_pos].num_rows = 1;
	 group_values[max_group_pos].allocated = ROW_BLOCK;
	 group_values[max_group_pos].row_list = (int *)malloc(sizeof(int) * ROW_BLOCK);
	 if (!group_values[max_group_pos].row_list)
	 {
	    xsink->outOfMemory();
	    return;
	 }

	 printd(5, "%d: start row_list: %08p\n", max_group_pos, 
		group_values[max_group_pos].row_list);
	 group_values[max_group_pos].row_list[0] = 
	    master_row_list[pos];
	 printd(4, "Context::Context() row %d creating unique value list %d\n",
		master_row_list[pos], max_group_pos);
	 max_group_pos++;
      }
      // resize array to final size if necessary
      if (max_group_pos != allocated)
	 group_values = (struct node_row_list_s *)
	    realloc(group_values,
		    sizeof(struct node_row_list_s) * max_group_pos);
      // prepare first context
      if (max_group_pos)
      {
	 row_list = group_values[0].row_list;
	 max_pos = group_values[0].num_rows;
      }
   }
   pos = 0;
   printd(5, "Context::Context() max_pos = %d\n", max_pos);
   traceout("Context::Context()");
}

int Context::next_summary()
{
   printd(5, "Context::next_summary() %08p %d/%d\n", this, group_pos, max_group_pos);
   group_pos++;
   if (group_pos == max_group_pos)
      return 0;
   max_pos = group_values[group_pos].num_rows;
   row_list = group_values[group_pos].row_list;
   return 1;
}

class QoreNode *evalComplexContextRef(class ComplexContextRef *c, class ExceptionSink *xsink)
{
   int count = 0;

   Context *cs = get_context_stack();
   while (count != c->stack_offset)
   {
      count++;
      cs = cs->next;
   }
   return cs->evalValue(c->member, xsink);
}

