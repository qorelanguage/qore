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

QoreString NothingTypeString("<NOTHING>");
QoreString NullTypeString("<NULL>");
QoreString TrueString("True");
QoreString FalseString("False");
QoreString EmptyHashString("<EMPTY HASH>");
QoreString EmptyListString("<EMPTY LIST>");

static qore_type_t lastid = 0;

class QoreTypeManager
{
   public:
      DLLLOCAL QoreTypeManager();
      DLLLOCAL ~QoreTypeManager();
};

static QoreTypeManager QTM;

// system types
qore_type_t NT_NOTHING, NT_INT, NT_FLOAT, NT_STRING, NT_DATE,
   NT_BOOLEAN, NT_NULL, NT_BINARY, NT_LIST, NT_HASH,
   NT_OBJECT, NT_BACKQUOTE, NT_CONTEXTREF, NT_COMPLEXCONTEXTREF,
   NT_VARREF, NT_TREE, NT_FIND, NT_FUNCTION_CALL, NT_SELF_VARREF,
   NT_SCOPE_REF, NT_CONSTANT, NT_BAREWORD, NT_REFERENCE, NT_CONTEXT_ROW,
   NT_REGEX_SUBST, NT_REGEX_TRANS, NT_REGEX, NT_CLASSREF,
   NT_OBJMETHREF, NT_FUNCREF, NT_FUNCREFCALL;

// default value nodes for builtin types
QoreNothingNode Nothing;
QoreNullNode Null;
QoreBoolTrueNode True;
QoreBoolFalseNode False;

QoreListNode *emptyList;
QoreHashNode *emptyHash;
QoreStringNode *NullString;
DateTimeNode *ZeroDate;

qore_type_t get_next_type_id()
{
   return lastid++;
}

QoreTypeManager::QoreTypeManager()
{
   tracein("QoreTypeManager::QoreTypeManager()");
   
   // register system data types
   // first, value types for operator matrix optimization
   NT_NOTHING = get_next_type_id();
   NT_INT     = get_next_type_id();
   NT_FLOAT   = get_next_type_id();
   NT_STRING  = get_next_type_id();
   NT_DATE    = get_next_type_id();
   NT_BOOLEAN = get_next_type_id();
   NT_NULL    = get_next_type_id();
   NT_BINARY  = get_next_type_id();
   NT_LIST    = get_next_type_id();
   NT_HASH    = get_next_type_id();
   NT_OBJECT  = get_next_type_id();

   // now parse types
   NT_BACKQUOTE         = get_next_type_id();
   NT_CONTEXTREF        = get_next_type_id();
   NT_COMPLEXCONTEXTREF = get_next_type_id();
   NT_VARREF            = get_next_type_id();
   NT_TREE              = get_next_type_id();
   NT_FIND              = get_next_type_id();
   NT_FUNCTION_CALL     = get_next_type_id();
   NT_SELF_VARREF       = get_next_type_id();
   NT_SCOPE_REF         = get_next_type_id();
   NT_CONSTANT          = get_next_type_id();
   NT_BAREWORD          = get_next_type_id();
   NT_REFERENCE         = get_next_type_id();
   NT_CONTEXT_ROW       = get_next_type_id();
   NT_REGEX_SUBST       = get_next_type_id();
   NT_REGEX_TRANS       = get_next_type_id();
   NT_REGEX             = get_next_type_id();
   NT_CLASSREF          = get_next_type_id();
   NT_OBJMETHREF        = get_next_type_id();
   NT_FUNCREF           = get_next_type_id();
   NT_FUNCREFCALL       = get_next_type_id();

   traceout("QoreTypeManager::QoreTypeManager()");
}

QoreTypeManager::~QoreTypeManager()
{
}

// at least the NullString must be created after the default character encoding is set
void init_qore_types()
{
   // initialize global default values
   NullString    = new QoreStringNode("");
   ZeroDate      = new DateTimeNode((int64)0);
   
   emptyList     = new QoreListNode();
   emptyHash     = new QoreHashNode();
}

void delete_qore_types()
{
   // dereference global default values
   NullString->deref();
   ZeroDate->deref();
   emptyList->deref(0);
   emptyHash->deref(0);
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

   return !l->is_equal_hard(r, xsink);
}

// this function calls the operator function that will
// convert values to do the conversion
// 0 = equal, 1 = not equal
bool compareSoft(const AbstractQoreNode *l, const AbstractQoreNode *r, ExceptionSink *xsink)
{
   // logical equals always returns an integer result
   return !OP_LOG_EQ->bool_eval(l, r, xsink);
}
