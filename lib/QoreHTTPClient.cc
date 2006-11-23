/*
  QoreHTTPClient.cc

  Qore Programming Language

  Copyright (C) 2006 QoreTechnologies
  
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

#include <qore/config.h>
#include <qore/common.h>
#include <qore/QoreHTTPClient.h>
#include <qore/minitest.hpp>

#ifdef DEBUG
#  include "tests/QoreHTTPClient_tests.cc"
#endif

const char* QoreHTTPClient::defaultHTTPVersion = "1.1";

//-----------------------------------------------------------------------------
Hash* QoreHTTPClient::get_DEFAULT_PROTOCOLS()
{
  ExceptionSink xsink;
  Hash* h = new Hash;
  Hash* h2 = new Hash;
  h2->setKeyValue("port", new QoreNode((int64)80), &xsink);
  h2->setKeyValue("ssl", new QoreNode(false), &xsink);
  assert(!xsink);
  h->setKeyValue("http", new QoreNode(h2), &xsink);
  assert(!xsink);
  h2 = new Hash;
  h2->setKeyValue("port", new QoreNode((int64)443), &xsink);
  h2->setKeyValue("ssl", new QoreNode(true), &xsink);
  assert(!xsink);
  h->setKeyValue("https", new QoreNode(h2), &xsink);
  assert(!xsink);

  return h;
}

//-----------------------------------------------------------------------------
List* QoreHTTPClient::get_ALLOWED_VERSIONS()
{
  List* l = new List;
  l->push(new QoreNode("1.0"));
  l->push(new QoreNode("1.1"));
  return l;
}

//-----------------------------------------------------------------------------
QoreHTTPClient::QoreHTTPClient(Hash* opts, ExceptionSink* xsink)
: protocols(0),
  ssl(false),
  port(0),
  timeout(0),
  connected(false)
{
  protocol = get_DEFAULT_PROTOCOLS();
  QoreNode* n = opts->getKeyValueExistence("protocols");
  if (n && n != (QoreNode*)-1 && n->type == NT_HASH) {
    protocols.merge(n->val.hash, xsink);
    if (xsink->isException()) return;
  }  

}

//-----------------------------------------------------------------------------
QoreHTTPClient::~QoreHTTPClient()
{
  ExceptionSink xsink;
  if (protocols) {
    protocols->derefAndDelete(&xsink);
    assert(!xsink);
  }
}

//-----------------------------------------------------------------------------
void QoreHTTPClient::process_url(Hash* opts, ExceptionSink* xsink)
{
  QoreNode* n = opts->getKeyValueExistence("url");
  if (!n || n == (QoreNode*)-1) return;
  if (n->type != NT_STRING) return;

  QoreURL url(n->val.String);
  if (!url.isValid()) {
    xsink->raiseException("HTTPClient::constructor", "url parameter cannot be parsed.");
    return;
  }

  port = url.port;
  if (url.host && url.host->size()) {
    host = url.host->getBuffer();
  }
  if (url.path && url.path->size()) {
    path = url.path->getBuffer();
  }
  if (url.username && url.username->size()) {
    username = url.username->getBuffer();
  }
  if (url.password && url.password->size()) {
    password = url.password->getBuffer();
  }

  if (url.protocol) {
 
    bool protocol_listed = false;
    HashIterator iter(protocols);

    while (iter.next()) {
      char* key = iter->getKey();
      if (!key) continue;

      if (strcmp(key, url.protocol->getBuffer()) == 0) {
        protocol_listed = true;
        break;
      }
    }
    if (!protocol_listed) {
      xsink->raiseException("HTTPClient::constructor", "Protocol %s is not supported.", url.protocol->getBuffer());
      return;
    }
    // find out SSL + port settings
    n = iter->getValue();
    if (n && n->type == NT_HASH) {
      QoreNode* n2 = n->getKeyValueExistence("ssl");
      if (n2 && n2 != (QoreNode*)-1 && n2->type == NT_BOOLEAN) {
        ssl = n2->val.boolval;
      }
      n2 = n->getKeyValueExistence("port");
      if (n2 && n2 != (QoreNode*)-1 && n2->type == NT_INT) {
        port = (int)n2->val.intval;
      }
    }
  }

  // is it a local port?
  if (port == 0 && !host.empty()) {
    char* aux;
    int val = strtol(host.c_str(), &aux, 10);
    if (aux != host.c_str()) {
      host = "localhost";
      port = val;
    }
  }
  if (host.empty()) {
    host = "localhost";
  }
  if (port == 0) {
    n = opts->getKeyValueExistence("default_port");
    if (n && n != (QoreNode*)-1 && n->type == NT_INT) {
      port = (int)n->val.intval;
    } else {
      port = 80;
    }
  }
  if (!path.empty()) {
    default_path = "/" + path;
  } else {
    n = opts->getKeyValueExists("default_path");
    if (n && n != (QoreNode*)-1 && n->type == NT_STRING && n->val.String->size()) {
      default_path = "/";
      default_path += n->val.String->getBuffer();
    }
  }

  n = opts->getKeyValueExistence("timeout");
  if (n && n != (QoreNode*)-1 && n->type == NT_INT) {
    timeout = (int)n->val.intval;
  } else {
    timeout = defaultTimeout;
  }

  n = opts->getKeyValueExietence("http_version");
  if (n && n != (QoreNode*)-1 && n->type == NT_STRING && n->val.String->size()) {
    setHTTPVersion(n->val.String->getBuffer(), xsink);
    if (xsink->isException()) return;
  } else {
    http_version = defaultHTTPVersion;
  }

  socketpath = host + ":";
  char buff[20];
  sprintf(buff, "%d", port);
  socketpath += buff;
}

//-----------------------------------------------------------------------------
void QoreHTTPClient::setHTTPVersion(char* version, ExceptionSink* xsink)
{
  bool ok = false;

  if (!version) version = "";
  if (version[0]) {
    List* l = get_ALLOWED_VERSIONS();
    for (int i = 0, n = l->size(); i != n; ++i) {
      QoreNode* n = l->retrieve_entry(i);
      if (n && n->type == NT_STRING && n->val.String->size()) {
        if (!strcmp(version, n->val.String->getBuffer()) {
          ok = true;
          break;
        }
      }
    }
    l->derefAndDelete(xsink);
    if (xsink->isException()) return;
  }
  if (!ok) {
    xsink->raiseException("HTTPClient:;setHTTPVersion", "Value %s is not valid version.", version) 
    return;
  }
  http_version = version;
}

// EOF

