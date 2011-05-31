/*
  ql_crypto.cpp
  
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
#include <qore/intern/ql_crypto.h>

#include <openssl/evp.h>
#include <openssl/des.h>

#include <stdio.h>
#include <stdlib.h>

#define QCRYPTO_DECRYPT 0
#define QCRYPTO_ENCRYPT 1

// NOTE: the trailing null ('\0') is no longer included when encrypting strings

// default initialization vector
static unsigned char def_iv[] = { 0, 0, 0, 0, 0, 0, 0, 0 };

class BaseHelper {
protected:
   unsigned char *input;
   int input_len;

   DLLLOCAL void setInput(const AbstractQoreNode *pt) {
      if (pt->getType() == NT_STRING) {
	 const QoreStringNode *str = reinterpret_cast<const QoreStringNode *>(pt);
	 input = (unsigned char *)str->getBuffer();
	 input_len = str->strlen();
	 return;
      }

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

   DLLLOCAL int doDigest(const char *err, const EVP_MD *md, ExceptionSink *xsink) {
      EVP_MD_CTX mdctx;
      EVP_MD_CTX_init(&mdctx);
	 
      EVP_DigestInit_ex(&mdctx, md, 0);

      if (!EVP_DigestUpdate(&mdctx, input, input_len) || !EVP_DigestFinal_ex(&mdctx, md_value, &md_len)) {
	 EVP_MD_CTX_cleanup(&mdctx);
	 xsink->raiseException(err, "error calculating digest");
	 return -1;
      }

      EVP_MD_CTX_cleanup(&mdctx);
      return 0;
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

class CryptoHelper : public BaseHelper {
private:
   unsigned char *iv, *output;
   int output_len;

   DLLLOCAL const char *getOrdinal(int n) {
      return n == 1 ? "first" : (n == 2 ? "second" : "third");
   }

   DLLLOCAL void setKey(const QoreListNode *args, int n) {
      const AbstractQoreNode *pt = get_param(args, n);
      if (pt->getType() == NT_STRING) {
	 const QoreStringNode *str = reinterpret_cast<const QoreStringNode *>(pt);
	 key[n - 1] = (unsigned char *)str->getBuffer();
	 keylen[n - 1] = str->strlen();
	 return;
      }
	 
      const BinaryNode *b = reinterpret_cast<const BinaryNode *>(pt);
      key[n - 1] = (unsigned char *)b->getPtr();
      keylen[n - 1] = b->size();
   }

   // set the initialization vector (used with hard typing)
   DLLLOCAL int setIV(const char *err, const AbstractQoreNode *pt, ExceptionSink *xsink) {
      if (pt->getType() == NT_STRING) {
	 const QoreStringNode *str = reinterpret_cast<const QoreStringNode *>(pt);
	 if (str->strlen() < 8) {
	    xsink->raiseException(err, "the input vector must be at least 8 bytes long (%d bytes passed)", str->strlen());
	    return -1;
	 }
	 iv = (unsigned char *)str->getBuffer();
	 return 0;
      }
      
      const BinaryNode *b = reinterpret_cast<const BinaryNode *>(pt);
      if (b->size() < 8) {
	 xsink->raiseException(err, "the input vector must be at least 8 bytes long (%d bytes passed)", b->size());
	 return -1;
      }
      iv = (unsigned char *)b->getPtr();
      return 0;
   }

public:
   unsigned char *key[3];
   int keylen[3];

   DLLLOCAL CryptoHelper() {
      output = 0;
   }

   DLLLOCAL ~CryptoHelper() {
      if (output)
	 free(output);
   }

   DLLLOCAL BinaryNode *getBinary() {
      BinaryNode *b = new BinaryNode(output, output_len);
      output = 0;
      return b;
   }

   DLLLOCAL QoreStringNode *getString(const QoreEncoding *enc = QCS_DEFAULT) {
      // create the string
      QoreStringNode *str = new QoreStringNode((char *)output, output_len, output_len, enc);
      // add trialing '\0'
      str->terminate(output_len);

      output = 0;
      return str;
   }

   DLLLOCAL int setSingleKey(const char *err, const QoreListNode *params, ExceptionSink *xsink) {
      if (setIV(err, get_param(params, 2), xsink))
	 return -1;

      setInput(get_param(params, 0));
      setKey(params, 1);
      return 0;
   }

   DLLLOCAL int setTwoKeys(const char *err, const QoreListNode *params, ExceptionSink *xsink) {
      if (setIV(err, get_param(params, 3), xsink))
	 return -1;

      setInput(get_param(params, 0));
      setKey(params, 1);
      setKey(params, 2);
      return 0;
   }

   DLLLOCAL int setThreeKeys(const char *err, const QoreListNode *params, ExceptionSink *xsink) {
      if (setIV(err, get_param(params, 4), xsink))
	 return -1;

      setInput(get_param(params, 0));
      setKey(params, 1);
      setKey(params, 2);
      setKey(params, 3);
      return 0;
   }

   DLLLOCAL int doCipher(const EVP_CIPHER *type, const char *cipher, int do_crypt, ExceptionSink *xsink) {
      const char *err = (do_crypt ? "ENCRYPT-ERROR" : "DECRYPT-ERROR");

      EVP_CIPHER_CTX ctx;
      EVP_CIPHER_CTX_init(&ctx);
      EVP_CipherInit_ex(&ctx, type, 0, 0, 0, do_crypt);
      if (key[0]) {
	 if (keylen[0] > EVP_MAX_KEY_LENGTH)
	    keylen[0] = EVP_MAX_KEY_LENGTH;

	 if (!EVP_CIPHER_CTX_set_key_length(&ctx, keylen[0]) || !EVP_CipherInit_ex(&ctx, 0, 0, key[0], iv, -1)) {
	    xsink->raiseException(err, "error setting %s key length=%d", cipher, keylen[0]);
	    EVP_CIPHER_CTX_cleanup(&ctx);
	    return -1;
	 }
      }

      // we allocate 1 byte more than we need in case we return as a string so we can terminate it
      output = (unsigned char *)malloc(sizeof(char) * (input_len + (EVP_MAX_BLOCK_LENGTH * 2)));

      if (!EVP_CipherUpdate(&ctx, output, &output_len, input, input_len)) {
	 xsink->raiseException(err, "error %scrypting %s block", do_crypt ? "en" : "de", cipher);
	 EVP_CIPHER_CTX_cleanup(&ctx);
	 return -1;
      }

      int tmplen;
      // Buffer passed to EVP_EncryptFinal() must be after data just encrypted to avoid overwriting it.
      if (!EVP_CipherFinal_ex(&ctx, output + output_len, &tmplen)) {
	 xsink->raiseException(err, "error %scrypting final %s block", do_crypt ? "en" : "de", cipher);
	 EVP_CIPHER_CTX_cleanup(&ctx);
	 return -1;
      }

      EVP_CIPHER_CTX_cleanup(&ctx);
      output_len += tmplen;
      //printd(5, "cipher_intern() %s: in=%08p (%d) out=%08p (%d)\n", cipher, buf, len, cbuf, *size);
      return 0;
   }

   DLLLOCAL int checkKeyLen(const char *err, int n, int len, ExceptionSink *xsink) {
      if (keylen[n] < len) {
	 xsink->raiseException(err, "key length is not %d bytes long (%d bytes)", len, keylen[n]);
	 return -1;
      }
      keylen[n] = len;
      return 0;
   }

   DLLLOCAL int setDESKey(int n, ExceptionSink *xsink) {
      // force odd parity 
      //DES_set_odd_parity((DES_cblock *)key[n]);
      // populate the schedule structure
      //DES_set_key_unchecked((DES_cblock *)key[n], schedule);
      //key[n] = (unsigned char *)schedule;
      return 0;
   }
};

static void missing_openssl_feature(const char *f, ExceptionSink *xsink) {
   xsink->raiseException("MISSING-FEATURE-ERROR", "the openssl library version that qore was compiled with did not support the %s algorithm; for maximum portability, check Option::HAVE_%s before calling this function", f, f);
}

static AbstractQoreNode *f_blowfish_encrypt_cbc(const QoreListNode *params, ExceptionSink *xsink) {
   CryptoHelper ch;

   if (ch.setSingleKey("BLOWFISH-ENCRYPT-PARAM-ERROR", params, xsink)
       || ch.doCipher(EVP_bf_cbc(), "blowfish", QCRYPTO_ENCRYPT, xsink))
      return 0;

   return ch.getBinary();
}

static AbstractQoreNode *f_blowfish_decrypt_cbc(const QoreListNode *params, ExceptionSink *xsink) {
   CryptoHelper ch;

   if (ch.setSingleKey("BLOWFISH-DECRYPT-PARAM-ERROR", params, xsink)
       || ch.doCipher(EVP_bf_cbc(), "blowfish", QCRYPTO_DECRYPT, xsink))
      return 0;

   return ch.getBinary();
}

static AbstractQoreNode *f_blowfish_decrypt_cbc_to_string(const QoreListNode *params, ExceptionSink *xsink) {
   CryptoHelper ch;

   if (ch.setSingleKey("BLOWFISH-DECRYPT-PARAM-ERROR", params, xsink)
       || ch.doCipher(EVP_bf_cbc(), "blowfish", QCRYPTO_DECRYPT, xsink))
      return 0;

   return ch.getString();
}

static AbstractQoreNode *f_des_encrypt_cbc(const QoreListNode *params, ExceptionSink *xsink) {
   CryptoHelper ch;

   if (ch.setSingleKey("DES-ENCRYPT-PARAM-ERROR", params, xsink) 
       || ch.checkKeyLen("DES-KEY-ERROR", 0, 8, xsink)
       || ch.doCipher(EVP_des_cbc(), "DES", QCRYPTO_ENCRYPT, xsink))
	    return 0;

   return ch.getBinary();
}

static AbstractQoreNode *f_des_decrypt_cbc(const QoreListNode *params, ExceptionSink *xsink) {
   CryptoHelper ch;

   if (ch.setSingleKey("DES-DECRYPT-PARAM-ERROR", params, xsink) 
       || ch.checkKeyLen("DES-KEY-ERROR", 0, 8, xsink)
       || ch.doCipher(EVP_des_cbc(), "DES", QCRYPTO_DECRYPT, xsink))
	    return 0;

   return ch.getBinary();
}

static AbstractQoreNode *f_des_decrypt_cbc_to_string(const QoreListNode *params, ExceptionSink *xsink) {
   CryptoHelper ch;

   if (ch.setSingleKey("DES-DECRYPT-PARAM-ERROR", params, xsink) 
       || ch.checkKeyLen("DES-KEY-ERROR", 0, 8, xsink)
       || ch.doCipher(EVP_des_cbc(), "DES", QCRYPTO_DECRYPT, xsink))
	    return 0;

   return ch.getString();
}

// params (data, [key, [input_vector]])
static AbstractQoreNode *f_des_ede_encrypt_cbc(const QoreListNode *params, ExceptionSink *xsink) {
   CryptoHelper ch;

   if (ch.setSingleKey("DES-ENCRYPT-PARAM-ERROR", params, xsink) 
       || ch.checkKeyLen("DES-KEY-ERROR", 0, 16, xsink)
       || ch.doCipher(EVP_des_ede_cbc(), "DES", QCRYPTO_ENCRYPT, xsink))
      return 0;

   return ch.getBinary();
}

static AbstractQoreNode *f_des_ede_decrypt_cbc(const QoreListNode *params, ExceptionSink *xsink) {
   CryptoHelper ch;

   if (ch.setSingleKey("DES-DECRYPT-PARAM-ERROR", params, xsink) 
       || ch.checkKeyLen("DES-KEY-ERROR", 0, 16, xsink)
       || ch.doCipher(EVP_des_ede_cbc(), "DES", QCRYPTO_DECRYPT, xsink))
      return 0;

   return ch.getBinary();
}

static AbstractQoreNode *f_des_ede_decrypt_cbc_to_string(const QoreListNode *params, ExceptionSink *xsink) {
   CryptoHelper ch;

   if (ch.setSingleKey("DES-DECRYPT-PARAM-ERROR", params, xsink) 
       || ch.checkKeyLen("DES-KEY-ERROR", 0, 16, xsink)
       || ch.doCipher(EVP_des_ede_cbc(), "DES", QCRYPTO_DECRYPT, xsink))
      return 0;

   return ch.getString();
}

// params (data, [key, [input_vector]])
static AbstractQoreNode *f_des_ede3_encrypt_cbc(const QoreListNode *params, ExceptionSink *xsink) {
   CryptoHelper ch;

   if (ch.setSingleKey("DES-ENCRYPT-PARAM-ERROR", params, xsink) 
       || ch.checkKeyLen("DES-KEY-ERROR", 0, 24, xsink)
       || ch.doCipher(EVP_des_ede3_cbc(), "DES", QCRYPTO_ENCRYPT, xsink))
      return 0;

   return ch.getBinary();
}

static AbstractQoreNode *f_des_ede3_decrypt_cbc(const QoreListNode *params, ExceptionSink *xsink) {
   CryptoHelper ch;

   if (ch.setSingleKey("DES-DECRYPT-PARAM-ERROR", params, xsink) 
       || ch.checkKeyLen("DES-KEY-ERROR", 0, 24, xsink)
       || ch.doCipher(EVP_des_ede3_cbc(), "DES", QCRYPTO_DECRYPT, xsink))
      return 0;

   return ch.getBinary();
}

static AbstractQoreNode *f_des_ede3_decrypt_cbc_to_string(const QoreListNode *params, ExceptionSink *xsink) {
   CryptoHelper ch;

   if (ch.setSingleKey("DES-DECRYPT-PARAM-ERROR", params, xsink) 
       || ch.checkKeyLen("DES-KEY-ERROR", 0, 24, xsink)
       || ch.doCipher(EVP_des_ede3_cbc(), "DES", QCRYPTO_DECRYPT, xsink))
      return 0;

   return ch.getString();
}

// params (data, key, [input_vector])
static AbstractQoreNode *f_desx_encrypt_cbc(const QoreListNode *params, ExceptionSink *xsink) {
   CryptoHelper ch;

   if (ch.setSingleKey("DESX-ENCRYPT-PARAM-ERROR", params, xsink) 
       || ch.checkKeyLen("DESX-KEY-ERROR", 0, 24, xsink)
       || ch.doCipher(EVP_desx_cbc(), "DESX", QCRYPTO_ENCRYPT, xsink))
      return 0;

   return ch.getBinary();
}

static AbstractQoreNode *f_desx_decrypt_cbc(const QoreListNode *params, ExceptionSink *xsink) {
   CryptoHelper ch;

   if (ch.setSingleKey("DESX-DECRYPT-PARAM-ERROR", params, xsink) 
       || ch.checkKeyLen("DESX-KEY-ERROR", 0, 24, xsink)
       || ch.doCipher(EVP_desx_cbc(), "DESX", QCRYPTO_DECRYPT, xsink))
      return 0;

   return ch.getBinary();
}

static AbstractQoreNode *f_desx_decrypt_cbc_to_string(const QoreListNode *params, ExceptionSink *xsink) {
   CryptoHelper ch;

   if (ch.setSingleKey("DESX-DECRYPT-PARAM-ERROR", params, xsink) 
       || ch.checkKeyLen("DESX-KEY-ERROR", 0, 24, xsink)
       || ch.doCipher(EVP_desx_cbc(), "DESX", QCRYPTO_DECRYPT, xsink))
      return 0;

   return ch.getString();
}

static AbstractQoreNode *f_rc4_encrypt(const QoreListNode *params, ExceptionSink *xsink) {
   CryptoHelper ch;

   if (ch.setSingleKey("RC4-ENCRYPT-PARAM-ERROR", params, xsink)
       || ch.doCipher(EVP_rc4(), "rc4", QCRYPTO_ENCRYPT, xsink))
      return 0;

   return ch.getBinary();
}

static AbstractQoreNode *f_rc4_decrypt(const QoreListNode *params, ExceptionSink *xsink) {
   CryptoHelper ch;

   if (ch.setSingleKey("RC4-DECRYPT-PARAM-ERROR", params, xsink)
       || ch.doCipher(EVP_rc4(), "rc4", QCRYPTO_DECRYPT, xsink))
      return 0;

   return ch.getBinary();
}

static AbstractQoreNode *f_rc4_decrypt_to_string(const QoreListNode *params, ExceptionSink *xsink) {
   CryptoHelper ch;

   if (ch.setSingleKey("RC4-DECRYPT-PARAM-ERROR", params, xsink)
       || ch.doCipher(EVP_rc4(), "rc4", QCRYPTO_DECRYPT, xsink))
      return 0;

   return ch.getString();
}

static AbstractQoreNode *f_rc2_encrypt_cbc(const QoreListNode *params, ExceptionSink *xsink) {
   CryptoHelper ch;

   if (ch.setSingleKey("RC2-ENCRYPT-PARAM-ERROR", params, xsink)
       || ch.doCipher(EVP_rc2_cbc(), "rc2", QCRYPTO_ENCRYPT, xsink))
      return 0;

   return ch.getBinary();
}

static AbstractQoreNode *f_rc2_decrypt_cbc(const QoreListNode *params, ExceptionSink *xsink) {
   CryptoHelper ch;

   if (ch.setSingleKey("RC2-DECRYPT-PARAM-ERROR", params, xsink)
       || ch.doCipher(EVP_rc2_cbc(), "rc2", QCRYPTO_DECRYPT, xsink))
      return 0;

   return ch.getBinary();
}

static AbstractQoreNode *f_rc2_decrypt_cbc_to_string(const QoreListNode *params, ExceptionSink *xsink) {
   CryptoHelper ch;

   if (ch.setSingleKey("RC2-DECRYPT-PARAM-ERROR", params, xsink)
       || ch.doCipher(EVP_rc2_cbc(), "rc2", QCRYPTO_DECRYPT, xsink))
      return 0;

   return ch.getString();
}

static AbstractQoreNode *f_cast5_encrypt_cbc(const QoreListNode *params, ExceptionSink *xsink) {
   CryptoHelper ch;

   if (ch.setSingleKey("CAST5-ENCRYPT-PARAM-ERROR", params, xsink)
       || ch.doCipher(EVP_cast5_cbc(), "CAST5", QCRYPTO_ENCRYPT, xsink))
      return 0;

   return ch.getBinary();
}

static AbstractQoreNode *f_cast5_decrypt_cbc(const QoreListNode *params, ExceptionSink *xsink) {
   CryptoHelper ch;

   if (ch.setSingleKey("CAST5-DECRYPT-PARAM-ERROR", params, xsink)
       || ch.doCipher(EVP_cast5_cbc(), "CAST5", QCRYPTO_DECRYPT, xsink))
      return 0;

   return ch.getBinary();
}

static AbstractQoreNode *f_cast5_decrypt_cbc_to_string(const QoreListNode *params, ExceptionSink *xsink) {
   CryptoHelper ch;

   if (ch.setSingleKey("CAST5-DECRYPT-PARAM-ERROR", params, xsink)
       || ch.doCipher(EVP_cast5_cbc(), "CAST5", QCRYPTO_DECRYPT, xsink))
      return 0;

   return ch.getString();
}

static AbstractQoreNode *f_rc5_encrypt_cbc(const QoreListNode *params, ExceptionSink *xsink) {
#ifndef OPENSSL_NO_RC5
   CryptoHelper ch;

   if (ch.setSingleKey("RC5-ENCRYPT-PARAM-ERROR", params, xsink)
       || ch.doCipher(EVP_rc5_32_12_16_cbc(), "rc5", QCRYPTO_ENCRYPT, xsink))
      return 0;

   return ch.getBinary();
#else
   missing_openssl_feature("RC5", xsink);
   return 0;
#endif
}

static AbstractQoreNode *f_rc5_decrypt_cbc(const QoreListNode *params, ExceptionSink *xsink) {
#ifndef OPENSSL_NO_RC5
   CryptoHelper ch;

   if (ch.setSingleKey("RC5-DECRYPT-PARAM-ERROR", params, xsink)
       || ch.doCipher(EVP_rc5_32_12_16_cbc(), "rc5", QCRYPTO_DECRYPT, xsink))
      return 0;

   return ch.getBinary();
#else
   missing_openssl_feature("RC5", xsink);
   return 0;
#endif
}

static AbstractQoreNode *f_rc5_decrypt_cbc_to_string(const QoreListNode *params, ExceptionSink *xsink) {
#ifndef OPENSSL_NO_RC5
   CryptoHelper ch;

   if (ch.setSingleKey("RC5-DECRYPT-PARAM-ERROR", params, xsink)
       || ch.doCipher(EVP_rc5_32_12_16_cbc(), "rc5", QCRYPTO_DECRYPT, xsink))
      return 0;

   return ch.getString();
#else
   missing_openssl_feature("RC5", xsink);
   return 0;
#endif
}

#define MD2_ERR "MD2-DIGEST-ERROR"
static AbstractQoreNode *f_MD2(const QoreListNode *params, ExceptionSink *xsink) {
#if !defined(OPENSSL_NO_MD2) && !defined(NO_MD2)
   DigestHelper dh(params);
   if (dh.doDigest(MD2_ERR, EVP_md2(), xsink))
      return 0;

   return dh.getString();
#else
   missing_openssl_feature("MD2", xsink);
   return 0;
#endif
}

#define MD4_ERR "MD4-DIGEST-ERROR"
static AbstractQoreNode *f_MD4(const QoreListNode *params, ExceptionSink *xsink) {
   DigestHelper dh(params);
   if (dh.doDigest(MD4_ERR, EVP_md4(), xsink))
      return 0;

   return dh.getString();
}

#define MD5_ERR "MD5-DIGEST-ERROR"
static AbstractQoreNode *f_MD5(const QoreListNode *params, ExceptionSink *xsink) {
   DigestHelper dh(params);
   if (dh.doDigest(MD5_ERR, EVP_md5(), xsink))
      return 0;

   return dh.getString();
}

#define SHA_ERR "SHA-DIGEST-ERROR"
static AbstractQoreNode *f_SHA(const QoreListNode *params, ExceptionSink *xsink) {
   DigestHelper dh(params);
   if (dh.doDigest(SHA_ERR, EVP_sha(), xsink))
      return 0;

   return dh.getString();
}

#define SHA1_ERR "SHA1-DIGEST-ERROR"
static AbstractQoreNode *f_SHA1(const QoreListNode *params, ExceptionSink *xsink) {
   DigestHelper dh(params);
   if (dh.doDigest(SHA1_ERR, EVP_sha1(), xsink))
      return 0;

   return dh.getString();
}

static const char SHA224_ERR[] = "SHA224-DIGEST-ERROR";
static AbstractQoreNode *f_SHA224(const QoreListNode *params, ExceptionSink *xsink) {
#if !defined(OPENSSL_NO_SHA256) && defined(HAVE_OPENSSL_SHA512)
   DigestHelper dh(params);
   if (dh.doDigest(SHA224_ERR, EVP_sha224(), xsink))
      return 0;

   return dh.getString();
#else
   missing_openssl_feature("SHA224", xsink);
   return 0;
#endif
}

static const char SHA256_ERR[] = "SHA256-DIGEST-ERROR";
static AbstractQoreNode *f_SHA256(const QoreListNode *params, ExceptionSink *xsink) {
#if !defined(OPENSSL_NO_SHA256) && defined(HAVE_OPENSSL_SHA512)
   DigestHelper dh(params);
   if (dh.doDigest(SHA256_ERR, EVP_sha256(), xsink))
      return 0;

   return dh.getString();
#else
   missing_openssl_feature("SHA256", xsink);
   return 0;
#endif
}

static const char SHA384_ERR[] = "SHA384-DIGEST-ERROR";
static AbstractQoreNode *f_SHA384(const QoreListNode *params, ExceptionSink *xsink) {
#if !defined(OPENSSL_NO_SHA512) && defined(HAVE_OPENSSL_SHA512)
   DigestHelper dh(params);
   if (dh.doDigest(SHA384_ERR, EVP_sha384(), xsink))
      return 0;

   return dh.getString();
#else
   missing_openssl_feature("SHA384", xsink);
   return 0;
#endif
}

static const char SHA512_ERR[] = "SHA512-DIGEST-ERROR";
static AbstractQoreNode *f_SHA512(const QoreListNode *params, ExceptionSink *xsink) {
#if !defined(OPENSSL_NO_SHA512) && defined(HAVE_OPENSSL_SHA512)
   DigestHelper dh(params);
   if (dh.doDigest(SHA512_ERR, EVP_sha512(), xsink))
      return 0;

   return dh.getString();
#else
   missing_openssl_feature("SHA512", xsink);
   return 0;
#endif
}

#define DSS_ERR "DSS-DIGEST-ERROR"
static AbstractQoreNode *f_DSS(const QoreListNode *params, ExceptionSink *xsink) {
   DigestHelper dh(params);
   if (dh.doDigest(DSS_ERR, EVP_dss(), xsink))
      return 0;

   return dh.getString();
}

#define DSS1_ERR "DSS1-DIGEST-ERROR"
static AbstractQoreNode *f_DSS1(const QoreListNode *params, ExceptionSink *xsink) {
   DigestHelper dh(params);
   if (dh.doDigest(DSS1_ERR, EVP_dss1(), xsink))
      return 0;

   return dh.getString();
}

static const char MDC2_ERR[] = "MDC2-DIGEST-ERROR";
static AbstractQoreNode *f_MDC2(const QoreListNode *params, ExceptionSink *xsink) {
#ifndef OPENSSL_NO_MDC2
   DigestHelper dh(params);
   if (dh.doDigest(MDC2_ERR, EVP_mdc2(), xsink))
      return 0;

   return dh.getString();
#else
   xsink->raiseException("MDC2-ERROR", "the openssl library version that qore was compiled with did not support the MDC2 algorithm; for maximum portability, check Option::HAVE_MDC2 before calling this function");
   return 0;
#endif
}

#define RIPEMD160_ERR "RIPEMD160-DIGEST-ERROR"
static AbstractQoreNode *f_RIPEMD160(const QoreListNode *params, ExceptionSink *xsink) {
   DigestHelper dh(params);
   if (dh.doDigest(RIPEMD160_ERR, EVP_ripemd160(), xsink))
      return 0;

   return dh.getString();
}

static AbstractQoreNode *f_MD2_bin(const QoreListNode *params, ExceptionSink *xsink) {
#if !defined(OPENSSL_NO_MD2) && !defined(NO_MD2)
   DigestHelper dh(params);
   if (dh.doDigest(MD2_ERR, EVP_md2(), xsink))
      return 0;
   
   return dh.getBinary();
#else
   missing_openssl_feature("MD2", xsink);
   return 0;
#endif
}

static AbstractQoreNode *f_MD4_bin(const QoreListNode *params, ExceptionSink *xsink) {
   DigestHelper dh(params);
   if (dh.doDigest(MD4_ERR, EVP_md4(), xsink))
      return 0;
   
   return dh.getBinary();
}

static AbstractQoreNode *f_MD5_bin(const QoreListNode *params, ExceptionSink *xsink) {
   DigestHelper dh(params);
   if (dh.doDigest(MD5_ERR, EVP_md5(), xsink))
      return 0;
   
   return dh.getBinary();
}

static AbstractQoreNode *f_SHA_bin(const QoreListNode *params, ExceptionSink *xsink) {
   DigestHelper dh(params);
   if (dh.doDigest(SHA_ERR, EVP_sha(), xsink))
      return 0;
   
   return dh.getBinary();
}

static AbstractQoreNode *f_SHA1_bin(const QoreListNode *params, ExceptionSink *xsink) {
   DigestHelper dh(params);
   if (dh.doDigest(SHA1_ERR, EVP_sha1(), xsink))
      return 0;
   
   return dh.getBinary();
}

static AbstractQoreNode *f_SHA224_bin(const QoreListNode *params, ExceptionSink *xsink) {
#if !defined(OPENSSL_NO_SHA256) && defined(HAVE_OPENSSL_SHA512)
   DigestHelper dh(params);
   if (dh.doDigest(SHA224_ERR, EVP_sha224(), xsink))
      return 0;
   
   return dh.getBinary();
#else
   missing_openssl_feature("SHA224", xsink);
   return 0;
#endif
}

static AbstractQoreNode *f_SHA256_bin(const QoreListNode *params, ExceptionSink *xsink) {
#if !defined(OPENSSL_NO_SHA256) && defined(HAVE_OPENSSL_SHA512)
   DigestHelper dh(params);
   if (dh.doDigest(SHA256_ERR, EVP_sha256(), xsink))
      return 0;
   
   return dh.getBinary();
#else
   missing_openssl_feature("SHA256", xsink);
   return 0;
#endif
}

static AbstractQoreNode *f_SHA384_bin(const QoreListNode *params, ExceptionSink *xsink) {
#if !defined(OPENSSL_NO_SHA512) && defined(HAVE_OPENSSL_SHA512)
   DigestHelper dh(params);
   if (dh.doDigest(SHA384_ERR, EVP_sha384(), xsink))
      return 0;
   
   return dh.getBinary();
#else
   missing_openssl_feature("SHA384", xsink);
   return 0;
#endif
}

static AbstractQoreNode *f_SHA512_bin(const QoreListNode *params, ExceptionSink *xsink) {
#if !defined(OPENSSL_NO_SHA512) && defined(HAVE_OPENSSL_SHA512)
   DigestHelper dh(params);
   if (dh.doDigest(SHA512_ERR, EVP_sha512(), xsink))
      return 0;
   
   return dh.getBinary();
#else
   missing_openssl_feature("SHA512", xsink);
   return 0;
#endif
}

static AbstractQoreNode *f_DSS_bin(const QoreListNode *params, ExceptionSink *xsink) {
   DigestHelper dh(params);
   if (dh.doDigest(DSS_ERR, EVP_dss(), xsink))
      return 0;
   
   return dh.getBinary();
}

static AbstractQoreNode *f_DSS1_bin(const QoreListNode *params, ExceptionSink *xsink) {
   DigestHelper dh(params);
   if (dh.doDigest(DSS1_ERR, EVP_dss1(), xsink))
      return 0;
   
   return dh.getBinary();
}

static AbstractQoreNode *f_MDC2_bin(const QoreListNode *params, ExceptionSink *xsink) {
#ifndef OPENSSL_NO_MDC2
   DigestHelper dh(params);
   if (dh.doDigest(MDC2_ERR, EVP_mdc2(), xsink))
      return 0;
   
   return dh.getBinary();
#else
   missing_openssl_feature("MDC2", xsink);
   return 0;
#endif
}

static AbstractQoreNode *f_RIPEMD160_bin(const QoreListNode *params, ExceptionSink *xsink) {
   DigestHelper dh(params);
   if (dh.doDigest(RIPEMD160_ERR, EVP_ripemd160(), xsink))
      return 0;
   
   return dh.getBinary();
}

static AbstractQoreNode *f_des_random_key(const QoreListNode *params, ExceptionSink *xsink) {
   DES_cblock *db = (DES_cblock *)malloc(sizeof(DES_cblock));
   DES_random_key(db);
   return new BinaryNode(db, sizeof(DES_cblock));
}

void init_crypto_functions() {
   // create default argument for the initialization vector
   SimpleRefHolder<BinaryNode> defaultIV(new BinaryNode);
   defaultIV->append(def_iv, 8);
   
   builtinFunctions.add2("blowfish_encrypt_cbc", f_blowfish_encrypt_cbc, QC_NO_FLAGS, QDOM_DEFAULT, binaryTypeInfo, 3, dataTypeInfo, QORE_PARAM_NO_ARG, dataTypeInfo, QORE_PARAM_NO_ARG, dataTypeInfo, defaultIV->refSelf());
   builtinFunctions.add2("blowfish_decrypt_cbc", f_blowfish_decrypt_cbc, QC_NO_FLAGS, QDOM_DEFAULT, binaryTypeInfo, 3, binaryTypeInfo, QORE_PARAM_NO_ARG, dataTypeInfo, QORE_PARAM_NO_ARG, dataTypeInfo, defaultIV->refSelf());
   builtinFunctions.add2("blowfish_decrypt_cbc_to_string", f_blowfish_decrypt_cbc_to_string, QC_NO_FLAGS, QDOM_DEFAULT, stringTypeInfo, 3, binaryTypeInfo, QORE_PARAM_NO_ARG, dataTypeInfo, QORE_PARAM_NO_ARG, dataTypeInfo, defaultIV->refSelf());

   builtinFunctions.add2("des_encrypt_cbc", f_des_encrypt_cbc, QC_NO_FLAGS, QDOM_DEFAULT, binaryTypeInfo, 3, dataTypeInfo, QORE_PARAM_NO_ARG, dataTypeInfo, QORE_PARAM_NO_ARG, dataTypeInfo, defaultIV->refSelf());
   builtinFunctions.add2("des_decrypt_cbc", f_des_decrypt_cbc, QC_NO_FLAGS, QDOM_DEFAULT, binaryTypeInfo, 3, binaryTypeInfo, QORE_PARAM_NO_ARG, dataTypeInfo, QORE_PARAM_NO_ARG, dataTypeInfo, defaultIV->refSelf());
   builtinFunctions.add2("des_decrypt_cbc_to_string", f_des_decrypt_cbc_to_string, QC_NO_FLAGS, QDOM_DEFAULT, stringTypeInfo, 3, binaryTypeInfo, QORE_PARAM_NO_ARG, dataTypeInfo, QORE_PARAM_NO_ARG, dataTypeInfo, defaultIV->refSelf());

   builtinFunctions.add2("des_ede_encrypt_cbc", f_des_ede_encrypt_cbc, QC_NO_FLAGS, QDOM_DEFAULT, binaryTypeInfo, 3, dataTypeInfo, QORE_PARAM_NO_ARG, dataTypeInfo, QORE_PARAM_NO_ARG, dataTypeInfo, defaultIV->refSelf());
   builtinFunctions.add2("des_ede_decrypt_cbc", f_des_ede_decrypt_cbc, QC_NO_FLAGS, QDOM_DEFAULT, binaryTypeInfo, 3, binaryTypeInfo, QORE_PARAM_NO_ARG, dataTypeInfo, QORE_PARAM_NO_ARG, dataTypeInfo, defaultIV->refSelf());
   builtinFunctions.add2("des_ede_decrypt_cbc_to_string", f_des_ede_decrypt_cbc_to_string, QC_NO_FLAGS, QDOM_DEFAULT, stringTypeInfo, 3, binaryTypeInfo, QORE_PARAM_NO_ARG, dataTypeInfo, QORE_PARAM_NO_ARG, dataTypeInfo, defaultIV->refSelf());

   builtinFunctions.add2("des_ede3_encrypt_cbc", f_des_ede3_encrypt_cbc, QC_NO_FLAGS, QDOM_DEFAULT, binaryTypeInfo, 3, dataTypeInfo, QORE_PARAM_NO_ARG, dataTypeInfo, QORE_PARAM_NO_ARG, dataTypeInfo, defaultIV->refSelf());
   builtinFunctions.add2("des_ede3_decrypt_cbc", f_des_ede3_decrypt_cbc, QC_NO_FLAGS, QDOM_DEFAULT, binaryTypeInfo, 3, binaryTypeInfo, QORE_PARAM_NO_ARG, dataTypeInfo, QORE_PARAM_NO_ARG, dataTypeInfo, defaultIV->refSelf());
   builtinFunctions.add2("des_ede3_decrypt_cbc_to_string", f_des_ede3_decrypt_cbc_to_string, QC_NO_FLAGS, QDOM_DEFAULT, stringTypeInfo, 3, binaryTypeInfo, QORE_PARAM_NO_ARG, dataTypeInfo, QORE_PARAM_NO_ARG, dataTypeInfo, defaultIV->refSelf());

   builtinFunctions.add2("desx_encrypt_cbc", f_desx_encrypt_cbc, QC_NO_FLAGS, QDOM_DEFAULT, binaryTypeInfo, 3, dataTypeInfo, QORE_PARAM_NO_ARG, dataTypeInfo, QORE_PARAM_NO_ARG, dataTypeInfo, defaultIV->refSelf());
   builtinFunctions.add2("desx_decrypt_cbc", f_desx_decrypt_cbc, QC_NO_FLAGS, QDOM_DEFAULT, binaryTypeInfo, 3, binaryTypeInfo, QORE_PARAM_NO_ARG, dataTypeInfo, QORE_PARAM_NO_ARG, dataTypeInfo, defaultIV->refSelf());
   builtinFunctions.add2("desx_decrypt_cbc_to_string", f_desx_decrypt_cbc_to_string, QC_NO_FLAGS, QDOM_DEFAULT, stringTypeInfo, 3, binaryTypeInfo, QORE_PARAM_NO_ARG, dataTypeInfo, QORE_PARAM_NO_ARG, dataTypeInfo, defaultIV->refSelf());

   builtinFunctions.add2("rc4_encrypt", f_rc4_encrypt, QC_NO_FLAGS, QDOM_DEFAULT, binaryTypeInfo, 3, dataTypeInfo, QORE_PARAM_NO_ARG, dataTypeInfo, QORE_PARAM_NO_ARG, dataTypeInfo, defaultIV->refSelf());
   builtinFunctions.add2("rc4_decrypt", f_rc4_decrypt, QC_NO_FLAGS, QDOM_DEFAULT, binaryTypeInfo, 3, binaryTypeInfo, QORE_PARAM_NO_ARG, dataTypeInfo, QORE_PARAM_NO_ARG, dataTypeInfo, defaultIV->refSelf());
   builtinFunctions.add2("rc4_decrypt_to_string", f_rc4_decrypt_to_string, QC_NO_FLAGS, QDOM_DEFAULT, stringTypeInfo, 3, binaryTypeInfo, QORE_PARAM_NO_ARG, dataTypeInfo, QORE_PARAM_NO_ARG, dataTypeInfo, defaultIV->refSelf());

   builtinFunctions.add2("rc2_encrypt_cbc", f_rc2_encrypt_cbc, QC_NO_FLAGS, QDOM_DEFAULT, binaryTypeInfo, 3, dataTypeInfo, QORE_PARAM_NO_ARG, dataTypeInfo, QORE_PARAM_NO_ARG, dataTypeInfo, defaultIV->refSelf());
   builtinFunctions.add2("rc2_decrypt_cbc", f_rc2_decrypt_cbc, QC_NO_FLAGS, QDOM_DEFAULT, binaryTypeInfo, 3, binaryTypeInfo, QORE_PARAM_NO_ARG, dataTypeInfo, QORE_PARAM_NO_ARG, dataTypeInfo, defaultIV->refSelf());
   builtinFunctions.add2("rc2_decrypt_cbc_to_string", f_rc2_decrypt_cbc_to_string, QC_NO_FLAGS, QDOM_DEFAULT, stringTypeInfo, 3, binaryTypeInfo, QORE_PARAM_NO_ARG, dataTypeInfo, QORE_PARAM_NO_ARG, dataTypeInfo, defaultIV->refSelf());

   builtinFunctions.add2("cast5_encrypt_cbc", f_cast5_encrypt_cbc, QC_NO_FLAGS, QDOM_DEFAULT, binaryTypeInfo, 3, dataTypeInfo, QORE_PARAM_NO_ARG, dataTypeInfo, QORE_PARAM_NO_ARG, dataTypeInfo, defaultIV->refSelf());
   builtinFunctions.add2("cast5_decrypt_cbc", f_cast5_decrypt_cbc, QC_NO_FLAGS, QDOM_DEFAULT, binaryTypeInfo, 3, binaryTypeInfo, QORE_PARAM_NO_ARG, dataTypeInfo, QORE_PARAM_NO_ARG, dataTypeInfo, defaultIV->refSelf());
   builtinFunctions.add2("cast5_decrypt_cbc_to_string", f_cast5_decrypt_cbc_to_string, QC_NO_FLAGS, QDOM_DEFAULT, stringTypeInfo, 3, binaryTypeInfo, QORE_PARAM_NO_ARG, dataTypeInfo, QORE_PARAM_NO_ARG, dataTypeInfo, defaultIV->refSelf());

   builtinFunctions.add2("rc5_encrypt_cbc", f_rc5_encrypt_cbc, QC_NO_FLAGS, QDOM_DEFAULT, binaryTypeInfo, 3, dataTypeInfo, QORE_PARAM_NO_ARG, dataTypeInfo, QORE_PARAM_NO_ARG, dataTypeInfo, defaultIV->refSelf());
   builtinFunctions.add2("rc5_decrypt_cbc", f_rc5_decrypt_cbc, QC_NO_FLAGS, QDOM_DEFAULT, binaryTypeInfo, 3, binaryTypeInfo, QORE_PARAM_NO_ARG, dataTypeInfo, QORE_PARAM_NO_ARG, dataTypeInfo, defaultIV->refSelf());
   builtinFunctions.add2("rc5_decrypt_cbc_to_string", f_rc5_decrypt_cbc_to_string, QC_NO_FLAGS, QDOM_DEFAULT, stringTypeInfo, 3, binaryTypeInfo, QORE_PARAM_NO_ARG, dataTypeInfo, QORE_PARAM_NO_ARG, dataTypeInfo, defaultIV->refSelf());

   // digest functions
   builtinFunctions.add2("MD2",           f_MD2, QC_NO_FLAGS, QDOM_DEFAULT, stringTypeInfo, 1, dataTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("MD2_bin",       f_MD2_bin, QC_NO_FLAGS, QDOM_DEFAULT, binaryTypeInfo, 1, dataTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("MD4",           f_MD4, QC_NO_FLAGS, QDOM_DEFAULT, stringTypeInfo, 1, dataTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("MD4_bin",       f_MD4_bin, QC_NO_FLAGS, QDOM_DEFAULT, binaryTypeInfo, 1, dataTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("MD5",           f_MD5, QC_NO_FLAGS, QDOM_DEFAULT, stringTypeInfo, 1, dataTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("MD5_bin",       f_MD5_bin, QC_NO_FLAGS, QDOM_DEFAULT, binaryTypeInfo, 1, dataTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("SHA",           f_SHA, QC_NO_FLAGS, QDOM_DEFAULT, stringTypeInfo, 1, dataTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("SHA_bin",       f_SHA_bin, QC_NO_FLAGS, QDOM_DEFAULT, binaryTypeInfo, 1, dataTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("SHA1",          f_SHA1, QC_NO_FLAGS, QDOM_DEFAULT, stringTypeInfo, 1, dataTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("SHA1_bin",      f_SHA1_bin, QC_NO_FLAGS, QDOM_DEFAULT, binaryTypeInfo, 1, dataTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("SHA224",        f_SHA224, QC_NO_FLAGS, QDOM_DEFAULT, stringTypeInfo, 1, dataTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("SHA224_bin",    f_SHA224_bin, QC_NO_FLAGS, QDOM_DEFAULT, binaryTypeInfo, 1, dataTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("SHA256",        f_SHA256, QC_NO_FLAGS, QDOM_DEFAULT, stringTypeInfo, 1, dataTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("SHA256_bin",    f_SHA256_bin, QC_NO_FLAGS, QDOM_DEFAULT, binaryTypeInfo, 1, dataTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("SHA384",        f_SHA384, QC_NO_FLAGS, QDOM_DEFAULT, stringTypeInfo, 1, dataTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("SHA384_bin",    f_SHA384_bin, QC_NO_FLAGS, QDOM_DEFAULT, binaryTypeInfo, 1, dataTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("SHA512",        f_SHA512, QC_NO_FLAGS, QDOM_DEFAULT, stringTypeInfo, 1, dataTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("SHA512_bin",    f_SHA512_bin, QC_NO_FLAGS, QDOM_DEFAULT, binaryTypeInfo, 1, dataTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("DSS",           f_DSS, QC_NO_FLAGS, QDOM_DEFAULT, stringTypeInfo, 1, dataTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("DSS_bin",       f_DSS_bin, QC_NO_FLAGS, QDOM_DEFAULT, binaryTypeInfo, 1, dataTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("DSS1",          f_DSS1, QC_NO_FLAGS, QDOM_DEFAULT, stringTypeInfo, 1, dataTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("DSS1_bin",      f_DSS1_bin, QC_NO_FLAGS, QDOM_DEFAULT, binaryTypeInfo, 1, dataTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("MDC2",          f_MDC2, QC_NO_FLAGS, QDOM_DEFAULT, stringTypeInfo, 1, dataTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("MDC2_bin",      f_MDC2_bin, QC_NO_FLAGS, QDOM_DEFAULT, binaryTypeInfo, 1, dataTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("RIPEMD160",     f_RIPEMD160, QC_NO_FLAGS, QDOM_DEFAULT, stringTypeInfo, 1, dataTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("RIPEMD160_bin", f_RIPEMD160_bin, QC_NO_FLAGS, QDOM_DEFAULT, binaryTypeInfo, 1, dataTypeInfo, QORE_PARAM_NO_ARG);

   // other functions
   builtinFunctions.add2("des_random_key", f_des_random_key, QC_CONSTANT, QDOM_DEFAULT, binaryTypeInfo);
}
