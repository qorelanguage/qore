/*
  QC_SSLPrivateKey.cc

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
#include <qore/intern/QC_SSLPrivateKey.h>

#include <openssl/ssl.h>
#include <openssl/pem.h>

int CID_SSLPRIVATEKEY;

/*
void createSSLPrivateKeyObject(class QoreObject *self, EVP_PKEY *cert)
{
   self->setPrivate(CID_SSLPRIVATEKEY, new QoreSSLPrivateKey(cert));
}
*/

// syntax: SSLPrivateKey(filename)
static void SSLPKEY_constructor(class QoreObject *self, const QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p0 = test_param(params, NT_STRING, 0);
   if (!p0)
   {
      xsink->raiseException("SSLPRIVATEKEY-CONSTRUCTOR-ERROR", "expecting file name as argument");
      return;
   }
   // get pass phrase if any
   class QoreNode *p1 = test_param(params, NT_STRING, 1);
   const char *pp = p1 ? p1->val.String->getBuffer() : NULL;   

   QoreSSLPrivateKey *qpk = new QoreSSLPrivateKey(p0->val.String->getBuffer(), (char *)pp, xsink);
   if (xsink->isEvent())
      qpk->deref();
   else
      self->setPrivate(CID_SSLPRIVATEKEY, qpk);
}

static void SSLPKEY_copy(class QoreObject *self, class QoreObject *old, class QoreSSLPrivateKey *pk, ExceptionSink *xsink)
{
   xsink->raiseException("SSLPRIVATEKEY-COPY-ERROR", "SSLPrivateKey objects cannot be copied");
}

static QoreNode *SSLPKEY_getInfo(class QoreObject *self, class QoreSSLPrivateKey *pk, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(pk->getInfo());
}

static QoreNode *SSLPKEY_getType(class QoreObject *self, class QoreSSLPrivateKey *pk, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(pk->getType());
}

static QoreNode *SSLPKEY_getVersion(class QoreObject *self, class QoreSSLPrivateKey *pk, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(pk->getVersion());
}

static QoreNode *SSLPKEY_getBitLength(class QoreObject *self, class QoreSSLPrivateKey *pk, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(pk->getBitLength());
}

class QoreClass *initSSLPrivateKeyClass()
{
   tracein("initSSLPrivateKeyClass()");

   class QoreClass *QC_SSLPRIVATEKEY = new QoreClass("SSLPrivateKey");
   CID_SSLPRIVATEKEY = QC_SSLPRIVATEKEY->getID();
   QC_SSLPRIVATEKEY->setConstructor(SSLPKEY_constructor);
   QC_SSLPRIVATEKEY->setCopy((q_copy_t)SSLPKEY_copy);
   QC_SSLPRIVATEKEY->addMethod("getType",          (q_method_t)SSLPKEY_getType);
   QC_SSLPRIVATEKEY->addMethod("getVersion",       (q_method_t)SSLPKEY_getVersion);
   QC_SSLPRIVATEKEY->addMethod("getBitLength",     (q_method_t)SSLPKEY_getBitLength);
   QC_SSLPRIVATEKEY->addMethod("getInfo",          (q_method_t)SSLPKEY_getInfo);

   traceout("initSSLPrivateKeyClass()");
   return QC_SSLPRIVATEKEY;
}
