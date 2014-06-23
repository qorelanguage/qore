/*
  Context.cc

  Qore programming language

  Copyright (C) 2003 - 2014 David Nichols

  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.

  Note that the Qore library is released under a choice of three open-source
  licenses: MIT (as above), LGPL 2+, or GPL 2+; see README-LICENSE for more
  information.
*/

#include <qore/Qore.h>

#include <qore/intern/QoreHashNodeIntern.h>

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include <algorithm>

class Templist {
public:
   AbstractQoreNode *node;
   int pos;
};

struct node_row_list_s {
   AbstractQoreNode *node;
   int *row_list;
   int num_rows;
   int allocated;
};

#define ROW_BLOCK 40

static inline int in_list(AbstractQoreNode *node, struct node_row_list_s *nlist, int max, int row, ExceptionSink *xsink) {
   int i;

   for (i = 0; i < max; i++)
      if (!compareSoft(node, nlist[i].node, xsink)) {
	 if (xsink->isEvent()) return 0;
	 // resize array if necessary
	 if (nlist[i].num_rows == nlist[i].allocated) {
	    printd(5, "%d: old row_list: %p\n", i, nlist[i].row_list);
	    int d = nlist[i].allocated >> 2;
	    nlist[i].allocated += (d > ROW_BLOCK ? d : ROW_BLOCK);
	    nlist[i].row_list = (int *)
	       realloc(nlist[i].row_list, sizeof(int) * nlist[i].allocated);
	    printd(5, "%d: new row_list: %p\n", i, nlist[i].row_list);
	 }
	 printd(5, "in_list() row %d added to list for unique value %d (%d)\n", row, i, nlist[i].num_rows);
	 nlist[i].row_list[nlist[i].num_rows++] = row;
	 return 1;
      }
   return 0;
}

/*
 * if exp == 0, then it is a subcontext.  Calling with exp == 0 
 * should only be possible if there is a parent context, so no checks are
 * needed to see if there really is a parent context.
 * The code in summary contexts is only executed once for each discrete value,
 * therefore subcontexts of summary contexts compare all values in the
 * current parent context to create the subcontext row list
 *
 * ROW_BLOCK will be used for normal row lists and summarized value row lists
 * (for now)
 */

