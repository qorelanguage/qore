# Copyright (c) 2015 Niclas Rosenvik <youremailsarecrap@gmail.com>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in
# copies of this file or files derived from it .
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
# Module to find iconv.
# Sets ICONV_INCLUDE_DIR and ICONV_LIBRARY in case libiconv is used.
#

include(CheckFunctionExists)
include(CheckIncludeFile)
include(CheckSymbolExists)
include(CMakePushCheckState)
include(FindPackageHandleStandardArgs)

cmake_push_check_state(RESET)
set(CMAKE_REQUIRED_QUIET true)
check_include_file(iconv.h ICONV_INCLUDE_IN_LIBC)
check_function_exists(iconv ICONV_FOUND_IN_LIBC)

if(ICONV_FOUND_IN_LIBC AND ICONV_INCLUDE_IN_LIBC)
    set(ICONV_LIBRARY_TYPE "libc")
else()
    find_path(ICONV_INCLUDE_DIR iconv.h)
    find_library(ICONV_LIBRARY NAMES iconv libiconv libiconv-2)
    if(ICONV_LIBRARY)
        set(CMAKE_REQUIRED_INCLUDES ${ICONV_INCLUDE_DIR})
        set(CMAKE_REQUIRED_LIBRARIES ${ICONV_LIBRARY})
        check_symbol_exists(libiconv iconv.h ICONV_FOUND_IN_LIBICONV)
        if(ICONV_FOUND_IN_LIBICONV)
            set(ICONV_LIBRARY_TYPE "GNU libiconv")
        endif()
    endif()
endif()
cmake_pop_check_state()

find_package_handle_standard_args(ICONV REQUIRED_VARS ICONV_LIBRARY_TYPE)
