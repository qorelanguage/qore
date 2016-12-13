/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  BufferedStreamReader.h

  Qore Programming Language

  Copyright (C) 2016 Qore Technologies, s.r.o.

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

#ifndef _QORE_BUFFEREDSTREAMREADER_H
#define _QORE_BUFFEREDSTREAMREADER_H

#include <cstdint>

#include "qore/qore_bitopts.h"
#include "qore/InputStream.h"
#include "qore/intern/StreamReader.h"

//! Private data for the Qore::BufferedStreamReader class.
class BufferedStreamReader : public StreamReader {

private:
   // Make sure to update the StreamReader test when updating this constant.
   static const int BSR_BUFFER_SIZE = 4096;

public:
   DLLLOCAL BufferedStreamReader(ExceptionSink* xsink, InputStream* is, const QoreEncoding* encoding = QCS_DEFAULT) :
      StreamReader(xsink, is, encoding),
      bufCapacity(BSR_BUFFER_SIZE),
      bufCount(0),
      buf(0) {
      if (!enc->isAsciiCompat()) {
         xsink->raiseException("UNSUPPORTED-ENCODING-ERROR", "BufferedStreamReader does not support ASCII-incompatible encodings");
         return;
      }

      // +1 is added to the real capacity for terminating null-character.
      buf = new char[bufCapacity + 1];
   }

   DLLLOCAL virtual ~BufferedStreamReader() {
      if (buf)
         delete [] buf;
   }

   DLLLOCAL virtual QoreStringNode* readLine(const QoreStringNode* eol = 0, bool trim = true, ExceptionSink* xsink = 0) override {
      SimpleRefHolder<QoreStringNode> eolstr;
      if (eol) {
         if (eol->getEncoding() == enc) {
            eolstr = eol->stringRefSelf();
         }
         else { // Create a temporary EOL string with correct encoding.
            eolstr = eol->convertEncoding(enc, xsink);
            if (*xsink)
               return 0;
         }
      }

      SimpleRefHolder<QoreStringNode> line(new QoreStringNode(enc));
      while (true) {
         int64 rc = fillBuffer(bufCapacity - bufCount, xsink);
         if (*xsink)
            return 0;
         if (bufCount == 0 && rc == 0)
            return line->empty() ? 0 : line.release();

         // Is done because of string searches in findEolInBuffer().
         // Won't overflow because the buffer actually has real capacity of bufCapacity+1.
         buf[bufCount] = '\0';

         qore_size_t eolLen;
         const char* p = findEolInBuffer(*eolstr, eolLen, rc==0);
         assert(eolLen >= 0);

         if (p) { // Found end of line.
            assert(p >= buf);
            qore_size_t dataSize = p - buf;
            line->concat(buf, dataSize + (trim ? 0 : eolLen));
            // Move remaining data from middle to the front of the buffer.
            bufCount -= dataSize + eolLen;
            memmove(buf, p + eolLen, bufCount);
            return line.release();
         }
         else {
            if (rc > 0) {
               qore_size_t dataSize = (bufCount >= eolLen) ? bufCount - eolLen : 0;
               line->concat(buf, dataSize);
               // Move remaining data from middle to the front of the buffer.
               assert(bufCount >= dataSize);
               bufCount -= dataSize;
               memmove(buf, buf + dataSize, bufCount);
               continue;
            }
            else if (rc == 0) { // At the end of stream.
               assert(bufCapacity != bufCount);
               line->concat(buf, bufCount);
               bufCount = 0;
               return line.release();
            }
         }
      }
   }

