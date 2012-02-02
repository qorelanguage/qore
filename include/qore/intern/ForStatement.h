/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
 ForStatement.h
 
 Qore Programming Language
 
 Copyright 2003 - 2012 David Nichols
 
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

#ifndef _QORE_FORSTATEMENT_H

#define _QORE_FORSTATEMENT_H

#include "intern/AbstractStatement.h"

class StatementBlock;
class LVList;

class ForStatement : public AbstractStatement {
   AbstractQoreNode *assignment;
   AbstractQoreNode *cond;
   AbstractQoreNode *iterator;
   StatementBlock *code;
   LVList *lvars;

   DLLLOCAL virtual int execImpl(AbstractQoreNode **return_value, ExceptionSink *xsink);
   DLLLOCAL virtual int parseInitImpl(LocalVar *oflag, int pflag = 0);
   
public:
   DLLLOCAL ForStatement(int start_line, int end_line, AbstractQoreNode *a, AbstractQoreNode *c, AbstractQoreNode *i, StatementBlock *cd);
   DLLLOCAL virtual ~ForStatement();
   // faked here; checked at runtime
   DLLLOCAL virtual bool hasFinalReturn() const {
      return true;
   }
};

#endif
