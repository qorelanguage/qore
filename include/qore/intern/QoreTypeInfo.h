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

#define NO_TYPE_INFO "<no type info>"

// used for return values when checking types with functions that return numeric codes
#define QTI_IDENT      -1  // the types are identical
#define QTI_AMBIGUOUS   1  // the types are ambiguously identical (NT_OBJECT and specific class)
#define QTI_NOT_EQUAL   0  // not equal
#define QTI_RECHECK     2  // possibly not equal

class AbstractQoreTypeInfo {
protected:
   DLLLOCAL virtual const char *getNameImpl() const = 0;
   DLLLOCAL virtual void concatNameImpl(std::string &str) const = 0;

   DLLLOCAL void concatClass(std::string &str, const char *cn) const {
      str.append("<class: ");
      str.append(cn);
      str.push_back('>');
   }

public:
   // FIXME: make protected
   qore_type_t qt : 11;
   bool has_type : 1;

   DLLLOCAL AbstractQoreTypeInfo(qore_type_t n_qt) : qt(n_qt), has_type(true) {
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
};

class QoreTypeInfo : public AbstractQoreTypeInfo {
protected:
   bool must_be_assigned;

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

   DLLLOCAL int checkTypeInstantiationIntern(bool obj, const char *param_name, const AbstractQoreNode *n, ExceptionSink *xsink) const;

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

