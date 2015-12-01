/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  AbstractPrivateData.h

  abstract class for private data in objects

  Qore Programming Language

  Copyright (C) 2003 - 2015 David Nichols

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

#ifndef _QORE_ABSTRACTPRIVATEDATA_H

#define _QORE_ABSTRACTPRIVATEDATA_H

#include <qore/QoreReferenceCounter.h>

//! the base class for all data to be used as private data of Qore objects
/** C++ constructor code for Qore classes must set private data of the class
    against the class ID using QoreObject::setPrivate()
 */
class AbstractPrivateData : public QoreReferenceCounter {
protected:
   //! as these objects are reference counted, the destructor should be called only when the reference count = 0 and not manually
   DLLLOCAL virtual ~AbstractPrivateData() {}

public:
   //! increments the reference count of the object
   /** FIXME: this function should be const
    */
   DLLLOCAL void ref() {
      ROreference();
   }

   //! decrements the reference count of the object
   /**
      @param xsink any Qore-language exception information is stored here
   */
   DLLLOCAL virtual void deref(ExceptionSink* xsink) {
      if (ROdereference())
         delete this;
   }

   //! decrements the reference count of the object without the possibility of throwing a Qore-language exception
   DLLLOCAL virtual void deref() {
      if (ROdereference())
         delete this;
   }
};

#endif // _QORE_ABSTRACTPRIVATEDATA_H
