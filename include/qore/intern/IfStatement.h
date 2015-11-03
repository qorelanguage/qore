/*
 IfStatement.h
 
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

#ifndef _QORE_IFSTATEMENT_H

#define _QORE_IFSTATEMENT_H

#include "intern/AbstractStatement.h"

class IfStatement : public AbstractStatement
{
   private:
      class AbstractQoreNode *cond;
      class StatementBlock *if_code;
      class StatementBlock *else_code;
      class LVList *lvars;

      DLLLOCAL virtual int execImpl(class AbstractQoreNode **return_value, class ExceptionSink *xsink);
      DLLLOCAL virtual int parseInitImpl(LocalVar *oflag, int pflag = 0);
   
   public:
      DLLLOCAL IfStatement(int start_line, int end_line, class AbstractQoreNode *c, class StatementBlock *i, class StatementBlock *e = NULL);
      DLLLOCAL virtual ~IfStatement();
};

#endif
