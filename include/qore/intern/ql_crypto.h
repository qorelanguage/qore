/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  ql_crypto.h

  libcrypto-based cryptographic functions

  Qore Programming Language

  Copyright 2003 - 2012 David Nichols

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

DLLLOCAL void init_crypto_functions(QoreNamespace& ns);

class BaseHelper {
protected:
   unsigned char *input;
   size_t input_len;

   DLLLOCAL void setInput(const AbstractQoreNode *pt) {
      if (pt->getType() == NT_STRING) {
	 const QoreStringNode *str = reinterpret_cast<const QoreStringNode *>(pt);
	 input = (unsigned char *)str->getBuffer();
	 input_len = str->strlen();
	 return;
      }
      assert(pt->getType() == NT_BINARY);

      const BinaryNode *b = reinterpret_cast<const BinaryNode *>(pt);
      input = (unsigned char *)b->getPtr();
      input_len = b->size();
   }
};

class DigestHelper : public BaseHelper {
private:
   unsigned char md_value[EVP_MAX_MD_SIZE];
   unsigned int md_len;

public:
   DLLLOCAL DigestHelper(const QoreListNode *params) {
      setInput(get_param(params, 0));
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

#endif // _QORE_QL_CRYPTO_H
