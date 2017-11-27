/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreLibIntern.h

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

#ifndef _QORE_QORELIBINTERN_H

#define _QORE_QORELIBINTERN_H

#include "qore/intern/config.h"

#include <atomic>

#include <stdarg.h>
#include <sys/types.h>

#ifdef HAVE_SYS_STATVFS_H
#define Q_HAVE_STATVFS
#include <sys/statvfs.h>
#endif
#include <sys/stat.h>
#include <unistd.h>
#ifdef HAVE_EXECINFO_H
#include <execinfo.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef HAVE_WINSOCK2_H
#include <winsock2.h>
#endif
#ifdef HAVE_WS2TCPIP_H
#include <ws2tcpip.h>
#endif
#ifdef HAVE_SYS_UN_H
#include <sys/un.h>
#endif
#ifdef HAVE_NETINET_TCP_H
#include <netinet/tcp.h>
#endif

#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif
#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif

#ifndef HAVE_STRCASESTR
extern char* strcasestr(const char* s1, const char* s2);
#endif

// make sure that intmax support is available from mpfr
#define MPFR_USE_INTMAX_T 1

// for arbitrary-precision numeric support
#include <mpfr.h>

// printf format for size_t or qore_size_t integers
#if TARGET_BITS == 64
#define QSD QLLD
#else
#define QSD "%d"
#endif

#include <set>
#include <list>
#include <map>
#include <vector>

// here we define virtual types
#define NT_NONE             -1
#define NT_ALL              -2
#define NT_CODE             -3
#define NT_SOFTINT          -4
#define NT_SOFTFLOAT        -5
#define NT_SOFTNUMBER       -6
#define NT_SOFTBOOLEAN      -7
#define NT_SOFTSTRING       -8
#define NT_SOFTDATE         -9
#define NT_SOFTLIST         -10
#define NT_TIMEOUT          -11
#define NT_INTORFLOAT       -12
#define NT_INTFLOATORNUMBER -13
#define NT_FLOATORNUMBER    -14

#define NT_SOMETHING    -101 // i.e. "not NOTHING"
#define NT_DATA         -102 // either QoreStringNode or BinaryNode

typedef std::set<QoreObject*> obj_set_t;

// returns true if the node needs to be scanned for recursive references or not
DLLLOCAL bool needs_scan(const AbstractQoreNode* n);
// increments or decrements the object count depending on the sign of the argument (cannot be 0)
DLLLOCAL void inc_container_obj(const AbstractQoreNode* n, int dt);

DLLLOCAL AbstractQoreNode* missing_openssl_feature(const char* f, ExceptionSink* xsink);

struct ParseWarnOptions {
   int64 parse_options;
   int warn_mask;

   DLLLOCAL ParseWarnOptions() : parse_options(0), warn_mask(0) {
   }

   DLLLOCAL ParseWarnOptions(int64 n_parse_options, int n_warn_mask = 0) : parse_options(n_parse_options), warn_mask(n_warn_mask) {
   }

   DLLLOCAL void operator=(const ParseWarnOptions& pwo) {
      parse_options = pwo.parse_options;
      warn_mask = pwo.warn_mask;
   }

   DLLLOCAL bool operator==(const ParseWarnOptions& pwo) const {
      return parse_options == pwo.parse_options && warn_mask == pwo.warn_mask;
   }
};

enum prog_loc_e { RunTimeLocation = 0, ParseLocation = 1 };

struct QoreProgramLineLocation {
   int start_line, end_line;

   // if sline is 0 and eline is > 0 then set sline to 1
   DLLLOCAL QoreProgramLineLocation(int sline, int eline) : start_line(sline ? sline : (eline ? 1 : 0)), end_line(eline) {
   }

   DLLLOCAL QoreProgramLineLocation() : start_line(-1), end_line(-1) {
   }

   DLLLOCAL QoreProgramLineLocation(const QoreProgramLineLocation& old) : start_line(old.start_line), end_line(old.end_line) {
   }
};

