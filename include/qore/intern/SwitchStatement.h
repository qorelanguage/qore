/*
  SwitchStatement.h

  Qore Programming Language

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

#ifndef _QORE_SWITCHSTATEMENT_H

#define _QORE_SWITCHSTATEMENT_H

#include "intern/AbstractStatement.h"

class CaseNode {
   private:
      DLLLOCAL virtual bool isCaseNodeImpl() const;
   
   public:
      class AbstractQoreNode *val;
      class StatementBlock *code;
      class CaseNode *next;

      DLLLOCAL CaseNode(class AbstractQoreNode *v, class StatementBlock *c);
      DLLLOCAL virtual bool matches(AbstractQoreNode* lhs_value, class ExceptionSink *xsink);
      DLLLOCAL virtual bool isDefault() const
      {
	 return !val;
      }
      DLLLOCAL bool isCaseNode() const;
      DLLLOCAL virtual ~CaseNode();
};

class SwitchStatement : public AbstractStatement
{
   private:
      class CaseNode *head, *tail;
      class AbstractQoreNode *sexp;
      class CaseNode *deflt;

      DLLLOCAL virtual int parseInitImpl(LocalVar *oflag, int pflag = 0);
      DLLLOCAL virtual int execImpl(class AbstractQoreNode **return_value, class ExceptionSink *xsink);

   public:
      class LVList *lvars;

      // start and end line are set later
      DLLLOCAL SwitchStatement(class CaseNode *f);
      DLLLOCAL virtual ~SwitchStatement();
      DLLLOCAL void setSwitch(class AbstractQoreNode *s);
      DLLLOCAL void addCase(class CaseNode *c);
};

#endif
