/*
  Hash.cc

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
#include <qore/Hash.h>

void Hash::dereference(class ExceptionSink *xsink)
{
   class HashMember *where = member_list;
   while (where)
   {
#if 0
      printd(5, "Hash::dereference() %s=%08x type=%s references=%d\n",
	     where->key ? where->key : "(null)",
	     where->node,
	     where->node ? where->node->type->name : "(null)",
	     where->node ? where->node->reference_count() : 0);
#endif
      class HashMember *om = where;
      if (where->node)
	 where->node->deref(xsink);
      where = where->next;
      if (om->key)
	 free(om->key);
      delete om;
   }
   member_list = NULL;
   tail = NULL;

#ifdef HAVE_QORE_HASH_MAP
   hm.clear();
#else
   num_elements = 0;
#endif
}

#ifdef HAVE_QORE_HASH_MAP

void Hash::deleteKey(char *key, ExceptionSink *xsink)
{
#ifdef DEBUG
   if (!key) run_time_error("Hash::deleteKey() key=NULL\n");
#endif
   hm_hm_t::iterator i = hm.find(key);

   if (i == hm.end())
      return;

   class HashMember *m = i->second;

   hm.erase(i);
   internDeleteKey(m, xsink);
}

#else  // HAVE_QORE_HASH_MAP

void Hash::deleteKey(char *key, ExceptionSink *xsink)
{
#ifdef DEBUG
   if (!key) run_time_error("Hash::deleteKey() key=NULL\n");
#endif
   class HashMember *om;

   if (!(om = findKey(key)))
      return;

   internDeleteKey(om, xsink);
}

#endif  // HAVE_QORE_HASH_MAP
