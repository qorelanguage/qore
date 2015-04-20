/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  ContextStatement.h
 
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

#ifndef _QORE_CONTEXTSTATEMENT_H

#define _QORE_CONTEXTSTATEMENT_H

#include "intern/AbstractStatement.h"

#include <qore/safe_dslist>

// context mod types defined in Context.h

class ContextMod {
public:
   int type;
   union ContextModUnion {
      class AbstractQoreNode *exp;
   } c;
   
   DLLLOCAL ContextMod(int t, class AbstractQoreNode *e);
   DLLLOCAL ~ContextMod();
};

typedef safe_dslist<ContextMod *> cxtmod_list_t;

class ContextModList : public cxtmod_list_t {
public:
   DLLLOCAL ContextModList(ContextMod *cm);
   DLLLOCAL ~ContextModList();
   DLLLOCAL void addContextMod(ContextMod *cm);
};

class ContextStatement :public AbstractStatement {
protected:
   DLLLOCAL virtual int execImpl(class AbstractQoreNode **return_value, class ExceptionSink *xsink);
   DLLLOCAL virtual int parseInitImpl(LocalVar *oflag, int pflag = 0);

public:
   char *name;
   class AbstractQoreNode *exp, *where_exp, *sort_ascending, *sort_descending;
   class StatementBlock *code;
   class LVList *lvars;
   
   DLLLOCAL ContextStatement(int start_line, int end_line, char *n, class AbstractQoreNode *expr, class ContextModList *cm, class StatementBlock *cd);
   DLLLOCAL virtual ~ContextStatement();

   // faked here; checked at runtime
   DLLLOCAL virtual bool hasFinalReturn() const {
      return false;
   }
};

#endif
