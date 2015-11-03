/*
 QoreSSLCertificate.cc
 
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

#include <openssl/err.h>

struct qore_sslcert_private {
      X509 *cert;

      DLLLOCAL qore_sslcert_private(X509 *c) : cert(c) {
      }

      DLLLOCAL ~qore_sslcert_private() {
	 if (cert)
	    X509_free(cert);
      }
};

class QoreBIO {
   protected:
      BIO *b;

      DLLLOCAL QoreBIO(BIO *n_b) : b(n_b) {}

   public:
      DLLLOCAL ~QoreBIO() { if (b) BIO_free(b); }
      DLLLOCAL int writePEMX509(X509 *cert) { return PEM_write_bio_X509(b, cert); }
      DLLLOCAL long getMemData(char **buf) { return BIO_get_mem_data(b, buf); }
      DLLLOCAL X509 *getX509(X509 **x) { return d2i_X509_bio(b, x); }
      DLLLOCAL BIO *getBIO() { return b; }
};

class QoreMemBIO : public QoreBIO {
   public:
      DLLLOCAL QoreMemBIO() : QoreBIO(BIO_new(BIO_s_mem())) {}
      DLLLOCAL QoreMemBIO(const BinaryNode *b) : QoreBIO(BIO_new_mem_buf((void *)b->getPtr(), (int)b->size())) {}
      DLLLOCAL QoreMemBIO(const QoreString *str) : QoreBIO(BIO_new_mem_buf((void *)str->getBuffer(), (int)str->strlen())) {}
};

QoreSSLCertificate::~QoreSSLCertificate() {
   delete priv;
}

QoreSSLCertificate::QoreSSLCertificate(X509 *c) : priv(new qore_sslcert_private(c)) {
}

QoreSSLCertificate::QoreSSLCertificate(const BinaryNode *bin, ExceptionSink *xsink) : priv(new qore_sslcert_private(0)) {
   OPENSSL_CONST unsigned char *p = (OPENSSL_CONST unsigned char *)bin->getPtr();
   priv->cert = d2i_X509(0, &p, (int)bin->size());
   if (!priv->cert) {
      long e = ERR_get_error();
      char buf[121];
      ERR_error_string(e, buf);
      xsink->raiseException("SSLCERTIFICATE-CONSTRUCTOR-ERROR", buf);
   }
}

// DEPRECATED constructor
QoreSSLCertificate::QoreSSLCertificate(const char *fn, ExceptionSink *xsink) : priv(new qore_sslcert_private(0)) {
   FILE *fp = fopen(fn, "r");
   if (!fp) {
      xsink->raiseException("SSLCERTIFICATE-CONSTRUCTOR-ERROR", "'%s': %s", fn, strerror(errno));
      return;
   }
   
   PEM_read_X509(fp, &priv->cert, 0, 0);
   fclose(fp);
   if (!priv->cert)
      xsink->raiseException("SSLCERTIFICATE-CONSTRUCTOR-ERROR", "error parsing certificate file '%s'", fn);
}

QoreSSLCertificate::QoreSSLCertificate(const QoreString *str, ExceptionSink *xsink) : priv(new qore_sslcert_private(0)) {
   QoreMemBIO mbio(str);

   PEM_read_bio_X509(mbio.getBIO(), &priv->cert, 0, 0);
   if (!priv->cert)
      xsink->raiseException("SSLCERTIFICATE-CONSTRUCTOR-ERROR", "error parsing certificate PEM string");
}

QoreSSLCertificate::operator bool() const { return (bool)priv->cert; }

AbstractQoreNode *QoreSSLCertificate::doPurposeValue(int id, int ca) const {
   int rc = X509_check_purpose(priv->cert, id, ca);
   if (rc == 1)
      return boolean_true();
   
   if (!rc)
      return boolean_false();
   
   return new QoreBigIntNode(rc);
}

X509 *QoreSSLCertificate::getData() const { 
   return priv->cert; 
}

QoreStringNode *QoreSSLCertificate::getPEM(ExceptionSink *xsink) const {
   QoreMemBIO b;
   
   if (!b.writePEMX509(priv->cert)) {
      xsink->raiseException("X509-ERROR", "could not create PEM string from X509 certificate data");
      return 0;
   }

   char *buf;
   long len = b.getMemData(&buf);
   
   return new QoreStringNode(buf, (int)len);
}

QoreHashNode *QoreSSLCertificate::getSubjectHash() const {
   return X509_NAME_to_hash(X509_get_subject_name(priv->cert));
}

QoreHashNode *QoreSSLCertificate::getIssuerHash() const {
   return X509_NAME_to_hash(X509_get_issuer_name(priv->cert));
}

int64 QoreSSLCertificate::getSerialNumber() const {
   return (int64)ASN1_INTEGER_get(X509_get_serialNumber(priv->cert));
}

int64 QoreSSLCertificate::getVersion() const {
   return (int64)(X509_get_version(priv->cert) + 1);
}

DateTimeNode *QoreSSLCertificate::getNotBeforeDate() const {
   return ASN1_TIME_to_DateTime(X509_get_notBefore(priv->cert));
}

DateTimeNode *QoreSSLCertificate::getNotAfterDate() const {
   return ASN1_TIME_to_DateTime(X509_get_notAfter(priv->cert));
}

QoreStringNode *QoreSSLCertificate::getSignatureType() const {
   return ASN1_OBJECT_to_QoreStringNode(priv->cert->sig_alg->algorithm);
}

BinaryNode *QoreSSLCertificate::getSignature() const {
   int len = priv->cert->signature->length;
   char *buf = (char *)malloc(len);
   if (!buf)
      return 0;
   memcpy(buf, priv->cert->signature->data, len);
   return new BinaryNode(buf, len);
}

QoreStringNode *QoreSSLCertificate::getPublicKeyAlgorithm() const {
   return ASN1_OBJECT_to_QoreStringNode(priv->cert->cert_info->key->algor->algorithm);
}

// returns the public key for the certificate in DER format
BinaryNode *QoreSSLCertificate::getPublicKey() const {
   EVP_PKEY *key = X509_PUBKEY_get(priv->cert->cert_info->key);
   if (!key)
      return 0;

   int size = i2d_PUBKEY(key, 0);
   //printd(5, "QoreSSLCertificate::getPublicKey() public key size=%d\n", size);
   unsigned char *buf = 0;
   i2d_PUBKEY(priv->cert->cert_info->key->pkey, &buf);
   if (!buf)
      return 0;

   return new BinaryNode(buf, size);
}

QoreHashNode *QoreSSLCertificate::getPurposeHash() const {
   QoreHashNode *h = new QoreHashNode();
   QoreString tstr;
   for (int i = 0; i < X509_PURPOSE_get_count(); i++) {
      X509_PURPOSE *pt = X509_PURPOSE_get0(i);
      
      int id = X509_PURPOSE_get_id(pt);
      const char *name, *nameca;
      switch (id) {
	 case X509_PURPOSE_SSL_CLIENT:
	    name = "SSLclient";
	    nameca = "SSLclientCA";
	    break;
	 case X509_PURPOSE_SSL_SERVER:
	    name = "SSLserver";
	    nameca = "SSLserverCA";
	    break;
	 case X509_PURPOSE_NS_SSL_SERVER:
	    name = "netscapeSSLserver";
	    nameca = "netscapeSSLserverCA";
	    break;
	 case X509_PURPOSE_SMIME_SIGN:
	    name = "SMIMEsigning";
	    nameca = "SMIMEsigningCA";
	    break;
	 case X509_PURPOSE_SMIME_ENCRYPT:
	    name = "SMIMEencryption";
	    nameca = "SMIMEencryptionCA";
	    break;
	 case X509_PURPOSE_CRL_SIGN:
	    name = "CRLsigning";
	    nameca = "CRLsigningCA";
	    break;
	 case X509_PURPOSE_ANY:
	    name = "anyPurpose";
	    nameca = "anyPurposeCA";
	    break;
	 case X509_PURPOSE_OCSP_HELPER:
	    name = "OCSPhelper";
	    nameca = "OCSPhelperCA";
	    break;
	 default:
	    name = X509_PURPOSE_get0_name(pt);
	    tstr.clear();
	    tstr.concat(name);
	    tstr.concat("CA");
	    nameca = (char *)tstr.getBuffer();
	    break;
      }
      
      // get non-CA value
      h->setKeyValue(name, doPurposeValue(id, 0), 0);
      // get CA value
      h->setKeyValue(nameca, doPurposeValue(id, 1), 0);
   }
   return h;
}

QoreHashNode *QoreSSLCertificate::getInfo() const {
   QoreHashNode *h = new QoreHashNode();
   // get version
   h->setKeyValue("version", new QoreBigIntNode(getVersion()), 0);
   // get serial number
   h->setKeyValue("serialNumber", new QoreBigIntNode(getSerialNumber()), 0);
   // do subject
   h->setKeyValue("subject", getSubjectHash(), 0);
   // do issuer
   h->setKeyValue("issuer", getIssuerHash(), 0);
   // get purposes
   h->setKeyValue("purposes", getPurposeHash(), 0);
   // get not before date
   h->setKeyValue("notBefore", getNotBeforeDate(), 0);
   // get not after date
   h->setKeyValue("notAfter", getNotAfterDate(), 0);
   // get signature type
   h->setKeyValue("signatureType", getSignatureType(), 0);
   // get signature
   //h->setKeyValue("signature", getSignature(), 0);
   // get public key
   //h->setKeyValue("publicKey", getPublicKey(), 0);
   
   return h;
}
