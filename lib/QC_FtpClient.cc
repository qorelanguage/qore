/*
  QC_FtpClient.cc

  Qore Programming Language

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
#include <qore/intern/ssl_constants.h>

#include <qore/intern/QC_FtpClient.h>

int CID_FTPCLIENT;

static void FC_constructor(class QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   class QoreStringNode *p0 = test_string_param(params, 0);

   class QoreFtpClientClass *f = new QoreFtpClientClass(p0, xsink);
   if (xsink->isException())
   {
      f->deref();
      return;
   }

   self->setPrivate(CID_FTPCLIENT, f);
}

static void FC_copy(class QoreObject *self, class QoreObject *old, class QoreFtpClientClass *f, class ExceptionSink *xsink)
{
   xsink->raiseException("FTPCLIENT-COPY-ERROR", "FtpClient objects cannot be copied.");
}

static class AbstractQoreNode *FC_connect(class QoreObject *self, class QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink)
{
   int rc = f->connect(xsink);

   if (xsink->isEvent())
      return NULL;

   return new QoreBigIntNode(rc);
}

static class AbstractQoreNode *FC_disconnect(class QoreObject *self, class QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink)
{
   int rc = f->disconnect();

   if (xsink->isEvent())
      return NULL;

   return new QoreBigIntNode(rc);
}

static class AbstractQoreNode *FC_list(class QoreObject *self, class QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink)
{
   const char *path;
   QoreStringNode *p0 = test_string_param(params, 0);
   if (p0)
      path = p0->getBuffer();
   else
      path = NULL;

   return f->list(path, true, xsink);
}

static class AbstractQoreNode *FC_nlst(class QoreObject *self, class QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink)
{
   const char *path;
   QoreStringNode *p0 = test_string_param(params, 0);
   if (p0)
      path = p0->getBuffer();
   else
      path = NULL;

   return f->list(path, false, xsink);
}

static class AbstractQoreNode *FC_pwd(class QoreObject *self, class QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink)
{
   return f->pwd(xsink);
}

static class AbstractQoreNode *FC_cwd(class QoreObject *self, class QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNode *p0 = test_string_param(params, 0);
   if (!p0 || !p0->strlen())
   {
      xsink->raiseException("FTPCLIENT-CWD-PARAMETER-ERROR", "expecting path(string) as first parameter of FtpClient::cwd()");
      return NULL;
   }

   int rc = f->cwd(p0->getBuffer(), xsink);
   if (xsink->isEvent())
      return  NULL;

   return new QoreBigIntNode(rc);
}

static class AbstractQoreNode *FC_put(class QoreObject *self, class QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNode *p0 = test_string_param(params, 0);
   if (!p0 || !p0->strlen())
   {
      xsink->raiseException("FTPCLIENT-PUT-PARAMETER-ERROR", "expecting path(string) as first parameter of FtpClient::put()");
      return NULL;
   }
   const char *rn;
   QoreStringNode *p1 = test_string_param(params, 1);
   if (p1)
      rn = p1->getBuffer();
   else
      rn = NULL;

   int rc = f->put(p0->getBuffer(), rn, xsink);
   if (xsink->isEvent())
      return NULL;

   return new QoreBigIntNode(rc);
}

static class AbstractQoreNode *FC_get(class QoreObject *self, class QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNode *p0 = test_string_param(params, 0);
   if (!p0 || !p0->strlen())
   {
      xsink->raiseException("FTPCLIENT-GET-PARAMETER-ERROR", "expecting path(string) as first parameter of FtpClient::get()");
      return NULL;
   }
   const char *ln;
   QoreStringNode *p1 = test_string_param(params, 1);
   if (p1)
      ln = p1->getBuffer();
   else
      ln = NULL;

   int rc = f->get(p0->getBuffer(), ln, xsink);
   if (xsink->isEvent())
      return NULL;

   return new QoreBigIntNode(rc);
}

static class AbstractQoreNode *FC_del(class QoreObject *self, class QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNode *p0 = test_string_param(params, 0);
   if (!p0 || !p0->strlen())
   {
      xsink->raiseException("FTPCLIENT-DEL-PARAMETER-ERROR", "expecting path(string) as first parameter of FtpClient::del()");
      return NULL;
   }

   int rc = f->del(p0->getBuffer(), xsink);
   if (xsink->isEvent())
      return NULL;

   return new QoreBigIntNode(rc);
}

static class AbstractQoreNode *FC_setUserName(class QoreObject *self, class QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNode *p0 = test_string_param(params, 0);
   if (!p0)
   {
      xsink->raiseException("FTPCLIENT-SETUSERNAME-PARAMETER-ERROR", "expecting path(string) as first parameter of FtpClient::setUserName()");
      return NULL;
   }

   f->setUserName(p0->getBuffer());
   return NULL;
}

static class AbstractQoreNode *FC_setPassword(class QoreObject *self, class QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNode *p0 = test_string_param(params, 0);
   if (!p0)
   {
      xsink->raiseException("FTPCLIENT-SETPASSWORD-PARAMETER-ERROR", "expecting path(string) as first parameter of FtpClient::setPassword()");
      return NULL;
   }

   f->setPassword(p0->getBuffer());
   return NULL;
}

static class AbstractQoreNode *FC_setHostName(class QoreObject *self, class QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNode *p0 = test_string_param(params, 0);
   if (!p0)
   {
      xsink->raiseException("FTPCLIENT-SETHOSTNAME-PARAMETER-ERROR", "expecting name(string) as first parameter of FtpClient::setHostName()");
      return NULL;
   }

   f->setHostName(p0->getBuffer());
   return NULL;
}

static class AbstractQoreNode *FC_setPort(class QoreObject *self, class QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p0 = get_param(params, 0);
   int port;
   if (p0)
      port = p0->getAsInt();
   else
      port = 0;
   if (!port)
   {
      xsink->raiseException("FTPCLIENT-SETPORT-PARAMETER-ERROR", "expecting non-zero port(int) as first parameter of FtpClient::setPort()");
      return NULL;
   }

   f->setPort(port);
   return NULL;
}

static class AbstractQoreNode *FC_setURL(class QoreObject *self, class QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNode *p0 = test_string_param(params, 0);
   if (!p0)
   {
      xsink->raiseException("FTPCLIENT-SETURL-PARAMETER-ERROR", "expecting url(string) as first parameter of FtpClient::setURL()");
      return NULL;
   }

   f->setURL(p0, xsink);
   return NULL;
}

static class AbstractQoreNode *FC_getUserName(class QoreObject *self, class QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink)
{
   const char *u = f->getUserName();
   if (u)
      return new QoreStringNode(u); 

   return NULL;
}

static class AbstractQoreNode *FC_getPassword(class QoreObject *self, class QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink)
{
   const char *p = f->getPassword();
   if (p)
      return new QoreStringNode(p); 

   return NULL;
}

static class AbstractQoreNode *FC_getHostName(class QoreObject *self, class QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink)
{
   const char *h = f->getHostName();
   if (h)
      return new QoreStringNode(h); 

   return NULL;
}

static class AbstractQoreNode *FC_getPort(class QoreObject *self, class QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(f->getPort());
}

static class AbstractQoreNode *FC_getURL(class QoreObject *self, class QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink)
{
   return f->getURL();
}

static class AbstractQoreNode *FC_setSecure(class QoreObject *self, class QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink)
{
   if (f->setSecure())
      xsink->raiseException("SET-SECURE-ERROR", "this method cannot be called while the control connection is established");

   return NULL;
}

static class AbstractQoreNode *FC_setInsecure(class QoreObject *self, class QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink)
{
   if (f->setInsecure())
      xsink->raiseException("SET-INSECURE-ERROR", "this method cannot be called while the control connection is established");

   return NULL;
}

static class AbstractQoreNode *FC_setInsecureData(class QoreObject *self, class QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink)
{
   if (f->setInsecureData())
      xsink->raiseException("SET-INSECUREDATA-ERROR", "this method cannot be called while the control connection is established");

   return NULL;
}

static class AbstractQoreNode *FC_isSecure(class QoreObject *self, class QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(f->isSecure());
}

static class AbstractQoreNode *FC_isDataSecure(class QoreObject *self, class QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(f->isDataSecure());
}

static class AbstractQoreNode *FC_getSSLCipherName(class QoreObject *self, class QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink)
{
   const char *str = f->getSSLCipherName();
   if (str)
      return new QoreStringNode(str);

   return NULL;
}

static class AbstractQoreNode *FC_getSSLCipherVersion(class QoreObject *self, class QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink)
{
   const char *str = f->getSSLCipherVersion();
   if (str)
      return new QoreStringNode(str);

   return NULL;
}

static class AbstractQoreNode *FC_verifyPeerCertificate(class QoreObject *self, class QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink)
{
   const char *c = getSSLCVCode(f->verifyPeerCertificate());
   return c ? new QoreStringNode(c) : NULL;
}

static class AbstractQoreNode *FC_setModeAuto(class QoreObject *self, class QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink)
{
   f->setModeAuto();
   return NULL;
}

static class AbstractQoreNode *FC_setModeEPSV(class QoreObject *self, class QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink)
{
   f->setModeEPSV();
   return NULL;
}

static class AbstractQoreNode *FC_setModePASV(class QoreObject *self, class QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink)
{
   f->setModePASV();
   return NULL;
}

static class AbstractQoreNode *FC_setModePORT(class QoreObject *self, class QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink)
{
   f->setModePORT();
   return NULL;
}

class QoreClass *initFtpClientClass()
{
   tracein("initFtpClientClass()");

   class QoreClass *QC_FTPCLIENT = new QoreClass("FtpClient", QDOM_NETWORK);
   CID_FTPCLIENT = QC_FTPCLIENT->getID();
   QC_FTPCLIENT->setConstructor(FC_constructor);
   QC_FTPCLIENT->setCopy((q_copy_t)FC_copy);
   QC_FTPCLIENT->addMethod("connect",               (q_method_t)FC_connect);
   QC_FTPCLIENT->addMethod("disconnect",            (q_method_t)FC_disconnect);
   QC_FTPCLIENT->addMethod("list",                  (q_method_t)FC_list);
   QC_FTPCLIENT->addMethod("nlst",                  (q_method_t)FC_nlst);
   QC_FTPCLIENT->addMethod("pwd",                   (q_method_t)FC_pwd);
   QC_FTPCLIENT->addMethod("cwd",                   (q_method_t)FC_cwd);
   QC_FTPCLIENT->addMethod("get",                   (q_method_t)FC_get);
   QC_FTPCLIENT->addMethod("put",                   (q_method_t)FC_put);
   QC_FTPCLIENT->addMethod("del",                   (q_method_t)FC_del);
   QC_FTPCLIENT->addMethod("setUserName",           (q_method_t)FC_setUserName);
   QC_FTPCLIENT->addMethod("setPassword",           (q_method_t)FC_setPassword);
   QC_FTPCLIENT->addMethod("setHostName",           (q_method_t)FC_setHostName);
   QC_FTPCLIENT->addMethod("setPort",               (q_method_t)FC_setPort);
   QC_FTPCLIENT->addMethod("setURL",                (q_method_t)FC_setURL);
   QC_FTPCLIENT->addMethod("getUserName",           (q_method_t)FC_getUserName);
   QC_FTPCLIENT->addMethod("getPassword",           (q_method_t)FC_getPassword);
   QC_FTPCLIENT->addMethod("getHostName",           (q_method_t)FC_getHostName);
   QC_FTPCLIENT->addMethod("getPort",               (q_method_t)FC_getPort);
   QC_FTPCLIENT->addMethod("getURL",                (q_method_t)FC_getURL);
   QC_FTPCLIENT->addMethod("setSecure",             (q_method_t)FC_setSecure );
   QC_FTPCLIENT->addMethod("setInsecure",           (q_method_t)FC_setInsecure ); 
   QC_FTPCLIENT->addMethod("setInsecureData",       (q_method_t)FC_setInsecureData );
   QC_FTPCLIENT->addMethod("isSecure",              (q_method_t)FC_isSecure ); 
   QC_FTPCLIENT->addMethod("isDataSecure",          (q_method_t)FC_isDataSecure );
   QC_FTPCLIENT->addMethod("getSSLCipherName",      (q_method_t)FC_getSSLCipherName );
   QC_FTPCLIENT->addMethod("getSSLCipherVersion",   (q_method_t)FC_getSSLCipherVersion );
   QC_FTPCLIENT->addMethod("verifyPeerCertificate", (q_method_t)FC_verifyPeerCertificate );
   QC_FTPCLIENT->addMethod("setModeAuto",           (q_method_t)FC_setModeAuto );
   QC_FTPCLIENT->addMethod("setModeEPSV",           (q_method_t)FC_setModeEPSV );
   QC_FTPCLIENT->addMethod("setModePASV",           (q_method_t)FC_setModePASV );
   QC_FTPCLIENT->addMethod("setModePORT",           (q_method_t)FC_setModePORT );

   traceout("initFtpClientClass()");
   return QC_FTPCLIENT;
}
