/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreTypeInfo.h

  Qore Programming Language

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

#ifndef _QORE_QORETYPEINFO_H

#define _QORE_QORETYPEINFO_H

#include <map>

#define NO_TYPE_INFO "<no type info>"

// internal "QTI" codes
#define QTI_IGNORE     -2
#define QTI_UNASSIGNED -1

// adds external types to global type map
DLLLOCAL void add_to_type_map(qore_type_t t, const QoreTypeInfo *typeInfo);
DLLLOCAL bool builtinTypeHasDefaultValue(qore_type_t t);
// returns the default value for any type >= 0 and < NT_OBJECT
DLLLOCAL AbstractQoreNode *getDefaultValueForBuiltinValueType(qore_type_t t);

class AbstractQoreTypeInfo {
protected:
   qore_type_t qt : 11;
   qore_type_t compat_qt : 11;
   bool has_type : 1;

   DLLLOCAL virtual const char *getNameImpl() const = 0;
   DLLLOCAL virtual void concatNameImpl(std::string &str) const = 0;

   DLLLOCAL void concatClass(std::string &str, const char *cn) const {
      str.append("<class: ");
      str.append(cn);
      str.push_back('>');
   }

public:
   DLLLOCAL AbstractQoreTypeInfo(qore_type_t n_qt) : qt(n_qt), compat_qt(n_qt), has_type(n_qt != NT_ALL ? true : false) {
   }
   DLLLOCAL AbstractQoreTypeInfo(qore_type_t n_qt, qore_type_t n_compat_qt) : qt(n_qt), compat_qt(n_compat_qt), has_type(n_qt != NT_ALL ? true : false) {
   }
   DLLLOCAL AbstractQoreTypeInfo() : qt(NT_ALL), compat_qt(NT_ALL), has_type(false) {
   }
   DLLLOCAL virtual ~AbstractQoreTypeInfo() {
   }
   DLLLOCAL const char *getName() const {
      if (!this || !has_type)
	 return NO_TYPE_INFO;
      return getNameImpl();
   }
   DLLLOCAL void concatName(std::string &str) const {
      if (!this || !has_type) {
	 str.append(NO_TYPE_INFO);
	 return;
      }
      return concatNameImpl(str);
   }
   DLLLOCAL bool hasDefaultValue() const {
      return this ? builtinTypeHasDefaultValue(qt) : false;
   }
   DLLLOCAL AbstractQoreNode *getDefaultValue() const {
      if (!this)
         return 0;
      if (qt >= 0 && qt < NT_OBJECT)
         return getDefaultValueForBuiltinValueType(qt);
      return getDefaultValueImpl();
   }

   DLLLOCAL virtual bool checkTypeInstantiationImpl(AbstractQoreNode *&n, ExceptionSink *xsink) const = 0;
   DLLLOCAL virtual int testTypeCompatibilityImpl(const AbstractQoreNode *n) const = 0;
   DLLLOCAL virtual int parseEqualImpl(const QoreTypeInfo *typeInfo) const = 0;
   DLLLOCAL virtual AbstractQoreNode *getDefaultValueImpl() const {
      return 0;
   }
   
   // returns false if there is no type or if the type can be converted to a numeric value, true if otherwise
   DLLLOCAL bool nonNumericValue() const {
      if (!this || !has_type)
         return false;

      return qt == NT_INT || qt == NT_FLOAT || qt == NT_STRING || qt == NT_BOOLEAN ? false : true;
   }

   DLLLOCAL qore_type_t typeCompatibility() const {
      return this ? compat_qt : NT_ALL;
   }
};

