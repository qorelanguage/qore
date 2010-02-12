/*
  QoreType.cc

  extensible and type system for the Qore programming language

  Copyright 2003 - 2010 David Nichols

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

static qore_type_t lastid = QORE_NUM_TYPES;

class QoreTypeManager {
public:
   DLLLOCAL QoreTypeManager() {}
   DLLLOCAL ~QoreTypeManager() {}
};

static QoreTypeManager QTM;

// default value nodes for builtin types
QoreNothingNode Nothing;
QoreNullNode Null;
QoreBoolTrueNode True;
QoreBoolFalseNode False;

QoreListNode *emptyList;
QoreHashNode *emptyHash;
QoreStringNode *NullString;
DateTimeNode *ZeroDate;

qore_type_t get_next_type_id() {
   return lastid++;
}

// at least the NullString must be created after the default character encoding is set
void init_qore_types() {
   // initialize global default values
   NullString    = new QoreStringNode("");
   ZeroDate      = new DateTimeNode((int64)0);
   
   emptyList     = new QoreListNode();
   emptyHash     = new QoreHashNode();
}

void delete_qore_types() {
   // dereference global default values
   NullString->deref();
   ZeroDate->deref();
   emptyList->deref(0);
   emptyHash->deref(0);
}

// 0 = equal, 1 = not equal
bool compareHard(const AbstractQoreNode *l, const AbstractQoreNode *r, ExceptionSink *xsink) {
   if (is_nothing(l)) {
      if (is_nothing(r))
         return 0;
      else
         return 1;
   }

   if (is_nothing(r))
      return 1;

   return !l->is_equal_hard(r, xsink);
}

// this function calls the operator function that will
// convert values to do the conversion
// 0 = equal, 1 = not equal
bool compareSoft(const AbstractQoreNode *l, const AbstractQoreNode *r, ExceptionSink *xsink) {
   // logical equals always returns an integer result
   return !OP_LOG_EQ->bool_eval(l, r, xsink);
}

QoreTypeInfoHelper::QoreTypeInfoHelper(const char *n_tname) : typeInfo(new ExternalTypeInfo(n_tname, *this)) {
}

QoreTypeInfoHelper::QoreTypeInfoHelper(qore_type_t id, const char *n_tname) : typeInfo(new ExternalTypeInfo(id, n_tname, *this)) {
   add_to_type_map(id, typeInfo);
}

QoreTypeInfoHelper::~QoreTypeInfoHelper() {
   delete typeInfo;
}

void QoreTypeInfoHelper::assign(qore_type_t id) {
   typeInfo->assign(id);
   add_to_type_map(id, typeInfo);
}

const QoreTypeInfo *QoreTypeInfoHelper::getTypeInfo() const {
   return typeInfo;
}

bool QoreTypeInfoHelper::checkTypeInstantiationImpl(AbstractQoreNode *&n, ExceptionSink *xsink) const {
   return false;
}

int QoreTypeInfoHelper::testTypeCompatibilityImpl(const AbstractQoreNode *n) const {
   return QTI_NOT_EQUAL;
}

int QoreTypeInfoHelper::parseEqualImpl(const QoreTypeInfo *otherTypeInfo) const {
   return QTI_NOT_EQUAL;
}

AbstractQoreClassTypeInfoHelper::AbstractQoreClassTypeInfoHelper(const char *name, int n_domain) : QoreTypeInfoHelper(new ExternalTypeInfo(*this)), qc(new QoreClass(name, n_domain, typeInfo)) {
   typeInfo->assign(qc);
   printd(0, "AbstractQoreClassTypeInfoHelper::AbstractQoreClassTypeInfoHelper() this=%p typeInfo=%p\n", this, typeInfo);
}

AbstractQoreClassTypeInfoHelper::~AbstractQoreClassTypeInfoHelper() {
   delete qc;
}

QoreClass *AbstractQoreClassTypeInfoHelper::getClass() {
   QoreClass *rv = qc;
   qc = 0;
   return rv;
}

int testObjectClassAccess(const QoreObject *obj, const QoreClass *shouldbeclass) {
   const QoreClass *objectclass = obj->getClass();
   if (shouldbeclass == objectclass)
      return QTI_IDENT;

   bool priv;
   if (!objectclass->getClass(shouldbeclass->getID(), priv))
      return QTI_NOT_EQUAL;

   if (!priv)
      return QTI_AMBIGUOUS;

   return runtimeCheckPrivateClassAccess(shouldbeclass) ? QTI_NOT_EQUAL : QTI_AMBIGUOUS;
}

const QoreClass *typeInfoGetClass(const QoreTypeInfo *typeInfo) {
   return typeInfo->qc;
}

qore_type_t typeInfoGetType(const QoreTypeInfo *typeInfo) {
   return typeInfo->getType();
}

