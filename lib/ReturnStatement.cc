/*
 ReturnStatement.cc
 
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

#include <qore/Qore.h>
#include <qore/intern/ReturnStatement.h>

ReturnStatement::ReturnStatement(int start_line, int end_line, AbstractQoreNode *v) : AbstractStatement(start_line, end_line)
{
   exp = v;
}

ReturnStatement::~ReturnStatement()
{
   // this should never be 0, but in case the implementation changes...
   if (exp)
      exp->deref(0);
}

int ReturnStatement::execImpl(AbstractQoreNode **return_value, ExceptionSink *xsink)
{
   if (exp)
      (*return_value) = exp->eval(xsink);
   return RC_RETURN;
}

int ReturnStatement::parseInitImpl(LocalVar *oflag, int pflag)
{
   return exp ? process_node(&exp, oflag, pflag) : 0;
}

