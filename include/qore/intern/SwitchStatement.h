/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  SwitchStatement.h

  Qore Programming Language

  Copyright (C) 2003 - 2015 David Nichols

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
