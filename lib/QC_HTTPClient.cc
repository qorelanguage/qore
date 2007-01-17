/*
  QC_HTTPClient.cc

  Qore Programming Language

  Copyright (C) 2006 Qore Technologies

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
#include <qore/QoreClass.h>
#include <qore/Namespace.h>
#include <qore/QC_HTTPClient.h>
#include <qore/QoreHTTPClient.h>
#include <qore/minitest.hpp>

#ifdef DEBUG
#  include "tests/QC_HTTPClient_tests.cc"
#endif

int CID_HTTPCLIENT;

//-----------------------------------------------------------------------------
static void getHTTPClient(void *obj)
{
   ((QoreHTTPClient*)obj)->ROreference();
}

static void releaseHTTPClient(void *obj)
{
   ((QoreHTTPClient*)obj)->deref();
}

//-----------------------------------------------------------------------------
static void HTTPClient_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
  char* err_name = "HTTPClient::constructor";
  char* err_text = "Single parameter, hash with options, expected.";
  if (num_params(params) != 1) {
    xsink->raiseException(err_name, err_text);
    return;
  }
  QoreNode* n = test_param(params, NT_HASH, 0);
  if (!n) {
    xsink->raiseException(err_name, err_text);
    return;
  }
  Hash* opts = n->val.hash;

  QoreHTTPClient* client = new QoreHTTPClient(opts, xsink);
  if (xsink->isException()) {
    delete client;
    return;
  }
  self->setPrivate(CID_HTTPCLIENT, client, getHTTPClient, releaseHTTPClient);
}

//-----------------------------------------------------------------------------
static void HTTPClient_copy(Object *self, Object *old, QoreHTTPClient* client, ExceptionSink *xsink)
{
  xsink->raiseException("HTTPClient", "copying HTTPClient objects is not yet supported.");
}

//-----------------------------------------------------------------------------
QoreClass *initHTTPClientClass()
{
   tracein("initHTTPClientClass");

   QoreClass* client = new QoreClass(QDOM_NETWORK, strdup("HTTPClient2"));
   CID_HTTPCLIENT = client->getID();

   client->setConstructor(HTTPClient_constructor);
   client->setCopy((q_copy_t)HTTPClient_copy);

  traceout("initHTTPClientClass");
  return client;
}

//-----------------------------------------------------------------------------
Namespace* addHTTPClientNamespace()
{
  Namespace* ns = new Namespace("HTTPClient");

/*
  // constants
  List* l = new List;
  l->push(new QoreNode("OPTIONS"));
  l->push(new QoreNode("GET"));
  l->push(new QoreNode("HEAD"));
  l->push(new QoreNode("POST"));
  l->push(new QoreNode("PUT"));
  l->push(new QoreNode("DELETE"));
  l->push(new QoreNode("TRACE"));
  l->push(new QoreNode("CONNECT"));
  QoreNode* n = new QoreNode(l); 
  ns->addConstant("DEFAULT_METHODS", n);

  Hash* h = new Hash;
  ExceptionSink xsink;
  h->setKeyValue("Accept", new QoreNode("text/html"), &xsink);
  h->setKeyValue("Content-Type", new QoreNode("text/html"), &xsink);
  h->setKeyValue("User-Agent", new QoreNode("Qore HTTP Client 0.3.2"), &xsink);
  h->setKeyValue("Connection", new QoreNode("Keep-Alive"), &xsink);
  assert(!xsink);
  n = new QoreNode(h);
  ns->addConstant("DEFAULT_HEADERS", n);

  l = new List;
  l->push(new QoreNode("Host"));
  l->push(new QoreNode("User-Agent"));
  l->push(new QoreNode("Content-Length"));
  n = new QoreNode(l);
  ns->addConstant("DEFAULT_HEADER_IGNORE", n);

  h = QoreHTTPClient::get_DEFAULT_PROTOCOLS();
  n = new QoreNode(h);
  ns->addConstant("DEFAULT_PROTOCOLS", n);

  ns->addConstant("DEFAULT_HTTP_VERSION", new QoreNode(QoreHTTPClient::defaultHTTPVersion));

  l = QoreHTTPClient::get_ALLOWED_VERSIONS();
  n = new QoreNode(l);
  ns->addConstant("allowed_versions", n);
*/

   QoreHTTPClient::method_set.insert("OPTIONS");
   QoreHTTPClient::method_set.insert("GET");
   QoreHTTPClient::method_set.insert("HEAD");
   QoreHTTPClient::method_set.insert("POST");
   QoreHTTPClient::method_set.insert("PUT");
   QoreHTTPClient::method_set.insert("DELETE");
   QoreHTTPClient::method_set.insert("TRACE");
   QoreHTTPClient::method_set.insert("CONNECT");
   
   QoreHTTPClient::default_headers["Accept"] = "text/html";
   QoreHTTPClient::default_headers["Content-Type"] = "text/html";
   QoreHTTPClient::default_headers["User-Agent"] = "Qore HTTP Client v" + PACKAGE_VERSION;
   QoreHTTPClient::default_headers["Connection"] = "Keep-Alive";

   class QoreString *user_agent = new QoreString();
   user_agent->sprintf("Qore HTTP Client v%s", PACKAGE_VERSION);
   QoreHTTPClient::default_headers->setKeyValue("User-Agent", new QoreNode(user_agent), NULL);

   char buf[HOSTNAMEBUFSIZE + 1];
   if (gethostname(buf, HOSTNAMEBUFSIZE))
      QoreHTTPClient::default_headers->setKeyValue("Host", new QoreNode("localhost"), NULL);
   else
      QoreHTTPClient::default_headers->setKeyValue("Host", new QoreNode(buf), NULL);
   
   QoreHTTPClient::header_ignore.insert("Host");
   QoreHTTPClient::header_ignore.insert("User-Agent");
   QoreHTTPClient::header_ignore.insert("Content-Length");

  return ns;
}

// EOF


