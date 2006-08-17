/*
  QoreRegexBase.h

  regular expression substitution node definition

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006 David Nichols

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

#ifndef _QORE_REGEXBASE_H

#define _QORE_REGEXBASE_H

// base class for regex and regex substitution classes

#include <pcre.h>

#define check_re_options(a) (a & ~(PCRE_CASELESS|PCRE_DOTALL|PCRE_EXTENDED|PCRE_MULTILINE|PCRE_UTF8))

// note that the following constant is > 32 bits
#define QRE_GLOBAL 0x100000000LL

#if 0
#define QRNT_STRING 0
#define QRNT_VAR    1

class QRNode {
   private:
      union {
	    class QoreString *str;
	    class VarRef *var;
      } d;
      int type;

   public:
      class QRNode *next;

      inline QRNode(class QoreString *str);
      inline QRNode(class VarRef *v);
      inline ~QRNode();
      inline int getType() { return type; }
      // returns 0 for OK, -1 for error
      inline int getValue(class QoreString *str, class ExceptionSink *xsink);
      inline class QoreString *getString()
      {
	 return d.str;
      }
      inline int parseInit(class ExceptionSink *xsink);
};

// the QRList must have at least one node
class QRList {
   private:
      class QRNode *head, *tail;
      bool stat;

      inline void add(class QRNode *n)
      {
	 if (tail)
	    tail->next = n;
	 else
	    head = n;
	 tail = n;
      }
      inline void add(class QoreString *str)
      {
	 add(new QRNode(str));
      }
      inline void add(class VarRef *v)
      {
	 add(new QRNode(v));
      }

   public:
      inline QRList(class QoreString *str)
      {
	 head = tail = NULL;
	 stat = true;
      }

      inline ~QRList()
      {
	 class QRNode *w = head;
	 while (w)
	 {
	    head = w->next;
	    delete w;
	    w = head;
	 }
      }

      inline void parse(char *buf);
      // returns 0 for OK, -1 for error
      inline int parseInit(class ExceptionSink *xsink);
      inline class QoreString *eval(bool &del, class ExceptionSink *xsink);
      inline bool isStatic() { return stat; }
};
#endif

class QoreRegexBase {
   protected:
      pcre *p;
      int options;
      class QoreString *str;

   public:
      inline void setCaseInsensitive()
      {
	 options |= PCRE_CASELESS;
      }
      inline void setDotAll()
      {
	 options |= PCRE_DOTALL;
      }
      inline void setExtended()
      {
	 options |= PCRE_EXTENDED;
      }
      inline void setMultiline()
      {
	 options |= PCRE_MULTILINE;
      }      
};

#include <qore/QoreString.h>
#include <qore/Variable.h>
#include <qore/support.h>

#if 0
#include <ctype.h>
#include <stdlib.h>

inline QRNode::QRNode(class QoreString *str)
{
   type = QRNT_STRING;
   d.str = str;
   next = NULL;
}

inline QRNode::QRNode(class VarRef *v)
{
   type = QRNT_VAR;
   d.var = v;
   next = NULL;
}

inline QRNode::~QRNode()
{
   if (type == QRNT_STRING)
      delete d.str;
   else
      delete d.var;
}

inline int QRNode::parseInit(class ExceptionSink *xsink)
{
   if (type == QRNT_VAR && d.var->resolveExisting())
   {
      xsink->raiseException("REGEX-COMPILATION-ERROR", "reference to unknown variable '$&s' in regular expression string", d.var->name);
      return -1;
   }
   return 0;
}

inline int QRNode::getValue(class QoreString *str, class ExceptionSink *xsink)
{
   if (type == QRNT_STRING)
   {
      str->concat(d.str);
      return 0;
   }
   class QoreNode *v = d.var->eval(xsink);
   if (xsink->isEvent())
   {
      if (v) v->deref(xsink);
      return -1;
   }
   if (!v) return 0;
   if (v->type == NT_STRING)
   {
      str->concat(v->val.String);
      return 0;
   }
   class QoreNode *t = v->convert(NT_STRING);
   v->deref(NULL);
   str->concat(t->val.String);
   t->deref(NULL);
   return 0;
}

inline void QRList::parse(char *buf)
{
   if (!buf[0])
   {
      add(new QoreString());
      return;
   }

   char *last;
   char *c = buf;

   while (*c)
   {
      last = c;
      if ((*c == '$') && *(c + 1) == '{' && isalnum(*(c + 2)))
      {
	 c += 3;
	 while (*c && *c != '}')
	    c++;
	 if (!*c)
	 {
	    parse_error("unbalanced '{' in regular expression variable substitution expression '%s'", buf);
	    return;
	 }
	 int len = c - last;
	 char *name = (char *)malloc(sizeof(char) * (len - 1));
	 strncpy(name, last + 2, len - 2);
	 name[len - 2] = '\0';
	 add(new VarRef(name, VT_UNRESOLVED));
	 stat = false;
      }
      else if ((*c == '$') && isalnum(*(c + 1)))
      {
	 c += 2;
	 while (isalnum(*c) || *c == '_')
	    c++;
	 int len = c - last;
	 char *name = (char *)malloc(sizeof(char) * len);
	 strncpy(name, last + 1, len - 1);
	 name[len - 1] = '\0';
	 add(new VarRef(name, VT_UNRESOLVED));
	 stat = false;
      }
      else
      {
	 bool esc = false;
	 while (*c)
	 {
	    if (!esc && *c == '$')
	       break;
	    if (*c == '\\')
	       esc = true;
	    else
	       esc = false;
	    
	    c++;
	 }
	 add(new QoreString(last, c - last));
      }
   }
}

// returns 0 for OK, -1 for error
inline int QRList::parseInit(class ExceptionSink *xsink)
{
   if (stat)
      return 0;

   class QRNode *w = head;
   while (w)
   {
      if (w->parseInit(xsink))
	 return -1;
      w = w->next;
   }
   return 0;
}

inline class QoreString *QRList::eval(bool &del, class ExceptionSink *xsink)
{
   if (stat)
   {
      del = false;
      return head->getString();
   }
   del = true;
   class QoreString *str = new QoreString();

   class QRNode *w = head;
   while (w)
   {
      if (w->getValue(str, xsink))
      {
	 delete str;
	 del = false;
	 return NULL;
      }
      w = w->next;
   }
   return str;
}
#endif

#endif
