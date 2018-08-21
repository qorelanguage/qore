/* -*- mode: c++; indent-tabs-mode: nil -*- */
/** @file QC_Class.h Class class definition */
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

#ifndef _QORE_INTERN_QC_CLASS_H

#define _QORE_INTERN_QC_CLASS_H

#include "qore/intern/AbstractReflectionObject.h"
#include "qore/intern/QC_AbstractMethod.h"

class QoreReflectionClass : public AbstractReflectionObject {
public:
    const QoreClass* cls;

    DLLLOCAL QoreReflectionClass(const char* name, ExceptionSink* xsink);

    DLLLOCAL QoreReflectionClass(QoreProgram* pgm, const QoreClass* cls);
};

DLLLOCAL QoreObject* get_method_object(ReferenceHolder<QoreReflectionMethod>& m, ExceptionSink* xsink);

DLLEXPORT extern qore_classid_t CID_CLASS;
DLLLOCAL extern QoreClass* QC_CLASS;

DLLLOCAL void preinitClassClass();
DLLLOCAL QoreClass* initClassClass(QoreNamespace& ns);

DLLLOCAL TypedHashDecl* init_hashdecl_ClassAccessInfo(QoreNamespace& ns);
DLLLOCAL TypedHashDecl* init_hashdecl_SourceLocationInfo(QoreNamespace& ns);

#endif