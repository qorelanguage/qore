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
QoreNothingNode *Nothing;
QoreNullNode *Null;
QoreBoolNode *True, *False;
QoreListNode *emptyList;
QoreHashNode *emptyHash;
QoreStringNode *NullString;
DateTimeNode *ZeroDate;

QoreType::QoreType(const char *p_name)
{
   name                   = p_name;
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

const char *QoreType::getName() const
{
   return name;
}

QoreTypeManager::QoreTypeManager()
{
   tracein("QoreTypeManager::QoreTypeManager()");
   
   // register system data types
   // first, value types for operator matrix optimization
   add(NT_NOTHING = new QoreType("nothing"));
   add(NT_INT = new QoreType("integer"));
   add(NT_FLOAT = new QoreType("float"));
   add(NT_STRING = new QoreType("string"));
   add(NT_DATE = new QoreType("date"));
   add(NT_BOOLEAN = new QoreType("boolean"));
   add(NT_NULL = new QoreType("NULL"));
   add(NT_BINARY = new QoreType("binary"));
   add(NT_LIST = new QoreType("list"));
   add(NT_HASH = new QoreType("hash"));
   add(NT_OBJECT = new QoreType("object"));

   // now parse types
   add(NT_BACKQUOTE = new QoreType("backquote"));
   add(NT_CONTEXTREF = new QoreType("context reference"));
   add(NT_COMPLEXCONTEXTREF = new QoreType("complex context reference"));
   add(NT_VARREF = new QoreType("variable reference"));
   add(NT_TREE = new QoreType("expression tree"));
   add(NT_FIND = new QoreType("find"));
   add(NT_FUNCTION_CALL = new QoreType("function call"));
   add(NT_SELF_VARREF = new QoreType("in-object variable reference"));
   add(NT_SCOPE_REF = new QoreType("scoped object call"));
   add(NT_CONSTANT = new QoreType("constant reference"));
   add(NT_BAREWORD = new QoreType("bareword"));
   add(NT_REFERENCE = new QoreType("reference to lvalue"));
   add(NT_CONTEXT_ROW = new QoreType("get context row"));
   add(NT_REGEX_SUBST = new QoreType("regular expression substitution"));
   add(NT_REGEX_TRANS = new QoreType("transliteration"));
   add(NT_REGEX = new QoreType("regular expression"));
   add(NT_CLASSREF = new QoreType("class reference"));
   add(NT_OBJMETHREF = new QoreType("object method reference"));
   add(NT_FUNCREF = new QoreType("call reference"));
   add(NT_FUNCREFCALL = new QoreType("call reference call"));

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
   Nothing       = new QoreNothingNode();
   Null          = new QoreNullNode();
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
   Nothing->deref();
   Null->deref();
   NullString->deref();
   ZeroDate->deref();
   emptyList->deref(NULL);
   emptyHash->deref(NULL);
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
   if (l->type != r->type)
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
