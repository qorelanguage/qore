/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
    QoreXSinkException.h

    Qore Programming Language

    Copyright (C) 2003 - 2024 Qore Technologies, s.r.o.

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

#ifndef _QORE_QOREXSINKEXCEPTION_H

#define _QORE_QOREXSINKEXCEPTION_H

/** @file QoreXSinkException.h
    Defines a class for C++ exception based on an ExceptionSink object
*/

#include "QoreValue.h"

#include <cstdarg>
#include <string>

// forward references
class QoreStringNode;

//! class for C++ exception based on an ExceptionSink object
/**
*/
class QoreXSinkException : public AbstractException {
public:
    //! creates the exception object
    /** after this call, the xsink argument can no longer be used
    */
    DLLEXPORT QoreXSinkException(ExceptionSink& xsink);

    //! copy constructor
    DLLLOCAL QoreXSinkException(QoreXSinkException&& xsink) = default;

    //! Destroys the object
    DLLEXPORT virtual ~QoreXSinkException();

    //! Default assignment operator
    DLLLOCAL QoreXSinkException& operator=(QoreXSinkException&&) = default;

    //! Raises the corresponding Qore exception in the ExceptionSink.
    /** @param xsink the exception sink
    */
    virtual void convert(ExceptionSink* xsink);

private:
    struct qore_es_private* priv;
};

#endif
