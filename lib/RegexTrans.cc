/*
 RegexTrans.cc
 
 regex-like transliteration class definition
 
 Qore Programming Language
 
 Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols
 
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
#include <qore/RegexTrans.h>

#include <stdlib.h>
#include <strings.h>
#include <ctype.h>

// constructor used when parsing
RegexTrans::RegexTrans()
{
   //printd(5, "RegexTrans::RegexTrans() this=%08p\n", this);
   source = new QoreString();
   target = new QoreString();
   sr = tr = false;
}

RegexTrans::~RegexTrans()
{
   //printd(5, "RegexTrans::~RegexTrans() this=%08p\n", this);
   if (source)
      delete source;
   if (target)
      delete target;
}

void RegexTrans::setTargetRange() 
{ 
   tr = true; 
}

void RegexTrans::setSourceRange() 
{ 
   sr = true; 
}

void RegexTrans::finishSource()
{
   if (sr)
      parse_error("no end character for character range at end of source pattern in transliteration");
}

void RegexTrans::finishTarget()
{
   if (tr)
      parse_error("no end character for character range at end of target pattern in transliteration");
}

void RegexTrans::doRange(class QoreString *str, char end)
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

void RegexTrans::concatSource(char c)
{
   if (sr)
   {
      doRange(source, c);
      sr = false;
   }
   else
      source->concat(c);
}

void RegexTrans::concatTarget(char c)
{
   if (tr)
   {
      doRange(target, c);
      tr = false;
   }
   else
      target->concat(c);
}

class QoreString *RegexTrans::exec(class QoreString *str, class ExceptionSink *xsink)
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
      const char *p = strchr(source->getBuffer(), c);
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


