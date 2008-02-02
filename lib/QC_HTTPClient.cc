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

int CID_HTTPCLIENT;

//-----------------------------------------------------------------------------
static void HC_constructor(class QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   SimpleRefHolder<QoreHTTPClient> client(new QoreHTTPClient());
   QoreHashNode *n = test_hash_param(params, 0);
   if (n && client->setOptions(n, xsink))
      return;
   
   self->setPrivate(CID_HTTPCLIENT, client.release());
}

//-----------------------------------------------------------------------------
static void HC_copy(QoreObject *self, QoreObject *old, QoreHTTPClient* client, ExceptionSink *xsink)
{
   xsink->raiseException("HTTPCLIENT-COPY-ERROR", "copying HTTPClient objects is not yet supported.");
}

static class AbstractQoreNode *HC_setHTTPVersion(class QoreObject *self, class QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink)
{
   class QoreStringNode *p = test_string_param(params, 0);
   if (!p)
   {
      xsink->raiseException("HTTP-CLIENT-SETHTTPVERSION-ERROR", "expecting string ('1.0' or '1.1') passed as only argument");
      return NULL;
   }
   client->setHTTPVersion(p->getBuffer(), xsink);
   return NULL;
}

static class AbstractQoreNode *HC_getHTTPVersion(class QoreObject *self, class QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(client->getHTTPVersion());
}

static class AbstractQoreNode *HC_setSecure(class QoreObject *self, class QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink)
{
   class AbstractQoreNode *n = get_param(params, 0);
   client->setSecure(n ? n->getAsBool() : false);
   return NULL;
}

static class AbstractQoreNode *HC_isSecure(class QoreObject *self, class QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(client->isSecure());
}

static class AbstractQoreNode *HC_verifyPeerCertificate(class QoreObject *self, class QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink)
{
   const char *c = getSSLCVCode(client->verifyPeerCertificate());
   return c ? new QoreStringNode(c) : NULL;
}

static class AbstractQoreNode *HC_getSSLCipherName(class QoreObject *self, class QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink)
{
   const char *str = client->getSSLCipherName();
   return str ? new QoreStringNode(str) : NULL;
}

static class AbstractQoreNode *HC_getSSLCipherVersion(class QoreObject *self, class QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink)
{
   const char *str = client->getSSLCipherVersion();
   return str ? new QoreStringNode(str) : NULL;
}

static class AbstractQoreNode *HC_connect(class QoreObject *self, class QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink)
{
   client->connect(xsink);
   return NULL;
}

static class AbstractQoreNode *HC_disconnect(class QoreObject *self, class QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink)
{
   client->disconnect();
   return NULL;
}

// send(data, method, path, headers, [getbody])
static class AbstractQoreNode *HC_send(class QoreObject *self, class QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink)
{
   const void *ptr = NULL;
   int size = 0;
   class AbstractQoreNode *p = get_param(params, 0);
   if (!is_nothing(p))
   {
      if (p->type == NT_STRING)
      {
	 const QoreStringNode *str = reinterpret_cast<const QoreStringNode *>(p);
	 assert(str);
	 ptr = str->getBuffer();
	 size = str->strlen();
      }
      else if (p->type == NT_BINARY)
      {
	 const BinaryNode *b = reinterpret_cast<const BinaryNode *>(p);
         ptr = b->getPtr();
         size = b->size();
      }
      // ignore other types - no exception raised
   }
   
   QoreStringNode *pstr = test_string_param(params, 1);
   if (!pstr)
   {
      xsink->raiseException("HTTP-CLIENT-SEND-ERROR", "expecting method name as second parameter");
      return NULL;
   }
   const char *meth = pstr->getBuffer();

   pstr = test_string_param(params, 2);
   const char *path = pstr ? pstr->getBuffer() : NULL;

   QoreHashNode *ph = test_hash_param(params, 3);

   p = get_param(params, 4);
   bool getbody = p ? p->getAsBool() : false;

   return client->send(meth, path, ph, ptr, size, getbody, xsink);
}

// get(path, headers)
static class AbstractQoreNode *HC_get(class QoreObject *self, class QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink)
{
   class QoreStringNode *pstr = test_string_param(params, 0);
   if (!pstr)
   {
      xsink->raiseException("HTTP-CLIENT-GET-ERROR", "expecting path as first parameter");
      return NULL;
   }
   const char *path = pstr->getBuffer();

   QoreHashNode *ph = test_hash_param(params, 1);
   return client->get(path, ph, xsink);
}

// head(path, headers)
static class AbstractQoreNode *HC_head(class QoreObject *self, class QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink)
{
   class QoreStringNode *pstr = test_string_param(params, 0);
   if (!pstr)
   {
      xsink->raiseException("HTTP-CLIENT-HEAD-ERROR", "expecting path as first parameter");
      return NULL;
   }
   const char *path = pstr->getBuffer();

   QoreHashNode *ph = test_hash_param(params, 1);
   return client->head(path, ph, xsink);
}

