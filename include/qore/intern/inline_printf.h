/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  inline_printf.h

  Qore Programming Language

  Copyright (C) 2007 - 2014 Qore Technologies, sro

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

#ifndef INC_UTILS_STRING_INLINE_PRINTF_H_PV20061219_
#define INC_UTILS_STRING_INLINE_PRINTF_H_PV20061219_

#include <qore/common.h>
#include <string>
#include <stdarg.h>
#include <assert.h>

// helper
extern DLLEXPORT std::string inline_printf_helper_format_string(const char* fmt, va_list arg);

//------------------------------------------------------------------------------
// Allow to construct std::string with printf() like operation in place.
// Short name used for easier typing and no visual space overhead.
// For the same reason it is placed into ths global namespace.
//
// Typical use:
//   string s = S("x = %d", x);
//
class S {
private:
  S& operator=(const S&); // not implemented
  S(const S&); // not implemented

  std::string result;
    
public:
  S(const std::string& fmt, ...) {
    assert(!fmt.empty());
    va_list arg;
    va_start(arg, fmt);
    result = inline_printf_helper_format_string(fmt.c_str(), arg);
    va_end(arg);
  }

  S(const char* fmt, ...) {
    assert(fmt && fmt[0]);
    va_list arg;
    va_start(arg, fmt);
    result = inline_printf_helper_format_string(fmt, arg);
    va_end(arg);
  }

  S(const char* fmt, va_list arg) {
    assert(fmt && fmt[0]);
    result = inline_printf_helper_format_string(fmt, arg);
  }
  
  S(const std::string& fmt, va_list arg) {
    assert(!fmt.empty());
    result = inline_printf_helper_format_string(fmt.c_str(), arg);
  }
  
  operator std::string() const { return result; }
  operator const char*() const { return result.c_str(); }
};

#endif
