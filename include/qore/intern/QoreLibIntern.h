/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreLibIntern.h

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

#ifndef _QORE_QORELIBINTERN_H

#define _QORE_QORELIBINTERN_H

#include <qore/intern/config.h>

#include <stdarg.h>
#include <vector>

// here we define virtual types
#define NT_NONE         -1
#define NT_ALL          -2
#define NT_CODE         -3
#define NT_SOFTINT      -4
#define NT_SOFTFLOAT    -5
#define NT_SOFTBOOLEAN  -6
#define NT_SOFTSTRING   -7
#define NT_SOFTDATE     -8
#define NT_TIMEOUT      -9

#define NT_SOMETHING    -101 // i.e. "not NOTHING"
#define NT_DATA         -102 // either QoreStringNode or BinaryNode

// the following functions are implemented in support.cc
// FIXME: remove this function and use the one below
DLLLOCAL void parse_error(int sline, int eline, const char *fmt, ...);
DLLLOCAL void parse_error(const char *file, int sline, int eline, const char *fmt, ...);

DLLLOCAL void parse_error(const char *fmt, ...);
DLLLOCAL void parseException(const char *err, const char *fmt, ...);
DLLLOCAL void parseException(const char *err, QoreStringNode *desc);
DLLLOCAL QoreString *findFileInPath(const char *file, const char *path);
DLLLOCAL QoreString *findFileInEnvPath(const char *file, const char *varname);

DLLLOCAL const QoreTypeInfo *getBuiltinUserTypeInfo(const char *str);
DLLLOCAL const QoreTypeInfo *getBuiltinUserOrNothingTypeInfo(const char *str);
//DLLLOCAL qore_type_t getBuiltinType(const char *str);
DLLLOCAL const char *getBuiltinTypeName(qore_type_t type);

// tests to see if the private implementation of the given class ID can be accessed at run time
DLLLOCAL bool runtimeCheckPrivateClassAccess(const QoreClass *testClass);

// tests to see if the private implementation of the given class can be accessed at run time
DLLLOCAL bool parseCheckPrivateClassAccess(const QoreClass *testClass);

// tests to see if testClass is equal to or a public subclass of shouldBeClass, or
// if we are currently parsing inside the class, it can be private too
DLLLOCAL bool parseCheckCompatibleClass(const QoreClass *shouldBeClass, const QoreClass *testClass);

// processes parameter information
DLLLOCAL void qore_process_params(unsigned num_params, type_vec_t &typeList, arg_vec_t &defaultArgList, va_list args);

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
typedef std::set<LocalVar *> lvar_set_t;

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

enum qore_call_t {
   CT_UNUSED     = -1,
   CT_USER       =  0,
   CT_BUILTIN    =  1,
   CT_NEWTHREAD  =  2,
   CT_RETHROW    =  3
};

// Datasource Access Helper codes
#define DAH_NONE     0 // acquire lock temporarily
#define DAH_ACQUIRE  1 // acquire lock and hold
#define DAH_RELEASE  2 // release lock at end of action

#define DAH_TEXT(d) (d == DAH_RELEASE ? "release" : (d == DAH_ACQUIRE ? "acquire" : "none"))

// keep a list of objects to find recursive data structures
typedef std::vector<QoreObject *> obj_vec_t;

#include <qore/intern/NamedScope.h>
#include <qore/intern/QoreTypeInfo.h>
#include <qore/intern/ParseNode.h>
#include <qore/intern/qore_thread_intern.h>
#include <qore/intern/Function.h>
#include <qore/intern/CallReferenceCallNode.h>
#include <qore/intern/CallReferenceNode.h>
#include <qore/intern/BuiltinFunction.h>
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
#include <qore/intern/QoreImplicitElementNode.h>
#include <qore/intern/QoreOperatorNode.h>
#include <qore/intern/QoreTimeZoneManager.h>
#include <qore/intern/ContextStatement.h>
#include <qore/intern/SwitchStatement.h>

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
DLLLOCAL BinaryNode *parseHex(const char *buf, int len);
DLLLOCAL void print_node(FILE *fp, const AbstractQoreNode *node);
DLLLOCAL void delete_global_variables();
DLLLOCAL void init_lib_intern(char *env[]);
DLLLOCAL ResolvedCallReferenceNode *getCallReference(const QoreString *str, ExceptionSink *xsink);
DLLLOCAL QoreListNode *makeArgs(AbstractQoreNode *arg);