// post(path, data, headers)
static class AbstractQoreNode *HC_post(class QoreObject *self, class QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink)
{
   class QoreStringNode *pstr = test_string_param(params, 0);
   if (!pstr)
   {
      xsink->raiseException("HTTP-CLIENT-POST-ERROR", "expecting path as first parameter");
      return NULL;
   }
   const char *path = pstr->getBuffer();

   const void *ptr = NULL;
   int size = 0;
   AbstractQoreNode *p = get_param(params, 1);
   if (!is_nothing(p))
   {
      if (p->type == NT_STRING)
      {
	 const QoreStringNode *str = reinterpret_cast<const QoreStringNode *>(p);
	 ptr = str->getBuffer();
	 size = str->strlen();
      }
      else if (p->type == NT_BINARY)
      {
	 const BinaryNode *b = reinterpret_cast<const BinaryNode *>(p);
         ptr = b->getPtr();
         size = b->size();
      }
      else
      {
	 xsink->raiseException("HTTP-CLIENT-POST-ERROR", "expecting string or binary as second argument for message data");
	 return NULL;
      }
   }

   QoreHashNode *ph = test_hash_param(params, 2);
   return client->post(path, ph, ptr, size, xsink);
}

static class AbstractQoreNode *HC_setTimeout(class QoreObject *self, class QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink)
{
   client->setTimeout(getMsZeroInt(get_param(params, 0)));
   return NULL;
}

static class AbstractQoreNode *HC_getTimeout(class QoreObject *self, class QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(client->getTimeout());
}

static class AbstractQoreNode *HC_setEncoding(class QoreObject *self, class QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNode *p0;

   if (!(p0 = test_string_param(params, 0)))
   {
      xsink->raiseException("HTTP-CLIENT-SET-ENCODING-ERROR", "expecting charset name (string) as parameter of HTTPClient::setEncoding() call");
      return NULL;
   }

   client->setEncoding(QEM.findCreate(p0));
   return NULL; 
}

static class AbstractQoreNode *HC_getEncoding(class QoreObject *self, class QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(client->getEncoding()->getCode());
}

class AbstractQoreNode *f_setURL(class QoreObject *self, class QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink)
{
   class QoreStringNode *n = test_string_param(params, 0);
   if (!n || !n->strlen())
      xsink->raiseException("HTTP-CLIENT-EMPTY-URL", "HTTPClient::setURL() called without a valid string argument");
   else
      client->setURL(n->getBuffer(), xsink);
   return NULL;
}

class AbstractQoreNode *f_getURL(class QoreObject *self, class QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink)
{
   return client->getURL();
}

class AbstractQoreNode *f_setProxyURL(class QoreObject *self, class QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink)
{
   class QoreStringNode *n = test_string_param(params, 0);
   if (!n)
      client->clearProxyURL();
   else
      client->setProxyURL(n->getBuffer(), xsink);
   return NULL;
}

class AbstractQoreNode *f_getProxyURL(class QoreObject *self, class QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink)
{
   return client->getProxyURL();
}

class AbstractQoreNode *f_clearProxyURL(class QoreObject *self, class QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink)
{
   client->clearProxyURL();
   return NULL;
}

class AbstractQoreNode *f_setProxySecure(class QoreObject *self, class QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink)
{
   class AbstractQoreNode *n = get_param(params, 0);
   client->setProxySecure(n ? n->getAsBool() : false);
   return NULL;
}

class AbstractQoreNode *f_isProxySecure(class QoreObject *self, class QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(client->isProxySecure());
}

class AbstractQoreNode *f_setMaxRedirects(class QoreObject *self, class QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink)
{
   class AbstractQoreNode *n = get_param(params, 0);
   client->setMaxRedirects(n ? n->getAsInt() : 0);
   return NULL;
}

class AbstractQoreNode *f_getMaxRedirects(class QoreObject *self, class QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(client->getMaxRedirects());
}

//-----------------------------------------------------------------------------
QoreClass *initHTTPClientClass()
{
   tracein("initHTTPClientClass");

   // initialize static data structures in the QoreHTTPClient class
   QoreHTTPClient::static_init();

   QoreClass* client = new QoreClass("HTTPClient", QDOM_NETWORK);
   CID_HTTPCLIENT = client->getID();

   client->setConstructor(HC_constructor);
   client->setCopy((q_copy_t)HC_copy);
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
   client->addMethod("setURL",                 (q_method_t)f_setURL);
   client->addMethod("getURL",                 (q_method_t)f_getURL);
   client->addMethod("setProxyURL",            (q_method_t)f_setProxyURL);
   client->addMethod("getProxyURL",            (q_method_t)f_getProxyURL);
   client->addMethod("clearProxyURL",          (q_method_t)f_clearProxyURL);
   client->addMethod("setProxySecure",         (q_method_t)f_setProxySecure);
   client->addMethod("isProxySecure",          (q_method_t)f_isProxySecure);
   client->addMethod("setMaxRedirects",        (q_method_t)f_setMaxRedirects);
   client->addMethod("getMaxRedirects",        (q_method_t)f_getMaxRedirects);

   traceout("initHTTPClientClass");
   return client;
}

// EOF


