/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  astparser-module.cpp

  Qore astparser module

  Copyright (C) 2017 Qore Technologies s.r.o.

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
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "QC_AstParser.h"

QoreStringNode *astparser_module_init();
void astparser_module_ns_init(QoreNamespace *rns, QoreNamespace *qns);
void astparser_module_delete();

// qore module symbols
DLLEXPORT char qore_module_name[] = "astparser";
DLLEXPORT char qore_module_version[] = PACKAGE_VERSION;
DLLEXPORT char qore_module_description[] = "Qore AST parser module";
DLLEXPORT char qore_module_author[] = "Ondrej Musil <ondrej.musil@qoretechnologies.com>";
DLLEXPORT char qore_module_url[] = "http://qore.org";
DLLEXPORT int qore_module_api_major = QORE_MODULE_API_MAJOR;
DLLEXPORT int qore_module_api_minor = QORE_MODULE_API_MINOR;
DLLEXPORT qore_module_init_t qore_module_init = astparser_module_init;
DLLEXPORT qore_module_ns_init_t qore_module_ns_init = astparser_module_ns_init;
DLLEXPORT qore_module_delete_t qore_module_delete = astparser_module_delete;
DLLEXPORT qore_license_t qore_module_license = QL_MIT;
DLLEXPORT char qore_module_license_str[] = "MIT";


QoreNamespace AstParserNS("astparser");

QoreStringNode* astparser_module_init() {
    AstParserNS.addSystemClass(initAstParserClass(AstParserNS));

    return nullptr;
}

void astparser_module_ns_init(QoreNamespace* rns, QoreNamespace* qns) {
    qns->addNamespace(AstParserNS.copy());
}

void astparser_module_delete() {
    // nothing to do here in this case
}

