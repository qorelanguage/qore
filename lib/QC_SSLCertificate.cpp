/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QC_SSLCertificate.cpp

  Qore Programming Language

  Copyright 2003 - 2011 David Nichols

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

/* Qore class Qore::SSLCertificate */

qore_classid_t CID_SSLCERTIFICATE;
QoreClass *QC_SSLCERTIFICATE;

// SSLCertificate::constructor(string pem) {}
static void SSLCertificate_constructor(QoreObject* self, const QoreListNode* args, ExceptionSink* xsink) {
   const QoreStringNode* pem = HARD_QORE_STRING(args, 0);
   // create assuming a certificate in PEM format
   SimpleRefHolder<QoreSSLCertificate> qc(new QoreSSLCertificate(pem, xsink));
   if (!*xsink)
      self->setPrivate(CID_SSLCERTIFICATE, qc.release());
}

// SSLCertificate::constructor(binary der) {}
static void SSLCertificate_constructor_1(QoreObject* self, const QoreListNode* args, ExceptionSink* xsink) {
   const BinaryNode* der = HARD_QORE_BINARY(args, 0);
   SimpleRefHolder<QoreSSLCertificate> qc(new QoreSSLCertificate(der, xsink));
   if (!*xsink)
      self->setPrivate(CID_SSLCERTIFICATE, qc.release());	
}

// SSLCertificate::copy() {}
static void SSLCertificate_copy(QoreObject* self, QoreObject* old, QoreSSLCertificate* s, ExceptionSink* xsink) {
   xsink->raiseException("SSLCERTIFICATE-COPY-ERROR", "SSLCertificate objects cannot be copied");
}

// hash SSLCertificate::getInfo() {}
static AbstractQoreNode* SSLCertificate_getInfo(QoreObject* self, QoreSSLCertificate* s, const QoreListNode* args, ExceptionSink* xsink) {
   return s->getInfo();
}

// hash SSLCertificate::getIssuerHash() {}
static AbstractQoreNode* SSLCertificate_getIssuerHash(QoreObject* self, QoreSSLCertificate* s, const QoreListNode* args, ExceptionSink* xsink) {
   return s->getIssuerHash();
}

// date SSLCertificate::getNotAfterDate() {}
static AbstractQoreNode* SSLCertificate_getNotAfterDate(QoreObject* self, QoreSSLCertificate* s, const QoreListNode* args, ExceptionSink* xsink) {
   return s->getNotAfterDate();
}

// date SSLCertificate::getNotBeforeDate() {}
static AbstractQoreNode* SSLCertificate_getNotBeforeDate(QoreObject* self, QoreSSLCertificate* s, const QoreListNode* args, ExceptionSink* xsink) {
   return s->getNotBeforeDate();
}

// string SSLCertificate::getPEM() {}
static AbstractQoreNode* SSLCertificate_getPEM(QoreObject* self, QoreSSLCertificate* s, const QoreListNode* args, ExceptionSink* xsink) {
   return s->getPEM(xsink);
}

// *binary SSLCertificate::getPublicKey() {}
static AbstractQoreNode* SSLCertificate_getPublicKey(QoreObject* self, QoreSSLCertificate* s, const QoreListNode* args, ExceptionSink* xsink) {
   return s->getPublicKey();
}

// string SSLCertificate::getPublicKeyAlgorithm() {}
static AbstractQoreNode* SSLCertificate_getPublicKeyAlgorithm(QoreObject* self, QoreSSLCertificate* s, const QoreListNode* args, ExceptionSink* xsink) {
   return s->getPublicKeyAlgorithm();
}

// hash SSLCertificate::getPurposeHash() {}
static AbstractQoreNode* SSLCertificate_getPurposeHash(QoreObject* self, QoreSSLCertificate* s, const QoreListNode* args, ExceptionSink* xsink) {
   return s->getPurposeHash();
}

// int SSLCertificate::getSerialNumber() {}
static int64 SSLCertificate_getSerialNumber(QoreObject* self, QoreSSLCertificate* s, const QoreListNode* args, ExceptionSink* xsink) {
   return s->getSerialNumber();
}

// binary SSLCertificate::getSignature() {}
static AbstractQoreNode* SSLCertificate_getSignature(QoreObject* self, QoreSSLCertificate* s, const QoreListNode* args, ExceptionSink* xsink) {
   return s->getSignature();
}

// string SSLCertificate::getSignatureType() {}
static AbstractQoreNode* SSLCertificate_getSignatureType(QoreObject* self, QoreSSLCertificate* s, const QoreListNode* args, ExceptionSink* xsink) {
   return s->getSignatureType();
}

// hash SSLCertificate::getSubjectHash() {}
static AbstractQoreNode* SSLCertificate_getSubjectHash(QoreObject* self, QoreSSLCertificate* s, const QoreListNode* args, ExceptionSink* xsink) {
   return s->getSubjectHash();
}