   //! Read binary data from the stream.
   /** @param limit max amount of data to read; if equal to -1, all data will be read, if equal to 0, no data will be read
    */
   DLLLOCAL virtual BinaryNode* readBinary(int64 limit, ExceptionSink* xsink) override {
      SimpleRefHolder<BinaryNode> b(new BinaryNode());
      if (limit >= 0 && static_cast<int64>(bufCount) >= limit) {
         b->append(buf, limit);
         shiftBuffer(limit);
         return b->empty() ? 0 : b.release();
      }
      else { // Either limit is -1 or we need more data than is currently in the buffer.
         int64 toRead = limit;
         b->append(buf, bufCount);
         toRead -= bufCount;
         bufCount = 0;
         while (true) {
            qore_size_t bytes = limit < 0 ? bufCapacity : QORE_MIN(static_cast<qore_size_t>(toRead), bufCapacity);
            int64 rc = fillBuffer(bytes, xsink);
            if (*xsink)
               return 0;
            if (rc == 0) // Input stream end.
               return b->empty() ? 0 : b.release();

            b->append(buf, bufCount);
            toRead -= bufCount;
            bufCount = 0;
            if (toRead == 0)
               return b.release();
         }
      }
   }

   //! Read string data from the stream.
   /** @param limit max amount of data to read; if equal to -1, all data will be read, if equal to 0, no data will be read
    */
   DLLLOCAL virtual QoreStringNode* readString(int64 limit, ExceptionSink* xsink) override {
      SimpleRefHolder<QoreStringNode> str(new QoreStringNode(enc));
      if (limit >= 0 && static_cast<int64>(bufCount) >= limit) {
         str->concat((const char*)buf, limit);
         shiftBuffer(limit);
         return str->empty() ? 0 : str.release();
      }
      else { // Either limit is -1 or we need more data than is currently in the buffer.
         int64 toRead = limit;
         str->concat((const char*)buf, bufCount);
         toRead -= bufCount;
         bufCount = 0;
         while (true) {
            qore_size_t bytes = limit < 0 ? bufCapacity : QORE_MIN(static_cast<qore_size_t>(toRead), bufCapacity);
            int64 rc = fillBuffer(bytes, xsink);
            if (*xsink)
               return 0;
            if (rc == 0) // Input stream end.
               return str->empty() ? 0 : str.release();

            str->concat((const char*)buf, bufCount);
            toRead -= bufCount;
            bufCount = 0;
            if (toRead == 0)
               return str.release();
         }
      }
   }

   DLLLOCAL virtual int64 readi1(ExceptionSink* xsink) override {
      char i = 0;
      if (!prepareEnoughData(1, xsink))
         return 0;
      i = buf[0];
      shiftBuffer(1);
      return i;
   }

   DLLLOCAL virtual int64 readi2(ExceptionSink* xsink) override {
      short i = 0;
      if (!prepareEnoughData(2, xsink))
         return 0;
      i = reinterpret_cast<short*>(buf)[0];
      shiftBuffer(2);
      i = ntohs(i);
      return i;
   }

   DLLLOCAL virtual int64 readi4(ExceptionSink* xsink) override {
      int32_t i = 0;
      if (!prepareEnoughData(4, xsink))
         return 0;
      i = reinterpret_cast<int32_t*>(buf)[0];
      shiftBuffer(4);
      i = ntohl(i);
      return i;
   }

   DLLLOCAL virtual int64 readi8(ExceptionSink* xsink) override {
      int64 i = 0;
      if (!prepareEnoughData(8, xsink))
         return 0;
      i = reinterpret_cast<int64*>(buf)[0];
      shiftBuffer(8);
      i = i8MSB(i);
      return i;
   }

   DLLLOCAL virtual int64 readi2LSB(ExceptionSink* xsink) override {
      short i = 0;
      if (!prepareEnoughData(2, xsink))
         return 0;
      i = reinterpret_cast<short*>(buf)[0];
      shiftBuffer(2);
      i = i2LSB(i);
      return i;
   }

   DLLLOCAL virtual int64 readi4LSB(ExceptionSink* xsink) override {
      int32_t i = 0;
      if (!prepareEnoughData(4, xsink))
         return 0;
      i = reinterpret_cast<int32_t*>(buf)[0];
      shiftBuffer(4);
      i = i4LSB(i);
      return i;
   }

   DLLLOCAL virtual int64 readi8LSB(ExceptionSink* xsink) override {
      int64 i = 0;
      if (!prepareEnoughData(8, xsink))
         return 0;
      i = reinterpret_cast<int64*>(buf)[0];
      shiftBuffer(8);
      i = i8LSB(i);
      return i;
   }

