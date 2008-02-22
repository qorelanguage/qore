/*
  QoreType.cc

  extensible and type system for the Qore programming language

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

#include <string.h>
#include <assert.h>

#include <typeinfo>

#define QTM_NO_VALUE     false
#define QTM_VALUE        true
#define QTM_NO_CONTAINER false
#define QTM_CONTAINER    true

QoreString NothingTypeString("<NOTHING>");
QoreString NullTypeString("<NULL>");
QoreString TrueString("True");
QoreString FalseString("False");
QoreString EmptyHashString("<EMPTY HASH>");
QoreString EmptyListString("<EMPTY LIST>");

DLLLOCAL int QoreTypeManager::lastid = 0;
DLLLOCAL class QoreType *QoreTypeManager::typelist[NUM_VALUE_TYPES];

DLLEXPORT class QoreTypeManager QTM;

// system types
DLLEXPORT class QoreType *NT_NOTHING, *NT_INT, *NT_FLOAT, *NT_STRING, *NT_DATE,
   *NT_BOOLEAN, *NT_NULL, *NT_BINARY, *NT_LIST, *NT_HASH,
   *NT_OBJECT, *NT_BACKQUOTE, *NT_CONTEXTREF, *NT_COMPLEXCONTEXTREF,
   *NT_VARREF, *NT_TREE, *NT_FIND, *NT_FUNCTION_CALL, *NT_SELF_VARREF,
   *NT_SCOPE_REF, *NT_CONSTANT, *NT_BAREWORD, *NT_REFERENCE, *NT_CONTEXT_ROW,
   *NT_REGEX_SUBST, *NT_REGEX_TRANS, *NT_REGEX, *NT_CLASSREF,
   *NT_OBJMETHREF, *NT_FUNCREF, *NT_FUNCREFCALL;

// default value nodes for builtin types
QoreNothingNode Nothing;
QoreNullNode Null;
QoreBoolNode *True, *False;
QoreListNode *emptyList;
QoreHashNode *emptyHash;
QoreStringNode *NullString;
DateTimeNode *ZeroDate;

QoreType::QoreType()
{
   id = QoreTypeManager::lastid++;
}

void QoreTypeManager::add(class QoreType *t)
{
   if (t->getID() < NUM_VALUE_TYPES)
      typelist[t->getID()] = t;

   insert(std::make_pair(t->getID(), t));
}

int QoreType::getID() const
{ 
   return id; 
}

QoreTypeManager::QoreTypeManager()
{
   tracein("QoreTypeManager::QoreTypeManager()");
   
   // register system data types
   // first, value types for operator matrix optimization
   add(NT_NOTHING = new QoreType);
   add(NT_INT = new QoreType);
   add(NT_FLOAT = new QoreType);
   add(NT_STRING = new QoreType);
   add(NT_DATE = new QoreType);
   add(NT_BOOLEAN = new QoreType);
   add(NT_NULL = new QoreType);
   add(NT_BINARY = new QoreType);
   add(NT_LIST = new QoreType);
   add(NT_HASH = new QoreType);
   add(NT_OBJECT = new QoreType);

   // now parse types
   add(NT_BACKQUOTE = new QoreType);
   add(NT_CONTEXTREF = new QoreType);
   add(NT_COMPLEXCONTEXTREF = new QoreType);
   add(NT_VARREF = new QoreType);
   add(NT_TREE = new QoreType);
   add(NT_FIND = new QoreType);
   add(NT_FUNCTION_CALL = new QoreType);
   add(NT_SELF_VARREF = new QoreType);
   add(NT_SCOPE_REF = new QoreType);
   add(NT_CONSTANT = new QoreType);
   add(NT_BAREWORD = new QoreType);
   add(NT_REFERENCE = new QoreType);
   add(NT_CONTEXT_ROW = new QoreType);
   add(NT_REGEX_SUBST = new QoreType);
   add(NT_REGEX_TRANS = new QoreType);
   add(NT_REGEX = new QoreType);
   add(NT_CLASSREF = new QoreType);
   add(NT_OBJMETHREF = new QoreType);
   add(NT_FUNCREF = new QoreType);
   add(NT_FUNCREFCALL = new QoreType);

   // from now on, assign IDs in the user space 
   lastid = QTM_USER_START;

   traceout("QoreTypeManager::QoreTypeManager()");
}

// at least the NullString must be created after the default character encoding is set
void QoreTypeManager::init()
{
   // initialize global default values
   False         = new QoreBoolNode(false);
   True          = new QoreBoolNode(true);
   NullString    = new QoreStringNode("");
   ZeroDate      = new DateTimeNode((int64)0);
   
   emptyList     = new QoreListNode();
   emptyHash     = new QoreHashNode();
}

void QoreTypeManager::del()
{
   // dereference global default values
   True->deref();
   False->deref();
   NullString->deref();
   ZeroDate->deref();
   emptyList->deref(0);
   emptyHash->deref(0);
}

QoreTypeManager::~QoreTypeManager()
{
   tracein("QoreTypeManager::~QoreTypeManager()");

   // delete all stored types
   qore_type_map_t::iterator i;
   while ((i = begin()) != end())
   {
      delete i->second;
      erase(i);
   }

   traceout("QoreTypeManager::~QoreTypeManager()");
}

class QoreType *QoreTypeManager::find(int id)
{
   if (id < NUM_VALUE_TYPES)
      return typelist[id];

   qore_type_map_t::const_iterator i = qore_type_map_t::find(id);
   if (i == end())
      return NULL;
   return i->second;
}

// 0 = equal, 1 = not equal
bool compareHard(const AbstractQoreNode *l, const AbstractQoreNode *r, ExceptionSink *xsink)
{
   if (is_nothing(l))
      if (is_nothing(r))
         return 0;
      else
         return 1;
   if (is_nothing(r))
      return 1;
   if (l->getType() != r->getType())
      return 1;

   // logical equals always returns an integer result
   // FIXME: fix Operator.h and cc instead of using const_cast<>!
   return !OP_ABSOLUTE_EQ->bool_eval(const_cast<AbstractQoreNode *>(l), const_cast<AbstractQoreNode *>(r), xsink);
}

// this function calls the operator function that will
// convert values to do the conversion
// 0 = equal, 1 = not equal
bool compareSoft(const AbstractQoreNode *l, const AbstractQoreNode *r, ExceptionSink *xsink)
{
   // logical equals always returns an integer result
   // FIXME: fix Operator.h and cc instead of using const_cast<>!
   return !OP_LOG_EQ->bool_eval(const_cast<AbstractQoreNode *>(l), const_cast<AbstractQoreNode *>(r), xsink);
}
