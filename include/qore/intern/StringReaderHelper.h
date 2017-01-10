/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  StringReaderHelper.h

  Qore Programming Language

  Copyright (C) 2016 - 2017 Qore Technologies, s.r.o.

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

#ifndef _QORE_STRINGREADERHELPER_H
#define _QORE_STRINGREADERHELPER_H

#include <functional>
#include <type_traits>

using namespace std::placeholders;

//! the maximum buffer/read size for a single read
#define DefaultStreamReaderHelperBufferSize 4096

typedef std::function<qore_offset_t(void*, size_t, ExceptionSink*)> f_read_t;

template <class T>
DLLLOCAL T* q_remove_bom_tmpl(T* str, const QoreEncoding*& enc) {
   static_assert(std::is_base_of<QoreString, T>::value, "T must inherit QoreString");
   assert(str->getEncoding() == enc);
   if (str->size() > 1 && !enc->isAsciiCompat()) {
      if ((enc == QCS_UTF16 || enc == QCS_UTF16BE) && str->c_str()[0] == (char)0xfe && str->c_str()[1] == (char)0xff) {
         str->replace(0, 2, (const char*)nullptr);
         if (enc == QCS_UTF16) {
            str->setEncoding(QCS_UTF16BE);
            enc = QCS_UTF16BE;
         }
      }
      else if ((enc == QCS_UTF16 || enc == QCS_UTF16LE) && str->c_str()[1] == (char)0xfe && str->c_str()[0] == (char)0xff) {
         str->replace(0, 2, (const char*)nullptr);
         if (enc == QCS_UTF16) {
            str->setEncoding(QCS_UTF16LE);
            enc = QCS_UTF16LE;
         }
      }
   }
   return str;
}

//! remove any BOM in UTF-16 strings, adjust the encoding if required
DLLLOCAL QoreString* q_remove_bom(QoreString* str, const QoreEncoding*& enc) {
   return q_remove_bom_tmpl<QoreString>(str, enc);
}

//! remove any BOM in UTF-16 strings, adjust the encoding if required
DLLLOCAL QoreStringNode* q_remove_bom(QoreStringNode* str, const QoreEncoding*& enc) {
   return q_remove_bom_tmpl<QoreStringNode>(str, enc);
}

//! helper function for reading all possible data and returning it as a string
/** @param xsink for Qore-language exceptions
    @param enc the encoding of the input data and the output string
    @param my_read a function object taking the arguments above, the return value means:
    - \c < 0: an error occurred (xsink has the exception info), 0 = end of data, > 0 the number of bytes read
 */
DLLLOCAL QoreStringNode* q_read_string_all(ExceptionSink* xsink, const QoreEncoding* enc, f_read_t my_read) {
   size_t size = 0;
   SimpleRefHolder<QoreStringNode> str(new QoreStringNode(enc));
   while (true) {
      // ensure there is space in the buffer
      str->reserve(size + DefaultStreamReaderHelperBufferSize);

      int64 rc = my_read((void*)(str->c_str() + size), DefaultStreamReaderHelperBufferSize, xsink);

      if (*xsink)
         return 0;
      if (!rc)
         break;

      size += rc;
   }
   if (!size)
      return 0;
   str->terminate(size);
   return str.release();;
}

//! helper function for reading valid strings with character semantics
/** @param xsink for Qore-language exceptions
    @param size the nubmer of characters to read, negative values = read all available data
    @param enc the encoding of the input data and the output string (must be ASCII compatible)
    @param my_read a function object taking the arguments above, the return value means:
    - \c < 0: an error occurred (xsink has the exception info), 0 = end of data, > 0 the number of bytes read

    @return the string returned, note that if \a size = 0 (or a Qore-language exception occurs), nullptr is returned, oitherwise the caller owns the QoreStringNode reference returned
 */
DLLLOCAL QoreStringNode* q_read_string(ExceptionSink* xsink, int64 size, const QoreEncoding* enc, f_read_t my_read) {
   if (!size)
      return nullptr;
   if (size < 0)
      return q_read_string_all(xsink, enc, my_read);

   // original number of characters requested
   size_t orig_size = size;

   // byte offset of the byte position directly after the last full character scanned
   size_t last_char = 0;

   // total number of characters read
   size_t char_len = 0;

   // minimum character width
   unsigned mw = enc->getMinCharWidth();
   // get minimum byte length
   size *= mw;

   bool check_bom = false;

   SimpleRefHolder<QoreStringNode> str(new QoreStringNode(enc));
   while (char_len < orig_size) {
      // get the minimum number of bytes to read
      size_t bs = size - str->size();

      // ensure there is space in the buffer
      str->reserve(str->size() + bs);

      int rc = my_read((void*)(str->c_str() + str->size()), bs, xsink);
      if (*xsink)
         return 0;
      if (rc == 0)
         break;

      str->terminate(str->size() + rc);

      //printd(5, "srh bs: %d rc: %d str: '%s' (%d %s)\n", bs, rc, str->c_str(), str->size(), enc->getCode());

      // if we have a non-multi-byte character encoding, then we can use byte lengths
      if (!enc->isMultiByte()) {
         if ((size_t)size == str->size())
            break;
         continue;
      }
      else if (!check_bom && str->size() > 1) {
         check_bom = true;
         q_remove_bom(*str, enc);
      }

      // scan data read and find the last valid character position
      const char* e = str->c_str() + str->size();
      while (char_len < orig_size && last_char < str->size()) {
         const char* p = str->c_str() + last_char;
         int cc = enc->getCharLen(p, e - p);
         if (!cc) {
            xsink->raiseException("STREAM-ENCODING-ERROR", "invalid multi-byte character received in byte offset " QLLD " according to the input encoding: '%s'", last_char, enc->getCode());

            return 0;
         }

         //printd(5, "StreamReader::readString() orig: " QLLD " size: " QLLD " char_len: " QLLD " rc: %d last_char: " QLLD " c: %d (offset: " QLLD ") cc: %d '%s'\n", orig_size, size, char_len, rc, last_char, *p, p - str->c_str(), cc, enc->getCode());

         if (cc > 0) {
            // increment character count
            ++char_len;
            // increment byte position after last full character read
            last_char += cc;
            continue;
         }

         // otherwise we need to recalculate the total size to read and break
         cc = -cc;
         // how many bytes of this character do we have
         unsigned hb = (str->size() - last_char);
         assert((unsigned)cc > hb);
         // we will add one byte for the missing character below; here we add in any other bytes we might need
         if ((unsigned)cc > (hb + 1))
            size += (cc - hb - 1);

         break;
      }

      // now we add the minimum character byte length to the remaining size
      // for every character we have not yet read
      size = str->size() + ((orig_size - char_len) * mw);
   }

   return str->empty() ? 0 : str.release();
}

#endif
