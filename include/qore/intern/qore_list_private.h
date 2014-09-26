/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  qore_list_private.h

  Qore Programming Language

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

#ifndef _QORE_QORELISTPRIVATE_H
#define _QORE_QORELISTPRIVATE_H

struct qore_list_private {
   AbstractQoreNode** entry;
   qore_size_t length;
   qore_size_t allocated;
   unsigned obj_count;
   bool finalized : 1;
   bool vlist : 1;

   DLLLOCAL qore_list_private() : entry(0), length(0), allocated(0), obj_count(0), finalized(false), vlist(false) {
   }

   DLLLOCAL ~qore_list_private() {
      assert(!length);

      if (entry)
	 free(entry);
   }

   DLLLOCAL void incObjectCount(int dt) {
      assert(dt);
      assert(obj_count || (dt > 0));
      //printd(5, "qore_list_private::incObjectCount() this: %p dt: %d: %d -> %d\n", this, dt, obj_count, obj_count + dt);
      obj_count += dt;
   }

   DLLLOCAL static unsigned getObjectCount(const QoreListNode& l) {
      return l.priv->obj_count;
   }

   DLLLOCAL static void incObjectCount(const QoreListNode& l, int dt) {
      l.priv->incObjectCount(dt);
   }
};

#endif
