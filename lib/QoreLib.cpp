/* -*- indent-tabs-mode: nil -*- */
/*
  QoreLib.cpp

  Qore Programming Language

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
#include <qore/intern/git-revision.h>
#include <qore/intern/qore_number_private.h>
#include <qore/intern/QoreSignal.h>
#include <qore/intern/QoreObjectIntern.h>
#include <qore/intern/qore_qd_private.h>
#include <qore/intern/ql_crypto.h>
#include <qore/intern/qore_program_private.h>

#include <string.h>
#ifdef HAVE_PWD_H
#include <pwd.h>
#include <grp.h>
#include <dirent.h>
#endif
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <ctype.h>

FeatureList qoreFeatureList;

#define cpp_str(s) #s
#define cpp_xstr(s) cpp_str(s)

// global library variables
const char* qore_version_string      = VERSION;
int qore_version_major               = VERSION_MAJOR;
int qore_version_minor               = VERSION_MINOR;
int qore_version_sub                 = VERSION_SUB;
int qore_target_bits                 = TARGET_BITS;

// obsolete: no more svn
int qore_build_number                = 0;

const char* qore_git_hash            = BUILD;
const char* qore_target_os           = TARGET_OS;
const char* qore_target_arch         = TARGET_ARCH;
const char* qore_module_dir          = MODULE_DIR;
const char* qore_user_module_dir     = USER_MODULE_DIR;
const char* qore_module_ver_dir      = MODULE_VER_DIR;
const char* qore_user_module_ver_dir = USER_MODULE_VER_DIR;
const char* qore_cplusplus_compiler  = QORE_LIB_CXX;
const char* qore_cflags              = QORE_LIB_CFLAGS;
const char* qore_ldflags             = QORE_LIB_LDFLAGS;
const char* qore_build_host          = QORE_BUILD_HOST;

int qore_min_mod_api_major = QORE_MODULE_COMPAT_API_MAJOR;
int qore_min_mod_api_minor = QORE_MODULE_COMPAT_API_MINOR;

DLLLOCAL QoreListNode* ARGV = 0;
DLLLOCAL QoreListNode* QORE_ARGV = 0;

QoreString random_salt;

DLLLOCAL bool q_disable_gc = false;

#ifndef HAVE_LOCALTIME_R
DLLLOCAL QoreThreadLock lck_localtime;
#endif

#ifndef HAVE_GMTIME_R
DLLLOCAL QoreThreadLock lck_gmtime;
#endif

#if defined(HAVE_GETPWUID_R) || defined(HAVE_GETPWNAM_R)
static long pwsize = 0;
#else
// for the getpwuid() and getpwnam() functions
static QoreThreadLock lck_passwd;
#endif

#if defined(HAVE_GETGRGID_R) || defined(HAVE_GETGRNAM_R)
static long grsize = 0;
#else
// for the getgrgid() and getgrnam() functions
static QoreThreadLock lck_group;
#endif

// time zone information source
QoreTimeZoneManager QTZM;

// parse location for objects parsed on the command-line
QoreCommandLineLocation qoreCommandLineLocation;

// for base64 encoding
char table64[64] = {
   'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
   'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
   'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
   'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
   'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
   'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
   'w', 'x', 'y', 'z', '0', '1', '2', '3',
   '4', '5', '6', '7', '8', '9', '+', '/' };

const qore_option_s qore_option_list_l[] = {
   { QORE_OPT_ATOMIC_OPERATIONS,
     "HAVE_ATOMIC_OPERATIONS",
     QO_OPTION,
#ifdef HAVE_ATOMIC_MACROS
     true
#else
     false
#endif
   },
   { QORE_OPT_STACK_GUARD,
     "HAVE_STACK_GUARD",
     QO_OPTION,
#ifdef QORE_MANAGE_STACK
     true
#else
     false
#endif
   },
   { QORE_OPT_SIGNAL_HANDLING,
     "HAVE_SIGNAL_HANDLING",
     QO_OPTION,
#ifdef HAVE_SIGNAL_HANDLING
     true
#else
     false
#endif
   },
   { QORE_OPT_LIBRARY_DEBUGGING,
     "HAVE_LIBRARY_DEBUGGING",
     QO_OPTION,
#ifdef DEBUG
     true
#else
     false
#endif
   },
   { QORE_OPT_RUNTIME_STACK_TRACE,
     "HAVE_RUNTIME_THREAD_STACK_TRACE",
     QO_OPTION,
#ifdef QORE_RUNTIME_THREAD_STACK_TRACE
     true
#else
     false
#endif
   },
   { QORE_OPT_TERMIOS,
     "HAVE_TERMIOS",
     QO_OPTION,
#ifdef HAVE_TERMIOS_H
     true
#else
     false
#endif
   },
   { QORE_OPT_UNIX_USERMGT,
     "HAVE_UNIX_USERMGT",
     QO_OPTION,
#ifdef HAVE_GETUID
     true
#else
     false
#endif
   },
   { QORE_OPT_UNIX_FILEMGT,
     "HAVE_UNIX_FILEMGT",
     QO_OPTION,
#ifdef HAVE_CHOWN
     true
#else
     false
#endif
   },
   { QORE_OPT_FILE_LOCKING,
     "HAVE_FILE_LOCKING",
     QO_OPTION,
#ifdef HAVE_STRUCT_FLOCK
     true
#else
     false
#endif
   },
   { QORE_OPT_DETERMINISTIC_GC,
     "HAVE_DETEMINISTIC_GC",
     QO_OPTION,
     true
   },
   { QORE_OPT_SHA,
     "HAVE_SHA",
     QO_ALGORITHM,
#ifdef HAVE_OPENSSL_SHA
     true
#else
     false
#endif
   },
   { QORE_OPT_SHA224,
     "HAVE_SSH224",
     QO_ALGORITHM,
#if !defined(OPENSSL_NO_SHA256) && defined(HAVE_OPENSSL_SHA512)
     true
#else
     false
#endif
   },
   { QORE_OPT_SHA256,
     "HAVE_SSH256",
     QO_ALGORITHM,
#if !defined(OPENSSL_NO_SHA256) && defined(HAVE_OPENSSL_SHA512)
     true
#else
     false
#endif
   },
   { QORE_OPT_SHA384,
     "HAVE_SSH384",
     QO_ALGORITHM,
#if !defined(OPENSSL_NO_SHA512) && defined(HAVE_OPENSSL_SHA512)
     true
#else
     false
#endif
   },
   { QORE_OPT_SHA512,
     "HAVE_SSH512",
     QO_ALGORITHM,
#if !defined(OPENSSL_NO_SHA512) && defined(HAVE_OPENSSL_SHA512)
     true
#else
     false
#endif
   },
   { QORE_OPT_MDC2,
     "HAVE_MDC2",
     QO_ALGORITHM,
#ifndef OPENSSL_NO_MDC2
     true
#else
     false
#endif
   },
   { QORE_OPT_RC5,
     "HAVE_RC5",
     QO_ALGORITHM,
#ifndef OPENSSL_NO_RC5
     true
#else
     false
#endif
   },
   { QORE_OPT_MD2,
     "HAVE_MD2",
     QO_ALGORITHM,
#ifndef OPENSSL_NO_MD2
     true
#else
     false
#endif
   },
   { QORE_OPT_FUNC_ROUND,
     "HAVE_ROUND",
     QO_FUNCTION,
#ifdef HAVE_ROUND
     true
#else
     false
#endif
   },
   // HAVE_TIMEGM is always true now, we don't use the system function anymore anyway
   { QORE_OPT_FUNC_TIMEGM,
     "HAVE_TIMEGM",
     QO_FUNCTION,
     true
   },
   { QORE_OPT_FUNC_SETEUID,
     "HAVE_SETEUID",
     QO_FUNCTION,
#ifdef HAVE_SETEUID
     true
#else
     false
#endif
   },
   { QORE_OPT_FUNC_SETEGID,
     "HAVE_SETEGID",
     QO_FUNCTION,
#ifdef HAVE_SETEGID
     true
#else
     false
#endif
   },
   { QORE_OPT_FUNC_SYSTEM,
     "HAVE_SYSTEM",
     QO_FUNCTION,
#ifdef HAVE_SYSTEM
     true
#else
     false
#endif
   },
   { QORE_OPT_FUNC_KILL,
     "HAVE_KILL",
     QO_FUNCTION,
#ifdef HAVE_KILL
     true
#else
     false
#endif
   },
   { QORE_OPT_FUNC_FORK,
     "HAVE_FORK",
     QO_FUNCTION,
#ifdef HAVE_FORK
     true
#else
     false
#endif
   },
   { QORE_OPT_FUNC_GETPPID,
     "HAVE_GETPPID",
     QO_FUNCTION,
#ifdef HAVE_GETPPID
     true
#else
     false
#endif
   },
   { QORE_OPT_FUNC_STATVFS,
     "HAVE_STATVFS",
     QO_FUNCTION,
#ifdef Q_HAVE_STATVFS
     true
#else
     false
#endif
   },
   { QORE_OPT_FUNC_SETSID,
     "HAVE_SETSID",
     QO_FUNCTION,
#ifdef HAVE_SETSID
     true
#else
     false
#endif
   },
   { QORE_OPT_FUNC_IS_EXECUTABLE,
     "HAVE_IS_EXECUTABLE",
     QO_FUNCTION,
#ifdef HAVE_PWD_H
     true
#else
     false
#endif
   },
};

const qore_option_s* qore_option_list = qore_option_list_l;

#define QORE_OPTION_LIST_SIZE (sizeof(qore_option_list_l) / sizeof(qore_option_s))

size_t qore_option_list_size = QORE_OPTION_LIST_SIZE;

bool q_get_option_value(const char* opt) {
   for (unsigned i = 0; i < QORE_OPTION_LIST_SIZE; ++i) {
      if (!strcasecmp(opt, qore_option_list_l[i].option))
         return qore_option_list_l[i].value;
   }
   return false;
}

bool q_get_option_constant_value(const char* opt) {
   for (unsigned i = 0; i < QORE_OPTION_LIST_SIZE; ++i) {
      if (!strcasecmp(opt, qore_option_list_l[i].constant))
         return qore_option_list_l[i].value;
   }
   return false;
}

static inline int get_number(char** param) {
   int num = 0;
   while (isdigit(**param)) {
      num = num*10 + (**param - '0');
      ++(*param);
   }
   //printd(0, "get_number(%x: %s) num: %d\n", *param, *param, num);
   return num;
}

// print options
#define P_JUSTIFY_LEFT      1
#define P_INCLUDE_PLUS      2
#define P_SPACE_FILL        4
#define P_ZERO_FILL         8

bool qore_has_debug() {
#ifdef DEBUG
   return true;
#else
   return false;
#endif
}

QoreAbstractIteratorBase::QoreAbstractIteratorBase() : tid(gettid()) {
}

QoreAbstractIteratorBase::~QoreAbstractIteratorBase() {
}

int QoreAbstractIteratorBase::check(ExceptionSink* xsink) const {
   if (tid != gettid()) {
      xsink->raiseException("ITERATOR-THREAD-ERROR", "this %s object was created in TID %d; it is an error to access it from any other thread (accessed from TID %d)", getName(), tid, gettid());
      return -1;
   }
   return 0;
}

QoreIteratorBase::QoreIteratorBase() {
}

QoreIteratorBase::~QoreIteratorBase() {
}

FeatureList::FeatureList() {
   // register default features
   push_back("sql");
   push_back("threads");
   push_back("DGC");
#ifdef DEBUG
   push_back("debug");
#endif
}

FeatureList::~FeatureList() {
}

// if type = 0 then field widths are soft limits, otherwise they are hard
static int process_opt(QoreString *cstr, char* param, QoreValue qv, int type, int *taken, ExceptionSink* xsink) {
   char* str = param;
   int opts = 0;
   int width = -1;
   int decimals = -1;
   int length;
   char fmt[20], *f;
   QoreString tbuf(cstr->getEncoding());

   printd(5, "process_opt(): param: %s type: %d qv->getType(): %s refs: %d\n",
	  param, type, qv.getTypeName(), qv.hasNode() ? qv.getInternalNode()->reference_count() : -1);
   qore_type_t t = qv.getType();
#ifdef DEBUG
   if (t == NT_STRING) {
      const QoreStringNode* nstr = qv.get<const QoreStringNode>();
      printd(5, "process_opt() %p (%d) \"%s\"\n", nstr->getBuffer(), nstr->strlen(), nstr->getBuffer());
   }
#endif

   // if it's just '%%' then output a single '%' and do not process arguments
   if (param[1] == '%') {
      cstr->concat('%');
      *taken = 0;
      return 1;
   }

  loop:
   switch (*(++param)) {
      case '-': opts |= P_JUSTIFY_LEFT; goto loop;
      case '+': opts |= P_INCLUDE_PLUS; goto loop;
      case ' ': opts |= P_SPACE_FILL; opts &= ~P_ZERO_FILL; goto loop;
      case '0': opts |= P_ZERO_FILL; opts &= ~P_SPACE_FILL; goto loop;
   }
   if (isdigit(*param))
      width = get_number(&param);
   if ((*param) == '.') {
      param++;
      decimals = get_number(&param);
   }
   if (decimals < 0)
      decimals = -1;

   char p = *param;
   switch (*param) {
      case 's': {
	 QoreStringValueHelper astr(qv);
	 length = astr->strlen();
	 if ((width != -1) && (length > width) && !type)
	    width = length;
	 if ((width != -1) && (length > width)) {
	    tbuf.concat(*astr, (size_t)width, xsink); // string encodings are converted here if necessary
	 }
	 else {
	    if ((width != -1) && (opts & P_JUSTIFY_LEFT)) {
	       tbuf.concat(*astr, xsink);
	       while (width > length) {
		  tbuf.concat(' ');
		  width--;
	       }
	    }
	    else {
	       while (width > length) {
		  tbuf.concat(' ');
		  width--;
	       }
	       tbuf.concat(*astr, xsink);
	    }
	 }
	 break;
      }
      case 'p':
	 p = 'x';
      case 'd':
      case 'o':
      case 'x':
      case 'X': {
	 // recreate the sprintf format argument
	 f = fmt;
	 *(f++) = '%';
	 if (opts & P_JUSTIFY_LEFT)
	    *(f++) = '-';
	 if (opts & P_INCLUDE_PLUS)
	    *(f++) = '+';
	 if (width != -1) {
	    if (opts & P_SPACE_FILL)
	       *(f++) = ' ';
	    else if (opts & P_ZERO_FILL)
	       *(f++) = '0';
	    f += sprintf(f, "%d", width);
	 }
#ifdef _Q_WINDOWS
	 *(f++) = 'I';
	 *(f++) = '6';
	 *(f++) = '4';
#else
	 *(f++) = 'l';
	 *(f++) = 'l';
#endif
	 *(f++) = p; // 'd', etc;
	 *f = '\0';
         int64 val = qv.getAsBigInt();
	 tbuf.sprintf(fmt, val);
	 if (type && (width != -1))
	    tbuf.terminate(width);
	 break;
      }
      case 'A':
      case 'a':
      case 'G':
      case 'g':
      case 'F':
      case 'f':
      case 'E':
      case 'e': {
	 // recreate the sprintf format argument
	 f = fmt;
	 *(f++) = '%';
	 if (opts & P_JUSTIFY_LEFT)
	    *(f++) = '-';
	 if (opts & P_INCLUDE_PLUS)
	    *(f++) = '+';
	 if (width != -1) {
	    if (opts & P_SPACE_FILL)
	       *(f++) = ' ';
	    else if (opts & P_ZERO_FILL)
	       *(f++) = '0';
	    f += sprintf(f, "%d", width);
	 }
	 if (decimals != -1) {
	    *(f++) = '.';
	    f += sprintf(f, "%d", decimals);
	 }
	 if (t == NT_NUMBER) {
	    *(f++) = QORE_MPFR_SPRINTF_ARG;
            *(f++) = *param; // a|A|e|E|f|F|g|G
            *f = '\0';
            qore_number_private::sprintf(*qv.get<const QoreNumberNode>(), tbuf, fmt);
	 }
	 else {
	    *(f++) = *param; // a|A|e|E|f|F|g|G
	    *f = '\0';
	    double val = qv.getAsFloat();
	    tbuf.sprintf(fmt, val);
	    //printd(5, "fmt: '%s' val: %f\n", fmt, val);
	 }
	 if (type && (width != -1))
	    tbuf.terminate(width);
	 break;
      }
      case 'n':
      case 'N': {
	 QoreNodeAsStringHelper t(qv, *param == 'N'
				  ? (width == -1 ? FMT_NORMAL : width)
				  : FMT_NONE, xsink);
	 tbuf.concat(*t, xsink);
	 break;
      }
      case 'y': {
	 QoreNodeAsStringHelper t(qv, FMT_YAML_SHORT, xsink);
	 tbuf.concat(*t, xsink);
	 break;
      }
      default:
	 // if the format argument is not understood, then make sure and just consume the '%' char
	 tbuf.concat('%');
	 param = str;
	 *taken = 0;
   }

   cstr->concat(&tbuf, xsink);
   return (int)(param - str);
}

QoreStringNode* q_sprintf(const QoreListNode* params, int field, int offset, ExceptionSink* xsink) {
   unsigned i, j, l;
   const QoreStringNode* p;

   if (!(p = test_string_param(params, offset)))
      return new QoreStringNode;

   SimpleRefHolder<QoreStringNode> buf(new QoreStringNode(p->getEncoding()));

   j = 1 + offset;

   const char* pstr = p->getBuffer();
   l = p->strlen();
   for (i = 0; i < l; i++) {
      int taken = 1;
      if ((pstr[i] == '%') && (j < params->size())) {
	 const AbstractQoreNode* node = get_param(params, j++);
	 i += process_opt(*buf, (char*)&pstr[i], node, field, &taken, xsink);
	 if (*xsink)
	    return 0;
	 if (!taken)
	    j--;
      }
      else {
	 buf->concat(pstr[i]);
	 if (pstr[i] == '%' && pstr[i+1] == '%')
	     ++i;
      }
   }

   return buf.release();
}

QoreStringNode* q_vsprintf(const QoreListNode* params, int field, int offset, ExceptionSink* xsink) {
   const QoreStringNode* fmt;

   if (!(fmt = test_string_param(params, offset)))
      return new QoreStringNode;

   const AbstractQoreNode* args = get_param(params, offset + 1);
   const QoreListNode* arg_list = get_node_type(args) == NT_LIST ? reinterpret_cast<const QoreListNode*>(args) : 0;

   SimpleRefHolder<QoreStringNode> buf(new QoreStringNode(fmt->getEncoding()));
   unsigned j = 0;

   const char* pstr = fmt->getBuffer();
   size_t l = fmt->strlen();
   for (unsigned i = 0; i < l; i++) {
      int taken = 1;
      bool havearg = false;
      const AbstractQoreNode* arg = 0;

      if ((pstr[i] == '%')) {
	 if (args) {
	    if (arg_list && j < arg_list->size()) {
	       havearg = true;
	       arg = get_param(arg_list, j);
	    }
	    else if (!j) {
	       arg = args;
	       havearg = true;
	    }
	 }
      }
      if (havearg) {
	 ++j;
	 i += process_opt(*buf, (char*)&pstr[i], arg, field, &taken, xsink);
	 if (*xsink)
	    return 0;
	 if (!taken)
	    --j;
      }
      else {
	 buf->concat(pstr[i]);
	 if (pstr[i] == '%' && pstr[i+1] == '%')
             ++i;
      }
   }
   return buf.release();
}

QoreStringNode* q_sprintf(const QoreValueList* params, int field, int offset, ExceptionSink* xsink) {
   unsigned i, j, l;

   QoreValue pv = get_param_value(params, offset);
   if (pv.getType() != NT_STRING)
      return new QoreStringNode;

   const QoreStringNode* p = pv.get<QoreStringNode>();

   SimpleRefHolder<QoreStringNode> buf(new QoreStringNode(p->getEncoding()));

   j = 1 + offset;

   const char* pstr = p->getBuffer();
   l = p->strlen();
   for (i = 0; i < l; i++) {
      int taken = 1;
      if ((pstr[i] == '%') && (j < params->size())) {
	 pv = get_param_value(params, j++);
	 i += process_opt(*buf, (char*)&pstr[i], pv, field, &taken, xsink);
	 if (*xsink)
	    return 0;
	 if (!taken)
	    j--;
      }
      else {
	 buf->concat(pstr[i]);
	 if (pstr[i] == '%' && pstr[i+1] == '%')
	     ++i;
      }
   }

   return buf.release();
}

QoreStringNode* q_vsprintf(const QoreValueList* params, int field, int offset, ExceptionSink* xsink) {
   QoreValue pv = get_param_value(params, offset);
   if (pv.getType() != NT_STRING)
      return new QoreStringNode;

   const QoreStringNode* fmt = pv.get<QoreStringNode>();

   pv = get_param_value(params, offset + 1);
   const QoreListNode* arg_list = pv.getType() == NT_LIST ? pv.get<const QoreListNode>() : 0;

   SimpleRefHolder<QoreStringNode> buf(new QoreStringNode(fmt->getEncoding()));
   unsigned j = 0;

   const char* pstr = fmt->getBuffer();
   size_t l = fmt->strlen();
   for (unsigned i = 0; i < l; i++) {
      int taken = 1;
      bool havearg = false;

      QoreValue cv;

      if ((pstr[i] == '%')) {
	 if (!pv.isNothing()) {
	    havearg = true;
	    if (arg_list && j < arg_list->size())
	       cv = get_param(arg_list, j);
	    else if (!j)
	       cv = pv;
	 }
      }
      if (havearg) {
	 ++j;
	 i += process_opt(*buf, (char*)&pstr[i], cv, field, &taken, xsink);
	 if (*xsink)
	    return 0;
	 if (!taken)
	    --j;
      }
      else {
	 buf->concat(pstr[i]);
	 if (pstr[i] == '%' && pstr[i+1] == '%')
             ++i;
      }
   }
   return buf.release();
}

static void concatASCII(QoreString &str, unsigned char c) {
   str.sprintf("ascii %03d", c);
   if (c >= 32 || c < 127)
      str.sprintf(" ('%c')", c);
}

static char getBase64Value(const char* buf, qore_size_t &offset, bool end_ok, ExceptionSink* xsink) {
   while (buf[offset] == '\n' || buf[offset] == '\r')
      ++offset;

   char c = buf[offset];

   if (c >= 'A' && c <= 'Z')
      return c - 'A';
   if (c >= 'a' && c <= 'z')
      return c - 'a' + 26;
   if (c >= '0' && c <= '9')
      return c - '0' + 52;
   if (c == '+')
      return 62;
   if (c == '/')
      return 63;

   if (!c) {
      if (!end_ok)
	 xsink->raiseException("BASE64-PARSE-ERROR", "premature end of base64 string at string byte offset %d", offset);
   }
   else {
      QoreStringNode* desc = new QoreStringNode;
      concatASCII(*desc, c);
      desc->concat(" is an invalid base64 character");
      xsink->raiseException("BASE64-PARSE-ERROR", desc);
   }
   return -1;
}

// see: RFC-1421: http://www.ietf.org/rfc/rfc1421.txt and RFC-2045: http://www.ietf.org/rfc/rfc2045.txt
BinaryNode* parseBase64(const char* buf, int len, ExceptionSink* xsink) {
   if (!len)
      return new BinaryNode;

   char* binbuf = (char* )malloc(sizeof(char) * (len + 3));
   int blen = 0;

   qore_size_t pos = 0;
   while (pos < (qore_size_t)len) {
      // add first 6 bits
      char b = getBase64Value(buf, pos, true, xsink);
      if (xsink->isEvent()) {
         free(binbuf);
         return 0;
      }
      // if we've reached the end of the string here, then exit the loop
      if (!buf[pos])
	 break;

      // get second 6 bits
      ++pos;
      char c = getBase64Value(buf, pos, false, xsink);
      if (xsink->isEvent()) {
         free(binbuf);
         return 0;
      }
      // do first byte (1st char=upper 6 bits, 2nd char=lower 2)
      binbuf[blen++] = (b << 2) | (c >> 4);

      // check special cases
      ++pos;
      if (buf[pos] == '=')
         break;

      // low 4 bits from 2nd char become high 4 bits of next value
      b = (c & 15) << 4;

      // get third 6 bits
      c = getBase64Value(buf, pos, false, xsink);
      if (xsink->isEvent()) {
         free(binbuf);
         return 0;
      }
      // do second byte (2nd char=upper 4 bits, 3rd char=lower 4 bits)
      binbuf[blen++] = b | (c >> 2);

      // check special cases
      ++pos;
      if (buf[pos] == '=')
         break;

      // low 2 bits from 3rd char become high 2 bits of next value
      b = (c & 3) << 6;

      // get fourth 6 bits
      c = getBase64Value(buf, pos, false, xsink);
      if (xsink->isEvent()) {
         free(binbuf);
         return 0;
      }

      binbuf[blen++] = b | c;
      ++pos;
   }
   return new BinaryNode(binbuf, blen);
}

int get_nibble(char c, ExceptionSink* xsink) {
   if (isdigit(c))
      return c - 48;
   if (c >= 'A' && c <= 'F')
      return c - 55;
   if (c >= 'a' && c <= 'f')
      return c - 87;

   xsink->raiseException("PARSE-HEX-ERROR", "invalid hex digit found '%c'", c);
   return -1;
}

BinaryNode* parseHex(const char* buf, int len, ExceptionSink* xsink) {
   if (!len)
      return new BinaryNode();

   if ((len / 2) * 2 != len) {
      xsink->raiseException("PARSE-HEX-ERROR", "cannot parse an odd number of hex digits (%d digit%s)", len, len == 1 ? "" : "s");
      return 0;
   }

   char* binbuf = (char* )malloc(sizeof(char) * (len / 2));
   int blen = 0;

   const char* end = buf + len;
   while (buf < end) {
      int b = get_nibble(*buf, xsink);
      if (b < 0) {
	 free(binbuf);
	 return 0;
      }
      buf++;
      int l = get_nibble(*buf, xsink);
      if (l < 0) {
	 free(binbuf);
	 return 0;
      }
      buf++;
      binbuf[blen++] = b << 4 | l;
   }
   return new BinaryNode(binbuf, blen);
}

static inline int parse_get_nibble(char c) {
   if (isdigit(c))
      return c - 48;
   if (c >= 'A' && c <= 'F')
      return c - 55;
   if (c >= 'a' && c <= 'f')
      return c - 87;

   parseException("PARSE-HEX-ERROR", "invalid hex digit found '%c'", c);
   return -1;
}

// for use while parsing - parses a null-terminated string and raises parse exceptions for errors
BinaryNode* parseHex(const char* buf, int len) {
   if (!buf || !(*buf))
      return new BinaryNode();

   char* binbuf = (char* )malloc(sizeof(char) * (len / 2));
   int blen = 0;

   const char* end = buf + len;
   while (buf < end) {
      int b = parse_get_nibble(*buf);
      if (b < 0) {
	 free(binbuf);
	 return 0;
      }
      buf++;
#if 0
      // this can never happen; the parser guarantees an even number of digits
      if (!(*buf)) {
	 free(binbuf);
	 parseError("PARSE-HEX-ERROR", "cannot parse an odd number of hex digits (%d digit%s)", len, len == 1 ? "" : "s");
	 return 0;
      }
#endif

      int l = parse_get_nibble(*buf);
      if (l < 0) {
	 free(binbuf);
	 return 0;
      }
      buf++;
      binbuf[blen++] = b << 4 | l;
   }
   return new BinaryNode(binbuf, blen);
}

char* make_class_name(const char* str) {
   char* cn = q_basename(str);
   char* p = strrchr(cn, '.');
   if (p && p != cn)
      *p = '\0';
   p = cn;
   while (*p) {
      if (*p == '-')
	 *p = '_';
      p++;
   }
   return cn;
}

void print_node(FILE* fp, const QoreValue qv) {
   printd(5, "print_node() node: %s\n", qv.getTypeName());
   QoreStringValueHelper str(qv);
   fputs(str->getBuffer(), fp);
}

void qore_setup_argv(int pos, int argc, char* argv[]) {
   ARGV = new QoreListNode();
   QORE_ARGV = new QoreListNode();
   int end = argc - pos;
   for (int i = 0; i < argc; i++) {
      if (i < end)
	 ARGV->push(new QoreStringNode(argv[i + pos]));
      QORE_ARGV->push(new QoreStringNode(argv[i]));
   }
}

void init_lib_intern(char* env[]) {
   // set up environment hash
   int i = 0;
   ENV = new QoreHashNode;
   while (env[i]) {
      char* p;

      if ((p = strchr(env[i], '='))) {
	 char save = *p;
	 *p = '\0';
	 ENV->setKeyValue(env[i], new QoreStringNode(p + 1), 0);
	 //printd(5, "creating $ENV{\"%s\"} = \"%s\"\n", env[i], p + 1);
	 *p = save;
      }
      i++;
   }

   // initialize process-default local time zone
   QTZM.init();

   // other misc initialization
#if defined(HAVE_GETPWUID_R) || defined(HAVE_GETPWNAM_R)
   pwsize = sysconf(_SC_GETPW_R_SIZE_MAX);
   if (pwsize == -1)
      pwsize = 4096; // more than enough?
#endif
#if defined(HAVE_GETGRGID_R) || defined(HAVE_GETGRNAM_R)
   grsize = sysconf(_SC_GETGR_R_SIZE_MAX);
   if (grsize == -1)
      grsize = 4096; // more than enough?
#endif
}

void delete_global_variables() {
   QORE_TRACE("delete_global_variables()");
   if (QORE_ARGV)
      QORE_ARGV->deref(0);
   if (ARGV)
      ARGV->deref(0);
   if (ENV)
      ENV->deref(0);
}

struct tm *q_localtime(const time_t* clock, struct tm* tms) {
#ifdef HAVE_LOCALTIME_R
   localtime_r(clock, tms);
#else
   lck_localtime.lock();
   struct tm *t = localtime(clock);
   memcpy(tms, t, sizeof(struct tm));
   lck_localtime.unlock();
#endif
   return tms;
}

struct tm *q_gmtime(const time_t* clock, struct tm* tms) {
#ifdef HAVE_GMTIME_R
   gmtime_r(clock, tms);
#else
   lck_gmtime.lock();
   struct tm *t = gmtime(clock);
   memcpy(tms, t, sizeof(struct tm));
   lck_gmtime.unlock();
#endif
   return tms;
}

const char* q_find_first_path_sep(const char* path) {
#ifdef _Q_WINDOWS
   // on windows we have to find either '\\' (standard directory separator character) or '/' (UNIX-style, also accepted on Windows)
   const char* p = path;
   while (*p) {
      if (*p == '/' || *p == '\\')
	 return p;
      ++p;
   }
   return 0;
#else
   return strchr(path, QORE_DIR_SEP);
#endif
}

const char* q_find_last_path_sep(const char* path) {
#ifdef _Q_WINDOWS
   // on windows we have to find either '\\' (standard directory separator character) or '/' (UNIX-style, also accepted on Windows)
   const char* p = path;
   const char* rv = 0;
   while (*p) {
      if (*p == '/' || *p == '\\')
	 rv = p;
      ++p;
   }
   return rv;
#else
   return strrchr(path, QORE_DIR_SEP);
#endif
}

// thread-safe basename function (resulting pointer must be free()ed)
char* q_basename(const char* path) {
   const char* p = q_find_last_path_sep(path);
   if (!p)
      return strdup(path);
   return strdup(p + 1);
}

// returns a pointer within the same string
char* q_basenameptr(const char* path) {
   const char* p = q_find_last_path_sep(path);
   if (!p)
      return (char* )path;
   return (char*)p + 1;
}

// thread-safe dirname function (resulting pointer must be free()ed)
char* q_dirname(const char* path) {
   const char* p = q_find_last_path_sep(path);
   if (!p || p == path) {
      char* x = (char* )malloc(sizeof(char) * 2);
      x[0] = !p ? '.' : QORE_DIR_SEP;
      x[1] = '\0';
      return x;
   }
   char* x = (char* )malloc(sizeof(char) * (p - path + 1));
   strncpy(x, path, p - path);
   x[p - path] = '\0';
   return x;
}

void *q_realloc(void* ptr, size_t size) {
   void *p = realloc(ptr, size);
   if (!p)
      free(ptr);
   return p;
}

static inline void assign_hv(QoreHashNode* h, const char* key, char* val) {
   h->setKeyValue(key, new QoreStringNode(val), 0);
}

static inline void assign_hv(QoreHashNode* h, const char* key, int val) {
   h->setKeyValue(key, new QoreBigIntNode(val), 0);
}

#ifdef HAVE_PWD_H
static QoreHashNode* pwd2hash(const struct passwd& pw) {
   QoreHashNode* h = new QoreHashNode;
   // assign values
   assign_hv(h, "pw_name", pw.pw_name);
   assign_hv(h, "pw_passwd", pw.pw_passwd);
   assign_hv(h, "pw_gecos", pw.pw_gecos);
   assign_hv(h, "pw_dir", pw.pw_dir);
   assign_hv(h, "pw_shell", pw.pw_shell);
   assign_hv(h, "pw_uid", pw.pw_uid);
   assign_hv(h, "pw_gid", pw.pw_gid);
   return h;
}

QoreHashNode* q_getpwuid(uid_t uid) {
   struct passwd *pw;
#ifdef HAVE_GETPWUID_R
   struct passwd pw_rec;
   char* buf = (char* )malloc(pwsize * sizeof(char));
   ON_BLOCK_EXIT(free, buf);
   int rc = getpwuid_r(uid, &pw_rec, buf, pwsize, &pw);
   if (rc)
      errno = rc;
#else
   AutoLocker al(&lck_passwd);
   pw = getpwuid(uid);
#endif
   return !pw ? 0 : pwd2hash(*pw);
}

QoreHashNode* q_getpwnam(const char* name) {
   struct passwd *pw;
#ifdef HAVE_GETPWNAM_R
   struct passwd pw_rec;
   char* buf = (char* )malloc(pwsize * sizeof(char));
   ON_BLOCK_EXIT(free, buf);
   int rc = getpwnam_r(name, &pw_rec, buf, pwsize, &pw);
   if (rc)
      errno = rc;
#else
   AutoLocker al(&lck_passwd);
   pw = getpwnam(name);
#endif
   return !pw ? 0 : pwd2hash(*pw);
}

int q_uname2uid(const char* name, uid_t& uid) {
   struct passwd *pw;
#ifdef HAVE_GETPWNAM_R
   struct passwd pw_rec;
   char* buf = (char* )malloc(pwsize * sizeof(char));
   ON_BLOCK_EXIT(free, buf);
   int rc = getpwnam_r(name, &pw_rec, buf, pwsize, &pw);
   if (!rc)
      uid = pw_rec.pw_uid;
   return rc;
#else
   AutoLocker al(&lck_passwd);
   pw = getpwnam(name);
   if (!pw)
      return errno;
   uid = pw->pw_uid;
   return 0;
#endif
}

static QoreHashNode* gr2hash(struct group& gr) {
   QoreHashNode* h = new QoreHashNode;
   // assign values
   assign_hv(h, "gr_name", gr.gr_name);
   assign_hv(h, "gr_passwd", gr.gr_passwd);
   assign_hv(h, "gr_gid", gr.gr_gid);

   QoreListNode* l = new QoreListNode;
   for (char* *p = gr.gr_mem; *p; ++p)
      l->push(new QoreStringNode(*p));

   h->setKeyValue("gr_mem", l, 0);
   return h;
}

QoreHashNode* q_getgrgid(gid_t gid) {
   struct group *gr;
#ifdef HAVE_GETGRGID_R
   struct group gr_rec;
   char* buf = (char* )malloc(grsize * sizeof(char));
   ON_BLOCK_EXIT(free, buf);
   int rc = getgrgid_r(gid, &gr_rec, buf, grsize, &gr);
   if (rc)
      errno = rc;
#else
   AutoLocker al(&lck_group);
   gr = getgrgid(gid);
#endif
   return !gr ? 0 : gr2hash(*gr);
}

QoreHashNode* q_getgrnam(const char* name) {
   struct group *gr;
#ifdef HAVE_GETGRNAM_R
   struct group gr_rec;
   char* buf = (char* )malloc(grsize * sizeof(char));
   ON_BLOCK_EXIT(free, buf);
   int rc = getgrnam_r(name, &gr_rec, buf, grsize, &gr);
   if (rc)
      errno = rc;
#else
   AutoLocker al(&lck_group);
   gr = getgrnam(name);
#endif
   return !gr ? 0 : gr2hash(*gr);
}

int q_gname2gid(const char* name, gid_t &gid) {
   struct group *gr;
#ifdef HAVE_GETGRNAM_R
   struct group gr_rec;
   char* buf = (char* )malloc(grsize * sizeof(char));
   ON_BLOCK_EXIT(free, buf);
   int rc = getgrnam_r(name, &gr_rec, buf, grsize, &gr);
   if (!rc)
      gid = gr_rec.gr_gid;
   return rc;
#else
   AutoLocker al(&lck_group);
   gr = getgrnam(name);
   if (!gr)
      return errno;
   gid = gr->gr_gid;
   return 0;
#endif
}
#endif // HAVE_PWD_H

qore_license_t qore_get_license() {
   return qore_license;
}

long long q_atoll(const char* str) {
   return atoll(str);
}

// returns seconds since epoch
int64 q_epoch() {
#ifdef HAVE_CLOCK_GETTIME
   struct timespec ts;
   if (clock_gettime(CLOCK_REALTIME, &ts)) {
      printd(0, "clock_gettime() failed: %s\n", strerror(errno));
      return 0;
   }
#else
   struct timeval ts;
   if (gettimeofday(&ts, 0)) {
      printd(0, "gettimeofday() failed: %s\n", strerror(errno));
      return 0;
   }
#endif
   return ts.tv_sec;
}

// returns seconds since epoch and gets microseconds
int64 q_epoch_us(int &us) {
#ifdef HAVE_CLOCK_GETTIME
   struct timespec ts;
   if (clock_gettime(CLOCK_REALTIME, &ts)) {
      printd(0, "clock_gettime() failed: %s\n", strerror(errno));
      us = 0;
      return 0;
   }
   us = ts.tv_nsec / 1000;
#else
   struct timeval ts;
   if (gettimeofday(&ts, 0)) {
      printd(0, "gettimeofday() failed: %s\n", strerror(errno));
      us = 0;
      return 0;
   }
   us = ts.tv_usec;
#endif
   return ts.tv_sec;
}

// returns seconds since epoch and gets nanoseconds
int64 q_epoch_ns(int &ns) {
#ifdef HAVE_CLOCK_GETTIME
   struct timespec ts;
   if (clock_gettime(CLOCK_REALTIME, &ts)) {
      printd(0, "clock_gettime() failed: %s\n", strerror(errno));
      ns = 0;
      return 0;
   }
   ns = ts.tv_nsec;
#else
   struct timeval ts;
   if (gettimeofday(&ts, 0)) {
      printd(0, "gettimeofday() failed: %s\n", strerror(errno));
      ns = 0;
      return 0;
   }
   ns = ts.tv_usec * 1000;
#endif
   return ts.tv_sec;
}

QoreListNode* make_args(AbstractQoreNode* arg) {
   if (!arg)
      return 0;

   QoreListNode* l;
   if (arg->getType() == NT_LIST) {
      l = reinterpret_cast<QoreListNode*>(arg);
      if (!l->isFinalized())
         return l;
   }

   l = new QoreListNode(arg->needs_eval());
   l->push(arg);
   return l;
}

const char* check_hash_key(const QoreHashNode* h, const char* key, const char* err, ExceptionSink* xsink) {
   const AbstractQoreNode* p = h->getKeyValue(key);
   if (is_nothing(p))
      return 0;

   if (p->getType() != NT_STRING) {
      xsink->raiseException(err, "'%s' key is not type 'string' but is type '%s'", key, get_type_name(p));
      return 0;
   }
   return reinterpret_cast<const QoreStringNode*>(p)->getBuffer();
}

void q_strerror(QoreString &str, int err) {
#ifdef HAVE_STRERROR_R

#ifndef STRERR_BUFSIZE
#define STRERR_BUFSIZE 256
#endif

   str.allocate(str.strlen() + STRERR_BUFSIZE);
   // ignore strerror() error message
#ifdef STRERROR_R_CHAR_P
   // we can't help but get this version because some of the Linux
   // header files define _GNU_SOURCE for us :-(
   str.concat(strerror_r(err, (char* )(str.getBuffer() + str.strlen()), STRERR_BUFSIZE));
#else
   // use portable XSI version of strerror_r()
   int rc = strerror_r(err, (char* )(str.getBuffer() + str.strlen()), STRERR_BUFSIZE);
   if (rc && rc != EINVAL && rc != ERANGE)
      str.sprintf("unable to retrieve error code %d: strerror() returned unexpected error code %d", err, rc);
   else
      str.terminate(str.strlen() + strlen(str.getBuffer() + str.strlen()));
#endif

#else
   // global static lock for strerror() access
   static QoreThreadLock strerror_m;

   AutoLocker al(strerror_m);
   str.concat(strerror(err));
#endif
}

QoreStringNode* q_strerror(int err) {
   QoreStringNode* rv = new QoreStringNode;
   q_strerror(*rv, err);
   return rv;
}

#ifdef HAVE_SIGNAL_HANDLING
QoreStringNode* qore_reassign_signal(int sig, const char* name) {
   return QSM.reassign_signal(sig, name);
}
#endif

// returns 0 for OK, -1 for error
int check_lvalue(AbstractQoreNode* node, bool assignment) {
   qore_type_t ntype = node->getType();
   //printd(5, "type: %s\n", node->getTypeName());
   if (ntype == NT_VARREF) {
      if (assignment)
         reinterpret_cast<VarRefNode*>(node)->parseAssigned();
      return 0;
   }

   if (ntype == NT_SELF_VARREF)
      return 0;

   if (ntype == NT_CLASS_VARREF)
      return 0;

   if (ntype == NT_OPERATOR) {
      QoreSquareBracketsOperatorNode* op = dynamic_cast<QoreSquareBracketsOperatorNode*>(node);
      if (op) {
	 return check_lvalue(op->getLeft(), assignment);
      }
      else {
         QoreHashObjectDereferenceOperatorNode* hop = dynamic_cast<QoreHashObjectDereferenceOperatorNode*>(node);
         if (hop) {
            return check_lvalue(hop->getLeft(), assignment);
         }
      }
      return -1;
   }

   return -1;
}

static void stat_get_blocks(const struct stat &sbuf, int64& blksize, int64& blocks) {
#ifdef HAVE_STRUCT_STAT_ST_BLKSIZE
   blksize = sbuf.st_blksize;
#else
   blksize = 0;
#endif
#ifdef HAVE_STRUCT_STAT_ST_BLOCKS
   blocks = sbuf.st_blocks;
#else
   blocks = 0;
#endif
}

QoreListNode* stat_to_list(const struct stat& sbuf) {
   QoreListNode* l = new QoreListNode;

   // note that dev_t on Linux is an unsigned 64-bit integer, so we could lose precision here
   l->push(new QoreBigIntNode((int64)sbuf.st_dev));
   l->push(new QoreBigIntNode(sbuf.st_ino));
   l->push(new QoreBigIntNode(sbuf.st_mode));
   l->push(new QoreBigIntNode(sbuf.st_nlink));
   l->push(new QoreBigIntNode(sbuf.st_uid));
   l->push(new QoreBigIntNode(sbuf.st_gid));
   // note that dev_t on Linux is an unsigned 64-bit integer, so we could lose precision here
   l->push(new QoreBigIntNode((int64)sbuf.st_rdev));
   l->push(new QoreBigIntNode(sbuf.st_size));

   l->push(DateTimeNode::makeAbsolute(currentTZ(), (int64)sbuf.st_atime));
   l->push(DateTimeNode::makeAbsolute(currentTZ(), (int64)sbuf.st_mtime));
   l->push(DateTimeNode::makeAbsolute(currentTZ(), (int64)sbuf.st_ctime));

   int64 blksize, blocks;
   stat_get_blocks(sbuf, blksize, blocks);
   l->push(new QoreBigIntNode(blksize));
   l->push(new QoreBigIntNode(blocks));

   return l;
}

QoreHashNode* stat_to_hash(const struct stat& sbuf) {
   QoreHashNode* h = new QoreHashNode;

   // note that dev_t on Linux is an unsigned 64-bit integer, so we could lose precision here
   h->setKeyValue("dev",     new QoreBigIntNode((int64)sbuf.st_dev), 0);
   h->setKeyValue("inode",   new QoreBigIntNode(sbuf.st_ino), 0);
   h->setKeyValue("mode",    new QoreBigIntNode(sbuf.st_mode), 0);
   h->setKeyValue("nlink",   new QoreBigIntNode(sbuf.st_nlink), 0);
   h->setKeyValue("uid",     new QoreBigIntNode(sbuf.st_uid), 0);
   h->setKeyValue("gid",     new QoreBigIntNode(sbuf.st_gid), 0);
   // note that dev_t on Linux is an unsigned 64-bit integer, so we could lose precision here
   h->setKeyValue("rdev",    new QoreBigIntNode((int64)sbuf.st_rdev), 0);
   h->setKeyValue("size",    new QoreBigIntNode(sbuf.st_size), 0);

   h->setKeyValue("atime",   DateTimeNode::makeAbsolute(currentTZ(), (int64)sbuf.st_atime), 0);
   h->setKeyValue("mtime",   DateTimeNode::makeAbsolute(currentTZ(), (int64)sbuf.st_mtime), 0);
   h->setKeyValue("ctime",   DateTimeNode::makeAbsolute(currentTZ(), (int64)sbuf.st_ctime), 0);

   int64 blksize, blocks;
   stat_get_blocks(sbuf, blksize, blocks);
   h->setKeyValue("blksize", new QoreBigIntNode(blksize), 0);
   h->setKeyValue("blocks",  new QoreBigIntNode(blocks), 0);

   // process permissions
   QoreStringNode* perm = new QoreStringNode;
   const char* type = q_mode_to_perm(sbuf.st_mode, *perm);

   h->setKeyValue("type", new QoreStringNode(type), 0);
   h->setKeyValue("perm", perm, 0);

   return h;
}

#ifdef Q_HAVE_STATVFS
QoreHashNode* statvfs_to_hash(const struct statvfs& vfs) {
   QoreHashNode* h = new QoreHashNode;

#ifdef DARWIN
#else
   h->setKeyValue("namemax", new QoreBigIntNode(vfs.f_namemax), 0);
#endif
   h->setKeyValue("fsid", new QoreBigIntNode(vfs.f_fsid), 0);
   h->setKeyValue("frsize", new QoreBigIntNode(vfs.f_frsize), 0);
   h->setKeyValue("bsize", new QoreBigIntNode(vfs.f_bsize), 0);
   h->setKeyValue("flag", new QoreBigIntNode(vfs.f_flag), 0);
   h->setKeyValue("blocks", new QoreBigIntNode(vfs.f_blocks), 0);
   h->setKeyValue("bfree", new QoreBigIntNode(vfs.f_bfree), 0);
   h->setKeyValue("bavail", new QoreBigIntNode(vfs.f_bavail), 0);
   h->setKeyValue("files", new QoreBigIntNode(vfs.f_files), 0);
   h->setKeyValue("ffree", new QoreBigIntNode(vfs.f_ffree), 0);
   h->setKeyValue("favail", new QoreBigIntNode(vfs.f_favail), 0);

   return h;
}
#endif

#if defined(DEBUG) && defined(HAVE_BACKTRACE)
#define _QORE_BT_SIZE 20
void qore_machine_backtrace() {
   void *array[_QORE_BT_SIZE];
   // get void*'s for all entries on the stack
   size_t size = backtrace(array, _QORE_BT_SIZE);

   // print out all the frames to stderr
   backtrace_symbols_fd(array, size, 2);
}
#else
void qore_machine_backtrace() {
}
#endif

#ifdef HAVE_NANOSLEEP
static int qore_nanosleep(int64 ns) {
   struct timespec ts;
   ts.tv_sec = ns / 1000000000ll;
   ts.tv_nsec = (ns - ts.tv_sec * 1000000000);
   return nanosleep(&ts, 0);
}
#endif

int qore_usleep(int64 usecs) {
#ifdef HAVE_NANOSLEEP
   return qore_nanosleep(usecs * 1000);
#else
#ifdef _Q_WINDOWS
   LARGE_INTEGER ft;
   ft.QuadPart = -(10 * usecs); // Convert to 100 nanosecond interval, negative value indicates relative time

   HANDLE timer = CreateWaitableTimer(0, TRUE, 0);
   SetWaitableTimer(timer, &ft, 0, 0, 0, 0);
   WaitForSingleObject(timer, INFINITE);
   CloseHandle(timer);
   return 0;
#else
   return ::usleep(usecs);
#endif
#endif
}

bool q_path_is_readable(const char* path) {
#if defined HAVE_PWD_H
   struct stat sbuf;
   int rc;

   if ((rc = stat(path, &sbuf)))
      return false;

   if (S_ISDIR(sbuf.st_mode)) { // If path is a directory.
      DIR* dp = opendir(path);
      if (dp != NULL) {
         closedir(dp);
         return true;
      }
      return false;
   }

   rc = open(path, O_RDONLY);
   if (rc != -1) {
      close(rc);
      return true;
   }
   return false;

#elif defined(HAVE_ACCESS) && defined(_Q_WINDOWS)
   // only use access(2) on Windows
   return !access(path, R_OK);
#else
   // check if it's a directory
   struct stat sbuf;
   int rc;
   if ((rc = stat(path, &sbuf)))
      return false;

   // just return true on windows if it's a directory
   if ((sbuf.st_mode & S_IFMT) == S_IFDIR)
      return true;

   // otherwise try to open file, if successful, then the file is readable
   rc = open(path, O_RDONLY);
   if (rc != -1) {
      close(rc);
      return true;
   }
   return false;
#endif
}

QoreProgramLocation::QoreProgramLocation(int sline, int eline) : QoreProgramLineLocation(sline, eline) {
   set_parse_file_info(*this);
}

QoreProgramLocation::QoreProgramLocation(prog_loc_e loc) {
   if (loc == ParseLocation)
      *this = get_parse_location();
   else
      *this = get_runtime_location();
}

void QoreProgramLocation::parseSet() const {
   update_parse_location(*this);
}

void QoreProgramLocation::toString(QoreString& str) const {
   str.concat(file ? file : "<unknown>");
   if (start_line > 0) {
      str.sprintf(":%d", start_line);
      if (end_line > 0 && end_line != start_line)
	 str.sprintf("-%d", end_line);
   }

   if (source)
      str.sprintf(" (source \"%s\":%d)", source, start_line + offset);
}

void LVarSet::add(LocalVar* var) {
   if (!needs_scan && var->needsScan())
      needs_scan = true;
   // insert var into the set
   insert(var);
}

bool q_parse_bool(const AbstractQoreNode* n) {
   if (get_node_type(n) == NT_STRING)
      return q_parse_bool(reinterpret_cast<const QoreStringNode*>(n)->getBuffer());

   return n->getAsBool();
}

bool q_parse_bool(const char* str) {
   if (!strcasecmp(str, "true") || !strcasecmp(str, "on") || !strcasecmp(str, "yes") || !strncasecmp(str, "enable", 6) || !strcasecmp(str, "y"))
      return true;

   return (bool)atoi(str);
}

int qore_get_library_init_options() {
   return qore_library_options;
}

int qore_set_library_cleanup_options(int options) {
   return (qore_library_options |= (options & QLO_CLEANUP_MASK));
}

bool qore_check_option(int opt) {
   return (qore_library_options & opt) == opt;
}

const char* q_mode_to_perm(mode_t mode, QoreString& perm) {
   const char* type;
   if (S_ISBLK(mode)) {
      type = "BLOCK-DEVICE";
      perm.concat('b');
   }
   else if (S_ISDIR(mode)) {
      type = "DIRECTORY";
      perm.concat('d');
   }
   else if (S_ISCHR(mode)) {
      type = "CHARACTER-DEVICE";
      perm.concat('c');
   }
   else if (S_ISFIFO(mode)) {
      type = "FIFO";
      perm.concat('p');
   }
#ifdef S_ISLNK
   else if (S_ISLNK(mode)) {
      type = "SYMBOLIC-LINK";
      perm.concat('l');
   }
#endif
#ifdef S_ISSOCK
   else if (S_ISSOCK(mode)) {
      type = "SOCKET";
      perm.concat('s');
   }
#endif
   else if (S_ISREG(mode)) {
      type = "REGULAR";
      perm.concat('-');
   }
   else {
      type = "UNKNOWN";
      perm.concat('?');
   }

   // add user permission flags
   perm.concat(mode & S_IRUSR ? 'r' : '-');
   perm.concat(mode & S_IWUSR ? 'w' : '-');
#ifdef S_ISUID
   if (mode & S_ISUID)
      perm.concat(mode & S_IXUSR ? 's' : 'S');
   else
      perm.concat(mode & S_IXUSR ? 'x' : '-');
#else
   // Windows
   perm.concat('-');
#endif

   // add group permission flags
#ifdef S_IRGRP
   perm.concat(mode & S_IRGRP ? 'r' : '-');
   perm.concat(mode & S_IWGRP ? 'w' : '-');
#else
   // Windows
   perm.concat("--");
#endif
#ifdef S_ISGID
   if (mode & S_ISGID)
      perm.concat(mode & S_IXGRP ? 's' : 'S');
   else
      perm.concat(mode & S_IXGRP ? 'x' : '-');
#else
   // Windows
   perm.concat('-');
#endif

#ifdef S_IROTH
   // add other permission flags
   perm.concat(mode & S_IROTH ? 'r' : '-');
   perm.concat(mode & S_IWOTH ? 'w' : '-');
#ifdef S_ISVTX
   if (mode & S_ISVTX)
      perm.concat(mode & S_IXOTH ? 't' : 'T');
   else
#endif
      perm.concat(mode & S_IXOTH ? 'x' : '-');
#else
   // Windows
   perm.concat("---");
#endif

   return type;
}

int64 q_clock_getmillis() {
   int us;
   int64 seconds = q_epoch_us(us);
   return seconds * 1000 + us / 1000;
}

int64 q_clock_getmicros() {
   int us;
   int64 seconds = q_epoch_us(us);
   return seconds * 1000000ll + us;
}

int64 q_clock_getnanos() {
   int ns;
   int64 seconds = q_epoch_ns(ns);
   return seconds * 1000000000ll + ns;
}

#ifndef HAVE_STRCASESTR
#ifdef HAVE_STRNCASECMP
char* strcasestr(const char* s1, const char* s2) {
   if (!s2 || !s1) return 0;

   size_t len1 = strlen(s1);
   size_t len2 = strlen(s2);
   if (len2 > len1)
      return 0;

   for (size_t i = 0, end = len1 - len2; i <= end; ++i) {
      if (!strncasecmp(s2, s1 + i, len2)) {
	 return ((char*)s1 + i);
      }
   }

   return 0;
}
#else
#error no strcasestr implementation
#endif
#endif

bool q_absolute_path_unix(const char* path) {
   return path && path[0] == '/';
}

// returns true if the path is not relative; may not be strictly-speaking an absolute path
bool q_absolute_path_windows(const char* path) {
   if (!path)
      return false;
   // note that '\' and '/' are both accepted as path separator characters on Windows
   if (path[0] == '\\' || path[0] == '/')
      return true;
   if (isalpha(path[0]) && path[1] == ':')
      return true;
   return false;
}

bool q_absolute_path(const char* path) {
#ifdef _Q_WINDOWS
   return q_absolute_path_windows(path);
#else
   return q_absolute_path_unix(path);
#endif
}

#ifdef _Q_WINDOWS
// this appends the root path
int q_get_root_path(QoreString& root, const char* cwd = 0) {
   QoreString Cwd;
   if (cwd) {
      Cwd.set(cwd);
      q_realpath(Cwd, Cwd);
   }
   else
      q_getcwd(Cwd);

   // return without making any changes in case of an error
   if (Cwd.empty())
      return -1;

   // see if we have a drive letter + ':'
   if (isalpha(Cwd[0])) {
      root.concat(Cwd[0]);
      root.concat(':');
      return 0;
   }

   // see if we have a UNC path, so we need to find the end of the \\server\share\ component
   if (Cwd[0] == '\\' && Cwd[1] == '\\') {
      char* c = strchr(Cwd.c_str() + 2, '\\');
      if (c) {
         c = strchr(c + 1, '\\');
         root.concat(Cwd.c_str(), c ? c - Cwd.c_str() : Cwd.size());
         return 0;
      }
   }
   return -1;
}
#endif

static void add_cwd(QoreString& path) {
   QoreString cwd_str;
   if (!q_getcwd(cwd_str))
      path.prepend(cwd_str.getBuffer(), cwd_str.size());
}

void q_normalize_path(QoreString& path, const char* cwd) {
   //printd(5, "q_normalize_path(path: '%s', cwd: '%s')\n", path.getBuffer(), cwd);
   if (!path.empty()) {
      if (!q_absolute_path(path.getBuffer())) {
         path.insertch(QORE_DIR_SEP, 0, 1);
         if (cwd) {
            QoreString Cwd(cwd);
            q_realpath(Cwd, Cwd);
            path.prepend(Cwd.getBuffer());
            if (path[0] == '.') {
               path.insertch(QORE_DIR_SEP, 0, 1);
               add_cwd(path);
            }
         }
         else
            add_cwd(path);
      }
#ifdef _Q_WINDOWS
      // prepend drive or UNC path root to path if the path begins with a single slash or backslash
      else if ((path[0] == '/' || path[0] == '\\')
               && (path[1] != '/' && path[1] != '\\')) {
         QoreString root;
         q_get_root_path(root, cwd);
         path.prepend(root.c_str());
         //printd(5, "normalized root: '%s' path: '%s'\n", root.c_str(), path.c_str());
      }
#endif
   }
   std::string str = path.getBuffer();
   str = qore_qd_private::normalizePath(str);
   //printd(5, "q_normalize_path() '%s' -> '%s' (cwd: '%s')\n", path.getBuffer(), str.c_str(), cwd);
   path = str;
}

int q_getcwd(QoreString& cwd) {
   int bs = 512;
   cwd.reserve(bs);

   while (true) {
      char* b = getcwd((char*)cwd.getBuffer(), bs);
      if (!b) {
          if (errno == ERANGE) {
              bs *= 2;
	      cwd.reserve(bs);
              continue;
          }
	  //printd(5, "q_getcwd() failed: errno: %d\n", errno);
          return -1;
      }
      break;
   }

   cwd.terminate(strlen(cwd.getBuffer()));
   //printd(5, "q_getcwd() succeeded: '%s'\n", cwd.getBuffer());
   return 0;
}

int q_get_mode(const QoreString& path) {
   struct stat sbuf;

   if (stat(path.getBuffer(), &sbuf))
      return 0;

   return sbuf.st_mode;
}

int q_realpath(const QoreString& path, QoreString& rv, ExceptionSink* xsink) {
#ifdef HAVE_REALPATH
#if defined(SOLARIS) || (defined(__NetBSD_Version__) && (__NetBSD_Version__ < 601000000))
   char buf[PATH_MAX];
   char* p = realpath(path.getBuffer(), buf);
#else
   char* p = realpath(path.getBuffer(), 0);
#endif
   if (!p) {
      if (xsink)
	 xsink->raiseErrnoException("REALPATH-ERROR", errno, "error calling realpath()");
      return -1;
   }
#if defined(SOLARIS) || (defined(__NetBSD_Version__) && (__NetBSD_Version__ < 601000000))
   rv.set(buf);
#else
   rv.takeAndTerminate(p, strlen(p));
#endif
#else
#ifndef _Q_WINDOWS
#error must implement an alternative to realpath on UNIX systems
#endif
   if (&rv != &path)
      rv = path;
   q_normalize_path(rv);
   // verify that the path exists
   if (!q_get_mode(rv)) {
      if (xsink)
	 xsink->raiseException("REALPATH-ERROR", "path '%s' does not exist", rv.getBuffer());
      return -1;
   }
   return 0;
#endif
   return 0;
}

void qore_disable_gc() {
   q_disable_gc = true;
}

bool qore_is_gc_enabled() {
   return !q_disable_gc;
}

void qore_init_random_salt() {
   random_salt.sprintf(QLLD, q_clock_getmicros());
}

int qore_get_ptr_hash(QoreString& str, const void* ptr) {
   assert(!random_salt.empty());
   QoreString tmp(random_salt);
   tmp.sprintf("%p", ptr);
   DigestHelper dh(tmp);
   int rc = dh.doDigest(SHA1_ERR, EVP_sha1());
   if (!rc)
      dh.getString(str);
   else
      printd(0, "qore_get_ptr_hash() digest calculation failed for ptr %p\n", ptr);
   return rc;
}

void* q_memmem(const void* big, size_t big_len, const void* little, size_t little_len) {
   assert(big_len && little_len);
#ifdef HAVE_MEMMEM
   return memmem(big, big_len, little, little_len);
#else
   const char* lt = (const char*)little;
   const char* bg = (const char*)big;
   const char* p = bg;
   const char* end = bg + big_len;
   while (p < end) {
      const char* f = (const char*)memchr(p, lt[0], end - p);
      if (!f || ((bg - f) < (ptrdiff_t)little_len))
	 return 0;
      p = f;
      bool found = true;
      for (size_t i = 1; i < little_len; ++i) {
	 if (*f != lt[i]) {
	    found = false;
	    break;
	 }
	 ++f;
      }
      if (found)
	 return (void*)p;
      ++p;
   }
   return 0;
#endif
}

#ifdef _Q_WINDOWS
int statvfs(const char* path, struct statvfs* buf) {
   ULARGE_INTEGER avail;
   ULARGE_INTEGER total;
   ULARGE_INTEGER free;

   if (!GetDiskFreeSpaceEx(path, &avail, &total, &free)) {
      return -1;
   }

   buf->set(avail.QuadPart, total.QuadPart, free.QuadPart);
   return 0;
}

int q_fstatvfs(const char* filepath, struct statvfs* buf) {
   char* dir = q_dirname(filepath);
   if (!dir)
      return -1;
   ON_BLOCK_EXIT(free, dir);

   return statvfs(dir, buf);
}
#endif

// call to get a node with reference count 1 (copy on write)
void ensure_unique(AbstractQoreNode* *v, ExceptionSink* xsink) {
   assert(*v);
   if (!(*v)->is_unique()) {
      AbstractQoreNode* old = *v;
      (*v) = old->realCopy();
      old->deref(xsink);
      assert(!*xsink);
   }
}

// checks for illegal "self" assignments in an object context
void check_self_assignment(AbstractQoreNode* n, LocalVar* selfid) {
   qore_type_t ntype = n->getType();

   // if it's a variable reference
   if (ntype == NT_VARREF) {
      VarRefNode* v = reinterpret_cast<VarRefNode*>(n);
      if (v->getType() == VT_LOCAL && v->ref.id == selfid)
         parse_error("illegal assignment to 'self' in an object context");
      return;
   }
}

int check_lvalue_int(const QoreTypeInfo*& typeInfo, const char* name) {
   // make sure the lvalue can be assigned an integer value
   // raise a parse exception only if parse exceptions are not suppressed
   if (!typeInfo->parseAcceptsReturns(NT_INT)) {
      if (getProgram()->getParseExceptionSink()) {
	 QoreStringNode* desc = new QoreStringNode("lvalue has type ");
	 typeInfo->getThisType(*desc);
	 desc->sprintf(", but the %s will assign it an integer value", name);
	 qore_program_private::makeParseException(getProgram(), "PARSE-TYPE-ERROR", desc);
      }
      return -1;
   }
   return 0;
}

int check_lvalue_number(const QoreTypeInfo*& typeInfo, const char* name) {
   // make sure the lvalue can be assigned a floating-point value
   // raise a parse exception only if parse exceptions are not suppressed
   if (!typeInfo->parseAcceptsReturns(NT_NUMBER) && getProgram()->getParseExceptionSink()) {
      QoreStringNode* desc = new QoreStringNode("lvalue has type ");
      typeInfo->getThisType(*desc);
      desc->sprintf(", but the %s will assign it a number value", name);
      qore_program_private::makeParseException(getProgram(), "PARSE-TYPE-ERROR", desc);
      return -1;
   }
   return 0;
}

int check_lvalue_float(const QoreTypeInfo*& typeInfo, const char* name) {
   // make sure the lvalue can be assigned a floating-point value
   // raise a parse exception only if parse exceptions are not suppressed
   if (!typeInfo->parseAcceptsReturns(NT_FLOAT) && getProgram()->getParseExceptionSink()) {
      QoreStringNode* desc = new QoreStringNode("lvalue has type ");
      typeInfo->getThisType(*desc);
      desc->sprintf(", but the %s will assign it a float value", name);
      qore_program_private::makeParseException(getProgram(), "PARSE-TYPE-ERROR", desc);
      return -1;
   }
   return 0;
}

int check_lvalue_int_float_number(const QoreTypeInfo*& typeInfo, const char* name) {
   // make sure the lvalue can be assigned an integer value
   // raise a parse exception only if parse exceptions are not suppressed
   if (!typeInfo->parseAcceptsReturns(NT_INT)
         && !typeInfo->parseAcceptsReturns(NT_FLOAT)
         && !typeInfo->parseAcceptsReturns(NT_NUMBER)) {
      if (getProgram()->getParseExceptionSink()) {
         QoreStringNode* desc = new QoreStringNode("lvalue has type ");
         typeInfo->getThisType(*desc);
         desc->sprintf(", but the %s only works with integer, floating-point, or numeric lvalues", name);
         qore_program_private::makeParseException(getProgram(), "PARSE-TYPE-ERROR", desc);
      }
      return -1;
   }
   if (typeInfo->parseReturnsType(NT_INT)) {
      if (typeInfo->parseReturnsType(NT_FLOAT)) {
         if (typeInfo->parseReturnsType(NT_NUMBER))
            typeInfo = bigIntFloatOrNumberTypeInfo;
         else
            typeInfo = bigIntOrFloatTypeInfo;
      }
      else
         typeInfo = bigIntTypeInfo;
   }
   else {
      if (typeInfo->parseReturnsType(NT_FLOAT))
         if (typeInfo->parseReturnsType(NT_NUMBER))
            typeInfo = floatOrNumberTypeInfo;
         else
            typeInfo = floatTypeInfo;
      else
         typeInfo = numberTypeInfo;
   }

   return 0;
}

static void do_subst(QoreString& str, const char* i, const char* ep) {
   assert(i < ep);
   QoreString var(i + 1, ep - i - 1, str.getEncoding());
   QoreString val;
   SystemEnvironment::get(var.c_str(), val);

   str.replace(i - str.c_str(), ep - i, val.c_str());
}

static int do_bracket_subst(QoreString& str, const char* i, char c) {
   const char* ep = strchr(i + 1, c);
   if (!ep)
      return -1;

   do_subst(str, i, ep);
   return 0;
}

int q_env_subst(QoreString& str) {
   const char* i;
   while ((i = strchr(str.c_str(), '$'))) {
      const char* ep = i + 1;
      if (!*ep)
         return -1;
      if (*ep == '(') {
         if (do_bracket_subst(str, i, ')'))
            return -1;
         continue;
      }
      if (*ep == '{') {
         if (do_bracket_subst(str, i, '}'))
            return -1;
         continue;
      }
      while (*ep && (*ep == '_' || isalnum(*ep)))
         ++ep;
      do_subst(str, i, ep);
   }

   return 0;
}
