/*
  QC_SSLCertificate.cc

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
#include <qore/intern/QC_SSLCertificate.h>

DLLEXPORT qore_classid_t CID_SSLCERTIFICATE;

/*
void createSSLCertificateObject(QoreObject *self, X509 *cert)
{
   self->setPrivate(CID_SSLCERTIFICATE, new QoreSSLCertificate(cert));
}
*/

// syntax: SSLCertificate(filename|binary)
static void SSLCERT_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0 = get_param(params, 0);
   if (p0) {
      qore_type_t t = p0->getType();
      if (t == NT_STRING) {
	 const QoreStringNode *str = reinterpret_cast<const QoreStringNode *>(p0);
	 // FIXME: this is not a good API!
	 // if strlen < 200 then we assume it's a filename to remain backward compatible with the old API
	 if (str->strlen() < 200) {
	    // FIXME: this class should not read file - we have to check the parse option PO_NO_FILESYSTEM at runtime
	    if (getProgram()->getParseOptions() & PO_NO_FILESYSTEM) {
	       xsink->raiseException("INVALID-FILESYSTEM-ACCESS", "passing a filename to SSLCertificate::constructor() violates parse option NO-FILESYSTEM");
	       return;
	    }

	    SimpleRefHolder<QoreSSLCertificate> qc(new QoreSSLCertificate(str->getBuffer(), xsink));
	    if (!*xsink)
	       self->setPrivate(CID_SSLCERTIFICATE, qc.release());
	    return;
	 }

	 // create assuming a certificate in PEM format
	 SimpleRefHolder<QoreSSLCertificate> qc(new QoreSSLCertificate(str, xsink));
	 if (!*xsink)
	    self->setPrivate(CID_SSLCERTIFICATE, qc.release());
	 return;
      }

      if (t == NT_BINARY) {
	 SimpleRefHolder<QoreSSLCertificate> qc(new QoreSSLCertificate(reinterpret_cast<const BinaryNode *>(p0), xsink));
	 if (!*xsink)
	    self->setPrivate(CID_SSLCERTIFICATE, qc.release());	    
	 return;
      }
   }

   xsink->raiseException("SSLCERTIFICATE-CONSTRUCTOR-ERROR", "expecting file name or binary object in DER format as sole argument to SLLCertificate::constructor");
}

static void SSLCERT_copy(QoreObject *self, QoreObject *old, QoreSSLCertificate *s, ExceptionSink *xsink)
{
   xsink->raiseException("SSLCERTIFICATE-COPY-ERROR", "SSLCertificate objects cannot be copied");
}

static AbstractQoreNode *SSLCERT_getPEM(QoreObject *self, QoreSSLCertificate *s, const QoreListNode *params, ExceptionSink *xsink)
{
   return s->getPEM(xsink);
}

static AbstractQoreNode *SSLCERT_getInfo(QoreObject *self, QoreSSLCertificate *s, const QoreListNode *params, ExceptionSink *xsink) {
   return s->getInfo();
}

static AbstractQoreNode *SSLCERT_getSignature(QoreObject *self, QoreSSLCertificate *s, const QoreListNode *params, ExceptionSink *xsink) {
   return s->getSignature();
}

static AbstractQoreNode *SSLCERT_getSignatureType(QoreObject *self, QoreSSLCertificate *s, const QoreListNode *params, ExceptionSink *xsink)
{
   return s->getSignatureType();
}

static AbstractQoreNode *SSLCERT_getPublicKey(QoreObject *self, QoreSSLCertificate *s, const QoreListNode *params, ExceptionSink *xsink)
{
   return s->getPublicKey();
}

static AbstractQoreNode *SSLCERT_getPublicKeyAlgorithm(QoreObject *self, QoreSSLCertificate *s, const QoreListNode *params, ExceptionSink *xsink)
{
   return s->getPublicKeyAlgorithm();
}

static AbstractQoreNode *SSLCERT_getSubjectHash(QoreObject *self, QoreSSLCertificate *s, const QoreListNode *params, ExceptionSink *xsink)
{
   return s->getSubjectHash();
}

static AbstractQoreNode *SSLCERT_getIssuerHash(QoreObject *self, QoreSSLCertificate *s, const QoreListNode *params, ExceptionSink *xsink)
{
   return s->getIssuerHash();
}

static AbstractQoreNode *SSLCERT_getSerialNumber(QoreObject *self, QoreSSLCertificate *s, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(s->getSerialNumber());
}

static AbstractQoreNode *SSLCERT_getVersion(QoreObject *self, QoreSSLCertificate *s, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(s->getVersion());
}

static AbstractQoreNode *SSLCERT_getPurposeHash(QoreObject *self, QoreSSLCertificate *s, const QoreListNode *params, ExceptionSink *xsink)
{
   return s->getPurposeHash();
}

static AbstractQoreNode *SSLCERT_getNotBeforeDate(QoreObject *self, QoreSSLCertificate *s, const QoreListNode *params, ExceptionSink *xsink)
{
   return s->getNotBeforeDate();
}

static AbstractQoreNode *SSLCERT_getNotAfterDate(QoreObject *self, QoreSSLCertificate *s, const QoreListNode *params, ExceptionSink *xsink)
{
   return s->getNotAfterDate();
}

QoreClass *initSSLCertificateClass()
{
   QORE_TRACE("initSSLCertificateClass()");

   QoreClass *QC_SSLCERTIFICATE = new QoreClass("SSLCertificate");
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

   return QC_SSLCERTIFICATE;
}
