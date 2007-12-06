/*
  CallStack.h

  QORE programming language

  Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols

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
      class QoreObject *obj;
      class CallNode *next;
      class CallNode *prev;

      DLLLOCAL CallNode(const char *f, int t, class QoreObject *o);
      DLLLOCAL void objectDeref(class ExceptionSink *xsink);
      DLLLOCAL class QoreHash *getInfo() const;
};

class CallStack {
   private:
      class CallNode *tail;

   public:      
      DLLLOCAL CallStack();
      DLLLOCAL ~CallStack();
      DLLLOCAL class QoreList *getCallStack() const;
      DLLLOCAL void push(const char *f, int t, class QoreObject *o);
      DLLLOCAL void pop(class ExceptionSink *xsink);
      DLLLOCAL void substituteObjectIfEqual(class QoreObject *o);
      DLLLOCAL class QoreObject *getStackObject() const;
      DLLLOCAL class QoreObject *substituteObject(class QoreObject *o);
      DLLLOCAL bool inMethod(const char *name, class QoreObject *o) const;
};

#endif // _QORE_LIB_INTERN

#endif // _QORE_CALLSTACK_H
