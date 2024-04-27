/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
    logger-module.cpp

    Qore logger module

    Copyright (C) 2017 - 2024 Qore Technologies s.r.o.

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
*/

#include "qore/Qore.h"

#include "qore_logger.h"

#include "QC_LoggerPattern.h"

#include <string.h>

QoreStringNode* logger_module_init();
void logger_module_ns_init(QoreNamespace *rns, QoreNamespace *qns);
void logger_module_delete();

// qore module symbols
DLLEXPORT char qore_module_name[] = "logger";
DLLEXPORT char qore_module_version[] = PACKAGE_VERSION;
DLLEXPORT char qore_module_description[] = "Qore logger module";
DLLEXPORT char qore_module_author[] = "David Nichols <david.nichols@qoretechnologies.com>";
DLLEXPORT char qore_module_url[] = "http://qore.org";
DLLEXPORT int qore_module_api_major = QORE_MODULE_API_MAJOR;
DLLEXPORT int qore_module_api_minor = QORE_MODULE_API_MINOR;
DLLEXPORT qore_module_init_t qore_module_init = logger_module_init;
DLLEXPORT qore_module_ns_init_t qore_module_ns_init = logger_module_ns_init;
DLLEXPORT qore_module_delete_t qore_module_delete = logger_module_delete;
DLLEXPORT qore_license_t qore_module_license = QL_MIT;
DLLEXPORT char qore_module_license_str[] = "MIT";

QoreNamespace LoggerNS("Logger");

QoreStringNode* logger_module_init() {
    char buf[HOSTNAMEBUFSIZE + 1];
    if (gethostname(buf, HOSTNAMEBUFSIZE)) {
        return new QoreStringNodeMaker("gethostname() failed: %s", strerror(errno));
    }
    QoreLoggerPattern::hostname = buf;

    // set up Logger namespace
    LoggerNS.addSystemClass(initLoggerPatternClass(LoggerNS));

    return nullptr;
}

void logger_module_ns_init(QoreNamespace* rns, QoreNamespace* qns) {
    qns->addNamespace(LoggerNS.copy());
}

void logger_module_delete() {
}

