/* -*- mode: c++; indent-tabs-mode: nil -*- */
/** @file QC_AbstractConstant.h AbstractConstant class definition */
/*
    Qore Programming Language

    Copyright (C) 2003 - 2018 Qore Technologies, s.r.o.

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

#ifndef _QORE_INTERN_QC_ABSTRACTCONSTANT_H

#define _QORE_INTERN_QC_ABSTRACTCONSTANT_H

#include "qore/intern/AbstractReflectionObject.h"

// forward references
class ConstantEntry;

class QoreReflectionConstant : public AbstractReflectionObject {
public:
    const ConstantEntry* ce;

    DLLLOCAL QoreReflectionConstant(QoreProgram* pgm, const ConstantEntry* ce) :
        AbstractReflectionObject(pgm), ce(ce) {
    }

    DLLLOCAL virtual const QoreClass* getClass() const = 0;
};

DLLEXPORT extern qore_classid_t CID_ABSTRACTCONSTANT;
DLLLOCAL extern QoreClass* QC_ABSTRACTCONSTANT;

DLLLOCAL void preinitAbstractConstantClass();
DLLLOCAL QoreClass* initAbstractConstantClass(QoreNamespace& ns);

#endif