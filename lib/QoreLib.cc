/*
  QoreLib.cc

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

#include <qore/Qore.h>
#include <qore/intern/svn-revision.h>

#include <string.h>
#include <pwd.h>
#include <grp.h>

FeatureList qoreFeatureList;

#define cpp_str(s) #s
#define cpp_xstr(s) cpp_str(s)

// global library variables
const char *qore_version_string      = VERSION "-" cpp_xstr(BUILD);
int qore_version_major               = VERSION_MAJOR;
int qore_version_minor               = VERSION_MINOR;
int qore_version_sub                 = VERSION_SUB;
int qore_build_number                = BUILD;
int qore_target_bits                 = TARGET_BITS;
const char *qore_target_os           = TARGET_OS;
const char *qore_target_arch         = TARGET_ARCH;
const char *qore_module_dir          = MODULE_DIR;
const char *qore_cplusplus_compiler  = QORE_LIB_CXX;
const char *qore_cflags              = QORE_LIB_CFLAGS;
const char *qore_ldflags             = QORE_LIB_LDFLAGS;
const char *qore_build_host          = QORE_BUILD_HOST;

DLLLOCAL QoreListNode *ARGV = 0;
DLLLOCAL QoreListNode *QORE_ARGV = 0;

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
#ifdef HAVE_CHECK_STACK_POS
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
   { QORE_OPT_FUNC_ROUND,
     "HAVE_ROUND",
     QO_FUNCTION,
#ifdef HAVE_ROUND
     true
#else
     false
#endif
   },
   { QORE_OPT_FUNC_TIMEGM,
     "HAVE_TIMEGM",
     QO_FUNCTION,
#ifdef HAVE_TIMEGM
     true
#else
     false
#endif
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
   { QORE_OPT_FUNC_PARSEXMLWITHSCHEMA,
     "HAVE_PARSEXMLWITHSCHEMA",
     QO_FUNCTION,
#ifdef HAVE_XMLTEXTREADERSETSCHEMA
     true
#else
     false
#endif
   },
   { QORE_OPT_FUNC_PARSEXMLWITHRELAXNG,
     "HAVE_PARSEXMLWITHRELAXNG",
     QO_FUNCTION,
#ifdef HAVE_XMLTEXTREADERRELAXNGSETSCHEMA
     true
#else
     false
#endif
   },
};

const qore_option_s *qore_option_list = qore_option_list_l;

#define QORE_OPTION_LIST_SIZE (sizeof(qore_option_list_l) / sizeof(qore_option_s))

size_t qore_option_list_size = QORE_OPTION_LIST_SIZE;

static inline int get_number(char **param) {
   int i, num; 
   
   i = strspn((*param), "0123456789");
   num = atoi(*param);
   (*param) += i;
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

FeatureList::FeatureList() {
   // register default features
   push_back("sql");
   push_back("threads");
   push_back("xml");
#ifdef DEBUG
   push_back("debug");
#endif
}

FeatureList::~FeatureList() {
}

// if type = 0 then field widths are soft limits, otherwise they are hard
static int process_opt(QoreString *cstr, char *param, const AbstractQoreNode *node, int type, int *taken, ExceptionSink *xsink) {
   char *str = param;
   int opts = 0;
   int width = -1;
   int decimals = -1;
   int length;
   char fmt[20], *f;
   QoreString tbuf(cstr->getEncoding());

   printd(5, "process_opt(): param=%s type=%d node=%08p node->getType()=%s refs=%d\n",
	  param, type, node, node ? node->getTypeName() : "(null)", node ? node->reference_count() : -1);
#ifdef DEBUG
   if (node && node->getType() == NT_STRING) {
      const QoreStringNode *nstr = reinterpret_cast<const QoreStringNode *>(node);
      printd(5, "process_opt() %08p (%d) \"%s\"\n", nstr->getBuffer(), nstr->strlen(), nstr->getBuffer());
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
      decimals = 0;

   char p = *param;
   switch (*param) {
      case 's': {
	 QoreStringValueHelper astr(node);

	 length = astr->strlen();
	 if ((width != -1) && (length > width) && !type)
	    width = length;
	 if ((width != -1) && (length > width)) {
	    tbuf.concat(*astr, width, xsink); // string encodings are converted here if necessary
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
	 int64 val;
	 if (!node)
	    val = 0;
	 else
	    val = node->getAsBigInt();
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
	 *(f++) = 'l';
	 *(f++) = 'l';
	 *(f++) = p; // 'd', etc;
	 *f = '\0';
	 tbuf.sprintf(fmt, val);
	 if (type && (width != -1))
	    tbuf.terminate(width);
	 break;
      }
      case 'f':
      case 'e': {
	 double val;
	 if (!node)
	    val = 0.0;
	 else 
	    val = node->getAsFloat();
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
	 *(f++) = *param; // 'f';
	 *f = '\0';
	 tbuf.sprintf(fmt, val);
	 if (type && (width != -1))
	    tbuf.terminate(width);
	 break;
      }
      case 'n':
      case 'N': {
	 QoreNodeAsStringHelper t(node, *param == 'N' 
				  ? (width == -1 ? FMT_NORMAL : width) 
				  : FMT_NONE, xsink);
	 tbuf.concat(*t);
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

QoreStringNode *q_sprintf(const QoreListNode *params, int field, int offset, ExceptionSink *xsink) {
   unsigned i, j, l;
   const QoreStringNode *p;

   if (!(p = test_string_param(params, offset)))
      return new QoreStringNode();

   SimpleRefHolder<QoreStringNode> buf(new QoreStringNode(p->getEncoding()));

   j = 1 + offset;

   const char *pstr = p->getBuffer();
   l = strlen(pstr);
   for (i = 0; i < l; i++) {
      int taken = 1;
      if ((pstr[i] == '%') && (j < params->size())) {
	 const AbstractQoreNode *node = get_param(params, j++);
	 i += process_opt(*buf, (char *)&pstr[i], node, field, &taken, xsink);	 
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

QoreStringNode *q_vsprintf(const QoreListNode *params, int field, int offset, ExceptionSink *xsink) {
   const QoreStringNode *fmt;
   const AbstractQoreNode *args;

   if (!(fmt = test_string_param(params, offset)))
      return new QoreStringNode();

   args = get_param(params, offset + 1);
   const QoreListNode *arg_list = dynamic_cast<const QoreListNode *>(args);

   SimpleRefHolder<QoreStringNode> buf(new QoreStringNode(fmt->getEncoding()));
   unsigned j = 0;
   unsigned l = fmt->strlen();
   for (unsigned i = 0; i < l; i++) {
      int taken = 1;
      bool havearg = false;
      const AbstractQoreNode *arg = 0;

      if ((fmt->getBuffer()[i] == '%')) {
	 if (args) {
	    if (arg_list && j < arg_list->size()) {
	       havearg = true;
	       arg = get_param(arg_list, j);
	    }
	    else if (!j) {
	       arg = args;
	       havearg = true;
	    }
	    j++;
	 }
      }
      if (havearg) {
	 i += process_opt(*buf, (char *)&fmt->getBuffer()[i], arg, field, &taken, xsink);
	 if (*xsink)
	    return 0;
	 if (!taken)
	    j--;
      }
      else
	 buf->concat(fmt->getBuffer()[i]);
   }
   return buf.release();
}

static void concatASCII(QoreString &str, unsigned char c) {
   str.sprintf("ascii %03d", c);
   if (c >= 32 || c < 127)
      str.sprintf(" ('%c')", c);
}

static inline char getBase64Value(const char *buf, qore_size_t &offset, bool end_ok, ExceptionSink *xsink) {
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
      QoreStringNode *desc = new QoreStringNode;
      concatASCII(*desc, c);
      desc->concat(" is an invalid base64 character");
      xsink->raiseException("BASE64-PARSE-ERROR", desc);
   }
   return -1;
}

// see: RFC-1421: http://www.ietf.org/rfc/rfc1421.txt and RFC-2045: http://www.ietf.org/rfc/rfc2045.txt
BinaryNode *parseBase64(const char *buf, int len, ExceptionSink *xsink) {
   char *binbuf = (char *)malloc(sizeof(char) * (len + 3));
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

int get_nibble(char c, ExceptionSink *xsink) {
   if (isdigit(c))
      return c - 48;
   if (c >= 'A' && c <= 'F')
      return c - 55;
   if (c >= 'a' && c <= 'f')
      return c - 87;

   xsink->raiseException("PARSE-HEX-ERROR", "invalid hex digit found '%c'", c);
   return -1;
}

BinaryNode *parseHex(const char *buf, int len, ExceptionSink *xsink) {
   if (!len)
      return new BinaryNode();

   if ((len / 2) * 2 != len) {
      xsink->raiseException("PARSE-HEX-ERROR", "cannot parse an odd number of hex digits (%d digit%s)", len, len == 1 ? "" : "s");
      return 0;
   }

   char *binbuf = (char *)malloc(sizeof(char) * (len / 2));
   int blen = 0;

   const char *end = buf + len;
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
BinaryNode *parseHex(const char *buf, int len) {
   if (!buf || !(*buf))
      return new BinaryNode();

   char *binbuf = (char *)malloc(sizeof(char) * (len / 2));
   int blen = 0;

   const char *end = buf + len;
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

char *make_class_name(const char *str) {
   char *cn = q_basename(str);
   char *p = strrchr(cn, '.');
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

void print_node(FILE *fp, const AbstractQoreNode *node) {
   printd(5, "print_node() node=%08p (%s)\n", node, node ? node->getTypeName() : "(null)");
   QoreStringValueHelper str(node);
   fputs(str->getBuffer(), fp);
}

void qore_setup_argv(int pos, int argc, char *argv[]) {
   ARGV = new QoreListNode();
   QORE_ARGV = new QoreListNode();
   int end = argc - pos;
   for (int i = 0; i < argc; i++) {
      if (i < end)
	 ARGV->push(new QoreStringNode(argv[i + pos]));
      QORE_ARGV->push(new QoreStringNode(argv[i]));
   }
}

void init_lib_intern(char *env[]) {
   // set up environment hash
   int i = 0;
   ENV = new QoreHashNode();
   while (env[i]) {
      char *p;

      if ((p = strchr(env[i], '='))) {
	 char save = *p;
	 *p = '\0';
	 ENV->setKeyValue(env[i], new QoreStringNode(p + 1), 0);
	 //printd(5, "creating $ENV{\"%s\"} = \"%s\"\n", env[i], p + 1);
	 *p = save;
      }
      i++;
   }

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

struct tm *q_localtime(const time_t *clock, struct tm *tms) {
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

struct tm *q_gmtime(const time_t *clock, struct tm *tms) {
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

// thread-safe basename function (resulting pointer must be free()ed)
char *q_basename(const char *path) {
   const char *p = strrchr(path, '/');
   if (!p)
      return strdup(path);
   return strdup(p + 1);
}

// returns a pointer within the same string
char *q_basenameptr(const char *path) {
   const char *p = strrchr(path, '/');
   if (!p)
      return (char *)path;
   return (char *)p + 1;   
}

// thread-safe basename function (resulting pointer must be free()ed)
char *q_dirname(const char *path) {
   const char *p = strrchr(path, '/');
   if (!p || p == path) {
      char *x = (char *)malloc(sizeof(char) * 2);
      x[0] = !p ? '.' : '/';
      x[1] = '\0';
      return x;
   }
   char *x = (char *)malloc(sizeof(char) * (p - path + 1));
   strncpy(x, path, p - path);
   x[p - path] = '\0';
   return x;
}

void *q_realloc(void *ptr, size_t size) {
   void *p = realloc(ptr, size);
   if (!p)
      free(ptr);
   return p;
}

static inline void assign_hv(QoreHashNode *h, const char *key, char *val) {
   h->setKeyValue(key, new QoreStringNode(val), 0);
}

static inline void assign_hv(QoreHashNode *h, const char *key, int val) {
   h->setKeyValue(key, new QoreBigIntNode(val), 0);
}

static QoreHashNode *pwd2hash(const struct passwd &pw) {
   QoreHashNode *h = new QoreHashNode;
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

QoreHashNode *q_getpwuid(uid_t uid) {
   struct passwd *pw;
#ifdef HAVE_GETPWUID_R
   struct passwd pw_rec;
   char *buf = (char *)malloc(pwsize * sizeof(char));
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

QoreHashNode *q_getpwnam(const char *name) {
   struct passwd *pw;
#ifdef HAVE_GETPWNAM_R
   struct passwd pw_rec;
   char *buf = (char *)malloc(pwsize * sizeof(char));
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

int q_uname2uid(const char *name, uid_t &uid) {
   struct passwd *pw;
#ifdef HAVE_GETPWNAM_R
   struct passwd pw_rec;
   char *buf = (char *)malloc(pwsize * sizeof(char));
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

static QoreHashNode *gr2hash(struct group &gr) {
   QoreHashNode *h = new QoreHashNode;
   // assign values
   assign_hv(h, "gr_name", gr.gr_name);
   assign_hv(h, "gr_passwd", gr.gr_passwd);
   assign_hv(h, "gr_gid", gr.gr_gid);

   QoreListNode *l = new QoreListNode;
   for (char **p = gr.gr_mem; *p; ++p)
      l->push(new QoreStringNode(*p));

   h->setKeyValue("gr_mem", l, 0);
   return h;
}

QoreHashNode *q_getgrgid(gid_t gid) {
   struct group *gr;
#ifdef HAVE_GETGRGID_R
   struct group gr_rec;
   char *buf = (char *)malloc(grsize * sizeof(char));
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

QoreHashNode *q_getgrnam(const char *name) {
   struct group *gr;
#ifdef HAVE_GETGRNAM_R
   struct group gr_rec;
   char *buf = (char *)malloc(grsize * sizeof(char));
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

int q_gname2gid(const char *name, gid_t &gid) {
   struct group *gr;
#ifdef HAVE_GETGRNAM_R
   struct group gr_rec;
   char *buf = (char *)malloc(grsize * sizeof(char));
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

ResolvedCallReferenceNode *getCallReference(const QoreString *str, ExceptionSink *xsink) {
   QoreProgram *pgm = getProgram();
   UserFunction *f = pgm->findUserFunction(str->getBuffer());
   if (!f) {
      xsink->raiseException("NO-SUCH-FUNCTION", "callback function '%s()' does not exist", str->getBuffer());
      return 0;
   }
   return new UserCallReferenceNode(f, pgm);
}

qore_license_t qore_get_license() {
   return qore_license;
}

long long q_atoll(const char *str) {
   return atoll(str);
}

