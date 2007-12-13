/*
  QoreRegex.h

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

/*
  PCRE-based matching (Perl-compatible regular expression matching)
  see: http://www.pcre.org for more information on this library

  NOTE: all regular expression matching is done with UTF-8 encoding, so character set
  encodings are converted if necessary
 */

#ifndef _QORE_QOREREGEX_H

#define _QORE_QOREREGEX_H

#include <qore/intern/QoreRegexBase.h>

class QoreRegex : public QoreRegexBase 
{
   private:
      DLLLOCAL void init();

   public:
      DLLLOCAL QoreRegex();
      DLLLOCAL QoreRegex(class QoreString *str);
      DLLLOCAL QoreRegex(class QoreString *str, int options, class ExceptionSink *xsink);
      DLLLOCAL ~QoreRegex();
      DLLLOCAL void concat(char c);
      DLLLOCAL void parse();
      DLLLOCAL void parseRT(class QoreString *pattern, class ExceptionSink *xsink);
      DLLLOCAL bool exec(class QoreString *target, class ExceptionSink *xsink);
      DLLLOCAL class QoreList *extractSubstrings(class QoreString *target, class ExceptionSink *xsink);
      DLLLOCAL class QoreString *getString();
};

#endif // _QORE_QOREREGEX_H
