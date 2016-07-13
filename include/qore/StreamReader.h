/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  StreamReader.h

  Qore Programming Language

  Copyright (C) 2016 Qore Technologies, sro

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

/**
 * @brief Private data for the Qore::StreamReader class.
 */
class StreamReader : public AbstractPrivateData {

private:
   // Make sure to update the StreamReader test when updating this constant.
   static const int STREAMREADER_BUFFER_SIZE = 4096;

public:
   DLLLOCAL StreamReader(ExceptionSink* xsink, InputStream* is, const QoreEncoding* encoding = QCS_DEFAULT) :
      in(is, xsink),
      enc(encoding),
      bufCapacity(STREAMREADER_BUFFER_SIZE),
      bufSize(0),
      buf(0) {
      is->ref();
      if (!enc->isAsciiCompat()) {
         xsink->raiseException("WRONG-ENCODING-ERROR", "StreamReader does not support ASCII-incompatible encodings");
         return;
      }

      // +1 is added to the real capacity for terminating null-character.
      buf = new char[bufCapacity + 1];
   }

   DLLLOCAL const QoreEncoding* getEncoding() const {
      return enc;
   }

   DLLLOCAL QoreStringNode* readLine(const QoreStringNode* eol = 0, bool trim = true, ExceptionSink* xsink = 0) {
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
         int64 rc = fillBuffer(bufCapacity - bufSize, xsink);
         if (*xsink)
            return 0;
         if (bufSize == 0 && rc == 0)
            return line->empty() ? 0 : line.release();

         // Is done because of string searches in findEolInBuffer().
         // Won't overflow because the buffer actually has real capacity of bufCapacity+1.
         buf[bufSize] = '\0';

         qore_size_t eolLen;
         const char* p = findEolInBuffer(*eolstr, eolLen, rc==0);
         assert(eolLen >= 0);

         if (p) { // Found end of line.
            assert(p >= buf);
            qore_size_t dataSize = p - buf;
            line->concat(buf, dataSize + (trim ? 0 : eolLen));
            // Move remaining data from middle to the front of the buffer.
            bufSize -= dataSize + eolLen;
            memmove(buf, p + eolLen, bufSize);
            return line.release();
         }
         else {
            if (rc > 0) {
               qore_size_t dataSize = (bufSize >= eolLen) ? bufSize - eolLen : 0;
               line->concat(buf, dataSize);
               // Move remaining data from middle to the front of the buffer.
               assert(bufSize >= dataSize);
               bufSize -= dataSize;
               memmove(buf, buf + dataSize, bufSize);
               continue;
            }
            else if (rc == 0) { // At the end of stream.
               assert(bufCapacity != bufSize);
               line->concat(buf, bufSize);
               bufSize = 0;
               return line.release();
            }
         }
      }
   }

   //! Read binary data from the stream.
   /** @param limit max amount of data to read; if equal to -1, all data will be read, if equal to 0, no data will be read
    */
   DLLLOCAL BinaryNode* readBinary(int64 limit, ExceptionSink* xsink) {
      SimpleRefHolder<BinaryNode> b(new BinaryNode());
      if (limit >= 0 && static_cast<int64>(bufSize) >= limit) {
         b->append(buf, limit);
         shiftBuffer(limit);
         return b->empty() ? 0 : b.release();
      }
      else { // Either limit is -1 or we need more data than is currently in the buffer.
         int64 toRead = limit;
         b->append(buf, bufSize);
         toRead -= bufSize;
         bufSize = 0;
         while (true) {
            qore_size_t bytes = limit < 0 ? bufCapacity : QORE_MIN(static_cast<qore_size_t>(toRead), bufCapacity);
            int64 rc = fillBuffer(bytes, xsink);
            if (*xsink)
               return 0;
            if (rc == 0) // Input stream end.
               return b->empty() ? 0 : b.release();

            b->append(buf, bufSize);
            toRead -= bufSize;
            bufSize = 0;
            if (toRead == 0)
               return b.release();
         }
      }
   }

   DLLLOCAL int64 readi1(ExceptionSink* xsink) {
      char i = 0;
      if (!prepareEnoughData(1, xsink))
         return 0;
      i = buf[0];
      shiftBuffer(1);
      return i;
   }

   DLLLOCAL int64 readi2(ExceptionSink* xsink) {
      short i = 0;
      if (!prepareEnoughData(2, xsink))
         return 0;
      i = reinterpret_cast<short*>(buf)[0];
      shiftBuffer(2);
      i = ntohs(i);
      return i;
   }

   DLLLOCAL int64 readi4(ExceptionSink* xsink) {
      int32_t i = 0;
      if (!prepareEnoughData(4, xsink))
         return 0;
      i = reinterpret_cast<int32_t*>(buf)[0];
      shiftBuffer(4);
      i = ntohl(i);
      return i;
   }

   DLLLOCAL int64 readi8(ExceptionSink* xsink) {
      int64 i = 0;
      if (!prepareEnoughData(8, xsink))
         return 0;
      i = reinterpret_cast<int64*>(buf)[0];
      shiftBuffer(8);
      i = i8MSB(i);
      return i;
   }

   DLLLOCAL int64 readi2LSB(ExceptionSink* xsink) {
      short i = 0;
      if (!prepareEnoughData(2, xsink))
         return 0;
      i = reinterpret_cast<short*>(buf)[0];
      shiftBuffer(2);
      i = i2LSB(i);
      return i;
   }

   DLLLOCAL int64 readi4LSB(ExceptionSink* xsink) {
      int32_t i = 0;
      if (!prepareEnoughData(4, xsink))
         return 0;
      i = reinterpret_cast<int32_t*>(buf)[0];
      shiftBuffer(4);
      i = i4LSB(i);
      return i;
   }

   DLLLOCAL int64 readi8LSB(ExceptionSink* xsink) {
      int64 i = 0;
      if (!prepareEnoughData(8, xsink))
         return 0;
      i = reinterpret_cast<int64*>(buf)[0];
      shiftBuffer(8);
      i = i8LSB(i);
      return i;
   }

   DLLLOCAL int64 readu1(ExceptionSink* xsink) {
      unsigned char i = 0;
      if (!prepareEnoughData(1, xsink))
         return 0;
      i = reinterpret_cast<unsigned char*>(buf)[0];
      shiftBuffer(1);
      return i;
   }

   DLLLOCAL int64 readu2(ExceptionSink* xsink) {
      unsigned short i = 0;
      if (!prepareEnoughData(2, xsink))
         return 0;
      i = reinterpret_cast<unsigned short*>(buf)[0];
      shiftBuffer(2);
      i = ntohs(i);
      return i;
   }

   DLLLOCAL int64 readu4(ExceptionSink* xsink) {
      uint32_t i = 0;
      if (!prepareEnoughData(4, xsink))
         return 0;
      i = reinterpret_cast<uint32_t*>(buf)[0];
      shiftBuffer(4);
      i = ntohl(i);
      return i;
   }

   DLLLOCAL int64 readu2LSB(ExceptionSink* xsink) {
      unsigned short i = 0;
      if (!prepareEnoughData(2, xsink))
         return 0;
      i = reinterpret_cast<unsigned short*>(buf)[0];
      shiftBuffer(2);
      i = i2LSB(i);
      return i;
   }

   DLLLOCAL int64 readu4LSB(ExceptionSink* xsink) {
      uint32_t i = 0;
      if (!prepareEnoughData(4, xsink))
         return 0;
      i = reinterpret_cast<uint32_t*>(buf)[0];
      shiftBuffer(4);
      i = i4LSB(i);
      return i;
   }

   DLLLOCAL virtual const char* getName() const { return "StreamReader"; }

