/*
 DeleteStatement.cc
 
 Qore Programming Language
 
 Copyright (C) 2003, 2004, 2005, 2006 David Nichols
 
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

#include <qore/config.h>
#include <qore/common.h>
#include <qore/DeleteStatement.h>
#include <qore/Statement.h>
#include <qore/Variable.h>
#include <qore/QoreNode.h>

DeleteStatement::DeleteStatement(class QoreNode *v)
{
   var = v;
}

DeleteStatement::~DeleteStatement()
{
   var->deref(NULL);
}

// only executed by Statement::exec()
void DeleteStatement::exec(ExceptionSink *xsink)
{
   delete_var_node(var, xsink);
}

int DeleteStatement::parseInit(lvh_t oflag, int pflag)
{
   return process_node(&var, oflag, pflag);
}

