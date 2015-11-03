/*
  TypeConstants.h

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

#ifndef _QORE_TYPECONSTANTS_H

#define _QORE_TYPECONSTANTS_H

static inline class QoreNamespace *get_type_ns()
{
   class QoreNamespace *Type = new QoreNamespace("Type");

   Type->addConstant("Boolean",        new QoreStringNode(QoreBoolNode::getStaticTypeName()));
   Type->addConstant("Int",            new QoreStringNode(QoreBigIntNode::getStaticTypeName()));
   Type->addConstant("Float",          new QoreStringNode(QoreFloatNode::getStaticTypeName()));
   Type->addConstant("String",         new QoreStringNode(QoreStringNode::getStaticTypeName()));
   Type->addConstant("Date",           new QoreStringNode(DateTimeNode::getStaticTypeName()));
   Type->addConstant("NothingType",    new QoreStringNode(QoreNothingNode::getStaticTypeName()));
   Type->addConstant("NullType",       new QoreStringNode(QoreNullNode::getStaticTypeName()));
   Type->addConstant("Binary",         new QoreStringNode(BinaryNode::getStaticTypeName()));
   Type->addConstant("List",           new QoreStringNode(QoreListNode::getStaticTypeName()));
   Type->addConstant("Hash",           new QoreStringNode(QoreHashNode::getStaticTypeName())); 
   Type->addConstant("Object",         new QoreStringNode(QoreObject::getStaticTypeName())); 
   Type->addConstant("CallReference",  new QoreStringNode(AbstractCallReferenceNode::getStaticTypeName()));
   Type->addConstant("Closure",        new QoreStringNode(QoreClosureBase::getStaticTypeName()));

   return Type;
}

#endif // _QORE_TYPECONSTANTS_H
