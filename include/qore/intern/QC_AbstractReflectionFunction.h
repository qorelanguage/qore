/* -*- mode: c++; indent-tabs-mode: nil -*- */
/** @file QC_AbstractReflectionFunction.h AbstractReflectionFunction class definition */
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

#ifndef _QORE_INTERN_QC_ABSTRACTREFLECTIONFUNCTION_H

#define _QORE_INTERN_QC_ABSTRACTREFLECTIONFUNCTION_H

#include "qore/intern/AbstractReflectionObject.h"

//! method type enum
enum method_type_e {
    MT_None = 0, // not a method function/variant
    MT_Normal = 1,
    MT_Static = 2,
    MT_Constructor = 3,
    MT_Destructor = 4,
    MT_Copy = 5,
};

class QoreReflectionFunction : public AbstractReflectionObject {
public:
    const QoreFunction* f = nullptr;

    DLLLOCAL QoreReflectionFunction(QoreProgram* pgm) : AbstractReflectionObject(pgm) {
    }

    DLLLOCAL QoreReflectionFunction(QoreProgram* pgm, const QoreFunction* f) : AbstractReflectionObject(pgm), f(f) {
    }

    DLLLOCAL QoreObject* getFunctionObject(ExceptionSink* xsink) const;
};

DLLLOCAL QoreObject* find_function_variant(QoreProgram* pgm, const QoreFunction* func, const QoreListNode* args, size_t offset, bool exact, method_type_e mtype, ExceptionSink* xsink);

DLLLOCAL QoreObject* get_variant_object(QoreProgram* pgm, const QoreFunction* f, method_type_e mtype, const AbstractQoreFunctionVariant* v, ExceptionSink* xsink);

DLLLOCAL void append_variant_objects(QoreListNode& l, QoreProgram* pgm, const QoreFunction* f, method_type_e mtype, ExceptionSink* xsink);

DLLEXPORT extern qore_classid_t CID_ABSTRACTREFLECTIONFUNCTION;
DLLLOCAL extern QoreClass* QC_ABSTRACTREFLECTIONFUNCTION;

DLLLOCAL void preinitAbstractReflectionFunctionClass();
DLLLOCAL QoreClass* initAbstractReflectionFunctionClass(QoreNamespace& ns);

#endif