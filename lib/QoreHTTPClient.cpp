/*
  QoreHTTPClient.cpp

  Qore Programming Language

  Copyright (C) 2006 - 2013 Qore Technologies
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

/*
  RFC 2616 HTTP 1.1
  RFC 2617 HTTP authentication
  RFC 3986 HTTP URI specification
*/

#include <qore/Qore.h>
#include <qore/QoreURL.h>
#include <qore/QoreHTTPClient.h>
#include <qore/intern/ql_misc.h>
#include <qore/intern/QC_Queue.h>
#include <qore/intern/QoreHttpClientObjectIntern.h>

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
