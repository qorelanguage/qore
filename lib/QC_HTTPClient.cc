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
  QoreHTTPClient* client = new QoreHTTPClient;
  self->setPrivate(CID_HTTPCLIENT, client, getHTTPClient, releaseHTTPClient);
}

//-----------------------------------------------------------------------------
static void HTTPClient_destructor(class Object *self, QoreHTTPClient* client, ExceptionSink *xsink)
{
   client->deref();
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
   client->setDestructor((q_destructor_t)HTTPClient_destructor);
   client->setCopy((q_copy_t)HTTPClient_copy);

  traceout("initHTTPClientClass");
  return client;
}

//-----------------------------------------------------------------------------
Namespace* addHTTPClientNamespace()
{
  Namespace* ns = new Namespace("HTTPClient");

  ns->addConstant("test", new QoreNode(1.0));

  // constants
  NamedScope name1(strdup("DEFAULT_METHODS"));
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
  ns->addConstant(&name1, n);

  NamedScope name2(strdup("DEFAULT_HEADERS"));
  Hash* h = new Hash;
  ExceptionSink xsink;
  h->setKeyValue("Accept", new QoreNode("text/html"), &xsink);
  h->setKeyValue("Content-Type", new QoreNode("text/html"), &xsink);
  h->setKeyValue("User-Agent", new QoreNode("Qore HTTP Client 0.3.2"), &xsink);
  h->setKeyValue("Connection", new QoreNode("Keep-Alive"), &xsink);
  assert(!xsink);
  n = new QoreNode(h);
  ns->addConstant(&name2, n);

  return ns;
}

// EOF


