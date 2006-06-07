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

#define QRNT_STRING 0
#define QRNT_VAR    1

#if 0
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
};

class QRList {
   private:
      class QRNode *head, *tail;

   public:
      inline QRList()
      {
	 head = tail = NULL;
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
      inline void add(class QRNode *n)
      {
	 if (tail)
	    tail->next = n;
	 else
	    head = n;
	 tail = n;
      }
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

#if 0
#include <qore/QoreString.h>
#include <qore/Variable.h>

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
#endif

#endif