Context::Context(char *nme, ExceptionSink *xsink, AbstractQoreNode *exp, AbstractQoreNode *cond, 
		 int sort_type, AbstractQoreNode *sort, AbstractQoreNode *summary,
		 int ignore_key) : value(0), master_row_list(0), row_list(0), group_values(0) {
   int allocated = 0;
   //int sense, lcolumn = -1, fcolumn = -1
   //class Key *key = 0;

   QORE_TRACE("Context::Context()");
   //e = ex;
   group_pos = max_group_pos = max_pos = pos = master_max_pos = 0;

   sub = !exp;
   // set up initial row list and parameters
   if (sub) { // copy subcontext
      // push context on stack
      next = get_context_stack();
      update_context_stack(this);

      name = next->name ? strdup(next->name) : 0;
      value = next->value;
      max_pos = next->max_pos;
      if (max_pos) {
	 row_list = (int *)malloc(sizeof(int) * max_pos);
	 if (!row_list) {
	    xsink->outOfMemory();
	    return;
	 }
	 memcpy(row_list, next->row_list, sizeof(int) * max_pos);
	 printd(5, "Context::Context() subcontext: max_pos=%d row_list=%p\n", max_pos, row_list);
      }
   }
   else { // copy object (query) list
      name = nme ? strdup(nme) : 0;
      ReferenceHolder<AbstractQoreNode> rv(exp->eval(xsink), xsink);

      // push context on stack
      next = get_context_stack();
      update_context_stack(this);

      if (*xsink)
	 return;

      value = dynamic_cast<QoreHashNode *>(*rv);
      if (!value)
	 return;

      AbstractQoreNode *fkv = qore_hash_private::getFirstKeyValue(value);

      QoreListNode *l = dynamic_cast<QoreListNode *>(fkv);
      if (l) {
	 max_pos = l->size();
	 row_list = (int *)malloc(sizeof(int) * max_pos);
	 if (!row_list)
	 {
	    xsink->outOfMemory();
	    return;
	 }

	 for (int i = 0; i < max_pos; i++)
	    row_list[i] = i;
	 printd(5, "Context::Context() object: max_pos=%d row_list=%p\n", max_pos, row_list);
      }
      else
	 max_pos = 0;
      
      rv.release();
   }

   printd(5, "Context::Context() %s max_pos=%d row_list=%p\n", 
	  sub ? "<SUBCONTEXT>" : "<NORMAL>", max_pos, row_list);
   // check for nested contexts
/*
   if (!ignore_key && next && !sub &&
       (key = find_key(query, 0, next->query, 0, &sense)))
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
   //printd(2, "Context::Context() %p (%s) cond=%p\n", key, key ? sense == K_DIRECT ? "direct" : "reverse" : "none", cond);
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
	 //printd(5, "Context::Context() row iteration: %d/%d (%p)\n", pos, max_pos, key);
	 
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
	    row_list = 0;
	 }
      }
      if (master_row_list)
      {
	 free(master_row_list);
	 master_row_list = 0;
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
	 AbstractQoreNode *node;

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

	 printd(5, "%d: start row_list: %p\n", max_group_pos, 
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

}

Context::~Context() {
   QORE_TRACE("Context::~Context()");

   assert(get_context_stack());
   update_context_stack(get_context_stack()->next);

   if (name)
      free(name);
   if (master_row_list) {
      free(master_row_list);
      if (group_values) {
	 int i;
	 
	 for (i = 0; i < max_group_pos; i++) {
	    printd(5, "%d/%d: ", i, max_group_pos);
	    group_values[i].node->deref(0);
	    printd(5, "row_list=%p (num_rows=%d, allocated=%d): ",
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
}

int Context::check_condition(AbstractQoreNode *cond, ExceptionSink *xsinkx) {
   AbstractQoreNode *val;
   int rc;
   
   QORE_TRACE("Context::check_condition()");
   val = cond->eval(xsinkx);
   if (xsinkx->isEvent()) {
      if (val) val->deref(xsinkx);
      return -1;
   }
   if (val) {
      rc = val->getAsInt();
      val->deref(xsinkx);
   }
   else
      rc = 0;

   return rc;
}

void Context::deref(ExceptionSink *xsink) {
   if (!sub && value)
      value->deref(xsink);
   delete this;
}

AbstractQoreNode *evalContextRef(const char *key, ExceptionSink *xsink) {
   class Context *c = get_context_stack();
   return c->evalValue(key, xsink);
}

AbstractQoreNode *evalContextRow(ExceptionSink *xsink) {
   return get_context_stack()->getRow(xsink);
}

AbstractQoreNode *Context::evalValue(const char *field, ExceptionSink *xsink) {
   if (!value)
      return 0;

   bool exists;
   AbstractQoreNode *v = value->getReferencedKeyValue(field, exists);
   if (!exists) {
      xsink->raiseException("CONTEXT-EXCEPTION", "\"%s\" is not a valid key for this context", field);
      return 0;
   }
   ReferenceHolder<AbstractQoreNode> val(v, xsink);
   QoreListNode *l = dynamic_cast<QoreListNode *>(v);
   if (!l)
      return 0;

   AbstractQoreNode *rv = l->retrieve_entry(row_list[pos]);
   if (rv) rv->ref();
   //printd(5, "Context::evalValue(%s) this=%p pos=%d rv=%p %s %lld\n", field, this, pos, rv, rv ? rv->getTypeName() : "none", rv && rv->getType() == NT_INT ? ((QoreBigIntNode *)rv)->val : -1);
   //printd(5, "Context::evalValue(%s) pos=%d, val=%s\n", field, pos, rv && rv->getType() == NT_STRING ? rv->val.String->getBuffer() : "?");
   return rv;
}

QoreHashNode *Context::getRow(ExceptionSink *xsink) {
   printd(5, "Context::getRow() value=%p %s\n", value, value ? value->getTypeName() : "NULL");
   if (!value)
      return 0;

   ReferenceHolder<QoreHashNode> h(new QoreHashNode(), xsink);

   HashIterator hi(value);
   while (hi.next()) {
      const char *key = hi.getKey();
      printd(5, "Context::getRow() key=%s\n", key);
      // get list from hash
      ReferenceHolder<AbstractQoreNode> v(hi.getReferencedValue(), xsink);

      // if the hash key does not contain a list, then set the value to NOTHING
      if (get_node_type(*v) != NT_LIST)
	 h->setKeyValue(key, 0, 0);
      else {
	 // set key value to list entry
	 QoreListNode *l = reinterpret_cast<QoreListNode *>(*v);
	 h->setKeyValue(key, l->eval_entry(row_list[pos], xsink), 0);
      }
   }
   
   return h.release();
}

// to sort non-existing values last
static inline int compare_templist(class Templist t1, class Templist t2) {
   //printd(5, "t1.node=%p pos=%d t2.node=%p pos=%d\n", t1.node, t1.pos, t2.node, t2.pos);

   if (is_nothing(t1.node))
      return 0;
   if (is_nothing(t2.node))
      return 1;

   ExceptionSink xsink;
   int rc = (int)OP_LOG_LT->bool_eval(t1.node, t2.node, &xsink);

   //printd(5, "t1.node->getType()=%s t2.node->getType()=%s\n", t1.node->getTypeName(), t2.node->getTypeName());
   //   print_node(stderr, t1.node); printd(1," == "); print_node(stderr, t2.node);
   //   printd(5, " result = %d\n", rc);
   return rc;
}

void Context::Sort(AbstractQoreNode *snode, int sort_type) {
   int sense = 1, i;

   QORE_TRACE("Context::Sort()");
      
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
      printd(5, "Context::Sort() eval(): max=%d list[%d].node = %p (refs=%d) pos=%d\n",
	     max_pos, pos, list[pos].node ? list[pos].node : 0,
	     list[pos].node ? list[pos].node->reference_count() : 0,
	     row_list[pos]);
      list[pos].pos = row_list[pos];
   }

   // sort the list with STL sort
   std::sort(list, list + max_pos, compare_templist);

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
      printd(5, "Context::Sort() deref(): max=%d list[%d].node = %p (refs=%d)\n",
	     max_pos, i, list[i].node ? list[pos].node : 0, 
	     list[pos].node ? list[i].node->reference_count() : 0);
      discard(list[i].node, sort_xsink);
      i += sense;
   }

   delete [] list;

}

int Context::next_summary() {
   printd(5, "Context::next_summary() %p %d/%d\n", this, group_pos, max_group_pos);
   group_pos++;
   if (group_pos == max_group_pos)
      return 0;
   max_pos = group_values[group_pos].num_rows;
   row_list = group_values[group_pos].row_list;
   return 1;
}
