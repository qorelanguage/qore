/*
  QoreType.cpp

  extensible and type system for the Qore programming language

  Copyright (C) 2003 - 2016 Qore Technologies, s.r.o.

  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.

  Note that the Qore library is released under a choice of three open-source
  licenses: MIT (as above), LGPL 2+, or GPL 2+; see README-LICENSE for more
  information.
*/

#include <qore/Qore.h>

#include <qore/intern/QoreClassIntern.h>

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

qore_type_t get_next_type_id() {
   return lastid++;
}

// 0 = equal, 1 = not equal
bool compareHard(const AbstractQoreNode* l, const AbstractQoreNode* r, ExceptionSink *xsink) {
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
// false = equal, true = not equal
bool compareSoft(const AbstractQoreNode* l, const AbstractQoreNode* r, ExceptionSink *xsink) {
   return !QoreLogicalEqualsOperatorNode::softEqual(l, r, xsink);
}

bool q_compare_soft(const QoreValue l, const QoreValue r, ExceptionSink *xsink) {
   return !QoreLogicalEqualsOperatorNode::softEqual(l, r, xsink);
}

QoreTypeInfoHelper::QoreTypeInfoHelper(const char* n_tname) : typeInfo(new ExternalTypeInfo(n_tname, *this)) {
}

QoreTypeInfoHelper::QoreTypeInfoHelper(qore_type_t id, const char* n_tname) : typeInfo(new ExternalTypeInfo(id, n_tname, *this)) {
   add_to_type_map(id, typeInfo);
}

QoreTypeInfoHelper::~QoreTypeInfoHelper() {
   delete typeInfo;
}

void QoreTypeInfoHelper::assign(qore_type_t id) {
   typeInfo->assign(id);
   add_to_type_map(id, typeInfo);
}

const QoreTypeInfo* QoreTypeInfoHelper::getTypeInfo() const {
   return typeInfo;
}

void QoreTypeInfoHelper::addAcceptsType(const QoreTypeInfo* n_typeInfo) {
   typeInfo->addAcceptsType(n_typeInfo);
}

void QoreTypeInfoHelper::setInt() {
   typeInfo->setInt();
}

void QoreTypeInfoHelper::setInexactReturn() {
   typeInfo->setInexactReturn();
}

void QoreTypeInfoHelper::setInputFilter() {
   typeInfo->setInputFilter();
}

void QoreTypeInfoHelper::setIntMatch() {
   typeInfo->setIntMatch();
}

bool QoreTypeInfoHelper::acceptInputImpl(QoreValue& n, ExceptionSink *xsink) const {
   assert(false);
   return false;
}

int QoreTypeInfoHelper::doAcceptError(bool priv_error, bool obj, int param_num, const char* param_name, AbstractQoreNode* n, ExceptionSink *xsink) const {
   return QoreTypeInfo::doAcceptError(typeInfo, priv_error, obj, param_num, param_name, n, xsink);
}

AbstractQoreClassTypeInfoHelper::AbstractQoreClassTypeInfoHelper(const char* name, int n_domain) : QoreTypeInfoHelper(new ExternalTypeInfo(*this)), qc(new QoreClass(name, n_domain, typeInfo)) {
   typeInfo->assign(qc);
   //printd(5, "AbstractQoreClassTypeInfoHelper::AbstractQoreClassTypeInfoHelper() this=%p typeInfo=%p\n", this, typeInfo);
}

AbstractQoreClassTypeInfoHelper::~AbstractQoreClassTypeInfoHelper() {
   qore_class_private::get(*qc)->deref();
}

QoreClass* AbstractQoreClassTypeInfoHelper::getClass() {
   QoreClass* rv = qc;
   qc = 0;
   return rv;
}

bool AbstractQoreClassTypeInfoHelper::hasClass() const {
   return qc;
}

int testObjectClassAccess(const QoreObject* obj, const QoreClass* shouldbeclass) {
   return qore_class_private::runtimeCheckCompatibleClass(*shouldbeclass, *(obj->getClass()));
}

const QoreClass* typeInfoGetUniqueReturnClass(const QoreTypeInfo* typeInfo) {
   return QoreTypeInfo::getUniqueReturnClass(typeInfo);
}

qore_type_result_e typeInfoAcceptsType(const QoreTypeInfo* typeInfo, const QoreTypeInfo* otherTypeInfo) {
   return QoreTypeInfo::parseAccepts(typeInfo, otherTypeInfo);
}

qore_type_result_e typeInfoReturnsType(const QoreTypeInfo* typeInfo, const QoreTypeInfo* otherTypeInfo) {
   return QoreTypeInfo::parseAccepts(otherTypeInfo, typeInfo);
}

bool typeInfoHasType(const QoreTypeInfo* typeInfo) {
   return typeInfo->hasType();
}

const char* typeInfoGetName(const QoreTypeInfo* typeInfo) {
   return QoreTypeInfo::getName(typeInfo);
}
