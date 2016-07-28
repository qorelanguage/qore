//--------------------------------------------------------------------*- C++ -*-
//
//  Qore Programming Language
//
//  Copyright (C) 2016 Qore Technologies, sro
//
//  Permission is hereby granted, free of charge, to any person obtaining a
//  copy of this software and associated documentation files (the "Software"),
//  to deal in the Software without restriction, including without limitation
//  the rights to use, copy, modify, merge, publish, distribute, sublicense,
//  and/or sell copies of the Software, and to permit persons to whom the
//  Software is furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
//  DEALINGS IN THE SOFTWARE.
//
//------------------------------------------------------------------------------
///
/// \file
/// \brief Defines a helper class wrapping libiconv.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_INTERN_ICONVHELPER_H_
#define INCLUDE_QORE_INTERN_ICONVHELPER_H_

#include <errno.h>
#include <iconv.h>
#include "qore/Qore.h"
#include "qore/intern/core/Exception.h"
#include "qore/intern/core/StringBuilder.h"

class IconvHelper {

public:
   enum class Result {
      Ok, Invalid, TooBig
   };

public:
   DLLLOCAL IconvHelper(const QoreEncoding *to, const QoreEncoding *from) : to(to), from(from) {
#ifdef NEED_ICONV_TRANSLIT
      QoreString to_code(const_cast<char *>(to->getCode()));
      to_code.concat("//TRANSLIT");
      c = iconv_open(to_code.getBuffer(), from->getCode());
#else
      c = iconv_open(to->getCode(), from->getCode());
#endif
      if (c == reinterpret_cast<iconv_t>(-1)) {
         if (errno == EINVAL) {
            throw qore::Exception("ENCODING-CONVERSION-ERROR", qore::StringBuilder() << "cannot convert from \""
                  << from->getCode() << "\" to \"" << to->getCode() << "\"");
         } else {
            throw reportUnknownError(errno);
         }
      }
   }

   DLLLOCAL ~IconvHelper() {
      iconv_close(c);
   }

   DLLLOCAL Result iconv(char **inbuf, size_t *inavail, char **outbuf, size_t *outavail, bool flushing) {
      if (iconv_adapter(::iconv, c, inbuf, inavail, outbuf, outavail) != static_cast<size_t>(-1)) {
         return Result::Ok;
      }
      switch (errno) {
         case EINVAL:
            if (flushing) {         //flushing - there will be no more input
               throw reportIllegalSequence();
            }
            return Result::Invalid;
         case E2BIG:
            return Result::TooBig;
         case EILSEQ:
            throw reportIllegalSequence();
         default:
            throw reportUnknownError(errno);
      }
   }

private:
   // needed for platforms where the input buffer is defined as "const char"
   template<typename T>
   static size_t iconv_adapter(size_t (*iconv_f)(iconv_t, T, size_t *, char **, size_t *), iconv_t handle,
         char **inbuf, size_t *inavail, char **outbuf, size_t *outavail) {
      return (*iconv_f) (handle, const_cast<T>(inbuf), inavail, outbuf, outavail);
   }

   qore::Exception reportIllegalSequence() {
      return qore::Exception("ENCODING-CONVERSION-ERROR", qore::StringBuilder()
            << "illegal character sequence found in input type \"" << from->getCode() << "\" (while converting to \""
            << to->getCode() << "\")");
   }

   qore::Exception reportUnknownError(int errCode) {
      return qore::Exception("ENCODING-CONVERSION-ERROR", qore::StringBuilder() << "unknown error (errno "
            << errCode << ") converting from \"" << from->getCode() << "\" to \"" << to->getCode() << "\"");
   }

private:
   const QoreEncoding *to;
   const QoreEncoding *from;
   iconv_t c;
};

#endif // INCLUDE_QORE_INTERN_ICONVHELPER_H_
