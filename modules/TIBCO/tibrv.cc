/*
  TIBCO/tibrv.cc

  TIBCO Active Enterprise integration to QORE

  Copyright (C) 2003, 2004, 2005, 2006 David Nichols

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
#include <qore/support.h>
#include <qore/BuiltinFunctionList.h>
#include <qore/QoreNode.h>
#include <qore/Exception.h>
#include <qore/QoreString.h>
#include <qore/params.h>
#include <qore/Object.h>
#include <qore/QC_SSLCertificate.h>
#include <qore/QC_SSLPrivateKey.h>

#include <tibrv/tibrvcpp.h>
#include <tibrv/sdcpp.h>

#ifdef TIBRV_SD
// params: name, certificate
static class QoreNode *f_tibrvSetDaemonCert(class QoreNode *params, class ExceptionSink *xsink)
{
   // get daemon name (format: "ssl:<host>:<port_number>")
   class QoreNode *pt = test_param(params, NT_STRING, 0);
   char *name = pt ? pt->val.String->getBuffer() : TIBRV_SECURE_DAEMON_ANY_NAME;

   // get certificate (SSLCertificate class)
   pt = test_param(params, NT_OBJECT, 1);
   class QoreSSLCertificate *sslc = pt ? (QoreSSLCertificate *)pt->val.object->getReferencedPrivateData(CID_SSLCERTIFICATE) : NULL;
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

   TibrvStatus status = TibrvSdContext::setDaemonCert((const char *)name, (const char *)(pemcert ? pemcert->getBuffer() : TIBRV_SECURE_DAEMON_ANY_CERT));
   if (pemcert)
   {
      delete pemcert;
      sslc->deref();
   }
   if (status != TIBRV_OK)
      xsink->raiseException("TIBRV-SET-DAEMON-CERT-ERROR", "%s", (char *)status.getText());

   return NULL;
}

// params: certificate, key, password
static class QoreNode *f_tibrvSetUserCertWithKey(class QoreNode *params, class ExceptionSink *xsink)
{
   // get certificate (SSLCertificate class)
   class QoreNode *pt = test_param(params, NT_OBJECT, 0);
   class QoreSSLCertificate *cert = pt ? (QoreSSLCertificate *)pt->val.object->getReferencedPrivateData(CID_SSLCERTIFICATE) : NULL;
   if (!cert)
   {
      xsink->raiseException("TIBRV-SET-USER-CERT-WTIH-KEY-ERROR", "expecting SSLCertificate object as first parameter to method");
      return NULL;
   }
   class QoreString *pemcert = cert->getPEM(xsink);
   cert->deref();
   if (!pemcert)
      return NULL;

   pt = test_param(params, NT_OBJECT, 1);
   class QoreSSLPrivateKey *pk = pt ? (QoreSSLPrivateKey *)pt->val.object->getReferencedPrivateData(CID_SSLPRIVATEKEY) : NULL;
   if (!pk)
   {
      xsink->raiseException("TIBRV-SET-USER-CERT-WTIH-KEY-ERROR", "expecting SSLPrivateKey object as second parameter to method");
      return NULL;
   }
   class QoreString *pempk = pk->getPEM(xsink);
   pk->deref();
   if (!pempk)
   {
      delete pemcert;
      return NULL;
   }
   pemcert->concat(pempk);
   delete pempk;

   pt = test_param(params, NT_STRING, 2);
   char *pw = pt ? pt->val.String->getBuffer() : NULL;

   TibrvStatus status = TibrvSdContext::setUserCertWithKey((const char *)pemcert->getBuffer(), (const char *)pw);

   delete pemcert;

   if (status != TIBRV_OK)
      xsink->raiseException("TIBRV-SET-USER-CERT-WTIH-KEY-ERROR", "%s", (char *)status.getText());

   return NULL;
}
#endif

static class QoreNode *f_tibrvGetVersion(class QoreNode *params, class ExceptionSink *xsink)
{
   return new QoreNode(Tibrv::version());
}

void init_tibrv_functions()
{
#ifdef TIBRV_SD
   builtinFunctions.add("tibrvSetDaemonCert", f_tibrvSetDaemonCert);
   builtinFunctions.add("tibrvSetUserCertWithKey", f_tibrvSetUserCertWithKey);
#endif
   builtinFunctions.add("tibrvGetVersion", f_tibrvGetVersion);
}
