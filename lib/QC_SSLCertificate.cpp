/*
  QC_SSLCertificate.cpp

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
#include <qore/intern/QC_SSLCertificate.h>

qore_classid_t CID_SSLCERTIFICATE;

// syntax: SSLCertificate(filename|binary)
static void SSLCERT_constructor_str(QoreObject *self, const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_PARAM(str, const QoreStringNode, args, 0);

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
}

// syntax: SSLCertificate(filename|binary)
static void SSLCERT_constructor_bin(QoreObject *self, const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_PARAM(bin, const BinaryNode, args, 0);

   SimpleRefHolder<QoreSSLCertificate> qc(new QoreSSLCertificate(bin, xsink));
   if (!*xsink)
      self->setPrivate(CID_SSLCERTIFICATE, qc.release());	    
}

static void SSLCERT_copy(QoreObject *self, QoreObject *old, QoreSSLCertificate *s, ExceptionSink *xsink) {
   xsink->raiseException("SSLCERTIFICATE-COPY-ERROR", "SSLCertificate objects cannot be copied");
}

static QoreStringNode *SSLCERT_getPEM(QoreObject *self, QoreSSLCertificate *s, const QoreListNode *args, ExceptionSink *xsink) {
   return s->getPEM(xsink);
}

static QoreHashNode *SSLCERT_getInfo(QoreObject *self, QoreSSLCertificate *s, const QoreListNode *args, ExceptionSink *xsink) {
   return s->getInfo();
}

static BinaryNode *SSLCERT_getSignature(QoreObject *self, QoreSSLCertificate *s, const QoreListNode *args, ExceptionSink *xsink) {
   return s->getSignature();
}

static QoreStringNode *SSLCERT_getSignatureType(QoreObject *self, QoreSSLCertificate *s, const QoreListNode *args, ExceptionSink *xsink) {
   return s->getSignatureType();
}

static BinaryNode *SSLCERT_getPublicKey(QoreObject *self, QoreSSLCertificate *s, const QoreListNode *args, ExceptionSink *xsink) {
   return s->getPublicKey();
}

static QoreStringNode *SSLCERT_getPublicKeyAlgorithm(QoreObject *self, QoreSSLCertificate *s, const QoreListNode *args, ExceptionSink *xsink) {
   return s->getPublicKeyAlgorithm();
}

static QoreHashNode *SSLCERT_getSubjectHash(QoreObject *self, QoreSSLCertificate *s, const QoreListNode *args, ExceptionSink *xsink) {
   return s->getSubjectHash();
}

static QoreHashNode *SSLCERT_getIssuerHash(QoreObject *self, QoreSSLCertificate *s, const QoreListNode *args, ExceptionSink *xsink) {
   return s->getIssuerHash();
}

static QoreBigIntNode *SSLCERT_getSerialNumber(QoreObject *self, QoreSSLCertificate *s, const QoreListNode *args, ExceptionSink *xsink) {
   return new QoreBigIntNode(s->getSerialNumber());
}

static QoreBigIntNode *SSLCERT_getVersion(QoreObject *self, QoreSSLCertificate *s, const QoreListNode *args, ExceptionSink *xsink) {
   return new QoreBigIntNode(s->getVersion());
}

static QoreHashNode *SSLCERT_getPurposeHash(QoreObject *self, QoreSSLCertificate *s, const QoreListNode *args, ExceptionSink *xsink) {
   return s->getPurposeHash();
}

static DateTimeNode *SSLCERT_getNotBeforeDate(QoreObject *self, QoreSSLCertificate *s, const QoreListNode *args, ExceptionSink *xsink) {
   return s->getNotBeforeDate();
}

static DateTimeNode *SSLCERT_getNotAfterDate(QoreObject *self, QoreSSLCertificate *s, const QoreListNode *args, ExceptionSink *xsink) {
   return s->getNotAfterDate();
}

QoreClass *initSSLCertificateClass() {
   QORE_TRACE("initSSLCertificateClass()");

   QoreClass *QC_SSLCERTIFICATE = new QoreClass("SSLCertificate");
   CID_SSLCERTIFICATE = QC_SSLCERTIFICATE->getID();

   // overloaded constructor
   QC_SSLCERTIFICATE->setConstructorExtended(SSLCERT_constructor_str, false, QC_NO_FLAGS, QDOM_DEFAULT, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   QC_SSLCERTIFICATE->setConstructorExtended(SSLCERT_constructor_bin, false, QC_NO_FLAGS, QDOM_DEFAULT, 1, binaryTypeInfo, QORE_PARAM_NO_ARG);
   QC_SSLCERTIFICATE->setCopy((q_copy_t)SSLCERT_copy);

   QC_SSLCERTIFICATE->addMethodExtended("getPEM",                (q_method_t)SSLCERT_getPEM, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, stringTypeInfo);
   QC_SSLCERTIFICATE->addMethodExtended("getInfo",               (q_method_t)SSLCERT_getInfo, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, hashTypeInfo);
   QC_SSLCERTIFICATE->addMethodExtended("getSignatureType",      (q_method_t)SSLCERT_getSignatureType, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, stringTypeInfo);
   QC_SSLCERTIFICATE->addMethodExtended("getSignature",          (q_method_t)SSLCERT_getSignature, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, binaryTypeInfo);
   QC_SSLCERTIFICATE->addMethodExtended("getPublicKeyAlgorithm", (q_method_t)SSLCERT_getPublicKeyAlgorithm, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, stringTypeInfo);
   QC_SSLCERTIFICATE->addMethodExtended("getPublicKey",          (q_method_t)SSLCERT_getPublicKey, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, binaryTypeInfo);
   QC_SSLCERTIFICATE->addMethodExtended("getSubjectHash",        (q_method_t)SSLCERT_getSubjectHash, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, hashTypeInfo);
   QC_SSLCERTIFICATE->addMethodExtended("getIssuerHash",         (q_method_t)SSLCERT_getIssuerHash, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, hashTypeInfo);
   QC_SSLCERTIFICATE->addMethodExtended("getSerialNumber",       (q_method_t)SSLCERT_getSerialNumber, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, bigIntTypeInfo);
   QC_SSLCERTIFICATE->addMethodExtended("getVersion",            (q_method_t)SSLCERT_getVersion, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, bigIntTypeInfo);
   QC_SSLCERTIFICATE->addMethodExtended("getPurposeHash",        (q_method_t)SSLCERT_getPurposeHash, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, hashTypeInfo);
   QC_SSLCERTIFICATE->addMethodExtended("getNotBeforeDate",      (q_method_t)SSLCERT_getNotBeforeDate, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, dateTypeInfo);
   QC_SSLCERTIFICATE->addMethodExtended("getNotAfterDate",       (q_method_t)SSLCERT_getNotAfterDate, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, dateTypeInfo);

   return QC_SSLCERTIFICATE;
}
