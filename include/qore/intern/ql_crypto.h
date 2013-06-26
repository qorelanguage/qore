/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  ql_crypto.h

  libcrypto-based cryptographic functions

  Qore Programming Language

  Copyright 2003 - 2013 David Nichols

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

#ifndef _QORE_QL_CRYPTO_H

#define _QORE_QL_CRYPTO_H

#include <openssl/evp.h>
#include <openssl/des.h>
#include <openssl/hmac.h>


DLLLOCAL void init_crypto_functions(QoreNamespace& ns);

class BaseHelper {
protected:
   unsigned char *input;
   size_t input_len;

   unsigned char md_value[EVP_MAX_MD_SIZE > HMAC_MAX_MD_CBLOCK ? EVP_MAX_MD_SIZE : HMAC_MAX_MD_CBLOCK];
   unsigned int md_len;

   DLLLOCAL void setInput(const QoreStringNode& str) {
      input = (unsigned char *)str.getBuffer();
      input_len = str.strlen();
   }

   DLLLOCAL void setInput(const BinaryNode& b) {
      input = (unsigned char *)b.getPtr();
      input_len = b.size();
   }

   DLLLOCAL void setInput(const AbstractQoreNode *pt) {
      if (pt->getType() == NT_STRING)
         setInput(*reinterpret_cast<const QoreStringNode *>(pt));
      else {
         assert(pt->getType() == NT_BINARY);
         setInput(*reinterpret_cast<const BinaryNode *>(pt));
      }
   }
   
public:
    
   DLLLOCAL unsigned int size() const {
      return md_len;
   }

   DLLLOCAL const void* getBuffer() const {
      return (const void*)md_value;
   }

   DLLLOCAL QoreStringNode *getString() const {
      QoreStringNode *str = new QoreStringNode();
      for (unsigned i = 0; i < md_len; i++)
	 str->sprintf("%02x", md_value[i]);

      return str;
   }

   DLLLOCAL BinaryNode *getBinary() const {
      BinaryNode *b = new BinaryNode();
      b->append(md_value, md_len);
      return b;
   }  
};

class DigestHelper : public BaseHelper {

public:
   DLLLOCAL DigestHelper(const QoreListNode *params) {
      setInput(get_param(params, 0));
   }

   DLLLOCAL DigestHelper(const QoreStringNode& str) {
      setInput(str);
   }

   DLLLOCAL DigestHelper(const BinaryNode& b) {
      setInput(b);
   }

   DLLLOCAL DigestHelper(const void* buf, size_t len) {
      input = (unsigned char*)buf;
      input_len = len;
   }

   DLLLOCAL int doDigest(const char *err, const EVP_MD *md, ExceptionSink *xsink = 0) {
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
    DLLLOCAL HMACHelper(const QoreListNode *params) {
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

    DLLLOCAL int doHMAC(const char *err, const EVP_MD *md, const QoreStringNode *key, ExceptionSink *xsink) {
        HMAC_CTX ctx;
        HMAC_CTX_init(&ctx);
 
        HMAC_Init_ex(&ctx, key->getBuffer(), key->strlen(), md, 0);

        if (!HMAC_Update(&ctx, input, input_len)
            || !HMAC_Final(&ctx, md_value, &md_len))
        {
            xsink->raiseException(err, "error calculating HMAC");
            return -1;
        }
    
        HMAC_CTX_cleanup(&ctx);
        return 0;
    }
};

#endif // _QORE_QL_CRYPTO_H
