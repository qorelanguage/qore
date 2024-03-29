/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
    QoreIteratorBase.h

    abstract class for private data for iterators in objects

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

#ifndef _QORE_QOREITERATORBASE_H

#define _QORE_QOREITERATORBASE_H

#include <qore/AbstractPrivateData.h>

DLLEXPORT extern QoreClass* QC_ABSTRACTITERATOR;
DLLEXPORT extern QoreClass* QC_ABSTRACTBIDIRECTIONALITERATOR;
DLLEXPORT extern QoreClass* QC_ABSTRACTQUANTIFIEDBIDIRECTIONALITERATOR;
DLLEXPORT extern QoreClass* QC_ABSTRACTQUANTIFIEDITERATOR;
DLLEXPORT extern QoreClass* QC_ABSTRACTLINEITERATOR;

class QoreAbstractIteratorBase {
protected:
    int tid;

public:
    //! creates the object and marks it as owned by the current thread
    DLLEXPORT QoreAbstractIteratorBase();

    //! destroys the object
    DLLEXPORT virtual ~QoreAbstractIteratorBase();

    //! checks for a valid operation, returns 0 if OK, -1 if not (exception thrown)
    DLLEXPORT int check(ExceptionSink* xsink) const;

    //! returns the name of the current iterator class
    DLLEXPORT virtual const char* getName() const = 0;

    //! returns the element type for the iterator
    DLLLOCAL virtual const QoreTypeInfo* getElementType() const = 0;
};

//! abstract base class for iterator private data
class QoreIteratorBase : public AbstractPrivateData, public QoreAbstractIteratorBase {
protected:
    //! destroys the object
    DLLEXPORT virtual ~QoreIteratorBase();

public:
    //! creates the object and marks it as owned by the current thread
    DLLEXPORT QoreIteratorBase();
};

#endif
