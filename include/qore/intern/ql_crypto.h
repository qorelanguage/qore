/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  ql_crypto.h

  libcrypto-based cryptographic functions

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

#ifndef _QORE_QL_CRYPTO_H

#define _QORE_QL_CRYPTO_H

#include <openssl/evp.h>
#include <openssl/des.h>
#include <openssl/hmac.h>

#define MD2_ERR "MD2-DIGEST-ERROR"
#define MD4_ERR "MD4-DIGEST-ERROR"
#define MD5_ERR "MD5-DIGEST-ERROR"
#define SHA_ERR "SHA-DIGEST-ERROR"
#define SHA1_ERR "SHA1-DIGEST-ERROR"
static const char SHA224_ERR[] = "SHA224-DIGEST-ERROR";
static const char SHA256_ERR[] = "SHA256-DIGEST-ERROR";
static const char SHA384_ERR[] = "SHA384-DIGEST-ERROR";
static const char SHA512_ERR[] = "SHA512-DIGEST-ERROR";
#define DSS_ERR "DSS-DIGEST-ERROR"
#define DSS1_ERR "DSS1-DIGEST-ERROR"
static const char MDC2_ERR[] = "MDC2-DIGEST-ERROR";
#define RIPEMD160_ERR "RIPEMD160-DIGEST-ERROR"

DLLLOCAL void init_crypto_functions(QoreNamespace& ns);

class BaseHelper {
protected:
   unsigned char* input;
   size_t input_len;

   unsigned char md_value[EVP_MAX_MD_SIZE > HMAC_MAX_MD_CBLOCK ? EVP_MAX_MD_SIZE : HMAC_MAX_MD_CBLOCK];
   unsigned int md_len;

   DLLLOCAL void setInput(const QoreString& str) {
      input = (unsigned char*)str.getBuffer();
      input_len = str.strlen();
   }

   DLLLOCAL void setInput(const BinaryNode& b) {
      input = (unsigned char*)b.getPtr();
      input_len = b.size();
   }

   DLLLOCAL void setInput(const AbstractQoreNode* pt) {
      if (pt->getType() == NT_STRING)
         setInput(*reinterpret_cast<const QoreStringNode*>(pt));
      else {
         assert(pt->getType() == NT_BINARY);
         setInput(*reinterpret_cast<const BinaryNode*>(pt));
      }
   }
   
public:
   DLLLOCAL unsigned int size() const {
      return md_len;
   }

   DLLLOCAL const void* getBuffer() const {
      return (const void*)md_value;
   }

   DLLLOCAL void getString(QoreString& str) const {
      for (unsigned i = 0; i < md_len; i++)
	 str.sprintf("%02x", md_value[i]);
   }

   DLLLOCAL QoreStringNode* getString() const {
      QoreStringNode* str = new QoreStringNode;
      for (unsigned i = 0; i < md_len; i++)
	 str->sprintf("%02x", md_value[i]);

      return str;
   }

   DLLLOCAL BinaryNode* getBinary() const {
      BinaryNode* b = new BinaryNode;
      b->append(md_value, md_len);
      return b;
   }
};

class DigestHelper : public BaseHelper {
public:
   DLLLOCAL DigestHelper(const QoreListNode* params) {
      setInput(get_param(params, 0));
   }

   DLLLOCAL DigestHelper(const QoreString& str) {
      setInput(str);
   }

   DLLLOCAL DigestHelper(const BinaryNode& b) {
      setInput(b);
   }

   DLLLOCAL DigestHelper(const void* buf, size_t len) {
      input = (unsigned char*)buf;
      input_len = len;
   }

   DLLLOCAL int doDigest(const char* err, const EVP_MD* md, ExceptionSink* xsink = 0) {
      EVP_MD_CTX mdctx;
      EVP_MD_CTX_init(&mdctx);
	 
      EVP_DigestInit_ex(&mdctx, md, 0);

      if (!EVP_DigestUpdate(&mdctx, input, input_len) || !EVP_DigestFinal_ex(&mdctx, md_value, &md_len)) {
	 EVP_MD_CTX_cleanup(&mdctx);
         if (xsink)
            xsink->raiseException(err, "error calculating digest");
	 return -1;
      }

      EVP_MD_CTX_cleanup(&mdctx);
      return 0;
   }
   
};

class HMACHelper : public BaseHelper {

public:
    DLLLOCAL HMACHelper(const QoreListNode* params) {
        setInput(get_param(params, 0));
    }

    DLLLOCAL HMACHelper(const QoreStringNode& str) {
        setInput(str);
    }

    DLLLOCAL HMACHelper(const BinaryNode& b) {
        setInput(b);
    }

    DLLLOCAL HMACHelper(const void* buf, size_t len) {
        input = (unsigned char*)buf;
        input_len = len;
    }

    DLLLOCAL int doHMAC(const char* err, const EVP_MD* md, const QoreString* key, ExceptionSink* xsink) {
        HMAC_CTX ctx;
        HMAC_CTX_init(&ctx);

#ifdef HAVE_OPENSSL_HMAC_RV
        int rc = HMAC_Init_ex(&ctx, key->getBuffer(), key->strlen(), md, 0);
        if (!rc) {
            xsink->raiseException(err, "error initalizing HMAC");
            return -1;
        }
#else
        HMAC_Init_ex(&ctx, key->getBuffer(), key->strlen(), md, 0);
#endif

#ifdef HAVE_OPENSSL_HMAC_RV
        if (!HMAC_Update(&ctx, input, input_len)
            || !HMAC_Final(&ctx, md_value, &md_len)) {
            xsink->raiseException(err, "error calculating HMAC");
            return -1;
        }
#else
        HMAC_Update(&ctx, input, input_len);
        HMAC_Final(&ctx, md_value, &md_len);
#endif
    
        HMAC_CTX_cleanup(&ctx);
        return 0;
    }
};

#endif // _QORE_QL_CRYPTO_H
