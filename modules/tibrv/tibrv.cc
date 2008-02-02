/*
  TIBCO/tibrv.cc

  TIBCO Active Enterprise integration to QORE

  Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols

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
#include <qore/QoreSSLCertificate.h>
#include <qore/QoreSSLPrivateKey.h>

#include <tibrv/tibrvcpp.h>

#ifdef TIBRV_SD
#include <tibrv/sdcpp.h>

// params: name, certificate
static class AbstractQoreNode *f_tibrvSetDaemonCert(const QoreListNode *params, class ExceptionSink *xsink)
{
   // get daemon name (format: "ssl:<host>:<port_number>")
   QoreStringNode *str = test_string_param(params, 0);
   const char *name = str ? str->getBuffer() : TIBRV_SECURE_DAEMON_ANY_NAME;

   // get certificate (SSLCertificate class)
   QoreObject *pt = test_object_param(params, 1);
   class QoreSSLCertificate *sslc = pt ? (QoreSSLCertificate *)pt->getReferencedPrivateData(CID_SSLCERTIFICATE, xsink) : NULL;
   if (*xsink)
      return NULL;
   class QoreString *pemcert = NULL;
   if (sslc)
   {
      pemcert = sslc->getPEM(xsink);
      if (!pemcert)
      {
	 sslc->deref();
	 return NULL;
      }
   }

   TibrvStatus status = TibrvSdContext::setDaemonCert((const char *)name, (pemcert ? pemcert->getBuffer() : TIBRV_SECURE_DAEMON_ANY_CERT));
   if (pemcert)
   {
      delete pemcert;
      sslc->deref();
   }
   if (status != TIBRV_OK)
      xsink->raiseException("TIBRV-SET-DAEMON-CERT-ERROR", "%s", status.getText());

   return NULL;
}

// params: certificate, key, password
static class AbstractQoreNode *f_tibrvSetUserCertWithKey(const QoreListNode *params, class ExceptionSink *xsink)
{
   // get certificate (SSLCertificate class)
   QoreObject *pt = test_object_param(params, 0);
   QoreSSLCertificate *cert = pt ? (QoreSSLCertificate *)pt->getReferencedPrivateData(CID_SSLCERTIFICATE, xsink) : NULL;
   if (*xsink)
      return NULL;
   if (!cert)
   {
      xsink->raiseException("TIBRV-SET-USER-CERT-WTIH-KEY-ERROR", "expecting SSLCertificate object as first parameter to method");
      return NULL;
   }
   TempString pemcert(cert->getPEM(xsink));
   cert->deref();
   if (!pemcert)
      return NULL;

   pt = test_object_param(params, 1);
   class QoreSSLPrivateKey *pk = pt ? (QoreSSLPrivateKey *)pt->getReferencedPrivateData(CID_SSLPRIVATEKEY, xsink) : NULL;
   if (*xsink)
      return NULL;
   if (!pk)
   {
      xsink->raiseException("TIBRV-SET-USER-CERT-WTIH-KEY-ERROR", "expecting SSLPrivateKey object as second parameter to method");
      return NULL;
   }
   class QoreString *pempk = pk->getPEM(xsink);
   pk->deref();
   if (!pempk)
      return 0;

   pemcert->concat(pempk->getBuffer());
   delete pempk;

   QoreStringNode *str = test_string_param(params, 2);
   const char *pw = str ? str->getBuffer() : NULL;

   TibrvStatus status = TibrvSdContext::setUserCertWithKey(pemcert->getBuffer(), pw);

   if (status != TIBRV_OK)
      xsink->raiseException("TIBRV-SET-USER-CERT-WTIH-KEY-ERROR", "%s", status.getText());

   return 0;
}
#endif

static QoreHashNode *tibrv_hash_helper(char *key, class AbstractQoreNode *val)
{
   QoreHashNode *h = new QoreHashNode();
   h->setKeyValue("^type^", new QoreStringNode(key), NULL);
   h->setKeyValue("^value^", val ? val->RefSelf() : 0, NULL);
   return h;
}

static class AbstractQoreNode *f_tibrv_i8(const QoreListNode *params, class ExceptionSink *xsink)
{
   return tibrv_hash_helper("i8", get_param(params, 0));
}

static class AbstractQoreNode *f_tibrv_u8(const QoreListNode *params, class ExceptionSink *xsink)
{
   return tibrv_hash_helper("u8", get_param(params, 0));
}

static class AbstractQoreNode *f_tibrv_i16(const QoreListNode *params, class ExceptionSink *xsink)
{
   return tibrv_hash_helper("i16", get_param(params, 0));
}

static class AbstractQoreNode *f_tibrv_u16(const QoreListNode *params, class ExceptionSink *xsink)
{
   return tibrv_hash_helper("u16", get_param(params, 0));
}

static class AbstractQoreNode *f_tibrv_i32(const QoreListNode *params, class ExceptionSink *xsink)
{
   return tibrv_hash_helper("i32", get_param(params, 0));
}

static class AbstractQoreNode *f_tibrv_u32(const QoreListNode *params, class ExceptionSink *xsink)
{
   return tibrv_hash_helper("u32", get_param(params, 0));
}

static class AbstractQoreNode *f_tibrv_i64(const QoreListNode *params, class ExceptionSink *xsink)
{
   return tibrv_hash_helper("i64", get_param(params, 0));
}

static class AbstractQoreNode *f_tibrv_u64(const QoreListNode *params, class ExceptionSink *xsink)
{
   return tibrv_hash_helper("u64", get_param(params, 0));
}

static class AbstractQoreNode *f_tibrv_f32(const QoreListNode *params, class ExceptionSink *xsink)
{
   return tibrv_hash_helper("f32", get_param(params, 0));
}

static class AbstractQoreNode *f_tibrv_f64(const QoreListNode *params, class ExceptionSink *xsink)
{
   return tibrv_hash_helper("f64", get_param(params, 0));
}

static class AbstractQoreNode *f_tibrv_ipport16(const QoreListNode *params, class ExceptionSink *xsink)
{
   return tibrv_hash_helper("ipport16", get_param(params, 0));
}

static class AbstractQoreNode *f_tibrv_ipaddr32(const QoreListNode *params, class ExceptionSink *xsink)
{
   return tibrv_hash_helper("ipaddr32", get_param(params, 0));
}

static class AbstractQoreNode *f_tibrv_xml(const QoreListNode *params, class ExceptionSink *xsink)
{
   return tibrv_hash_helper("xml", get_param(params, 0));
}

static class AbstractQoreNode *f_tibrv_bool(const QoreListNode *params, class ExceptionSink *xsink)
{
   return tibrv_hash_helper("bool", get_param(params, 0));
}

static class AbstractQoreNode *f_tibrvGetVersion(const QoreListNode *params, class ExceptionSink *xsink)
{
   return new QoreStringNode(Tibrv::version());
}

void init_tibrv_functions()
{
#ifdef TIBRV_SD
   builtinFunctions.add("tibrvSetDaemonCert", f_tibrvSetDaemonCert);
   builtinFunctions.add("tibrvSetUserCertWithKey", f_tibrvSetUserCertWithKey);
#endif
   builtinFunctions.add("tibrvGetVersion", f_tibrvGetVersion);
   builtinFunctions.add("tibrv_i8",        f_tibrv_i8);
   builtinFunctions.add("tibrv_u8",        f_tibrv_u8);
   builtinFunctions.add("tibrv_i16",       f_tibrv_i16);
   builtinFunctions.add("tibrv_u16",       f_tibrv_u16);
   builtinFunctions.add("tibrv_i32",       f_tibrv_i32);
   builtinFunctions.add("tibrv_u32",       f_tibrv_u32);
   builtinFunctions.add("tibrv_i64",       f_tibrv_i64);
   builtinFunctions.add("tibrv_u64",       f_tibrv_u64);
   builtinFunctions.add("tibrv_f32",       f_tibrv_f32);
   builtinFunctions.add("tibrv_f64",       f_tibrv_f64);
   builtinFunctions.add("tibrv_ipport16",  f_tibrv_ipport16);
   builtinFunctions.add("tibrv_ipaddr32",  f_tibrv_ipaddr32);
   builtinFunctions.add("tibrv_xml",       f_tibrv_xml);
   builtinFunctions.add("tibrv_bool",      f_tibrv_bool);
}
