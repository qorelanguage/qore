/*
  inline_printf.h

  Qore Programming Language

  Copyright (C) 2007 Qore Technologies

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
class S
{
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

// EOF

