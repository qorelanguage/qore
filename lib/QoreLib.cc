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

DLLEXPORT class featureList qoreFeatureList;

// global library variables
DLLEXPORT char qore_version_string[] = VERSION_STRING;
DLLEXPORT int qore_version_major     = VERSION_MAJOR;
DLLEXPORT int qore_version_minor     = VERSION_MINOR;
DLLEXPORT int qore_version_sub       = VERSION_SUB;
DLLEXPORT int qore_build_number      = BUILD;
DLLEXPORT int qore_target_bits       = TARGET_BITS;
DLLEXPORT char qore_target_os[]      = TARGET_OS;
DLLEXPORT char qore_target_arch[]    = TARGET_ARCH;

DLLLOCAL class List *ARGV = NULL;
DLLLOCAL class List *QORE_ARGV = NULL;

#ifndef HAVE_LOCALTIME_R
DLLLOCAL class LockedObject lck_localtime;
#endif

#ifndef HAVE_GMTIME_R
DLLLOCAL class LockedObject lck_gmtime;
#endif

DLLLOCAL char table64[64] = {
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

featureList::featureList()
{
   // register default features
   push_back(strdup("sql"));
   push_back(strdup("threads"));
   push_back(strdup("xml"));
#ifdef QORE_DEBUG
   push_back(strdup("debug"));
#endif
#ifdef QORE_MONOLITHIC
# ifdef NCURSES
   push_back(strdup("ncurses"));
# endif
# ifdef ORACLE
   push_back(strdup("oracle"));
# endif
# ifdef QORE_MYSQL
   push_back(strdup("mysql"));
# endif
# ifdef TIBRV
   push_back(strdup("tibrv"));
# endif
# ifdef TIBAE
   push_back(strdup("tibae"));
# endif
#endif
}

featureList::~featureList()
{
   featureList::iterator i;
   while ((i = begin()) != end())
   {
      free(*i);
      erase(i);
   }
}

// if type = 0 then field widths are soft limits, otherwise they are hard
static int process_opt(QoreString *cstr, char *param, class QoreNode *node, int type, int *taken, class ExceptionSink *xsink)
{
   char *str = param;
   int opts = 0;
   int width = -1;
   int decimals = -1;
   class QoreNode *arg = node;
   int length;
   char fmt[20], *f;
   QoreString tbuf(cstr->getEncoding());

   printd(3, "process_opt(): param=%s type=%d node=%08p node->type=%s refs=%d\n",
	  param, type, node, node ? node->type->getName() : "(null)", node ? node->reference_count() : -1);
   if (node && node->type == NT_STRING)
      printd(4, "process_opt() %08p (%d) \"%s\"\n",
	     node->val.String->getBuffer(), node->val.String->strlen(), node->val.String->getBuffer());
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
      case 's':
	 if (!node)
	    arg = null_string();
	 else if (node->type != NT_STRING)
	    arg = node->convert(NT_STRING);
	 length = arg->val.String->strlen();
	 if ((width != -1) && (length > width) && !type)
	    width = length;
	 if ((width != -1) && (length > width))
	 {
	    tbuf.concat(arg->val.String, width, xsink);
	 }
	 else
	 {
	    if ((width != -1) && (opts & P_JUSTIFY_LEFT))
	    {
	       tbuf.concat(arg->val.String, xsink);
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
	       tbuf.concat(arg->val.String, xsink);
	    }
	 }
	 break;
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
	 // printd(5, "int arg=%lld\n", arg->val.intval);
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
	 QoreString *t = node->getAsString(*param == 'N' 
					   ? (width == -1 ? FMT_NORMAL : width) 
					   : FMT_NONE, xsink);
	 tbuf.concat(t);
	 delete t;
	 break;
      }
      default:
	 // if the format argument is not understood, then print it in its entirety
	 tbuf.concat('%');
	 tbuf.concat(*param);
	 *taken = 0;
   }
   if (arg != node)
      arg->deref(NULL);

   cstr->concat(&tbuf, xsink);
   return (int)(param - str);
}

class QoreString *q_sprintf(class QoreNode *params, int field, int offset, class ExceptionSink *xsink)
{
   int i, j, l;
   QoreNode *p;

   if (!(p = test_param(params, NT_STRING, offset)))
      return new QoreString();

   QoreString *buf = new QoreString(p->val.String->getEncoding());

