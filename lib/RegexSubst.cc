/*
 RegexSubst.cc
 
 regular expression substitution node definition
 
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
#include <qore/RegexSubst.h>

#include <stdlib.h>
#include <strings.h>
#include <ctype.h>

void RegexSubst::init()
{
   p = NULL;
   global = false;
   options = PCRE_UTF8;
}

// constructor used when parsing
RegexSubst::RegexSubst()
{
   //printd(5, "RegexSubst::RegexSubst() this=%08p\n", this);
   init();
   str = new QoreString();
   newstr = new QoreString();
}

// constructor when used at run-time
RegexSubst::RegexSubst(class QoreString *pstr, int opts, class ExceptionSink *xsink)
{
   init();
   if (check_re_options(opts))
      xsink->raiseException("REGEX-OPTION-ERROR", "%d contains invalid option bits", opts);
   else
      options |= opts;
   
   newstr = NULL;
   str = NULL;
   parseRT(pstr, xsink);
}

RegexSubst::~RegexSubst()
{
   //printd(5, "RegexSubst::~RegexSubst() this=%08p\n", this);
   if (newstr)
      delete newstr;
   if (p)
      pcre_free(p);
   if (str)
      delete str;
}

void RegexSubst::concatSource(char c)
{
   str->concat(c);
}

void RegexSubst::concatTarget(char c)
{
   newstr->concat(c);
}

// returns 0 for OK, -1 if parse error raised
void RegexSubst::parseRT(class QoreString *pstr, class ExceptionSink *xsink)
{
   // convert to UTF-8 if necessary
   TempEncodingHelper t(pstr, QCS_UTF8, xsink);
   if (*xsink)
      return;
   
   const char *err;
   int eo;
   p = pcre_compile(t->getBuffer(), options, &err, &eo, NULL);
   if (err)
      xsink->raiseException("REGEX-COMPILATION-ERROR", (char *)err);
}

void RegexSubst::parse()
{
   //printd(5, "RegexSubst() this=%08p: str='%s', divider=%d\n", this, str->getBuffer(), divider);
   class ExceptionSink xsink;
   parseRT(str, &xsink);
   if (xsink.isEvent())
      getProgram()->addParseException(&xsink);
   
   //printd(5, "RegexSubst::parse() this=%08p: pstr=%s, newstr=%s, global=%s\n", this, pstr->getBuffer(), newstr->getBuffer(), global ? "true" : "false"); 
   
   delete str;
   str = NULL;
}

void RegexSubst::concat(class QoreString *cstr, int *ovector, int olen, const char *ptr, const char *target)
{
   while (*ptr)
   {
      if (*ptr == '$' && isdigit(ptr[1]))
      {
	 class QoreString n;
	 ptr++;
	 do
	    n.concat(*(ptr++));
	 while (isdigit(*ptr));
	 int pos = atoi(n.getBuffer()) * 2;
	 if (pos > 0 && pos < olen && ovector[pos] != -1)
	    cstr->concat(target + ovector[pos], ovector[pos + 1] - ovector[pos]);
      }
      else
	 cstr->concat(*(ptr++));
   }
}

#define SUBST_OVECSIZE 30
#define SUBST_LASTELEM 20
// called directly for run-time evaluation
class QoreString *RegexSubst::exec(class QoreString *target, class QoreString *nstr, class ExceptionSink *xsink)
{
   class QoreString *t;
   
   // convert to UTF-8 if necessary
   if (target->getEncoding() != QCS_UTF8)
   {
      t = target->convertEncoding(QCS_UTF8, xsink);
      if (xsink->isEvent())
	 return false;
   }
   else 
      t = target;
   
   class QoreString *tstr = new QoreString();
   
   //printd(5, "RegexSubst::exec(%s) this=%08p: global=%s\n", target->getBuffer(), this, global ? "true" : "false"); 
   const char *ptr = t->getBuffer();
   while (true)
   {
      int ovector[SUBST_OVECSIZE];
      int offset = ptr - t->getBuffer();
      int rc = pcre_exec(p, NULL, t->getBuffer(), t->strlen(), offset, 0, ovector, SUBST_OVECSIZE);
      if (rc < 0)
	 break;
      
      if (ovector[0] > offset)
	 tstr->concat(ptr, ovector[0] - offset);
      
      concat(tstr, ovector, SUBST_LASTELEM, nstr->getBuffer(), target->getBuffer());
      
      //printd(5, "RegexSubst::exec() '%s' =~ s/?/%s/%s offset=%d, 0=%d, 1=%d ('%s')\n", t->getBuffer(), nstr->getBuffer(), global ? "g" : "", offset, ovector[0], ovector[1], tstr->getBuffer());
      
      ptr = t->getBuffer() + ovector[1];
      
      if (!global)
	 break;
   } 
   
   //printd(5, "RegexSubst::exec() *ptr=%d ('%s') tstr='%s'\n", *ptr, ptr, tstr->getBuffer());
   if (*ptr)
      tstr->concat(ptr);
   
   if (t != target)
      delete t;
   
   //printd(5, "RegexSubst::exec() this=%08p: returning '%s'\n", this, tstr->getBuffer());
   return tstr;
}

// called for run-time evaluation of parse-time-created objects
class QoreString *RegexSubst::exec(class QoreString *target, class ExceptionSink *xsink)
{
   return exec(target, newstr, xsink);
}

void RegexSubst::setGlobal()
{
   global = true;
}

QoreString *RegexSubst::getPattern() const
{
   return str;
}

