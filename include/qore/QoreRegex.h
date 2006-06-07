/*
  QoreRegex.h

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

/*
  PCRE-based matching (Perl-compatible regular expression matching)
  see: http://www.pcre.org for more information on this library

  NOTE: all regular expression matching is done with UTF-8 encoding, so character set
  encodings are converted if necessary
 */

#ifndef _QORE_QOREREGEX_H

#define _QORE_QOREREGEX_H

#include <qore/QoreRegexBase.h>

class QoreRegex : public QoreRegexBase 
{
   private:
      inline void init()
      {
	 p = NULL;
	 options = PCRE_UTF8;
      }

   public:
      inline QoreRegex();
      inline QoreRegex(class QoreString *str);
      inline QoreRegex(class QoreString *str, int options, class ExceptionSink *xsink);
      inline ~QoreRegex();
      inline void concat(char c);
      inline void parse();
      inline void parseRT(class QoreString *pattern, class ExceptionSink *xsink);
      inline bool exec(class QoreString *target, class ExceptionSink *xsink);
      inline class QoreString *getString() 
      {
	 class QoreString *rs = str;
	 str = NULL;
	 return rs;
      }
};

#include <qore/QoreString.h>
#include <qore/support.h>

inline QoreRegex::QoreRegex()
{
   init();
   str = new QoreString();
}

inline QoreRegex::QoreRegex(class QoreString *s)
{
   init();
   str = s;
   parse();
}

inline QoreRegex::QoreRegex(class QoreString *s, int opts, class ExceptionSink *xsink)
{
   init();
   str = NULL;

   if (check_re_options(opts))
      xsink->raiseException("REGEX-OPTION-ERROR", "%d contains invalid option bits", opts);
   else
      options |= opts;

   parseRT(s, xsink);
}

inline QoreRegex::~QoreRegex()
{
   if (p)
      pcre_free(p);
   if (str)
      delete str;
}

inline void QoreRegex::concat(char c)
{
   str->concat(c);
}

inline void QoreRegex::parseRT(class QoreString *pattern, class ExceptionSink *xsink)
{
   const char *err;
   int eo;

   // convert to UTF-8 if necessary
   class QoreString *t;
   if (pattern->getEncoding() && pattern->getEncoding() != QCS_UTF8)
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
      //printd(5, "QoreRegex::parse() error parsing '%s': %s", t->getBuffer(), (char *)err);
      xsink->raiseException("REGEX-COMPILATION-ERROR", (char *)err);
   }
   if (t != pattern)
      delete t;
}

inline void QoreRegex::parse()
{
   class ExceptionSink xsink;
   parseRT(str, &xsink);
   delete str;
   str = NULL;
   if (xsink.isEvent())
      getProgram()->addParseException(&xsink);
}

#define OVECCOUNT 30
inline bool QoreRegex::exec(class QoreString *target, class ExceptionSink *xsink)
{
   class QoreString *t;

   // convert to UTF-8 if necessary
   if (target->getEncoding() && target->getEncoding() != QCS_UTF8)
   {
      t = target->convertEncoding(QCS_UTF8, xsink);
      if (xsink->isEvent())
	 return false;
   }
   else 
      t = target;

   // the PCRE docs say that if we don't send an ovector here the library may have to malloc
   // memory, so, even though we don't need the results, we include the vector to avoid 
   // extraneous malloc()s
   int ovector[OVECCOUNT];
   int rc = pcre_exec(p, NULL, t->getBuffer(), t->strlen(), 0, 0, ovector, OVECCOUNT);
   //printd(0, "QoreRegex::exec(%s =~ /%s/ = %d\n", target->getBuffer(), str->getBuffer(), rc);

   if (t != target)
      delete t;

   return rc >= 0;
}

#endif // _QORE_QOREREGEX_H
