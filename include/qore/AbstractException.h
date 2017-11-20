/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  AbstractException.h

  Qore Programming Language

  Copyright (C) 2003 - 2017 Qore Technologies, s.r.o.

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

#ifndef _QORE_ABSTRACTEXCEPTION_H

#define _QORE_ABSTRACTEXCEPTION_H

/** @file AbstractException.h
    Defines the abstract base class for c++ exceptions in the %Qore library
*/

//! abstract base class for c++ Exceptions in the %Qore library
class AbstractException {
public:
   //! Default virtual destructor.
   DLLEXPORT virtual ~AbstractException() = default;

   //! Raises the corresponding Qore exception in the ExceptionSink.
   /** @param xsink the exception sink
    */
   virtual void convert(ExceptionSink* xsink) = 0;

   DLLEXPORT AbstractException(AbstractException&&) = default;
   DLLEXPORT AbstractException& operator=(AbstractException&&) = default;

protected:
   //! Default constructor.
   DLLEXPORT AbstractException() = default;

private:
   AbstractException(const AbstractException&) = delete;
   AbstractException& operator=(const AbstractException&) = delete;
};

#endif
