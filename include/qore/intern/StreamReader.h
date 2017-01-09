/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  StreamReader.h

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

#ifndef _QORE_STREAMREADER_H
#define _QORE_STREAMREADER_H

#include <cstdint>

#include "qore/qore_bitopts.h"
#include "qore/InputStream.h"
#include "qore/intern/StringReaderHelper.h"

DLLLOCAL extern qore_classid_t CID_STREAMREADER;
DLLLOCAL extern QoreClass* QC_STREAMREADER;

//! Private data for the Qore::StreamReader class.
class StreamReader : public AbstractPrivateData {
public:
   DLLLOCAL StreamReader(ExceptionSink* xsink, InputStream* is, const QoreEncoding* encoding = QCS_DEFAULT) :
      in(is, xsink),
      enc(encoding)
   {
      if (!enc->isAsciiCompat()) {
         xsink->raiseException("UNSUPPORTED-ENCODING-ERROR", "StreamReader does not support ASCII-incompatible encodings");
         return;
      }
   }

   virtual DLLLOCAL ~StreamReader() {
   }

   DLLLOCAL const QoreEncoding* getEncoding() const {
      return enc;
   }

   //! Read binary data from the stream.
   /** @param limit max amount of data to read; if equal to -1, all data will be read, if equal to 0, no data will be read
       @param xsink exception sink

       @return Qore binary read from the stream
    */
   DLLLOCAL BinaryNode* readBinary(int64 limit, ExceptionSink* xsink) {
      if (limit == 0)
         return 0;
      SimpleRefHolder<BinaryNode> b(new BinaryNode());
      char buffer[STREAMREADER_BUFFER_SIZE];
      if (limit == -1) {
         while (true) {
            int rc = readData(xsink, buffer, STREAMREADER_BUFFER_SIZE, false);
            if (*xsink)
               return 0;
            if (rc == 0)
               break;
            b->append(buffer, rc);
         }
      }
      else {
         while (limit > 0) {
            int rc = readData(xsink, buffer, QORE_MIN(limit, STREAMREADER_BUFFER_SIZE), false);
            if (*xsink)
               return 0;
            if (rc == 0)
               break;
            b->append(buffer, rc);
            limit -= rc;
         }
      }

      return b->empty() ? 0 : b.release();
   }

   //! Read string data from the stream.
   /** @param size max amount of data to read as a number of characters; if equal to -1, all data will be read, if equal to 0, no data will be read
       @param xsink exception sink

       @return Qore string read from the stream
    */
   DLLLOCAL QoreStringNode* readString(int64 size, ExceptionSink* xsink) {
      return q_read_string(xsink, size, enc, std::bind(&StreamReader::readData, this, _3, _1, _2, false));
   }

   //! Read one line.
   /** @param eol end-of-line symbol, only '\n', '\r' and '\r\n' are supported
       @param trim whether to trim the EOL symbols
       @param xsink exception sink

       @return Qore string read from the stream
    */
   DLLLOCAL QoreStringNode* readLine(const QoreStringNode* eol, bool trim, ExceptionSink* xsink) {
      return eol ? readLineEol(eol, trim, xsink) : readLine(trim, xsink);
   }

   DLLLOCAL QoreStringNode* readLineEol(const QoreStringNode* eol, bool trim, ExceptionSink* xsink) {
      TempEncodingHelper eolstr(eol, enc, xsink);
      if (*xsink)
         return 0;

      SimpleRefHolder<QoreStringNode> str(new QoreStringNode(enc));

      qore_size_t eolpos = 0;

      while (true) {
         char c;
         int64 rc = readData(xsink, &c, 1, false);
         if (*xsink)
            return 0;
         if (!rc)
            return str->empty() ? 0 : str.release();

         // add the char to the string
         str->concat(c);

         if ((**eolstr)[eolpos] == c) {
            ++eolpos;
            if (eolpos == eolstr->size()) {
               if (trim)
                  str->terminate(str->size() - eolpos);
               return str.release();
            }
         }
         else if (eolpos) {
            // check all positions to see if the string matches
            bool found = false;
            for (size_t i = eolpos; i; --i) {
               if (!strncmp(eolstr->c_str(), str->c_str() + str->size() - i, i)) {
                  found = true;
                  if (eolpos != i)
                     eolpos = i;
                  break;
               }
            }
            if (!found)
               eolpos = 0;
         }
      }
   }

   DLLLOCAL QoreStringNode* readLine(bool trim, ExceptionSink* xsink) {
      SimpleRefHolder<QoreStringNode> str(new QoreStringNode(enc));

      while (true) {
         char c;
         int64 rc = readData(xsink, &c, 1, false);
         if (*xsink)
            return 0;
         if (!rc) { // End of stream.
            return str->empty() ? 0 : str.release();
         }

         if (c == '\n') {
            if (!trim)
               str->concat(c);
            return str.release();
         }
         else if (c == '\r') {
            if (!trim)
               str->concat(c);
            int64 p = peek(xsink);
            if (*xsink)
               return 0;
            if (p == '\n') {
               readData(xsink, &c, 1);
               if (!trim)
                  str->concat((char)p);
            }
            return str.release();
         }
         str->concat(c);
      }
   }

