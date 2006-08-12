/*
  QC_SSLCertificate.cc

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
#include <qore/QC_SSLCertificate.h>
#include <qore/QoreClass.h>
#include <qore/params.h>
#include <qore/Exception.h>
#include <qore/support.h>
#include <qore/Object.h>

#include <openssl/ssl.h>
#include <openssl/pem.h>

int CID_SSLCERTIFICATE;

static inline void *getSSLCertificate(void *obj)
{
   ((QoreSSLCertificate *)obj)->ROreference();
   return obj;
}

/*
void createSSLCertificateObject(class Object *self, X509 *cert)
{
   self->setPrivate(CID_SSLCERTIFICATE, new QoreSSLCertificate(cert), getSSLCertificate);
}
*/

// syntax: SSLCertificate(filename)
static QoreNode *SSLCERT_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p0 = get_param(params, 0);
   if (!p0 || p0->type != NT_STRING)
   {
      xsink->raiseException("SSLCERTIFICATE-CONSTRUCTOR-ERROR", "expecting file name as argument");
      return NULL;
   }
   
   QoreSSLCertificate *qc = new QoreSSLCertificate(p0->val.String->getBuffer(), xsink);
   if (xsink->isEvent())
      qc->deref();
   else
      self->setPrivate(CID_SSLCERTIFICATE, qc, getSSLCertificate);

   return NULL;
}

static QoreNode *SSLCERT_destructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreSSLCertificate *s = (QoreSSLCertificate *)self->getAndClearPrivateData(CID_SSLCERTIFICATE);
   if (s)
      s->deref();
   return NULL;
}

static QoreNode *SSLCERT_copy(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   xsink->raiseException("SSLCERTIFICATE-COPY-ERROR", "SSLCertificate objects cannot be copied");
   return NULL;
}

static QoreNode *SSLCERT_getPEM(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreSSLCertificate *s = (QoreSSLCertificate *)self->getReferencedPrivateData(CID_SSLCERTIFICATE);
   class QoreNode *rv = NULL;
   if (s)
   {
      class QoreString *ps = s->getPEM(xsink);
      if (ps)
	 rv = new QoreNode(ps);
      s->deref();
   }
   else
      alreadyDeleted(xsink, "SSLCertificate::getPEM");

   return rv;
}

static QoreNode *SSLCERT_getInfo(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreSSLCertificate *s = (QoreSSLCertificate *)self->getReferencedPrivateData(CID_SSLCERTIFICATE);
   class QoreNode *rv;
   if (s)
   {
      rv = new QoreNode(s->getInfo());
      s->deref();
   }
   else
   {
      alreadyDeleted(xsink, "SSLCertificate::getInfo");
      rv = NULL;
   }
   return rv;
}

static QoreNode *SSLCERT_getSignature(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreSSLCertificate *s = (QoreSSLCertificate *)self->getReferencedPrivateData(CID_SSLCERTIFICATE);
   class QoreNode *rv;
   if (s)
   {
      rv = new QoreNode(s->getSignature());
      s->deref();
   }
   else
   {
      alreadyDeleted(xsink, "SSLCertificate::getSignature");
      rv = NULL;
   }
   return rv;
}

static QoreNode *SSLCERT_getSignatureType(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreSSLCertificate *s = (QoreSSLCertificate *)self->getReferencedPrivateData(CID_SSLCERTIFICATE);
   class QoreNode *rv;
   if (s)
   {
      rv = new QoreNode(s->getSignatureType());
      s->deref();
   }
   else
   {
      alreadyDeleted(xsink, "SSLCertificate::getSignatureType");
      rv = NULL;
   }
   return rv;
}

static QoreNode *SSLCERT_getPublicKey(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreSSLCertificate *s = (QoreSSLCertificate *)self->getReferencedPrivateData(CID_SSLCERTIFICATE);
   class QoreNode *rv;
   if (s)
   {
      rv = new QoreNode(s->getPublicKey());
      s->deref();
   }
   else
   {
      alreadyDeleted(xsink, "SSLCertificate::getPublicKey");
      rv = NULL;
   }
   return rv;
}

static QoreNode *SSLCERT_getPublicKeyAlgorithm(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreSSLCertificate *s = (QoreSSLCertificate *)self->getReferencedPrivateData(CID_SSLCERTIFICATE);
   class QoreNode *rv;
   if (s)
   {
      rv = new QoreNode(s->getPublicKeyAlgorithm());
      s->deref();
   }
   else
   {
      alreadyDeleted(xsink, "SSLCertificate::getPublicKeyAlgorithm");
      rv = NULL;
   }
   return rv;
}

static QoreNode *SSLCERT_getSubjectHash(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreSSLCertificate *s = (QoreSSLCertificate *)self->getReferencedPrivateData(CID_SSLCERTIFICATE);
   class QoreNode *rv;
   if (s)
   {
      rv = new QoreNode(s->getSubjectHash());
      s->deref();
   }
   else
   {
      alreadyDeleted(xsink, "SSLCertificate::getSubjectHash");
      rv = NULL;
   }
   return rv;
}

