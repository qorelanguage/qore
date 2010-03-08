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

// adds external types to global type map
DLLLOCAL void add_to_type_map(qore_type_t t, const QoreTypeInfo *typeInfo);
DLLLOCAL bool builtinTypeHasDefaultValue(qore_type_t t);
// returns the default value for any type >= 0 and < NT_OBJECT
DLLLOCAL AbstractQoreNode *getDefaultValueForBuiltinValueType(qore_type_t t);

class AbstractQoreTypeInfo {
protected:
   qore_type_t qt : 11;
   bool has_type : 1;

   DLLLOCAL virtual const char *getNameImpl() const = 0;
   DLLLOCAL virtual void concatNameImpl(std::string &str) const = 0;

   DLLLOCAL void concatClass(std::string &str, const char *cn) const {
      str.append("<class: ");
      str.append(cn);
      str.push_back('>');
   }

public:
   DLLLOCAL AbstractQoreTypeInfo(qore_type_t n_qt) : qt(n_qt), has_type(n_qt != NT_ALL ? true : false) {
   }
   DLLLOCAL AbstractQoreTypeInfo() : qt(NT_ALL), has_type(false) {
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
};

class QoreTypeInfo : public AbstractQoreTypeInfo {
protected:
   DLLLOCAL int doTypeException(const char *param_name, const AbstractQoreNode *n, ExceptionSink *xsink) const {
      // xsink may be null in case parse exceptions have been disabled in the QoreProgram object
      // for example if there was a "requires" error
      if (!xsink)
	 return -1;

      QoreStringNode *desc = new QoreStringNode;
      if (param_name)
	 desc->sprintf("parameter '$%s' is ", param_name);
      desc->concat("expecting ");
      getThisType(*desc);
      desc->concat(", but got ");
      getNodeType(*desc, n);
      desc->concat(" instead");
      xsink->raiseException("RUNTIME-TYPE-ERROR", desc);
      return -1;
   }

   DLLLOCAL int doPrivateClassException(const char *param_name, const AbstractQoreNode *n, ExceptionSink *xsink) const {
      // xsink may be null in case that parse exceptions have been disabled in the QoreProgram object
      // for example if there was a "requires" error
      if (!xsink)
	 return -1;

      QoreStringNode *desc = new QoreStringNode;
      if (param_name)
	 desc->sprintf("parameter '$%s' is ", param_name);
      desc->concat("expecting ");
      getThisType(*desc);
      desc->concat(", but got an object where this class is privately inherited instead");
      xsink->raiseException("RUNTIME-TYPE-ERROR", desc);
      return -1;
   }

   DLLLOCAL int doObjectTypeException(const char *param_name, const AbstractQoreNode *n, ExceptionSink *xsink) const {
      assert(xsink);
      QoreStringNode *desc = new QoreStringNode;
      desc->sprintf("member '$.%s' is ", param_name);
      desc->concat("expecting ");
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
      desc->sprintf("member '$.%s' is ", param_name);
      desc->concat("expecting ");
      getThisType(*desc);
      desc->concat(", but got an object where this class is privately inherited instead");
      xsink->raiseException("RUNTIME-TYPE-ERROR", desc);
      return -1;
   }

   // returns 0 = OK, -1 = error
   DLLLOCAL int checkTypeInstantiationDefault(AbstractQoreNode *n, bool &priv_error) const;

   DLLLOCAL AbstractQoreNode *checkTypeInstantiationIntern(bool obj, const char *param_name, AbstractQoreNode *n, ExceptionSink *xsink) const {
      bool priv_error = false;
      if (!checkTypeInstantiationDefault(n, priv_error))
	 return n;

      //printd(0, "QoreTypeInfo::checkTypeInstantiationIntern() this=%p %s n=%p %s\n", this, getName(), n, n ? n->getTypeName() : "NOTHING");
      if (!checkTypeInstantiationImpl(n, xsink)) {
	 if (*xsink)
	    return n;
	 if (priv_error) {
	    if (obj) doObjectPrivateClassException(param_name, n, xsink);
	    else doPrivateClassException(param_name, n, xsink);
	 }
	 else {
	    if (obj) doObjectTypeException(param_name, n, xsink);
	    else doTypeException(param_name, n, xsink);
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
   
   DLLLOCAL AbstractQoreNode *checkType(AbstractQoreNode *n, ExceptionSink *xsink) const {
      return checkTypeInstantiation(0, n, xsink);
   }

   DLLLOCAL AbstractQoreNode *checkTypeInstantiation(const char *param_name, AbstractQoreNode *n, ExceptionSink *xsink) const {
      if (!this) return n;
      return checkTypeInstantiationIntern(false, param_name, n, xsink);
   }

   DLLLOCAL AbstractQoreNode *checkMemberTypeInstantiation(const char *param_name, AbstractQoreNode *n, ExceptionSink *xsink) const {
      if (!this) return n;
      return checkTypeInstantiationIntern(true, param_name, n, xsink);
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

class FloatTypeInfo : public QoreTypeInfo {
protected:
   DLLLOCAL virtual bool checkTypeInstantiationImpl(AbstractQoreNode *&n, ExceptionSink *xsink) const {
      //printd(0, "FloatTypeInfo::checkTypeInstantiationImpl() n=%p %s\n", n, n->getTypeName());
      if (!n || n->getType() != NT_INT)
	 return false;

      QoreFloatNode *f = new QoreFloatNode(reinterpret_cast<QoreBigIntNode *>(n)->val);
      n->deref(xsink);
      n = f;
      return true;
   }
   DLLLOCAL virtual int testTypeCompatibilityImpl(const AbstractQoreNode *n) const {
      return n && n->getType() == NT_INT ? QTI_AMBIGUOUS : QTI_NOT_EQUAL;
   }
   DLLLOCAL virtual int parseEqualImpl(const QoreTypeInfo *typeInfo) const {
      return typeInfo && typeInfo->getType() == NT_INT ? QTI_AMBIGUOUS : QTI_NOT_EQUAL;
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
   DLLLOCAL virtual int parseEqualImpl(const QoreTypeInfo *typeInfo) const {
      if (!typeInfo->hasType())
         return QTI_NOT_EQUAL;

      return typeInfo->parseEqualImpl(this);
   }

public:
   DLLLOCAL BigIntTypeInfo() : QoreTypeInfo(NT_INT) {
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
