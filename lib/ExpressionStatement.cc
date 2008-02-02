/*
 ExpressionStatement.cc
 
 Qore Programming Language
 
 Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols
 
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
#include <qore/intern/ExpressionStatement.h>

ExpressionStatement::ExpressionStatement(int start_line, int end_line, class AbstractQoreNode *v) : AbstractStatement(start_line, end_line)
{
   exp = v;

   // if it is a global variable declaration, then do not register
   if (exp->type == NT_VARREF && reinterpret_cast<VarRefNode *>(exp)->type == VT_GLOBAL)
      is_declaration = true;
   else {
      QoreListNode *l = dynamic_cast<QoreListNode *>(exp);
      if (l && l->isVariableList() && reinterpret_cast<VarRefNode *>(l->retrieve_entry(0))->type == VT_GLOBAL)
	 is_declaration = true;
      else
	 is_declaration = false;
   }
}

ExpressionStatement::~ExpressionStatement()
{
   // this should never be NULL, but in case the implementation changes...
   if (exp)
      exp->deref(0);
}

int ExpressionStatement::execImpl(class AbstractQoreNode **return_value, ExceptionSink *xsink)
{
   discard(exp->eval(xsink), xsink);
   return 0;
}

int ExpressionStatement::parseInitImpl(lvh_t oflag, int pflag)
{
   //printd(5, "ES::pII() exp=%08p (%s)\n", exp, exp->getTypeName());
   return process_node(&exp, oflag, pflag);
}

