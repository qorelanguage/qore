/*
  RegexSubst.h

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

#ifndef _QORE_REGEXSUBST_H

#define _QORE_REGEXSUBST_H

#include <qore/QoreRegexBase.h>
#include <sys/types.h>

class RegexSubst : public QoreRegexBase
{
   private:
      bool global;
      class QoreString *newstr;

      DLLLOCAL void init();
      DLLLOCAL void concat(class QoreString *str, int *ovector, int olen, const char *ptr, const char *target);

   public:
      DLLLOCAL RegexSubst();
      DLLLOCAL RegexSubst(class QoreString *pstr, int opts, class ExceptionSink *xsink);
      DLLLOCAL ~RegexSubst();
      DLLLOCAL void parseRT(class QoreString *pstr, class ExceptionSink *xsink);
      DLLLOCAL void parse();
      DLLLOCAL class QoreString *exec(class QoreString *target, class ExceptionSink *xsink);
      DLLLOCAL class QoreString *exec(class QoreString *target, class QoreString *newstr, class ExceptionSink *xsink);
      DLLLOCAL void concatSource(char c);
      DLLLOCAL void concatTarget(char c);
      DLLLOCAL void setGlobal();
      DLLLOCAL QoreString *getPattern() const;
};

#endif // _QORE_REGEXSUBST_H
