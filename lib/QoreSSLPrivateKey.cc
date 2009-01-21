/*
 QoreSSLPrivateKey.h
 
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
#include <qore/QoreSSLPrivateKey.h>

#include <errno.h>

struct qore_sslpk_private {
      EVP_PKEY *pk;

      DLLLOCAL qore_sslpk_private(EVP_PKEY *p) : pk(p) {
      }

      DLLLOCAL ~qore_sslpk_private() {
	 if (pk)
	    EVP_PKEY_free(pk);
      }
};

QoreSSLPrivateKey::QoreSSLPrivateKey(EVP_PKEY *p) : priv(new qore_sslpk_private(p)) {
}

QoreSSLPrivateKey::~QoreSSLPrivateKey() {
   delete priv;
}

QoreSSLPrivateKey::QoreSSLPrivateKey(const char *fn, char *pp, ExceptionSink *xsink) : priv(new qore_sslpk_private(0)) {
   priv->pk = 0;
   FILE *fp = fopen(fn, "r");
   if (!fp)
   {
      xsink->raiseException("SSLPRIVATEKEY-CONSTRUCTOR-ERROR", "'%s': %s", fn, strerror(errno));
      return;
   }
   PEM_read_PrivateKey(fp, &priv->pk, 0, pp ? pp : (void *)"_none_");
   fclose(fp);
   if (!priv->pk)
      xsink->raiseException("SSLPRIVATEKEY-CONSTRUCTOR-ERROR", "error parsing private key file '%s'", fn);
}

EVP_PKEY *QoreSSLPrivateKey::getData() const { 
   return priv->pk; 
}

QoreStringNode *QoreSSLPrivateKey::getPEM(ExceptionSink *xsink) const {
   BIO *bp = BIO_new(BIO_s_mem());
   if (!PEM_write_bio_PrivateKey(bp, priv->pk, 0, 0, 0, 0, 0))
   {
      BIO_free(bp);
      xsink->raiseException("SSLPRIVATEKEY-ERROR", "could not create PEM string from private key data");
      return 0;
   }
   char *buf;
   long len = BIO_get_mem_data(bp, &buf);
   
   QoreStringNode *str = new QoreStringNode(buf, (int)len);
   BIO_free(bp);
   return str;
}

const char *QoreSSLPrivateKey::getType() const {
   switch (EVP_PKEY_type(priv->pk->type)) {
#ifndef OPENSSL_NO_RSA
      case EVP_PKEY_RSA:
	 return "RSA";
      case EVP_PKEY_RSA2:
	 return "RSA2";
#endif
#ifndef OPENSSL_NO_DSA
      case EVP_PKEY_DSA:
	 return "DSA";
      case EVP_PKEY_DSA1:
	 return "DSA1";
      case EVP_PKEY_DSA2:
	 return "DSA2";
      case EVP_PKEY_DSA3:
	 return "DSA3";
      case EVP_PKEY_DSA4:
	 return "DSA4";
#endif
#ifndef OPENSSL_NO_DH
      case EVP_PKEY_DH:
	 return "DH";
#endif
      default:
	 return "unknown";
   }
}

int64 QoreSSLPrivateKey::getVersion() const {
   switch (EVP_PKEY_type(priv->pk->type)) {
#ifndef OPENSSL_NO_RSA
      case EVP_PKEY_RSA:
	 return (int64)priv->pk->pkey.rsa->version + 1;
#endif
#ifndef OPENSSL_NO_DSA
      case EVP_PKEY_DSA:
	 return (int64)priv->pk->pkey.dsa->version + 1;
#endif
#ifndef OPENSSL_NO_DH
      case EVP_PKEY_DH:
	 return (int64)priv->pk->pkey.dh->version + 1;
#endif
      default:
	 return 0;
   }
}

// returns the length in bits
int64 QoreSSLPrivateKey::getBitLength() const {
   switch (EVP_PKEY_type(priv->pk->type)) {
#ifndef OPENSSL_NO_RSA
      case EVP_PKEY_RSA:
	 return (int64)RSA_size(priv->pk->pkey.rsa) * 8;
#endif
#ifndef OPENSSL_NO_DSA
      case EVP_PKEY_DSA:
	 return (int64)DSA_size(priv->pk->pkey.dsa) * 8;
#endif
#ifndef OPENSSL_NO_DH
      case EVP_PKEY_DH:
	 return (int64)DH_size(priv->pk->pkey.dh) * 8;
#endif
      default:
	 return 0;
   }
}

QoreHashNode *QoreSSLPrivateKey::getInfo() const {
   QoreHashNode *h = new QoreHashNode();
   h->setKeyValue("type", new QoreStringNode(getType()), 0);
   h->setKeyValue("version", new QoreBigIntNode(getVersion()), 0);
   h->setKeyValue("bitLength", new QoreBigIntNode(getBitLength()), 0);
   return h;
}