   DLLLOCAL QoreTypeInfo() : must_be_assigned(false), qc(0) {}
   DLLLOCAL QoreTypeInfo(qore_type_t n_qt) : AbstractQoreTypeInfo(n_qt), must_be_assigned(false), qc(0) {}
   DLLLOCAL QoreTypeInfo(const QoreClass *n_qc) : AbstractQoreTypeInfo(NT_OBJECT), must_be_assigned(false), qc(n_qc) {}
   DLLLOCAL virtual ~QoreTypeInfo() {
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
   // returns true if the prototype does not expect any type or the types are compatible, 
   // false if otherwise
   DLLLOCAL bool parseEqual(const QoreTypeInfo *typeInfo) const {
      if (!this || !has_type || !typeInfo || !typeInfo->has_type)
	 return true;
      return qt == typeInfo->qt && (!qc || parseCheckCompatibleClass(qc, typeInfo->qc));
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

   // returns true for compatible, false for not, "this" is the parameter type, n is the argument
   DLLLOCAL bool testTypeCompatibility(const AbstractQoreNode *n) const;
   
   DLLLOCAL int checkType(const AbstractQoreNode *n, ExceptionSink *xsink) const {
      return checkTypeInstantiation(0, n, xsink);
   }

   DLLLOCAL int checkTypeInstantiation(const char *param_name, const AbstractQoreNode *n, ExceptionSink *xsink) const {
      return checkTypeInstantiationIntern(false, param_name, n, xsink);
   }

   DLLLOCAL int checkMemberTypeInstantiation(const char *param_name, const AbstractQoreNode *n, ExceptionSink *xsink) const {
      return checkTypeInstantiationIntern(true, param_name, n, xsink);
   }

   DLLLOCAL bool mustBeAssigned() const {
      return this ? must_be_assigned : false;
   }

   // used when parsing user code to find duplicate signatures after types are resolved
   DLLLOCAL int checkIdentical(const QoreTypeInfo *typeInfo) const {
      bool thisnt = (!this || !has_type);
      bool typent = (!typeInfo || !typeInfo->has_type);

      if (thisnt && typent)
	 return QTI_IDENT;

      if (thisnt || typent)
	 return QTI_NOT_EQUAL;

      // from this point on, we know that both have types
      if (qt != typeInfo->qt)
	 return QTI_NOT_EQUAL;

      // both types are identical
      if (qt != NT_OBJECT)
	 return QTI_IDENT;

      if (qc) {
	 if (!typeInfo->qc)
	    return QTI_AMBIGUOUS;
	 return qc == typeInfo->qc ? QTI_IDENT : QTI_NOT_EQUAL;
      }
      return typeInfo->qc ? QTI_AMBIGUOUS : QTI_IDENT;
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
      QoreTypeInfo::concatNameImpl(str);
   }

public:
   NamedScope *cscope; // namespace scope for class

   DLLLOCAL QoreParseTypeInfo() : QoreTypeInfo(), cscope(0) {}
   DLLLOCAL QoreParseTypeInfo(qore_type_t qt) : QoreTypeInfo(qt), cscope(0) {}
   DLLLOCAL QoreParseTypeInfo(char *n_cscope) : QoreTypeInfo(NT_OBJECT), cscope(new NamedScope(n_cscope)) {}
   DLLLOCAL QoreParseTypeInfo(const QoreClass *qc) : QoreTypeInfo(qc), cscope(0) {}
   DLLLOCAL QoreParseTypeInfo(const QoreTypeInfo *typeInfo) : QoreTypeInfo(), cscope(0) {
      if (typeInfo) {
	 qc = typeInfo->qc;
	 qt = typeInfo->qt;
	 has_type = typeInfo->has_type;
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
   DLLLOCAL int parseStageOneIdenticalWithParsed(const QoreTypeInfo *typeInfo) const {
      bool thisnt = (!this || !has_type);
      bool typent = (!typeInfo || !typeInfo->has_type);

      if (thisnt && typent)
	 return QTI_IDENT;

      if (thisnt || typent)
	 return QTI_NOT_EQUAL;

      // from this point on, we know that both have types
      if (qt != typeInfo->qt)
	 return QTI_NOT_EQUAL;

      // both types are identical
      if (qt != NT_OBJECT)
	 return QTI_IDENT;

      if (cscope) {
	 if (!typeInfo->qc)
	    return QTI_AMBIGUOUS;
	 // both have class info
	 return strcmp(cscope->getIdentifier(), qc->getName()) ? QTI_RECHECK : QTI_NOT_EQUAL;
      }
      return typeInfo->qc ? QTI_AMBIGUOUS : QTI_IDENT;
   }

   // used when parsing user code to find duplicate signatures
   DLLLOCAL int parseStageOneIdentical(const QoreParseTypeInfo *typeInfo) const {
      bool thisnt = (!this || !has_type);
      bool typent = (!typeInfo || !typeInfo->has_type);

      if (thisnt && typent)
	 return QTI_IDENT;

      if (thisnt || typent)
	 return QTI_NOT_EQUAL;

      // from this point on, we know that both have types
      if (qt != typeInfo->qt)
	 return QTI_NOT_EQUAL;

      // both types are identical
      if (qt != NT_OBJECT)
	 return QTI_IDENT;

      if (cscope) {
	 if (!typeInfo->cscope)
	    return QTI_AMBIGUOUS;
	 return strcmp(cscope->ostr, typeInfo->cscope->ostr) ? QTI_NOT_EQUAL : QTI_IDENT;
      }
      return typeInfo->cscope ? QTI_AMBIGUOUS : QTI_IDENT;
   }

   DLLLOCAL void resolve();
   DLLLOCAL bool needsResolving() const { 
      return cscope;
   }
   DLLLOCAL void setMustBeAssigned() { 
      if (this && qt >= NT_OBJECT)
	 must_be_assigned = true;
   }
#ifdef DEBUG
   DLLLOCAL const char *getCID() const { return this && cscope ? cscope->getIdentifier() : "n/a"; }
#endif
   DLLLOCAL QoreParseTypeInfo *copy() const {
      if (!this)
	 return 0;

      if (qc)
	 return new QoreParseTypeInfo(qc);
      if (!has_type)
	 return new QoreParseTypeInfo;
      return new QoreParseTypeInfo(qt);
   }
};

class ExternalTypeInfo : public QoreTypeInfo {
protected:
   const char *tname;

   DLLLOCAL virtual const char *getNameImpl() const {
      return tname;
   }

   DLLLOCAL virtual void concatNameImpl(std::string &str) const {
      str.append(tname);
   }

public:
   DLLLOCAL ExternalTypeInfo(qore_type_t n_qt, const char *n_tname) : QoreTypeInfo(n_qt), tname(n_tname) {
      // ensure this class is only used for external classes
      assert(qt >= QORE_NUM_TYPES); 
   }
   DLLLOCAL ExternalTypeInfo(const char *n_tname) : tname(n_tname) {
   }
   DLLLOCAL void assign(qore_type_t n_qt) {
      has_type = true;
      qt = n_qt;
   }
};

// returns the default value for any type >= 0 and < NT_OBJECT
DLLLOCAL AbstractQoreNode *getDefaultValueForBuiltinValueType(qore_type_t t);
// returns type info information for parse types (values)
DLLLOCAL const QoreTypeInfo *getTypeInfoForValue(const AbstractQoreNode *n);

#endif // _QORE_QORETYPEINFO_H
