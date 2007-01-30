/*
 ThrowStatement.cc
 
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
#include <qore/ThrowStatement.h>

ThrowStatement::ThrowStatement(class QoreNode *v)
{
   if (!v)
   {
      args = NULL;
      return;
   }
   if (v->type == NT_LIST)
   {
      // take list
      args = v;
      return;
   }
   class List *l = new List(1);
   l->push(v);
   args = new QoreNode(l);
}

ThrowStatement::~ThrowStatement()
{
   if (args)
      args->deref(NULL);
}

// only executed by Statement::exec()
void ThrowStatement::exec(ExceptionSink *xsink)
{
   class QoreNode *a;
   if (args)
      a = args->eval(xsink);
   else
      a = NULL;
   
   xsink->raiseException(a);
   if (a)
      a->deref(NULL);
}

int ThrowStatement::parseInit(lvh_t oflag, int pflag)
{
   if (args)
      return process_node(&args, oflag, pflag);
   return 0;
}