struct QoreProgramLocation : public QoreProgramLineLocation {
protected:
   DLLLOCAL explicit QoreProgramLocation(const char* f, int sline = 0, int eline = 0) : QoreProgramLineLocation(sline, eline), file(f), source(nullptr), offset(0) {
   }

public:
   const char* file;
   const char* source;
   int offset;

   // "blank" constructor
   DLLLOCAL QoreProgramLocation() : file(nullptr), source(nullptr), offset(0) {
   }

   // sets file position info from thread-local parse information
   DLLLOCAL QoreProgramLocation(int sline, int eline);

   // sets from current parse or runtime location in thread-local data
   DLLLOCAL QoreProgramLocation(prog_loc_e loc);

   DLLLOCAL QoreProgramLocation(const QoreProgramLocation& old) : QoreProgramLineLocation(old), file(old.file), source(old.source), offset(old.offset) {
   }

   DLLLOCAL void clear() {
      start_line = end_line = -1;
      file = nullptr;
      source = nullptr;
      offset = 0;
   }

   DLLLOCAL void parseSet() const;

   DLLLOCAL void toString(QoreString& str) const;
};

struct QoreCommandLineLocation : public QoreProgramLocation {
   DLLLOCAL QoreCommandLineLocation() : QoreProgramLocation("<command-line>", 1, 1) {
   }
};

// parse location for objects parsed on the command-line
DLLLOCAL extern QoreCommandLineLocation qoreCommandLineLocation;

// the following functions are implemented in support.cc
DLLLOCAL void parse_error(const QoreProgramLocation& loc, const char* fmt, ...);
DLLLOCAL void parseException(const QoreProgramLocation& loc, const char* err, const char* fmt, ...);
DLLLOCAL void parseException(const QoreProgramLocation& loc, const char* err, QoreStringNode* desc);

DLLLOCAL QoreString* findFileInPath(const char* file, const char* path);
DLLLOCAL QoreString* findFileInEnvPath(const char* file, const char* varname);
DLLLOCAL int qore_find_file_in_path(QoreString& str, const char* file, const char* path);

DLLLOCAL const QoreTypeInfo* getBuiltinUserTypeInfo(const char* str);
DLLLOCAL const QoreTypeInfo* getBuiltinUserOrNothingTypeInfo(const char* str);
//DLLLOCAL qore_type_t getBuiltinType(const char* str);
DLLLOCAL const char* getBuiltinTypeName(qore_type_t type);

// processes parameter information
DLLLOCAL void qore_process_params(unsigned num_params, type_vec_t& typeList, arg_vec_t& defaultArgList, va_list args);
DLLLOCAL void qore_process_params(unsigned num_params, type_vec_t& typeList, arg_vec_t& defaultArgList, name_vec_t& nameList, va_list args);

// call to get a node with reference count 1 (copy on write)
void ensure_unique(AbstractQoreNode** v, ExceptionSink* xsink);