class QoreTypeInfo : public AbstractQoreTypeInfo {
protected:
   DLLLOCAL static void ptext(QoreStringNode &str, int param_num, const char *param_name) {
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
            str.sprintf("('$%s') ", param_name);
      }
      else
         str.sprintf("'$%s' ", param_name);
   }

   DLLLOCAL int doTypeException(int param_num, const char *param_name, const AbstractQoreNode *n, ExceptionSink *xsink) const {
      // xsink may be null in case parse exceptions have been disabled in the QoreProgram object
      // for example if there was a "requires" error
      if (!xsink)
	 return -1;

      QoreStringNode *desc = new QoreStringNode;
      QoreTypeInfo::ptext(*desc, param_num, param_name);
      desc->concat("expects ");
      getThisType(*desc);
      desc->concat(", but got ");
      getNodeType(*desc, n);
      desc->concat(" instead");
      xsink->raiseException("RUNTIME-TYPE-ERROR", desc);
      return -1;
   }

   DLLLOCAL int doPrivateClassException(int param_num, const char *param_name, const AbstractQoreNode *n, ExceptionSink *xsink) const {
      // xsink may be null in case that parse exceptions have been disabled in the QoreProgram object
      // for example if there was a "requires" error
      if (!xsink)
	 return -1;

      QoreStringNode *desc = new QoreStringNode;
      QoreTypeInfo::ptext(*desc, param_num, param_name);
      desc->concat("expects ");
      getThisType(*desc);
      desc->concat(", but got an object where this class is privately inherited instead");
      xsink->raiseException("RUNTIME-TYPE-ERROR", desc);
      return -1;
   }

   DLLLOCAL int doObjectTypeException(const char *param_name, const AbstractQoreNode *n, ExceptionSink *xsink) const {
      assert(xsink);
      QoreStringNode *desc = new QoreStringNode;
      desc->sprintf("member '$.%s' expects ", param_name);
      getThisType(*desc);
      desc->concat(", but got ");
      getNodeType(*desc, n);
      desc->concat(" instead");
      xsink->raiseException("RUNTIME-TYPE-ERROR", desc);
      return -1;
   }

   DLLLOCAL int doObjectPrivateClassException(const char *param_name, const AbstractQoreNode *n, ExceptionSink *xsink) const {
      assert(xsink);
      QoreStringNode *desc = new QoreStringNode;
      desc->sprintf("member '$.%s' expects ", param_name);
      getThisType(*desc);
      desc->concat(", but got an object where this class is privately inherited instead");
      xsink->raiseException("RUNTIME-TYPE-ERROR", desc);
      return -1;
   }

   // returns 0 = OK, -1 = error
   DLLLOCAL int checkTypeInstantiationDefault(AbstractQoreNode *n, bool &priv_error) const;

   DLLLOCAL AbstractQoreNode *checkTypeInstantiationIntern(bool obj, int param_num, const char *param_name, AbstractQoreNode *n, ExceptionSink *xsink) const {
      bool priv_error = false;
      if (!checkTypeInstantiationDefault(n, priv_error))
	 return n;

      //printd(0, "QoreTypeInfo::checkTypeInstantiationIntern() this=%p %s n=%p %s\n", this, getName(), n, n ? n->getTypeName() : "NOTHING");
      if (!checkTypeInstantiationImpl(n, xsink)) {
	 if (*xsink)
	    return n;
	 if (priv_error) {
	    if (obj) doObjectPrivateClassException(param_name, n, xsink);
	    else doPrivateClassException(param_num + 1, param_name, n, xsink);
	 }
	 else {
	    if (obj) doObjectTypeException(param_name, n, xsink);
	    else doTypeException(param_num + 1, param_name, n, xsink);
	 }
      }
      return n;
   }

   DLLLOCAL virtual const char *getNameImpl() const {
      return qc ? qc->getName() : getBuiltinTypeName(qt);
   }

   DLLLOCAL virtual void concatNameImpl(std::string &str) const {
      if (qc)
	 concatClass(str, qc->getName());
      else
	 str.append(getBuiltinTypeName(qt));
   }

