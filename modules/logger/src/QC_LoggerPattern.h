/* -*- mode: c++; indent-tabs-mode: nil -*- */
/** @file QC_LoggerPattern.h LoggerPattern class definition */
/*
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

#ifndef _QORE_INTERN_QC_LOGGERPATTERN_H

#define _QORE_INTERN_QC_LOGGERPATTERN_H

DLLLOCAL int check_constructor(const QoreClass* cls, ExceptionSink* xsink);

DLLEXPORT extern qore_classid_t CID_LOGGERPATTERN;
DLLLOCAL extern QoreClass* QC_LOGGERPATTERN;

DLLLOCAL void preinitLoggerPatternClass();
DLLLOCAL QoreClass* initLoggerPatternClass(QoreNamespace& ns);

//! default layout pattern
#define DEFAULT_PATTERN "%r [%t] %p %c - %m%n"
//! default date format
#define DEFAULT_DATE_FORMAT "YYYY-MM-DD HH:mm:SS.u"

class QoreLoggerPattern : public AbstractPrivateData {
public:
    //! This hostname
    static std::string hostname;

    using AbstractPrivateData::deref;
    DLLLOCAL virtual void deref(ExceptionSink *xsink) {
        if (ROdereference()) {
            delete this;
        }
    }
};

#endif
