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

#include <qore/config.h>
#include <qore/Qore.h>
#include <qore/QoreLib.h>
#include <qore/LockedObject.h>
#include <qore/List.h>
#include <qore/params.h>
#include <qore/StringList.h>

class featureList qoreFeatureList;

// global library variables
char qore_version_string[] = VERSION_STRING;
int qore_version_major     = VERSION_MAJOR;
int qore_version_minor     = VERSION_MINOR;
int qore_version_sub       = VERSION_SUB;
int qore_build_number      = BUILD;
char qore_target_os[]      = TARGET_OS;
char qore_target_arch[]    = TARGET_ARCH;

#ifndef HAVE_LOCALTIME_R
class LockedObject lck_localtime;
#endif

#ifndef HAVE_GMTIME_R
class LockedObject lck_gmtime;
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
	  param, type, node, node ? node->type->name : "(null)", node ? node->reference_count() : -1);
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

#ifdef DEBUG
static inline void strindent(QoreString *s, int indent)
{
   for (int i = 0; i < indent; i++)
      s->concat(' ');
}

class QoreString *dni(class QoreString *s, class QoreNode *n, int indent, class ExceptionSink *xsink)
{
   tracein("dni()");
   if (!n)
   {
      s->concat("node=NULL\n");
      traceout("dni()");
      return s;
   }

   s->sprintf("node=%08p refs=%d type=%s ", n, n->reference_count(), n->type->name);

   if (n->type == NT_BOOLEAN)
      s->sprintf("val=%s\n", n->val.boolval ? "True" : "False");
   
   else if (n->type == NT_INT)
      s->sprintf("val=%lld\n", n->val.intval);

   else if (n->type == NT_NOTHING)
      s->sprintf("val=NOTHING\n");

   else if (n->type == NT_NULL)
      s->sprintf("val=SQL NULL\n");
      
   else if (n->type == NT_FLOAT)
      s->sprintf("val=%f\n", n->val.floatval);

   else if (n->type == NT_STRING)
      s->sprintf("val=(enc=%s, %d:%d) \"%s\"\n", 
                 n->val.String->getEncoding()->code,
                 n->val.String->length(),
                 n->val.String->strlen(),
                 n->val.String->getBuffer());

   else if (n->type ==  NT_LIST)
   {
      s->sprintf("elements=%d\n", n->val.list->size());
      for (int i = 0; i < n->val.list->size(); i++)
      {
         strindent(s, indent);
         s->sprintf("list element %d/%d: ", i, n->val.list->size());
         dni(s, n->val.list->retrieve_entry(i), indent + 3, xsink);
      }
   }

   else if (n->type == NT_OBJECT)
   {
      s->sprintf("elements=%d (type=%s)\n", n->val.object->size(),
                 n->val.object->getClass() ? n->val.object->getClass()->name : "<none>");
      {
         List *l = n->val.object->getMemberList(xsink);
         if (l)
         {
            for (int i = 0; i < l->size(); i++)
            {
               strindent(s, indent);
               s->sprintf("key %d/%d \"%s\" = ", i, l->size(), l->retrieve_entry(i)->val.String->getBuffer());
               QoreNode *nn;
               dni(s, nn = n->val.object->evalMemberNoMethod(l->retrieve_entry(i)->val.String->getBuffer(), xsink), indent + 3, xsink);
               discard(nn, xsink);
            }
            delete l;
         }
      }
   }

   else if (n->type == NT_HASH)
   {
      s->sprintf("elements=%d\n", n->val.hash->size());
      {
         int i = 0;
         HashIterator hi(n->val.hash);
         while (hi.next())
         {
            strindent(s, indent);
            s->sprintf("key %d/%d \"%s\" = ", i++, n->val.hash->size(), hi.getKey());
            dni(s, hi.getValue(), indent + 3, xsink);
         }
      }
   }

   else if (n->type == NT_DATE)
      s->sprintf("%04d-%02d-%02d %02d:%02d:%02d.%03d (rel=%s)\n", 
                 n->val.date_time->getYear(),
                 n->val.date_time->getMonth(),
                 n->val.date_time->getDay(),
                 n->val.date_time->getHour(),
                 n->val.date_time->getMinute(),
                 n->val.date_time->getSecond(),
                 n->val.date_time->getMillisecond(),
                 n->val.date_time->isRelative() ? "True" : "False");
   else
      s->sprintf("don't know how to print value :-(\n");

   //printd(5, "D\n");
   traceout("dni()");
   return s;
}

#endif

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