public:
   const QoreClass *qc;

   DLLLOCAL QoreTypeInfo() : qc(0) {}
   DLLLOCAL QoreTypeInfo(qore_type_t n_qt) : AbstractQoreTypeInfo(n_qt), qc(0) {}
   DLLLOCAL QoreTypeInfo(qore_type_t n_qt, qore_type_t n_compat_qt) : AbstractQoreTypeInfo(n_qt, n_compat_qt), qc(0) {}
   DLLLOCAL QoreTypeInfo(const QoreClass *n_qc) : AbstractQoreTypeInfo(NT_OBJECT), qc(n_qc) {}
   DLLLOCAL virtual ~QoreTypeInfo() {
   }

   DLLLOCAL bool parseExactMatch(qore_type_t t) const {
      return this && qt == t;
   }

   DLLLOCAL qore_type_t getType() const { return qt; }
   DLLLOCAL void getNodeType(QoreStringNode &str, const AbstractQoreNode *n) const {
      if (is_nothing(n)) {
	 str.concat("no value");
	 return;
      }
      if (n->getType() != NT_OBJECT) {
	 str.sprintf("builtin type '%s'", n->getTypeName());
	 return;
      }
      str.sprintf("an object of class '%s'", reinterpret_cast<const QoreObject *>(n)->getClassName());
   }

   DLLLOCAL void getThisType(QoreStringNode &str) const {
      if (!this || qt == NT_NOTHING) {
	 str.sprintf("no value");
	 return;
      }
      if (qc) {
	 str.sprintf("an object of class '%s'", qc->getName());
	 return;
      }
      str.sprintf("builtin type '%s'", getBuiltinTypeName(qt));
   }

   // prototype (expecting type) should be "this"
   // returns:
   //  QTI_IDENT      if the prototype does not expect any type or the types are equal
   //  QTI_AMBIGUOUS  if the prototype can accept the given type
   //  QTI_NOT_EQUAL  if the prototype cannot accept the given type
   // false if otherwise
   DLLLOCAL int parseEqualDefault(const QoreTypeInfo *typeInfo) const;

   DLLLOCAL int parseEqual(const QoreTypeInfo *typeInfo) const {
      int rc = parseEqualDefault(typeInfo);
      return rc == QTI_NOT_EQUAL ? parseEqualImpl(typeInfo) : rc;
   }

   // can be called when this == null
   DLLLOCAL bool hasType() const { return this ? has_type : false; }

   DLLLOCAL void set(const QoreTypeInfo &ti) {
      assert(!has_type);
      assert(ti.has_type);
      qc = ti.qc;
      qt = ti.qt;
      has_type = true;
   }

   // returns QTI_IDENT for perfect match, QTI_AMBIGUOUS for an acceptible match, QTI_NOT_EQUAL for no match, "this" is the parameter type, n is the argument
   DLLLOCAL int testTypeCompatibilityDefault(const AbstractQoreNode *n) const;

   DLLLOCAL int testTypeCompatibility(const AbstractQoreNode *n) const {
      int rc = testTypeCompatibilityDefault(n);
      return rc == QTI_NOT_EQUAL ? testTypeCompatibilityImpl(n) : rc;
   }
   
   DLLLOCAL AbstractQoreNode *checkType(const char *text, AbstractQoreNode *n, ExceptionSink *xsink) const {
      assert(text && text[0] == '<');
      return checkTypeInstantiation(-1, text, n, xsink);
   }

   DLLLOCAL AbstractQoreNode *checkTypeInstantiation(int param_num, const char *param_name, AbstractQoreNode *n, ExceptionSink *xsink) const {
      if (!this) return n;
      return checkTypeInstantiationIntern(false, param_num, param_name, n, xsink);
   }

   DLLLOCAL AbstractQoreNode *checkMemberTypeInstantiation(const char *param_name, AbstractQoreNode *n, ExceptionSink *xsink) const {
      if (!this) return n;
      return checkTypeInstantiationIntern(true, -1, param_name, n, xsink);
   }

   // used when parsing user code to find duplicate signatures after types are resolved
   DLLLOCAL bool checkIdentical(const QoreTypeInfo *typeInfo) const {
      bool thisnt = (!this || !has_type);
      bool typent = (!typeInfo || !typeInfo->has_type);

      if (thisnt && typent)
	 return true;

      if (thisnt || typent)
	 return false;

      // from this point on, we know that both have types
      if (qt != typeInfo->qt)
	 return false;

      // both types are identical
      if (qt != NT_OBJECT)
	 return true;

      if (qc) {
	 if (!typeInfo->qc)
	    return false;
	 return qc == typeInfo->qc;
      }
      return !typeInfo->qc;
   }

   DLLLOCAL virtual bool checkTypeInstantiationImpl(AbstractQoreNode *&n, ExceptionSink *xsink) const {
      return false;
   }

   DLLLOCAL virtual int testTypeCompatibilityImpl(const AbstractQoreNode *n) const {
      return QTI_NOT_EQUAL;
   }

   DLLLOCAL virtual int parseEqualImpl(const QoreTypeInfo *typeInfo) const {
      return QTI_NOT_EQUAL;
   }

   DLLLOCAL bool parseTestCompatibleClass(const QoreClass *qc) const;

   DLLLOCAL void doNonNumericWarning(const char *preface) const {
      QoreStringNode *desc = new QoreStringNode(preface);
      getThisType(*desc);
      desc->sprintf(", which does not evaluate to a numeric type, therefore will always evaluate to 0 at runtime");
      getProgram()->makeParseWarning(QP_WARN_INVALID_OPERATION, "INVALID-OPERATION", desc);
   }

   DLLLOCAL void doNonBooleanWarning(const char *preface) const {
      QoreStringNode *desc = new QoreStringNode(preface);
      getThisType(*desc);
      desc->sprintf(", which does not evaluate to a numeric type, therefore will always evaluate to False at runtime");
      getProgram()->makeParseWarning(QP_WARN_INVALID_OPERATION, "INVALID-OPERATION", desc);
   }

#ifdef DEBUG
   DLLLOCAL const char *getTypeName() const { return this && qt >= 0 ? getBuiltinTypeName(qt) : "n/a"; }
#endif
};

class QoreParseTypeInfo : public QoreTypeInfo {
protected:
   DLLLOCAL virtual const char *getNameImpl() const {
      if (cscope)
	 return cscope->getIdentifier();
      return qc ? qc->getName() : getBuiltinTypeName(qt);
   }

   DLLLOCAL virtual void concatNameImpl(std::string &str) const {
      if (cscope)
	 concatClass(str, cscope->getIdentifier());
      else
         QoreTypeInfo::concatNameImpl(str);
   }

