/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreTypeInfo.h

  Qore Programming Language

  Copyright (C) 2003 - 2017 Qore Technologies, s.r.o.

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

#ifndef _QORE_QORETYPEINFO_H

#define _QORE_QORETYPEINFO_H

#include <map>
#include <vector>

#define NO_TYPE_INFO "any"

// adds external types to global type map
DLLLOCAL void add_to_type_map(qore_type_t t, const QoreTypeInfo* typeInfo);
DLLLOCAL bool builtinTypeHasDefaultValue(qore_type_t t);
// returns the default value for any type >= 0 and < NT_OBJECT
DLLLOCAL AbstractQoreNode* getDefaultValueForBuiltinValueType(qore_type_t t);

DLLLOCAL void concatClass(std::string& str, const char* cn);

class AbstractQoreTypeInfo {
friend class QoreClassTypeInfo;
public:
   DLLLOCAL AbstractQoreTypeInfo() {
   }

   DLLLOCAL virtual ~AbstractQoreTypeInfo() = default;

   DLLLOCAL virtual const type_vec_t& getReturnTypeList() const {
      assert(!returnsSingleImpl());
      return getReturnTypeListImpl();
   }

   DLLLOCAL virtual const type_vec_t& getAcceptTypeList() const {
      assert(!acceptsSingleImpl());
      return getAcceptTypeListImpl();
   }

   // static version of method, checking for null pointer
   DLLLOCAL static qore_type_t getSingleType(const AbstractQoreTypeInfo* ti) {
      return ti ? ti->getSingleTypeImpl() : NT_ALL;
   }

   // static version of method, checking for null pointer
   DLLLOCAL static bool parseAcceptsReturns(const AbstractQoreTypeInfo* ti, qore_type_t t) {
      return ti ? ti->parseAcceptsReturnsImpl(t) : true;
   }

   // static version of method, checking for null pointer
   DLLLOCAL static qore_type_result_e parseReturnsType(const AbstractQoreTypeInfo* ti, qore_type_t t) {
      return ti ? ti->parseReturnsTypeImpl(t) : QTI_AMBIGUOUS;
   }

   // static version of method, checking for null pointer
   DLLLOCAL static bool isType(const AbstractQoreTypeInfo* ti, qore_type_t t) {
      return ti ? ti->isTypeImpl(t) : false;
   }

   // static version of method, checking for null pointer
   DLLLOCAL static bool isClass(const AbstractQoreTypeInfo* ti, const QoreClass* qc) {
      return ti ? ti->isClassImpl(qc) : false;
   }

   // static version of method, checking for null pointer
   DLLLOCAL static qore_type_result_e runtimeAcceptsValue(const AbstractQoreTypeInfo* ti, const QoreValue n) {
      return ti ? ti->runtimeAcceptsValueImpl(n) : QTI_AMBIGUOUS;
   }

   // static version of method, checking for null pointer
   DLLLOCAL static qore_type_result_e parseAccepts(const AbstractQoreTypeInfo* first, const AbstractQoreTypeInfo* second) {
      bool may_not_match = true;
      return parseAccepts(first, second, may_not_match);
   }

   // static version of method, checking for null pointer
   DLLLOCAL static qore_type_result_e parseAccepts(const AbstractQoreTypeInfo* first, const AbstractQoreTypeInfo* second, bool& may_not_match) {
      if (!first || !second || !first->hasTypeImpl() || !second->hasTypeImpl())
         return QTI_AMBIGUOUS;
      return first->parseAcceptsImpl(second, may_not_match);
   }

   // static version of method, checking for null pointer
   DLLLOCAL static bool returnsSingle(const AbstractQoreTypeInfo* ti) {
      return ti ? ti->returnsSingleImpl() : false;
   }

   // static version of method, checking for null pointer
   DLLLOCAL static bool acceptsSingle(const AbstractQoreTypeInfo* ti) {
      return ti ? ti->acceptsSingleImpl() : false;
   }

   // static version of method, checking for null pointer
   DLLLOCAL static qore_type_result_e parseReturnsClass(const AbstractQoreTypeInfo* ti, const QoreClass* qc) {
      return ti ? ti->parseReturnsClassImpl(qc) : QTI_AMBIGUOUS;
   }

   // static version of method, checking for null pointer
   DLLLOCAL static const QoreClass* getUniqueReturnClass(const AbstractQoreTypeInfo* ti) {
      return ti ? ti->getUniqueReturnClassImpl() : nullptr;
   }

   // static version of method, checking for null pointer
   DLLLOCAL static bool hasType(const AbstractQoreTypeInfo* ti) {
      return ti ? ti->hasTypeImpl() : false;
   }

   // static version of method, checking for null pointer
   DLLLOCAL static bool needsScan(const AbstractQoreTypeInfo* ti) {
      return ti ? ti->needsScanImpl() : true;
   }

   // static version of method, checking for null pointer
   DLLLOCAL static bool hasInputFilter(const AbstractQoreTypeInfo* ti) {
      return ti ? ti->hasInputFilterImpl() : false;
   }

   // static version of method, checking for null pointer
   DLLLOCAL static const char* getName(const AbstractQoreTypeInfo* ti) {
      return ti ? ti->getNameImpl() : NO_TYPE_INFO;
   }

   // static version of method, checking for null pointer
   DLLLOCAL static void getThisType(const AbstractQoreTypeInfo* ti, QoreString& str) {
      if (ti)
         ti->getThisTypeImpl(str);
      else
         str.sprintf("no value");
   }

   // static version of method, checking for null pointer
   DLLLOCAL static void acceptInputParam(const AbstractQoreTypeInfo* ti, int param_num, const char* param_name, QoreValue& n, ExceptionSink* xsink) {
      if (ti)
         ti->acceptInputInternImpl(false, param_num, param_name, n, xsink);
   }

   // static version of method, checking for null pointer
   DLLLOCAL static void acceptInputMember(const AbstractQoreTypeInfo* ti, const char* member_name, QoreValue& n, ExceptionSink* xsink) {
      if (ti)
         ti->acceptInputInternImpl(true, -1, member_name, n, xsink);
   }

   // static version of method, checking for null pointer
   DLLLOCAL static void acceptAssignment(const AbstractQoreTypeInfo* ti, const char* text, QoreValue& n, ExceptionSink* xsink) {
      assert(text && text[0] == '<');
      if (ti)
         ti->acceptInputInternImpl(false, -1, text, n, xsink);
   }

   // static version of method, checking for null pointer
   DLLLOCAL static bool hasDefaultValue(const AbstractQoreTypeInfo* ti) {
      return ti ? ti->hasDefaultValueImpl() : false;
   }

   // static version of method, checking for null pointer
   DLLLOCAL static QoreValue getDefaultQoreValue(const AbstractQoreTypeInfo* ti) {
      return ti ? ti->getDefaultQoreValueImpl() : QoreValue();
   }

   // static version of method, checking for null pointer
   DLLLOCAL static bool mayRequireFilter(const AbstractQoreTypeInfo* ti, const QoreValue& n) {
      return ti ? ti->mayRequireFilterImpl(n) : false;
   }

   // static version of method, checking for null pointer
   DLLLOCAL static bool isInputIdentical(const AbstractQoreTypeInfo* a, const AbstractQoreTypeInfo* b) {
      if (a && b)
         return a->isInputIdenticalImpl(b);
      if (a)
         return !a->hasTypeImpl();
      if (b)
         return !b->hasTypeImpl();
      return true;
   }

   // static version of method, checking for null pointer
   DLLLOCAL static bool isOutputIdentical(const AbstractQoreTypeInfo* a, const AbstractQoreTypeInfo* b) {
      if (a && b)
         return a->isOutputIdenticalImpl(b);
      if (a)
         return !a->hasTypeImpl();
      if (b)
         return !b->hasTypeImpl();
      return true;
   }

   // if second's return type is compatible with first's return type
   // static version of method, checking for null pointer
   DLLLOCAL static bool isOutputCompatible(const AbstractQoreTypeInfo* first, const AbstractQoreTypeInfo* second) {
      if (first && second)
         return first->isOutputCompatibleImpl(second);
      return true;
   }

protected:
   DLLLOCAL int doAcceptError(bool priv_error, bool obj, int param_num, const char* param_name, QoreValue& n, ExceptionSink* xsink) const {
      if (priv_error) {
         if (obj)
            doObjectPrivateClassException(param_name, xsink);
         else
            doPrivateClassException(param_num + 1, param_name, xsink);
      }
      else {
         if (obj)
            doObjectTypeException(param_name, n, xsink);
         else
            doTypeException(param_num + 1, param_name, n, xsink);
      }
      return -1;
   }

   DLLLOCAL int doObjectPrivateClassException(const char* param_name, ExceptionSink* xsink) const {
      assert(xsink);
      QoreStringNode* desc = new QoreStringNode;
      desc->sprintf("member '%s' expects ", param_name);
      getThisTypeImpl(*desc);
      desc->concat(", but got an object where this class is privately inherited instead");
      xsink->raiseException("RUNTIME-TYPE-ERROR", desc);
      return -1;
   }

   DLLLOCAL int doPrivateClassException(int param_num, const char* param_name, ExceptionSink* xsink) const {
      // xsink may be null in case that parse exceptions have been disabled in the QoreProgram object
      // for example if there was a "requires" error
      if (!xsink)
         return -1;

      QoreStringNode* desc = new QoreStringNode;
      AbstractQoreTypeInfo::ptext(*desc, param_num, param_name);
      desc->concat("expects ");
      getThisTypeImpl(*desc);
      desc->concat(", but got an object where this class is privately inherited instead");
      xsink->raiseException("RUNTIME-TYPE-ERROR", desc);
      return -1;
   }

   DLLLOCAL int doObjectTypeException(const char* param_name, const QoreValue& n, ExceptionSink* xsink) const {
      assert(xsink);
      QoreStringNode* desc = new QoreStringNode;
      desc->sprintf("member '%s' expects ", param_name);
      getThisTypeImpl(*desc);
      desc->concat(", but got ");
      AbstractQoreTypeInfo::getNodeType(*desc, n);
      desc->concat(" instead");
      xsink->raiseException("RUNTIME-TYPE-ERROR", desc);
      return -1;
   }

   DLLLOCAL int doTypeException(int param_num, const char* param_name, const QoreValue& n, ExceptionSink* xsink) const {
      // xsink may be null in case parse exceptions have been disabled in the QoreProgram object
      // for example if there was a "requires" error
      if (!xsink)
         return -1;

      QoreStringNode* desc = new QoreStringNode;
      AbstractQoreTypeInfo::ptext(*desc, param_num, param_name);
      desc->concat("expects ");
      getThisTypeImpl(*desc);
      desc->concat(", but got ");
      AbstractQoreTypeInfo::getNodeType(*desc, n);
      desc->concat(" instead");
      xsink->raiseException("RUNTIME-TYPE-ERROR", desc);
      return -1;
   }

   DLLLOCAL virtual qore_type_t getSingleTypeImpl() const = 0;

   DLLLOCAL virtual bool parseAcceptsReturnsImpl(qore_type_t t) const = 0;

   DLLLOCAL virtual qore_type_result_e parseReturnsTypeImpl(qore_type_t t) const = 0;

   // returns true if this type only returns the value given
   DLLLOCAL virtual bool isTypeImpl(qore_type_t t) const = 0;

   DLLLOCAL virtual bool isClassImpl(const QoreClass* qc) const {
      return false;
   }

   DLLLOCAL virtual qore_type_result_e runtimeAcceptsValueImpl(const QoreValue n) const = 0;

   DLLLOCAL virtual qore_type_result_e parseAcceptsImpl(const AbstractQoreTypeInfo* typeInfo, bool& may_not_match) const = 0;

   // returns true if the type returns a single type
   DLLLOCAL virtual bool returnsSingleImpl() const = 0;

   // returns true if the type accepts a single type
   DLLLOCAL virtual bool acceptsSingleImpl() const = 0;

   DLLLOCAL virtual qore_type_result_e parseReturnsClassImpl(const QoreClass* n_qc) const = 0;

   DLLLOCAL virtual const QoreClass* getUniqueReturnClassImpl() const {
      return nullptr;
   }

   DLLLOCAL virtual bool hasTypeImpl() const {
      return true;
   }

   // returns true if this type could contain an object or a closure
   DLLLOCAL virtual bool needsScanImpl() const {
      return false;
   }

   DLLLOCAL virtual bool hasInputFilterImpl() const {
      return false;
   }

   DLLLOCAL virtual bool mayRequireFilterImpl(const QoreValue& n) const {
      return false;
   }

   DLLLOCAL virtual const char* getNameImpl() const = 0;

   DLLLOCAL virtual void getThisTypeImpl(QoreString& str) const = 0;

   DLLLOCAL virtual const type_vec_t& getReturnTypeListImpl() const {
      assert(false);
      throw;
   }

   DLLLOCAL virtual const type_vec_t& getAcceptTypeListImpl() const {
      assert(false);
      throw;
   }

   DLLLOCAL virtual void acceptInputInternImpl(bool obj, int param_num, const char* param_name, QoreValue& n, ExceptionSink* xsink) const = 0;

   DLLLOCAL virtual bool hasDefaultValueImpl() const = 0;

   DLLLOCAL virtual QoreValue getDefaultQoreValueImpl() const = 0;

   // used when parsing user code to find duplicate signatures after types are resolved
   DLLLOCAL virtual bool isInputIdenticalImpl(const AbstractQoreTypeInfo* typeInfo) const = 0;

