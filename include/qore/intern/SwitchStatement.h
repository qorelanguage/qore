/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  SwitchStatement.h

  Qore Programming Language

  Copyright 2003 - 2011 David Nichols

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
   AbstractQoreNode *val;
   StatementBlock *code;
   CaseNode *next;

   DLLLOCAL CaseNode(AbstractQoreNode *v, StatementBlock *c);
   DLLLOCAL virtual bool matches(AbstractQoreNode* lhs_value, ExceptionSink *xsink);
   DLLLOCAL virtual bool isDefault() const {
      return !val;
   }
   DLLLOCAL bool isCaseNode() const;
   DLLLOCAL virtual ~CaseNode();
};

class SwitchStatement : public AbstractStatement {
private:
   CaseNode *head, *tail;
   AbstractQoreNode *sexp;
   CaseNode *deflt;

   DLLLOCAL virtual int parseInitImpl(LocalVar *oflag, int pflag = 0);
   DLLLOCAL virtual int execImpl(class AbstractQoreNode **return_value, class ExceptionSink *xsink);

public:
    LVList *lvars;

   // start and end line are set later
   DLLLOCAL SwitchStatement(CaseNode *f);
   DLLLOCAL virtual ~SwitchStatement();
   DLLLOCAL void setSwitch(AbstractQoreNode *s);
   DLLLOCAL void addCase(CaseNode *c);

   // fake it here and let it be checked at runtime
   DLLLOCAL virtual bool hasFinalReturn() const {
      return true;
   }
};

#endif
