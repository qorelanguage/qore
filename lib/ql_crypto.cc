/*
  ql_crypto.cc
  
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
#include <qore/intern/ql_crypto.h>

#include <openssl/evp.h>
#include <openssl/des.h>

#include <stdio.h>
#include <stdlib.h>

#define QC_DECRYPT 0
#define QC_ENCRYPT 1

// NOTE: the trailing null character is included when encrypting strings, but not when calculating digests

// default initialization vector
static unsigned char def_iv[] = { 0, 0, 0, 0, 0, 0, 0, 0 };

class BaseHelper {
   protected:
      unsigned char *input;
      int input_len;

      DLLLOCAL int getInput(const char *err, const QoreListNode *params, ExceptionSink *xsink, bool include_null = true) {
	 const AbstractQoreNode *pt = get_param(params, 0);

	 if (is_nothing(pt)) {
	    xsink->raiseException(err, "missing data (string or binary) parameter to function");
	    return -1;
	 }
	 {
	    const QoreStringNode *str = dynamic_cast<const QoreStringNode *>(pt);
	    if (str) {
	       input = (unsigned char *)str->getBuffer();
	       input_len = str->strlen() + include_null;
	       return 0;
	    }
	 }

	 const BinaryNode *b = dynamic_cast<const BinaryNode *>(pt);
	 if (b) {
	    input = (unsigned char *)b->getPtr();
	    input_len = b->size();
	    return 0;
	 }
	 
	 xsink->raiseException(err, "don't know how to process type '%s' (expecing string or binary)", pt->getTypeName());
	 return -1;
      }      
};

class DigestHelper : public BaseHelper {
   private:
      unsigned char md_value[EVP_MAX_MD_SIZE];
      unsigned int md_len;

   public:
      DLLLOCAL int getData(const char *err, const QoreListNode *params, ExceptionSink *xsink) {
	 return getInput(err, params, xsink, false);
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

      DLLLOCAL class BinaryNode *getBinary() const {
	 class BinaryNode *b = new BinaryNode();
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

      DLLLOCAL int getKey(const char *err, const QoreListNode *params, int n, ExceptionSink *xsink) {
	 const AbstractQoreNode *pt = get_param(params, n);

	 if (is_nothing(pt)) {
	    xsink->raiseException(err, "missing %s key parameter", getOrdinal(n));
	    return -1;
	 }

	 {
	    const QoreStringNode *str = dynamic_cast<const QoreStringNode *>(pt);
	    if (str) {
	       key[n - 1] = (unsigned char *)str->getBuffer();
	       keylen[n - 1] = str->strlen();
	       return 0;
	    }
	 }
	 
	 const BinaryNode *b = dynamic_cast<const BinaryNode *>(pt);
	 if (b) {
	    key[n - 1] = (unsigned char *)b->getPtr();
	    keylen[n - 1] = b->size();
	    return 0;
	 }

	 xsink->raiseException(err, "can't use type '%s' for %s key value", pt->getTypeName(), getOrdinal(n));
	 return -1;
      }

      // get initialization vector
      DLLLOCAL int getIV(const char *err, const QoreListNode *params, int n, ExceptionSink *xsink) {
	 const AbstractQoreNode *pt = get_param(params, n);
	 if (is_nothing(pt)) {
	    iv = def_iv;
	    return 0;
	 }

	 {
	    const QoreStringNode *str = dynamic_cast<const QoreStringNode *>(pt);
	    if (str) {
	       if (str->strlen() < 8) {
		  xsink->raiseException(err, "the input vector must be at least 8 bytes long (%d bytes passed)", str->strlen());
		  return -1;
	       }
	       iv = (unsigned char *)str->getBuffer();
	       return 0;
	    }
	 }

	 const BinaryNode *b = dynamic_cast<const BinaryNode *>(pt);
	 if (b) {
	    if (b->size() < 8) {
	       xsink->raiseException(err, "the input vector must be at least 8 bytes long (%d bytes passed)", b->size());
	       return -1;
	    }
	    iv = (unsigned char *)b->getPtr();
	    return 0;
	 }

	 xsink->raiseException(err, "can't use type '%s' as an input vector", pt->getTypeName());
	 return -1;
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
	 class BinaryNode *b = new BinaryNode(output, output_len);
	 output = 0;
	 return b;
      }

      DLLLOCAL QoreStringNode *getString(const QoreEncoding *enc = QCS_DEFAULT) {
	 // create the string
	 QoreStringNode *str = new QoreStringNode((char *)output, output_len, output_len, enc);
	 // terminate and set length as appropriate by checking the final byte
	 //str->terminate(((char *)output)[output_len - 1] ? output_len : output_len - 1);

	 output = 0;
	 return str;
      }

      DLLLOCAL int getSingleKey(const char *err, const QoreListNode *params, ExceptionSink *xsink) {
	 if (getInput(err, params, xsink) || getKey(err, params, 1, xsink) || getIV(err, params, 2, xsink))
	    return -1;
	 return 0;
      }

      DLLLOCAL int getTwoKeys(const char *err, const QoreListNode *params, ExceptionSink *xsink) {
	 if (getInput(err, params, xsink) || getKey(err, params, 1, xsink) || getKey(err, params, 2, xsink) || getIV(err, params, 3, xsink))
	    return -1;
	 return 0;
      }

      DLLLOCAL int getThreeKeys(const char *err, const QoreListNode *params, ExceptionSink *xsink) {
	 if (getInput(err, params, xsink) || getKey(err, params, 1, xsink) || getKey(err, params, 2, xsink) || getKey(err, params, 3, xsink) || getIV(err, params, 4, xsink))
	    return -1;
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

static AbstractQoreNode *f_blowfish_encrypt_cbc(const QoreListNode *params, ExceptionSink *xsink) {
   CryptoHelper ch;

   if (ch.getSingleKey("BLOWFISH-ENCRYPT-PARAM-ERROR", params, xsink)
       || ch.doCipher(EVP_bf_cbc(), "blowfish", QC_ENCRYPT, xsink))
      return 0;

   return ch.getBinary();
}

static AbstractQoreNode *f_blowfish_decrypt_cbc(const QoreListNode *params, ExceptionSink *xsink) {
   CryptoHelper ch;

   if (ch.getSingleKey("BLOWFISH-DECRYPT-PARAM-ERROR", params, xsink)
       || ch.doCipher(EVP_bf_cbc(), "blowfish", QC_DECRYPT, xsink))
      return 0;

   return ch.getBinary();
}

static AbstractQoreNode *f_blowfish_decrypt_cbc_to_string(const QoreListNode *params, ExceptionSink *xsink) {
   CryptoHelper ch;

   if (ch.getSingleKey("BLOWFISH-DECRYPT-PARAM-ERROR", params, xsink)
       || ch.doCipher(EVP_bf_cbc(), "blowfish", QC_DECRYPT, xsink))
      return 0;

   return ch.getString();
}

static AbstractQoreNode *f_des_encrypt_cbc(const QoreListNode *params, ExceptionSink *xsink) {
   CryptoHelper ch;

   if (ch.getSingleKey("DES-ENCRYPT-PARAM-ERROR", params, xsink) 
       || ch.checkKeyLen("DES-KEY-ERROR", 0, 8, xsink)
       || ch.doCipher(EVP_des_cbc(), "DES", QC_ENCRYPT, xsink))
	    return 0;

   return ch.getBinary();
}

static AbstractQoreNode *f_des_decrypt_cbc(const QoreListNode *params, ExceptionSink *xsink) {
   CryptoHelper ch;

   if (ch.getSingleKey("DES-DECRYPT-PARAM-ERROR", params, xsink) 
       || ch.checkKeyLen("DES-KEY-ERROR", 0, 8, xsink)
       || ch.doCipher(EVP_des_cbc(), "DES", QC_DECRYPT, xsink))
	    return 0;

   return ch.getBinary();
}

static AbstractQoreNode *f_des_decrypt_cbc_to_string(const QoreListNode *params, ExceptionSink *xsink) {
   CryptoHelper ch;

   if (ch.getSingleKey("DES-DECRYPT-PARAM-ERROR", params, xsink) 
       || ch.checkKeyLen("DES-KEY-ERROR", 0, 8, xsink)
       || ch.doCipher(EVP_des_cbc(), "DES", QC_DECRYPT, xsink))
	    return 0;

   return ch.getString();
}

// params (data, [key, [input_vector]])
static AbstractQoreNode *f_des_ede_encrypt_cbc(const QoreListNode *params, ExceptionSink *xsink) {
   CryptoHelper ch;

   if (ch.getSingleKey("DES-ENCRYPT-PARAM-ERROR", params, xsink) 
       || ch.checkKeyLen("DES-KEY-ERROR", 0, 16, xsink)
       || ch.doCipher(EVP_des_ede_cbc(), "DES", QC_ENCRYPT, xsink))
      return 0;

   return ch.getBinary();
}

static AbstractQoreNode *f_des_ede_decrypt_cbc(const QoreListNode *params, ExceptionSink *xsink) {
   CryptoHelper ch;

   if (ch.getSingleKey("DES-DECRYPT-PARAM-ERROR", params, xsink) 
       || ch.checkKeyLen("DES-KEY-ERROR", 0, 16, xsink)
       || ch.doCipher(EVP_des_ede_cbc(), "DES", QC_DECRYPT, xsink))
      return 0;

   return ch.getBinary();
}

static AbstractQoreNode *f_des_ede_decrypt_cbc_to_string(const QoreListNode *params, ExceptionSink *xsink) {
   CryptoHelper ch;

   if (ch.getSingleKey("DES-DECRYPT-PARAM-ERROR", params, xsink) 
       || ch.checkKeyLen("DES-KEY-ERROR", 0, 16, xsink)
       || ch.doCipher(EVP_des_ede_cbc(), "DES", QC_DECRYPT, xsink))
      return 0;

   return ch.getString();
}

// params (data, [key, [input_vector]])
static AbstractQoreNode *f_des_ede3_encrypt_cbc(const QoreListNode *params, ExceptionSink *xsink) {
   CryptoHelper ch;

   if (ch.getSingleKey("DES-ENCRYPT-PARAM-ERROR", params, xsink) 
       || ch.checkKeyLen("DES-KEY-ERROR", 0, 24, xsink)
       || ch.doCipher(EVP_des_ede3_cbc(), "DES", QC_ENCRYPT, xsink))
      return 0;

   return ch.getBinary();
}

static AbstractQoreNode *f_des_ede3_decrypt_cbc(const QoreListNode *params, ExceptionSink *xsink) {
   CryptoHelper ch;

   if (ch.getSingleKey("DES-DECRYPT-PARAM-ERROR", params, xsink) 
       || ch.checkKeyLen("DES-KEY-ERROR", 0, 24, xsink)
       || ch.doCipher(EVP_des_ede3_cbc(), "DES", QC_DECRYPT, xsink))
      return 0;

   return ch.getBinary();
}

static AbstractQoreNode *f_des_ede3_decrypt_cbc_to_string(const QoreListNode *params, ExceptionSink *xsink) {
   CryptoHelper ch;

   if (ch.getSingleKey("DES-DECRYPT-PARAM-ERROR", params, xsink) 
       || ch.checkKeyLen("DES-KEY-ERROR", 0, 24, xsink)
       || ch.doCipher(EVP_des_ede3_cbc(), "DES", QC_DECRYPT, xsink))
      return 0;

   return ch.getString();
}

// params (data, key, [input_vector])
static AbstractQoreNode *f_desx_encrypt_cbc(const QoreListNode *params, ExceptionSink *xsink) {
   CryptoHelper ch;

   if (ch.getSingleKey("DESX-ENCRYPT-PARAM-ERROR", params, xsink) 
       || ch.checkKeyLen("DESX-KEY-ERROR", 0, 24, xsink)
       || ch.doCipher(EVP_desx_cbc(), "DESX", QC_ENCRYPT, xsink))
      return 0;

   return ch.getBinary();
}

static AbstractQoreNode *f_desx_decrypt_cbc(const QoreListNode *params, ExceptionSink *xsink) {
   CryptoHelper ch;

   if (ch.getSingleKey("DESX-DECRYPT-PARAM-ERROR", params, xsink) 
       || ch.checkKeyLen("DESX-KEY-ERROR", 0, 24, xsink)
       || ch.doCipher(EVP_desx_cbc(), "DESX", QC_DECRYPT, xsink))
      return 0;

   return ch.getBinary();
}

static AbstractQoreNode *f_desx_decrypt_cbc_to_string(const QoreListNode *params, ExceptionSink *xsink) {
   CryptoHelper ch;

   if (ch.getSingleKey("DESX-DECRYPT-PARAM-ERROR", params, xsink) 
       || ch.checkKeyLen("DESX-KEY-ERROR", 0, 24, xsink)
       || ch.doCipher(EVP_desx_cbc(), "DESX", QC_DECRYPT, xsink))
      return 0;

   return ch.getString();
}

static AbstractQoreNode *f_rc4_encrypt(const QoreListNode *params, ExceptionSink *xsink) {
   CryptoHelper ch;

   if (ch.getSingleKey("RC4-ENCRYPT-PARAM-ERROR", params, xsink)
       || ch.doCipher(EVP_rc4(), "rc4", QC_ENCRYPT, xsink))
      return 0;

   return ch.getBinary();
}

static AbstractQoreNode *f_rc4_decrypt(const QoreListNode *params, ExceptionSink *xsink) {
   CryptoHelper ch;

   if (ch.getSingleKey("RC4-DECRYPT-PARAM-ERROR", params, xsink)
       || ch.doCipher(EVP_rc4(), "rc4", QC_DECRYPT, xsink))
      return 0;

   return ch.getBinary();
}

static AbstractQoreNode *f_rc4_decrypt_to_string(const QoreListNode *params, ExceptionSink *xsink) {
   CryptoHelper ch;

   if (ch.getSingleKey("RC4-DECRYPT-PARAM-ERROR", params, xsink)
       || ch.doCipher(EVP_rc4(), "rc4", QC_DECRYPT, xsink))
      return 0;

   return ch.getString();
}

static AbstractQoreNode *f_rc2_encrypt_cbc(const QoreListNode *params, ExceptionSink *xsink) {
   CryptoHelper ch;

   if (ch.getSingleKey("RC2-ENCRYPT-PARAM-ERROR", params, xsink)
       || ch.doCipher(EVP_rc2_cbc(), "rc2", QC_ENCRYPT, xsink))
      return 0;

   return ch.getBinary();
}

static AbstractQoreNode *f_rc2_decrypt_cbc(const QoreListNode *params, ExceptionSink *xsink) {
   CryptoHelper ch;

   if (ch.getSingleKey("RC2-DECRYPT-PARAM-ERROR", params, xsink)
       || ch.doCipher(EVP_rc2_cbc(), "rc2", QC_DECRYPT, xsink))
      return 0;

   return ch.getBinary();
}

static AbstractQoreNode *f_rc2_decrypt_cbc_to_string(const QoreListNode *params, ExceptionSink *xsink) {
   CryptoHelper ch;

   if (ch.getSingleKey("RC2-DECRYPT-PARAM-ERROR", params, xsink)
       || ch.doCipher(EVP_rc2_cbc(), "rc2", QC_DECRYPT, xsink))
      return 0;

   return ch.getString();
}

static AbstractQoreNode *f_cast5_encrypt_cbc(const QoreListNode *params, ExceptionSink *xsink) {
   CryptoHelper ch;

   if (ch.getSingleKey("CAST5-ENCRYPT-PARAM-ERROR", params, xsink)
       || ch.doCipher(EVP_cast5_cbc(), "CAST5", QC_ENCRYPT, xsink))
      return 0;

   return ch.getBinary();
}

static AbstractQoreNode *f_cast5_decrypt_cbc(const QoreListNode *params, ExceptionSink *xsink) {
   CryptoHelper ch;

   if (ch.getSingleKey("CAST5-DECRYPT-PARAM-ERROR", params, xsink)
       || ch.doCipher(EVP_cast5_cbc(), "CAST5", QC_DECRYPT, xsink))
      return 0;

   return ch.getBinary();
}

static AbstractQoreNode *f_cast5_decrypt_cbc_to_string(const QoreListNode *params, ExceptionSink *xsink) {
   CryptoHelper ch;

   if (ch.getSingleKey("CAST5-DECRYPT-PARAM-ERROR", params, xsink)
       || ch.doCipher(EVP_cast5_cbc(), "CAST5", QC_DECRYPT, xsink))
      return 0;

   return ch.getString();
}

static AbstractQoreNode *f_rc5_encrypt_cbc(const QoreListNode *params, ExceptionSink *xsink) {
#ifndef OPENSSL_NO_RC5
   CryptoHelper ch;

   if (ch.getSingleKey("RC5-ENCRYPT-PARAM-ERROR", params, xsink)
       || ch.doCipher(EVP_rc5_32_12_16_cbc(), "rc5", QC_ENCRYPT, xsink))
      return 0;

   return ch.getBinary();
#else
   xsink->raiseException("MISSING-FEATURE-ERROR", "the openssl library version that qore was compiled with did not support the RC5 algorithm; for maximum portability, check Option::HAVE_RC5 before calling this function");
   return 0;
#endif
}

static AbstractQoreNode *f_rc5_decrypt_cbc(const QoreListNode *params, ExceptionSink *xsink) {
#ifndef OPENSSL_NO_RC5
   CryptoHelper ch;

   if (ch.getSingleKey("RC5-DECRYPT-PARAM-ERROR", params, xsink)
       || ch.doCipher(EVP_rc5_32_12_16_cbc(), "rc5", QC_DECRYPT, xsink))
      return 0;

   return ch.getBinary();
#else
   xsink->raiseException("MISSING-FEATURE-ERROR", "the openssl library version that qore was compiled with did not support the RC5 algorithm; for maximum portability, check Option::HAVE_RC5 before calling this function");
   return 0;
#endif
}

static AbstractQoreNode *f_rc5_decrypt_cbc_to_string(const QoreListNode *params, ExceptionSink *xsink) {
#ifndef OPENSSL_NO_RC5
   CryptoHelper ch;

   if (ch.getSingleKey("RC5-DECRYPT-PARAM-ERROR", params, xsink)
       || ch.doCipher(EVP_rc5_32_12_16_cbc(), "rc5", QC_DECRYPT, xsink))
      return 0;

   return ch.getString();
#else
   xsink->raiseException("MISSING-FEATURE-ERROR", "the openssl library version that qore was compiled with did not support the RC5 algorithm; for maximum portability, check Option::HAVE_RC5 before calling this function");
   return 0;
#endif
}

#define MD2_ERR "MD2-DIGEST-ERROR"
static AbstractQoreNode *f_MD2(const QoreListNode *params, ExceptionSink *xsink) {
   DigestHelper dh;
   if (dh.getData(MD2_ERR, params, xsink) || dh.doDigest(MD2_ERR, EVP_md2(), xsink))
      return 0;

   return dh.getString();
}

#define MD4_ERR "MD4-DIGEST-ERROR"
static AbstractQoreNode *f_MD4(const QoreListNode *params, ExceptionSink *xsink) {
   DigestHelper dh;
   if (dh.getData(MD4_ERR, params, xsink) || dh.doDigest(MD4_ERR, EVP_md4(), xsink))
      return 0;

   return dh.getString();
}

#define MD5_ERR "MD5-DIGEST-ERROR"
static AbstractQoreNode *f_MD5(const QoreListNode *params, ExceptionSink *xsink) {
   DigestHelper dh;
   if (dh.getData(MD5_ERR, params, xsink) || dh.doDigest(MD5_ERR, EVP_md5(), xsink))
      return 0;

   return dh.getString();
}

#define SHA_ERR "SHA-DIGEST-ERROR"
static AbstractQoreNode *f_SHA(const QoreListNode *params, ExceptionSink *xsink) {
   DigestHelper dh;
   if (dh.getData(SHA_ERR, params, xsink) || dh.doDigest(SHA_ERR, EVP_sha(), xsink))
      return 0;

   return dh.getString();
}

#define SHA1_ERR "SHA1-DIGEST-ERROR"
static AbstractQoreNode *f_SHA1(const QoreListNode *params, ExceptionSink *xsink) {
   DigestHelper dh;
   if (dh.getData(SHA1_ERR, params, xsink) || dh.doDigest(SHA1_ERR, EVP_sha1(), xsink))
      return 0;

   return dh.getString();
}

static const char SHA224_ERR[] = "SHA224-DIGEST-ERROR";
static AbstractQoreNode *f_SHA224(const QoreListNode *params, ExceptionSink *xsink) {
#if !defined(OPENSSL_NO_SHA256) && defined(HAVE_OPENSSL_SHA512)
   DigestHelper dh;
   if (dh.getData(SHA224_ERR, params, xsink) || dh.doDigest(SHA224_ERR, EVP_sha224(), xsink))
      return 0;

   return dh.getString();
#else
   xsink->raiseException("MISSING-FEATURE-ERROR", "the openssl library version that qore was compiled with did not support the SHA224 algorithm; for maximum portability, check Option::HAVE_SHA224 before calling this function");
   return 0;
#endif
}

static const char SHA256_ERR[] = "SHA256-DIGEST-ERROR";
static AbstractQoreNode *f_SHA256(const QoreListNode *params, ExceptionSink *xsink) {
#if !defined(OPENSSL_NO_SHA256) && defined(HAVE_OPENSSL_SHA512)
   DigestHelper dh;
   if (dh.getData(SHA256_ERR, params, xsink) || dh.doDigest(SHA256_ERR, EVP_sha256(), xsink))
      return 0;

   return dh.getString();
#else
   xsink->raiseException("MISSING-FEATURE-ERROR", "the openssl library version that qore was compiled with did not support the SHA256 algorithm; for maximum portability, check Option::HAVE_SHA256 before calling this function");
   return 0;
#endif
}

static const char SHA384_ERR[] = "SHA384-DIGEST-ERROR";
static AbstractQoreNode *f_SHA384(const QoreListNode *params, ExceptionSink *xsink) {
#if !defined(OPENSSL_NO_SHA512) && defined(HAVE_OPENSSL_SHA512)
   DigestHelper dh;
   if (dh.getData(SHA384_ERR, params, xsink) || dh.doDigest(SHA384_ERR, EVP_sha384(), xsink))
      return 0;

   return dh.getString();
#else
   xsink->raiseException("MISSING-FEATURE-ERROR", "the openssl library version that qore was compiled with did not support the SHA384 algorithm; for maximum portability, check Option::HAVE_SHA384 before calling this function");
   return 0;
#endif
}

static const char SHA512_ERR[] = "SHA512-DIGEST-ERROR";
static AbstractQoreNode *f_SHA512(const QoreListNode *params, ExceptionSink *xsink) {
#if !defined(OPENSSL_NO_SHA512) && defined(HAVE_OPENSSL_SHA512)
   DigestHelper dh;
   if (dh.getData(SHA512_ERR, params, xsink) || dh.doDigest(SHA512_ERR, EVP_sha512(), xsink))
      return 0;

   return dh.getString();
#else
   xsink->raiseException("MISSING-FEATURE-ERROR", "the openssl library version that qore was compiled with did not support the SHA512 algorithm; for maximum portability, check Option::HAVE_SHA512 before calling this function");
   return 0;
#endif
}

#define DSS_ERR "DSS-DIGEST-ERROR"
static AbstractQoreNode *f_DSS(const QoreListNode *params, ExceptionSink *xsink) {
   DigestHelper dh;
   if (dh.getData(DSS_ERR, params, xsink) || dh.doDigest(DSS_ERR, EVP_dss(), xsink))
      return 0;

   return dh.getString();
}

#define DSS1_ERR "DSS1-DIGEST-ERROR"
static AbstractQoreNode *f_DSS1(const QoreListNode *params, ExceptionSink *xsink) {
   DigestHelper dh;
   if (dh.getData(DSS1_ERR, params, xsink) || dh.doDigest(DSS1_ERR, EVP_dss1(), xsink))
      return 0;

   return dh.getString();
}

static const char MDC2_ERR[] = "MDC2-DIGEST-ERROR";
static AbstractQoreNode *f_MDC2(const QoreListNode *params, ExceptionSink *xsink) {
#ifndef OPENSSL_NO_MDC2
   DigestHelper dh;
   if (dh.getData(MDC2_ERR, params, xsink) || dh.doDigest(MDC2_ERR, EVP_mdc2(), xsink))
      return 0;

   return dh.getString();
#else
   xsink->raiseException("MDC2-ERROR", "the openssl library version that qore was compiled with did not support the MDC2 algorithm; for maximum portability, check Option::HAVE_MDC2 before calling this function");
   return 0;
#endif
}

#define RIPEMD160_ERR "RIPEMD160-DIGEST-ERROR"
static AbstractQoreNode *f_RIPEMD160(const QoreListNode *params, ExceptionSink *xsink) {
   DigestHelper dh;
   if (dh.getData(RIPEMD160_ERR, params, xsink) || dh.doDigest(RIPEMD160_ERR, EVP_ripemd160(), xsink))
      return 0;

   return dh.getString();
}

static AbstractQoreNode *f_MD2_bin(const QoreListNode *params, ExceptionSink *xsink) {
   DigestHelper dh;
   if (dh.getData(MD2_ERR, params, xsink) || dh.doDigest(MD2_ERR, EVP_md2(), xsink))
      return 0;
   
   return dh.getBinary();
}

static AbstractQoreNode *f_MD4_bin(const QoreListNode *params, ExceptionSink *xsink) {
   DigestHelper dh;
   if (dh.getData(MD4_ERR, params, xsink) || dh.doDigest(MD4_ERR, EVP_md4(), xsink))
      return 0;
   
   return dh.getBinary();
}

static AbstractQoreNode *f_MD5_bin(const QoreListNode *params, ExceptionSink *xsink) {
   DigestHelper dh;
   if (dh.getData(MD5_ERR, params, xsink) || dh.doDigest(MD5_ERR, EVP_md5(), xsink))
      return 0;
   
   return dh.getBinary();
}

static AbstractQoreNode *f_SHA_bin(const QoreListNode *params, ExceptionSink *xsink) {
   DigestHelper dh;
   if (dh.getData(SHA_ERR, params, xsink) || dh.doDigest(SHA_ERR, EVP_sha(), xsink))
      return 0;
   
   return dh.getBinary();
}

static AbstractQoreNode *f_SHA1_bin(const QoreListNode *params, ExceptionSink *xsink) {
   DigestHelper dh;
   if (dh.getData(SHA1_ERR, params, xsink) || dh.doDigest(SHA1_ERR, EVP_sha1(), xsink))
      return 0;
   
   return dh.getBinary();
}

static AbstractQoreNode *f_SHA224_bin(const QoreListNode *params, ExceptionSink *xsink) {
#if !defined(OPENSSL_NO_SHA256) && defined(HAVE_OPENSSL_SHA512)
   DigestHelper dh;
   if (dh.getData(SHA224_ERR, params, xsink) || dh.doDigest(SHA224_ERR, EVP_sha224(), xsink))
      return 0;
   
   return dh.getBinary();
#else
   xsink->raiseException("MISSING-FEATURE-ERROR", "the openssl library version that qore was compiled with did not support the SHA224 algorithm; for maximum portability, check Option::HAVE_SHA224 before calling this function");
   return 0;
#endif
}

static AbstractQoreNode *f_SHA256_bin(const QoreListNode *params, ExceptionSink *xsink) {
#if !defined(OPENSSL_NO_SHA256) && defined(HAVE_OPENSSL_SHA512)
   DigestHelper dh;
   if (dh.getData(SHA256_ERR, params, xsink) || dh.doDigest(SHA256_ERR, EVP_sha256(), xsink))
      return 0;
   
   return dh.getBinary();
#else
   xsink->raiseException("MISSING-FEATURE-ERROR", "the openssl library version that qore was compiled with did not support the SHA256 algorithm; for maximum portability, check Option::HAVE_SHA256 before calling this function");
   return 0;
#endif
}

static AbstractQoreNode *f_SHA384_bin(const QoreListNode *params, ExceptionSink *xsink) {
#if !defined(OPENSSL_NO_SHA512) && defined(HAVE_OPENSSL_SHA512)
   DigestHelper dh;
   if (dh.getData(SHA384_ERR, params, xsink) || dh.doDigest(SHA384_ERR, EVP_sha384(), xsink))
      return 0;
   
   return dh.getBinary();
#else
   xsink->raiseException("MISSING-FEATURE-ERROR", "the openssl library version that qore was compiled with did not support the SHA384 algorithm; for maximum portability, check Option::HAVE_SHA384 before calling this function");
   return 0;
#endif
}

static AbstractQoreNode *f_SHA512_bin(const QoreListNode *params, ExceptionSink *xsink) {
#if !defined(OPENSSL_NO_SHA512) && defined(HAVE_OPENSSL_SHA512)
   DigestHelper dh;
   if (dh.getData(SHA512_ERR, params, xsink) || dh.doDigest(SHA512_ERR, EVP_sha512(), xsink))
      return 0;
   
   return dh.getBinary();
#else
   xsink->raiseException("MISSING-FEATURE-ERROR", "the openssl library version that qore was compiled with did not support the SHA512 algorithm; for maximum portability, check Option::HAVE_SHA512 before calling this function");
   return 0;
#endif
}

static AbstractQoreNode *f_DSS_bin(const QoreListNode *params, ExceptionSink *xsink) {
   DigestHelper dh;
   if (dh.getData(DSS_ERR, params, xsink) || dh.doDigest(DSS_ERR, EVP_dss(), xsink))
      return 0;
   
   return dh.getBinary();
}

static AbstractQoreNode *f_DSS1_bin(const QoreListNode *params, ExceptionSink *xsink) {
   DigestHelper dh;
   if (dh.getData(DSS1_ERR, params, xsink) || dh.doDigest(DSS1_ERR, EVP_dss1(), xsink))
      return 0;
   
   return dh.getBinary();
}

static AbstractQoreNode *f_MDC2_bin(const QoreListNode *params, ExceptionSink *xsink) {
#ifndef OPENSSL_NO_MDC2
   DigestHelper dh;
   if (dh.getData(MDC2_ERR, params, xsink) || dh.doDigest(MDC2_ERR, EVP_mdc2(), xsink))
      return 0;
   
   return dh.getBinary();
#else
   xsink->raiseException("MISSING-FEATURE-ERROR", "the openssl library version that qore was compiled with did not support the MDC2 algorithm; for maximum portability, check Option::HAVE_MDC2 before calling this function");
   return 0;
#endif
}

static AbstractQoreNode *f_RIPEMD160_bin(const QoreListNode *params, ExceptionSink *xsink) {
   DigestHelper dh;
   if (dh.getData(RIPEMD160_ERR, params, xsink) || dh.doDigest(RIPEMD160_ERR, EVP_ripemd160(), xsink))
      return 0;
   
   return dh.getBinary();
}

static AbstractQoreNode *f_des_random_key(const QoreListNode *params, ExceptionSink *xsink) {
   DES_cblock *db = (DES_cblock *)malloc(sizeof(DES_cblock));
   DES_random_key(db);
   return new BinaryNode(db, sizeof(DES_cblock));
}

void init_crypto_functions() {
   builtinFunctions.add("blowfish_encrypt_cbc", f_blowfish_encrypt_cbc);
   builtinFunctions.add("blowfish_decrypt_cbc", f_blowfish_decrypt_cbc);
   builtinFunctions.add("blowfish_decrypt_cbc_to_string", f_blowfish_decrypt_cbc_to_string);

   builtinFunctions.add("des_encrypt_cbc", f_des_encrypt_cbc);
   builtinFunctions.add("des_decrypt_cbc", f_des_decrypt_cbc);
   builtinFunctions.add("des_decrypt_cbc_to_string", f_des_decrypt_cbc_to_string);

   builtinFunctions.add("des_ede_encrypt_cbc", f_des_ede_encrypt_cbc);
   builtinFunctions.add("des_ede_decrypt_cbc", f_des_ede_decrypt_cbc);
   builtinFunctions.add("des_ede_decrypt_cbc_to_string", f_des_ede_decrypt_cbc_to_string);

   builtinFunctions.add("des_ede3_encrypt_cbc", f_des_ede3_encrypt_cbc);
   builtinFunctions.add("des_ede3_decrypt_cbc", f_des_ede3_decrypt_cbc);
   builtinFunctions.add("des_ede3_decrypt_cbc_to_string", f_des_ede3_decrypt_cbc_to_string);

   builtinFunctions.add("desx_encrypt_cbc", f_desx_encrypt_cbc);
   builtinFunctions.add("desx_decrypt_cbc", f_desx_decrypt_cbc);
   builtinFunctions.add("desx_decrypt_cbc_to_string", f_desx_decrypt_cbc_to_string);

   builtinFunctions.add("rc4_encrypt", f_rc4_encrypt);
   builtinFunctions.add("rc4_decrypt", f_rc4_decrypt);
   builtinFunctions.add("rc4_decrypt_to_string", f_rc4_decrypt_to_string);

   builtinFunctions.add("rc2_encrypt_cbc", f_rc2_encrypt_cbc);
   builtinFunctions.add("rc2_decrypt_cbc", f_rc2_decrypt_cbc);
   builtinFunctions.add("rc2_decrypt_cbc_to_string", f_rc2_decrypt_cbc_to_string);

   builtinFunctions.add("cast5_encrypt_cbc", f_cast5_encrypt_cbc);
   builtinFunctions.add("cast5_decrypt_cbc", f_cast5_decrypt_cbc);
   builtinFunctions.add("cast5_decrypt_cbc_to_string", f_cast5_decrypt_cbc_to_string);

   builtinFunctions.add("rc5_encrypt_cbc", f_rc5_encrypt_cbc);
   builtinFunctions.add("rc5_decrypt_cbc", f_rc5_decrypt_cbc);
   builtinFunctions.add("rc5_decrypt_cbc_to_string", f_rc5_decrypt_cbc_to_string);

   // digest functions
   builtinFunctions.add("MD2",           f_MD2);
   builtinFunctions.add("MD2_bin",       f_MD2_bin);
   builtinFunctions.add("MD4",           f_MD4);
   builtinFunctions.add("MD4_bin",       f_MD4_bin);
   builtinFunctions.add("MD5",           f_MD5);
   builtinFunctions.add("MD5_bin",       f_MD5_bin);
   builtinFunctions.add("SHA",           f_SHA);
   builtinFunctions.add("SHA_bin",       f_SHA_bin);
   builtinFunctions.add("SHA1",          f_SHA1);
   builtinFunctions.add("SHA1_bin",      f_SHA1_bin);

   builtinFunctions.add("SHA224",        f_SHA224);
   builtinFunctions.add("SHA224_bin",    f_SHA224_bin);
   builtinFunctions.add("SHA256",        f_SHA256);
   builtinFunctions.add("SHA256_bin",    f_SHA256_bin);
   builtinFunctions.add("SHA384",        f_SHA384);
   builtinFunctions.add("SHA384_bin",    f_SHA384_bin);
   builtinFunctions.add("SHA512",        f_SHA512);
   builtinFunctions.add("SHA512_bin",    f_SHA512_bin);

   builtinFunctions.add("DSS",           f_DSS);
   builtinFunctions.add("DSS_bin",       f_DSS_bin);
   builtinFunctions.add("DSS1",          f_DSS1);
   builtinFunctions.add("DSS1_bin",      f_DSS1_bin);

   builtinFunctions.add("MDC2",          f_MDC2);
   builtinFunctions.add("MDC2_bin",      f_MDC2_bin);

   builtinFunctions.add("RIPEMD160",     f_RIPEMD160);
   builtinFunctions.add("RIPEMD160_bin", f_RIPEMD160_bin);

   // other functions
   builtinFunctions.add("des_random_key", f_des_random_key);
}
