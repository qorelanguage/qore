/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  EncodingConversionInputStream.h

  Qore Programming Language

  Copyright (C) 2003 - 2016 David Nichols

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

#ifndef _QORE_ENCODINGCONVERSIONINPUTSTREAM_H
#define _QORE_ENCODINGCONVERSIONINPUTSTREAM_H

#include "qore/intern/ConversionInputStreamBase.h"
#include "qore/intern/IconvHelper.h"

/**
 * @brief Private data for the Qore::EncodingConversionInputStream class.
 */
class EncodingConversionInputStream : public ConversionInputStreamBase<> {

public:
   DLLLOCAL EncodingConversionInputStream(QoreObject *isObj, InputStream *is, const QoreEncoding *srcEncoding,
         const QoreEncoding *dstEncoding, ExceptionSink *xsink) : ConversionInputStreamBase(isObj, is, xsink),
         conv(dstEncoding, srcEncoding, xsink) {
   }

   DLLLOCAL const char *getName() /*override*/ {
      return "EncodingConversionInputStream";
   }

protected:
   DLLLOCAL bool performConversion(size_t outAvail, ExceptionSink *xsink) {
      char *outPtr = outBuf.getWritePtr();
      if (conv.iconv(&inBuf.ptr, &inBuf.count, &outPtr, &outAvail) == (size_t) -1) {
         switch (errno) {
            case EINVAL:
               if (eofReached) {
                  conv.reportIllegalSequence(xsink);
                  return false;
               }
               break;
            case E2BIG:
               break;
            case EILSEQ:
               conv.reportIllegalSequence(xsink);
               return false;
            default:
               conv.reportUnknownError(xsink);
               return false;
         }
      }
      outBuf.count += outPtr - outBuf.getWritePtr();
      return true;
   }

private:
   IconvHelper conv;
};


#endif // _QORE_ENCODINGCONVERSIONINPUTSTREAM_H