   DLLLOCAL virtual bool isOutputIdenticalImpl(const AbstractQoreTypeInfo* typeInfo) const = 0;

   // if the argument's return type is compatible with "this"'s return type
   DLLLOCAL virtual bool isOutputCompatibleImpl(const AbstractQoreTypeInfo* typeInfo) const = 0;

   DLLLOCAL static void getNodeType(QoreString& str, const QoreValue& n) {
      qore_type_t nt = n.getType();
      if (nt == NT_NOTHING) {
         str.concat("no value");
         return;
      }
      if (nt != NT_OBJECT) {
         str.sprintf("type '%s'", n.getTypeName());
         return;
      }
      str.sprintf("an object of class '%s'", n.get<const QoreObject>()->getClassName());
   }

   DLLLOCAL static void ptext(QoreString& str, int param_num, const char* param_name) {
      if (!param_num && param_name && param_name[0] == '<') {
         str.concat(param_name);
         str.concat(' ');
         return;
      }
      if (param_name && param_name[0] == '<') {
         str.concat(param_name);
         str.concat(' ');
      }
      str.concat("parameter ");
      if (param_num) {
         str.sprintf("%d ", param_num);
         if (param_name && param_name[0] != '<')
            str.sprintf("('%s') ", param_name);
      }
      else
         str.sprintf("'%s' ", param_name);
   }
};

class QoreClassTypeInfo : public AbstractQoreTypeInfo {
public:
   DLLLOCAL QoreClassTypeInfo(const QoreClass* qc) {
   }

protected:
   QoreClass* qc;

   DLLLOCAL virtual qore_type_result_e runtimeAcceptsClassMult(const QoreClass* n_qc) const;

   DLLLOCAL virtual bool parseAcceptsReturnsImpl(qore_type_t t) const {
      return t == NT_OBJECT ? QTI_IDENT : QTI_NOT_EQUAL;
   }

   DLLLOCAL virtual qore_type_t getSingleTypeImpl() const {
      return NT_OBJECT;
   }

   DLLLOCAL virtual qore_type_result_e parseReturnsTypeImpl(qore_type_t t) const {
      return t == NT_OBJECT ? QTI_IDENT : QTI_NOT_EQUAL;
   }

   DLLLOCAL virtual bool isTypeImpl(qore_type_t t) const {
      return t == NT_OBJECT;
   }

   DLLLOCAL virtual bool isClassImpl(const QoreClass* qc) const;

   DLLLOCAL virtual qore_type_result_e runtimeAcceptsValueImpl(const QoreValue n) const;

   DLLLOCAL virtual qore_type_result_e parseAcceptsImpl(const AbstractQoreTypeInfo* typeInfo, bool& may_not_match) const {
      if (!typeInfo->returnsSingleImpl())
         may_not_match = true;
      return typeInfo->parseReturnsClassImpl(qc);
   }

   // returns true if the type returns a single type
   DLLLOCAL virtual bool returnsSingleImpl() const {
      return true;
   }

   // returns true if the type accepts a single type
   DLLLOCAL virtual bool acceptsSingleImpl() const {
      return true;
   }

   DLLLOCAL virtual qore_type_result_e parseReturnsClassImpl(const QoreClass* qc) const;

   DLLLOCAL virtual const QoreClass* getUniqueReturnClassImpl() const {
      return qc;
   }

   // returns true if this type could contain an object or a closure
   DLLLOCAL virtual bool needsScanImpl() const {
      return true;
   }

   DLLLOCAL virtual const char* getNameImpl() const {
      return qc->getName();
   }

   DLLLOCAL virtual void getThisType(QoreString& str) const {
      str.sprintf("an object of class '%s'", qc->getName());
   }

   DLLLOCAL virtual void acceptInputInternImpl(bool obj, int param_num, const char* param_name, QoreValue& n, ExceptionSink* xsink) const;

   DLLLOCAL virtual bool hasDefaultValueImpl() const {
      return false;
   }

   DLLLOCAL virtual QoreValue getDefaultQoreValueImpl() const {
      return QoreValue();
   }

   // used when parsing user code to find duplicate signatures after types are resolved
   DLLLOCAL virtual bool isInputIdenticalImpl(const AbstractQoreTypeInfo* typeInfo) const;

   DLLLOCAL virtual bool isOutputIdenticalImpl(const AbstractQoreTypeInfo* typeInfo) const;

   // if the argument's return type is compatible with "this"'s return type
   DLLLOCAL virtual bool isOutputCompatibleImpl(const AbstractQoreTypeInfo* typeInfo) const {
      return typeInfo->parseReturnsClassImpl(qc) == QTI_NOT_EQUAL ? false : true;
   }
};

/*
 * if input_filter is true, then
   + returns_mult must be false xxx <- REMOVE THIS RESTRICTION
   + accepts_mult must be true
 * if accepts_mult is false, then qc and qt apply to the type accepted
 * if returns_mult is false, then qc and qt apply to the type returned
 * if both accepts_mult and returns_mult are true, then qc and qt have no relevance to the type
 * in a type list:
   + no entry may be NULL or have qt = NT_ALL
   + all entries must be different types
 * if exact_return is true then returns_mult must be false
 */

class QoreTypeInfo {
   friend class OrNothingTypeInfo;

protected:
   // class pointer
   const QoreClass* qc;
   // basic type
#ifdef __SUNPRO_CC
   // seems to be converted to unsigned with oracle studio compilers
   qore_type_t qt;
#else
   qore_type_t qt : 11;
#endif
   // true if type indicates more than one return type can be returned
   bool returns_mult : 1;
   // true if type accepts multiple types
   bool accepts_mult : 1;
   // true if multiple types accepted on input that produce an output type
   bool input_filter : 1;
   // true if type has a subtype
   bool has_subtype : 1;
   // true if has a custom name
   bool has_name : 1;
   // true if the type has a default value implementation function
   bool has_defval : 1;
   // true if the type is an implementation of QoreBigIntNode (for ints and enums)
   bool is_int : 1;
   // true if the single return type makes an exact match or ambiguous on input
   bool exact_return : 1;
   // if true then any type with is_int sets matches NT_INT ambiguously
   bool ambiguous_int_match : 1;
   // if true then this type accepts all types
   bool accepts_all : 1;

   DLLLOCAL qore_type_result_e parseReturnsType(qore_type_t t, bool n_is_int) const {
      if (!hasType())
         return QTI_AMBIGUOUS;

      if (returns_mult)
         return parseReturnsTypeMult(t, n_is_int);

      return matchTypeIntern(t, n_is_int);
   }

   DLLLOCAL qore_type_result_e parseAcceptsType(qore_type_t t, bool n_is_int) const {
      // set to true because value is ignored and can short-circuit logic in parseAcceptsMult() if called
      bool may_not_match = true;
      return parseAcceptsType(t, n_is_int, may_not_match);
   }

   DLLLOCAL qore_type_result_e parseAcceptsType(qore_type_t t, bool n_is_int, bool& may_not_match) const {
      //printd(5, "QoreTypeInfo::parseAcceptsType() this: %p %s t: %d accepts_mult: %d\n", this, getName(), t, accepts_mult);

      if (!hasType() || accepts_all)
         return QTI_AMBIGUOUS;

      if (accepts_mult)
         return parseAcceptsTypeMult(t, n_is_int, may_not_match);

      qore_type_result_e rc = matchTypeIntern(t, n_is_int);
      if (rc == QTI_IDENT && qc) {
         rc = QTI_AMBIGUOUS;
         if (!may_not_match)
            may_not_match = true;
      }
      //printd(5, "QoreTypeInfo::parseAcceptsType() this: %p %s t: %d rc: %d may_not_match: %d\n", this, getName(), t, rc, may_not_match);
      return rc;
   }

   DLLLOCAL qore_type_result_e runtimeAcceptsClass(const QoreClass* n_qc) const {
      assert(n_qc);

      if (!hasType() || accepts_all)
         return QTI_AMBIGUOUS;

      if (accepts_mult)
         return runtimeAcceptsClassMult(n_qc);

      return runtimeMatchClassIntern(n_qc);
   }

   DLLLOCAL qore_type_result_e parseAcceptsClass(const QoreClass* n_qc) const {
      if (!hasType() || accepts_all)
         return QTI_AMBIGUOUS;

      if (accepts_mult)
         return parseAcceptsClassMult(n_qc);

      return matchClassIntern(n_qc);
   }

   DLLLOCAL qore_type_result_e parseReturnsTypeMult(qore_type_t t, bool n_is_int) const {
      const type_vec_t &rt = getReturnTypeList();

      for (type_vec_t::const_iterator i = rt.begin(), e = rt.end(); i != e; ++i) {
         if ((*i)->parseReturnsType(t, n_is_int))
            return QTI_AMBIGUOUS;
      }

      // now check fundamental type
      return matchTypeIntern(t, n_is_int);
   }

   DLLLOCAL qore_type_result_e parseReturnsClassMult(const QoreClass* n_qc) const {
      const type_vec_t &rt = getReturnTypeList();

      for (type_vec_t::const_iterator i = rt.begin(), e = rt.end(); i != e; ++i) {
         if ((*i)->parseReturnsClass(n_qc))
            return QTI_AMBIGUOUS;
      }

      // now check fundamental type
      return matchClassIntern(n_qc);
   }

   DLLLOCAL qore_type_result_e parseAcceptsTypeMult(qore_type_t t, bool n_is_int, bool& may_not_match) const {
      if (!returns_mult) {
         qore_type_result_e rc = matchTypeIntern(t, n_is_int);
         if (rc) {
            if (rc == QTI_IDENT && qc) {
               rc = QTI_AMBIGUOUS;
               if (!may_not_match)
                  may_not_match = true;
            }
            return rc;
         }
      }

      const type_vec_t &at = getAcceptTypeList();

      for (type_vec_t::const_iterator i = at.begin(), e = at.end(); i != e; ++i) {
         if ((*i)->parseAcceptsType(t, n_is_int, may_not_match))
            return QTI_AMBIGUOUS;
      }

      // now check fundamental type
      qore_type_result_e rc = matchTypeIntern(t, n_is_int);
      if (rc == QTI_IDENT) {
         rc = QTI_AMBIGUOUS;
         if (qc && !may_not_match)
            may_not_match = true;
      }
      return rc;
   }

   DLLLOCAL qore_type_result_e parseAcceptsClassMult(const QoreClass* n_qc) const {
      if (!returns_mult && qc && qc->getID() == n_qc->getID())
         return exact_return ? QTI_IDENT : QTI_AMBIGUOUS;

      const type_vec_t &at = getAcceptTypeList();

      for (type_vec_t::const_iterator i = at.begin(), e = at.end(); i != e; ++i) {
         if ((*i)->parseAcceptsClass(n_qc))
            return QTI_AMBIGUOUS;
      }

      // now check fundamental type
      return matchClassIntern(n_qc);
   }

   DLLLOCAL qore_type_result_e runtimeAcceptsClassMult(const QoreClass* n_qc) const {
      if (!returns_mult && qc && qc->getID() == n_qc->getID())
         return exact_return ? QTI_IDENT : QTI_AMBIGUOUS;

      const type_vec_t &at = getAcceptTypeList();

      for (type_vec_t::const_iterator i = at.begin(), e = at.end(); i != e; ++i) {
         if ((*i)->runtimeAcceptsClass(n_qc))
            return QTI_AMBIGUOUS;
      }

      // now check fundamental type
      return runtimeMatchClassIntern(n_qc);
   }

   DLLLOCAL qore_type_result_e parseAcceptsBasic(const QoreTypeInfo* typeInfo, bool& may_not_match) const {
      return typeInfo->qc ? parseAcceptsClass(typeInfo->qc) : parseAcceptsType(typeInfo->qt, typeInfo->is_int, may_not_match);
   }

   DLLLOCAL static bool parseAcceptsMultHelper(bool val, qore_type_result_e& rc, bool& may_not_match) {
      if (val) {
         rc = QTI_AMBIGUOUS;
         return may_not_match ? true : false;
      }

      may_not_match = true;
      return rc == QTI_AMBIGUOUS ? true : false;
   }

   // see if any of of the types we accept match any of the types that can be returned by typeInfo
   DLLLOCAL qore_type_result_e parseAcceptsMult(const QoreTypeInfo* typeInfo, bool& may_not_match) const {
      assert(accepts_mult);
      assert(typeInfo->returns_mult);

      const type_vec_t& at = getAcceptTypeList();
      const type_vec_t& rt = typeInfo->getReturnTypeList();

      qore_type_result_e rc = QTI_NOT_EQUAL;
      for (type_vec_t::const_iterator i = at.begin(), e = at.end(); i != e; ++i) {
         for (type_vec_t::const_iterator j = rt.begin(), je = rt.end(); j != je; ++j) {
            //printd(5, "QoreTypeInfo::parseAcceptsMult() this=%p (%s) accepts %p (%s) testing if %p (%s) may_not_match=%d rc=%d accepts %p (%s) = %d\n", this, getName(), typeInfo, typeInfo->getName(), *i, (*i)->getName(), may_not_match, rc, *j, (*j)->getName(), (*i)->parseAccepts(*j));

            if (parseAcceptsMultHelper((*i)->parseAccepts(*j), rc, may_not_match))
               return rc;
         }
         // now check basic return type
         if (parseAcceptsMultHelper((*i)->parseAcceptsBasic(typeInfo, may_not_match), rc, may_not_match))
            return rc;
      }

      // now check basic accept type against all return types
      for (type_vec_t::const_iterator j = rt.begin(), je = rt.end(); j != je; ++j) {
         if (parseAcceptsMultHelper(parseAcceptsBasic(*j, may_not_match), rc, may_not_match))
            return rc;

         //printd(5, "QoreTypeInfo::parseAcceptsMult() this=%p (%s) accepts %p (%s) testing may_not_match=%d rc=%d accepts %p (%s) = %d\n", this, getName(), typeInfo, typeInfo->getName(), may_not_match, rc, *j, (*j)->getName(), parseAcceptsBasic(*j));
      }

      // now check basic accept type against basic return types
      parseAcceptsMultHelper(parseAcceptsBasic(typeInfo, may_not_match), rc, may_not_match);
      return rc;
   }

