/*
 QoreProgramStack.cc
 
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

#include <qore/config.h>
#include <qore/common.h>
#include <qore/QoreProgramStack.h>
#include <qore/qore_thread.h>
#include <qore/support.h>

#include <assert.h>

ProgramNode::ProgramNode(class QoreProgram *p)
{ 
   pgm = p; 
}

QoreProgramStack::QoreProgramStack(class QoreProgram *p)
{
   tail = new ProgramNode(p);
   tail->prev = NULL;
}

QoreProgramStack::~QoreProgramStack()
{
   while (tail)
   {
      class ProgramNode *c = tail->prev;
      delete tail;
      tail = c;
   }
}

void QoreProgramStack::push(class QoreProgram *p)
{
   tracein("QoreProgramStack::push()");
   printd(5, "QoreProgramStack::push(%08p)\n", p);

   assert(p);

   ProgramNode *n = new ProgramNode(p);
   n->prev = tail;
   tail = n;
   traceout("QoreProgramStack::push()");
}

void QoreProgramStack::pop()
{
   tracein("QoreProgramStack::pop()");
   printd(5, "QoreProgramStack::pop()\n", tail->pgm);
   ProgramNode *n = tail;
   tail = tail->prev;
   delete n;
   traceout("QoreProgramStack::pop()");
}

class QoreProgram *QoreProgramStack::getProgram() const
{ 
   return tail->pgm; 
}