private:
   DLLLOCAL int64 fillBuffer(qore_size_t bytes, ExceptionSink* xsink) {
      assert(bufSize + bytes <= bufCapacity);
      int64 rc = in->read(buf + bufSize, bytes, xsink);
      bufSize += rc;
      return rc;
   }

   DLLLOCAL bool prepareEnoughData(qore_size_t bytes, ExceptionSink* xsink) {
      assert(bytes <= bufCapacity);
      if (bufSize < bytes) {
         while (true) {
            int64 rc = fillBuffer(bytes - bufSize, xsink);
            if (rc == 0) {
               xsink->raiseException("END-OF-STREAM-ERROR", "there is not enough data available in the stream");
               return false;
            }
            if (bufSize >= bytes)
               break;
         }
      }
      return true;
   }

   DLLLOCAL void shiftBuffer(qore_size_t bytes) {
      assert(bytes <= bufSize);
      bufSize -= bytes;
      memmove(buf, buf+bytes, bufSize);
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
               else if (static_cast<qore_size_t>(p - buf + 1) == bufSize) {
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
   ReferenceHolder<InputStream> in;
   const QoreEncoding* enc;
   QoreStringNode* line;
   QoreStringNode* eol;
   int64 num;
   bool validp;
   bool trim;
   qore_size_t bufCapacity; //! Total capacity of buf.
   qore_size_t bufSize; //! Current size of data in buf.
   char* buf;
};

#endif // _QORE_STREAMREADER_H