   DLLLOCAL qore_type_result_e matchTypeIntern(qore_type_t t, bool n_is_int) const {
      if (qt == NT_ALL || t == NT_ALL)
         return QTI_AMBIGUOUS;

      if (qt == t)
         return exact_return ? QTI_IDENT : QTI_AMBIGUOUS;

      // if the type to compare is equivalent to int
      if (n_is_int) {
         if (is_int)
            return QTI_AMBIGUOUS;

         if (qt == NT_INT)
            return ambiguous_int_match ? QTI_AMBIGUOUS : QTI_NOT_EQUAL;
      }

      return QTI_NOT_EQUAL;
   }

   DLLLOCAL qore_type_result_e matchClassIntern(const QoreClass* n_qc) const;
   DLLLOCAL qore_type_result_e runtimeMatchClassIntern(const QoreClass* n_qc) const;

   DLLLOCAL int doPrivateClassException(int param_num, const char* param_name, const AbstractQoreNode* n, ExceptionSink* xsink) const {
      return QoreTypeInfo::doPrivateClassException(this, param_num, param_name, n, xsink);
   }

   // static version of method, checking for null pointer
   DLLLOCAL static int doPrivateClassException(const QoreTypeInfo* ti, int param_num, const char* param_name, const AbstractQoreNode* n, ExceptionSink* xsink) {
      // xsink may be null in case that parse exceptions have been disabled in the QoreProgram object
      // for example if there was a "requires" error
      if (!xsink)
         return -1;

      QoreStringNode* desc = new QoreStringNode;
      QoreTypeInfo::ptext(*desc, param_num, param_name);
      desc->concat("expects ");
      QoreTypeInfo::getThisType(ti, *desc);
      desc->concat(", but got an object where this class is privately inherited instead");
      xsink->raiseException("RUNTIME-TYPE-ERROR", desc);
      return -1;
   }

   DLLLOCAL int doObjectTypeException(const char* param_name, const QoreValue& n, ExceptionSink* xsink) const {
      return QoreTypeInfo::doObjectTypeException(this, param_name, n, xsink);
   }

   // static version of method, checking for null pointer
   DLLLOCAL static int doObjectTypeException(const QoreTypeInfo* ti, const char* param_name, const QoreValue& n, ExceptionSink* xsink) {
      assert(xsink);
      QoreStringNode* desc = new QoreStringNode;
      desc->sprintf("member '%s' expects ", param_name);
      QoreTypeInfo::getThisType(ti, *desc);
      desc->sprintf(", but got %s instead", n.getTypeName());
      xsink->raiseException("RUNTIME-TYPE-ERROR", desc);
      return -1;
   }

   DLLLOCAL int doObjectTypeException(const char* param_name, const AbstractQoreNode* n, ExceptionSink* xsink) const {
      return QoreTypeInfo::doObjectTypeException(this, param_name, n, xsink);
   }

   // static version of method, checking for null pointer
   DLLLOCAL static int doObjectTypeException(const QoreTypeInfo* ti, const char* param_name, const AbstractQoreNode* n, ExceptionSink* xsink) {
      assert(xsink);
      QoreStringNode* desc = new QoreStringNode;
      desc->sprintf("member '%s' expects ", param_name);
      QoreTypeInfo::getThisType(ti, *desc);
      desc->concat(", but got ");
      getNodeType(*desc, n);
      desc->concat(" instead");
      xsink->raiseException("RUNTIME-TYPE-ERROR", desc);
      return -1;
   }

   DLLLOCAL int doObjectPrivateClassException(const char* param_name, const AbstractQoreNode* n, ExceptionSink* xsink) const {
      return QoreTypeInfo::doObjectPrivateClassException(this, param_name, n, xsink);
   }

   // static version of method, checking for null pointer
   DLLLOCAL static int doObjectPrivateClassException(const QoreTypeInfo* ti, const char* param_name, const AbstractQoreNode* n, ExceptionSink* xsink) {
      assert(xsink);
      QoreStringNode* desc = new QoreStringNode;
      desc->sprintf("member '%s' expects ", param_name);
      QoreTypeInfo::getThisType(ti, *desc);
      desc->concat(", but got an object where this class is privately inherited instead");
      xsink->raiseException("RUNTIME-TYPE-ERROR", desc);
      return -1;
   }

   // returns -1 for error encountered, 0 for OK
   // can only be called with accepts_mult is false
   DLLLOCAL int runtimeAcceptInputIntern(bool& priv_error, QoreValue& n) const;

   // returns -1 for error encountered, 0 for OK
   DLLLOCAL int acceptInputDefault(bool& priv_error, QoreValue& n) const;

   DLLLOCAL void acceptInputIntern(bool obj, int param_num, const char* param_name, QoreValue& n, ExceptionSink* xsink) const {
      if (!input_filter) {
         bool priv_error = false;
         if (acceptInputDefault(priv_error, n))
            doAcceptError(priv_error, obj, param_num, param_name, n, xsink);
         return;
      }

      // first check if input matches default type
      bool priv_error = false;
      if (!runtimeAcceptInputIntern(priv_error, n))
         return;

      if (!acceptInputImpl(n, xsink) && !*xsink)
         doAcceptError(false, obj, param_num, param_name, n, xsink);
   }

   DLLLOCAL bool isTypeIdenticalIntern(const QoreTypeInfo* typeInfo) const {
      if (qt != typeInfo->qt)
         return false;

      // both types are identical
      if (qt != NT_OBJECT)
         return true;

      if (qc) {
         if (!typeInfo->qc)
            return false;
         return qc->getID() == typeInfo->qc->getID();
      }
      return !typeInfo->qc;
   }

   // must be reimplemented in subclasses if input_filter is true
   DLLLOCAL virtual bool acceptInputImpl(QoreValue& n, ExceptionSink* xsink) const {
      assert(false);
      return false;
   }

   // must be reimplemented in subclasses if has_defval is true
   DLLLOCAL virtual AbstractQoreNode* getDefaultValueImpl() const {
      assert(false);
      return nullptr;
   }

   // must be reimplemented in subclasses if has_name is true
   DLLLOCAL virtual const char* getNameImpl() const {
      assert(false);
      return nullptr;
   }

   DLLLOCAL static void getNodeType(QoreString& str, const QoreValue n) {
      qore_type_t nt = n.getType();
      if (nt == NT_NOTHING) {
         str.concat("no value");
         return;
      }
      if (nt != NT_OBJECT) {
         str.sprintf("type '%s'", n.getTypeName());
         return;
      }
      str.sprintf("an object of class '%s'", n.get<const QoreObject>()->getClassName());
   }

   DLLLOCAL static void ptext(QoreString& str, int param_num, const char* param_name) {
      if (!param_num && param_name && param_name[0] == '<') {
         str.concat(param_name);
         str.concat(' ');
         return;
      }
      if (param_name && param_name[0] == '<') {
         str.concat(param_name);
         str.concat(' ');
      }
      str.concat("parameter ");
      if (param_num) {
         str.sprintf("%d ", param_num);
         if (param_name && param_name[0] != '<')
            str.sprintf("('%s') ", param_name);
      }
      else
         str.sprintf("'%s' ", param_name);
   }

   DLLLOCAL bool returnsSingleIntern() const {
      return (!returns_mult && qt != NT_ALL);
   }

   DLLLOCAL QoreTypeInfo* getSubtypeIntern() const {
      assert(has_subtype);
      return getSubtypeImpl();
   }

   DLLLOCAL virtual QoreTypeInfo* getSubtypeImpl() const {
      assert(false);
      return nullptr;
   }

   DLLLOCAL QoreTypeInfo(const QoreClass* n_qc, qore_type_t n_qt, bool n_returns_mult,
                         bool n_accepts_mult, bool n_input_filter, bool n_has_subtype,
                         bool n_has_name, bool n_has_defval,
                         bool n_is_int, bool n_exact_return, bool n_accepts_all) :
      qc(n_qc), qt(n_qt), returns_mult(n_returns_mult), accepts_mult(n_accepts_mult), input_filter(n_input_filter),
      has_subtype(n_has_subtype), has_name(n_has_name), has_defval(n_has_defval),
      is_int(n_is_int), exact_return(n_exact_return), ambiguous_int_match(false), accepts_all(n_accepts_all) {
      assert(!is_int || !qc);
      assert(!(exact_return && returns_mult));
   }

public:
   DLLLOCAL QoreTypeInfo() : qc(0), qt(NT_ALL), returns_mult(false), accepts_mult(false),
                             input_filter(false), has_subtype(false), has_name(false), has_defval(false),
                             is_int(false), exact_return(false),
                             ambiguous_int_match(false), accepts_all(true) {
   }

   DLLLOCAL QoreTypeInfo(qore_type_t n_qt) : qc(0), qt(n_qt), returns_mult(false), accepts_mult(false),
                                             input_filter(false), has_subtype(false), has_name(false), has_defval(false),
                                             is_int(n_qt == NT_INT),
                                             exact_return(true), ambiguous_int_match(false), accepts_all(false) {
   }

   DLLLOCAL QoreTypeInfo(const QoreClass* n_qc) : qc(n_qc), qt(NT_OBJECT), returns_mult(false), accepts_mult(false),
                                                  input_filter(false), has_subtype(false), has_name(false), has_defval(false),
                                                  is_int(false),
                                                  exact_return(true), ambiguous_int_match(false), accepts_all(false) {
   }

   DLLLOCAL virtual ~QoreTypeInfo() {
   }

   DLLLOCAL qore_type_t getSingleType() const {
      return qt;
   }

   // static version of method, checking for null pointer
   DLLLOCAL static qore_type_t getSingleType(const QoreTypeInfo* ti) {
      return ti ? ti->getSingleType() : NT_ALL;
   }

   // static version of method, checking for null pointer
   DLLLOCAL static QoreTypeInfo* getSubtype(const QoreTypeInfo* ti) {
      return ti && ti->has_subtype ? ti->getSubtypeIntern() : nullptr;
   }

   DLLLOCAL bool parseAcceptsReturns(qore_type_t t) const {
      if (!hasType())
         return true;

      bool n_is_int = (t == NT_INT);

      // see if type accepts given type
      if (!parseAcceptsType(t, n_is_int))
         return false;

      return parseReturnsType(t, n_is_int) ? true : false;
   }

   // static version of method, checking for null pointer
   DLLLOCAL static bool parseAcceptsReturns(const QoreTypeInfo* ti, qore_type_t t) {
      return ti ? ti->parseAcceptsReturns(t) : true;
   }

   DLLLOCAL qore_type_result_e parseReturnsType(qore_type_t t) const {
      if (!hasType())
         return QTI_AMBIGUOUS;

      bool n_is_int = t == NT_INT;
      if (returns_mult)
         return parseReturnsTypeMult(t, n_is_int);

      return matchTypeIntern(t, n_is_int);
   }

   // static version of method, checking for null pointer
   DLLLOCAL static qore_type_result_e parseReturnsType(const QoreTypeInfo* ti, qore_type_t t) {
      return ti ? ti->parseReturnsType(t) : QTI_AMBIGUOUS;
   }

   // returns true if this type only returns the value given
   DLLLOCAL bool isType(qore_type_t t) const {
      if (returns_mult)
         return false;

      return t == qt;
   }

   // returns true if type "a"" only returns the value given
   // static version of method, checking for null pointer
   DLLLOCAL static bool isType(const QoreTypeInfo* ti, qore_type_t t) {
      return ti ? ti->isType(t) : false;
   }

   DLLLOCAL bool isClass(const QoreClass* n_qc) const {
      if (returns_mult || !qc)
         return false;

      return qc->getID() == n_qc->getID();
   }

   // static version of method, checking for null pointer
   DLLLOCAL static bool isClass(const QoreTypeInfo* ti, const QoreClass* n_qc) {
      return ti ? ti->isClass(n_qc) : false;
   }

   DLLLOCAL qore_type_result_e runtimeAcceptsValue(const QoreValue n) const {
      if (!hasType() || accepts_all)
         return QTI_AMBIGUOUS;

      qore_type_t t = n.getType();

      if (t == NT_OBJECT)
         return runtimeAcceptsClass(n.get<const QoreObject>()->getClass());

      return parseAcceptsType(t, t == NT_INT);
   }

   // static version of method, checking for null pointer
   DLLLOCAL static qore_type_result_e runtimeAcceptsValue(const QoreTypeInfo* ti, const QoreValue n) {
      return ti ? ti->runtimeAcceptsValue(n) : QTI_AMBIGUOUS;
   }

   DLLLOCAL qore_type_result_e parseAccepts(const QoreTypeInfo* typeInfo) const {
      // set to true because value is ignored and can short-circuit logic in parseAcceptsMult() if called
      bool may_not_match = true;
      return parseAccepts(typeInfo, may_not_match);
   }

   // static version of method, checking for null pointer
   DLLLOCAL static qore_type_result_e parseAccepts(const QoreTypeInfo* first, const QoreTypeInfo* second) {
      return (!first || !second) ? QTI_AMBIGUOUS : first->parseAccepts(second);
   }

