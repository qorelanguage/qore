/*
  List.cc

  Qore Programming Language

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
#include <qore/List.h>
#include <qore/support.h>

#include <algorithm>
using namespace std;

static int compareListEntries(class QoreNode *l, class QoreNode *r)
{
   //printd(5, "compareListEntries(%08p, %08p) (%s %s)\n", l, r, l->type == NT_STRING ? l->val.String->getBuffer() : "?", r->type == NT_STRING ? r->val.String->getBuffer() : "?");

   // sort non-existant values last
   if (is_nothing(l))
      return 0;
   if (is_nothing(r))
      return 1;

   class ExceptionSink xsink;
   class QoreNode *rv = OP_LOG_LT->eval(l, r, &xsink);
   int rc = (int)rv->val.boolval;
   rv->deref(NULL);
   //printd(5, "compareListEntries() returning %d\n", rc);
   return rc;
}

static int compareListEntriesDescending(class QoreNode *l, class QoreNode *r)
{
   return compareListEntries(l, r) ? 0 : 1;
}

class QoreNode *List::sort() const
{
   class QoreNode *rv = copy();
   //printd(5, "List::sort() entry=%08p length=%d\n", rv->val.list->entry, length);
   std::sort(rv->val.list->entry, rv->val.list->entry + length, compareListEntries);
   return rv;
}

class QoreNode *List::sortDescending() const
{
   class QoreNode *rv = copy();
   //printd(5, "List::sort() entry=%08p length=%d\n", rv->val.list->entry, length);
   std::sort(rv->val.list->entry, rv->val.list->entry + length, compareListEntriesDescending);
   return rv;
}

// does a deep dereference
void List::dereference(ExceptionSink *xsink)
{
//   tracein("List::dereference()");
   for (int i = 0; i < length; i++)
      if (entry[i])
         entry[i]->deref(xsink);
   length = 0;
//   traceout("List::dereference()");
}

void List::resize(int num)
{
   if (num < length) // make smaller
   {
      ExceptionSink xsink;
      for (int i = length - 1; i >= num; i--)
	 if (entry[i])
	 {
	    // resize should only be called when the missing
	    // entries are already NULL
	    entry[i]->deref(&xsink);
	    entry[i] = NULL;
	 }
      //entry = (class QoreNode **)realloc(entry, sizeof (QoreNode **) * num);
      length = num;
      return;
   }
   // make larger
   if (num >= allocated)
   {
      allocated = num + LIST_PAD;
      entry = (class QoreNode **)realloc(entry, sizeof (QoreNode *) * allocated);
      for (int i = length; i < allocated; i++)
	 entry[i] = NULL;
   }
   length = num;
}

void List::splice_intern(int offset, int len, class ExceptionSink *xsink)
{
   //printd(5, "splice_intern(offset=%d, len=%d, length=%d)\n", offset, len, length);
   int end;
   if (len > (length - offset))
   {
      end = length;
      len = length - offset;
   }
   else
      end = offset + len;

   // dereference all entries that will be removed
   for (int i = offset; i < end; i++)
      if (entry[i])
	 entry[i]->deref(xsink);

   // move down entries if necessary
   if (end != length)
   {
      memmove(entry + offset, entry + end, sizeof(entry) * (length - end));
      // zero out trailing entries
      for (int i = length - len; i < length; i++)
	 entry[i] = NULL;
   }
   else // set last entry to NULL
      entry[end - 1] = NULL;
      
   resize(length - len);
}

void List::splice_intern(int offset, int len, class QoreNode *l, class ExceptionSink *xsink)
{
   //printd(5, "splice_intern(offset=%d, len=%d, length=%d)\n", offset, len, length);
   int end;
   if (len > (length - offset))
   {
      end = length;
      len = length - offset;
   }
   else
      end = offset + len;

   // dereference all entries that will be removed
   for (int i = offset; i < end; i++)
      if (entry[i])
	 entry[i]->deref(xsink);

   // get number of entries to insert
   int n;
   if (!l)
      n = 1;
   else if (l->type == NT_LIST)
      n = l->val.list->size();
   else
      n = 1;
   // difference
   if (n > len) // make bigger
   {
      int ol = length;
      resize(length - len + n);
      // move trailing entries forward if necessary
      if (end != ol)
	 memmove(entry + (end - len + n), entry + end, sizeof(entry) * (ol - end));
   }
   else if (len > n) // make list smaller
   {
      memmove(entry + offset + n, entry + offset + len, sizeof(entry) * (length - offset - n));
      // zero out trailing entries
      for (int i = length - (len - n); i < length; i++)
	 entry[i] = NULL;
      // resize list
      resize(length - (len - n));
   }

   // add in new entries
   if (!l || l->type != NT_LIST)
   {
      if (l)
	 l->ref();
      entry[offset] = l;
   }
   else
      for (int i = 0; i < n; i++)
	 entry[offset + i] = l->val.list->retrieve_entry(i) ? l->val.list->retrieve_entry(i)->RefSelf() : NULL;
}

