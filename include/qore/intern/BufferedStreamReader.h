/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  BufferedStreamReader.h

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

#ifndef _QORE_BUFFEREDSTREAMREADER_H
#define _QORE_BUFFEREDSTREAMREADER_H

#include <cstdint>

#include "qore/qore_bitopts.h"
#include "qore/InputStream.h"
#include "qore/intern/StreamReader.h"

// this corresponds to the Qore constant size in QC_BufferedStreamReader; these values must be identical
#define DefaultStreamBufferSize 4096

//! Private data for the Qore::BufferedStreamReader class.
class BufferedStreamReader : public StreamReader {
public:
   DLLLOCAL BufferedStreamReader(ExceptionSink* xsink, InputStream* is, const QoreEncoding* encoding, int64 bufsize = DefaultStreamBufferSize) :
      StreamReader(xsink, is, encoding),
      bufCapacity((size_t)bufsize),
      bufCount(0),
      buf(0) {
      if (bufsize <= 0) {
         xsink->raiseException("STREAM-BUFFER-ERROR", "the buffer size must be > 0 (value provided: " QLLD ")", bufsize);
         return;
      }
      if (!enc->isAsciiCompat()) {
         xsink->raiseException("UNSUPPORTED-ENCODING-ERROR", "BufferedStreamReader only supports ASCII-compatible encodings (encoding provided: '%s')", enc->getCode());
         return;
      }

      // +1 is added to the real capacity for terminating null-character.
      buf = new char[bufCapacity + 1];
   }

   DLLLOCAL virtual ~BufferedStreamReader() {
      if (buf)
         delete [] buf;
   }