   DLLLOCAL qore_type_result_e parseAccepts(const QoreTypeInfo* typeInfo, bool& may_not_match) const {
      //printd(5, "QoreTypeInfo::parseAccepts() this: %p (%s) ti: %p (%s) ti->returnsSingleIntern(): %d\n", this, getName(), typeInfo, typeInfo->getName(), typeInfo->returnsSingleIntern());
      if (!hasType() || !typeInfo->hasType() || accepts_all)
         return QTI_AMBIGUOUS;

      if (!typeInfo->returnsSingleIntern()) {
         if (!accepts_mult) {
            may_not_match = true;
            return qc ? typeInfo->parseReturnsClass(qc) : typeInfo->parseReturnsType(qt, is_int);
         }
         return parseAcceptsMult(typeInfo, may_not_match);
      }

      return parseAcceptsBasic(typeInfo, may_not_match);
   }

   /*
   xxx
   DLLLOCAL qore_type_result_e parseAcceptsInitialAssignment(const QoreTypeInfo* typeInfo) const {
   }
   */

   // static version of method, checking for null pointer
   DLLLOCAL static qore_type_result_e parseAccepts(const QoreTypeInfo* first, const QoreTypeInfo* second, bool& may_not_match) {
      return (!first || !second) ? QTI_AMBIGUOUS : first->parseAccepts(second, may_not_match);
   }

   DLLLOCAL qore_type_result_e parseReturnsClass(const QoreClass* n_qc) const {
      if (!hasType())
         return QTI_AMBIGUOUS;

      if (returns_mult)
         return parseReturnsClassMult(n_qc);

      return matchClassIntern(n_qc);
   }

   // static version of method, checking for null pointer
   DLLLOCAL static qore_type_result_e parseReturnsClass(const QoreTypeInfo* ti, const QoreClass* n_qc) {
      return ti ? ti->parseReturnsClass(n_qc) : QTI_AMBIGUOUS;
   }

   DLLLOCAL const QoreClass* getUniqueReturnClass() const {
      return returns_mult ? 0 : qc;
   }

   // static version of method, checking for null pointer
   DLLLOCAL static const QoreClass* getUniqueReturnClass(const QoreTypeInfo* ti) {
      return ti ? ti->getUniqueReturnClass() : 0;
   }

   DLLLOCAL bool returnsSingle() const {
      return returnsSingleIntern() && qt >= 0;
   }

   // static version of method, checking for null pointer
   DLLLOCAL static bool returnsSingle(const QoreTypeInfo* ti) {
      return ti ? ti->returnsSingle() : false;
   }

   DLLLOCAL bool acceptsSingle() const {
      return !accepts_mult && qt != NT_ALL;
   }

   DLLLOCAL bool hasType() const {
      return (accepts_mult || returns_mult || qt != NT_ALL);
   }

   // static version of method, checking for null pointer
   DLLLOCAL static bool hasType(const QoreTypeInfo* ti) {
      return ti ? ti->hasType() : false;
   }

   // returns true if this type could contain an object or a closure
   DLLLOCAL bool needsScan() const {
      return parseReturnsType(NT_OBJECT) != QTI_NOT_EQUAL
         || parseReturnsType(NT_RUNTIME_CLOSURE) != QTI_NOT_EQUAL
         || parseReturnsType(NT_LIST) != QTI_NOT_EQUAL
         || parseReturnsType(NT_HASH) != QTI_NOT_EQUAL;
   }

   // static version of method, checking for null pointer
   DLLLOCAL static bool needsScan(const QoreTypeInfo* ti) {
      return ti ? ti->needsScan() : true;
   }

   DLLLOCAL bool hasInputFilter() const {
      return input_filter;
   }

   // static version of method, checking for null pointer
   DLLLOCAL static bool hasInputFilter(const QoreTypeInfo* ti) {
      return ti ? ti->hasInputFilter() : false;
   }

   DLLLOCAL const char* getName() const {
      if (!hasType())
         return NO_TYPE_INFO;

      if (has_name)
         return getNameImpl();

      return qc ? qc->getName() : getBuiltinTypeName(qt);
   }

   // static version of method, checking for null pointer
   DLLLOCAL static const char* getName(const QoreTypeInfo* ti) {
      return ti ? ti->getName() : NO_TYPE_INFO;
   }

   DLLLOCAL void getThisType(QoreString& str) const {
      if (qt == NT_NOTHING) {
         str.sprintf("no value");
         return;
      }
      if (qc) {
         str.sprintf("an object of class '%s'", qc->getName());
         return;
      }
      str.sprintf("type '%s'", getName());
   }

   // static version of method, checking for null pointer
   DLLLOCAL static void getThisType(const QoreTypeInfo* ti, QoreString& str) {
      if (ti)
         ti->getThisType(str);
      else
         str.sprintf("no value");
   }

   // must be reimplemented in subclasses if returns_mult is true
   DLLLOCAL virtual const type_vec_t& getReturnTypeList() const {
      assert(false);
      throw;
   }

   // must be reimplemented in subclasses if accepts_mult is true
   DLLLOCAL virtual const type_vec_t& getAcceptTypeList() const {
      assert(false);
      throw;
   }

   // FIXME: eliminate
   DLLLOCAL AbstractQoreNode* acceptInputParam(int param_num, const char* param_name, AbstractQoreNode* n, ExceptionSink* xsink) const {
      if (!hasType())
         return n;
      QoreValue v(n);
      acceptInputIntern(false, param_num, param_name, v, xsink);
      return v.takeNode();
   }

   // static version of method, checking for null pointer
   DLLLOCAL static AbstractQoreNode* acceptInputParam(const QoreTypeInfo* ti, int param_num, const char* param_name, AbstractQoreNode* n, ExceptionSink* xsink) {
      return ti ? ti->acceptInputParam(param_num, param_name, n, xsink) : n;
   }

   DLLLOCAL void acceptInputParam(int param_num, const char* param_name, QoreValue& n, ExceptionSink* xsink) const {
      if (hasType())
         acceptInputIntern(false, param_num, param_name, n, xsink);
   }

   // static version of method, checking for null pointer
   DLLLOCAL static void acceptInputParam(const QoreTypeInfo* ti, int param_num, const char* param_name, QoreValue& n, ExceptionSink* xsink) {
      if (ti)
         ti->acceptInputParam(param_num, param_name, n, xsink);
   }

   DLLLOCAL AbstractQoreNode* acceptInputMember(const char* member_name, AbstractQoreNode* n, ExceptionSink* xsink) const {
      if (!hasType())
         return n;
      QoreValue v(n);
      acceptInputIntern(true, -1, member_name, v, xsink);
      return v.takeNode();
   }

   // static version of method, checking for null pointer
   DLLLOCAL static AbstractQoreNode* acceptInputMember(const QoreTypeInfo* ti, const char* member_name, AbstractQoreNode* n, ExceptionSink* xsink) {
      return ti ? ti->acceptInputMember(member_name, n, xsink) : n;
   }

   DLLLOCAL void acceptInputMember(const char* member_name, QoreValue& n, ExceptionSink* xsink) const {
      if (hasType())
         acceptInputIntern(true, -1, member_name, n, xsink);
   }

   // static version of method, checking for null pointer
   DLLLOCAL static void acceptInputMember(const QoreTypeInfo* ti, const char* member_name, QoreValue& n, ExceptionSink* xsink) {
      if (ti)
         ti->acceptInputMember(member_name, n, xsink);
   }

   DLLLOCAL void acceptAssignment(const char* text, QoreValue& n, ExceptionSink* xsink) const {
      assert(text && text[0] == '<');
      if (hasType())
         acceptInputIntern(false, -1, text, n, xsink);
   }

   // static version of method, checking for null pointer
   DLLLOCAL static void acceptAssignment(const QoreTypeInfo* ti, const char* text, QoreValue& n, ExceptionSink* xsink) {
      if (ti)
         ti->acceptAssignment(text, n, xsink);
   }

   DLLLOCAL bool hasDefaultValue() const {
      if (!hasType())
         return false;

      return (!returns_mult && qt >= 0 && qt < NT_OBJECT) || has_defval;
   }

   // static version of method, checking for null pointer
   DLLLOCAL static bool hasDefaultValue(const QoreTypeInfo* ti) {
      return ti ? ti->hasDefaultValue() : false;
   }

   DLLLOCAL AbstractQoreNode* getDefaultValue() const {
      if (!hasType())
         return 0;

      if (has_defval)
         return getDefaultValueImpl();

      if (!returns_mult && qt >= 0 && qt < NT_OBJECT)
         return getDefaultValueForBuiltinValueType(qt);

      return 0;
   }

   // static version of method, checking for null pointer
   DLLLOCAL static AbstractQoreNode* getDefaultValue(const QoreTypeInfo* ti) {
      return ti ? ti->getDefaultValue() : 0;
   }

   DLLLOCAL QoreValue getDefaultQoreValue() const {
      if (!hasType())
         return QoreValue();

      if (has_defval)
         return getDefaultValueImpl();

      if (!returns_mult && qt >= 0 && qt < NT_OBJECT) {
         switch (qt) {
            case NT_BOOLEAN:
               return QoreValue(false);
            case NT_INT:
               return QoreValue((int64)0);
            case NT_FLOAT:
               return QoreValue((double)0.0);
            default:
               return QoreValue(getDefaultValueForBuiltinValueType(qt));
         }
      }
      return QoreValue();
   }

   // static version of method, checking for null pointer
   DLLLOCAL static QoreValue getDefaultQoreValue(const QoreTypeInfo* ti) {
      return ti ? ti->getDefaultQoreValue() : QoreValue();
   }

   // quick function to tell if the argument may be subject to an input filter for this type
   DLLLOCAL bool mayRequireFilter(const QoreValue& n) const {
      if (!hasType() || !input_filter)
         return false;

      qore_type_t nt = n.getType();
      if (nt == NT_OBJECT && qc)
         return qc->getID() == n.get<const QoreObject>()->getClass()->getID() ? false : true;

      // only set n_is_int = true if our 'is_int' is true
      // only perform the dynamic cast if the type is external
      bool n_is_int = (is_int && nt == NT_INT) ? true : false;
      if (n_is_int)
         return qt == nt ? false : true;

      return matchTypeIntern(nt, false) == QTI_IDENT ? false : true;
   }

   // static version of method, checking for null pointer
   DLLLOCAL static bool mayRequireFilter(const QoreTypeInfo* ti, const QoreValue& n) {
      return ti ? ti->mayRequireFilter(n) : false;
   }

   // quick function to tell if the argument may be subject to an input filter for this type
   DLLLOCAL bool mayRequireFilter(const AbstractQoreNode* n) const {
      if (!hasType() || !input_filter)
         return false;

      qore_type_t nt = get_node_type(n);
      if (nt == NT_OBJECT && qc)
         return qc->getID() == reinterpret_cast<const QoreObject*>(n)->getClass()->getID() ? false : true;

      // only set n_is_int = true if our 'is_int' is true
      // only perform the dynamic cast if the type is external
      bool n_is_int = (is_int && nt == NT_INT) ? true : false;
      if (n_is_int)
         return qt == nt ? false : true;

      return matchTypeIntern(nt, false) == QTI_IDENT ? false : true;
   }

   // static version of method, checking for null pointer
   DLLLOCAL static bool mayRequireFilter(const QoreTypeInfo* ti, const AbstractQoreNode* n) {
      return ti ? ti->mayRequireFilter(n) : false;
   }

   // used when parsing user code to find duplicate signatures after types are resolved
   DLLLOCAL bool isInputIdentical(const QoreTypeInfo* typeInfo) const;

   // static version of method, checking for null pointer
   DLLLOCAL static bool isInputIdentical(const QoreTypeInfo* a, const QoreTypeInfo* b) {
      if (a && b)
         return a->isInputIdentical(b);
      else if (a)
         return !a->hasType();
      else if (b)
         return !b->hasType();
      else
         return true;
   }

   DLLLOCAL bool isOutputIdentical(const QoreTypeInfo* typeInfo) const;

   // static version of method, checking for null pointer
   DLLLOCAL static bool isOutputIdentical(const QoreTypeInfo* a, const QoreTypeInfo* b) {
      if (a && b)
         return a->isOutputIdentical(b);
      else if (a)
         return !a->hasType();
      else if (b)
         return !b->hasType();
      else
         return true;
   }

   // if the argument's return type is compatible with "this"'s return type
   DLLLOCAL bool isOutputCompatible(const QoreTypeInfo* typeInfo) const;

   // if second's return type is compatible with first's return type
   // static version of method, checking for null pointer
   DLLLOCAL static bool isOutputCompatible(const QoreTypeInfo* first, const QoreTypeInfo* second) {
      if (!first || !first->hasType())
         return true;
      if (!second)
         return false;
      return first->isOutputCompatible(second);
   }

   // returns false if there is no type or if the type can be converted to a numeric value, true if otherwise
   DLLLOCAL bool nonNumericValue() const {
      if (!hasType())
         return false;

      if (returns_mult) {
         const type_vec_t& rt = getReturnTypeList();

         // return true only if none of the return types are numeric
         for (type_vec_t::const_iterator i = rt.begin(), e = rt.end(); i != e; ++i) {
            if (!(*i)->nonNumericValue())
               return false;
         }
         return true;
      }

      return is_int || qt == NT_FLOAT || qt == NT_STRING || qt == NT_BOOLEAN || qt == NT_DATE ? false : true;
   }