   j = 1 + offset;
   l = strlen(p->val.String->getBuffer());
   for (i = 0; i < l; i++)
   {
      int taken = 1;
      if ((p->val.String->getBuffer()[i] == '%') 
	  && (j < params->val.list->size()))
      {
	 i += process_opt(buf, &p->val.String->getBuffer()[i], 
			  get_param(params, j++), field, &taken, xsink);
	 if (!taken)
	    j--;
      }
      else
	 buf->concat(p->val.String->getBuffer()[i]);
   }

   return buf;
}

class QoreString *q_vsprintf(class QoreNode *params, int field, int offset, class ExceptionSink *xsink)
{
   class QoreNode *fmt, *args;

   if (!(fmt = test_param(params, NT_STRING, offset)))
      return new QoreString();

   args = get_param(params, offset + 1);

   QoreString *buf = new QoreString(fmt->val.String->getEncoding());
   int j = 0;
   int l = fmt->val.String->strlen();
   for (int i = 0; i < l; i++)
   {
      int taken = 1;
      bool havearg = false;
      class QoreNode *arg = NULL;

      if ((fmt->val.String->getBuffer()[i] == '%'))
      {
	 if (args)
	 {
	    if (args->type == NT_LIST
		&& j < args->val.list->size())
	    {
	       havearg = true;
	       arg = get_param(args, j);
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
	 i += process_opt(buf, &fmt->val.String->getBuffer()[i], 
			  arg, field, &taken, xsink);
	 if (!taken)
	    j--;
      }
      else
	 buf->concat(fmt->val.String->getBuffer()[i]);
   }
   return buf;
}

// FIXME: SLOW! make a lookup table for characters to value
static inline char getBase64Value(char c, class ExceptionSink *xsink)
{
   for (int i = 0; i < 64; i++)
      if (table64[i] == c)
	 return i;
   xsink->raiseException("BASE64-PARSE-ERROR", "'%c' is an invalid base64 character.", c);
   return -1;
}

class BinaryObject *parseBase64(char *buf, int len, class ExceptionSink *xsink)
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
   return new BinaryObject(binbuf, blen);
}

int get_nibble(char c, class ExceptionSink *xsink)
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

class BinaryObject *parseHex(char *buf, int len, class ExceptionSink *xsink)
{
   if (!len)
      return new BinaryObject();

   if ((len / 2) * 2 != len)
   {
      xsink->raiseException("PARSE-HEX-ERROR", "cannot parse an odd number of hex digits (%d digit%s)", len, len == 1 ? "" : "s");
      return NULL;
   }

   char *binbuf = (char *)malloc(sizeof(char) * (len / 2));
   int blen = 0;

   char *end = buf + len;
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
   return new BinaryObject(binbuf, blen);
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
class BinaryObject *parseHex(char *buf, int len)
{
   if (!buf || !(*buf))
      return new BinaryObject();

   char *binbuf = (char *)malloc(sizeof(char) * (len / 2));
   int blen = 0;

   char *end = buf + len;
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
   return new BinaryObject(binbuf, blen);
}

char *make_class_name(char *str)
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

void print_node(FILE *fp, class QoreNode *node)
{
   class QoreNode *n_node;

   printd(5, "print_node() node=%08p (%s)\n", node, node ? node->type->getName() : "(null)");
   if (!node)
      return;
   if (node->type != NT_STRING)
   {
      n_node = node->convert(NT_STRING);
      fputs(n_node->val.String->getBuffer(), fp);
      n_node->deref(NULL);
      return;
   }
   fputs(node->val.String->getBuffer(), fp);
}

void qore_setup_argv(int pos, int argc, char *argv[])
{
   ARGV = new List();
   QORE_ARGV = new List();
   int end = argc - pos;
   for (int i = 0; i < argc; i++)
   {
      if (i < end)
	 ARGV->push(new QoreNode(argv[i + pos]));
      QORE_ARGV->push(new QoreNode(argv[i]));
   }
}

void initENV(char *env[])
{
   // set up environment hash
   int i = 0;
   ENV = new Hash();
   while (env[i])
   {
      char *p;

      if ((p = strchr(env[i], '=')))
      {
	 char save = *p;
	 *p = '\0';
	 ENV->setKeyValue(env[i], new QoreNode(p + 1), NULL);
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
      QORE_ARGV->derefAndDelete(NULL);
   if (ARGV)
      ARGV->derefAndDelete(NULL);
   if (ENV)
      ENV->derefAndDelete(NULL);
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