   DLLLOCAL QoreParseTypeInfo(const NamedScope *n_cscope) : QoreTypeInfo(NT_OBJECT), cscope(n_cscope->copy()) {
   }

public:
   NamedScope *cscope; // namespace scope for class

   DLLLOCAL QoreParseTypeInfo() : QoreTypeInfo(), cscope(0) {}
   DLLLOCAL QoreParseTypeInfo(qore_type_t qt) : QoreTypeInfo(qt), cscope(0) {}
   DLLLOCAL QoreParseTypeInfo(char *n_cscope) : QoreTypeInfo(NT_OBJECT), cscope(new NamedScope(n_cscope)) {
      assert(strcmp(n_cscope, "any"));
   }
   DLLLOCAL QoreParseTypeInfo(const QoreClass *qc) : QoreTypeInfo(qc), cscope(0) {}
   DLLLOCAL QoreParseTypeInfo(const QoreTypeInfo *typeInfo) : QoreTypeInfo(), cscope(0) {
      if (typeInfo) {
	 qc = typeInfo->qc;
	 qt = typeInfo->getType();
	 has_type = typeInfo->hasType();
      }
   }
/*
   DLLLOCAL QoreParseTypeInfo(qore_type_t qt, char *n_cscope) : QoreTypeInfo(qt), cscope(n_cscope ? new NamedScope(n_cscope) : 0) {
      assert(!cscope || qt == NT_OBJECT);
   }
*/
   DLLLOCAL virtual ~QoreParseTypeInfo() {
      delete cscope;
   }
   // prototype (expecting type) should be "this"
   // returns true if the prototype does not expect any type or the types are compatible, 
   // false if otherwise
   DLLLOCAL bool parseStageOneEqual(const QoreParseTypeInfo *typeInfo) const {
      if (!this || !has_type || !typeInfo || !typeInfo->has_type)
	 return true;

      if (!cscope && !typeInfo->cscope)
	 return parseEqual(typeInfo);

      assert(cscope && typeInfo->cscope);
      // check for equal types or equal class paths
      if (qt != typeInfo->qt)
	 return false;
      if (!cscope && !typeInfo->cscope)
	 return true;
      if (!cscope || !typeInfo->cscope)
	 return false;
      return !strcmp(cscope->getIdentifier(), typeInfo->cscope->getIdentifier());
   }

   // used when parsing user code to find duplicate signatures
   DLLLOCAL bool parseStageOneIdenticalWithParsed(const QoreTypeInfo *typeInfo, bool &recheck) const {
      bool thisnt = (!this || !has_type);
      bool typent = (!typeInfo || !typeInfo->hasType());

      if (thisnt && typent)
	 return true;

      if (thisnt || typent)
	 return false;

      // from this point on, we know that both have types
      if (qt != typeInfo->getType())
	 return false;

      // both types are identical
      if (qt != NT_OBJECT)
	 return true;

      if (cscope) {
	 if (!typeInfo->qc)
	    return false;
	 // both have class info
	 if (!strcmp(cscope->getIdentifier(), qc->getName()))
	    return recheck = true;
	 else
	    return false;
      }
      return !typeInfo->qc;
   }

   // used when parsing user code to find duplicate signatures
   DLLLOCAL bool parseStageOneIdentical(const QoreParseTypeInfo *typeInfo) const {
      bool thisnt = (!this || !has_type);
      bool typent = (!typeInfo || !typeInfo->has_type);

      if (thisnt && typent)
	 return true;

      if (thisnt || typent)
	 return false;

      // from this point on, we know that both have types
      if (qt != typeInfo->qt)
	 return false;

      // both types are identical
      if (qt != NT_OBJECT)
	 return true;

      if (cscope) {
	 if (!typeInfo->cscope)
	    return false;
	 return !strcmp(cscope->ostr, typeInfo->cscope->ostr);
      }
      return !typeInfo->cscope;
   }

   // resolves the current type to a QoreTypeInfo pointer and deletes itself
   DLLLOCAL const QoreTypeInfo *resolveAndDelete();
   DLLLOCAL bool needsResolving() const { 
      return cscope;
   }
#ifdef DEBUG
   DLLLOCAL const char *getCID() const { return this && cscope ? cscope->getIdentifier() : "n/a"; }
#endif
   DLLLOCAL QoreParseTypeInfo *copy() const {
      if (!this)
	 return 0;

      assert(!qc);

      if (cscope)
         return new QoreParseTypeInfo(cscope);

      if (qc)
	 return new QoreParseTypeInfo(qc);
      if (!has_type)
	 return new QoreParseTypeInfo;
      return new QoreParseTypeInfo(qt);
   }

   DLLLOCAL NamedScope *takeName() {
      NamedScope *rv = cscope;
      cscope = 0;
      return rv;
   }
};

