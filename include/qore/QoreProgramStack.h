/*
  QoreProgramStack.h

  QORE programming language

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

#ifndef _QORE_QOREPROGRAMSTACK_H

#define _QORE_QOREPROGRAMSTACK_H

class ProgramNode {
   public:
      class QoreProgram *pgm;
      inline ProgramNode(class QoreProgram *p) { pgm = p; next = NULL; }
      class QoreProgram *getInfo();
      class ProgramNode *next;
      class ProgramNode *prev;
};

class QoreProgramStack {
      class ProgramNode *tail;
   public:
      inline QoreProgramStack(class QoreProgram *p);
      inline ~QoreProgramStack();
      inline void push(class QoreProgram *p);
      inline void pop();
      class QoreProgram *getProgram() { return tail->pgm; }
};

#include <qore/thread.h>
#include <qore/support.h>

inline QoreProgramStack::QoreProgramStack(class QoreProgram *p)
{
   tail = new ProgramNode(p);
   tail->prev = NULL;
}

inline QoreProgramStack::~QoreProgramStack()
{
   while (tail)
   {
      class ProgramNode *c = tail->prev;
      delete tail;
      tail = c;
   }
}

inline void QoreProgramStack::push(class QoreProgram *p)
{
   tracein("QoreProgramStack::push()");
   printd(5, "QoreProgramStack::push(%08x)\n", p);
#ifdef DEBUG
   if (!p)
   {
      run_time_error("QoreProgramStack::push() NULL\n");
      exit(1);
   }
#endif
   ProgramNode *n = new ProgramNode(p);
   n->next = NULL;
   n->prev = tail;
   if (tail)
      tail->next = n;
   tail = n;
   traceout("QoreProgramStack::push()");
}

inline void QoreProgramStack::pop()
{
   tracein("QoreProgramStack::pop()");
   printd(5, "QoreProgramStack::pop()\n", tail->pgm);
   ProgramNode *n = tail;
   tail = tail->prev;
   if (tail)
      tail->next = NULL;
   delete n;
   traceout("QoreProgramStack::pop()");
}

#endif // _QORE_QOREPROGRAMSTACK_H
