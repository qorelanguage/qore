/*
  QoreLib.cc

  Qore Programming Language

  Copyright (C) 2005 David Nichols

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
#include <qore/QoreLib.h>
#include <qore/StringList.h>

#include <string.h>

class FeatureList qoreFeatureList;

// global library variables
char qore_version_string[] = VERSION_STRING;
int qore_version_major     = VERSION_MAJOR;
int qore_version_minor     = VERSION_MINOR;
int qore_version_sub       = VERSION_SUB;
int qore_build_number      = BUILD;
int qore_target_bits       = TARGET_BITS;
char qore_target_os[]      = TARGET_OS;
char qore_target_arch[]    = TARGET_ARCH;

DLLLOCAL QoreListNode *ARGV = NULL;
DLLLOCAL QoreListNode *QORE_ARGV = NULL;

#ifndef HAVE_LOCALTIME_R
DLLLOCAL class LockedObject lck_localtime;
#endif

#ifndef HAVE_GMTIME_R
DLLLOCAL class LockedObject lck_gmtime;
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

static inline int get_number(char **param)
{
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

FeatureList::FeatureList()
{
   // register default features
   push_back("sql");
   push_back("threads");
   push_back("xml");
#ifdef DEBUG
   push_back("debug");
#endif
#ifdef QORE_MONOLITHIC
# ifdef NCURSES
   push_back("ncurses");
# endif
# ifdef ORACLE
   push_back("oracle");
# endif
# ifdef QORE_MYSQL
   push_back("mysql");
# endif
# ifdef TIBRV
   push_back("tibrv");
# endif
# ifdef TIBAE
   push_back("tibae");
# endif
# ifdef TUXEDO
   push_back("tuxedo");
# endif
# ifdef SYBASE
   push_back("sybase");
# endif
# ifdef FREETDS
   push_back("freetds");
# endif
#endif
}

FeatureList::~FeatureList()
{
}

// if type = 0 then field widths are soft limits, otherwise they are hard
static int process_opt(QoreString *cstr, char *param, const AbstractQoreNode *node, int type, int *taken, ExceptionSink *xsink)
{
   char *str = param;
   int opts = 0;
   int width = -1;
   int decimals = -1;
   int length;
   char fmt[20], *f;
   QoreString tbuf(cstr->getEncoding());

   printd(5, "process_opt(): param=%s type=%d node=%08p node->type=%s refs=%d\n",
	  param, type, node, node ? node->getTypeName() : "(null)", node ? node->reference_count() : -1);
#ifdef DEBUG
   if (node && node->type == NT_STRING) {
      const QoreStringNode *str = reinterpret_cast<const QoreStringNode *>(node);
      printd(5, "process_opt() %08p (%d) \"%s\"\n", str->getBuffer(), str->strlen(), str->getBuffer());
   }
#endif
  loop:
   switch (*(++param))
   {
      case '-': opts |= P_JUSTIFY_LEFT; goto loop;
      case '+': opts |= P_INCLUDE_PLUS; goto loop;
      case ' ': opts |= P_SPACE_FILL; opts &= ~P_ZERO_FILL; goto loop;
      case '0': opts |= P_ZERO_FILL; opts &= ~P_SPACE_FILL; goto loop;
   }
   if (isdigit(*param))
      width = get_number(&param);
   if ((*param) == '.')
   {
      param++;
      decimals = get_number(&param);
   }
   if (decimals < 0)
      decimals = 0;

   char p = *param;
   switch (*param)
   {
      case 's': {
	 QoreStringValueHelper astr(node);

	 length = astr->strlen();
	 if ((width != -1) && (length > width) && !type)
	    width = length;
	 if ((width != -1) && (length > width))
	 {
	    tbuf.concat(*astr, width, xsink); // string encodings are converted here if necessary
	 }
	 else
	 {
	    if ((width != -1) && (opts & P_JUSTIFY_LEFT))
	    {
	       tbuf.concat(*astr, xsink);
	       while (width > length)
	       {
		  tbuf.concat(' ');
		  width--;
	       }
	    }
	    else
	    {
	       while (width > length)
	       {
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
      case 'X':
      {
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
	 if (width != -1)
	 {
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
      case 'e':
      {
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
	 if (width != -1)
	 {
	    if (opts & P_SPACE_FILL)
	       *(f++) = ' ';
	    else if (opts & P_ZERO_FILL)
	       *(f++) = '0';
	    f += sprintf(f, "%d", width);
	 }
	 if (decimals != -1)
	 {
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
      case 'N':
      {
	 QoreNodeAsStringHelper t(node, *param == 'N' 
				  ? (width == -1 ? FMT_NORMAL : width) 
				  : FMT_NONE, xsink);
	 tbuf.concat(*t);
	 break;
      }
      default:
	 // if the format argument is not understood, then print it in its entirety
	 tbuf.concat('%');
	 tbuf.concat(*param);
	 *taken = 0;
   }

   cstr->concat(&tbuf, xsink);
   return (int)(param - str);
}

QoreStringNode *q_sprintf(const QoreListNode *params, int field, int offset, ExceptionSink *xsink)
{
   int i, j, l;
   const QoreStringNode *p;

   if (!(p = test_string_param(params, offset)))
      return new QoreStringNode();

   QoreStringNode *buf = new QoreStringNode(p->getEncoding());

   j = 1 + offset;
   l = strlen(p->getBuffer());
   for (i = 0; i < l; i++)
   {
      int taken = 1;
      if ((p->getBuffer()[i] == '%') 
	  && (j < params->size()))
      {
	 i += process_opt(buf, (char *)&p->getBuffer()[i], get_param(params, j++), field, &taken, xsink);
	 if (!taken)
	    j--;
      }
      else
	 buf->concat(p->getBuffer()[i]);
   }

   return buf;
}

QoreStringNode *q_vsprintf(const QoreListNode *params, int field, int offset, ExceptionSink *xsink)
{
   const QoreStringNode *fmt;
   const AbstractQoreNode *args;

   if (!(fmt = test_string_param(params, offset)))
      return new QoreStringNode();

   args = get_param(params, offset + 1);
   const QoreListNode *arg_list = dynamic_cast<const QoreListNode *>(args);

   QoreStringNode *buf = new QoreStringNode(fmt->getEncoding());
   int j = 0;
   int l = fmt->strlen();
   for (int i = 0; i < l; i++)
   {
      int taken = 1;
      bool havearg = false;
      const AbstractQoreNode *arg = NULL;

      if ((fmt->getBuffer()[i] == '%'))
      {
	 if (args)
	 {
	    if (arg_list && j < arg_list->size()) {
	       havearg = true;
	       arg = get_param(arg_list, j);
	    }
	    else if (!j)
	    {
	       arg = args;
	       havearg = true;
	    }
	    j++;
	 }
      }
      if (havearg)
      {
	 i += process_opt(buf, (char *)&fmt->getBuffer()[i], 
			  arg, field, &taken, xsink);
	 if (!taken)
	    j--;
      }
      else
	 buf->concat(fmt->getBuffer()[i]);
   }
   return buf;
}

// FIXME: SLOW! make a lookup table for characters to value
static inline char getBase64Value(char c, ExceptionSink *xsink)
{
   for (int i = 0; i < 64; i++)
      if (table64[i] == c)
	 return i;
   xsink->raiseException("BASE64-PARSE-ERROR", "'%c' is an invalid base64 character.", c);
   return -1;
}

class BinaryNode *parseBase64(const char *buf, int len, ExceptionSink *xsink)
{
   char *binbuf = (char *)malloc(sizeof(char) * (len + 3));
   int blen = 0;

   int pos = 0;
   while (pos < len)
   {
      // add first 6 bits
      char b = getBase64Value(buf[pos], xsink);
      if (xsink->isEvent())
      {
         free(binbuf);
         return NULL;
      }
      // get second 6 bits
      char c = getBase64Value(buf[pos + 1], xsink);
      if (xsink->isEvent())
      {
         free(binbuf);
         return NULL;
      }
      // do first byte (1st char=upper 6 bits, 2nd char=lower 2)
      binbuf[blen++] = (b << 2) | (c >> 4);

      // check special cases
      if (buf[pos + 2] == '=')
         break;

      // low 4 bits from 2nd char become high 4 bits of next value
      b = (c & 15) << 4;
      
      // get third 6 bits
      c = getBase64Value(buf[pos + 2], xsink);
      if (xsink->isEvent())
      {
         free(binbuf);
         return NULL;
      }
      // do second byte (2nd char=upper 4 bits, 3rd char=lower 4 bits)
      binbuf[blen++] = b | (c >> 2);

      // check special cases
      if (buf[pos + 3] == '=')
         break;

      // low 2 bits from 3rd char become high 2 bits of next value
      b = (c & 3) << 6;

      // get fourth 6 bits
      c = getBase64Value(buf[pos + 3], xsink);
      if (xsink->isEvent())
      {
         free(binbuf);
         return NULL;
      }

      binbuf[blen++] = b | c;
      pos += 4;
   }
   return new BinaryNode(binbuf, blen);
}

int get_nibble(char c, ExceptionSink *xsink)
{
   if (isdigit(c))
      return c - 48;
   if (c >= 'A' && c <= 'F')
      return c - 55;
   if (c >= 'a' && c <= 'f')
      return c - 87;

   xsink->raiseException("PARSE-HEX-ERROR", "invalid hex digit found '%c'", c);
   return -1;
}

class BinaryNode *parseHex(const char *buf, int len, ExceptionSink *xsink)
{
   if (!len)
      return new BinaryNode();

   if ((len / 2) * 2 != len)
   {
      xsink->raiseException("PARSE-HEX-ERROR", "cannot parse an odd number of hex digits (%d digit%s)", len, len == 1 ? "" : "s");
      return NULL;
   }

   char *binbuf = (char *)malloc(sizeof(char) * (len / 2));
   int blen = 0;

   const char *end = buf + len;
   while (buf < end)
   {
      int b = get_nibble(*buf, xsink);
      if (b < 0)
      {
	 free(binbuf);
	 return NULL;
      }
      buf++;
      int l = get_nibble(*buf, xsink);
      if (l < 0)
      {
	 free(binbuf);
	 return NULL;
      }
      buf++;
      binbuf[blen++] = b << 4 | l;
   }
   return new BinaryNode(binbuf, blen);
}

static inline int parse_get_nibble(char c)
{
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
class BinaryNode *parseHex(const char *buf, int len)
{
   if (!buf || !(*buf))
      return new BinaryNode();

   char *binbuf = (char *)malloc(sizeof(char) * (len / 2));
   int blen = 0;

   const char *end = buf + len;
   while (buf < end)
   {
      int b = parse_get_nibble(*buf);
      if (b < 0)
      {
	 free(binbuf);
	 return NULL;
      }
      buf++;
#if 0
      // this can never happen; the parser guarantees an even number of digits
      if (!(*buf))
      {
	 free(binbuf);
	 parseError("PARSE-HEX-ERROR", "cannot parse an odd number of hex digits (%d digit%s)", len, len == 1 ? "" : "s");
	 return NULL;
      }
#endif

      int l = parse_get_nibble(*buf);
      if (l < 0)
      {
	 free(binbuf);
	 return NULL;
      }
      buf++;
      binbuf[blen++] = b << 4 | l;
   }
   return new BinaryNode(binbuf, blen);
}

char *make_class_name(const char *str)
{
   char *cn = q_basename(str);
   char *p = strrchr(cn, '.');
   if (p && p != cn)
      *p = '\0';
   p = cn;
   while (*p)
   {
      if (*p == '-')
	 *p = '_';
      p++;
   }
   return cn;
}

void print_node(FILE *fp, const AbstractQoreNode *node)
{
   printd(5, "print_node() node=%08p (%s)\n", node, node ? node->getTypeName() : "(null)");
   QoreStringValueHelper str(node);
   fputs(str->getBuffer(), fp);
}

void qore_setup_argv(int pos, int argc, char *argv[])
{
   ARGV = new QoreListNode();
   QORE_ARGV = new QoreListNode();
   int end = argc - pos;
   for (int i = 0; i < argc; i++)
   {
      if (i < end)
	 ARGV->push(new QoreStringNode(argv[i + pos]));
      QORE_ARGV->push(new QoreStringNode(argv[i]));
   }
}

void initENV(char *env[])
{
   // set up environment hash
   int i = 0;
   ENV = new QoreHashNode();
   while (env[i])
   {
      char *p;

      if ((p = strchr(env[i], '=')))
      {
	 char save = *p;
	 *p = '\0';
	 ENV->setKeyValue(env[i], new QoreStringNode(p + 1), NULL);
	 //printd(5, "creating $ENV{\"%s\"} = \"%s\"\n", env[i], p + 1);
	 *p = save;
      }
      i++;
   }
   //traceout("initProgramGlobalVars()");
}

void delete_global_variables()
{
   tracein("delete_global_variables()");
   if (QORE_ARGV)
      QORE_ARGV->deref(0);
   if (ARGV)
      ARGV->deref(0);
   if (ENV)
      ENV->deref(0);
   traceout("delete_global_variables()");
}

struct tm *q_localtime(const time_t *clock, struct tm *tms)
{
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

struct tm *q_gmtime(const time_t *clock, struct tm *tms)
{
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
char *q_basename(const char *path)
{
   const char *p = strrchr(path, '/');
   if (!p)
      return strdup(path);
   return strdup(p + 1);
}

// returns a pointer within the same string
char *q_basenameptr(const char *path)
{
   const char *p = strrchr(path, '/');
   if (!p)
      return (char *)path;
   return (char *)p + 1;   
}

// thread-safe basename function (resulting pointer must be free()ed)
char *q_dirname(const char *path)
{
   const char *p = strrchr(path, '/');
   if (!p || p == path)
   {
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

ResolvedFunctionReferenceNode *getFunctionReference(const QoreString *str, ExceptionSink *xsink)
{
   QoreProgram *pgm = getProgram();
   UserFunction *f = pgm->findUserFunction(str->getBuffer());
   if (!f)
   {
      xsink->raiseException("NO-SUCH-FUNCTION", "callback function '%s()' does not exist", str->getBuffer());
      return 0;
   }
   return new UserFunctionReferenceNode(f, pgm);
}
