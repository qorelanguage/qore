/*
  CallStack.h

  QORE programming language

  Copyright 2003 - 2009 David Nichols

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

#ifndef _QORE_CALLSTACK_H
#define _QORE_CALLSTACK_H

#ifdef _QORE_LIB_INTERN

#include <qore/common.h>

class CallNode {
   public:
      const char *func;
      const char *file_name;
      int start_line, end_line, type;
      QoreObject *obj;
      CallNode *next, *prev;

      DLLLOCAL CallNode(const char *f, int t, QoreObject *o);
      DLLLOCAL void objectDeref(ExceptionSink *xsink);
      DLLLOCAL QoreHashNode *getInfo() const;
};

class CallStack {
   private:
      CallNode *tail;

   public:      
      DLLLOCAL CallStack();
      DLLLOCAL ~CallStack();
      DLLLOCAL QoreListNode *getCallStack() const;
      DLLLOCAL void push(CallNode *cn);
      DLLLOCAL void pop(ExceptionSink *xsink);
      DLLLOCAL void substituteObjectIfEqual(QoreObject *o);
      DLLLOCAL QoreObject *getStackObject() const;
      DLLLOCAL QoreObject *substituteObject(QoreObject *o);
      //DLLLOCAL bool inMethod(const char *name, QoreObject *o) const;
};

#endif // _QORE_LIB_INTERN

#endif // _QORE_CALLSTACK_H
