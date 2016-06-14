/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  InputStreamLineIterator.h

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

#ifndef _QORE_INPUTSTREAMLINEITERATOR_H
#define _QORE_INPUTSTREAMLINEITERATOR_H

#include <string.h>
#include <errno.h>
#include <stdio.h>

#include "qore/InputStream.h"

const int __INPUTSTREAMLINEITERATOR_BUFFER_SIZE = 4096;

/**
 * @brief Private data for the Qore::InputStreamLineIterator class.
 */
class InputStreamLineIterator : public QoreIteratorBase {

public:
   DLLLOCAL InputStreamLineIterator(ExceptionSink* xsink, InputStream* is, QoreStringNode* n_eol = 0, bool n_trim = true) :
      src(is, xsink),
      line(new QoreStringNode(QCS_UTF8)),
      eol(n_eol),
      num(0),
      validp(false),
      trim(n_trim),
      bufCapacity(__INPUTSTREAMLINEITERATOR_BUFFER_SIZE),
      bufSize(0),
      buf(0) {
      // +1 is added to the real capacity for terminating null-character.
      buf = new char[bufCapacity + 1];
   }

   DLLLOCAL ~InputStreamLineIterator() {
      if (eol)
         eol->deref();
      if (line)
         line->deref();

      if (buf)
         delete [] buf;
   }

   DLLLOCAL bool getLine(ExceptionSink* xsink) {
      while (true) {
         int64 rc = src->bulkRead(buf + bufSize, bufCapacity - bufSize, xsink);
         if (bufSize == 0 && rc == 0) {
            if (line->empty())
               return false;
            else
               return true;
         }
         bufSize += rc;

         // Is done because of string searches in findEolInBuffer().
         // Won't overflow because the buffer actually has real capacity of bufCapacity+1.
         buf[bufSize] = '\0';

         qore_size_t eolLen;
         const char* p = findEolInBuffer(eolLen, rc==0);
         assert(eolLen >= 0);

         if (p) { // Found end of line.
            qore_size_t dataSize = p - buf;
            line->concat(buf, dataSize + (trim ? 0 : eolLen));
            // Move remaining data from middle to the front of the buffer.
            bufSize -= dataSize + eolLen;
            memmove(buf, p + eolLen, bufSize);
            return true;
         }
         else {
            if (rc > 0) {
               qore_size_t dataSize = bufSize - eolLen;
               line->concat(buf, dataSize);
               // Move remaining data from middle to the front of the buffer.
               bufSize -= dataSize;
               memmove(buf, buf + dataSize, bufSize);
               continue;
            }
            else if (rc == 0) { // At the end of stream.
               assert(bufCapacity != bufSize);
               line->concat(buf, bufSize);
               bufSize = 0;
               return true;
            }
         }
      }
   }

   DLLLOCAL bool next(ExceptionSink* xsink) {
      // Make sure to use a new string if the iterator was already valid.
      if (validp && !line->empty()) {
         line->deref();
         line = new QoreStringNode(QCS_UTF8);
      }
      validp = getLine(xsink);
      if (validp) {
         ++num;   // Increment line number.
      }
      else {
         num = 0;   // Reset iterator.
      }
      //printd(5, "InputStreamLineIterator::next() this: %p line: %d offset: %lld validp: %d '%s'\n", this, num, offset, validp, line->getBuffer());
      return validp;
   }

   DLLLOCAL int64 index() const {
      return num;
   }

   DLLLOCAL QoreStringNode* getValue() {
      assert(validp);
      return line->stringRefSelf();
   }

   DLLLOCAL bool valid() const {
      return validp;
   }

   DLLLOCAL int checkValid(ExceptionSink* xsink) const {
      if (!validp) {
         xsink->raiseException("ITERATOR-ERROR", "the %s is not pointing at a valid element; make sure %s::next() returns True before calling this method", getName(), getName());
         return -1;
      }
      return 0;
   }

   DLLLOCAL virtual void deref() {
      if (ROdereference())
         delete this;
   }

   DLLLOCAL virtual const char* getName() const { return "InputStreamLineIterator"; }

private:
   //! Try to find EOL in buffer and write it's size to eolLen parameter.
   /** Try to find EOL in buffer and write it's size to eolLen parameter.
       @param eolLen size of eol in bytes
       @param endOfStream whether this is end of the stream
    */
   DLLLOCAL const char* findEolInBuffer(qore_size_t& eolLen, bool endOfStream) const {
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
   ReferenceHolder<InputStream> src;
   QoreStringNode* line;
   QoreStringNode* eol;
   int64 num;
   bool validp;
   bool trim;
   qore_size_t bufCapacity; //! Total capacity of buf.
   qore_size_t bufSize; //! Current size of data in buf.
   char* buf;
};

#endif // _QORE_INPUTSTREAMLINEITERATOR_H
