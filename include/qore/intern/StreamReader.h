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
   DLLLOCAL virtual BinaryNode* readBinary(int64 limit, ExceptionSink* xsink) {
      if (limit == 0)
         return 0;
      SimpleRefHolder<BinaryNode> b(new BinaryNode());
      char buffer[STREAMREADER_BUFFER_SIZE];
      if (limit == -1) {
         while (true) {
            int rc = readDataLimit(buffer, STREAMREADER_BUFFER_SIZE, xsink);
            if (*xsink)
               return 0;
            if (rc == 0)
               break;
            b->append(buffer, rc);
         }
      }
      else {
         while (limit > 0) {
            int rc = readDataLimit(buffer, QORE_MIN(limit, STREAMREADER_BUFFER_SIZE), xsink);
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
   /** @param limit max amount of data to read as a number of characters; if equal to -1, all data will be read, if equal to 0, no data will be read
       @param xsink exception sink

       @return Qore string read from the stream
    */
   DLLLOCAL virtual QoreStringNode* readString(int64 limit, ExceptionSink* xsink) {
      if (limit == 0)
         return 0;
      SimpleRefHolder<QoreStringNode> str(new QoreStringNode(enc));
      char buffer[STREAMREADER_BUFFER_SIZE];
      if (limit == -1) {
         while (true) {
            int rc = readDataLimit(buffer, STREAMREADER_BUFFER_SIZE, xsink);
            if (*xsink)
               return 0;
            if (rc == 0)
               break;
            str->concat(buffer, rc);
         }
      }
      else {
         while (limit > 0) {
            int rc = readDataLimit(buffer, QORE_MIN(limit, STREAMREADER_BUFFER_SIZE), xsink);
            if (*xsink)
               return 0;
            if (rc == 0)
               break;
            str->concat(buffer, rc);
            limit -= rc;
         }
      }

      return str->empty() ? 0 : str.release();
   }

   //! Read one line.
   /** @param eol end-of-line symbol, only '\n', '\r' and '\r\n' are supported
       @param trim whether to trim the EOL symbols
       @param xsink exception sink

       @return Qore string read from the stream
    */
   DLLLOCAL virtual QoreStringNode* readLine(const QoreStringNode* eol = 0, bool trim = true, ExceptionSink* xsink = 0) {
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

      SimpleRefHolder<QoreStringNode> str(new QoreStringNode(enc));
      char buffer[STREAMREADER_BUFFER_SIZE];
      qore_size_t bufCount = 0;
      if (eolstr) {
         while (true) {
            int64 rc = in->read(buffer + bufCount, 1, xsink);
            if (*xsink)
               return 0;
            if (rc == 0) { // End of stream.
               str->concat(buffer, bufCount);
               return str->empty() ? 0 : str.release();
            }
            if (buffer[bufCount] == eolstr->c_str()[0]) {
               if (eolstr->size() > 1) {
                  int64 p = in->peek(xsink);
                  if (*xsink)
                     return 0;
                  if (p == eolstr->c_str()[1]) {
                     char c2;
                     rc = in->read(&c2, 1, xsink);
                     if (*xsink)
                        return 0;
                     if (trim) {
                        str->concat(buffer, bufCount);
                     }
                     else {
                        str->concat(buffer, bufCount + 1);
                        str->concat(c2);
                     }
                     return str.release();
                  }
               }
               else {
                  if (trim)
                     str->concat(buffer, bufCount);
                  else
                     str->concat(buffer, bufCount + 1);
                  return str.release();
               }
               if (*xsink)
                  return 0;
            }
            ++bufCount;
            if (bufCount == STREAMREADER_BUFFER_SIZE) {
               str->concat(buffer, bufCount);
               bufCount = 0;
            }
         }
      }
      else {
         while (true) {
            int64 rc = in->read(buffer + bufCount, 1, xsink);
            if (*xsink)
               return 0;
            if (rc == 0) { // End of stream.
               str->concat(buffer, bufCount);
               return str->empty() ? 0 : str.release();
            }
            char c = buffer[bufCount];
            if (c == '\n') {
               if (trim)
                  str->concat(buffer, bufCount);
               else
                  str->concat(buffer, bufCount + 1);
               return str.release();
            }
            else if (c == '\r') {
               int64 p = in->peek(xsink);
               if (*xsink)
                  return 0;
               if (p == '\n') {
                  if (trim)
                     --bufCount;
               }
               else {
                  if (trim)
                     str->concat(buffer, bufCount);
                  else
                     str->concat(buffer, bufCount + 1);
                  return str.release();
               }
            }
            ++bufCount;
            if (bufCount == STREAMREADER_BUFFER_SIZE) {
               str->concat(buffer, bufCount);
               bufCount = 0;
            }
         }
      }
   }

   DLLLOCAL virtual int64 readi1(ExceptionSink* xsink) {
      char i = 0;
      if (readData(&i, 1, xsink))
         return 0;
      return i;
   }

   DLLLOCAL virtual int64 readi2(ExceptionSink* xsink) {
      short i = 0;
      if (readData(&i, 2, xsink))
         return 0;
      i = ntohs(i);
      return i;
   }

   DLLLOCAL virtual int64 readi4(ExceptionSink* xsink) {
      int32_t i = 0;
      if (readData(&i, 4, xsink))
         return 0;
      i = ntohl(i);
      return i;
   }

   DLLLOCAL virtual int64 readi8(ExceptionSink* xsink) {
      int64 i = 0;
      if (readData(&i, 8, xsink))
         return 0;
      i = i8MSB(i);
      return i;
   }

   DLLLOCAL virtual int64 readi2LSB(ExceptionSink* xsink) {
      short i = 0;
      if (readData(&i, 2, xsink))
         return 0;
      i = i2LSB(i);
      return i;
   }

   DLLLOCAL virtual int64 readi4LSB(ExceptionSink* xsink) {
      int32_t i = 0;
      if (readData(&i, 4, xsink))
         return 0;
      i = i4LSB(i);
      return i;
   }

   DLLLOCAL virtual int64 readi8LSB(ExceptionSink* xsink) {
      int64 i = 0;
      if (readData(&i, 8, xsink))
         return 0;
      i = i8LSB(i);
      return i;
   }

   DLLLOCAL virtual int64 readu1(ExceptionSink* xsink) {
      unsigned char i = 0;
      if (readData(&i, 1, xsink))
         return 0;
      return i;
   }

   DLLLOCAL virtual int64 readu2(ExceptionSink* xsink) {
      unsigned short i = 0;
      if (readData(&i, 2, xsink))
         return 0;
      i = ntohs(i);
      return i;
   }

   DLLLOCAL virtual int64 readu4(ExceptionSink* xsink) {
      uint32_t i = 0;
      if (readData(&i, 4, xsink))
         return 0;
      i = ntohl(i);
      return i;
   }

   DLLLOCAL virtual int64 readu2LSB(ExceptionSink* xsink) {
      unsigned short i = 0;
      if (readData(&i, 2, xsink))
         return 0;
      i = i2LSB(i);
      return i;
   }

   DLLLOCAL virtual int64 readu4LSB(ExceptionSink* xsink) {
      uint32_t i = 0;
      if (readData(&i, 4, xsink))
         return 0;
      i = i4LSB(i);
      return i;
   }

   DLLLOCAL virtual const char* getName() const { return "StreamReader"; }

protected:
   // Make sure to update the StreamReader tests when updating this constant.
   static const int STREAMREADER_BUFFER_SIZE = 4096;

   //! Source input stream.
   ReferenceHolder<InputStream> in;

   //! Encoding of the source input stream.
   const QoreEncoding* enc;

private:
   //! Read all the required data.
   /** @param dest destination buffer
       @param bytes amount of data to read
       @param xsink exception sink

       @return 0 if all the data has been read, -1 in case of error or if not enough data was available
    */
   DLLLOCAL int readData(void* dest, qore_size_t bytes, ExceptionSink* xsink) {
      char* destPtr = static_cast<char*>(dest);
      qore_size_t read = 0;
      if (bytes > 0) {
         while (true) {
            int64 rc = in->read(destPtr + read, bytes - read, xsink);
            if (*xsink)
               return -1;
            if (rc == 0) {
               xsink->raiseException("END-OF-STREAM-ERROR", "there is not enough data available in the stream");
               return -1;
            }
            read += rc;
            if (read == bytes)
               break;
         }
      }
      return 0;
   }

   //! Read data until a limit.
   /** @param dest destination buffer
       @param limit maximum amount of data to read
       @param xsink exception sink

       @return amount of data read, -1 in case of error
    */
   DLLLOCAL int readDataLimit(void* dest, qore_size_t limit, ExceptionSink* xsink) {
      char* destPtr = static_cast<char*>(dest);
      qore_size_t read = 0;
      if (limit > 0) {
         while (true) {
            int64 rc = in->read(destPtr + read, limit - read, xsink);
            if (*xsink)
               return -1;
            read += rc;
            if (read == limit || rc == 0)
               break;
         }
      }
      return read;
   }
};

#endif // _QORE_STREAMREADER_H
