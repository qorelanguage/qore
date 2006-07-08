/*
  RegexTrans.h

  regex-like transliteration class definition

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

#ifndef _QORE_REGEXTRANS_H

#define _QORE_REGEXTRANS_H

class RegexTrans
{
   private:
      class QoreString *source, *target;
      bool sr, tr;

      void doRange(class QoreString *str, char end);

   public:
      inline RegexTrans();
      inline ~RegexTrans();
      inline void finishSource();
      inline void finishTarget();
      inline class QoreString *exec(class QoreString *target, class ExceptionSink *xsink);
      inline void concatSource(char c);
      inline void concatTarget(char c);
      inline void setTargetRange() { tr = true; }
      inline void setSourceRange() { sr = true; }
};

#include <qore/support.h>
#include <qore/Exception.h>
#include <qore/QoreString.h>

#include <stdlib.h>
#include <strings.h>
#include <ctype.h>

// constructor used when parsing
inline RegexTrans::RegexTrans()
{
   //printd(5, "RegexTrans::RegexTrans() this=%08x\n", this);
   source = new QoreString();
   target = new QoreString();
   sr = tr = false;
}

inline RegexTrans::~RegexTrans()
{
   //printd(5, "RegexTrans::~RegexTrans() this=%08x\n", this);
   if (source)
      delete source;
   if (target)
      delete target;
}

inline void RegexTrans::finishSource()
{
   if (sr)
      parse_error("no end character for character range at end of source pattern in transliteration");
}

inline void RegexTrans::finishTarget()
{
   if (tr)
      parse_error("no end character for character range at end of target pattern in transliteration");
}

inline void RegexTrans::doRange(class QoreString *str, char end)
{
   if (!str->strlen())
   {
      parse_error("no start character for character range in transliteration");
      return;
   }
   char start = str->getBuffer()[str->strlen() - 1];
   str->terminate(str->strlen() - 1);
   if (start > end)
   {
      parse_error("invalid range '%c' - '%c' in transliteration operator", start, end);
      return;
   }
   do
      str->concat(start++);
   while (start <= end);
}

inline void RegexTrans::concatSource(char c)
{
   if (sr)
   {
      doRange(source, c);
      sr = false;
   }
   else
      source->concat(c);
}

inline void RegexTrans::concatTarget(char c)
{
   if (tr)
   {
      doRange(target, c);
      tr = false;
   }
   else
      target->concat(c);
}

inline class QoreString *RegexTrans::exec(class QoreString *str, class ExceptionSink *xsink)
{
   //printd(5, "source='%s' target='%s' ('%s')\n", source->getBuffer(), target->getBuffer(), str->getBuffer());
   class QoreString *tstr;
   if (str->getEncoding() != QCS_DEFAULT)
   {
      tstr = str->convertEncoding(QCS_DEFAULT, xsink);
      if (!tstr)
	 return NULL;
   }
   else
      tstr = str;

   class QoreString *ns = new QoreString();
   for (int i = 0; i < tstr->strlen(); i++)
   {
      char c = tstr->getBuffer()[i];
      char *p = strchr(source->getBuffer(), c);
      if (p)
      {
	 int pos = p - source->getBuffer();
	 if (target->strlen() <= pos)
	    pos = target->strlen() - 1;
	 if (pos >= 0)
	    ns->concat(target->getBuffer()[pos]);
      }
      else
	 ns->concat(c);
   }
   if (tstr != str)
      delete tstr;
   return ns;
}

#endif // _QORE_REGEXTRANS_H
