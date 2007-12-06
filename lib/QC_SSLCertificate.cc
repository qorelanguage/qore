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
void createSSLCertificateObject(class QoreObject *self, X509 *cert)
{
   self->setPrivate(CID_SSLCERTIFICATE, new QoreSSLCertificate(cert));
}
*/

// syntax: SSLCertificate(filename)
static void SSLCERT_constructor(class QoreObject *self, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p0 = get_param(params, 0);
   if (!p0 || p0->type != NT_STRING)
   {
      xsink->raiseException("SSLCERTIFICATE-CONSTRUCTOR-ERROR", "expecting file name as argument");
      return;
   }
   
   QoreSSLCertificate *qc = new QoreSSLCertificate(p0->val.String->getBuffer(), xsink);
   if (xsink->isEvent())
      qc->deref();
   else
      self->setPrivate(CID_SSLCERTIFICATE, qc);
}

static void SSLCERT_copy(class QoreObject *self, class QoreObject *old, class QoreSSLCertificate *s, ExceptionSink *xsink)
{
   xsink->raiseException("SSLCERTIFICATE-COPY-ERROR", "SSLCertificate objects cannot be copied");
}

static QoreNode *SSLCERT_getPEM(class QoreObject *self, class QoreSSLCertificate *s, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreString *ps = s->getPEM(xsink);
   if (ps)
      return new QoreNode(ps);

   return NULL;
}

static QoreNode *SSLCERT_getInfo(class QoreObject *self, class QoreSSLCertificate *s, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(s->getInfo());
}

static QoreNode *SSLCERT_getSignature(class QoreObject *self, class QoreSSLCertificate *s, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(s->getSignature());
}

static QoreNode *SSLCERT_getSignatureType(class QoreObject *self, class QoreSSLCertificate *s, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(s->getSignatureType());
}

static QoreNode *SSLCERT_getPublicKey(class QoreObject *self, class QoreSSLCertificate *s, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(s->getPublicKey());
}

static QoreNode *SSLCERT_getPublicKeyAlgorithm(class QoreObject *self, class QoreSSLCertificate *s, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(s->getPublicKeyAlgorithm());
}

static QoreNode *SSLCERT_getSubjectHash(class QoreObject *self, class QoreSSLCertificate *s, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(s->getSubjectHash());
}

static QoreNode *SSLCERT_getIssuerHash(class QoreObject *self, class QoreSSLCertificate *s, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(s->getIssuerHash());
}

static QoreNode *SSLCERT_getSerialNumber(class QoreObject *self, class QoreSSLCertificate *s, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(s->getSerialNumber());
}

static QoreNode *SSLCERT_getVersion(class QoreObject *self, class QoreSSLCertificate *s, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(s->getVersion());
}

static QoreNode *SSLCERT_getPurposeHash(class QoreObject *self, class QoreSSLCertificate *s, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(s->getPurposeHash());
}

static QoreNode *SSLCERT_getNotBeforeDate(class QoreObject *self, class QoreSSLCertificate *s, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(s->getNotBeforeDate());
}

static QoreNode *SSLCERT_getNotAfterDate(class QoreObject *self, class QoreSSLCertificate *s, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(s->getNotAfterDate());
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