DLLLOCAL AbstractQoreNode *copy_and_resolve_lvar_refs(const AbstractQoreNode *n, ExceptionSink *xsink);

DLLLOCAL void addProgramConstants(QoreNamespace *ns);

DLLLOCAL void init_qore_types();
DLLLOCAL void delete_qore_types();

// only called in stage 1 parsing: true means node requires run-time evaluation
//DLLLOCAL bool needsEval(AbstractQoreNode *n);

DLLLOCAL const char *check_hash_key(const QoreHashNode *h, const char *key, const char *err, ExceptionSink *xsink);

// class for master namespace of all builtin classes, constants, etc
class StaticSystemNamespace : public RootQoreNamespace {
public:
   DLLLOCAL void init();
};

// master namespace of all builtin classes, constants, etc
DLLLOCAL extern StaticSystemNamespace staticSystemNamespace;

class QoreListNodeParseInitHelper : public ListIterator {
private:
   LocalVar *oflag;
   int pflag;
   int &lvids;

public:
   DLLLOCAL QoreListNodeParseInitHelper(QoreListNode *n_l, LocalVar *n_oflag, int n_pflag, int &n_lvids) : 
      ListIterator(n_l), oflag(n_oflag), pflag(n_pflag), lvids(n_lvids) {
   }
   
   void parseInit(const QoreTypeInfo *&typeInfo) {
      //printd(0, "QoreListNodeParseInitHelper::parseInit() this=%p %d/%d (l=%p)\n", this, index(), getList()->size(), getList());

      typeInfo = 0;
      AbstractQoreNode **n = getValuePtr();
      if (n && *n) {
	 (*n) = (*n)->parseInit(oflag, pflag, lvids, typeInfo);

	 //printd(0, "QoreListNodeParseInitHelper::parseInit() this=%p %d/%d (l=%p) prototype: %s (%s)\n", this, index(), getList()->size(), getList(), typeInfo && typeInfo->qt ? getBuiltinTypeName(typeInfo->qt) : "n/a", typeInfo && typeInfo->qc ? typeInfo->qc->getName() : "n/a");

	 if (!getList()->needs_eval() && (*n) && (*n)->needs_eval())
	    getList()->setNeedsEval();
      }
   }
};

class QorePossibleListNodeParseInitHelper {
private:
   LocalVar *oflag;
   int pflag;
   int &lvids;
   QoreListNode *l;
   bool finished;
   qore_size_t pos;
   const QoreTypeInfo *singleTypeInfo;

public:
   DLLLOCAL QorePossibleListNodeParseInitHelper(AbstractQoreNode **n, LocalVar *n_oflag, int n_pflag, int &n_lvids) : 
      oflag(n_oflag), 
      pflag(n_pflag & ~PF_REFERENCE_OK), 
      lvids(n_lvids), 
      l(n && *n && (*n)->getType() == NT_LIST ? reinterpret_cast<QoreListNode *>(*n) : 0),
      finished(!l),
      pos(-1),
      singleTypeInfo(0) {
      // if the expression is not a list, then initialize it now
      // and save the return type
      if (!l) {
         *n = (*n)->parseInit(oflag, pflag, lvids, singleTypeInfo);
         // set type info to 0 if the expression can return a list
         // FIXME: set list element type here when list elements can have types
         //printd(0, "singleTypeInfo=%s la=%d\n", singleTypeInfo->getName(), listTypeInfo->parseAccepts(singleTypeInfo));
         if (listTypeInfo->parseAccepts(singleTypeInfo))
            singleTypeInfo = 0;
      }
   }

   DLLLOCAL bool noArgument() const {
      return finished;
   }

   DLLLOCAL bool next() {
      ++pos;

      if (finished)
	 return false;
      
      if (pos == l->size()) {
         finished = true;
         return false;
      }
      return true;
   }
   
