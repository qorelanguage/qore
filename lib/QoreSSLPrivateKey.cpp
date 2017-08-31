/*
  QoreSSLPrivateKey.h
 
  Qore Programming Language
 
  Copyright (C) 2003 - 2015 David Nichols
  
  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.

  Note that the Qore library is released under a choice of three open-source
  licenses: MIT (as above), LGPL 2+, or GPL 2+; see README-LICENSE for more
  information.
*/

#include <qore/Qore.h>
#include <qore/QoreSSLPrivateKey.h>
#include "qore/intern/QoreSSLIntern.h"

#include <errno.h>
#include <openssl/err.h>

struct qore_sslpk_private {
   EVP_PKEY* pk;

   DLLLOCAL qore_sslpk_private(EVP_PKEY* p) : pk(p) {
   }

   DLLLOCAL ~qore_sslpk_private() {
      if (pk)
         EVP_PKEY_free(pk);
   }
};

QoreSSLPrivateKey::QoreSSLPrivateKey(EVP_PKEY* p) : priv(new qore_sslpk_private(p)) {
}

QoreSSLPrivateKey::~QoreSSLPrivateKey() {
   delete priv;
}

QoreSSLPrivateKey::QoreSSLPrivateKey(const char* fn, const char* pp, ExceptionSink* xsink) : priv(new qore_sslpk_private(0)) {
   priv->pk = 0;
   FILE* fp = fopen(fn, "r");
   if (!fp) {
      xsink->raiseErrnoException("SSLPRIVATEKEY-CONSTRUCTOR-ERROR", errno, "'%s'", fn);
      return;
   }
   PEM_read_PrivateKey(fp, &priv->pk, 0, pp ? (void* )pp : (void* )"_none_");
   fclose(fp);
   if (!priv->pk)
      xsink->raiseException("SSLPRIVATEKEY-CONSTRUCTOR-ERROR", "error parsing private key file '%s'", fn);
}

QoreSSLPrivateKey::QoreSSLPrivateKey(const BinaryNode* bin, ExceptionSink* xsink) : priv(new qore_sslpk_private(0)) {
   OPENSSL_CONST unsigned char* p = (OPENSSL_CONST unsigned char* )bin->getPtr();
   priv->pk = d2i_AutoPrivateKey(0, &p, (int)bin->size());
   if (!priv->pk) {
      long e = ERR_get_error();
      char buf[121];
      ERR_error_string(e, buf);
      xsink->raiseException("SSLPRIVATEKEY-CONSTRUCTOR-ERROR", buf);
   }
}

QoreSSLPrivateKey::QoreSSLPrivateKey(const QoreString* str, const char* pp, ExceptionSink* xsink) : priv(new qore_sslpk_private(0)) {
   QoreMemBIO mbio(str);

   PEM_read_bio_PrivateKey(mbio.getBIO(), &priv->pk, 0, pp ? (void* )pp : (void* )"_none_");
   if (!priv->pk)
      xsink->raiseException("SSLPRIVATEKEY-CONSTRUCTOR-ERROR", "error parsing PEM string");
}

EVP_PKEY* QoreSSLPrivateKey::getData() const { 
   return priv->pk; 
}

QoreStringNode* QoreSSLPrivateKey::getPEM(ExceptionSink* xsink) const {
   BIO* bp = BIO_new(BIO_s_mem());
   if (!PEM_write_bio_PrivateKey(bp, priv->pk, 0, 0, 0, 0, 0)) {
      BIO_free(bp);
      xsink->raiseException("SSLPRIVATEKEY-ERROR", "could not create PEM string from private key data");
      return 0;
   }
   char* buf;
   long len = BIO_get_mem_data(bp, &buf);
   
   QoreStringNode* str = new QoreStringNode(buf, (int)len);
   BIO_free(bp);
   return str;
}

const char* QoreSSLPrivateKey::getType() const {
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

QoreHashNode* QoreSSLPrivateKey::getInfo() const {
   QoreHashNode* h = new QoreHashNode;
   h->setKeyValue("type", new QoreStringNode(getType()), 0);
   h->setKeyValue("version", new QoreBigIntNode(getVersion()), 0);
   h->setKeyValue("bitLength", new QoreBigIntNode(getBitLength()), 0);
   return h;
}

QoreSSLPrivateKey* QoreSSLPrivateKey::pkRefSelf() const {
   const_cast<QoreSSLPrivateKey*>(this)->ref();
   return const_cast<QoreSSLPrivateKey*>(this);
}
