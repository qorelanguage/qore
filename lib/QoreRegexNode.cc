/*
 QoreRegexNode.cc
 
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

QoreRegexNode::QoreRegexNode() : ParseNoEvalNode(NT_REGEX)
{
   init();
   str = new QoreString();
}

QoreRegexNode::QoreRegexNode(class QoreString *s) : ParseNoEvalNode(NT_REGEX)
{
   init();
   str = s;
   parse();
}

QoreRegexNode::QoreRegexNode(class QoreString *s, int opts, class ExceptionSink *xsink) : ParseNoEvalNode(NT_REGEX)
{
   init();
   str = NULL;
   
   if (check_re_options(opts))
      xsink->raiseException("REGEX-OPTION-ERROR", "%d contains invalid option bits", opts);
   else
      options |= opts;
   
   parseRT(s, xsink);
}

QoreRegexNode::~QoreRegexNode()
{
   if (p)
      pcre_free(p);
   if (str)
      delete str;
}

// get string representation (for %n and %N), foff is for multi-line formatting offset, -1 = no line breaks
// the ExceptionSink is only needed for QoreObject where a method may be executed
// use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using these functions directly
// returns -1 for exception raised, 0 = OK
int QoreRegexNode::getAsString(QoreString &str, int foff, ExceptionSink *xsink) const
{
   str.sprintf("regular expression (0x%08p)", this);
   return 0;
}

// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
QoreString *QoreRegexNode::getAsString(bool &del, int foff, ExceptionSink *xsink) const
{
   del = true;
   QoreString *rv = new QoreString();
   getAsString(*rv, foff, xsink);
   return rv;
}

// returns the data type
const QoreType *QoreRegexNode::getType() const
{
   return NT_REGEX;
}

// returns the type name as a c string
const char *QoreRegexNode::getTypeName() const
{
   return "regular expression";
}

void QoreRegexNode::concat(char c)
{
   str->concat(c);
}

void QoreRegexNode::parseRT(class QoreString *pattern, class ExceptionSink *xsink)
{
   const char *err;
   int eo;
   
   // convert to UTF-8 if necessary
   class QoreString *t;
   if (pattern->getEncoding() != QCS_UTF8)
   {
      t = pattern->convertEncoding(QCS_UTF8, xsink);
      if (xsink->isEvent())
	 return;
   }
   else
      t = pattern;
   
   p = pcre_compile(t->getBuffer(), options, &err, &eo, NULL);
   if (err)
   {
      //printd(5, "QoreRegexNode::parse() error parsing '%s': %s", t->getBuffer(), (char *)err);
      xsink->raiseException("REGEX-COMPILATION-ERROR", (char *)err);
   }
   if (t != pattern)
      delete t;
}

void QoreRegexNode::parse()
{
   class ExceptionSink xsink;
   parseRT(str, &xsink);
   delete str;
   str = NULL;
   if (xsink.isEvent())
      getProgram()->addParseException(&xsink);
}

#define OVECCOUNT 30
bool QoreRegexNode::exec(const QoreString *target, class ExceptionSink *xsink) const
{
   ConstTempEncodingHelper t(target, QCS_UTF8, xsink);
   if (!t)
      return false;
   
   // the PCRE docs say that if we don't send an ovector here the library may have to malloc
   // memory, so, even though we don't need the results, we include the vector to avoid 
   // extraneous malloc()s
   int ovector[OVECCOUNT];
   int rc = pcre_exec(p, NULL, t->getBuffer(), t->strlen(), 0, 0, ovector, OVECCOUNT);
   //printd(0, "QoreRegexNode::exec(%s =~ /%s/ = %d\n", target->getBuffer(), str->getBuffer(), rc);   
   return rc >= 0;
}

#define OVEC_LATELEM 20
class QoreListNode *QoreRegexNode::extractSubstrings(const QoreString *target, class ExceptionSink *xsink) const
{
   ConstTempEncodingHelper t(target, QCS_UTF8, xsink);
   if (!t)
      return false;
   
   // FIXME: rc = 0 means that not enough space was available in ovector!
   
   // the PCRE docs say that if we don't send an ovector here the library may have to malloc
   // memory, so, even though we don't need the results, we include the vector to avoid 
   // extraneous malloc()s
   int ovector[OVECCOUNT];
   int rc = pcre_exec(p, NULL, t->getBuffer(), t->strlen(), 0, 0, ovector, OVECCOUNT);
   //printd(0, "QoreRegexNode::exec(%s =~ /%s/ = %d\n", target->getBuffer(), str->getBuffer(), rc);
   
   if (rc < 1)
      return NULL;
   
   class QoreListNode *l = new QoreListNode();
   
   if (rc > 1)
   {
      int x = 0;
      while (++x < rc)
      {
	 int pos = x * 2;
	 if (ovector[pos] == -1)
	 {
	    l->push(nothing());
	    continue;
	 }
	 QoreStringNode *tstr = new QoreStringNode();
	 //printd(5, "substring %d: %d - %d (len %d)\n", x, ovector[pos], ovector[pos + 1], ovector[pos + 1] - ovector[pos]);
	 tstr->concat(t->getBuffer() + ovector[pos], ovector[pos + 1] - ovector[pos]);
	 l->push(tstr);
      }
   }
   
   return l;
}

void QoreRegexNode::init()
{
   p = NULL;
   options = PCRE_UTF8;
}

QoreString *QoreRegexNode::getString() 
{
   class QoreString *rs = str;
   str = NULL;
   return rs;
}