   DLLLOCAL AbstractQoreNode **getValuePtr() {
      if (finished)
	 return 0;

      return l->get_entry_ptr(pos);
   }

   DLLLOCAL void parseInit(const QoreTypeInfo *&typeInfo) {
      //printd(0, "QoreListNodeParseInitHelper::parseInit() this=%p %d/%d (l=%p)\n", this, l ? pos : 0, l ? l->size() : 1, l);

      typeInfo = 0;
      if (!l) {
         // FIXME: return list type info when list elements can be typed
         if (!pos) {
            if (singleTypeInfo)
               typeInfo = singleTypeInfo;
         }
         else {
            // no argument available
            if (singleTypeInfo)
               typeInfo = nothingTypeInfo;
         }
         return;
      }

      AbstractQoreNode **p = getValuePtr();
      if (!p || !(*p)) {
         // no argument available
         typeInfo = nothingTypeInfo;
      }
      else {
	 (*p) = (*p)->parseInit(oflag, pflag, lvids, typeInfo);

	 //printd(0, "QorePossibleListNodeParseInitHelper::parseInit() this=%p %d/%d (l=%p) type: %s (%s) *p=%p (%s)\n", this, pos, l ? l->size() : 1, l, typeInfo && typeInfo->qt ? getBuiltinTypeName(typeInfo->qt) : "n/a", typeInfo && typeInfo->qc ? typeInfo->qc->getName() : "n/a", p && *p ? *p : 0, p && *p ? (*p)->getTypeName() : "n/a");

	 if (l && !l->needs_eval() && (*p) && (*p)->needs_eval())
	    l->setNeedsEval();
      }
   }
};

DLLLOCAL void raiseNonExistentMethodCallWarning(const QoreClass *qc, const char *method);

DLLLOCAL void qoreCheckContainer(AbstractQoreNode *v);

/*
class abstract_assignment_helper {
public:
   DLLLOCAL virtual AbstractQoreNode *swapImpl(AbstractQoreNode *v, ExceptionSink *xsink) = 0;
   DLLLOCAL virtual AbstractQoreNode *getValueImpl() const = 0;
};
*/

class qore_hash_private;

class hash_assignment_priv {
public:
   qore_hash_private &h;
   HashMember *om;

   DLLLOCAL hash_assignment_priv(qore_hash_private &n_h, HashMember *n_om) : h(n_h), om(n_om) {
   }

   DLLLOCAL hash_assignment_priv(qore_hash_private &n_h, const char *key, bool must_already_exist = false);

   DLLLOCAL hash_assignment_priv(QoreHashNode &n_h, const char *key, bool must_already_exist = false);

   DLLLOCAL hash_assignment_priv(QoreHashNode &n_h, const std::string &key, bool must_already_exist = false);

   DLLLOCAL hash_assignment_priv(ExceptionSink *xsink, QoreHashNode &n_h, const QoreString &key, bool must_already_exist = false);

   DLLLOCAL hash_assignment_priv(ExceptionSink *xsink, QoreHashNode &n_h, const QoreString *key, bool must_already_exist = false);

   DLLLOCAL AbstractQoreNode *swapImpl(AbstractQoreNode *v, ExceptionSink *xsink);

   DLLLOCAL AbstractQoreNode *getValueImpl() const;

   DLLLOCAL AbstractQoreNode *operator*() const {
      return getValueImpl();
   }

   DLLLOCAL void assign(AbstractQoreNode *v, ExceptionSink *xsink) {
      AbstractQoreNode *old = swapImpl(v, xsink);
      if (*xsink)
         return;
      //qoreCheckContainer(v);
      if (old) {
         // "remove" logic here
         old->deref(xsink);
      }
   }

   DLLLOCAL AbstractQoreNode *swap(AbstractQoreNode *v, ExceptionSink *xsink) {      
      AbstractQoreNode *old = swapImpl(v, xsink);
      if (*xsink)
         return 0;
      if (old == v)
         return v;
      // "remove" and "add" logic here
      return old;
   }
};


#endif