// int SSLCertificate::getVersion() {}
static int64 SSLCertificate_getVersion(QoreObject* self, QoreSSLCertificate* s, const QoreListNode* args, ExceptionSink* xsink) {
   return s->getVersion();
}

QoreClass* initSSLCertificateClass(QoreNamespace &qorens) {
   QC_SSLCERTIFICATE = new QoreClass("SSLCertificate");
   CID_SSLCERTIFICATE = QC_SSLCERTIFICATE->getID();

   // SSLCertificate::constructor(string pem) {}
   QC_SSLCERTIFICATE->setConstructorExtended(SSLCertificate_constructor, false, QC_NO_FLAGS, QDOM_DEFAULT, 1, stringTypeInfo, NULL);

   // SSLCertificate::constructor(binary der) {}
   QC_SSLCERTIFICATE->setConstructorExtended(SSLCertificate_constructor_1, false, QC_NO_FLAGS, QDOM_DEFAULT, 1, binaryTypeInfo, NULL);

   // SSLCertificate::copy() {}
   QC_SSLCERTIFICATE->setCopy((q_copy_t)SSLCertificate_copy);

   // hash SSLCertificate::getInfo() {}
   QC_SSLCERTIFICATE->addMethodExtended("getInfo", (q_method_t)SSLCertificate_getInfo, false, QC_CONSTANT, QDOM_DEFAULT, hashTypeInfo);

   // hash SSLCertificate::getIssuerHash() {}
   QC_SSLCERTIFICATE->addMethodExtended("getIssuerHash", (q_method_t)SSLCertificate_getIssuerHash, false, QC_CONSTANT, QDOM_DEFAULT, hashTypeInfo);

   // date SSLCertificate::getNotAfterDate() {}
   QC_SSLCERTIFICATE->addMethodExtended("getNotAfterDate", (q_method_t)SSLCertificate_getNotAfterDate, false, QC_CONSTANT, QDOM_DEFAULT, dateTypeInfo);

   // date SSLCertificate::getNotBeforeDate() {}
   QC_SSLCERTIFICATE->addMethodExtended("getNotBeforeDate", (q_method_t)SSLCertificate_getNotBeforeDate, false, QC_CONSTANT, QDOM_DEFAULT, dateTypeInfo);

   // string SSLCertificate::getPEM() {}
   QC_SSLCERTIFICATE->addMethodExtended("getPEM", (q_method_t)SSLCertificate_getPEM, false, QC_NO_FLAGS, QDOM_DEFAULT, stringTypeInfo);

   // *binary SSLCertificate::getPublicKey() {}
   QC_SSLCERTIFICATE->addMethodExtended("getPublicKey", (q_method_t)SSLCertificate_getPublicKey, false, QC_CONSTANT, QDOM_DEFAULT, binaryOrNothingTypeInfo);

   // string SSLCertificate::getPublicKeyAlgorithm() {}
   QC_SSLCERTIFICATE->addMethodExtended("getPublicKeyAlgorithm", (q_method_t)SSLCertificate_getPublicKeyAlgorithm, false, QC_CONSTANT, QDOM_DEFAULT, stringTypeInfo);

   // hash SSLCertificate::getPurposeHash() {}
   QC_SSLCERTIFICATE->addMethodExtended("getPurposeHash", (q_method_t)SSLCertificate_getPurposeHash, false, QC_CONSTANT, QDOM_DEFAULT, hashTypeInfo);

   // int SSLCertificate::getSerialNumber() {}
   QC_SSLCERTIFICATE->addMethodExtended("getSerialNumber", (q_method_int64_t)SSLCertificate_getSerialNumber, false, QC_CONSTANT, QDOM_DEFAULT, bigIntTypeInfo);

   // binary SSLCertificate::getSignature() {}
   QC_SSLCERTIFICATE->addMethodExtended("getSignature", (q_method_t)SSLCertificate_getSignature, false, QC_CONSTANT, QDOM_DEFAULT, binaryTypeInfo);

   // string SSLCertificate::getSignatureType() {}
   QC_SSLCERTIFICATE->addMethodExtended("getSignatureType", (q_method_t)SSLCertificate_getSignatureType, false, QC_CONSTANT, QDOM_DEFAULT, stringTypeInfo);

   // hash SSLCertificate::getSubjectHash() {}
   QC_SSLCERTIFICATE->addMethodExtended("getSubjectHash", (q_method_t)SSLCertificate_getSubjectHash, false, QC_CONSTANT, QDOM_DEFAULT, hashTypeInfo);

   // int SSLCertificate::getVersion() {}
   QC_SSLCERTIFICATE->addMethodExtended("getVersion", (q_method_int64_t)SSLCertificate_getVersion, false, QC_CONSTANT, QDOM_DEFAULT, bigIntTypeInfo);

   return QC_SSLCERTIFICATE;
}
