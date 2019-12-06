/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
    QoreRegexBase.h

    regular expression substitution node definition

    Qore Programming Language

    Copyright (C) 2003 - 2019 Qore Technologies, s.r.o.

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.

    Note that the Qore library is released under a choice of three open-source
    licenses: MIT (as above), LGPL 2+, or GPL 2+; see README-LICENSE for more
    information.
*/

#ifndef _QORE_REGEXBASE_H

#define _QORE_REGEXBASE_H

// base class for regex and regex substitution classes
#include <pcre.h>

#define check_re_options(a) (a & ~(PCRE_CASELESS|PCRE_DOTALL|PCRE_EXTENDED|PCRE_MULTILINE|PCRE_UTF8|PCRE_UCP))

// note that the following constant is > 32 bits
#define QRE_GLOBAL 0x100000000LL

class QoreRegexBase {
public:
    DLLLOCAL void setCaseInsensitive();
    DLLLOCAL void setDotAll();
    DLLLOCAL void setExtended();
    DLLLOCAL void setMultiline();
    DLLLOCAL void setUnicode();

protected:
    pcre* p = nullptr;
    int options = 0;
    QoreString* str = nullptr;
};

#endif