   DLLLOCAL int64 readi1(ExceptionSink* xsink) {
      char i = 0;
      if (readData(xsink, &i, 1) < 0)
         return 0;
      return i;
   }

   DLLLOCAL int64 readi2(ExceptionSink* xsink) {
      short i = 0;
      if (readData(xsink, &i, 2) < 0)
         return 0;
      i = ntohs(i);
      return i;
   }

   DLLLOCAL int64 readi4(ExceptionSink* xsink) {
      int32_t i = 0;
      if (readData(xsink, &i, 4) < 0)
         return 0;
      i = ntohl(i);
      return i;
   }

   DLLLOCAL int64 readi8(ExceptionSink* xsink) {
      int64 i = 0;
      if (readData(xsink, &i, 8) < 0)
         return 0;
      i = i8MSB(i);
      return i;
   }

   DLLLOCAL int64 readi2LSB(ExceptionSink* xsink) {
      short i = 0;
      if (readData(xsink, &i, 2) < 0)
         return 0;
      i = i2LSB(i);
      return i;
   }

   DLLLOCAL int64 readi4LSB(ExceptionSink* xsink) {
      int32_t i = 0;
      if (readData(xsink, &i, 4) < 0)
         return 0;
      i = i4LSB(i);
      return i;
   }

   DLLLOCAL int64 readi8LSB(ExceptionSink* xsink) {
      int64 i = 0;
      if (readData(xsink, &i, 8) < 0)
         return 0;
      i = i8LSB(i);
      return i;
   }

   DLLLOCAL int64 readu1(ExceptionSink* xsink) {
      unsigned char i = 0;
      if (readData(xsink, &i, 1) < 0)
         return 0;
      return i;
   }

   DLLLOCAL int64 readu2(ExceptionSink* xsink) {
      unsigned short i = 0;
      if (readData(xsink, &i, 2) < 0)
         return 0;
      i = ntohs(i);
      return i;
   }

   DLLLOCAL int64 readu4(ExceptionSink* xsink) {
      uint32_t i = 0;
      if (readData(xsink, &i, 4) < 0)
         return 0;
      i = ntohl(i);
      return i;
   }

   DLLLOCAL int64 readu2LSB(ExceptionSink* xsink) {
      unsigned short i = 0;
      if (readData(xsink, &i, 2) < 0)
         return 0;
      i = i2LSB(i);
      return i;
   }

   DLLLOCAL int64 readu4LSB(ExceptionSink* xsink) {
      uint32_t i = 0;
      if (readData(xsink, &i, 4) < 0)
         return 0;
      i = i4LSB(i);
      return i;
   }

   DLLLOCAL virtual const char* getName() const { return "StreamReader"; }

protected:
   // default buffer size (note that I/O is generally unbuffered in this class)
   static const int STREAMREADER_BUFFER_SIZE = 4096;

   //! Source input stream.
   ReferenceHolder<InputStream> in;

   //! Encoding of the source input stream.
   const QoreEncoding* enc;

private:
   //! Read data until a limit.
   /** @param xsink exception sink
       @param dest destination buffer
       @param limit maximum amount of data to read
       @param require_all if true then throw an exception if the required amount of data cannot be read from the stream

       @return amount of data read, -1 in case of error
    */
   DLLLOCAL virtual qore_offset_t readData(ExceptionSink* xsink, void* dest, qore_size_t limit, bool require_all = true) {
      assert(dest);
      assert(limit > 0);
      char* destPtr = static_cast<char*>(dest);
      qore_size_t read = 0;
      while (true) {
         int64 rc = in->read(destPtr + read, limit - read, xsink);
         if (*xsink)
            return -1;
         //printd(5, "StreamReader::readData() dest: %p limit: " QLLD " read: " QLLD " rc: " QLLD " char: %d\n", dest, limit, read, rc, destPtr[0]);
         if (!rc) {
            if (require_all) {
               xsink->raiseException("END-OF-STREAM-ERROR", "there is not enough data available in the stream; " QLLD " bytes were requested, and " QLLD " were read", limit, read);
               return -1;
            }
            break;
         }
         read += rc;
         if (read == limit)
            break;
      }
      return read;
   }

   /**
    * @brief Peeks the next byte from the input stream.
    * @param xsink the exception sink
    * @return the next byte available to be read, -1 indicates end of the stream, -2 indicates an error
    */
   virtual int64 peek(ExceptionSink* xsink) {
      return in->peek(xsink);
   }
};

#endif // _QORE_STREAMREADER_H
