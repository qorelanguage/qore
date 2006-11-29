/*
  QC_SSLCertificate.h
  
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

#ifndef _QORE_CLASS_SSLCERTIFICATE_H

#define _QORE_CLASS_SSLCERTIFICATE_H

extern int CID_SSLCERTIFICATE;
class QoreClass *initSSLCertificateClass();

#include <qore/AbstractPrivateData.h>
#include <qore/Exception.h>
#include <qore/QoreSSLBase.h>
#include <qore/support.h>
#include <qore/BinaryObject.h>

#include <errno.h>

class QoreSSLCertificate : public AbstractPrivateData, public QoreSSLBase
{
   private:
      X509 *cert;

      inline class QoreNode *doPurposeValue(int id, int ca)
      {
	 int rc = X509_check_purpose(cert, id, ca);
	 if (rc == 1)
	    return boolean_true();
	 
	 if (!rc)
	    return boolean_false();
	 
	 return new QoreNode((int64)rc);
      }

   protected:
      virtual ~QoreSSLCertificate()
      {
	 if (cert)
	    X509_free(cert);
      }

   public:
      inline QoreSSLCertificate(X509 *c) : cert(c) {}
      inline QoreSSLCertificate(char *fn, class ExceptionSink *xsink)
      {
	 cert = NULL;
	 FILE *fp = fopen(fn, "r");
	 if (!fp)
	 {
	    xsink->raiseException("SSLCERTIFICATE-CONSTRUCTOR-ERROR", "'%s': %s", fn, strerror(errno));
	    return;
	 }

	 PEM_read_X509(fp, &cert, NULL, NULL);
	 fclose(fp);
	 if (!cert)
	    xsink->raiseException("SSLCERTIFICATE-CONSTRUCTOR-ERROR", "error parsing certificate file '%s'", fn);
      }
      inline X509 *getData() { return cert; }
      inline class QoreString *getPEM(class ExceptionSink *xsink)
      {
	 BIO *bp = BIO_new(BIO_s_mem());
	 if (!PEM_write_bio_X509(bp, cert))
	 {
	    BIO_free(bp);
	    xsink->raiseException("X509-ERROR", "could not create PEM string from X509 certificate data");
	    return NULL;
	 }
	 char *buf;
	 long len = BIO_get_mem_data(bp, &buf);

	 class QoreString *str = new QoreString(buf, (int)len);
	 BIO_free(bp);
	 return str;
      }
      inline class Hash *getSubjectHash()
      {
	 return X509_NAME_to_hash(X509_get_subject_name(cert));
      }
      inline class Hash *getIssuerHash()
      {
	 return X509_NAME_to_hash(X509_get_issuer_name(cert));
      }
      inline int64 getSerialNumber()
      {
	 return (int64)ASN1_INTEGER_get(X509_get_serialNumber(cert));
      }
      inline int64 getVersion()
      {
	 return (int64)(X509_get_version(cert) + 1);
      }
      inline class Hash *getPurposeHash();
      inline class DateTime *getNotBeforeDate()
      {
	 return ASN1_TIME_to_DateTime(X509_get_notBefore(cert));
      }
      inline class DateTime *getNotAfterDate()
      {
	 return ASN1_TIME_to_DateTime(X509_get_notAfter(cert));
      }
      inline class QoreString *getSignatureType()
      {
	 return ASN1_OBJECT_to_QoreString(cert->sig_alg->algorithm);
      }
      inline class BinaryObject *getSignature()
      {
	 int len = cert->signature->length;
	 char *buf = (char *)malloc(len);
	 if (!buf)
	    return NULL;
	 memcpy(buf, cert->signature->data, len);
	 return new BinaryObject(buf, len);
      }
      inline class QoreString *getPublicKeyAlgorithm()
      {
	 return ASN1_OBJECT_to_QoreString(cert->cert_info->key->algor->algorithm);
      }
      inline class BinaryObject *getPublicKey()
      {
	 int len = cert->cert_info->key->public_key->length;
	 char *buf = (char *)malloc(len);
	 if (!buf)
	    return NULL;
	 memcpy(buf, cert->cert_info->key->public_key->data, len);
	 return new BinaryObject(buf, len);
      }

      inline class Hash *getInfo();
};

inline class Hash *QoreSSLCertificate::getPurposeHash()
{
   class Hash *h = new Hash();
   QoreString tstr;
   for (int i = 0; i < X509_PURPOSE_get_count(); i++)
   {
      X509_PURPOSE *pt = X509_PURPOSE_get0(i);

      int id = X509_PURPOSE_get_id(pt);
      char *name, *nameca;
      switch (id)
      {
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
	    tstr.terminate(0);
	    tstr.concat(name);
	    tstr.concat("CA");
	    nameca = tstr.getBuffer();
	    break;
      }

      // get non-CA value
      h->setKeyValue(name, doPurposeValue(id, 0), NULL);
      // get CA value
      h->setKeyValue(nameca, doPurposeValue(id, 1), NULL);
   }
   return h;
}

inline Hash *QoreSSLCertificate::getInfo()
{
   class Hash *h = new Hash();
   // get version
   h->setKeyValue("version", new QoreNode(getVersion()), NULL);
   // get serial number
   h->setKeyValue("serialNumber", new QoreNode(getSerialNumber()), NULL);
   // do subject
   h->setKeyValue("subject", new QoreNode(getSubjectHash()), NULL);
   // do issuer
   h->setKeyValue("issuer", new QoreNode(getIssuerHash()), NULL);
   // get purposes
   h->setKeyValue("purposes", new QoreNode(getPurposeHash()), NULL);
   // get not before date
   h->setKeyValue("notBefore", new QoreNode(getNotBeforeDate()), NULL);
   // get not after date
   h->setKeyValue("notAfter", new QoreNode(getNotAfterDate()), NULL);
   // get signature type
   h->setKeyValue("signatureType", new QoreNode(getSignatureType()), NULL);
   // get signature
   //h->setKeyValue("signature", new QoreNode(getSignature()), NULL);
   // get public key
   //h->setKeyValue("publicKey", new QoreNode(getPublicKey()), NULL);

   return h;
}

#endif // _QORE_CLASS_SSLCERTIFICATE_H