// SomethingTypeInfo, i.e. not NOTHING
class SomethingTypeInfo : public QoreTypeInfo {
protected:
   DLLLOCAL virtual bool checkTypeInstantiationImpl(AbstractQoreNode *&n, ExceptionSink *xsink) const {
      return is_nothing(n) ? false : true;
   }
   DLLLOCAL virtual int testTypeCompatibilityImpl(const AbstractQoreNode *n) const {
      return is_nothing(n) ? QTI_NOT_EQUAL : QTI_AMBIGUOUS;
   }
   DLLLOCAL virtual int parseEqualImpl(const QoreTypeInfo *typeInfo) const {
      return typeInfo && (typeInfo->getType() != NT_NOTHING) ? QTI_AMBIGUOUS : QTI_NOT_EQUAL;
   }

public:
   DLLLOCAL SomethingTypeInfo() : QoreTypeInfo(NT_SOMETHING) {
   }
};

class FloatTypeInfo : public QoreTypeInfo {
protected:
   DLLLOCAL virtual bool checkTypeInstantiationImpl(AbstractQoreNode *&n, ExceptionSink *xsink) const {
      //printd(0, "FloatTypeInfo::checkTypeInstantiationImpl() n=%p %s\n", n, n->getTypeName());
      if (!n)
	 return false;

      if (n->getType() != NT_INT)
         return false;

      QoreFloatNode *f = new QoreFloatNode(reinterpret_cast<QoreBigIntNode *>(n)->val);
      n->deref(xsink);
      n = f;
      return true;
   }
   DLLLOCAL virtual int testTypeCompatibilityImpl(const AbstractQoreNode *n) const {
      return n && (n->getType() == NT_INT) ? QTI_AMBIGUOUS : QTI_NOT_EQUAL;
   }
   DLLLOCAL virtual int parseEqualImpl(const QoreTypeInfo *typeInfo) const {
      return typeInfo && (typeInfo->getType() == NT_INT) ? QTI_AMBIGUOUS : QTI_NOT_EQUAL;
   }

public:
   DLLLOCAL FloatTypeInfo() : QoreTypeInfo(NT_FLOAT) {
   }
};

class BigIntTypeInfo : public QoreTypeInfo {
protected:
   DLLLOCAL virtual bool checkTypeInstantiationImpl(AbstractQoreNode *&n, ExceptionSink *xsink) const {
      //printd(0, "BigIntTypeInfo::checkTypeInstantiationImpl() n=%p %s\n", n, n->getTypeName());
      return dynamic_cast<QoreBigIntNode *>(n) ? true : false;
   }
   DLLLOCAL virtual int testTypeCompatibilityImpl(const AbstractQoreNode *n) const {
      return dynamic_cast<const QoreBigIntNode *>(n) ? QTI_AMBIGUOUS : QTI_NOT_EQUAL;
   }

public:
   DLLLOCAL BigIntTypeInfo() : QoreTypeInfo(NT_INT) {
   }
};

class StringTypeInfo : public QoreTypeInfo {
protected:
   DLLLOCAL virtual bool checkTypeInstantiationImpl(AbstractQoreNode *&n, ExceptionSink *xsink) const {
      //printd(0, "BigIntTypeInfo::checkTypeInstantiationImpl() n=%p %s\n", n, n->getTypeName());
      return dynamic_cast<QoreStringNode *>(n) ? true : false;
   }
   DLLLOCAL virtual int testTypeCompatibilityImpl(const AbstractQoreNode *n) const {
      return dynamic_cast<const QoreStringNode *>(n) ? QTI_AMBIGUOUS : QTI_NOT_EQUAL;
   }

public:
   DLLLOCAL StringTypeInfo() : QoreTypeInfo(NT_STRING) {
   }
};

// expect a ResolvedCallReferenceNode with this type 
class CodeTypeInfo : public QoreTypeInfo {
protected:
   DLLLOCAL virtual bool checkTypeInstantiationImpl(AbstractQoreNode *&n, ExceptionSink *xsink) const {
      //printd(0, "CodeTypeInfo::checkTypeInstantiationImpl() n=%p %s\n", n, n->getTypeName());
      if (!n || (n->getType() != NT_FUNCREF && (n->getType() != NT_RUNTIME_CLOSURE)))
	 return false;

      return true;
   }
   DLLLOCAL virtual int testTypeCompatibilityImpl(const AbstractQoreNode *n) const {
      return n && (n->getType() == NT_FUNCREF || n->getType() == NT_RUNTIME_CLOSURE) ? QTI_AMBIGUOUS : QTI_NOT_EQUAL;
   }
   DLLLOCAL virtual int parseEqualImpl(const QoreTypeInfo *typeInfo) const {
      qore_type_t t = typeInfo ? typeInfo->getType() : NT_NONE;
      return t == NT_FUNCREF || t == NT_RUNTIME_CLOSURE ? QTI_AMBIGUOUS : QTI_NOT_EQUAL;
   }

public:
   DLLLOCAL CodeTypeInfo() : QoreTypeInfo(NT_CODE) {
   }
};

