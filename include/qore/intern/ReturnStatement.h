/*
 ReturnStatement.h
 
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

#ifndef _QORE_RETURNSTATEMENT_H

#define _QORE_RETURNSTATEMENT_H

#include "intern/AbstractStatement.h"

class ReturnStatement : public AbstractStatement
{
   private:
      class AbstractQoreNode *exp;

      DLLLOCAL virtual int execImpl(class AbstractQoreNode **return_value, ExceptionSink *xsink);
      DLLLOCAL virtual int parseInitImpl(LocalVar *oflag, int pflag = 0);
   
   public:
      DLLLOCAL ReturnStatement(int start_line, int end_line, class AbstractQoreNode *v = NULL);
      DLLLOCAL virtual ~ReturnStatement();
      DLLLOCAL virtual bool endsBlock() const
      {
	 return true;
      }
};

#endif