   // static version of method, checking for null pointer
   DLLLOCAL static bool nonNumericValue(const QoreTypeInfo* ti) {
      return ti ? ti->nonNumericValue() : false;
   }

   DLLLOCAL void doNonNumericWarning(const QoreProgramLocation& loc, const char* preface) const;
   DLLLOCAL void doNonBooleanWarning(const QoreProgramLocation& loc, const char* preface) const;

   // static version of method, checking for null pointer
   DLLLOCAL static void doNonNumericWarning(const QoreTypeInfo* ti, const QoreProgramLocation& loc, const char* preface);
   // static version of method, checking for null pointer
   DLLLOCAL static void doNonBooleanWarning(const QoreTypeInfo* ti, const QoreProgramLocation& loc, const char* preface);

   // returns false if there is no type or if the type can be converted to a string value, true if otherwise
   DLLLOCAL bool nonStringValue() const {
      if (!hasType())
         return false;

      if (returns_mult) {
         const type_vec_t& rt = getReturnTypeList();

         // return true only if none of the return types are a string
         for (type_vec_t::const_iterator i = rt.begin(), e = rt.end(); i != e; ++i) {
            if (!(*i)->nonStringValue())
               return false;
         }
         return true;
      }

      return is_int || qt == NT_FLOAT || qt == NT_STRING || qt == NT_BOOLEAN || qt == NT_DATE ? false : true;
   }

   // static version of method, checking for null pointer
   DLLLOCAL static bool nonStringValue(const QoreTypeInfo* ti) {
      return ti ? ti->nonStringValue() : false;
   }

   DLLLOCAL void doNonStringWarning(const QoreProgramLocation& loc, const char* preface) const;

   // static version of method, checking for null pointer
   DLLLOCAL static void doNonStringWarning(const QoreTypeInfo* ti, const QoreProgramLocation& loc, const char* preface);

   DLLLOCAL void concatName(std::string& str) const {
      if (!hasType()) {
         str.append(NO_TYPE_INFO);
         return;
      }

      if (returns_mult || accepts_mult || has_name || !qc)
         str.append(getName());
      else
         concatClass(str, qc->getName());
   }

   // static version of method, checking for null pointer
   DLLLOCAL static void concatName(const QoreTypeInfo* ti, std::string& str) {
      if (ti)
         ti->concatName(str);
      else
         str.append(NO_TYPE_INFO);
   }

   DLLLOCAL int doAcceptError(bool priv_error, bool obj, int param_num, const char* param_name, QoreValue& n, ExceptionSink* xsink) const {
      return QoreTypeInfo::doAcceptError(this, priv_error, obj, param_num, param_name, n, xsink);
   }

   // static version of method, checking for null pointer
   DLLLOCAL static int doAcceptError(const QoreTypeInfo* ti, bool priv_error, bool obj, int param_num, const char* param_name, QoreValue& n, ExceptionSink* xsink) {
      if (priv_error) {
         if (obj)
            QoreTypeInfo::doObjectPrivateClassException(ti, param_name, n.getInternalNode(), xsink);
         else
            QoreTypeInfo::doPrivateClassException(ti, param_num + 1, param_name, n.getInternalNode(), xsink);
      }
      else {
         if (obj)
            QoreTypeInfo::doObjectTypeException(ti, param_name, n, xsink);
         else
            QoreTypeInfo::doTypeException(ti, param_num + 1, param_name, n, xsink);
      }
      return -1;
   }

   DLLLOCAL int doAcceptError(bool priv_error, bool obj, int param_num, const char* param_name, AbstractQoreNode* n, ExceptionSink* xsink) const {
      return QoreTypeInfo::doAcceptError(this, priv_error, obj, param_num, param_name, n, xsink);
   }

   // static version of method, checking for null pointer
   DLLLOCAL static int doAcceptError(const QoreTypeInfo* ti, bool priv_error, bool obj, int param_num, const char* param_name, AbstractQoreNode* n, ExceptionSink* xsink) {
      if (priv_error) {
         if (obj)
            QoreTypeInfo::doObjectPrivateClassException(ti, param_name, n, xsink);
         else
            QoreTypeInfo::doPrivateClassException(ti, param_num + 1, param_name, n, xsink);
      }
      else {
         if (obj)
            QoreTypeInfo::doObjectTypeException(ti, param_name, n, xsink);
         else
            QoreTypeInfo::doTypeException(ti, param_num + 1, param_name, n, xsink);
      }
      return -1;
   }

   DLLLOCAL int doTypeException(int param_num, const char* param_name, const QoreValue& n, ExceptionSink* xsink) const {
      return doTypeException(this, param_num, param_name, n, xsink);
   }

   // static version of method, checking for null pointer
   DLLLOCAL static int doTypeException(const QoreTypeInfo* ti, int param_num, const char* param_name, const QoreValue& n, ExceptionSink* xsink) {
      // xsink may be null in case parse exceptions have been disabled in the QoreProgram object
      // for example if there was a "requires" error
      if (!xsink)
         return -1;

      QoreStringNode* desc = new QoreStringNode;
      QoreTypeInfo::ptext(*desc, param_num, param_name);
      desc->concat("expects ");
      QoreTypeInfo::getThisType(ti, *desc);
      desc->concat(", but got ");
      getNodeType(*desc, n);
      desc->concat(" instead");
      xsink->raiseException("RUNTIME-TYPE-ERROR", desc);
      return -1;
   }

   DLLLOCAL int doTypeException(int param_num, const char* param_name, const AbstractQoreNode* n, ExceptionSink* xsink) const {
      return doTypeException(this, param_num, param_name, n, xsink);
   }

   // static version of method, checking for null pointer
   DLLLOCAL static int doTypeException(const QoreTypeInfo* ti, int param_num, const char* param_name, const AbstractQoreNode* n, ExceptionSink* xsink) {
      // xsink may be null in case parse exceptions have been disabled in the QoreProgram object
      // for example if there was a "requires" error
      if (!xsink)
         return -1;

      QoreStringNode* desc = new QoreStringNode;
      QoreTypeInfo::ptext(*desc, param_num, param_name);
      desc->concat("expects ");
      QoreTypeInfo::getThisType(ti, *desc);
      desc->concat(", but got ");
      getNodeType(*desc, n);
      desc->concat(" instead");
      xsink->raiseException("RUNTIME-TYPE-ERROR", desc);
      return -1;
   }

   DLLLOCAL int doTypeException(int param_num, const char* param_name, const char* n, ExceptionSink* xsink) const {
      return doTypeException(this, param_num, param_name, n, xsink);
   }

   // static version of method, checking for null pointer
   DLLLOCAL static int doTypeException(const QoreTypeInfo* ti, int param_num, const char* param_name, const char* n, ExceptionSink* xsink) {
      assert(xsink);
      QoreStringNode* desc = new QoreStringNode;
      QoreTypeInfo::ptext(*desc, param_num, param_name);
      desc->concat("expects ");
      QoreTypeInfo::getThisType(ti, *desc);
      desc->sprintf(", but got type '%s' instead", n);
      xsink->raiseException("RUNTIME-TYPE-ERROR", desc);
      return -1;
   }
};

// this is basically just a wrapper around NamedScope
class QoreParseTypeInfo {
protected:
   bool or_nothing;
   std::string tname;

   DLLLOCAL QoreParseTypeInfo(const NamedScope* n_cscope) : or_nothing(false), cscope(n_cscope->copy()) {
      setName();
   }

   DLLLOCAL void setName() {
      if (or_nothing)
         tname = "*";
      tname += cscope->getIdentifier();
   }

public:
   NamedScope* cscope; // namespace scope for class
   QoreParseTypeInfo* subtype = nullptr;

   DLLLOCAL QoreParseTypeInfo(char* n_cscope, bool n_or_nothing = false, QoreParseTypeInfo* subtype = nullptr) : or_nothing(n_or_nothing), cscope(new NamedScope(n_cscope)), subtype(subtype) {
      setName();
      assert(strcmp(n_cscope, "any"));
   }

   DLLLOCAL QoreParseTypeInfo(const QoreParseTypeInfo& old) : or_nothing(old.or_nothing), tname(old.tname), cscope(old.cscope ? new NamedScope(*old.cscope) : nullptr), subtype(old.subtype ? new QoreParseTypeInfo(*old.subtype) : nullptr) {
   }

   DLLLOCAL ~QoreParseTypeInfo() {
      delete cscope;
      delete subtype;
   }

   // prototype (expecting type) should be "this"
   // returns true if the prototype does not expect any type or the types are compatible,
   // false if otherwise
   DLLLOCAL bool parseStageOneEqual(const QoreParseTypeInfo* typeInfo) const {
      return !strcmp(cscope->getIdentifier(), typeInfo->cscope->getIdentifier());
   }

   // used when parsing user code to find duplicate signatures
   DLLLOCAL bool parseStageOneIdenticalWithParsed(const QoreTypeInfo* typeInfo, bool& recheck) const {
      if (!typeInfo->hasType())
         return false;

      const QoreClass* qc = QoreTypeInfo::getUniqueReturnClass(typeInfo);
      if (!qc)
         return false;

      // both have class info
      if (!strcmp(cscope->getIdentifier(), qc->getName()))
         return recheck = true;
      else
         return false;
   }

   // static version of method, checking for null pointer
   DLLLOCAL static bool parseStageOneIdenticalWithParsed(const QoreParseTypeInfo* pti, const QoreTypeInfo* typeInfo, bool& recheck) {
      if (pti && typeInfo)
         return pti->parseStageOneIdenticalWithParsed(typeInfo, recheck);
      else if (pti)
         return false;
      else if (typeInfo)
         return !typeInfo->hasType();
      else
         return true;
   }

   // used when parsing user code to find duplicate signatures
   DLLLOCAL bool parseStageOneIdentical(const QoreParseTypeInfo* typeInfo) const {
      if (!typeInfo)
         return false;

      return !strcmp(cscope->ostr, typeInfo->cscope->ostr);
   }

   // static version of method, checking for null pointer
   DLLLOCAL static bool parseStageOneIdentical(const QoreParseTypeInfo* pti, const QoreParseTypeInfo* typeInfo) {
      if (pti && typeInfo)
         return pti->parseStageOneIdentical(typeInfo);
      else
         return !(pti || typeInfo);
   }

   // resolves the current type to a QoreTypeInfo pointer and deletes itself
   DLLLOCAL const QoreTypeInfo* resolveAndDelete(const QoreProgramLocation& loc);

   // static version of method, checking for null pointer
   DLLLOCAL static const QoreTypeInfo* resolveAndDelete(QoreParseTypeInfo* pti, const QoreProgramLocation& loc) {
      return pti ? pti->resolveAndDelete(loc) : 0;
   }

#ifdef DEBUG
   DLLLOCAL const char* getCID() const { return cscope ? cscope->getIdentifier() : "n/a"; }

   // static version of method, checking for null pointer
   DLLLOCAL static const char* getCID(const QoreParseTypeInfo* pti) { return pti ? pti->getCID() : "n/a"; }
#endif

   DLLLOCAL QoreParseTypeInfo* copy() const {
      return new QoreParseTypeInfo(cscope);
   }

   DLLLOCAL const char* getName() const {
      return tname.c_str();
   }

   // static version of method, checking for null pointer
   DLLLOCAL static const char* getName(const QoreParseTypeInfo* pti) {
      return pti ? pti->getName() : NO_TYPE_INFO;
   }

   DLLLOCAL void concatName(std::string& str) const {
      concatClass(str, cscope->getIdentifier());
   }

   // static version of method, checking for null pointer
   DLLLOCAL static void concatName(const QoreParseTypeInfo* pti, std::string& str) {
      if (pti)
         pti->concatName(str);
      else
         str.append(NO_TYPE_INFO);
   }
};

class AcceptsMultiTypeInfo : public QoreTypeInfo {
protected:
   type_vec_t at;

   DLLLOCAL virtual const type_vec_t& getAcceptTypeList() const {
      return at;
   }

public:
   DLLLOCAL AcceptsMultiTypeInfo(const QoreClass* n_qc, qore_type_t n_qt, bool n_returns_mult,
                                 bool n_input_filter = false, bool n_has_subtype = false,
                                 bool n_has_name = false, bool n_has_defval = false,
                                 bool n_is_int = false, bool n_exact_return = false,
                                 bool n_accepts_all = false) :
      QoreTypeInfo(n_qc, n_qt, n_returns_mult, true, n_input_filter, n_has_subtype, n_has_name,
                   n_has_defval, n_is_int, n_exact_return, n_accepts_all) {
   }
};

class AcceptsMultiFilterTypeInfo : public AcceptsMultiTypeInfo {
protected:
   // must be reimplemented in subclasses if input_filter is true
   DLLLOCAL virtual bool acceptInputImpl(QoreValue& n, ExceptionSink* xsink) const = 0;

public:
   DLLLOCAL AcceptsMultiFilterTypeInfo(const QoreClass* n_qc, qore_type_t n_qt, bool n_returns_mult, bool n_has_subtype = false,
                                       bool n_has_name = false, bool n_has_defval = false,
                                       bool n_is_int = false, bool n_exact_return = false,
                                       bool n_accepts_all = false) :
      AcceptsMultiTypeInfo(n_qc, n_qt, n_returns_mult, true, n_has_subtype, n_has_name,
                           n_has_defval, n_is_int, n_exact_return, n_accepts_all) {
   }
};

class AcceptsReturnsMultiFilterTypeInfo : public AcceptsMultiFilterTypeInfo {
protected:
   type_vec_t rt;