class UserReferenceTypeInfo : public QoreTypeInfo {
protected:
   // accept any type at runtime
   DLLLOCAL virtual bool checkTypeInstantiationImpl(AbstractQoreNode *&n, ExceptionSink *xsink) const {
      return true;
   }
   DLLLOCAL virtual int testTypeCompatibilityImpl(const AbstractQoreNode *n) const {
      return QTI_AMBIGUOUS;
   }
   DLLLOCAL virtual int parseEqualImpl(const QoreTypeInfo *typeInfo) const {
      return QTI_AMBIGUOUS;
   }

public:
   DLLLOCAL UserReferenceTypeInfo() : QoreTypeInfo(NT_REFERENCE) {
   }
};

// accepts QoreStringNode or BinaryNode and passes through
class DataTypeInfo : public QoreTypeInfo {
protected:
   DLLLOCAL virtual bool checkTypeInstantiationImpl(AbstractQoreNode *&n, ExceptionSink *xsink) const {
      qore_type_t t = n ? n->getType() : NT_NOTHING;
      return t == NT_STRING || t == NT_BINARY ? true : false;
   }
   DLLLOCAL virtual int testTypeCompatibilityImpl(const AbstractQoreNode *n) const {
      qore_type_t t = n ? n->getType() : NT_NOTHING;
      return t == NT_STRING || t == NT_BINARY ? QTI_AMBIGUOUS : QTI_NOT_EQUAL;
   }
   DLLLOCAL virtual int parseEqualImpl(const QoreTypeInfo *typeInfo) const {
      qore_type_t t = typeInfo ? typeInfo->getType() : NT_NOTHING;
      return t == NT_STRING || t == NT_BINARY ? QTI_AMBIGUOUS : QTI_NOT_EQUAL;
   }

public:
   DLLLOCAL DataTypeInfo() : QoreTypeInfo(NT_DATA) {
   }
};

// accepts int, float, string, date, or boolean and returns an int
class SoftBigIntTypeInfo : public QoreTypeInfo {
protected:
   DLLLOCAL virtual bool checkTypeInstantiationImpl(AbstractQoreNode *&n, ExceptionSink *xsink) const {
      if (!n) return false;
      qore_type_t t = n->getType();
      if (t == NT_INT) return true;
      if (t != NT_FLOAT && t != NT_STRING && t != NT_BOOLEAN && t != NT_DATE
          && t != NT_SOFTSTRING && t != NT_SOFTFLOAT && t != NT_SOFTBOOLEAN && t != NT_SOFTDATE)
         return false;
      int64 rv = n->getAsBigInt();
      n->deref(xsink);
      n = new QoreBigIntNode(rv);
      return true;
   }
   DLLLOCAL virtual int testTypeCompatibilityImpl(const AbstractQoreNode *n) const {
      qore_type_t t = n ? n->getType() : NT_NOTHING;
      if (t == NT_INT)
         return QTI_IDENT;
      return t == NT_FLOAT || t == NT_STRING || t == NT_BOOLEAN || t == NT_DATE
         || t == NT_SOFTSTRING || t == NT_SOFTFLOAT || t == NT_SOFTBOOLEAN || t == NT_SOFTDATE
         ? QTI_AMBIGUOUS : QTI_NOT_EQUAL;
   }
   DLLLOCAL virtual int parseEqualImpl(const QoreTypeInfo *typeInfo) const {
      qore_type_t t = typeInfo ? typeInfo->getType() : NT_NOTHING;
      if (t == NT_INT)
         return QTI_IDENT;
      return t == NT_FLOAT || t == NT_STRING || t == NT_BOOLEAN || t == NT_DATE
         || t == NT_SOFTSTRING || t == NT_SOFTFLOAT || t == NT_SOFTBOOLEAN || t == NT_SOFTDATE
         ? QTI_AMBIGUOUS : QTI_NOT_EQUAL;
   }

public:
   DLLLOCAL SoftBigIntTypeInfo() : QoreTypeInfo(NT_SOFTINT, NT_INT) {
   }
};

