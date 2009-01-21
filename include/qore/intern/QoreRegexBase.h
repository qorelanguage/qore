/*
  QoreRegexBase.h

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

#ifndef _QORE_REGEXBASE_H

#define _QORE_REGEXBASE_H

// base class for regex and regex substitution classes

#include <pcre.h>

#define check_re_options(a) (a & ~(PCRE_CASELESS|PCRE_DOTALL|PCRE_EXTENDED|PCRE_MULTILINE|PCRE_UTF8))

// note that the following constant is > 32 bits
#define QRE_GLOBAL 0x100000000LL

class QoreRegexBase {
   protected:
      pcre *p;
      int options;
      class QoreString *str;

   public:
      DLLLOCAL void setCaseInsensitive();
      DLLLOCAL void setDotAll();
      DLLLOCAL void setExtended();
      DLLLOCAL void setMultiline();
};

#endif
