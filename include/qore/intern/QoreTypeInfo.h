/*
  QoreTypeInfo.h

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

#ifndef _QORE_QORETYPEINFO_H

#define _QORE_QORETYPEINFO_H

class QoreTypeInfo {
protected:
   DLLLOCAL int doTypeException(const char *param_name, const AbstractQoreNode *n, ExceptionSink *xsink) const {
      // xsink may be null in case that parse exceptions have been disabled in the QoreProgram object
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

public:
   const QoreClass *qc;
   qore_type_t qt : 11;
   bool has_type : 1;

   DLLLOCAL QoreTypeInfo() : qc(0), qt(NT_ALL), has_type(false) {}
   DLLLOCAL QoreTypeInfo(qore_type_t n_qt) : qc(0), qt(n_qt), has_type(true) {}
   DLLLOCAL QoreTypeInfo(const QoreClass *n_qc) : qc(n_qc), qt(NT_OBJECT), has_type(true) {}

   DLLLOCAL void getNodeType(QoreStringNode &str, const AbstractQoreNode *n) const {
      if (is_nothing(n)) {
	 str.concat("no value");
	 return;
      }
      if (n->getType() != NT_OBJECT) {
	 str.sprintf("builtin type '%s'", n->getTypeName());
	 return;
      }
      str.sprintf("object of class '%s'", reinterpret_cast<const QoreObject *>(n)->getClassName());
   }

   DLLLOCAL void getThisType(QoreStringNode &str) const {
      if (!this) {
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
   
   DLLLOCAL int checkType(const AbstractQoreNode *n, ExceptionSink *xsink) const {
      return checkTypeInstantiation(0, n, xsink);
   }

   DLLLOCAL int checkTypeInstantiation(const char *param_name, const AbstractQoreNode *n, ExceptionSink *xsink) const {
      if (!this || !has_type) return 0;
      if (qt == NT_NOTHING && is_nothing(n)) return 0;
      if (is_nothing(n))
	 return doTypeException(param_name, n, xsink);

      // from here on we know n != 0
      if (qt == NT_OBJECT) {
	 if (n->getType() != NT_OBJECT)
	    return doTypeException(param_name, n, xsink);

	 if (!qc)
	    return 0;

	 bool priv;
	 if (reinterpret_cast<const QoreObject *>(n)->getClass(qc->getID(), priv)) {
	    if (!priv)
	       return 0;

	    // check private access
	    if (private_class_access_ok(qc->getID()))
	       return 0;

	    return doPrivateClassException(param_name, n, xsink);
	 }

	 return doTypeException(param_name, n, xsink);
      }
      if (n->getType() != qt)
	 return doTypeException(param_name, n, xsink);

      return 0;
   }

#ifdef DEBUG
   DLLLOCAL const char *getTypeName() const { return this && qt >= 0 ? getBuiltinTypeName(qt) : "n/a"; }
#endif
};

class QoreParseTypeInfo : public QoreTypeInfo {
protected:
   NamedScope *cscope; // namespace scope for class

public:
   DLLLOCAL QoreParseTypeInfo() : QoreTypeInfo(), cscope(0) {}
   DLLLOCAL QoreParseTypeInfo(qore_type_t qt) : QoreTypeInfo(qt), cscope(0) {}
   DLLLOCAL QoreParseTypeInfo(char *n_cscope) : QoreTypeInfo(NT_OBJECT), cscope(new NamedScope(n_cscope)) {}
   DLLLOCAL QoreParseTypeInfo(const QoreClass *qc) : QoreTypeInfo(qc) {}
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
   DLLLOCAL ~QoreParseTypeInfo() {
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
   DLLLOCAL void resolve();
   DLLLOCAL bool needsResolving() const { 
      return cscope;
   }
#ifdef DEBUG
   DLLLOCAL const char *getCID() const { return this && cscope ? cscope->getIdentifier() : "n/a"; }
#endif
};

DLLLOCAL extern QoreTypeInfo bigIntTypeInfo, floatTypeInfo, boolTypeInfo, 
   stringTypeInfo, binaryTypeInfo, dateTypeInfo, objectTypeInfo, hashTypeInfo, 
   listTypeInfo, nothingTypeInfo, nullTypeInfo, runTimeClosureTypeInfo,
   callReferenceTypeInfo;

#endif // _QORE_QORETYPEINFO_H
