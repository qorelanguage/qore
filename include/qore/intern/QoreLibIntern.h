/*
  QoreLibIntern.h

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

#ifndef _QORE_QORELIBINTERN_H

#define _QORE_QORELIBINTERN_H

#include <qore/intern/config.h>

#define NT_NONE  -1
#define NT_ALL   -2

// the following functions are implemented in support.cc
DLLLOCAL void parse_error(int sline, int eline, const char *fmt, ...);
DLLLOCAL void parse_error(const char *fmt, ...);
DLLLOCAL void parseException(const char *err, const char *fmt, ...);
DLLLOCAL QoreString *findFileInPath(const char *file, const char *path);
DLLLOCAL QoreString *findFileInEnvPath(const char *file, const char *varname);

DLLLOCAL qore_type_t getBuiltinType(const char *str);
DLLLOCAL const char *getBuiltinTypeName(qore_type_t type);
DLLLOCAL bool private_class_access_ok(qore_classid_t id);

// tests to see if testClass is equal to or a public subclass of shouldBeClass, or
// if we are currently parsing inside the class, it can be private too
bool parseCheckCompatibleClass(const QoreClass *shouldBeClass, const QoreClass *testClass);

class QoreTypeInfo {
protected:
   DLLLOCAL int doTypeException(const AbstractQoreNode *n, ExceptionSink *xsink) const {
      // xsink may be null in case that parse exceptions have been disabled in the QoreProgram object
      // for example if there was a "requires" error
      if (!xsink)
	 return -1;

      QoreStringNode *desc = new QoreStringNode("expecting ");
      getThisType(*desc);
      desc->concat(", but got ");
      getNodeType(*desc, n);
      desc->concat(" instead");
      xsink->raiseException("RUNTIME-TYPE-ERROR", desc);
      return -1;
   }

   DLLLOCAL int doPrivateClassException(const AbstractQoreNode *n, ExceptionSink *xsink) const {
      // xsink may be null in case that parse exceptions have been disabled in the QoreProgram object
      // for example if there was a "requires" error
      if (!xsink)
	 return -1;

      QoreStringNode *desc = new QoreStringNode("expecting ");
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
      if (qc) {
	 str.sprintf("an object of class '%s'", qc->getName());
	 return;
      }
      str.sprintf("builtin type '%s'", getBuiltinTypeName(qt));
   }

/*
   // returns true if types are equal (or both null)
   DLLLOCAL bool equal(const QoreTypeInfo *typeInfo) const {
      if (!this || !has_type)
	 return !typeInfo || !typeInfo->has_type ? true : false;
      if (!typeInfo || !typeInfo->has_type)
	 return false;
      return qt == typeInfo->qt && qc == typeInfo->qc;      
   }
*/
   // prototype (expecting type) should be "this"
   DLLLOCAL bool parseEqual(const QoreTypeInfo &typeInfo) const {
      if (!has_type)
	 return !typeInfo.has_type ? true : false;
      if (!typeInfo.has_type)
	 return false;
      return qt == typeInfo.qt && (!qc || parseCheckCompatibleClass(qc, typeInfo.qc));
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
      if (!this || !has_type) return 0;
      if (qt == NT_NOTHING && is_nothing(n)) return 0;
      if (is_nothing(n))
	 return doTypeException(n, xsink);

      // from here on we know n != 0
      if (qt == NT_OBJECT) {
	 if (n->getType() != NT_OBJECT)
	    return doTypeException(n, xsink);

	 if (!qc)
	    return 0;

	 bool priv;
	 if (reinterpret_cast<const QoreObject *>(n)->getClass(qc->getID(), priv)) {
	    if (!priv)
	       return 0;

	    // check private access
	    if (private_class_access_ok(qc->getID()))
	       return 0;

	    return doPrivateClassException(n, xsink);
	 }

	 return doTypeException(n, xsink);
      }
      if (n->getType() != qt)
	 return doTypeException(n, xsink);

      return 0;
   }
};

#include <qore/intern/NamedScope.h>

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
   DLLLOCAL void resolve();
};

#ifndef HAVE_ATOLL
#ifdef HAVE_STRTOIMAX
#include <inttypes.h>
static inline long long atoll(const char *str) {
   return strtoimax(str, 0, 10);
}
#else
static inline long long atoll(const char *str) {
   long long i;
   sscanf(str, "%lld", &i);
   return i;
}
#endif
#endif

#if !defined(HAVE_STRTOLL) && defined(HAVE_STRTOIMAX)
#include <inttypes.h>
#define strtoll strtoimax
#endif

// use umem for memory allocation if available
#ifdef HAVE_UMEM_H
#include <umem.h>
#endif