   DLLLOCAL virtual const type_vec_t& getReturnTypeList() const {
      return rt;
   }

public:
   DLLLOCAL AcceptsReturnsMultiFilterTypeInfo(const QoreClass* n_qc, qore_type_t n_qt, bool n_has_subtype = false,
                                              bool n_has_name = false, bool n_has_defval = false,
                                              bool n_is_int = false) :
      AcceptsMultiFilterTypeInfo(n_qc, n_qt, true, n_has_subtype, n_has_name, n_has_defval, n_is_int, false) {
   }
};

class FloatOrNothingTypeInfo : public AcceptsReturnsMultiFilterTypeInfo {
protected:
   DLLLOCAL virtual const char* getNameImpl() const {
      return "*float";
   }

   DLLLOCAL bool acceptInputImpl(QoreValue& n, ExceptionSink* xsink) const {
      qore_type_t t = n.getType();

      if (t == NT_FLOAT || t == NT_NOTHING)
         return true;

      if (t == NT_NULL) {
         discard(n.assign((AbstractQoreNode*)0), xsink);
         return true;
      }

      if (t != NT_INT)
         return false;

      discard(n.assign((double)n.getAsBigInt()), xsink);
      return true;
   }

public:
   DLLLOCAL FloatOrNothingTypeInfo() : AcceptsReturnsMultiFilterTypeInfo(0, NT_FLOAT, false, true, false, false) {
      assert(bigIntTypeInfo);
      at.push_back(bigIntTypeInfo);
      assert(floatTypeInfo);
      at.push_back(floatTypeInfo);
      assert(nothingTypeInfo);
      at.push_back(nothingTypeInfo);
      assert(nullTypeInfo);
      at.push_back(nullTypeInfo);

      rt.push_back(floatTypeInfo);
      rt.push_back(nothingTypeInfo);
   }
};

class FloatTypeInfo : public AcceptsMultiFilterTypeInfo {
protected:
   DLLLOCAL bool acceptInputImpl(QoreValue& n, ExceptionSink* xsink) const {
      qore_type_t t = n.getType();

      if (t == NT_FLOAT)
         return true;

      if (t != NT_INT)
         return false;

      discard(n.assign((double)n.getAsBigInt()), xsink);
      return true;
   }

public:
   DLLLOCAL FloatTypeInfo() : AcceptsMultiFilterTypeInfo(0, NT_FLOAT, false, false, false, false, false, true) {
      assert(bigIntTypeInfo);
      at.push_back(bigIntTypeInfo);
   }
};

class NumberOrNothingTypeInfo : public AcceptsReturnsMultiFilterTypeInfo {
protected:
   DLLLOCAL virtual const char* getNameImpl() const {
      return "*number";
   }

   DLLLOCAL bool acceptInputImpl(QoreValue& n, ExceptionSink* xsink) const {
      qore_type_t t = n.getType();

      if (t == NT_NUMBER || t == NT_NOTHING)
         return true;

      if (t == NT_NULL) {
         discard(n.assign((AbstractQoreNode*)0), xsink);
         return true;
      }

      if (t == NT_FLOAT) {
         discard(n.assign(new QoreNumberNode(n.getAsFloat())), xsink);
         return true;
      }

      if (t != NT_INT)
         return false;

      discard(n.assign(new QoreNumberNode(n.getAsBigInt())), xsink);
      return true;
   }

public:
   DLLLOCAL NumberOrNothingTypeInfo() : AcceptsReturnsMultiFilterTypeInfo(0, NT_NUMBER, false, true, false, false) {
      assert(numberTypeInfo);
      at.push_back(numberTypeInfo);
      assert(bigIntTypeInfo);
      at.push_back(bigIntTypeInfo);
      assert(floatTypeInfo);
      at.push_back(floatTypeInfo);
      assert(nothingTypeInfo);
      at.push_back(nothingTypeInfo);
      assert(nullTypeInfo);
      at.push_back(nullTypeInfo);

      rt.push_back(numberTypeInfo);
      rt.push_back(nothingTypeInfo);
   }
};

class NumberTypeInfo : public AcceptsMultiFilterTypeInfo {
protected:
   DLLLOCAL bool acceptInputImpl(QoreValue& n, ExceptionSink* xsink) const {
      qore_type_t t = n.getType();

      if (t == NT_NUMBER)
         return true;

      if (t == NT_FLOAT) {
         discard(n.assign(new QoreNumberNode(n.getAsFloat())), xsink);
         return true;
      }

      // only perform dynamic cast if type is external
      if (t != NT_INT)
         return false;

      discard(n.assign(new QoreNumberNode(n.getAsBigInt())), xsink);
      return true;
   }

public:
   DLLLOCAL NumberTypeInfo() : AcceptsMultiFilterTypeInfo(0, NT_NUMBER, false, false, false, false, false, true) {
      assert(bigIntTypeInfo);
      at.push_back(bigIntTypeInfo);
      assert(floatTypeInfo);
      at.push_back(floatTypeInfo);
   }
};

class IntTypeInfo : public QoreTypeInfo {
public:
   DLLLOCAL IntTypeInfo(qore_type_t n_qt, bool n_accepts_mult = false, bool n_input_filter = false,
                        bool n_has_name = false, bool n_has_defval = false, bool n_exact_return = true) :
      QoreTypeInfo(0, n_qt, false, n_accepts_mult, n_input_filter, false, n_has_name, n_has_defval,
                   true, n_exact_return, false) {
   }
};

class BigIntTypeInfo : public IntTypeInfo {
public:
   DLLLOCAL BigIntTypeInfo() : IntTypeInfo(NT_INT) {
   }
};

class AcceptsReturnsSameMultiTypeInfo : public AcceptsMultiTypeInfo {
protected:
   DLLLOCAL virtual const type_vec_t& getReturnTypeList() const {
      return at;
   }

   DLLLOCAL virtual const char* getNameImpl() const = 0;

public:
   DLLLOCAL AcceptsReturnsSameMultiTypeInfo(const QoreClass* n_qc, qore_type_t n_qt,
                                            bool n_input_filter = false, bool n_has_subtype = false,
                                            bool n_is_int = false) :
      AcceptsMultiTypeInfo(n_qc, n_qt, true, n_input_filter, n_has_subtype, true, false, n_is_int) {
   }
};

class OrNothingTypeInfo : public AcceptsReturnsMultiFilterTypeInfo {
protected:
   QoreString tname;

   DLLLOCAL virtual const char* getNameImpl() const {
      return tname.getBuffer();
   }

   DLLLOCAL bool acceptInputImpl(QoreValue& n, ExceptionSink* xsink) const;

public:
   DLLLOCAL OrNothingTypeInfo(const QoreTypeInfo& ti, const char* name) : AcceptsReturnsMultiFilterTypeInfo(ti.qc, ti.qt, ti.has_subtype, true) {
      assert(ti.hasType());

      tname = "*";
      tname += name;

      assert(!ti.input_filter);

      if (ti.accepts_mult)
         at = ti.getAcceptTypeList();
      else
         at.push_back(&ti);

      at.push_back(nothingTypeInfo);
      at.push_back(nullTypeInfo);

      if (ti.returns_mult)
         rt = ti.getReturnTypeList();
      else
         rt.push_back(&ti);
      rt.push_back(nothingTypeInfo);
   }
};

// expect a ResolvedCallReferenceNode with this type
class CodeTypeInfo : public AcceptsReturnsSameMultiTypeInfo {
protected:
   DLLLOCAL virtual const char* getNameImpl() const {
      return "code";
   }

public:
   DLLLOCAL CodeTypeInfo() : AcceptsReturnsSameMultiTypeInfo(0, NT_CODE) {
      at.push_back(callReferenceTypeInfo);
      at.push_back(runTimeClosureTypeInfo);
   }
};

class CodeOrNothingTypeInfo : public AcceptsReturnsMultiFilterTypeInfo {
protected:
   DLLLOCAL virtual const char* getNameImpl() const {
      return "*code";
   }

   DLLLOCAL bool acceptInputImpl(QoreValue& n, ExceptionSink* xsink) const {
      if (n.getType() == NT_NULL)
         discard(n.assign((AbstractQoreNode*)0), xsink);
      return true;
   }

public:
   DLLLOCAL CodeOrNothingTypeInfo() : AcceptsReturnsMultiFilterTypeInfo(0, NT_CODE, false, true, false, false) {
      at.push_back(codeTypeInfo);
      at.push_back(nothingTypeInfo);
      at.push_back(nullTypeInfo);

      rt.push_back(codeTypeInfo);
      rt.push_back(nothingTypeInfo);
   }
};

class IntOrFloatTypeInfo : public AcceptsReturnsSameMultiTypeInfo {
protected:
   DLLLOCAL virtual const char* getNameImpl() const {
      return "int|float";
   }

public:
   DLLLOCAL IntOrFloatTypeInfo() : AcceptsReturnsSameMultiTypeInfo(0, NT_INTORFLOAT) {
      at.push_back(bigIntTypeInfo);
      at.push_back(floatTypeInfo);
   }
};

class IntFloatOrNumberTypeInfo : public AcceptsReturnsSameMultiTypeInfo {
protected:
   DLLLOCAL virtual const char* getNameImpl() const {
      return "int|float|number";
   }

public:
   DLLLOCAL IntFloatOrNumberTypeInfo() : AcceptsReturnsSameMultiTypeInfo(0, NT_INTFLOATORNUMBER) {
      at.push_back(bigIntTypeInfo);
      at.push_back(floatTypeInfo);
      at.push_back(numberTypeInfo);
   }
};

class FloatOrNumberTypeInfo : public AcceptsReturnsSameMultiTypeInfo {
protected:
   DLLLOCAL virtual const char* getNameImpl() const {
      return "float|number";
   }

public:
   DLLLOCAL FloatOrNumberTypeInfo() : AcceptsReturnsSameMultiTypeInfo(0, NT_FLOATORNUMBER) {
      at.push_back(floatTypeInfo);
      at.push_back(numberTypeInfo);
   }
};

// accepts QoreStringNode or BinaryNode and passes through
class DataTypeInfo : public AcceptsReturnsSameMultiTypeInfo {
protected:
   DLLLOCAL virtual const char* getNameImpl() const {
      return "data";
   }

public:
   DLLLOCAL DataTypeInfo() : AcceptsReturnsSameMultiTypeInfo(0, NT_DATA) {
      at.push_back(stringTypeInfo);
      at.push_back(binaryTypeInfo);
   }
};

class DataOrNothingTypeInfo : public AcceptsReturnsMultiFilterTypeInfo {
protected:
   DLLLOCAL virtual const char* getNameImpl() const {
      return "*data";
   }

   DLLLOCAL bool acceptInputImpl(QoreValue& n, ExceptionSink* xsink) const {
      if (n.getType() == NT_NULL)
         discard(n.assign((AbstractQoreNode*)0), xsink);
      return true;
   }

public:
   DLLLOCAL DataOrNothingTypeInfo() : AcceptsReturnsMultiFilterTypeInfo(0, NT_DATA, false, true, false, false) {
      at.push_back(stringTypeInfo);
      at.push_back(binaryTypeInfo);
      at.push_back(nothingTypeInfo);
      at.push_back(nullTypeInfo);

      rt.push_back(stringTypeInfo);
      rt.push_back(binaryTypeInfo);
      rt.push_back(nothingTypeInfo);
   }
};

// accepts int, float, number, string, date, null, or boolean and returns an int
class SoftBigIntTypeInfo : public AcceptsMultiFilterTypeInfo {
protected:
   DLLLOCAL virtual const char* getNameImpl() const {
      return "softint";
   }

   DLLLOCAL virtual bool acceptInputImpl(QoreValue& n, ExceptionSink* xsink) const {
      qore_type_t t = n.getType();

      if (t == NT_INT)
         return true;

      if (t != NT_FLOAT
            && t != NT_NUMBER
            && t != NT_STRING
            && t != NT_BOOLEAN
            && t != NT_DATE
            && t != NT_NULL)
         return false;

      discard(n.assign(n.getAsBigInt()), xsink);
      return true;
   }

   // must be reimplemented in subclasses if has_defval is true
   DLLLOCAL virtual AbstractQoreNode* getDefaultValueImpl() const {
      return zero();
   }

   DLLLOCAL void init() {
      at.push_back(floatTypeInfo);
      at.push_back(numberTypeInfo);
      at.push_back(stringTypeInfo);
      at.push_back(boolTypeInfo);
      at.push_back(dateTypeInfo);
      at.push_back(nullTypeInfo);
   }

public:
   DLLLOCAL SoftBigIntTypeInfo(bool n_returns_mult = false) : AcceptsMultiFilterTypeInfo(0, NT_INT, n_returns_mult, false, true, n_returns_mult ? false : true, n_returns_mult ? false : true, n_returns_mult ? false : true) {
      init();
   }
};

class SoftBigIntOrNothingTypeInfo : public AcceptsReturnsMultiFilterTypeInfo {
protected:
   DLLLOCAL virtual const char* getNameImpl() const {
      return "*softint";
   }

   DLLLOCAL virtual bool acceptInputImpl(QoreValue& n, ExceptionSink* xsink) const {
      qore_type_t t = n.getType();

      if (t == NT_NULL) {
         discard(n.assign((AbstractQoreNode*)0), xsink);
         return true;
      }

      if (t == NT_NOTHING || t == NT_INT)
         return true;

      if (t != NT_FLOAT
            && t != NT_NUMBER
            && t != NT_STRING
            && t != NT_BOOLEAN
            && t != NT_DATE
            && t != NT_NULL)
         return false;

      discard(n.assign(n.getAsBigInt()), xsink);
      return true;
   }

public:
   DLLLOCAL SoftBigIntOrNothingTypeInfo() : AcceptsReturnsMultiFilterTypeInfo(0, NT_INT, false, true, false, false) {
      at.push_back(floatTypeInfo);
      at.push_back(numberTypeInfo);
      at.push_back(stringTypeInfo);
      at.push_back(boolTypeInfo);
      at.push_back(dateTypeInfo);
      at.push_back(nullTypeInfo);
      at.push_back(nothingTypeInfo);

      rt.push_back(bigIntTypeInfo);
      rt.push_back(nothingTypeInfo);
   }
};

