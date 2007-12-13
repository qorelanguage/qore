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


#include <qore/intern/inline_printf.h>
#include <qore/minitest.hpp>

#include <stdio.h>

//------------------------------------------------------------------------------
namespace {
class array
{
  array(const array&);
  array& operator=(const array&);

  char* data;
public:
  array() : data(0) {}
  array(char* d) : data(d) {}
  ~array() { delete[] data; }
  void reset(char* d) { delete[] data; data = d; }
  char* get() const { return data; }
  char& operator[](unsigned idx) { return data[idx]; }
};
}
//------------------------------------------------------------------------------
std::string inline_printf_helper_format_string(const char* fmt, va_list arg)
{
  // try first the local buffer for small strings
  const int SmallBufferSize = 256;
  char small_buffer[SmallBufferSize + 1];
  int cnt = vsnprintf(small_buffer, SmallBufferSize, fmt, arg);
  if (cnt >= 0 && cnt < SmallBufferSize) { // also checks for truncation
      small_buffer[cnt] = 0;
      return std::string(small_buffer);
  } 

  // larger buffer is needed
  int LargeBufferSize = 1024;
  array large_buffer;
  for (;;) {
    large_buffer.reset(new char[LargeBufferSize + 1]);
    cnt = vsnprintf(large_buffer.get(), LargeBufferSize, fmt, arg);
    if (cnt >= 0 && cnt < LargeBufferSize) { // also checks for truncation
      large_buffer[cnt] = 0;
      return std::string(large_buffer.get());
    }
    LargeBufferSize *= 2;
  }
}

//------------------------------------------------------------------------------
#ifdef DEBUG
namespace test_inline_printf_cpp_018236 {
TEST()
{
  printf("inline printf test1\n");
  std::string s = S("a");
  assert(s == "a");
}

TEST()
{
  printf("inline printf test2\n");
  std::string s = S("a = %d, b = %c", 123, 'r');
  assert(s == "a = 123, b = r");
}

TEST()
{    
  printf("inline printf test3\n");
  std::string fmt = "%d";
  std::string s2 = S(fmt, 999);
  assert(s2 == "999");
}

TEST()
{
  printf("inline printf test4\n");
  // test very long formatted string
  std::string s(10000, 'a');
  std::string res = S("%s%s", s.c_str(), s.c_str());
  assert(res.size() == 2 * 10000);
}

static std::string foo1(const char* fmt, va_list arg)
{
  return S(fmt, arg);
}

static std::string bar1(const char* fmt, ...)
{
  va_list arg;
  va_start(arg, fmt);
  std::string res = foo1(fmt, arg);
  va_end(arg);
  return res;
}

static std::string foo2(const std::string& fmt, va_list arg)
{
  return S(fmt, arg);
}

static std::string bar2(const std::string& fmt, ...)
{
  va_list arg;
  va_start(arg, fmt);
  std::string res = foo2(fmt, arg);
  va_end(arg);
  return res;
}

TEST()
{
  printf("inline printf test5\n");
  std::string s = bar1("%s%d", "aaa", 111);
  assert(s ==  "aaa111"); 
}

TEST()
{
  printf("inline printf test6\n");
  std::string fmt("%d%d");
  std::string s = bar2(fmt, 0, 0);
  assert(s ==  "00");
}

} // namespace
#endif

// EOF

