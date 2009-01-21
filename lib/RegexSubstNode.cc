/*
 RegexSubstNode.cc
 
 regular expression substitution node definition
 
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
#include <qore/intern/RegexSubstNode.h>

#include <stdlib.h>
#include <strings.h>
#include <ctype.h>

void RegexSubstNode::init()
{
   p = 0;
   global = false;
   options = PCRE_UTF8;
}

// constructor used when parsing
RegexSubstNode::RegexSubstNode() : ParseNoEvalNode(NT_REGEX_SUBST)
{
   //printd(5, "RegexSubstNode::RegexSubstNode() this=%08p\n", this);
   init();
   str = new QoreString();
   newstr = new QoreString();
}

// constructor when used at run-time
RegexSubstNode::RegexSubstNode(const QoreString *pstr, int opts, ExceptionSink *xsink) : ParseNoEvalNode(NT_REGEX_SUBST)
{
   init();
   if (check_re_options(opts))
      xsink->raiseException("REGEX-OPTION-ERROR", "%d contains invalid option bits", opts);
   else
      options |= opts;
   
   newstr = 0;
   str = 0;
   parseRT(pstr, xsink);
}

RegexSubstNode::~RegexSubstNode()
{
   //printd(5, "RegexSubstNode::~RegexSubstNode() this=%08p\n", this);
   if (newstr)
      delete newstr;
   if (p)
      pcre_free(p);
   if (str)
      delete str;
}

// get string representation (for %n and %N), foff is for multi-line formatting offset, -1 = no line breaks
// the ExceptionSink is only needed for QoreObject where a method may be executed
// use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using these functions directly
// returns -1 for exception raised, 0 = OK
int RegexSubstNode::getAsString(QoreString &mstr, int foff, ExceptionSink *xsink) const
{
   mstr.sprintf("regular expression substitution expression (0x%08p)", this);
   return 0;
}

// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
QoreString *RegexSubstNode::getAsString(bool &del, int foff, ExceptionSink *xsink) const
{
   del = true;
   QoreString *rv = new QoreString();
   getAsString(*rv, foff, xsink);
   return rv;
}

// returns the data type
qore_type_t RegexSubstNode::getType() const
{
   return NT_REGEX_SUBST;
}

// returns the type name as a c string
const char *RegexSubstNode::getTypeName() const
{
   return "regular expression substitution expression";
}

void RegexSubstNode::concatSource(char c)
{
   str->concat(c);
}

void RegexSubstNode::concatTarget(char c)
{
   newstr->concat(c);
}

// returns 0 for OK, -1 if parse error raised
void RegexSubstNode::parseRT(const QoreString *pstr, ExceptionSink *xsink)
{
   // convert to UTF-8 if necessary
   TempEncodingHelper t(pstr, QCS_UTF8, xsink);
   if (*xsink)
      return;
   
   const char *err;
   int eo;
   p = pcre_compile(t->getBuffer(), options, &err, &eo, 0);
   if (err)
      xsink->raiseException("REGEX-COMPILATION-ERROR", (char *)err);
}

void RegexSubstNode::parse()
{
   //printd(5, "RegexSubstNode() this=%08p: str='%s', divider=%d\n", this, str->getBuffer(), divider);
   ExceptionSink xsink;
   parseRT(str, &xsink);
   if (xsink.isEvent())
      getProgram()->addParseException(&xsink);
   
   //printd(5, "RegexSubstNode::parse() this=%08p: pstr=%s, newstr=%s, global=%s\n", this, pstr->getBuffer(), newstr->getBuffer(), global ? "true" : "false"); 
   
   delete str;
   str = 0;
}

// static function
void RegexSubstNode::concat(class QoreString *cstr, int *ovector, int olen, const char *ptr, const char *target)
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
QoreStringNode *RegexSubstNode::exec(const QoreString *target, const QoreString *nstr, ExceptionSink *xsink) const
{
   TempEncodingHelper t(target, QCS_UTF8, xsink);
   if (*xsink)
      return 0;

   QoreStringNode *tstr = new QoreStringNode();
   
   const char *ptr = t->getBuffer();
   //printd(5, "RegexSubstNode::exec(%s) this=%08p: global=%s\n", ptr, this, global ? "true" : "false"); 
   while (true)
   {
      int ovector[SUBST_OVECSIZE];
      int offset = ptr - t->getBuffer();
      int rc = pcre_exec(p, 0, t->getBuffer(), t->strlen(), offset, 0, ovector, SUBST_OVECSIZE);
      if (rc < 0)
	 break;
      
      if (ovector[0] > offset)
	 tstr->concat(ptr, ovector[0] - offset);
      
      concat(tstr, ovector, SUBST_LASTELEM, nstr->getBuffer(), target->getBuffer());
      
      //printd(5, "RegexSubstNode::exec() '%s' =~ s/?/%s/%s offset=%d, 0=%d, 1=%d ('%s')\n", t->getBuffer(), nstr->getBuffer(), global ? "g" : "", offset, ovector[0], ovector[1], tstr->getBuffer());
      
      ptr = t->getBuffer() + ovector[1];
      
      if (!global)
	 break;
   } 
   
   //printd(5, "RegexSubstNode::exec() *ptr=%d ('%s') tstr='%s'\n", *ptr, ptr, tstr->getBuffer());
   if (*ptr)
      tstr->concat(ptr);
   
   //printd(5, "RegexSubstNode::exec() this=%08p: returning '%s'\n", this, tstr->getBuffer());
   return tstr;
}

// called for run-time evaluation of parse-time-created objects
QoreStringNode *RegexSubstNode::exec(const QoreString *target, ExceptionSink *xsink) const
{
   return exec(target, newstr, xsink);
}

void RegexSubstNode::setGlobal()
{
   global = true;
}

QoreString *RegexSubstNode::getPattern() const
{
   return str;
}