// accepts int, float, number, string, date, null, or boolean and returns a float
class SoftFloatTypeInfo : public AcceptsMultiFilterTypeInfo {
protected:
   DLLLOCAL virtual const char* getNameImpl() const {
      return "softfloat";
   }

   DLLLOCAL virtual bool acceptInputImpl(QoreValue& n, ExceptionSink* xsink) const {
      qore_type_t t = n.getType();

      if (t == NT_FLOAT)
         return true;

      if (t != NT_INT
            && t != NT_NUMBER
            && t != NT_STRING
            && t != NT_BOOLEAN
            && t != NT_DATE
            && t != NT_NULL)
         return false;

      discard(n.assign(n.getAsFloat()), xsink);
      return true;
   }

   // must be reimplemented in subclasses if has_defval is true
   DLLLOCAL virtual AbstractQoreNode* getDefaultValueImpl() const {
      return zero_float();
   }

public:
   DLLLOCAL SoftFloatTypeInfo(bool n_returns_mult = false) : AcceptsMultiFilterTypeInfo(0, NT_FLOAT, n_returns_mult, false, true, n_returns_mult ? false : true, false, n_returns_mult ? false : true) {
      at.push_back(numberTypeInfo);
      at.push_back(bigIntTypeInfo);
      at.push_back(stringTypeInfo);
      at.push_back(boolTypeInfo);
      at.push_back(dateTypeInfo);
      at.push_back(nullTypeInfo);
   }
};

class SoftFloatOrNothingTypeInfo : public AcceptsReturnsMultiFilterTypeInfo {
protected:
   DLLLOCAL virtual const char* getNameImpl() const {
      return "*softfloat";
   }

   DLLLOCAL virtual bool acceptInputImpl(QoreValue& n, ExceptionSink* xsink) const {
      qore_type_t t = n.getType();

      if (t == NT_FLOAT || t == NT_NOTHING)
         return true;

      if (t == NT_NULL) {
         discard(n.assign((AbstractQoreNode*)0), xsink);
         return true;
      }

      if (t != NT_INT
            && t != NT_NUMBER
            && t != NT_STRING
            && t != NT_BOOLEAN
            && t != NT_DATE
            && t != NT_NULL)
         return false;

      discard(n.assign(n.getAsFloat()), xsink);
      return true;
   }

public:
   DLLLOCAL SoftFloatOrNothingTypeInfo() : AcceptsReturnsMultiFilterTypeInfo(0, NT_FLOAT, false, true, false, false) {
      at.push_back(numberTypeInfo);
      at.push_back(bigIntTypeInfo);
      at.push_back(stringTypeInfo);
      at.push_back(boolTypeInfo);
      at.push_back(dateTypeInfo);
      at.push_back(nullTypeInfo);
      at.push_back(nothingTypeInfo);

      rt.push_back(floatTypeInfo);
      rt.push_back(nothingTypeInfo);
   }
};

// accepts int, float, number, string, date, null, or boolean and returns a number
class SoftNumberTypeInfo : public AcceptsMultiFilterTypeInfo {
protected:
   DLLLOCAL virtual const char* getNameImpl() const {
      return "softnumber";
   }

   DLLLOCAL virtual bool acceptInputImpl(QoreValue& n, ExceptionSink* xsink) const {
      qore_type_t t = n.getType();

      if (t == NT_NUMBER)
         return true;

      if (t == NT_FLOAT) {
         discard(n.assign(new QoreNumberNode(n.getAsFloat())), xsink);
         return true;
      }

      if (t == NT_STRING) {
         discard(n.assign(new QoreNumberNode(reinterpret_cast<const QoreStringNode*>(n.getInternalNode())->getBuffer())), xsink);
         return true;
      }

      if (t == NT_INT) {
         discard(n.assign(new QoreNumberNode(n.getAsBigInt())), xsink);
         return true;
      }

      if (t != NT_BOOLEAN
          && t != NT_DATE
          && t != NT_NULL)
         return false;

      discard(n.assign(new QoreNumberNode(n.getAsFloat())), xsink);
      return true;
   }

   // must be reimplemented in subclasses if has_defval is true
   DLLLOCAL virtual AbstractQoreNode* getDefaultValueImpl() const {
      // XXX return zero_number();
      return 0;
   }

public:
   DLLLOCAL SoftNumberTypeInfo(bool n_returns_mult = false) : AcceptsMultiFilterTypeInfo(0, NT_NUMBER, n_returns_mult, false, true, n_returns_mult ? false : true, false, n_returns_mult ? false : true) {
      at.push_back(floatTypeInfo);
      at.push_back(bigIntTypeInfo);
      at.push_back(stringTypeInfo);
      at.push_back(boolTypeInfo);
      at.push_back(dateTypeInfo);
      at.push_back(nullTypeInfo);
   }
};

class SoftNumberOrNothingTypeInfo : public AcceptsReturnsMultiFilterTypeInfo {
protected:
   DLLLOCAL virtual const char* getNameImpl() const {
      return "*softnumber";
   }

   DLLLOCAL virtual bool acceptInputImpl(QoreValue& n, ExceptionSink* xsink) const {
      qore_type_t t = n.getType();

      if (t == NT_NUMBER || t == NT_NOTHING)
         return true;

      if (t == NT_NULL) {
         discard(n.assign((AbstractQoreNode*)0), xsink);
         return true;
      }

      if (t == NT_FLOAT) {
         discard(n.assign(new QoreNumberNode(n.getAsFloat())), xsink);
         return true;
      }

      if (t == NT_STRING) {
         discard(n.assign(new QoreNumberNode(reinterpret_cast<const QoreStringNode*>(n.getInternalNode())->getBuffer())), xsink);
         return true;
      }

      if (t == NT_INT) {
         discard(n.assign(new QoreNumberNode(n.getAsBigInt())), xsink);
         return true;
      }

      if (t != NT_BOOLEAN
          && t != NT_DATE
          && t != NT_NULL)
         return false;

      discard(n.assign(new QoreNumberNode(n.getAsFloat())), xsink);
      return true;
   }

public:
   DLLLOCAL SoftNumberOrNothingTypeInfo() : AcceptsReturnsMultiFilterTypeInfo(0, NT_NUMBER, false, true, false, false) {
      at.push_back(floatTypeInfo);
      at.push_back(bigIntTypeInfo);
      at.push_back(stringTypeInfo);
      at.push_back(boolTypeInfo);
      at.push_back(dateTypeInfo);
      at.push_back(nullTypeInfo);
      at.push_back(nothingTypeInfo);

      rt.push_back(numberTypeInfo);
      rt.push_back(nothingTypeInfo);
   }
};

// accepts int, float, number, string, date, null, or boolean and returns a boolean
class SoftBoolTypeInfo : public AcceptsMultiFilterTypeInfo {
protected:
   DLLLOCAL virtual const char* getNameImpl() const {
      return "softbool";
   }

   DLLLOCAL virtual bool acceptInputImpl(QoreValue& n, ExceptionSink* xsink) const {
      qore_type_t t = n.getType();

      if (t == NT_BOOLEAN)
         return true;

      if (t != NT_INT
            && t != NT_FLOAT
            && t != NT_NUMBER
            && t != NT_STRING
            && t != NT_DATE
            && t != NT_NULL)
         return false;

      discard(n.assign(n.getAsBool()), xsink);
      return true;
   }

   // must be reimplemented in subclasses if has_defval is true
   DLLLOCAL virtual AbstractQoreNode* getDefaultValueImpl() const {
      return &False;
   }

public:
   DLLLOCAL SoftBoolTypeInfo(bool n_returns_mult = false) : AcceptsMultiFilterTypeInfo(0, NT_BOOLEAN, n_returns_mult, false, true, n_returns_mult ? false : true, false, n_returns_mult ? false : true) {
      at.push_back(bigIntTypeInfo);
      at.push_back(floatTypeInfo);
      at.push_back(numberTypeInfo);
      at.push_back(stringTypeInfo);
      at.push_back(dateTypeInfo);
      at.push_back(nullTypeInfo);
   }
};

class SoftBoolOrNothingTypeInfo : public AcceptsReturnsMultiFilterTypeInfo {
protected:
   DLLLOCAL virtual const char* getNameImpl() const {
      return "*softbool";
   }

   DLLLOCAL virtual bool acceptInputImpl(QoreValue& n, ExceptionSink* xsink) const {
      qore_type_t t = n.getType();

      if (t == NT_BOOLEAN || t == NT_NOTHING)
         return true;

      if (t == NT_NULL) {
         discard(n.assign((AbstractQoreNode*)0), xsink);
         return true;
      }

      if (t != NT_INT
            && t != NT_NUMBER
            && t != NT_FLOAT
            && t != NT_STRING
            && t != NT_DATE
            && t != NT_NULL)
         return false;

      discard(n.assign(n.getAsBool()), xsink);
      return true;
   }

public:
   DLLLOCAL SoftBoolOrNothingTypeInfo() : AcceptsReturnsMultiFilterTypeInfo(0, NT_BOOLEAN, false, true, false, false) {
      at.push_back(bigIntTypeInfo);
      at.push_back(floatTypeInfo);
      at.push_back(numberTypeInfo);
      at.push_back(stringTypeInfo);
      at.push_back(dateTypeInfo);
      at.push_back(nullTypeInfo);
      at.push_back(nothingTypeInfo);

      rt.push_back(boolTypeInfo);
      rt.push_back(nothingTypeInfo);
   }
};

// accepts int, float, number, string, date, bool, or null and returns a date
class SoftDateTypeInfo : public AcceptsMultiFilterTypeInfo {
protected:
   DLLLOCAL virtual const char* getNameImpl() const {
      return "softdate";
   }

   DLLLOCAL virtual bool acceptInputImpl(QoreValue& n, ExceptionSink* xsink) const {
      qore_type_t t = n.getType();

      if (t == NT_DATE)
         return true;

      if (t == NT_INT
          || t == NT_BOOLEAN
          || t == NT_FLOAT
          || t == NT_NULL) {
         discard(n.assign(new DateTimeNode(n.getAsBigInt())), xsink);
         return true;
      }

      if (t != NT_NUMBER
          && t != NT_STRING)
         return false;

      DateTimeNodeValueHelper dt(n.getInternalNode());
      discard(n.assign(dt.getReferencedValue()), xsink);
      return true;
   }

   // must be reimplemented in subclasses if has_defval is true
   DLLLOCAL virtual AbstractQoreNode* getDefaultValueImpl() const {
      return &False;
   }

public:
   DLLLOCAL SoftDateTypeInfo(bool n_returns_mult = false) : AcceptsMultiFilterTypeInfo(0, NT_DATE, n_returns_mult, false, true, n_returns_mult ? false : true, false, n_returns_mult ? false : true) {
      at.push_back(bigIntTypeInfo);
      at.push_back(floatTypeInfo);
      at.push_back(numberTypeInfo);
      at.push_back(stringTypeInfo);
      at.push_back(boolTypeInfo);
      at.push_back(nullTypeInfo);
   }
};

class SoftDateOrNothingTypeInfo : public AcceptsReturnsMultiFilterTypeInfo {
protected:
   DLLLOCAL virtual const char* getNameImpl() const {
      return "*softdate";
   }

   DLLLOCAL virtual bool acceptInputImpl(QoreValue& n, ExceptionSink* xsink) const {
      qore_type_t t = n.getType();

      if (t == NT_DATE || t == NT_NOTHING)
         return true;

      if (t == NT_NULL) {
         discard(n.assign((AbstractQoreNode*)0), xsink);
         return true;
      }

      if (t == NT_INT
          || t == NT_BOOLEAN
          || t == NT_FLOAT
          || t == NT_NULL) {
         discard(n.assign(new DateTimeNode(n.getAsBigInt())), xsink);
         return true;
      }

      if (t != NT_NUMBER
          && t != NT_STRING)
         return false;

      DateTimeNodeValueHelper dt(n.getInternalNode());
      discard(n.assign(dt.getReferencedValue()), xsink);
      return true;
   }

public:
   DLLLOCAL SoftDateOrNothingTypeInfo() : AcceptsReturnsMultiFilterTypeInfo(0, NT_DATE, false, true, false, false) {
      at.push_back(bigIntTypeInfo);
      at.push_back(floatTypeInfo);
      at.push_back(numberTypeInfo);
      at.push_back(stringTypeInfo);
      at.push_back(boolTypeInfo);
      at.push_back(nullTypeInfo);
      at.push_back(nothingTypeInfo);

      rt.push_back(dateTypeInfo);
      rt.push_back(nothingTypeInfo);
   }
};

// accepts int, float, number, string, date, null, or boolean and returns a string
class SoftStringTypeInfo : public AcceptsMultiFilterTypeInfo {
protected:
   DLLLOCAL virtual const char* getNameImpl() const {
      return "softstring";
   }