#ifndef HAVE_ATOLL
#ifdef HAVE_STRTOIMAX
static inline long long atoll(const char* str) {
   return strtoimax(str, 0, 10);
}
#else
static inline long long atoll(const char* str) {
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

#ifdef HAVE_X509_GET0_SIGNATURE_CONST
#define OPENSSL_X509_GET0_SIGNATURE_CONST const
#else
#define OPENSSL_X509_GET0_SIGNATURE_CONST
#endif

typedef std::set<const AbstractQoreNode*> const_node_set_t;
typedef std::set<LocalVar*> lvar_set_t;

class LVarSet : public lvar_set_t {
protected:
   // true if at least one variable in the set could contain an object or a closure (also through a container)
   bool needs_scan;

public:
   // creates the object
   DLLLOCAL LVarSet() : needs_scan(false) {
   }

   // adds a local variable to the set
   DLLLOCAL void add(LocalVar* var);

   // returns true if at least one variable in the set could contain an object or a closure (also through a container)
   DLLLOCAL bool needsScan() const {
      return needs_scan;
   }
};

enum obe_type_e { OBE_Unconditional, OBE_Success, OBE_Error };

class StatementBlock;
typedef std::pair<enum obe_type_e, StatementBlock*> qore_conditional_block_exit_statement_t;

typedef std::list<qore_conditional_block_exit_statement_t> block_list_t;

// for maps of thread condition variables to TIDs
typedef std::map<QoreCondition*, int> cond_map_t;

#if defined(HAVE_CHECK_STACK_POS)
#define QORE_MANAGE_STACK
#endif

// Datasource Access Helper codes
#define DAH_NOCHANGE  0 // acquire lock temporarily
#define DAH_ACQUIRE   1 // acquire lock and hold
#define DAH_RELEASE   2 // release lock at end of action
#define DAH_NOCONN    3 // acquire lock temporarily and do not make a connection

#define DAH_TEXT(d) (d == DAH_RELEASE ? "RELEASE" : (d == DAH_ACQUIRE ? "ACQUIRE" : "NOCHANGE"))

DLLLOCAL int check_lvalue(AbstractQoreNode* n, bool assign = true);
DLLLOCAL int check_lvalue_int(const QoreProgramLocation& loc, const QoreTypeInfo*& typeInfo, const char* name);
DLLLOCAL int check_lvalue_float(const QoreProgramLocation& loc, const QoreTypeInfo*& typeInfo, const char* name);
DLLLOCAL int check_lvalue_int_float_number(const QoreProgramLocation& loc, const QoreTypeInfo*& typeInfo, const char* name);
DLLLOCAL int check_lvalue_number(const QoreProgramLocation& loc, const QoreTypeInfo*& typeInfo, const char* name);

DLLLOCAL extern QoreClass* QC_PSEUDOVALUE;
DLLLOCAL extern QoreClass* QC_PSEUDONOTHING;

DLLLOCAL bool node_has_effect(const AbstractQoreNode* n);

DLLLOCAL QoreString* q_fix_decimal(QoreString* str, size_t offset = 0);
DLLLOCAL QoreStringNode* q_fix_decimal(QoreStringNode* str, size_t offset = 0);

#ifdef _Q_WINDOWS
// simulated block size for statvfs() on Windows
#define Q_SVF_BSIZE 4096
#define Q_HAVE_STATVFS
struct statvfs {
   unsigned long   f_bsize;        /* File system block size */
   unsigned long   f_frsize;       /* Fundamental file system block size */
   unsigned int    f_blocks;       /* Blocks on FS in units of f_frsize */
   unsigned int    f_bfree;        /* Free blocks */
   unsigned int    f_bavail;       /* Blocks available to non-root */
   unsigned int    f_files;        /* Total inodes */
   unsigned int    f_ffree;        /* Free inodes */
   unsigned int    f_favail;       /* Free inodes for non-root */
   unsigned long   f_fsid;         /* Filesystem ID */
   unsigned long   f_flag;         /* Bit mask of values */
   unsigned long   f_namemax;      /* Max file name length */

   DLLLOCAL void set(int64 avail, int64 total, int64 free) {
      f_frsize = f_bsize = Q_SVF_BSIZE;
      f_blocks = total / Q_SVF_BSIZE;
      f_bfree = free / Q_SVF_BSIZE;
      f_bavail = avail / Q_SVF_BSIZE;
      // simulate inodes
      f_files = f_blocks / 8;
      f_ffree = f_bfree / 8;
      f_favail = f_bavail / 8;
      f_fsid = 0;
      f_flag = 0;
      f_namemax = 256;
   }
};
DLLLOCAL int statvfs(const char* path, struct statvfs* buf);
DLLLOCAL int q_fstatvfs(const char* filepath, struct statvfs* buf);
#endif

class QoreParseListNode;

#include "qore/intern/NamedScope.h"
#include "qore/intern/QoreTypeInfo.h"
#include "qore/intern/ParseNode.h"
#include "qore/intern/QoreThreadList.h"
#include "qore/intern/lvalue_ref.h"
#include "qore/intern/qore_thread_intern.h"
#include "qore/intern/Function.h"
#include "qore/intern/CallReferenceCallNode.h"
#include "qore/intern/CallReferenceNode.h"
#include "qore/intern/BuiltinFunction.h"
#include "qore/intern/AbstractStatement.h"
#include "qore/intern/Variable.h"
#include "qore/intern/LocalVar.h"
#include "qore/intern/ScopedObjectCallNode.h"
#include "qore/intern/NewComplexTypeNode.h"
#include "qore/intern/ScopedRefNode.h"
#include "qore/intern/ClassRefNode.h"
#include "qore/intern/Context.h"
#include "qore/intern/BarewordNode.h"
#include "qore/intern/SelfVarrefNode.h"
#include "qore/intern/StaticClassVarRefNode.h"
#include "qore/intern/BackquoteNode.h"
#include "qore/intern/ContextrefNode.h"
#include "qore/intern/ContextRowNode.h"
#include "qore/intern/ComplexContextrefNode.h"
#include "qore/intern/FindNode.h"
#include "qore/intern/VRMutex.h"
#include "qore/intern/VLock.h"
#include "qore/intern/QoreException.h"
#include "qore/intern/StatementBlock.h"
#include "qore/intern/VarRefNode.h"
#include "qore/intern/FunctionCallNode.h"
#include "qore/intern/QoreRegexSubst.h"
#include "qore/intern/QoreRegex.h"
#include "qore/intern/QoreTransliteration.h"
#include "qore/intern/ObjectMethodReferenceNode.h"
#include "qore/intern/QoreClosureParseNode.h"
#include "qore/intern/QoreClosureNode.h"
#include "qore/intern/QoreImplicitArgumentNode.h"
#include "qore/intern/QoreImplicitElementNode.h"
#include "qore/intern/QoreOperatorNode.h"
#include "qore/intern/QoreTimeZoneManager.h"
#include "qore/intern/ContextStatement.h"
#include "qore/intern/SwitchStatement.h"
#include "qore/intern/QorePseudoMethods.h"
#include "qore/intern/ParseReferenceNode.h"
#include "qore/intern/WeakReferenceNode.h"

DLLLOCAL extern std::atomic<bool> qore_shutdown;

DLLLOCAL extern int qore_library_options;

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

DLLLOCAL int get_nibble(char c, ExceptionSink* xsink);
DLLLOCAL BinaryNode* parseHex(const QoreProgramLocation& loc, const char* buf, int len);
DLLLOCAL void print_node(FILE* fp, const QoreValue qv);
DLLLOCAL void delete_global_variables();
DLLLOCAL void init_lib_intern(char* env[]);
DLLLOCAL QoreParseListNode* make_args(const QoreProgramLocation& loc, AbstractQoreNode* arg);

DLLLOCAL AbstractQoreNode* copy_and_resolve_lvar_refs(const AbstractQoreNode* n, ExceptionSink* xsink);
DLLLOCAL QoreValue copy_value_and_resolve_lvar_refs(const QoreValue n, ExceptionSink* xsink);

DLLLOCAL void init_qore_types();
DLLLOCAL void delete_qore_types();

DLLLOCAL QoreListNode* stat_to_list(const struct stat& sbuf);
DLLLOCAL QoreHashNode* stat_to_hash(const struct stat& sbuf, const TypedHashDecl* hd = hashdeclStatInfo);
DLLLOCAL QoreHashNode* statvfs_to_hash(const struct statvfs& statvfs);

// only called in stage 1 parsing: true means node requires run-time evaluation
//DLLLOCAL bool needsEval(AbstractQoreNode* n);

DLLLOCAL const char* check_hash_key(const QoreHashNode* h, const char* key, const char* err, ExceptionSink* xsink);

// class for master namespace of all builtin classes, constants, etc
class StaticSystemNamespace : public RootQoreNamespace {
public:
   DLLLOCAL StaticSystemNamespace();

   DLLLOCAL ~StaticSystemNamespace();
};

// master namespace of all builtin classes, constants, etc
DLLLOCAL extern StaticSystemNamespace* staticSystemNamespace;

class QoreListNodeParseInitHelper : public ListIterator {
private:
   LocalVar* oflag;
   int pflag;
   int& lvids;

public:
   DLLLOCAL QoreListNodeParseInitHelper(QoreListNode* n_l, LocalVar* n_oflag, int n_pflag, int& n_lvids) :
      ListIterator(n_l), oflag(n_oflag), pflag(n_pflag), lvids(n_lvids) {
   }

   DLLLOCAL AbstractQoreNode* parseInit(const QoreTypeInfo*& typeInfo) {
      //printd(0, "QoreListNodeParseInitHelper::parseInit() this=%p %d/%d (l=%p)\n", this, index(), getList()->size(), getList());

      typeInfo = nullptr;
      AbstractQoreNode** n = getValuePtr();
      if (n && *n) {
         (*n) = (*n)->parseInit(oflag, pflag, lvids, typeInfo);

         //printd(0, "QoreListNodeParseInitHelper::parseInit() this=%p %d/%d (l=%p) prototype: %s (%s)\n", this, index(), getList()->size(), getList(), typeInfo && typeInfo->qt ? getBuiltinTypeName(typeInfo->qt) : "n/a", typeInfo && typeInfo->qc ? typeInfo->qc->getName() : "n/a");

         return *n;
      }

      return nullptr;
   }
};

class QoreParseListNodeParseInitHelper {
private:
   QoreParseListNode* l;
   int pos = -1;
   LocalVar* oflag;
   int pflag;
   int& lvids;

public:
   DLLLOCAL QoreParseListNodeParseInitHelper(QoreParseListNode* n_l, LocalVar* n_oflag, int n_pflag, int& n_lvids) :
      l(n_l), oflag(n_oflag), pflag(n_pflag), lvids(n_lvids) {
   }

   DLLLOCAL AbstractQoreNode* parseInit(const QoreTypeInfo*& typeInfo) {
      //printd(0, "QoreListNodeParseInitHelper::parseInit() this=%p %d/%d (l=%p)\n", this, index(), getList()->size(), getList());

      typeInfo = nullptr;
      AbstractQoreNode** n = l->getPtr(pos);
      if (n && *n) {
         (*n) = (*n)->parseInit(oflag, pflag, lvids, typeInfo);

         //printd(0, "QoreListNodeParseInitHelper::parseInit() this=%p %d/%d (l=%p) prototype: %s (%s)\n", this, index(), getList()->size(), getList(), typeInfo && typeInfo->qt ? getBuiltinTypeName(typeInfo->qt) : "n/a", typeInfo && typeInfo->qc ? typeInfo->qc->getName() : "n/a");

         return *n;
      }

      return nullptr;
   }

   DLLLOCAL bool next() {
      ++pos;
      if (pos == (int)l->size()) {
         pos = -1;
         return false;
      }
      return true;
   }

   DLLLOCAL int index() {
      return pos;
   }
};

class QorePossibleListNodeParseInitHelper {
private:
   LocalVar* oflag;
   int pflag;
   int& lvids;
   QoreListNode* l;
   bool finished;
   qore_size_t pos = -1;
   const QoreTypeInfo* singleTypeInfo = nullptr;

public:
   DLLLOCAL QorePossibleListNodeParseInitHelper(AbstractQoreNode** n, LocalVar* n_oflag, int n_pflag, int& n_lvids) :
      oflag(n_oflag),
      pflag(n_pflag),
      lvids(n_lvids),
      l(n && *n && (*n)->getType() == NT_LIST ? reinterpret_cast<QoreListNode*>(*n) : nullptr),
      finished(!l) {
      // if the expression is not a list, then initialize it now
      // and save the return type
      if (!l) {
         *n = (*n)->parseInit(oflag, pflag, lvids, singleTypeInfo);
         // set type info to 0 if the expression can return a list
         // FIXME: set list element type here when list elements can have types
         //printd(0, "singleTypeInfo=%s la=%d\n", QoreTypeInfo::getName(singleTypeInfo), QoreTypeInfo::parseAccepts(listTypeInfo, singleTypeInfo));
         if (QoreTypeInfo::parseAccepts(listTypeInfo, singleTypeInfo))
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

   DLLLOCAL AbstractQoreNode** getValuePtr() {
      if (finished)
         return 0;

      return l->get_entry_ptr(pos);
   }

   DLLLOCAL void parseInit(const QoreTypeInfo*& typeInfo) {
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

      AbstractQoreNode** p = getValuePtr();
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

DLLLOCAL void raise_nonexistent_method_call_warning(const QoreProgramLocation& loc, const QoreClass* qc, const char* method);

/*
class abstract_assignment_helper {
public:
   DLLLOCAL virtual AbstractQoreNode* swapImpl(AbstractQoreNode* v, ExceptionSink* xsink) = 0;
   DLLLOCAL virtual AbstractQoreNode* getValueImpl() const = 0;
};
*/

class qore_hash_private;
class qore_object_private;
class HashMember;

class hash_assignment_priv {
public:
   qore_hash_private& h;
   HashMember* om;
   qore_object_private* o = nullptr;

   DLLLOCAL hash_assignment_priv(qore_hash_private& n_h, HashMember* n_om) : h(n_h), om(n_om) {
   }

   DLLLOCAL hash_assignment_priv(qore_hash_private& n_h, const char* key, bool must_already_exist = false, qore_object_private* obj = nullptr);

   DLLLOCAL hash_assignment_priv(QoreHashNode& n_h, const char* key, bool must_already_exist = false);

   DLLLOCAL hash_assignment_priv(QoreHashNode& n_h, const std::string &key, bool must_already_exist = false);

   DLLLOCAL hash_assignment_priv(ExceptionSink* xsink, QoreHashNode& n_h, const QoreString& key, bool must_already_exist = false);

   DLLLOCAL hash_assignment_priv(ExceptionSink* xsink, QoreHashNode& n_h, const QoreString* key, bool must_already_exist = false);

   DLLLOCAL void reassign(const char* key, bool must_already_exist = false);

   DLLLOCAL AbstractQoreNode* swapImpl(AbstractQoreNode* v);

   DLLLOCAL AbstractQoreNode* getValueImpl() const;

   DLLLOCAL AbstractQoreNode* operator*() const {
      return getValueImpl();
   }

   DLLLOCAL void assign(AbstractQoreNode* v, ExceptionSink* xsink);

   DLLLOCAL AbstractQoreNode* swap(AbstractQoreNode* v) {
      return swapImpl(v);
   }

   DLLLOCAL static hash_assignment_priv* get(HashAssignmentHelper& h) {
      return h.priv;
   }
};

DLLLOCAL void qore_machine_backtrace();

#ifndef QORE_THREAD_STACK_BLOCK
#define QORE_THREAD_STACK_BLOCK 1024
#endif

template <typename T, int S1 = QORE_THREAD_STACK_BLOCK>
class ThreadBlock {
private:
   DLLLOCAL ThreadBlock(const ThreadBlock&);

public:
   T var[S1];
   int pos;
   ThreadBlock<T, S1>* prev, * next;

   DLLLOCAL ThreadBlock(ThreadBlock* n_prev = 0) : pos(0), prev(n_prev), next(0) { }
   DLLLOCAL ~ThreadBlock() { }
   DLLLOCAL T& get(int p) {
      return var[p];
   }

   DLLLOCAL bool frameBoundary(int p);
};

template <typename T, int S1 = QORE_THREAD_STACK_BLOCK>
class ThreadLocalDataIterator {
   typedef ThreadLocalDataIterator<T, S1> self_t;

public:
   typedef ThreadBlock<T, S1> Block;

protected:
   Block* orig, * curr;
   int pos;

public:
   DLLLOCAL ThreadLocalDataIterator(Block* n_orig) : orig(n_orig && n_orig->pos ? n_orig : 0), curr(0), pos(0) {
   }

   DLLLOCAL ThreadLocalDataIterator() : orig(0), curr(0), pos(0) {
   }

   DLLLOCAL bool next() {
      if (!orig)
         return false;

      do {
         if (!curr) {
            curr = orig;
            pos = orig->pos - 1;
         }
         else {
            --pos;
            if (pos < 0) {
               if (!curr->prev) {
                  curr = 0;
                  pos = 0;
                  return false;
               }
               curr = curr->prev;
               pos = curr->pos - 1;
            }
         }
      } while (!curr->frameBoundary(pos));

      return true;
   }

   DLLLOCAL T& get() {
      assert(curr);
      return curr->get(pos);
   }
};

template <typename T, int S1 = QORE_THREAD_STACK_BLOCK>
class ThreadLocalData {
public:
   typedef ThreadBlock<T, S1> Block;
   typedef ThreadLocalDataIterator<T, S1> iterator;

   Block* curr;

   DLLLOCAL ThreadLocalData() {
      curr = new Block;
      //printf("this=%p: first curr=%p\n", this, curr);
   }

   DLLLOCAL ~ThreadLocalData() {
#ifdef DEBUG
      //if (curr->pos)
         //printf("~ThreadLocalData::~~ThreadLocalData() this=%p: del curr=%p pos=%d next=%p prev=%p\n", this, curr, curr->pos, curr->next, curr->prev);
#endif
      assert(!curr->prev);
      assert(!curr->pos);
      if (curr->next)
         delete curr->next;
      delete curr;
   }
#ifdef DEBUG
   DLLLOCAL bool empty() const {
      return (!curr->pos && !curr->prev);
   }
#endif

   DLLLOCAL int getFrameCount() const {
     return frame_count;
   }

protected:
   int frame_count = -1;

private:
   DLLLOCAL ThreadLocalData(const ThreadLocalData&);
};

DLLLOCAL int q_get_af(int type);
DLLLOCAL int q_get_sock_type(int t);

class OptHashRefHelper {
   const ReferenceNode* ref;
   ExceptionSink* xsink;
   ReferenceHolder<QoreHashNode> info;
public:
   DLLLOCAL OptHashRefHelper(QoreListNode* args, unsigned i, ExceptionSink* n_xsink) : ref(test_reference_param(args, i)), xsink(n_xsink), info(ref ? new QoreHashNode : 0, xsink) {
   }
   DLLLOCAL OptHashRefHelper(const ReferenceNode* n_ref, ExceptionSink* n_xsink) : ref(n_ref), xsink(n_xsink), info(ref ? new QoreHashNode : 0, xsink) {
   }
   DLLLOCAL ~OptHashRefHelper() {
      if (!ref)
         return;

      ExceptionSink xs;
      QoreTypeSafeReferenceHelper rh(ref, &xs);
      if (xs)
         xsink->assimilate(xs);
      if (!rh)
         return;

      rh.assign(info.release(), &xs);
      if (xs)
         xsink->assimilate(xs);
   }
   DLLLOCAL QoreHashNode* operator*() {
      return *info;
   }
};

class ParseLocationHelper : private QoreProgramLocation {
public:
   DLLLOCAL ParseLocationHelper(const QoreProgramLocation& loc) : QoreProgramLocation(ParseLocation) {
      loc.parseSet();
   }
   DLLLOCAL ~ParseLocationHelper() {
      parseSet();
   }
};

// pushes a marker on the local variable parse stack so that searches can skip to global thread-local variables when the search hits the marker
class VariableBlockHelper {
public:
   DLLLOCAL VariableBlockHelper();
   DLLLOCAL ~VariableBlockHelper();
};

class ParseOptionMaps {
protected:
   DLLLOCAL void doMap(int64 code, const char* desc);

public:
   typedef std::map<int64, const char*> pomap_t;
   typedef std::map<const char*, int64, ltstr> pormap_t;

   pomap_t pomap;
   pormap_t pormap;

   DLLLOCAL ParseOptionMaps();

   DLLLOCAL QoreHashNode* getCodeToStringMap() const;
   DLLLOCAL QoreHashNode* getStringToCodeMap() const;
};

DLLLOCAL extern ParseOptionMaps pomaps;

DLLLOCAL extern QoreString YamlNullString;

DLLLOCAL extern bool q_disable_gc;

DLLLOCAL AbstractQoreNode* qore_parse_get_define_value(const QoreProgramLocation& loc, const char* str, QoreString& arg, bool& ok);

#ifndef HAVE_INET_NTOP
DLLLOCAL const char* inet_ntop(int af, const void* src, char* dst, size_t size);
#endif
#ifndef HAVE_INET_PTON
DLLLOCAL int inet_pton(int af, const char* src, void* dst);
#endif

DLLLOCAL AbstractQoreNode* missing_function_error(const char* func, ExceptionSink* xsink);
DLLLOCAL AbstractQoreNode* missing_function_error(const char* func, const char* opt, ExceptionSink* xsink);
DLLLOCAL AbstractQoreNode* missing_method_error(const char* meth, const char* opt, ExceptionSink* xsink);

// checks for illegal $self assignments in an object context
DLLLOCAL void check_self_assignment(const QoreProgramLocation& loc, AbstractQoreNode* n, LocalVar* selfid);

DLLLOCAL void ignore_return_value(AbstractQoreNode* n);

DLLLOCAL void qore_string_init();

DLLLOCAL QoreListNode* split_intern(const char* pattern, qore_size_t pl, const char* str, qore_size_t sl, const QoreEncoding* enc, bool with_separator = false);
DLLLOCAL QoreStringNode* join_intern(const QoreStringNode* p0, const QoreListNode* l, int offset, ExceptionSink* xsink);
DLLLOCAL QoreListNode* split_with_quote(const QoreString* sep, const QoreString* str, const QoreString* quote, bool trim_unquoted, ExceptionSink* xsink);
DLLLOCAL bool inlist_intern(const QoreValue arg, const QoreListNode* l, ExceptionSink* xsink);
DLLLOCAL QoreStringNode* format_float_intern(const QoreString& fmt, double num, ExceptionSink* xsink);
DLLLOCAL QoreStringNode* format_float_intern(int prec, const QoreString& dsep, const QoreString& tsep, double num, ExceptionSink* xsink);
DLLLOCAL DateTimeNode* make_date_with_mask(const AbstractQoreZoneInfo* tz, const QoreString& dtstr, const QoreString& mask, ExceptionSink* xsink);
DLLLOCAL QoreHashNode* date_info(const DateTime& d);
DLLLOCAL void init_charmaps();
DLLLOCAL int do_unaccent(QoreString& str, const QoreString& src, ExceptionSink* xsink);
DLLLOCAL int do_unaccent(QoreString& str, const QoreString& src, ExceptionSink* xsink);
DLLLOCAL int do_tolower(QoreString& str, const QoreString& src, ExceptionSink* xsink);
DLLLOCAL int do_toupper(QoreString& str, const QoreString& src, ExceptionSink* xsink);

DLLLOCAL int64 q_clock_getmillis();
DLLLOCAL int64 q_clock_getmicros();
DLLLOCAL int64 q_clock_getnanos();

DLLLOCAL void qore_init_random_salt();
DLLLOCAL int qore_get_ptr_hash(QoreString& str, const void* ptr);

// find the position of the first path separator in the string, or 0
DLLLOCAL const char* q_find_first_path_sep(const char* path);
// find the position of the last path separator in the string, or 0
DLLLOCAL const char* q_find_last_path_sep(const char* path);

// reutrns the given file's mode or 0 if the stat() call fails
DLLLOCAL int q_get_mode(const QoreString& path);
//! returns the byte length of the next UTF-8 character or 0 for an encoding error or a negative number if the string is too short to represent the character
/** FIXME: change return type to qore_offset_t
 */
DLLLOCAL qore_offset_t q_UTF8_get_char_len(const char* p, qore_size_t valid_len);

//! returns the byte length of the next UTF-16 (big-endian encoded) character or 0 for an encoding error or a negative number if the string is too short to represent the character
/** FIXME: change return type to qore_offset_t
 */
DLLLOCAL qore_offset_t q_UTF16BE_get_char_len(const char* p, qore_size_t valid_len);
DLLLOCAL qore_offset_t q_UTF16LE_get_char_len(const char* p, qore_size_t len);

DLLLOCAL int64 get_ms_zero(const QoreValue& v);

DLLLOCAL AbstractQoreNode* copy_strip_complex_types(const AbstractQoreNode* n);

// for IPv4/v6 only
DLLLOCAL void* qore_get_in_addr(struct sockaddr *sa);
// for IPv4/v6 only
DLLLOCAL size_t qore_get_in_len(struct sockaddr *sa);

#endif
