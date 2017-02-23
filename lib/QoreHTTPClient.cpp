/*
  QoreHTTPClient.cpp

  Qore Programming Language

  Copyright (C) 2006 - 2015 Qore Technologies
  
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

/*
  RFC 2616 HTTP 1.1
  RFC 2617 HTTP authentication
  RFC 3986 HTTP URI specification
*/

#include <qore/Qore.h>
#include <qore/QoreURL.h>
#include <qore/QoreHTTPClient.h>
#include "qore/intern/ql_misc.h"
#include "qore/intern/QC_Queue.h"
#include "qore/intern/QoreHttpClientObjectIntern.h"

#include <qore/minitest.hpp>

#include <string>
#include <map>
#include <set>

#include <ctype.h>

#ifdef DEBUG_TESTS
#  include "tests/QoreHTTPClient_tests.cpp"
#endif

QoreHTTPClient::QoreHTTPClient() {
}

QoreHTTPClient::~QoreHTTPClient() {
}

void QoreHTTPClient::setDefaultPath(const char* pth) {
   QoreHttpClientObject::setDefaultPath(pth);
}

void QoreHTTPClient::setDefaultHeaderValue(const char* header, const char* val) {
   QoreHttpClientObject::setDefaultHeaderValue(header, val);
}

void QoreHTTPClient::addProtocol(const char* prot, int port, bool ssl) {
   QoreHttpClientObject::addProtocol(prot, port, ssl);
}

int QoreHTTPClient::setOptions(const QoreHashNode* opts, ExceptionSink* xsink) {
   return QoreHttpClientObject::setOptions(opts, xsink);
}

const QoreEncoding* QoreHTTPClient::getEncoding() const {
   return QoreHttpClientObject::getEncoding();
}

int QoreHTTPClient::connect(ExceptionSink* xsink) {
   return QoreHttpClientObject::connect(xsink);
}

QoreHashNode* QoreHTTPClient::send(const char* meth, const char* path, const QoreHashNode* headers, const void* data, unsigned size, bool getbody, QoreHashNode* info, ExceptionSink* xsink) {
   return QoreHttpClientObject::send(meth, path, headers, data, size, getbody, info, xsink);
}

void QoreHTTPClient::setEventQueue(Queue* cbq, ExceptionSink* xsink) {
   QoreHttpClientObject::setEventQueue(cbq, xsink);
}