static QoreNode *SSLCERT_getIssuerHash(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreSSLCertificate *s = (QoreSSLCertificate *)self->getReferencedPrivateData(CID_SSLCERTIFICATE);
   class QoreNode *rv;
   if (s)
   {
      rv = new QoreNode(s->getIssuerHash());
      s->deref();
   }
   else
   {
      alreadyDeleted(xsink, "SSLCertificate::getIssuerHash");
      rv = NULL;
   }
   return rv;
}

static QoreNode *SSLCERT_getSerialNumber(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreSSLCertificate *s = (QoreSSLCertificate *)self->getReferencedPrivateData(CID_SSLCERTIFICATE);
   class QoreNode *rv;
   if (s)
   {
      rv = new QoreNode(s->getSerialNumber());
      s->deref();
   }
   else
   {
      alreadyDeleted(xsink, "SSLCertificate::getSerialNumber");
      rv = NULL;
   }
   return rv;
}

static QoreNode *SSLCERT_getVersion(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreSSLCertificate *s = (QoreSSLCertificate *)self->getReferencedPrivateData(CID_SSLCERTIFICATE);
   class QoreNode *rv;
   if (s)
   {
      rv = new QoreNode(s->getVersion());
      s->deref();
   }
   else
   {
      alreadyDeleted(xsink, "SSLCertificate::getVersion");
      rv = NULL;
   }
   return rv;
}

static QoreNode *SSLCERT_getPurposeHash(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreSSLCertificate *s = (QoreSSLCertificate *)self->getReferencedPrivateData(CID_SSLCERTIFICATE);
   class QoreNode *rv;
   if (s)
   {
      rv = new QoreNode(s->getPurposeHash());
      s->deref();
   }
   else
   {
      alreadyDeleted(xsink, "SSLCertificate::getPurposeHash");
      rv = NULL;
   }
   return rv;
}

static QoreNode *SSLCERT_getNotBeforeDate(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreSSLCertificate *s = (QoreSSLCertificate *)self->getReferencedPrivateData(CID_SSLCERTIFICATE);
   class QoreNode *rv;
   if (s)
   {
      rv = new QoreNode(s->getNotBeforeDate());
      s->deref();
   }
   else
   {
      alreadyDeleted(xsink, "SSLCertificate::getNotBeforeDate");
      rv = NULL;
   }
   return rv;
}

static QoreNode *SSLCERT_getNotAfterDate(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreSSLCertificate *s = (QoreSSLCertificate *)self->getReferencedPrivateData(CID_SSLCERTIFICATE);
   class QoreNode *rv;
   if (s)
   {
      rv = new QoreNode(s->getNotAfterDate());
      s->deref();
   }
   else
   {
      alreadyDeleted(xsink, "SSLCertificate::getNotAfterDate");
      rv = NULL;
   }
   return rv;
}

class QoreClass *initSSLCertificateClass()
{
   tracein("initSSLCertificateClass()");

   class QoreClass *QC_SSLCERTIFICATE = new QoreClass(strdup("SSLCertificate"));
   CID_SSLCERTIFICATE = QC_SSLCERTIFICATE->getID();
   QC_SSLCERTIFICATE->addMethod("constructor",           SSLCERT_constructor);
   QC_SSLCERTIFICATE->addMethod("destructor",            SSLCERT_destructor);
   QC_SSLCERTIFICATE->addMethod("copy",                  SSLCERT_copy);
   QC_SSLCERTIFICATE->addMethod("getPEM",                SSLCERT_getPEM);
   QC_SSLCERTIFICATE->addMethod("getInfo",               SSLCERT_getInfo);
   QC_SSLCERTIFICATE->addMethod("getSignatureType",      SSLCERT_getSignatureType);
   QC_SSLCERTIFICATE->addMethod("getSignature",          SSLCERT_getSignature);
   QC_SSLCERTIFICATE->addMethod("getPublicKeyAlgorithm", SSLCERT_getPublicKeyAlgorithm);
   QC_SSLCERTIFICATE->addMethod("getPublicKey",          SSLCERT_getPublicKey);
   QC_SSLCERTIFICATE->addMethod("getSubjectHash",        SSLCERT_getSubjectHash);
   QC_SSLCERTIFICATE->addMethod("getIssuerHash",         SSLCERT_getIssuerHash);
   QC_SSLCERTIFICATE->addMethod("getSerialNumber",       SSLCERT_getSerialNumber);
   QC_SSLCERTIFICATE->addMethod("getVersion",            SSLCERT_getVersion);
   QC_SSLCERTIFICATE->addMethod("getPurposeHash",        SSLCERT_getPurposeHash);
   QC_SSLCERTIFICATE->addMethod("getNotBeforeDate",      SSLCERT_getNotBeforeDate);
   QC_SSLCERTIFICATE->addMethod("getNotAfterDate",       SSLCERT_getNotAfterDate);

   traceout("initSSLCertificateClass()");
   return QC_SSLCERTIFICATE;
}