// accepts int, float, string, date, or boolean and returns a float
class SoftFloatTypeInfo : public QoreTypeInfo {
protected:
   DLLLOCAL virtual bool checkTypeInstantiationImpl(AbstractQoreNode *&n, ExceptionSink *xsink) const {
      if (!n) return false;
      qore_type_t t = n->getType();
      if (t == NT_FLOAT) return true;
      if (t != NT_INT && t != NT_STRING && t != NT_BOOLEAN && t != NT_DATE
          && t != NT_SOFTSTRING && t != NT_SOFTINT && t != NT_SOFTBOOLEAN && t != NT_SOFTDATE)
         return false;
      double rv = n->getAsFloat();
      n->deref(xsink);
      n = new QoreFloatNode(rv);
      return true;
   }
   DLLLOCAL virtual int testTypeCompatibilityImpl(const AbstractQoreNode *n) const {
      qore_type_t t = n ? n->getType() : NT_NOTHING;
      if (t == NT_FLOAT)
         return QTI_IDENT;
      return t == NT_INT || t == NT_STRING || t == NT_BOOLEAN || t == NT_DATE
         || t == NT_SOFTSTRING || t == NT_SOFTINT || t == NT_SOFTBOOLEAN || t == NT_SOFTDATE
         ? QTI_AMBIGUOUS : QTI_NOT_EQUAL;
   }
   DLLLOCAL virtual int parseEqualImpl(const QoreTypeInfo *typeInfo) const {
      qore_type_t t = typeInfo ? typeInfo->getType() : NT_NOTHING;
      if (t == NT_FLOAT)
         return QTI_IDENT;
      return t == NT_INT || t == NT_STRING || t == NT_BOOLEAN || t == NT_DATE
         || t == NT_SOFTSTRING || t == NT_SOFTINT || t == NT_SOFTBOOLEAN || t == NT_SOFTDATE
         ? QTI_AMBIGUOUS : QTI_NOT_EQUAL;
   }

public:
   DLLLOCAL SoftFloatTypeInfo() : QoreTypeInfo(NT_SOFTFLOAT, NT_FLOAT) {
   }
};

// accepts int, float, string, date, or boolean and returns a boolean
class SoftBoolTypeInfo : public QoreTypeInfo {
protected:
   DLLLOCAL virtual bool checkTypeInstantiationImpl(AbstractQoreNode *&n, ExceptionSink *xsink) const {
      if (!n) return false;
      qore_type_t t = n->getType();
      if (t == NT_BOOLEAN) return true;
      if (t != NT_INT && t != NT_STRING && t != NT_FLOAT && t != NT_DATE
          && t != NT_SOFTSTRING && t != NT_SOFTFLOAT && t != NT_SOFTINT && t != NT_SOFTDATE)
         return false;
      bool rv = n->getAsBool();
      n->deref(xsink);
      n = get_bool_node(rv);
      return true;
   }
   DLLLOCAL virtual int testTypeCompatibilityImpl(const AbstractQoreNode *n) const {
      qore_type_t t = n ? n->getType() : NT_NOTHING;
      if (t == NT_BOOLEAN)
         return QTI_IDENT;
      return t == NT_INT || t == NT_FLOAT || t == NT_STRING || t == NT_DATE
         || t == NT_SOFTSTRING || t == NT_SOFTFLOAT || t == NT_SOFTINT || t == NT_SOFTDATE
         ? QTI_AMBIGUOUS : QTI_NOT_EQUAL;
   }
   DLLLOCAL virtual int parseEqualImpl(const QoreTypeInfo *typeInfo) const {
      qore_type_t t = typeInfo ? typeInfo->getType() : NT_NOTHING;
      if (t == NT_BOOLEAN)
         return QTI_IDENT;
      return t == NT_INT || t == NT_FLOAT || t == NT_STRING || t == NT_DATE
         || t == NT_SOFTSTRING || t == NT_SOFTFLOAT || t == NT_SOFTINT || t == NT_SOFTDATE
         ? QTI_AMBIGUOUS : QTI_NOT_EQUAL;
   }

public:
   DLLLOCAL SoftBoolTypeInfo() : QoreTypeInfo(NT_SOFTBOOLEAN, NT_BOOLEAN) {
   }
};

