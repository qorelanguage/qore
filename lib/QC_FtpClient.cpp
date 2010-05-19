/*
  QC_FtpClient.cpp

  Qore Programming Language

  Copyright 2003 - 2010 David Nichols

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
#include <qore/intern/QC_Queue.h>

qore_classid_t CID_FTPCLIENT;

static void FC_constructor(QoreObject *self, const QoreListNode *args, ExceptionSink *xsink) {
   self->setPrivate(CID_FTPCLIENT, new QoreFtpClientClass);
}

static void FC_constructor_str(QoreObject *self, const QoreListNode *args, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(args, 0);

   ReferenceHolder<QoreFtpClientClass> f(new QoreFtpClientClass(p0, xsink), xsink);
   if (*xsink)
      return;

   self->setPrivate(CID_FTPCLIENT, f.release());
}

static void FC_copy(QoreObject *self, QoreObject *old, class QoreFtpClientClass *f, ExceptionSink *xsink) {
   xsink->raiseException("FTPCLIENT-COPY-ERROR", "FtpClient objects cannot be copied");
}

static void FC_destructor(QoreObject *self, QoreFtpClientClass *f, ExceptionSink *xsink) {
   // have to clear callbacks before destroying
   f->cleanup(xsink);
   f->deref(xsink);
}

static AbstractQoreNode *FC_connect(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *args, ExceptionSink *xsink) {
   int rc = f->connect(xsink);
   return *xsink ? 0 : new QoreBigIntNode(rc);
}

static AbstractQoreNode *FC_disconnect(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *args, ExceptionSink *xsink) {
   int rc = f->disconnect();
   return *xsink ? 0 : new QoreBigIntNode(rc);
}

static AbstractQoreNode *FC_list(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *args, ExceptionSink *xsink) {
   return f->list(0, true, xsink);
}

static AbstractQoreNode *FC_list_str(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *args, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(args, 0);
   return f->list(p0->getBuffer(), true, xsink);
}

static AbstractQoreNode *FC_nlst(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *args, ExceptionSink *xsink) {
   return f->list(0, false, xsink);
}

static AbstractQoreNode *FC_nlst_str(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *args, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(args, 0);
   return f->list(p0->getBuffer(), true, xsink);
}

static AbstractQoreNode *FC_pwd(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *args, ExceptionSink *xsink) {
   return f->pwd(xsink);
}

static AbstractQoreNode *FC_cwd(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *args, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(args, 0);
   int rc = f->cwd(p0->getBuffer(), xsink);
   return *xsink ? 0 : new QoreBigIntNode(rc);
}

static AbstractQoreNode *FC_put_str_str(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *args, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(args, 0);
   HARD_QORE_PARAM(p1, const QoreStringNode, args, 1);

   int rc = f->put(p0->getBuffer(), p1->getBuffer(), xsink);
   return *xsink ? 0 : new QoreBigIntNode(rc);
}

static AbstractQoreNode *FC_put_str(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *args, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(args, 0);

   int rc = f->put(p0->getBuffer(), 0, xsink);
   return *xsink ? 0 : new QoreBigIntNode(rc);
}

static AbstractQoreNode *FC_get_str(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *args, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(args, 0);
   int rc = f->get(p0->getBuffer(), 0, xsink);
   return *xsink ? 0 : new QoreBigIntNode(rc);
}

static AbstractQoreNode *FC_get_str_str(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *args, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(args, 0);
   HARD_QORE_PARAM(p1, const QoreStringNode, args, 1);

   int rc = f->get(p0->getBuffer(), p1->getBuffer(), xsink);
   return *xsink ? 0 : new QoreBigIntNode(rc);
}

static AbstractQoreNode *FC_getAsString(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *args, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(args, 0);
   return f->getAsString(p0->getBuffer(), xsink);
}

static AbstractQoreNode *FC_getAsBinary(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *args, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(args, 0);
   return f->getAsBinary(p0->getBuffer(), xsink);
}

static AbstractQoreNode *FC_del(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *args, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(args, 0);
   int rc = f->del(p0->getBuffer(), xsink);
   return *xsink ? 0 : new QoreBigIntNode(rc);
}

static AbstractQoreNode *FC_setUserName(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *args, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(args, 0);
   f->setUserName(p0->getBuffer());
   return 0;
}

static AbstractQoreNode *FC_setPassword(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *args, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(args, 0);
   f->setPassword(p0->getBuffer());
   return 0;
}

static AbstractQoreNode *FC_setHostName(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *args, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(args, 0);
   f->setHostName(p0->getBuffer());
   return 0;
}

static AbstractQoreNode *FC_setPort(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *args, ExceptionSink *xsink) {
   int port = HARD_QORE_INT(args, 0);
   if (port <= 0) {
      xsink->raiseException("FTPCLIENT-SETPORT-PARAMETER-ERROR", "expecting positive port number as first parameter of FtpClient::setPort(softint $port); got %d", port);
      return 0;
   }
   f->setPort(port);
   return 0;
}

static AbstractQoreNode *FC_setURL(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *args, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(args, 0);
   f->setURL(p0, xsink);
   return 0;
}

static AbstractQoreNode *FC_getUserName(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *args, ExceptionSink *xsink) {
   const char *u = f->getUserName();
   return u ? new QoreStringNode(u) : 0;
}

static AbstractQoreNode *FC_getPassword(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *args, ExceptionSink *xsink) {
   const char *p = f->getPassword();
   return p ? new QoreStringNode(p) : 0;
}

static AbstractQoreNode *FC_getHostName(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *args, ExceptionSink *xsink) {
   const char *h = f->getHostName();
   return h ? new QoreStringNode(h) : 0;
}

static AbstractQoreNode *FC_getPort(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *args, ExceptionSink *xsink) {
   return new QoreBigIntNode(f->getPort());
}

static AbstractQoreNode *FC_getURL(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *args, ExceptionSink *xsink) {
   return f->getURL();
}

static AbstractQoreNode *FC_setSecure(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *args, ExceptionSink *xsink) {
   bool b = HARD_QORE_BOOL(args, 0);
   if (b ? f->setSecure() : f->setInsecure())
      xsink->raiseException("SET-SECURE-ERROR", "this method cannot be called while the control connection is established");

   return 0;
}

static AbstractQoreNode *FC_setInsecure(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *args, ExceptionSink *xsink) {
   if (f->setInsecure())
      xsink->raiseException("SET-INSECURE-ERROR", "this method cannot be called while the control connection is established");

   return 0;
}

static AbstractQoreNode *FC_setInsecureData(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *args, ExceptionSink *xsink) {
   if (f->setInsecureData())
      xsink->raiseException("SET-INSECUREDATA-ERROR", "this method cannot be called while the control connection is established");

   return 0;
}

static AbstractQoreNode *FC_isSecure(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *args, ExceptionSink *xsink) {
   return get_bool_node(f->isSecure());
}

static AbstractQoreNode *FC_isDataSecure(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *args, ExceptionSink *xsink) {
   return get_bool_node(f->isDataSecure());
}

static AbstractQoreNode *FC_getSSLCipherName(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *args, ExceptionSink *xsink) {
   const char *str = f->getSSLCipherName();
   return str ? new QoreStringNode(str) : 0;
}

static AbstractQoreNode *FC_getSSLCipherVersion(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *args, ExceptionSink *xsink) {
   const char *str = f->getSSLCipherVersion();
   return str ? new QoreStringNode(str) : 0;
}

static AbstractQoreNode *FC_verifyPeerCertificate(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *args, ExceptionSink *xsink) {
   const char *c = getSSLCVCode(f->verifyPeerCertificate());
   return c ? new QoreStringNode(c) : 0;
}

static AbstractQoreNode *FC_setModeAuto(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *args, ExceptionSink *xsink) {
   f->setModeAuto();
   return 0;
}

static AbstractQoreNode *FC_setModeEPSV(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *args, ExceptionSink *xsink) {
   f->setModeEPSV();
   return 0;
}

static AbstractQoreNode *FC_setModePASV(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *args, ExceptionSink *xsink) {
   f->setModePASV();
   return 0;
}

static AbstractQoreNode *FC_setModePORT(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *args, ExceptionSink *xsink) {
   f->setModePORT();
   return 0;
}

static AbstractQoreNode *FC_setEventQueue_nothing(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *args, ExceptionSink *xsink) {
   f->setEventQueue(0, xsink);
   return 0;
}

static AbstractQoreNode *FC_setEventQueue_queue(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_OBJ_DATA(q, Queue, args, 0, CID_QUEUE, "Queue", "FtpClient::setEventQueue", xsink);
   if (*xsink)
      return 0;

   // pass reference from QoreObject::getReferencedPrivateData() to function
   f->setEventQueue(q, xsink);
   return 0;
}

static AbstractQoreNode *FC_setDataEventQueue_nothing(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *args, ExceptionSink *xsink) {
   f->setDataEventQueue(0, xsink);
   return 0;
}

static AbstractQoreNode *FC_setDataEventQueue_queue(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_OBJ_DATA(q, Queue, args, 0, CID_QUEUE, "Queue", "FtpClient::setDataEventQueue", xsink);
   if (*xsink)
      return 0;

   // pass reference from QoreObject::getReferencedPrivateData() to function
   f->setDataEventQueue(q, xsink);
   return 0;
}

static AbstractQoreNode *FC_setControlEventQueue_nothing(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *args, ExceptionSink *xsink) {
    f->setControlEventQueue(0, xsink);
    return 0;
}

static AbstractQoreNode *FC_setControlEventQueue_queue(QoreObject *self, QoreFtpClientClass *f, const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_OBJ_DATA(q, Queue, args, 0, CID_QUEUE, "Queue", "FtpClient::setDataEventQueue", xsink);
   if (*xsink)
      return 0;

   // pass reference from QoreObject::getReferencedPrivateData() to function
   f->setControlEventQueue(q, xsink);
   return 0;
}

QoreClass *initFtpClientClass() {
   QORE_TRACE("initFtpClientClass()");

   // make sure queue class has been created
   assert(QC_QUEUE);

   // NOTE: the QoreFtpClient class is thread-safe 
   QoreClass *QC_FTPCLIENT = new QoreClass("FtpClient", QDOM_NETWORK);
   CID_FTPCLIENT = QC_FTPCLIENT->getID();

   QC_FTPCLIENT->setConstructorExtended(FC_constructor);
   QC_FTPCLIENT->setConstructorExtended(FC_constructor_str, false, QC_NO_FLAGS, QDOM_DEFAULT, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   QC_FTPCLIENT->setDestructor((q_destructor_t)FC_destructor);

   QC_FTPCLIENT->setCopy((q_copy_t)FC_copy);
   QC_FTPCLIENT->addMethodExtended("connect",               (q_method_t)FC_connect, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo);
   QC_FTPCLIENT->addMethodExtended("disconnect",            (q_method_t)FC_disconnect, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo);

   // returns string or nothing
   QC_FTPCLIENT->addMethodExtended("list",                  (q_method_t)FC_list, false, QC_NO_FLAGS, QDOM_DEFAULT);
   QC_FTPCLIENT->addMethodExtended("list",                  (q_method_t)FC_list_str, false, QC_NO_FLAGS, QDOM_DEFAULT, 0, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   // returns string or nothing
   QC_FTPCLIENT->addMethodExtended("nlst",                  (q_method_t)FC_nlst, false, QC_NO_FLAGS, QDOM_DEFAULT);
   QC_FTPCLIENT->addMethodExtended("nlst",                  (q_method_t)FC_nlst_str, false, QC_NO_FLAGS, QDOM_DEFAULT, 0, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   QC_FTPCLIENT->addMethodExtended("pwd",                   (q_method_t)FC_pwd, false, QC_NO_FLAGS, QDOM_DEFAULT, stringTypeInfo);

   QC_FTPCLIENT->addMethodExtended("cwd",                   (q_method_t)FC_cwd, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   // accesses filesystem so tagged with QDOM_FILESYSTEM
   QC_FTPCLIENT->addMethodExtended("get",                   (q_method_t)FC_get_str, false, QC_NO_FLAGS, QDOM_FILESYSTEM, bigIntTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   QC_FTPCLIENT->addMethodExtended("get",                   (q_method_t)FC_get_str_str, false, QC_NO_FLAGS, QDOM_FILESYSTEM, bigIntTypeInfo, 2, stringTypeInfo, QORE_PARAM_NO_ARG, stringTypeInfo, QORE_PARAM_NO_ARG);

   QC_FTPCLIENT->addMethodExtended("getAsString",           (q_method_t)FC_getAsString, false, QC_NO_FLAGS, QDOM_DEFAULT, stringTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   QC_FTPCLIENT->addMethodExtended("getAsBinary",           (q_method_t)FC_getAsBinary, false, QC_NO_FLAGS, QDOM_DEFAULT, binaryTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   // accesses filesystem so tagged with QDOM_FILESYSTEM
   QC_FTPCLIENT->addMethodExtended("put",                   (q_method_t)FC_put_str, false, QC_NO_FLAGS, QDOM_FILESYSTEM, bigIntTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   QC_FTPCLIENT->addMethodExtended("put",                   (q_method_t)FC_put_str_str, false, QC_NO_FLAGS, QDOM_FILESYSTEM, bigIntTypeInfo, 2, stringTypeInfo, QORE_PARAM_NO_ARG, stringTypeInfo, QORE_PARAM_NO_ARG);

   QC_FTPCLIENT->addMethodExtended("del",                   (q_method_t)FC_del, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   QC_FTPCLIENT->addMethodExtended("setUserName",           (q_method_t)FC_setUserName, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   QC_FTPCLIENT->addMethodExtended("setPassword",           (q_method_t)FC_setPassword, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   QC_FTPCLIENT->addMethodExtended("setHostName",           (q_method_t)FC_setHostName, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   QC_FTPCLIENT->addMethodExtended("setPort",               (q_method_t)FC_setPort, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 1, softBigIntTypeInfo, QORE_PARAM_NO_ARG);

   QC_FTPCLIENT->addMethodExtended("setURL",                (q_method_t)FC_setURL, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   // returns string or NOTHING
   QC_FTPCLIENT->addMethodExtended("getUserName",           (q_method_t)FC_getUserName, false, QC_RET_VALUE_ONLY);

   // returns string or NOTHING
   QC_FTPCLIENT->addMethodExtended("getPassword",           (q_method_t)FC_getPassword, false, QC_RET_VALUE_ONLY);

   // returns string or NOTHING
   QC_FTPCLIENT->addMethodExtended("getHostName",           (q_method_t)FC_getHostName, false, QC_RET_VALUE_ONLY);

   QC_FTPCLIENT->addMethodExtended("getPort",               (q_method_t)FC_getPort, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, bigIntTypeInfo);

   QC_FTPCLIENT->addMethodExtended("getURL",                (q_method_t)FC_getURL, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, stringTypeInfo);

   QC_FTPCLIENT->addMethodExtended("setSecure",             (q_method_t)FC_setSecure, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 1, softBoolTypeInfo, &True);
   QC_FTPCLIENT->addMethodExtended("setInsecure",           (q_method_t)FC_setInsecure, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo); 
   QC_FTPCLIENT->addMethodExtended("setInsecureData",       (q_method_t)FC_setInsecureData, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo);

   QC_FTPCLIENT->addMethodExtended("isSecure",              (q_method_t)FC_isSecure, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, boolTypeInfo); 
   QC_FTPCLIENT->addMethodExtended("isDataSecure",          (q_method_t)FC_isDataSecure, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, boolTypeInfo);

   // returns string or NOTHING
   QC_FTPCLIENT->addMethodExtended("getSSLCipherName",      (q_method_t)FC_getSSLCipherName, false, QC_RET_VALUE_ONLY);

   // returns string or NOTHING
   QC_FTPCLIENT->addMethodExtended("getSSLCipherVersion",   (q_method_t)FC_getSSLCipherVersion, false, QC_RET_VALUE_ONLY);

   // returns string or NOTHING
   QC_FTPCLIENT->addMethodExtended("verifyPeerCertificate", (q_method_t)FC_verifyPeerCertificate, false, QC_RET_VALUE_ONLY);

   QC_FTPCLIENT->addMethodExtended("setModeAuto",           (q_method_t)FC_setModeAuto, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo);
   QC_FTPCLIENT->addMethodExtended("setModeEPSV",           (q_method_t)FC_setModeEPSV, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo);
   QC_FTPCLIENT->addMethodExtended("setModePASV",           (q_method_t)FC_setModePASV, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo);
   QC_FTPCLIENT->addMethodExtended("setModePORT",           (q_method_t)FC_setModePORT, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo);

   QC_FTPCLIENT->addMethodExtended("setEventQueue",         (q_method_t)FC_setEventQueue_nothing, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo);
   QC_FTPCLIENT->addMethodExtended("setEventQueue",         (q_method_t)FC_setEventQueue_queue, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 1, QC_QUEUE->getTypeInfo(), QORE_PARAM_NO_ARG);

   QC_FTPCLIENT->addMethodExtended("setDataEventQueue",     (q_method_t)FC_setDataEventQueue_nothing, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo);
   QC_FTPCLIENT->addMethodExtended("setDataEventQueue",     (q_method_t)FC_setDataEventQueue_queue, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 1, QC_QUEUE->getTypeInfo(), QORE_PARAM_NO_ARG);

   QC_FTPCLIENT->addMethodExtended("setControlEventQueue",  (q_method_t)FC_setControlEventQueue_nothing, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo);
   QC_FTPCLIENT->addMethodExtended("setControlEventQueue",  (q_method_t)FC_setControlEventQueue_queue, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 1, QC_QUEUE->getTypeInfo(), QORE_PARAM_NO_ARG);

   return QC_FTPCLIENT;
}
