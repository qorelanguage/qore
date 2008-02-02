/*
  QC_SSLCertificate.cc

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
#include <qore/intern/QC_SSLCertificate.h>

DLLEXPORT int CID_SSLCERTIFICATE;

/*
void createSSLCertificateObject(QoreObject *self, X509 *cert)
{
   self->setPrivate(CID_SSLCERTIFICATE, new QoreSSLCertificate(cert));
}
*/

// syntax: SSLCertificate(filename)
static void SSLCERT_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *p0 = test_string_param(params, 0);
   if (!p0)
   {
      xsink->raiseException("SSLCERTIFICATE-CONSTRUCTOR-ERROR", "expecting file name as argument");
      return;
   }
   
   QoreSSLCertificate *qc = new QoreSSLCertificate(p0->getBuffer(), xsink);
   if (xsink->isEvent())
      qc->deref();
   else
      self->setPrivate(CID_SSLCERTIFICATE, qc);
}

static void SSLCERT_copy(QoreObject *self, QoreObject *old, class QoreSSLCertificate *s, ExceptionSink *xsink)
{
   xsink->raiseException("SSLCERTIFICATE-COPY-ERROR", "SSLCertificate objects cannot be copied");
}

static AbstractQoreNode *SSLCERT_getPEM(QoreObject *self, class QoreSSLCertificate *s, const QoreListNode *params, ExceptionSink *xsink)
{
   return s->getPEM(xsink);
}

static AbstractQoreNode *SSLCERT_getInfo(QoreObject *self, class QoreSSLCertificate *s, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(s->getInfo());
}

static AbstractQoreNode *SSLCERT_getSignature(QoreObject *self, class QoreSSLCertificate *s, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(s->getSignature());
}

static AbstractQoreNode *SSLCERT_getSignatureType(QoreObject *self, class QoreSSLCertificate *s, const QoreListNode *params, ExceptionSink *xsink)
{
   return s->getSignatureType();
}

static AbstractQoreNode *SSLCERT_getPublicKey(QoreObject *self, class QoreSSLCertificate *s, const QoreListNode *params, ExceptionSink *xsink)
{
   return s->getPublicKey();
}

static AbstractQoreNode *SSLCERT_getPublicKeyAlgorithm(QoreObject *self, class QoreSSLCertificate *s, const QoreListNode *params, ExceptionSink *xsink)
{
   return s->getPublicKeyAlgorithm();
}

static AbstractQoreNode *SSLCERT_getSubjectHash(QoreObject *self, class QoreSSLCertificate *s, const QoreListNode *params, ExceptionSink *xsink)
{
   return s->getSubjectHash();
}

static AbstractQoreNode *SSLCERT_getIssuerHash(QoreObject *self, class QoreSSLCertificate *s, const QoreListNode *params, ExceptionSink *xsink)
{
   return s->getIssuerHash();
}

static AbstractQoreNode *SSLCERT_getSerialNumber(QoreObject *self, class QoreSSLCertificate *s, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(s->getSerialNumber());
}

static AbstractQoreNode *SSLCERT_getVersion(QoreObject *self, class QoreSSLCertificate *s, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(s->getVersion());
}

static AbstractQoreNode *SSLCERT_getPurposeHash(QoreObject *self, class QoreSSLCertificate *s, const QoreListNode *params, ExceptionSink *xsink)
{
   return s->getPurposeHash();
}

static AbstractQoreNode *SSLCERT_getNotBeforeDate(QoreObject *self, class QoreSSLCertificate *s, const QoreListNode *params, ExceptionSink *xsink)
{
   return s->getNotBeforeDate();
}

static AbstractQoreNode *SSLCERT_getNotAfterDate(QoreObject *self, class QoreSSLCertificate *s, const QoreListNode *params, ExceptionSink *xsink)
{
   return s->getNotAfterDate();
}

class QoreClass *initSSLCertificateClass()
{
   tracein("initSSLCertificateClass()");

   class QoreClass *QC_SSLCERTIFICATE = new QoreClass("SSLCertificate");
   CID_SSLCERTIFICATE = QC_SSLCERTIFICATE->getID();
   QC_SSLCERTIFICATE->setConstructor(SSLCERT_constructor);
   QC_SSLCERTIFICATE->setCopy((q_copy_t)SSLCERT_copy);
   QC_SSLCERTIFICATE->addMethod("getPEM",                (q_method_t)SSLCERT_getPEM);
   QC_SSLCERTIFICATE->addMethod("getInfo",               (q_method_t)SSLCERT_getInfo);
   QC_SSLCERTIFICATE->addMethod("getSignatureType",      (q_method_t)SSLCERT_getSignatureType);
   QC_SSLCERTIFICATE->addMethod("getSignature",          (q_method_t)SSLCERT_getSignature);
   QC_SSLCERTIFICATE->addMethod("getPublicKeyAlgorithm", (q_method_t)SSLCERT_getPublicKeyAlgorithm);
   QC_SSLCERTIFICATE->addMethod("getPublicKey",          (q_method_t)SSLCERT_getPublicKey);
   QC_SSLCERTIFICATE->addMethod("getSubjectHash",        (q_method_t)SSLCERT_getSubjectHash);
   QC_SSLCERTIFICATE->addMethod("getIssuerHash",         (q_method_t)SSLCERT_getIssuerHash);
   QC_SSLCERTIFICATE->addMethod("getSerialNumber",       (q_method_t)SSLCERT_getSerialNumber);
   QC_SSLCERTIFICATE->addMethod("getVersion",            (q_method_t)SSLCERT_getVersion);
   QC_SSLCERTIFICATE->addMethod("getPurposeHash",        (q_method_t)SSLCERT_getPurposeHash);
   QC_SSLCERTIFICATE->addMethod("getNotBeforeDate",      (q_method_t)SSLCERT_getNotBeforeDate);
   QC_SSLCERTIFICATE->addMethod("getNotAfterDate",       (q_method_t)SSLCERT_getNotAfterDate);

   traceout("initSSLCertificateClass()");
   return QC_SSLCERTIFICATE;
}
