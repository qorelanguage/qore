/*
  QC_HTTPClient.cc

  Qore Programming Language

  Copyright (C) 2006, 2007 Qore Technologies

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

#include <qore/Qore.h>
#include <qore/QoreHTTPClient.h>
#include <qore/intern/QC_HTTPClient.h>
#include <qore/intern/ssl_constants.h>
#include <qore/minitest.hpp>

#ifdef DEBUG_TESTS
#  include "tests/QC_HTTPClient_tests.cc"
#endif

qore_classid_t CID_HTTPCLIENT;

static void HC_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink) {
   ReferenceHolder<QoreHTTPClient> client(new QoreHTTPClient(), xsink);
   const QoreHashNode *n = test_hash_param(params, 0);
   if (n && client->setOptions(n, xsink))
      return;
   
   self->setPrivate(CID_HTTPCLIENT, client.release());
}

static void HC_copy(QoreObject *self, QoreObject *old, QoreHTTPClient* client, ExceptionSink *xsink) {
   xsink->raiseException("HTTPCLIENT-COPY-ERROR", "copying HTTPClient objects is not yet supported");
}

static void HC_destructor(QoreObject *self, QoreHTTPClient *client, ExceptionSink *xsink) {
   // have to clear callbacks before destroying
   client->cleanup(xsink);
   client->deref(xsink);
}

static AbstractQoreNode *HC_setHTTPVersion(QoreObject *self, QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p = test_string_param(params, 0);
   if (!p) {
      xsink->raiseException("HTTP-CLIENT-SETHTTPVERSION-ERROR", "expecting string ('1.0' or '1.1') passed as only argument");
      return 0;
   }
   client->setHTTPVersion(p->getBuffer(), xsink);
   return 0;
}

static AbstractQoreNode *HC_getHTTPVersion(QoreObject *self, QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreStringNode(client->getHTTPVersion());
}

static AbstractQoreNode *HC_setSecure(QoreObject *self, QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink) {
   const AbstractQoreNode *n = get_param(params, 0);
   client->setSecure(n ? n->getAsBool() : false);
   return 0;
}

static AbstractQoreNode *HC_isSecure(QoreObject *self, QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink) {
   return get_bool_node(client->isSecure());
}

static AbstractQoreNode *HC_verifyPeerCertificate(QoreObject *self, QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink) {
   const char *c = getSSLCVCode(client->verifyPeerCertificate());
   return c ? new QoreStringNode(c) : 0;
}

static AbstractQoreNode *HC_getSSLCipherName(QoreObject *self, QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink) {
   const char *str = client->getSSLCipherName();
   return str ? new QoreStringNode(str) : 0;
}

static AbstractQoreNode *HC_getSSLCipherVersion(QoreObject *self, QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink) {
   const char *str = client->getSSLCipherVersion();
   return str ? new QoreStringNode(str) : 0;
}

static AbstractQoreNode *HC_connect(QoreObject *self, QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink) {
   client->connect(xsink);
   return 0;
}

static AbstractQoreNode *HC_disconnect(QoreObject *self, QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink) {
   client->disconnect();
   return 0;
}

// send(data, method, path, headers, [getbody], [info_reference])
static AbstractQoreNode *HC_send(QoreObject *self, QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink) {
   const void *ptr = 0;
   int size = 0;
   const AbstractQoreNode *p = get_param(params, 0);
   if (!is_nothing(p)) {
      if (p->getType() == NT_STRING) {
	 const QoreStringNode *str = reinterpret_cast<const QoreStringNode *>(p);
	 assert(str);
	 ptr = str->getBuffer();
	 size = str->strlen();
      }
      else if (p->getType() == NT_BINARY) {
	 const BinaryNode *b = reinterpret_cast<const BinaryNode *>(p);
         ptr = b->getPtr();
         size = b->size();
      }
      // ignore other types - no exception raised
   }
   
   const QoreStringNode *pstr = test_string_param(params, 1);
   if (!pstr) {
      xsink->raiseException("HTTP-CLIENT-SEND-ERROR", "expecting method name as second parameter");
      return 0;
   }
   const char *meth = pstr->getBuffer();

   pstr = test_string_param(params, 2);
   const char *path = pstr ? pstr->getBuffer() : 0;

   const QoreHashNode *ph = test_hash_param(params, 3);

   p = get_param(params, 4);
   bool getbody = p ? p->getAsBool() : false;

   ReferenceHolder<QoreHashNode> info(xsink);
   const ReferenceNode *ref = test_reference_param(params, 5);
   if (ref)
      info = new QoreHashNode();

   ReferenceHolder<AbstractQoreNode> rv(client->send(meth, path, ph, ptr, size, getbody, *info, xsink), xsink);

   // write info to reference first
   if (ref) {
      AutoVLock vl(xsink);
      ReferenceHelper rh(ref, vl, xsink);
      if (!rh)
	 return 0;

      if (rh.assign(info.release(), xsink))
	 return 0;
   }

   // return 0 if an exception occured
   if (!rv || *xsink)
      return 0;

   return rv.release();
}

// get(path, headers, [info_reference])
static AbstractQoreNode *HC_get(QoreObject *self, QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *pstr = test_string_param(params, 0);
   if (!pstr) {
      xsink->raiseException("HTTP-CLIENT-GET-ERROR", "expecting path as first parameter");
      return 0;
   }
   const char *path = pstr->getBuffer();

   const QoreHashNode *ph = test_hash_param(params, 1);

   ReferenceHolder<QoreHashNode> info(xsink);
   const ReferenceNode *ref = test_reference_param(params, 2);
   if (ref)
      info = new QoreHashNode();

   ReferenceHolder<AbstractQoreNode> rv(client->get(path, ph, *info, xsink), xsink);

   // write info to reference first
   if (ref) {
      AutoVLock vl(xsink);
      ReferenceHelper rh(ref, vl, xsink);
      if (!rh)
	 return 0;

      if (rh.assign(info.release(), xsink))
	 return 0;
   }

   // return 0 if an exception occured
   if (!rv || *xsink)
      return 0;

   return rv.release();
}

// head(path, headers, [info_reference])
static AbstractQoreNode *HC_head(QoreObject *self, QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *pstr = test_string_param(params, 0);
   if (!pstr) {
      xsink->raiseException("HTTP-CLIENT-HEAD-ERROR", "expecting path as first parameter");
      return 0;
   }
   const char *path = pstr->getBuffer();

   const QoreHashNode *ph = test_hash_param(params, 1);

   ReferenceHolder<QoreHashNode> info(xsink);
   const ReferenceNode *ref = test_reference_param(params, 2);
   if (ref)
      info = new QoreHashNode();

   ReferenceHolder<AbstractQoreNode> rv(client->head(path, ph, *info, xsink), xsink);

   // write info to reference first
   if (ref) {
      AutoVLock vl(xsink);
      ReferenceHelper rh(ref, vl, xsink);
      if (!rh)
	 return 0;

      if (rh.assign(info.release(), xsink))
	 return 0;
   }

   // return 0 if an exception occured
   if (!rv || *xsink)
      return 0;

   return rv.release();
}

// post(path, data, headers, [info_reference])
static AbstractQoreNode *HC_post(QoreObject *self, QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *pstr = test_string_param(params, 0);
   if (!pstr) {
      xsink->raiseException("HTTP-CLIENT-POST-ERROR", "expecting path as first parameter");
      return 0;
   }
   const char *path = pstr->getBuffer();

   const void *ptr = 0;
   int size = 0;
   const AbstractQoreNode *p = get_param(params, 1);
   if (!is_nothing(p)) {
      if (p->getType() == NT_STRING) {
	 const QoreStringNode *str = reinterpret_cast<const QoreStringNode *>(p);
	 ptr = str->getBuffer();
	 size = str->strlen();
      }
      else if (p->getType() == NT_BINARY) {
	 const BinaryNode *b = reinterpret_cast<const BinaryNode *>(p);
         ptr = b->getPtr();
         size = b->size();
      }
      else {
	 xsink->raiseException("HTTP-CLIENT-POST-ERROR", "expecting string or binary as second argument for message data");
	 return 0;
      }
   }

   const QoreHashNode *ph = test_hash_param(params, 2);

   ReferenceHolder<QoreHashNode> info(xsink);
   const ReferenceNode *ref = test_reference_param(params, 3);
   if (ref)
      info = new QoreHashNode();

   ReferenceHolder<AbstractQoreNode> rv(client->post(path, ph, ptr, size, *info, xsink), xsink);

   // write info to reference first
   if (ref) {
      AutoVLock vl(xsink);
      ReferenceHelper rh(ref, vl, xsink);
      if (!rh)
	 return 0;

      if (rh.assign(info.release(), xsink))
	 return 0;
   }

   // return 0 if an exception occured
   if (!rv || *xsink)
      return 0;

   return rv.release();
}

static AbstractQoreNode *HC_setTimeout(QoreObject *self, QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink) {
   client->setTimeout(getMsZeroInt(get_param(params, 0)));
   return 0;
}

static AbstractQoreNode *HC_getTimeout(QoreObject *self, QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreBigIntNode(client->getTimeout());
}

static AbstractQoreNode *HC_setEncoding(QoreObject *self, QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0;

   if (!(p0 = test_string_param(params, 0))) {
      xsink->raiseException("HTTP-CLIENT-SET-ENCODING-ERROR", "expecting charset name (string) as parameter of HTTPClient::setEncoding() call");
      return 0;
   }

   client->setEncoding(QEM.findCreate(p0));
   return 0; 
}

static AbstractQoreNode *HC_getEncoding(QoreObject *self, QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreStringNode(client->getEncoding()->getCode());
}

static AbstractQoreNode *HC_setURL(QoreObject *self, QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *n = test_string_param(params, 0);
   if (!n || !n->strlen())
      xsink->raiseException("HTTP-CLIENT-EMPTY-URL", "HTTPClient::setURL() called without a valid string argument");
   else
      client->setURL(n->getBuffer(), xsink);
   return 0;
}

static AbstractQoreNode *HC_getURL(QoreObject *self, QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink) {
   return client->getURL();
}

static AbstractQoreNode *HC_setProxyURL(QoreObject *self, QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *n = test_string_param(params, 0);
   if (!n)
      client->clearProxyURL();
   else
      client->setProxyURL(n->getBuffer(), xsink);
   return 0;
}

static AbstractQoreNode *HC_getProxyURL(QoreObject *self, QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink) {
   return client->getProxyURL();
}

static AbstractQoreNode *HC_clearProxyURL(QoreObject *self, QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink) {
   client->clearProxyURL();
   return 0;
}

static AbstractQoreNode *HC_setProxySecure(QoreObject *self, QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink) {
   const AbstractQoreNode *n = get_param(params, 0);
   client->setProxySecure(n ? n->getAsBool() : false);
   return 0;
}

static AbstractQoreNode *HC_isProxySecure(QoreObject *self, QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink) {
   return get_bool_node(client->isProxySecure());
}

static AbstractQoreNode *HC_setMaxRedirects(QoreObject *self, QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink) {
   const AbstractQoreNode *n = get_param(params, 0);
   client->setMaxRedirects(n ? n->getAsInt() : 0);
   return 0;
}

static AbstractQoreNode *HC_getMaxRedirects(QoreObject *self, QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreBigIntNode(client->getMaxRedirects());
}

static AbstractQoreNode *HC_setEventQueue(QoreObject *self, QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink) {
    const QoreObject *o = test_object_param(params, 0);
    Queue *q = o ? (Queue *)o->getReferencedPrivateData(CID_QUEUE, xsink) : 0;
    if (*xsink)
        return 0;
    // pass reference from QoreObject::getReferencedPrivateData() to function
    client->setEventQueue(q, xsink);
    return 0;
}

static AbstractQoreNode *HC_setConnectTimeout(QoreObject *self, QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink) {
   client->setConnectTimeout(getMsMinusOneInt(get_param(params, 0)));
   return 0;
}

static AbstractQoreNode *HC_getConnectTimeout(QoreObject *self, QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreBigIntNode(client->getConnectTimeout());
}

static AbstractQoreNode *HC_setNoDelay(QoreObject *self, QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink) {
    return new QoreBigIntNode(client->setNoDelay(get_int_param(params, 0)));
}

static AbstractQoreNode *HC_getNoDelay(QoreObject *self, QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink) {
    return get_bool_node(client->getNoDelay());
}

static AbstractQoreNode *HC_isConnected(QoreObject *self, QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink) {
    return get_bool_node(client->isConnected());
}

QoreClass *initHTTPClientClass() {
   QORE_TRACE("initHTTPClientClass");

   // initialize static data structures in the QoreHTTPClient class
   QoreHTTPClient::static_init();

   QoreClass* client = new QoreClass("HTTPClient", QDOM_NETWORK);
   // no need to set the class synchronous flag because the QoreHTTPClient class is already thread-safe
   CID_HTTPCLIENT = client->getID();

   client->setConstructor(HC_constructor);
   client->setCopy((q_copy_t)HC_copy);
   client->setDestructor((q_destructor_t)HC_destructor);

   client->addMethod("setHTTPVersion",         (q_method_t)HC_setHTTPVersion);
   client->addMethod("getHTTPVersion",         (q_method_t)HC_getHTTPVersion);
   client->addMethod("setSecure",              (q_method_t)HC_setSecure);
   client->addMethod("isSecure",               (q_method_t)HC_isSecure);
   client->addMethod("verifyPeerCertificate",  (q_method_t)HC_verifyPeerCertificate);
   client->addMethod("getSSLCipherName",       (q_method_t)HC_getSSLCipherName);
   client->addMethod("getSSLCipherVersion",    (q_method_t)HC_getSSLCipherVersion);
   client->addMethod("connect",                (q_method_t)HC_connect);
   client->addMethod("disconnect",             (q_method_t)HC_disconnect);
   client->addMethod("send",                   (q_method_t)HC_send);
   client->addMethod("get",                    (q_method_t)HC_get);
   client->addMethod("head",                   (q_method_t)HC_head);
   client->addMethod("post",                   (q_method_t)HC_post);
   client->addMethod("setTimeout",             (q_method_t)HC_setTimeout);
   client->addMethod("getTimeout",             (q_method_t)HC_getTimeout);
   client->addMethod("setEncoding",            (q_method_t)HC_setEncoding);
   client->addMethod("getEncoding",            (q_method_t)HC_getEncoding);
   client->addMethod("setURL",                 (q_method_t)HC_setURL);
   client->addMethod("getURL",                 (q_method_t)HC_getURL);
   client->addMethod("setProxyURL",            (q_method_t)HC_setProxyURL);
   client->addMethod("getProxyURL",            (q_method_t)HC_getProxyURL);
   client->addMethod("clearProxyURL",          (q_method_t)HC_clearProxyURL);
   client->addMethod("setProxySecure",         (q_method_t)HC_setProxySecure);
   client->addMethod("isProxySecure",          (q_method_t)HC_isProxySecure);
   client->addMethod("setMaxRedirects",        (q_method_t)HC_setMaxRedirects);
   client->addMethod("getMaxRedirects",        (q_method_t)HC_getMaxRedirects);
   client->addMethod("setEventQueue",          (q_method_t)HC_setEventQueue);
   client->addMethod("setConnectTimeout",      (q_method_t)HC_setConnectTimeout);
   client->addMethod("getConnectTimeout",      (q_method_t)HC_getConnectTimeout);
   client->addMethod("setNoDelay",             (q_method_t)HC_setNoDelay);
   client->addMethod("getNoDelay",             (q_method_t)HC_getNoDelay);
   client->addMethod("isConnected",            (q_method_t)HC_isConnected);

   return client;
}
