/*
  TypeConstants.h

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

#ifndef _QORE_TYPECONSTANTS_H

#define _QORE_TYPECONSTANTS_H

static inline class QoreNamespace *get_type_ns()
{
   class QoreNamespace *Type = new QoreNamespace("Type");

   Type->addConstant("Boolean",        new QoreStringNode(NT_BOOLEAN->getName()));
   Type->addConstant("Int",            new QoreStringNode(NT_INT->getName()));
   Type->addConstant("Float",          new QoreStringNode(NT_FLOAT->getName()));
   Type->addConstant("String",         new QoreStringNode(NT_STRING->getName()));
   Type->addConstant("Date",           new QoreStringNode(NT_DATE->getName()));
   Type->addConstant("NothingType",    new QoreStringNode(NT_NOTHING->getName()));
   Type->addConstant("NullType",       new QoreStringNode(NT_NULL->getName()));
   Type->addConstant("Binary",         new QoreStringNode(NT_BINARY->getName()));
   Type->addConstant("List",           new QoreStringNode(NT_LIST->getName()));
   Type->addConstant("Hash",           new QoreStringNode(NT_HASH->getName())); 
   Type->addConstant("Object",         new QoreStringNode(NT_OBJECT->getName())); 
   Type->addConstant("CallReference",  new QoreStringNode(NT_FUNCREF->getName()));
   //Type->addConstant("FList", new QoreStringNode("")); //new QoreStringNode((int64)(int)NT_FLIST));
   //Type->addConstant("BackquoteExpression", new QoreStringNode("")); //new QoreStringNode((int64)(int)NT_BACKQUOTE));
   //Type->addConstant("ContextRef", new QoreStringNode("")); //new QoreStringNode((int64)(int)NT_CONTEXTREF));
   //Type->addConstant("VariableReference", new QoreStringNode("")); //new QoreStringNode((int64)(int)NT_VARREF));
   //Type->addConstant("ExpressionTree", new QoreStringNode("")); //new QoreStringNode((int64)(int)NT_TREE));
   //Type->addConstant("FindExpression", new QoreStringNode("")); //new QoreStringNode((int64)(int)NT_FIND));
   //Type->addConstant("FunctionCall", new QoreStringNode("")); //new QoreStringNode((int64)(int)NT_FUNCTION_CALL));
   //Type->addConstant("ObjectVariableReference", new QoreStringNode("")); //new QoreStringNode((int64)(int)NT_SELF_VARREF));
   //Type->addConstant("ScopedReference", new QoreStringNode("")); //new QoreStringNode((int64)(int)NT_SCOPE_REF));

   return Type;
}

#endif // _QORE_TYPECONSTANTS_H