#ifdef HAVE_OPENSSL_CONST
#define OPENSSL_CONST const
#else
#define OPENSSL_CONST
#endif

#include <set>

typedef std::set<const AbstractQoreNode *> const_node_set_t;

#include <list>

enum obe_type_e { OBE_Unconditional, OBE_Success, OBE_Error };

class StatementBlock;
typedef std::pair<enum obe_type_e, StatementBlock *> qore_conditional_block_exit_statement_t;

typedef std::list<qore_conditional_block_exit_statement_t> block_list_t;

// for maps of thread condition variables to TIDs
typedef std::map<QoreCondition *, int> cond_map_t;

#if defined(HAVE_PTHREAD_ATTR_GETSTACKSIZE) && defined(HAVE_CHECK_STACK_POS)
#define QORE_MANAGE_STACK
#endif

#include <qore/intern/ParseNode.h>
#include <qore/intern/CallReferenceCallNode.h>
#include <qore/intern/CallReferenceNode.h>
#include <qore/intern/Function.h>
#include <qore/intern/AbstractStatement.h>
#include <qore/intern/Variable.h>
#include <qore/intern/LocalVar.h>
#include <qore/intern/ScopedObjectCallNode.h>
#include <qore/intern/ConstantNode.h>
#include <qore/intern/ClassRefNode.h>
#include <qore/intern/Context.h>
#include <qore/intern/Operator.h>
#include <qore/intern/QoreTreeNode.h>
#include <qore/intern/BarewordNode.h>
#include <qore/intern/SelfVarrefNode.h>
#include <qore/intern/BackquoteNode.h>
#include <qore/intern/ContextrefNode.h>
#include <qore/intern/ContextRowNode.h>
#include <qore/intern/ComplexContextrefNode.h>
#include <qore/intern/FindNode.h>
#include <qore/intern/VRMutex.h>
#include <qore/intern/VLock.h>
#include <qore/intern/QoreException.h>
#include <qore/intern/qore_thread_intern.h>
#include <qore/intern/StatementBlock.h>
#include <qore/intern/VarRefNode.h>
#include <qore/intern/FunctionCallNode.h>
#include <qore/intern/RegexSubstNode.h>
#include <qore/intern/QoreRegexNode.h>
#include <qore/intern/RegexTransNode.h>
#include <qore/intern/ObjectMethodReferenceNode.h>
#include <qore/intern/QoreClosureParseNode.h>
#include <qore/intern/QoreClosureNode.h>
#include <qore/intern/QoreImplicitArgumentNode.h>

DLLLOCAL extern int qore_library_options;

#ifndef HAVE_GETHOSTBYNAME_R
DLLLOCAL extern QoreThreadLock lck_gethostbyname;
#endif
#ifndef HAVE_GETHOSTBYADDR_R
DLLLOCAL extern QoreThreadLock lck_gethostbyaddr;
#endif

DLLLOCAL extern qore_license_t qore_license;

#ifndef NET_BUFSIZE
#define NET_BUFSIZE      1024
#endif

#ifndef HOSTNAMEBUFSIZE
#define HOSTNAMEBUFSIZE 512
#endif

#ifndef HAVE_LOCALTIME_R
DLLLOCAL extern QoreThreadLock lck_localtime;
#endif

#ifndef HAVE_GMTIME_R
DLLLOCAL extern QoreThreadLock lck_gmtime;
#endif

DLLLOCAL extern char table64[64];

DLLLOCAL int get_nibble(char c, ExceptionSink *xsink);
DLLLOCAL BinaryNode *parseBase64(const char *buf, int len, ExceptionSink *xsink);
DLLLOCAL BinaryNode *parseHex(const char *buf, int len, ExceptionSink *xsink);
DLLLOCAL BinaryNode *parseHex(const char *buf, int len);
DLLLOCAL void print_node(FILE *fp, const AbstractQoreNode *node);
DLLLOCAL void delete_global_variables();
DLLLOCAL void initENV(char *env[]);
DLLLOCAL ResolvedCallReferenceNode *getCallReference(const QoreString *str, ExceptionSink *xsink);

DLLLOCAL AbstractQoreNode *copy_and_resolve_lvar_refs(const AbstractQoreNode *n, ExceptionSink *xsink);

DLLLOCAL void addProgramConstants(QoreNamespace *ns);

DLLLOCAL extern QoreTypeInfo bigIntTypeInfo, floatTypeInfo, boolTypeInfo, 
   stringTypeInfo, binaryTypeInfo, objectTypeInfo, hashTypeInfo, listTypeInfo;

#endif