   DLLLOCAL int check(const QoreStringNode* eolstr, ExceptionSink* xsink) {
      if (eolstr) {
         if (eolstr->size() > bufCapacity) {
            xsink->raiseException("EOL-BUFFER-ERROR", "BufferedStreamReader has a buffer size of " QLLD " while the eol string has a size of " QLLD "; the eol string size must be less than the buffer size", bufCapacity, eolstr->size());
            return -1;
         }
      }
      else if (bufCapacity < 2) {
         xsink->raiseException("EOL-BUFFER-ERROR", "BufferedStreamReader has a buffer size of " QLLD " which is too small to process a maximum eol string of 2; either set an explicit EOL character or increase the buffer size to 2 or more", bufCapacity);
         return -1;
      }
      return 0;
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
      if (check(*eolstr, xsink))
         return 0;

      SimpleRefHolder<QoreStringNode> line(new QoreStringNode(enc));
      while (true) {
         assert(bufCapacity != bufCount);
         int64 rc = fillBuffer(bufCapacity - bufCount, xsink);
         if (*xsink)
            return 0;
         if (bufCount == 0 && rc == 0)
            return line->empty() ? 0 : line.release();

         // Is done because of string searches in findEolInBuffer().
         // Won't overflow because the buffer actually has real capacity of bufCapacity+1.
         buf[bufCount] = '\0';

         //printd(5, "BufferedStreamReader::readLine() buf: '%s' bufCount: " QLLD "\n", buf, bufCount);

         qore_size_t eolLen;
         char this_pmatch;
         const char* p = findEolInBuffer(*eolstr, eolLen, rc==0, this_pmatch);
         assert(eolLen >= 0);

         if (p) { // Found end of line.
            assert(p >= buf);
            qore_size_t dataSize = p - buf;
            line->concat(buf, dataSize + (trim ? 0 : eolLen));
            assert(bufCount >= (dataSize + eolLen));
            // Move remaining data to the front of the buffer.
            bufCount -= dataSize + eolLen;
            if (bufCount)
               memmove(buf, p + eolLen, bufCount);
            return line.release();
         }
         else {
            if (rc > 0) {
               if (eol) {
                  // we read in new data, so here we need to look for a partial eol match at the end of the buffer
                  // and move the buffer data accordingly
                  bool moved = false;
                  for (size_t i = eolLen; i; --i) {
                     p = buf + bufCount - i;
                     if (!strncmp(p, eolstr->c_str(), i)) {
                        size_t dataSize = p - buf;
                        assert(dataSize);
                        assert((bufCount - dataSize) == i);
                        //printd(5, "BufferedStreamReader::readLine() found eol partial match: buf: '%s' eol: '%s' i: " QLLD " p: '%s' consuming " QLLD " byte(s), leaving " QLLD " byte(s) in the buffer\n", buf, eolstr->c_str(), i, p, dataSize, bufCount - dataSize);
                        line->concat(buf, dataSize);
                        // Move remaining data to the front of the buffer.
                        assert(bufCount > dataSize);
                        bufCount = i;
                        memmove(buf, buf + dataSize, bufCount);
                        moved = true;
                        break;
                     }
                  }
                  if (!moved) {
                     //printd(5, "BufferedStreamReader::readLine() no eol partial match; consuming entire buffer '%s'\n", buf);
                     line->concat(buf, bufCount);
                     bufCount = 0;
                  }
                  continue;
               }
               else if (this_pmatch) {
                  // we read in new data and we got a possible partial \r\n match with a trailing \r character
                  // so move the final character to the beginning of the buffer and continue processing
                  --bufCount;
                  assert(bufCount);
                  //printd(5, "BufferedStreamReader::readLine() found eol partial match: buf: '%s' eol: '%s' i: " QLLD " p: '%s' consuming " QLLD " byte(s), leaving " QLLD " byte(s) in the buffer\n", buf, eolstr->c_str(), i, p, dataSize, bufCount - dataSize);
                  line->concat(buf, bufCount);
                  bufCount = 1;
                  buf[0] = this_pmatch;
                  continue;
               }
               // we must process at least one byte
               qore_size_t dataSize = (bufCount > eolLen) ? bufCount - eolLen : 1;
               assert(dataSize);
               line->concat(buf, dataSize);
               // Move remaining data to the front of the buffer.
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
   /** @param size max amount of data to read as a number of characters; if equal to -1, all data will be read, if equal to 0, no data will be read
       @param xsink exception sink

       @return Qore string read from the stream
    */
   DLLLOCAL virtual QoreStringNode* readString(int64 size, ExceptionSink* xsink) override {
      if (size == 0)
         return 0;
      if (size < 0)
         return readStringAll(xsink);

      // original number of characters requested
      size_t orig_size = size;

      // byte offset of the byte position directly after the last full character scanned
      size_t last_char = 0;

      // total number of characters read
      size_t char_len = 0;

      SimpleRefHolder<QoreStringNode> str(new QoreStringNode(enc));
      while (char_len < orig_size) {
         // get the minimum number of bytes to read
         size_t bs = size - str->size();

         // ensure there is space in the buffer
         str->reserve(str->size() + bs);

         int rc = readBufferedDataLimit((void*)(str->c_str() + str->size()), bs, xsink);
         if (*xsink)
            return 0;
         if (rc == 0)
            break;

         str->terminate(str->size() + rc);

         // if we have a non-multi-byte character encoding, then we can use byte lengths
         if (!enc->isMultiByte()) {
            if (size == str->size())
               break;
            continue;
         }

         // scan data read and find the last valid character position
         const char* e = str->c_str() + str->size();
         while (char_len < orig_size && last_char < str->size()) {
            const char* p = str->c_str() + last_char;
            int cc = enc->getCharLen(p, e - p);
            if (!cc) {
               xsink->raiseException("STREAM-ENCODING-ERROR", "invalid multi-byte character received in byte offset " QLLD " according to the file's encoding: '%s'", last_char, enc->getCode());

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
            assert(cc > hb);
            // we will add one byte for the missing character below; here we add in any other bytes we might need
            if (cc > (hb + 1))
               size += (cc - hb - 1);
            break;
         }

         // now we add 1 byte to the remaining size to get for every character we have not yet read
         size += (orig_size - char_len);
      }

      return str->empty() ? 0 : str.release();
   }

   DLLLOCAL QoreStringNode* readStringAll(ExceptionSink* xsink) {
      SimpleRefHolder<QoreStringNode> str(new QoreStringNode(enc));
      str->concat((const char*)buf, bufCount);
      bufCount = 0;
      while (true) {
         qore_size_t bytes = bufCapacity;
         int64 rc = fillBuffer(bytes, xsink);
         if (*xsink)
            return 0;
         if (rc == 0) // Input stream end.
            return str->empty() ? 0 : str.release();

         str->concat((const char*)buf, bufCount);
         bufCount = 0;
      }
   }

   //! Read buffered and then stream data until a limit.
   /** @param dest destination buffer
       @param limit maximum amount of data to read
       @param xsink exception sink

       @return amount of data read, -1 in case of error
    */
   DLLLOCAL qore_offset_t readBufferedDataLimit(void* dest, qore_size_t limit, ExceptionSink* xsink) {
      size_t read = 0;
      if (bufCount) {
         size_t len = QORE_MIN(limit, bufCount);
         memcpy(dest, buf, len);
         bufCount -= len;
         limit -= len;
         read = len;
         dest = (char*)dest + len;
      }
      if (!limit)
         return read;
      // if we have data still left to read, we read it directly into the target buffer
      qore_offset_t rc = readDataLimit(dest, limit, xsink);
      return rc < 0 ? rc : read + rc;
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
   //! returns 0 = no data read (end of stream or error), > 0 = number of bytes read, increments bufCount
   DLLLOCAL int64 fillBuffer(qore_size_t bytes, ExceptionSink* xsink) {
      assert(bytes);
      assert(bufCount + bytes <= bufCapacity);
      int64 rc = in->read(buf + bufCount, bytes, xsink);
      if (*xsink)
         return 0;
      bufCount += rc;
      return rc;
   }

   DLLLOCAL bool prepareEnoughData(qore_size_t bytes, ExceptionSink* xsink) {
      if (bytes > bufCapacity) {
         xsink->raiseException("STREAM-BUFFER-ERROR", "a read of " QLLD " bytes was attempted on a BufferedStreamReader with a capacity of " QLLD " bytes", bytes, bufCapacity);
         return false;
      }
      if (bufCount < bytes) {
         while (true) {
            int64 rc = fillBuffer(bytes - bufCount, xsink);
            if (*xsink) {
               return false;
            }
            if (rc == 0) {
               xsink->raiseException("END-OF-STREAM-ERROR", "a read of " QLLD " bytes cannot be performed because there is not enough data available in the stream to satisfy the request", bytes);
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
   DLLLOCAL const char* findEolInBuffer(const QoreStringNode* eol, qore_size_t& eolLen, bool endOfStream, char& pmatch) const {
      pmatch = '\0';
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
                  if (!endOfStream) {
                     pmatch = *p;
                     p = 0;
                  }
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
