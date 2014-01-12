/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  qore_list_private.h

  Qore Programming Language

  Copyright 2003 - 2014 David Nichols

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

#ifndef _QORE_QORELISTPRIVATE_H
#define _QORE_QORELISTPRIVATE_H

struct qore_list_private {
   AbstractQoreNode **entry;
   qore_size_t length;
   qore_size_t allocated;
   bool finalized : 1;
   bool vlist : 1;

   DLLLOCAL qore_list_private() {
      entry = 0;
      length = 0;
      allocated = 0;
      finalized = false;
      vlist = false;
   }
   DLLLOCAL ~qore_list_private() {
      assert(!length);

      if (entry)
	 free(entry);
   }
   DLLLOCAL void clear() {
      entry = 0;
      length = 0;
   }
};

#endif
