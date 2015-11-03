/*
  QC_FtpClient.cc

  Qore Programming Language

  Copyright 2003 - 2009 David Nichols

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

qore_classid_t CID_FTPCLIENT;

static void FC_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = test_string_param(params, 0);

   QoreFtpClientClass *f = new QoreFtpClientClass(p0, xsink);
   if (xsink->isException()) {
      f->deref(xsink);
      return;
   }

   self->setPrivate(CID_FTPCLIENT, f);
}

static void FC_copy(QoreObject *self, QoreObject *old, class QoreFtpClientClass *f, ExceptionSink *xsink) {
   xsink->raiseException("FTPCLIENT-COPY-ERROR", "FtpClient objects cannot be copied.");
}

static void FC_destructor(QoreObject *self, QoreFtpClientClass *f, ExceptionSink *xsink) {
   // have to clear callbacks before destroying
   f->cleanup(xsink);
   f->deref(xsink);
}

static AbstractQoreNode *FC_connect(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink) {
   int rc = f->connect(xsink);

   if (xsink->isEvent())
      return 0;

   return new QoreBigIntNode(rc);
}

static AbstractQoreNode *FC_disconnect(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink) {
   int rc = f->disconnect();

   if (xsink->isEvent())
      return 0;

   return new QoreBigIntNode(rc);
}

static AbstractQoreNode *FC_list(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink)
{
   const char *path;
   const QoreStringNode *p0 = test_string_param(params, 0);
   if (p0)
      path = p0->getBuffer();
   else
      path = 0;

   return f->list(path, true, xsink);
}

static AbstractQoreNode *FC_nlst(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink)
{
   const char *path;
   const QoreStringNode *p0 = test_string_param(params, 0);
   if (p0)
      path = p0->getBuffer();
   else
      path = 0;

   return f->list(path, false, xsink);
}

static AbstractQoreNode *FC_pwd(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink)
{
   return f->pwd(xsink);
}

static AbstractQoreNode *FC_cwd(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *p0 = test_string_param(params, 0);
   if (!p0 || !p0->strlen())
   {
      xsink->raiseException("FTPCLIENT-CWD-PARAMETER-ERROR", "expecting path(string) as first parameter of FtpClient::cwd()");
      return 0;
   }

   int rc = f->cwd(p0->getBuffer(), xsink);
   if (xsink->isEvent())
      return  NULL;

   return new QoreBigIntNode(rc);
}

static AbstractQoreNode *FC_put(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = test_string_param(params, 0);
   if (!p0 || !p0->strlen()) {
      xsink->raiseException("FTPCLIENT-PUT-PARAMETER-ERROR", "expecting path(string) as first parameter of FtpClient::put()");
      return 0;
   }

   // FIXME: this class cannot write to a file when parse option PO_NO_FILESYSTEM is set
   if (getProgram()->getParseOptions() & PO_NO_FILESYSTEM) {
      xsink->raiseException("INVALID-FILESYSTEM-ACCESS", "FtpClient::put() cannot be used when parse option NO-FILESYSTEM is set");
      return 0;
   }

   const char *rn;
   const QoreStringNode *p1 = test_string_param(params, 1);
   if (p1)
      rn = p1->getBuffer();
   else
      rn = 0;

   int rc = f->put(p0->getBuffer(), rn, xsink);
   if (xsink->isEvent())
      return 0;

   return new QoreBigIntNode(rc);
}

static AbstractQoreNode *FC_get(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = test_string_param(params, 0);
   if (!p0 || !p0->strlen()) {
      xsink->raiseException("FTPCLIENT-GET-PARAMETER-ERROR", "expecting path(string) as first parameter of FtpClient::get()");
      return 0;
   }

   // FIXME: this class cannot write to a file when parse option PO_NO_FILESYSTEM is set
   if (getProgram()->getParseOptions() & PO_NO_FILESYSTEM) {
      xsink->raiseException("INVALID-FILESYSTEM-ACCESS", "FtpClient::get() cannot be used when parse option NO-FILESYSTEM is set");
      return 0;
   }

   const QoreStringNode *p1 = test_string_param(params, 1);
   const char *ln = p1 ? p1->getBuffer() : 0;

   int rc = f->get(p0->getBuffer(), ln, xsink);
   if (xsink->isEvent())
      return 0;

   return new QoreBigIntNode(rc);
}

static AbstractQoreNode *FC_getAsString(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = test_string_param(params, 0);
   if (!p0 || !p0->strlen()) {
      xsink->raiseException("FTPCLIENT-GETASSTRING-PARAMETER-ERROR", "expecting path(string) as first parameter of FtpClient::getAsString()");
      return 0;
   }

   return f->getAsString(p0->getBuffer(), xsink);
}

static AbstractQoreNode *FC_getAsBinary(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = test_string_param(params, 0);
   if (!p0 || !p0->strlen()) {
      xsink->raiseException("FTPCLIENT-GETASBINARY-PARAMETER-ERROR", "expecting path(string) as first parameter of FtpClient::getAsBinary()");
      return 0;
   }

   return f->getAsBinary(p0->getBuffer(), xsink);
}

static AbstractQoreNode *FC_del(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = test_string_param(params, 0);
   if (!p0 || !p0->strlen()) {
      xsink->raiseException("FTPCLIENT-DEL-PARAMETER-ERROR", "expecting path(string) as first parameter of FtpClient::del()");
      return 0;
   }

   int rc = f->del(p0->getBuffer(), xsink);
   if (xsink->isEvent())
      return 0;

   return new QoreBigIntNode(rc);
}

static AbstractQoreNode *FC_setUserName(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = test_string_param(params, 0);
   if (!p0) {
      xsink->raiseException("FTPCLIENT-SETUSERNAME-PARAMETER-ERROR", "expecting path(string) as first parameter of FtpClient::setUserName()");
      return 0;
   }

   f->setUserName(p0->getBuffer());
   return 0;
}

static AbstractQoreNode *FC_setPassword(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = test_string_param(params, 0);
   if (!p0) {
      xsink->raiseException("FTPCLIENT-SETPASSWORD-PARAMETER-ERROR", "expecting path(string) as first parameter of FtpClient::setPassword()");
      return 0;
   }

   f->setPassword(p0->getBuffer());
   return 0;
}

static AbstractQoreNode *FC_setHostName(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = test_string_param(params, 0);
   if (!p0) {
      xsink->raiseException("FTPCLIENT-SETHOSTNAME-PARAMETER-ERROR", "expecting name(string) as first parameter of FtpClient::setHostName()");
      return 0;
   }

   f->setHostName(p0->getBuffer());
   return 0;
}

static AbstractQoreNode *FC_setPort(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0 = get_param(params, 0);
   int port;
   if (p0)
      port = p0->getAsInt();
   else
      port = 0;
   if (!port)
   {
      xsink->raiseException("FTPCLIENT-SETPORT-PARAMETER-ERROR", "expecting non-zero port(int) as first parameter of FtpClient::setPort()");
      return 0;
   }

   f->setPort(port);
   return 0;
}

static AbstractQoreNode *FC_setURL(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *p0 = test_string_param(params, 0);
   if (!p0)
   {
      xsink->raiseException("FTPCLIENT-SETURL-PARAMETER-ERROR", "expecting url(string) as first parameter of FtpClient::setURL()");
      return 0;
   }

   f->setURL(p0, xsink);
   return 0;
}

static AbstractQoreNode *FC_getUserName(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink) {
   const char *u = f->getUserName();
   if (u)
      return new QoreStringNode(u); 

   return 0;
}

static AbstractQoreNode *FC_getPassword(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink) {
   const char *p = f->getPassword();
   if (p)
      return new QoreStringNode(p); 

   return 0;
}

static AbstractQoreNode *FC_getHostName(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink) {
   const char *h = f->getHostName();
   if (h)
      return new QoreStringNode(h); 

   return 0;
}

static AbstractQoreNode *FC_getPort(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreBigIntNode(f->getPort());
}

static AbstractQoreNode *FC_getURL(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink) {
   return f->getURL();
}

static AbstractQoreNode *FC_setSecure(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink) {
   if (f->setSecure())
      xsink->raiseException("SET-SECURE-ERROR", "this method cannot be called while the control connection is established");

   return 0;
}

static AbstractQoreNode *FC_setInsecure(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink) {
   if (f->setInsecure())
      xsink->raiseException("SET-INSECURE-ERROR", "this method cannot be called while the control connection is established");

   return 0;
}

static AbstractQoreNode *FC_setInsecureData(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink) {
   if (f->setInsecureData())
      xsink->raiseException("SET-INSECUREDATA-ERROR", "this method cannot be called while the control connection is established");

   return 0;
}

static AbstractQoreNode *FC_isSecure(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink) {
   return get_bool_node(f->isSecure());
}

static AbstractQoreNode *FC_isDataSecure(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink) {
   return get_bool_node(f->isDataSecure());
}

static AbstractQoreNode *FC_getSSLCipherName(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink) {
   const char *str = f->getSSLCipherName();
   if (str)
      return new QoreStringNode(str);

   return 0;
}

static AbstractQoreNode *FC_getSSLCipherVersion(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink) {
   const char *str = f->getSSLCipherVersion();
   if (str)
      return new QoreStringNode(str);

   return 0;
}

static AbstractQoreNode *FC_verifyPeerCertificate(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink) {
   const char *c = getSSLCVCode(f->verifyPeerCertificate());
   return c ? new QoreStringNode(c) : 0;
}

static AbstractQoreNode *FC_setModeAuto(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink) {
   f->setModeAuto();
   return 0;
}

static AbstractQoreNode *FC_setModeEPSV(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink) {
   f->setModeEPSV();
   return 0;
}

static AbstractQoreNode *FC_setModePASV(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink) {
   f->setModePASV();
   return 0;
}

static AbstractQoreNode *FC_setModePORT(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink) {
   f->setModePORT();
   return 0;
}

static AbstractQoreNode *FC_setEventQueue(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink) {
    const QoreObject *o = test_object_param(params, 0);
    Queue *q = o ? (Queue *)o->getReferencedPrivateData(CID_QUEUE, xsink) : 0;
    if (*xsink)
        return 0;
    // pass reference from QoreObject::getReferencedPrivateData() to function
    f->setEventQueue(q, xsink);
    return 0;
}

static AbstractQoreNode *FC_setDataEventQueue(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink) {
    const QoreObject *o = test_object_param(params, 0);
    Queue *q = o ? (Queue *)o->getReferencedPrivateData(CID_QUEUE, xsink) : 0;
    if (*xsink)
        return 0;
    // pass reference from QoreObject::getReferencedPrivateData() to function
    f->setDataEventQueue(q, xsink);
    return 0;
}

static AbstractQoreNode *FC_setControlEventQueue(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *params, ExceptionSink *xsink) {
    const QoreObject *o = test_object_param(params, 0);
    Queue *q = o ? (Queue *)o->getReferencedPrivateData(CID_QUEUE, xsink) : 0;
    if (*xsink)
        return 0;
    // pass reference from QoreObject::getReferencedPrivateData() to function
    f->setControlEventQueue(q, xsink);
    return 0;
}

QoreClass *initFtpClientClass() {
   QORE_TRACE("initFtpClientClass()");

   // NOTE: the QoreFtpClient class is thread-safe 
   QoreClass *QC_FTPCLIENT = new QoreClass("FtpClient", QDOM_NETWORK);
   CID_FTPCLIENT = QC_FTPCLIENT->getID();

   QC_FTPCLIENT->setConstructor(FC_constructor);
   QC_FTPCLIENT->setDestructor((q_destructor_t)FC_destructor);
   QC_FTPCLIENT->setCopy((q_copy_t)FC_copy);
   QC_FTPCLIENT->addMethod("connect",               (q_method_t)FC_connect);
   QC_FTPCLIENT->addMethod("disconnect",            (q_method_t)FC_disconnect);
   QC_FTPCLIENT->addMethod("list",                  (q_method_t)FC_list);
   QC_FTPCLIENT->addMethod("nlst",                  (q_method_t)FC_nlst);
   QC_FTPCLIENT->addMethod("pwd",                   (q_method_t)FC_pwd);
   QC_FTPCLIENT->addMethod("cwd",                   (q_method_t)FC_cwd);
   QC_FTPCLIENT->addMethod("get",                   (q_method_t)FC_get);
   QC_FTPCLIENT->addMethod("getAsString",           (q_method_t)FC_getAsString);
   QC_FTPCLIENT->addMethod("getAsBinary",           (q_method_t)FC_getAsBinary);
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
   QC_FTPCLIENT->addMethod("setSecure",             (q_method_t)FC_setSecure);
   QC_FTPCLIENT->addMethod("setInsecure",           (q_method_t)FC_setInsecure); 
   QC_FTPCLIENT->addMethod("setInsecureData",       (q_method_t)FC_setInsecureData);
   QC_FTPCLIENT->addMethod("isSecure",              (q_method_t)FC_isSecure); 
   QC_FTPCLIENT->addMethod("isDataSecure",          (q_method_t)FC_isDataSecure);
   QC_FTPCLIENT->addMethod("getSSLCipherName",      (q_method_t)FC_getSSLCipherName);
   QC_FTPCLIENT->addMethod("getSSLCipherVersion",   (q_method_t)FC_getSSLCipherVersion);
   QC_FTPCLIENT->addMethod("verifyPeerCertificate", (q_method_t)FC_verifyPeerCertificate);
   QC_FTPCLIENT->addMethod("setModeAuto",           (q_method_t)FC_setModeAuto);
   QC_FTPCLIENT->addMethod("setModeEPSV",           (q_method_t)FC_setModeEPSV);
   QC_FTPCLIENT->addMethod("setModePASV",           (q_method_t)FC_setModePASV);
   QC_FTPCLIENT->addMethod("setModePORT",           (q_method_t)FC_setModePORT);
   QC_FTPCLIENT->addMethod("setEventQueue",         (q_method_t)FC_setEventQueue);
   QC_FTPCLIENT->addMethod("setDataEventQueue",     (q_method_t)FC_setDataEventQueue);
   QC_FTPCLIENT->addMethod("setControlEventQueue",  (q_method_t)FC_setControlEventQueue);

   return QC_FTPCLIENT;
}