   DLLLOCAL virtual int64 readu1(ExceptionSink* xsink) override {
      unsigned char i = 0;
      if (!prepareEnoughData(1, xsink))
         return 0;
      i = reinterpret_cast<unsigned char*>(buf)[0];
      shiftBuffer(1);
      return i;
   }

   DLLLOCAL virtual int64 readu2(ExceptionSink* xsink) override {
      unsigned short i = 0;
      if (!prepareEnoughData(2, xsink))
         return 0;
      i = reinterpret_cast<unsigned short*>(buf)[0];
      shiftBuffer(2);
      i = ntohs(i);
      return i;
   }

   DLLLOCAL virtual int64 readu4(ExceptionSink* xsink) override {
      uint32_t i = 0;
      if (!prepareEnoughData(4, xsink))
         return 0;
      i = reinterpret_cast<uint32_t*>(buf)[0];
      shiftBuffer(4);
      i = ntohl(i);
      return i;
   }

   DLLLOCAL virtual int64 readu2LSB(ExceptionSink* xsink) override {
      unsigned short i = 0;
      if (!prepareEnoughData(2, xsink))
         return 0;
      i = reinterpret_cast<unsigned short*>(buf)[0];
      shiftBuffer(2);
      i = i2LSB(i);
      return i;
   }

   DLLLOCAL virtual int64 readu4LSB(ExceptionSink* xsink) override {
      uint32_t i = 0;
      if (!prepareEnoughData(4, xsink))
         return 0;
      i = reinterpret_cast<uint32_t*>(buf)[0];
      shiftBuffer(4);
      i = i4LSB(i);
      return i;
   }

   DLLLOCAL virtual const char* getName() const override { return "BufferedStreamReader"; }

private:
   DLLLOCAL int64 fillBuffer(qore_size_t bytes, ExceptionSink* xsink) {
      assert(bufCount + bytes <= bufCapacity);
      int64 rc = in->read(buf + bufCount, bytes, xsink);
      if (*xsink)
         return 0;
      bufCount += rc;
      return rc;
   }

   DLLLOCAL bool prepareEnoughData(qore_size_t bytes, ExceptionSink* xsink) {
      assert(bytes <= bufCapacity);
      if (bufCount < bytes) {
         while (true) {
            int64 rc = fillBuffer(bytes - bufCount, xsink);
            if (*xsink) {
               return false;
            }
            if (rc == 0) {
               xsink->raiseException("END-OF-STREAM-ERROR", "there is not enough data available in the stream");
               return false;
            }
            if (bufCount >= bytes)
               break;
         }
      }
      return true;
   }

   DLLLOCAL void shiftBuffer(qore_size_t bytes) {
      assert(bytes <= bufCount);
      bufCount -= bytes;
      memmove(buf, buf+bytes, bufCount);
   }

   //! Try to find EOL in buffer and write it's size to eolLen parameter.
   /** @param eol end-of-line string
       @param eolLen size of eol in bytes
       @param endOfStream whether this is end of the stream
    */
   DLLLOCAL const char* findEolInBuffer(const QoreStringNode* eol, qore_size_t& eolLen, bool endOfStream) const {
      if (eol) {
         const char* p = strstr(buf, eol->getBuffer());
         eolLen = eol->strlen();
         return p;
      }
      else {
         const char* p = strpbrk(buf, "\n\r"); // Find first occurence of '\n' or '\r'.
         if (p) { // Found end of line.
            if (*p == '\n') {
               eolLen = 1;
            }
            else { // *p == '\r'
               if (*(p+1) == '\n') {
                  eolLen = 2;
               }
               // '\r' is the last character in the buffer, '\n' could be next in the stream.
               // Unless this is the end of the stream.
               else if (static_cast<qore_size_t>(p - buf + 1) == bufCount) {
                  eolLen = 1;
                  if (!endOfStream)
                     p = 0;
               }
               else {
                  eolLen = 1;
               }
            }
         }
         else {
            eolLen = 0;
         }
         return p;
      }
   }

private:
   qore_size_t bufCapacity; //! Total capacity of buf.
   qore_size_t bufCount; //! Current size of data in buf.
   char* buf;
};

#endif // _QORE_BUFFEREDSTREAMREADER_H
