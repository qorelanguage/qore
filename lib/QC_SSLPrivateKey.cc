/*
  QC_SSLPrivateKey.cc

  Qore Programming Language

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
#include <qore/common.h>
#include <qore/QC_SSLPrivateKey.h>
#include <qore/QoreClass.h>
#include <qore/params.h>
#include <qore/Exception.h>
#include <qore/support.h>
#include <qore/Object.h>

#include <openssl/ssl.h>
#include <openssl/pem.h>

int CID_SSLPRIVATEKEY;

static inline void *getSSLPrivateKey(void *obj)
{
   ((QoreSSLPrivateKey *)obj)->ROreference();
   return obj;
}

/*
void createSSLPrivateKeyObject(class Object *self, EVP_PKEY *cert)
{
   self->setPrivate(CID_SSLPRIVATEKEY, new QoreSSLPrivateKey(cert), getSSLPrivateKey);
}
*/

// syntax: SSLPrivateKey(filename)
static QoreNode *SSLPKEY_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p0 = test_param(params, NT_STRING, 0);
   if (!p0)
   {
      xsink->raiseException("SSLPRIVATEKEY-CONSTRUCTOR-ERROR", "expecting file name as argument");
      return NULL;
   }
   // get pass phrase if any
   class QoreNode *p1 = test_param(params, NT_STRING, 1);
   char *pp = p1 ? p1->val.String->getBuffer() : NULL;   

   QoreSSLPrivateKey *qpk = new QoreSSLPrivateKey(p0->val.String->getBuffer(), pp, xsink);
   if (xsink->isEvent())
      qpk->deref();
   else
      self->setPrivate(CID_SSLPRIVATEKEY, qpk, getSSLPrivateKey);

   return NULL;
}

static QoreNode *SSLPKEY_destructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreSSLPrivateKey *s = (QoreSSLPrivateKey *)self->getAndClearPrivateData(CID_SSLPRIVATEKEY);
   if (s)
      s->deref();
   return NULL;
}

static QoreNode *SSLPKEY_copy(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   xsink->raiseException("SSLPRIVATEKEY-COPY-ERROR", "SSLPrivateKey objects cannot be copied");
   return NULL;
}

static QoreNode *SSLPKEY_getInfo(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreSSLPrivateKey *pk = (QoreSSLPrivateKey *)self->getReferencedPrivateData(CID_SSLPRIVATEKEY);
   class QoreNode *rv = NULL;

   if (pk)
   {
      rv = new QoreNode(pk->getInfo());
      pk->deref();
   }
   else
      alreadyDeleted(xsink, "SSLPrivateKey::getInfo");

   return rv;
}

static QoreNode *SSLPKEY_getType(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreSSLPrivateKey *pk = (QoreSSLPrivateKey *)self->getReferencedPrivateData(CID_SSLPRIVATEKEY);
   class QoreNode *rv = NULL;

   if (pk)
   {
      rv = new QoreNode(pk->getType());
      pk->deref();
   }
   else
      alreadyDeleted(xsink, "SSLPrivateKey::getType");

   return rv;
}

static QoreNode *SSLPKEY_getVersion(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreSSLPrivateKey *pk = (QoreSSLPrivateKey *)self->getReferencedPrivateData(CID_SSLPRIVATEKEY);
   class QoreNode *rv = NULL;

   if (pk)
   {
      rv = new QoreNode(pk->getVersion());
      pk->deref();
   }
   else
      alreadyDeleted(xsink, "SSLPrivateKey::getVersion");

   return rv;
}

static QoreNode *SSLPKEY_getBitLength(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreSSLPrivateKey *pk = (QoreSSLPrivateKey *)self->getReferencedPrivateData(CID_SSLPRIVATEKEY);
   class QoreNode *rv = NULL;

   if (pk)
   {
      rv = new QoreNode(pk->getBitLength());
      pk->deref();
   }
   else
      alreadyDeleted(xsink, "SSLPrivateKey::getBitLength");

   return rv;
}

class QoreClass *initSSLPrivateKeyClass()
{
   tracein("initSSLPrivateKeyClass()");

   class QoreClass *QC_SSLPRIVATEKEY = new QoreClass(strdup("SSLPrivateKey"));
   CID_SSLPRIVATEKEY = QC_SSLPRIVATEKEY->getID();
   QC_SSLPRIVATEKEY->addMethod("constructor",      SSLPKEY_constructor);
   QC_SSLPRIVATEKEY->addMethod("destructor",       SSLPKEY_destructor);
   QC_SSLPRIVATEKEY->addMethod("copy",             SSLPKEY_copy);
   QC_SSLPRIVATEKEY->addMethod("getType",          SSLPKEY_getType);
   QC_SSLPRIVATEKEY->addMethod("getVersion",       SSLPKEY_getVersion);
   QC_SSLPRIVATEKEY->addMethod("getBitLength",     SSLPKEY_getBitLength);
   QC_SSLPRIVATEKEY->addMethod("getInfo",          SSLPKEY_getInfo);

   traceout("initSSLPrivateKeyClass()");
   return QC_SSLPRIVATEKEY;
}