   DLLLOCAL virtual bool acceptInputImpl(QoreValue& n, ExceptionSink* xsink) const {
      qore_type_t t = n.getType();

      if (t == NT_STRING)
         return true;

      if (t == NT_INT
          || t == NT_BOOLEAN) {
         discard(n.assign(new QoreStringNodeMaker(QLLD, n.getAsBigInt())), xsink);
         return true;
      }

      if (t == NT_NULL) {
         n.assign(NullString->stringRefSelf());
         return true;
      }

      if (t == NT_FLOAT) {
         discard(n.assign(q_fix_decimal(new QoreStringNodeMaker("%.9g", n.getAsFloat()))), xsink);
         return true;
      }

      if (t != NT_NUMBER
          && t != NT_DATE)
         return false;

      QoreStringNodeValueHelper str(n.getInternalNode());
      discard(n.assign(str.getReferencedValue()), xsink);
      return true;
   }

   // must be reimplemented in subclasses if has_defval is true
   DLLLOCAL virtual AbstractQoreNode* getDefaultValueImpl() const {
      return &False;
   }

public:
   DLLLOCAL SoftStringTypeInfo(bool n_returns_mult = false) : AcceptsMultiFilterTypeInfo(0, NT_STRING, n_returns_mult, false, true, n_returns_mult ? false : true, false, n_returns_mult ? false : true) {
      at.push_back(bigIntTypeInfo);
      at.push_back(floatTypeInfo);
      at.push_back(numberTypeInfo);
      at.push_back(boolTypeInfo);
      at.push_back(dateTypeInfo);
      at.push_back(nullTypeInfo);
   }
};

class SoftStringOrNothingTypeInfo : public AcceptsReturnsMultiFilterTypeInfo {
protected:
   DLLLOCAL virtual const char* getNameImpl() const {
      return "*softstring";
   }

   DLLLOCAL virtual bool acceptInputImpl(QoreValue& n, ExceptionSink* xsink) const {
      qore_type_t t = n.getType();

      if (t == NT_STRING || t == NT_NOTHING)
         return true;

      if (t == NT_NULL) {
         discard(n.assign((AbstractQoreNode*)0), xsink);
         return true;
      }

      if (t == NT_INT
          || t == NT_BOOLEAN
          || t == NT_NULL) {
         discard(n.assign(new QoreStringNodeMaker(QLLD, n.getAsBigInt())), xsink);
         return true;
      }

      if (t == NT_FLOAT) {
         discard(n.assign(q_fix_decimal(new QoreStringNodeMaker("%.9g", n.getAsFloat()))), xsink);
         return true;
      }

      if (t != NT_NUMBER
          && t != NT_DATE)
         return false;

      QoreStringNodeValueHelper str(n.getInternalNode());
      discard(n.assign(str.getReferencedValue()), xsink);
      return true;
   }

public:
   DLLLOCAL SoftStringOrNothingTypeInfo() : AcceptsReturnsMultiFilterTypeInfo(0, NT_STRING, false, true, false, false) {
      at.push_back(bigIntTypeInfo);
      at.push_back(floatTypeInfo);
      at.push_back(numberTypeInfo);
      at.push_back(boolTypeInfo);
      at.push_back(dateTypeInfo);
      at.push_back(nullTypeInfo);
      at.push_back(nothingTypeInfo);

      rt.push_back(stringTypeInfo);
      rt.push_back(nothingTypeInfo);
   }
};

// accepts int or date and returns an int representing time in milliseconds
class TimeoutTypeInfo : public AcceptsMultiFilterTypeInfo {
protected:
   DLLLOCAL virtual const char* getNameImpl() const {
      return "timeout";
   }

   DLLLOCAL virtual bool acceptInputImpl(QoreValue& n, ExceptionSink* xsink) const {
      qore_type_t t = n.getType();

      if (t == NT_INT)
         return true;

      if (t != NT_DATE)
         return false;

      int64 ms = reinterpret_cast<const DateTimeNode*>(n.getInternalNode())->getRelativeMilliseconds();
      discard(n.assign(ms), xsink);
      return true;
   }

   // must be reimplemented in subclasses if has_defval is true
   DLLLOCAL virtual AbstractQoreNode* getDefaultValueImpl() const {
      return &False;
   }

public:
   DLLLOCAL TimeoutTypeInfo(bool n_returns_mult = false) : AcceptsMultiFilterTypeInfo(0, NT_INT, n_returns_mult, false, true, n_returns_mult ? false : true, n_returns_mult ? false : true, n_returns_mult ? false : true) {
      at.push_back(dateTypeInfo);
   }
};

class TimeoutOrNothingTypeInfo : public AcceptsReturnsMultiFilterTypeInfo {
protected:
   DLLLOCAL virtual const char* getNameImpl() const {
      return "*timeout";
   }

   DLLLOCAL virtual bool acceptInputImpl(QoreValue& n, ExceptionSink* xsink) const {
      qore_type_t t = n.getType();

      if (t == NT_INT || t == NT_NOTHING)
         return true;

      if (t == NT_NULL) {
         discard(n.assign((AbstractQoreNode*)0), xsink);
         return true;
      }

      if (t != NT_DATE)
         return false;

      int64 ms = reinterpret_cast<const DateTimeNode*>(n.getInternalNode())->getRelativeMilliseconds();
      discard(n.assign(ms), xsink);
      return true;
   }

public:
   DLLLOCAL TimeoutOrNothingTypeInfo() : AcceptsReturnsMultiFilterTypeInfo(0, NT_INT, false, true, false, false) {
      at.push_back(bigIntTypeInfo);
      at.push_back(dateTypeInfo);
      at.push_back(nothingTypeInfo);
      at.push_back(nullTypeInfo);

      rt.push_back(bigIntTypeInfo);
      rt.push_back(nothingTypeInfo);
   }
};

// accepts everything and returns a list:
/** NOTHING -> ()
    list -> same list
    everything else: list(arg)
*/
class SoftListTypeInfo : public AcceptsMultiFilterTypeInfo {
protected:
   DLLLOCAL virtual const char* getNameImpl() const {
      return "softlist";
   }

   DLLLOCAL virtual bool acceptInputImpl(QoreValue& n, ExceptionSink* xsink) const {
      //printd(5, "SoftListTypeInfo::acceptInputImpl() n=%p %s\n", n, get_type_name(n));
      qore_type_t t = n.getType();

      if (t == NT_LIST)
         return true;

      QoreListNode* l = new QoreListNode;
      if (t != NT_NOTHING)
         l->push(n.takeNode());

      discard(n.assign(l), xsink);
      return true;
   }

   // must be reimplemented in subclasses if has_defval is true
   DLLLOCAL virtual AbstractQoreNode* getDefaultValueImpl() const {
      return new QoreListNode;
   }

public:
   DLLLOCAL SoftListTypeInfo(bool n_returns_mult = false) : AcceptsMultiFilterTypeInfo(0, NT_LIST, n_returns_mult, false, true, n_returns_mult ? false : true, n_returns_mult ? false : true, n_returns_mult ? false : true, true) {
   }
};

// accepts everything and returns a list:
/** NOTHING || list -> same value
    everything else: list(arg)
*/
class SoftListOrNothingTypeInfo : public SoftListTypeInfo {
protected:
   type_vec_t rt;

   DLLLOCAL virtual const type_vec_t& getReturnTypeList() const {
      return rt;
   }

   DLLLOCAL virtual bool acceptInputImpl(QoreValue& n, ExceptionSink* xsink) const {
      qore_type_t t = n.getType();

      if (t == NT_NULL) {
         discard(n.assign((AbstractQoreNode*)0), xsink);
         return true;
      }

      if (t == NT_LIST || t == NT_NOTHING)
         return true;

      QoreListNode* l = new QoreListNode;
      l->push(n.takeNode());
      discard(n.assign(l), xsink);
      return true;
   }

public:
   DLLLOCAL SoftListOrNothingTypeInfo() : SoftListTypeInfo(true) {
      rt.push_back(listTypeInfo);
      rt.push_back(nothingTypeInfo);
   }
};

/*
class ReferenceTypeInfo : public AcceptsReturnsSameMultiTypeInfo {
public:
   DLLLOCAL ReferenceTypeInfo() : AcceptsReturnsSameMultiTypeInfo(nullptr, NT_REFERENCE, false, false, false) {
      at.push_back(anyTypeInfo);
   }

   DLLLOCAL virtual const char* getNameImpl() const {
      return "reference";
   }
};
*/

class ReferenceTypeInfo : public QoreTypeInfo {
public:
   DLLLOCAL ReferenceTypeInfo() : QoreTypeInfo(nullptr, NT_REFERENCE, false, false, false, false, true, false, false, false, false) {
   }

   DLLLOCAL virtual const char* getNameImpl() const {
      return "reference";
   }
};

/*
   DLLLOCAL QoreTypeInfo(const QoreClass* n_qc, qore_type_t n_qt, bool n_returns_mult,
                         bool n_accepts_mult, bool n_input_filter, bool n_has_subtype,
                         bool n_has_name, bool n_has_defval,
                         bool n_is_int, bool n_exact_return, bool n_accepts_all) : {}
   DLLLOCAL AcceptsMultiTypeInfo(const QoreClass* n_qc, qore_type_t n_qt, bool n_returns_mult,
                                 bool n_input_filter = false, bool n_has_subtype = false,
                                 bool n_has_name = false, bool n_has_defval = false,
                                 bool n_is_int = false, bool n_exact_return = false, bool n_accepts_all = false) :
   DLLLOCAL AcceptsMultiFilterTypeInfo(const QoreClass* n_qc, qore_type_t n_qt, bool n_returns_mult, bool n_has_subtype = false,
                                       bool n_has_name = false, bool n_has_defval = false,
                                       bool n_is_int = false, bool n_exact_return = false, bool n_accepts_all = false) :
*/

class ExternalTypeInfo : public QoreTypeInfo {
protected:
   const char* tname;
   const QoreTypeInfoHelper& helper;
   type_vec_t at;

   DLLLOCAL virtual const char* getNameImpl() const {
      return tname;
   }

   DLLLOCAL virtual const type_vec_t& getAcceptTypeList() const {
      return at;
   }

   DLLLOCAL virtual bool acceptInputImpl(QoreValue& n, ExceptionSink* xsink) const {
      return helper.acceptInputImpl(n, xsink);
   }

   DLLLOCAL virtual bool acceptInputImpl(AbstractQoreNode*& v, ExceptionSink* xsink) const {
      QoreValue n(v);
      bool b = helper.acceptInputImpl(n, xsink);
      v = n.takeNode();
      return b;
   }

public:
   // used for base types
   DLLLOCAL ExternalTypeInfo(qore_type_t n_qt, const char* n_tname, const QoreTypeInfoHelper& n_helper, bool n_is_int = false, bool n_exact_return = true, bool n_accepts_all = false) :
      QoreTypeInfo(0, n_qt,
                   false, // returns_mult
                   false, // accepts_mult
                   false, // input_filter
                   false, // has_subtype
                   true,  // has_name
                   false, // has_defval
                   n_is_int, n_exact_return, n_accepts_all), tname(n_tname), helper(n_helper) {
      assert(tname);

      assert(has_name);

      //printd(5, "ExternalTypeInfo::ExternalTypeInfo() this=%p qt=%n qc=%p name=%s\n", this, qt, qc, tname);
   }

   // used for classes
   DLLLOCAL ExternalTypeInfo(const QoreClass* n_qc, const QoreTypeInfoHelper& n_helper) : QoreTypeInfo(n_qc), tname(0), helper(n_helper) {
      //printd(5, "ExternalTypeInfo::ExternalTypeInfo() this=%p qt=%n qc=%p name=%s\n", this, qt, qc, qc->getName());
   }

   // used when assigning a base type after the fact
   DLLLOCAL ExternalTypeInfo(const char* n_tname, const QoreTypeInfoHelper& n_helper) :
      QoreTypeInfo(0, NT_NOTHING,
                   false,  // returns_mult
                   false,  // accepts_mult
                   false,  // input_filter
                   false,  // has_subtype
                   true,   // has_name
                   false,  // has_defval
                   false,  // is_int
                   true,   // exact_return
                   false   // accepts_all
         ),
      tname(n_tname), helper(n_helper) {
   }
   // used for assigning a class after the fact
   DLLLOCAL ExternalTypeInfo(const QoreTypeInfoHelper& n_helper) : tname(0), helper(n_helper) {
   }
   DLLLOCAL void assign(qore_type_t n_qt, const char* n_tname = 0) {
      qt = n_qt;
      if (n_tname) {
         has_name = true;
         tname = n_tname;
      }
   }
   DLLLOCAL void assign(const QoreClass* n_qc) {
      assert(n_qc);
      qt = NT_OBJECT;
      qc = n_qc;
      assert(!tname);
      //tname = qc->getName();
   }
   DLLLOCAL void addAcceptsType(const QoreTypeInfo* typeInfo) {
      assert(typeInfo);
      assert(typeInfo != this);

      if (!accepts_mult)
         accepts_mult = true;

      at.push_back(typeInfo);
   }
   DLLLOCAL void setInt() {
      assert(!qc);
      is_int = true;
   }
   DLLLOCAL void setInexactReturn() {
      exact_return = false;
   }
   DLLLOCAL void setInputFilter() {
      input_filter = true;
   }
   DLLLOCAL void setIntMatch() {
      ambiguous_int_match = true;
   }
};

// returns type info for base types
DLLLOCAL const QoreTypeInfo* getTypeInfoForType(qore_type_t t);
// returns type info information for parse types (values)
DLLLOCAL const QoreTypeInfo* getTypeInfoForValue(const AbstractQoreNode* n);

#endif // _QORE_QORETYPEINFO_H
