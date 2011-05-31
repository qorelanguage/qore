/*
 QoreSSLPrivateKey.h
 
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
#include <qore/QoreSSLPrivateKey.h>
#include <qore/intern/QoreSSLIntern.h>

#include <errno.h>
#include <openssl/err.h>

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

QoreSSLPrivateKey::QoreSSLPrivateKey(const char *fn, const char *pp, ExceptionSink *xsink) : priv(new qore_sslpk_private(0)) {
   priv->pk = 0;
   FILE *fp = fopen(fn, "r");
   if (!fp) {
      xsink->raiseErrnoException("SSLPRIVATEKEY-CONSTRUCTOR-ERROR", errno, "'%s'", fn);
      return;
   }
   PEM_read_PrivateKey(fp, &priv->pk, 0, pp ? (void *)pp : (void *)"_none_");
   fclose(fp);
   if (!priv->pk)
      xsink->raiseException("SSLPRIVATEKEY-CONSTRUCTOR-ERROR", "error parsing private key file '%s'", fn);
}

QoreSSLPrivateKey::QoreSSLPrivateKey(const BinaryNode *bin, ExceptionSink *xsink) : priv(new qore_sslpk_private(0)) {
   OPENSSL_CONST unsigned char *p = (OPENSSL_CONST unsigned char *)bin->getPtr();
   priv->pk = d2i_AutoPrivateKey(0, &p, (int)bin->size());
   if (!priv->pk) {
      long e = ERR_get_error();
      char buf[121];
      ERR_error_string(e, buf);
      xsink->raiseException("SSLPRIVATEKEY-CONSTRUCTOR-ERROR", buf);
   }
}

QoreSSLPrivateKey::QoreSSLPrivateKey(const QoreString *str, const char *pp, ExceptionSink *xsink) : priv(new qore_sslpk_private(0)) {
   QoreMemBIO mbio(str);

   PEM_read_bio_PrivateKey(mbio.getBIO(), &priv->pk, 0, pp ? (void *)pp : (void *)"_none_");
   if (!priv->pk)
      xsink->raiseException("SSLPRIVATEKEY-CONSTRUCTOR-ERROR", "error parsing PEM string");
}

EVP_PKEY *QoreSSLPrivateKey::getData() const { 
   return priv->pk; 
}

QoreStringNode *QoreSSLPrivateKey::getPEM(ExceptionSink *xsink) const {
   BIO *bp = BIO_new(BIO_s_mem());
   if (!PEM_write_bio_PrivateKey(bp, priv->pk, 0, 0, 0, 0, 0)) {
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
   return 1;
}

// returns the length in bits
int64 QoreSSLPrivateKey::getBitLength() const {
   return (int64)EVP_PKEY_bits(priv->pk);
}

QoreHashNode *QoreSSLPrivateKey::getInfo() const {
   QoreHashNode *h = new QoreHashNode();
   h->setKeyValue("type", new QoreStringNode(getType()), 0);
   h->setKeyValue("version", new QoreBigIntNode(getVersion()), 0);
   h->setKeyValue("bitLength", new QoreBigIntNode(getBitLength()), 0);
   return h;
}
