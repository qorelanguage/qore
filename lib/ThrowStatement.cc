/*
 ThrowStatement.cc
 
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
#include <qore/intern/ThrowStatement.h>

ThrowStatement::ThrowStatement(int start_line, int end_line, AbstractQoreNode *v) : AbstractStatement(start_line, end_line)
{
   if (!v)
   {
      args = 0;
      return;
   }
   args = dynamic_cast<QoreListNode *>(v);
   if (!args) {
      args = new QoreListNode(v->needs_eval());
      args->push(v);
   }
}

ThrowStatement::~ThrowStatement()
{
   if (args)
      args->deref(0);
}

int ThrowStatement::execImpl(AbstractQoreNode **return_value, ExceptionSink *xsink)
{
   QoreListNodeEvalOptionalRefHolder a(args, xsink);
   if (*xsink)
      return 0;
   
   xsink->raiseException(*a);
   return 0;
}

int ThrowStatement::parseInitImpl(LocalVar *oflag, int pflag)
{
   if (args)
      return process_list_node(&args, oflag, pflag);
   return 0;
}