// accepts int, float, string, date, or boolean and returns a string
class SoftStringTypeInfo : public QoreTypeInfo {
protected:
   DLLLOCAL virtual bool checkTypeInstantiationImpl(AbstractQoreNode *&n, ExceptionSink *xsink) const {
      if (!n) return false;
      qore_type_t t = n->getType();
      if (t == NT_STRING) return true;
      if (t != NT_INT && t != NT_BOOLEAN && t != NT_FLOAT && t != NT_DATE
          && t != NT_SOFTINT && t != NT_SOFTFLOAT && t != NT_SOFTBOOLEAN && t != NT_SOFTDATE)
         return false;
      QoreStringNodeValueHelper str(n);
      QoreStringNode *rv = str.getReferencedValue();
      n->deref(xsink);
      n = rv;
      return true;
   }
   DLLLOCAL virtual int testTypeCompatibilityImpl(const AbstractQoreNode *n) const {
      qore_type_t t = n ? n->getType() : NT_NOTHING;
      if (t == NT_STRING)
         return QTI_IDENT;
      return t == NT_INT || t == NT_FLOAT || t == NT_BOOLEAN || t == NT_DATE
         || t == NT_SOFTINT || t == NT_SOFTFLOAT || t == NT_SOFTBOOLEAN || t == NT_SOFTDATE
         ? QTI_AMBIGUOUS : QTI_NOT_EQUAL;
   }
   DLLLOCAL virtual int parseEqualImpl(const QoreTypeInfo *typeInfo) const {
      qore_type_t t = typeInfo ? typeInfo->getType() : NT_NOTHING;
      if (t == NT_STRING)
         return QTI_IDENT;
      return t == NT_INT || t == NT_FLOAT || t == NT_BOOLEAN || t == NT_DATE
         || t == NT_SOFTINT || t == NT_SOFTFLOAT || t == NT_SOFTBOOLEAN || t == NT_SOFTDATE
         ? QTI_AMBIGUOUS : QTI_NOT_EQUAL;
   }

public:
   DLLLOCAL SoftStringTypeInfo() : QoreTypeInfo(NT_SOFTSTRING, NT_STRING) {
   }
};

class ExternalTypeInfo : public QoreTypeInfo {
protected:
   const char *tname;
   const QoreTypeInfoHelper &helper;

   DLLLOCAL virtual const char *getNameImpl() const {
      assert(tname);
      return tname;
   }

   DLLLOCAL virtual void concatNameImpl(std::string &str) const {
      assert(tname);
      str.append(tname);
   }

   DLLLOCAL virtual bool checkTypeInstantiationImpl(AbstractQoreNode *&n, ExceptionSink *xsink) const {
      return helper.checkTypeInstantiationImpl(n, xsink);
   }

   DLLLOCAL virtual int testTypeCompatibilityImpl(const AbstractQoreNode *n) const {
      return helper.testTypeCompatibilityImpl(n);
   }

   DLLLOCAL virtual int parseEqualImpl(const QoreTypeInfo *typeInfo) const {
      return helper.parseEqualImpl(typeInfo);
   }

public:
   // used for base types
   DLLLOCAL ExternalTypeInfo(qore_type_t n_qt, const char *n_tname, const QoreTypeInfoHelper &n_helper) : QoreTypeInfo(n_qt), tname(n_tname), helper(n_helper) {
      // ensure this class is only used for external classes
      assert(qt >= QORE_NUM_TYPES); 

      //printd(0, "ExternalTypeInfo::ExternalTypeInfo() this=%p qt=%n qc=%p has_type=%d name=%s\n", this, qt, qc, has_type, tname);
   }

   // used for base types providing binary class equivalency with a builtin Qore base type
   DLLLOCAL ExternalTypeInfo(qore_type_t n_qt, qore_type_t n_compat_qt, const char *n_tname, const QoreTypeInfoHelper &n_helper) : QoreTypeInfo(n_qt, n_compat_qt), tname(n_tname), helper(n_helper) {
      // ensure this class is only used for external classes
      assert(qt >= QORE_NUM_TYPES); 

      //printd(0, "ExternalTypeInfo::ExternalTypeInfo() this=%p qt=%n qc=%p has_type=%d name=%s\n", this, qt, qc, has_type, tname);
   }

   // used for classes
   DLLLOCAL ExternalTypeInfo(const QoreClass *n_qc, const QoreTypeInfoHelper &n_helper) : QoreTypeInfo(n_qc), tname(n_qc->getName()), helper(n_helper) {
      assert(qc);
   }
   // used when assigning a base type after the fact
   DLLLOCAL ExternalTypeInfo(const char *n_tname, const QoreTypeInfoHelper &n_helper) : tname(n_tname), helper(n_helper) {
   }
   // used for assigning a class after the fact
   DLLLOCAL ExternalTypeInfo(const QoreTypeInfoHelper &n_helper) : tname(0), helper(n_helper) {
   }
   DLLLOCAL void assign(qore_type_t n_qt) {
      has_type = true;
      qt = n_qt;
   }
   DLLLOCAL void assignCompat(qore_type_t n_compat_qt) {
      compat_qt = n_compat_qt;
   }
   DLLLOCAL void assign(const QoreClass *n_qc) {
      has_type = true;
      qt = NT_OBJECT;
      qc = n_qc;
      assert(!tname);
      tname = qc->getName();
   }
};

// returns type info for base types
DLLLOCAL const QoreTypeInfo *getTypeInfoForType(qore_type_t t);
// returns type info information for parse types (values)
DLLLOCAL const QoreTypeInfo *getTypeInfoForValue(const AbstractQoreNode *n);

#endif // _QORE_QORETYPEINFO_H
