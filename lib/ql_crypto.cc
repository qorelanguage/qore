/*
  ql_crypto.cc
  
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

#include <qore/config.h>
#include <qore/ql_crypto.h>
#include <qore/QoreNode.h>
#include <qore/support.h>
#include <qore/QoreString.h>
#include <qore/params.h>
#include <qore/BuiltinFunctionList.h>
#include <qore/BinaryObject.h>

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

      int getInput(char *err, class QoreNode *params, class ExceptionSink *xsink, bool include_null = true)
      {
	 class QoreNode *pt = get_param(params, 0);

	 if (is_nothing(pt))
	 {
	    xsink->raiseException(err, "missing data (string or binary) parameter to function");
	    return -1;
	 }
	 if (pt->type == NT_STRING)
	 {
	    input = (unsigned char *)pt->val.String->getBuffer();
	    input_len = pt->val.String->strlen() + include_null;
	    return 0;
	 }	 
	 if (pt->type == NT_BINARY)
	 {
	    input = (unsigned char *)pt->val.bin->getPtr();
	    input_len = pt->val.bin->size();
	    return 0;
	 }
	 
	 xsink->raiseException(err, "don't know how to process type '%s' (expecing string or binary)", pt->type->name);
	 return -1;
      }      
};

class DigestHelper : public BaseHelper
{
   private:
      unsigned char md_value[EVP_MAX_MD_SIZE];
      unsigned int md_len;

   public:
      inline int getData(char *err, class QoreNode *params, class ExceptionSink *xsink)
      {
	 return getInput(err, params, xsink, false);
      }
      
      int doDigest(char *err, const EVP_MD *md, class ExceptionSink *xsink)
      {
	 EVP_MD_CTX mdctx;
	 EVP_MD_CTX_init(&mdctx);
	 
	 EVP_DigestInit_ex(&mdctx, md, NULL);

	 if (!EVP_DigestUpdate(&mdctx, input, input_len) || !EVP_DigestFinal_ex(&mdctx, md_value, &md_len))
	 {
	    EVP_MD_CTX_cleanup(&mdctx);
	    xsink->raiseException(err, "error calculating digest");
	    return -1;
	 }

	 EVP_MD_CTX_cleanup(&mdctx);
	 return 0;
      }

      class QoreString *getString()
      {
	 class QoreString *str = new QoreString();
	 for (unsigned i = 0; i < md_len; i++)
	    str->sprintf("%02x", md_value[i]);

	 return str;
      }
};

class CryptoHelper : public BaseHelper
{
   private:
      unsigned char *iv, *output;
      int output_len;

      const char *getOrdinal(int n)
      {
	 return n == 1 ? "first" : (n == 2 ? "second" : "third");
      }

      int getKey(char *err, class QoreNode *params, int n, class ExceptionSink *xsink)
      {
	 class QoreNode *pt = get_param(params, n);

	 if (is_nothing(pt))
	 {
	    xsink->raiseException(err, "missing %s key parameter", getOrdinal(n));
	    return -1;
	 }

	 if (pt->type == NT_STRING)
	 {
	    key[n - 1] = (unsigned char *)pt->val.String->getBuffer();
	    keylen[n - 1] = pt->val.String->strlen();
	    return 0;
	 }
	 
	 if (pt->type == NT_BINARY)
	 {
	    key[n - 1] = (unsigned char *)pt->val.bin->getPtr();
	    keylen[n - 1] = pt->val.bin->size();
	    return 0;
	 }

	 xsink->raiseException(err, "can't use type '%s' for %s key value", pt->type->name, getOrdinal(n));
	 return -1;
      }

      // get initialization vector
      int getIV(char *err, class QoreNode *params, int n, class ExceptionSink *xsink)
      {
	 class QoreNode *pt = get_param(params, n);
	 if (is_nothing(pt))
	 {
	    iv = def_iv;
	    return 0;
	 }

	 if (pt->type == NT_STRING)
	 {
	    if (pt->val.String->strlen() < 8)
	    {
	       xsink->raiseException(err, "the input vector must be at least 8 bytes long (%d bytes passed)", pt->val.String->strlen());
	       return -1;
	    }
	    iv = (unsigned char *)pt->val.String->getBuffer();
	    return 0;
	 }

	 if (pt->type == NT_BINARY)
	 {
	    if (pt->val.bin->size() < 8)
	    {
	       xsink->raiseException(err, "the input vector must be at least 8 bytes long (%d bytes passed)", pt->val.bin->size());
	       return -1;
	    }
	    iv = (unsigned char *)pt->val.bin->getPtr();
	    return 0;
	 }

	 xsink->raiseException(err, "can't use type '%s' as an input vector", pt->type->name);
	 return -1;
      }

   public:
      unsigned char *key[3];
      int keylen[3];

      inline CryptoHelper()
      {
	 output = NULL;
      }

      inline ~CryptoHelper()
      {
	 if (output)
	    free(output);
      }

      inline class BinaryObject *getBinary()
      {
	 class BinaryObject *b = new BinaryObject(output, output_len);
	 output = NULL;
	 return b;
      }

      inline class QoreString *getString()
      {
	 class QoreString *str = new QoreString();
	 // terminate string only if necessary
	 if (output[output_len - 1] == '\0')
	    str->take((char *)output, output_len - 1);
	 else
	 {
	    output[output_len] = '\0';
	    str->take((char *)output, output_len);
	 }
	 output = NULL;
	 return str;
      }

      int getSingleKey(char *err, class QoreNode *params, class ExceptionSink *xsink)
      {
	 if (getInput(err, params, xsink) || getKey(err, params, 1, xsink) || getIV(err, params, 2, xsink))
	    return -1;
	 return 0;
      }

      int getTwoKeys(char *err, class QoreNode *params, class ExceptionSink *xsink)
      {
	 if (getInput(err, params, xsink) || getKey(err, params, 1, xsink) || getKey(err, params, 2, xsink) || getIV(err, params, 3, xsink))
	    return -1;
	 return 0;
      }

      int getThreeKeys(char *err, class QoreNode *params, class ExceptionSink *xsink)
      {
	 if (getInput(err, params, xsink) || getKey(err, params, 1, xsink) || getKey(err, params, 2, xsink) || getKey(err, params, 3, xsink) || getIV(err, params, 4, xsink))
	    return -1;
	 return 0;
      }

      int doCipher(const EVP_CIPHER *type, char *cipher, int do_crypt, ExceptionSink *xsink)
      {
	 char *err = (char *)(do_crypt ? "ENCRYPT-ERROR" : "DECRYPT-ERROR");

	 EVP_CIPHER_CTX ctx;
	 EVP_CIPHER_CTX_init(&ctx);
	 EVP_CipherInit_ex(&ctx, type, NULL, NULL, NULL, do_crypt);
	 if (key[0])
	 {
	    if (keylen[0] > EVP_MAX_KEY_LENGTH)
	       keylen[0] = EVP_MAX_KEY_LENGTH;

	    if (!EVP_CIPHER_CTX_set_key_length(&ctx, keylen[0]) || !EVP_CipherInit_ex(&ctx, NULL, NULL, key[0], iv, -1))
	    {
	       xsink->raiseException(err, "error setting %s key length=%d", cipher, keylen[0]);
	       EVP_CIPHER_CTX_cleanup(&ctx);
	       return -1;
	    }
	 }

	 // we allocate 1 byte more than we need in case we return as a string so we can terminate it
	 output = (unsigned char *)malloc(sizeof(char) * (input_len + (EVP_MAX_BLOCK_LENGTH * 2)));
   
	 if (!EVP_CipherUpdate(&ctx, output, &output_len, input, input_len))
	 {
	    xsink->raiseException(err, "error %scrypting %s block", do_crypt ? "en" : "de", cipher);
	    EVP_CIPHER_CTX_cleanup(&ctx);
	    return -1;
	 }

	 int tmplen;
	 // Buffer passed to EVP_EncryptFinal() must be after data just encrypted to avoid overwriting it.
	 if (!EVP_CipherFinal_ex(&ctx, output + output_len, &tmplen))
	 {
	    xsink->raiseException(err, "error %scrypting final %s block", do_crypt ? "en" : "de", cipher);
	    EVP_CIPHER_CTX_cleanup(&ctx);
	    return -1;
	 }

	 EVP_CIPHER_CTX_cleanup(&ctx);
	 output_len += tmplen;
	 //printd(5, "cipher_intern() %s: in=%08p (%d) out=%08p (%d)\n", cipher, buf, len, cbuf, *size);
	 return 0;
      }

      inline int checkKeyLen(char *err, int n, int len, class ExceptionSink *xsink)
      {
	 if (keylen[n] < len)
	 {
	    xsink->raiseException(err, "key length is not %d bytes long (%d bytes)", len, keylen[n]);
	    return -1;
	 }
	 keylen[n] = len;
	 return 0;
      }

      int setDESKey(int n, class ExceptionSink *xsink)
      {

	 // force odd parity 
	 //DES_set_odd_parity((DES_cblock *)key[n]);
	 // populate the schedule structure
	 //DES_set_key_unchecked((DES_cblock *)key[n], schedule);
	 //key[n] = (unsigned char *)schedule;
	 return 0;
      }

};

static class QoreNode *f_blowfish_encrypt_cbc(class QoreNode *params, ExceptionSink *xsink)
{
   class CryptoHelper ch;

   if (ch.getSingleKey("BLOWFISH-ENCRYPT-PARAM-ERROR", params, xsink)
       || ch.doCipher(EVP_bf_cbc(), "blowfish", QC_ENCRYPT, xsink))
      return NULL;

   return new QoreNode(ch.getBinary());
}

static class QoreNode *f_blowfish_decrypt_cbc(class QoreNode *params, ExceptionSink *xsink)
{
   class CryptoHelper ch;

   if (ch.getSingleKey("BLOWFISH-DECRYPT-PARAM-ERROR", params, xsink)
       || ch.doCipher(EVP_bf_cbc(), "blowfish", QC_DECRYPT, xsink))
      return NULL;

   return new QoreNode(ch.getBinary());
}

static class QoreNode *f_blowfish_decrypt_cbc_to_string(class QoreNode *params, ExceptionSink *xsink)
{
   class CryptoHelper ch;

   if (ch.getSingleKey("BLOWFISH-DECRYPT-PARAM-ERROR", params, xsink)
       || ch.doCipher(EVP_bf_cbc(), "blowfish", QC_DECRYPT, xsink))
      return NULL;

   return new QoreNode(ch.getString());
}

static class QoreNode *f_des_encrypt_cbc(class QoreNode *params, ExceptionSink *xsink)
{
   class CryptoHelper ch;

   if (ch.getSingleKey("DES-ENCRYPT-PARAM-ERROR", params, xsink) 
       || ch.checkKeyLen("DES-KEY-ERROR", 0, 8, xsink)
       || ch.doCipher(EVP_des_cbc(), "DES", QC_ENCRYPT, xsink))
	    return NULL;

   return new QoreNode(ch.getBinary());
}

static class QoreNode *f_des_decrypt_cbc(class QoreNode *params, ExceptionSink *xsink)
{
   class CryptoHelper ch;

   if (ch.getSingleKey("DES-DECRYPT-PARAM-ERROR", params, xsink) 
       || ch.checkKeyLen("DES-KEY-ERROR", 0, 8, xsink)
       || ch.doCipher(EVP_des_cbc(), "DES", QC_DECRYPT, xsink))
	    return NULL;

   return new QoreNode(ch.getBinary());
}

static class QoreNode *f_des_decrypt_cbc_to_string(class QoreNode *params, ExceptionSink *xsink)
{
   class CryptoHelper ch;

   if (ch.getSingleKey("DES-DECRYPT-PARAM-ERROR", params, xsink) 
       || ch.checkKeyLen("DES-KEY-ERROR", 0, 8, xsink)
       || ch.doCipher(EVP_des_cbc(), "DES", QC_DECRYPT, xsink))
	    return NULL;

   return new QoreNode(ch.getString());
}

// params (data, [key, [input_vector]])
static class QoreNode *f_des_ede_encrypt_cbc(class QoreNode *params, ExceptionSink *xsink)
{
   class CryptoHelper ch;

   if (ch.getSingleKey("DES-ENCRYPT-PARAM-ERROR", params, xsink) 
       || ch.checkKeyLen("DES-KEY-ERROR", 0, 16, xsink)
       || ch.doCipher(EVP_des_ede_cbc(), "DES", QC_ENCRYPT, xsink))
      return NULL;

   return new QoreNode(ch.getBinary());
}

static class QoreNode *f_des_ede_decrypt_cbc(class QoreNode *params, ExceptionSink *xsink)
{
   class CryptoHelper ch;

   if (ch.getSingleKey("DES-DECRYPT-PARAM-ERROR", params, xsink) 
       || ch.checkKeyLen("DES-KEY-ERROR", 0, 16, xsink)
       || ch.doCipher(EVP_des_ede_cbc(), "DES", QC_DECRYPT, xsink))
      return NULL;

   return new QoreNode(ch.getBinary());
}

static class QoreNode *f_des_ede_decrypt_cbc_to_string(class QoreNode *params, ExceptionSink *xsink)
{
   class CryptoHelper ch;

   if (ch.getSingleKey("DES-DECRYPT-PARAM-ERROR", params, xsink) 
       || ch.checkKeyLen("DES-KEY-ERROR", 0, 16, xsink)
       || ch.doCipher(EVP_des_ede_cbc(), "DES", QC_DECRYPT, xsink))
      return NULL;

   return new QoreNode(ch.getString());
}

// params (data, [key, [input_vector]])
static class QoreNode *f_des_ede3_encrypt_cbc(class QoreNode *params, ExceptionSink *xsink)
{
   class CryptoHelper ch;

   if (ch.getSingleKey("DES-ENCRYPT-PARAM-ERROR", params, xsink) 
       || ch.checkKeyLen("DES-KEY-ERROR", 0, 24, xsink)
       || ch.doCipher(EVP_des_ede3_cbc(), "DES", QC_ENCRYPT, xsink))
      return NULL;

   return new QoreNode(ch.getBinary());
}

static class QoreNode *f_des_ede3_decrypt_cbc(class QoreNode *params, ExceptionSink *xsink)
{
   class CryptoHelper ch;

   if (ch.getSingleKey("DES-DECRYPT-PARAM-ERROR", params, xsink) 
       || ch.checkKeyLen("DES-KEY-ERROR", 0, 24, xsink)
       || ch.doCipher(EVP_des_ede3_cbc(), "DES", QC_DECRYPT, xsink))
      return NULL;

   return new QoreNode(ch.getBinary());
}

static class QoreNode *f_des_ede3_decrypt_cbc_to_string(class QoreNode *params, ExceptionSink *xsink)
{
   class CryptoHelper ch;

   if (ch.getSingleKey("DES-DECRYPT-PARAM-ERROR", params, xsink) 
       || ch.checkKeyLen("DES-KEY-ERROR", 0, 24, xsink)
       || ch.doCipher(EVP_des_ede3_cbc(), "DES", QC_DECRYPT, xsink))
      return NULL;

   return new QoreNode(ch.getString());
}

// params (data, key, [input_vector])
static class QoreNode *f_desx_encrypt_cbc(class QoreNode *params, ExceptionSink *xsink)
{
   class CryptoHelper ch;

   if (ch.getSingleKey("DESX-ENCRYPT-PARAM-ERROR", params, xsink) 
       || ch.checkKeyLen("DESX-KEY-ERROR", 0, 24, xsink)
       || ch.doCipher(EVP_desx_cbc(), "DESX", QC_ENCRYPT, xsink))
      return NULL;

   return new QoreNode(ch.getBinary());
}

static class QoreNode *f_desx_decrypt_cbc(class QoreNode *params, ExceptionSink *xsink)
{
   class CryptoHelper ch;

   if (ch.getSingleKey("DESX-DECRYPT-PARAM-ERROR", params, xsink) 
       || ch.checkKeyLen("DESX-KEY-ERROR", 0, 24, xsink)
       || ch.doCipher(EVP_desx_cbc(), "DESX", QC_DECRYPT, xsink))
      return NULL;

   return new QoreNode(ch.getBinary());
}

static class QoreNode *f_desx_decrypt_cbc_to_string(class QoreNode *params, ExceptionSink *xsink)
{
   class CryptoHelper ch;

   if (ch.getSingleKey("DESX-DECRYPT-PARAM-ERROR", params, xsink) 
       || ch.checkKeyLen("DESX-KEY-ERROR", 0, 24, xsink)
       || ch.doCipher(EVP_desx_cbc(), "DESX", QC_DECRYPT, xsink))
      return NULL;

   return new QoreNode(ch.getString());
}

static class QoreNode *f_rc4_encrypt(class QoreNode *params, ExceptionSink *xsink)
{
   class CryptoHelper ch;

   if (ch.getSingleKey("RC4-ENCRYPT-PARAM-ERROR", params, xsink)
       || ch.doCipher(EVP_rc4(), "rc4", QC_ENCRYPT, xsink))
      return NULL;

   return new QoreNode(ch.getBinary());
}

static class QoreNode *f_rc4_decrypt(class QoreNode *params, ExceptionSink *xsink)
{
   class CryptoHelper ch;

   if (ch.getSingleKey("RC4-DECRYPT-PARAM-ERROR", params, xsink)
       || ch.doCipher(EVP_rc4(), "rc4", QC_DECRYPT, xsink))
      return NULL;

   return new QoreNode(ch.getBinary());
}

static class QoreNode *f_rc4_decrypt_to_string(class QoreNode *params, ExceptionSink *xsink)
{
   class CryptoHelper ch;

   if (ch.getSingleKey("RC4-DECRYPT-PARAM-ERROR", params, xsink)
       || ch.doCipher(EVP_rc4(), "rc4", QC_DECRYPT, xsink))
      return NULL;

   return new QoreNode(ch.getString());
}

static class QoreNode *f_rc2_encrypt_cbc(class QoreNode *params, ExceptionSink *xsink)
{
   class CryptoHelper ch;

   if (ch.getSingleKey("RC2-ENCRYPT-PARAM-ERROR", params, xsink)
       || ch.doCipher(EVP_rc2_cbc(), "rc2", QC_ENCRYPT, xsink))
      return NULL;

   return new QoreNode(ch.getBinary());
}

static class QoreNode *f_rc2_decrypt_cbc(class QoreNode *params, ExceptionSink *xsink)
{
   class CryptoHelper ch;

   if (ch.getSingleKey("RC2-DECRYPT-PARAM-ERROR", params, xsink)
       || ch.doCipher(EVP_rc2_cbc(), "rc2", QC_DECRYPT, xsink))
      return NULL;

   return new QoreNode(ch.getBinary());
}

static class QoreNode *f_rc2_decrypt_cbc_to_string(class QoreNode *params, ExceptionSink *xsink)
{
   class CryptoHelper ch;

   if (ch.getSingleKey("RC2-DECRYPT-PARAM-ERROR", params, xsink)
       || ch.doCipher(EVP_rc2_cbc(), "rc2", QC_DECRYPT, xsink))
      return NULL;

   return new QoreNode(ch.getString());
}

static class QoreNode *f_cast5_encrypt_cbc(class QoreNode *params, ExceptionSink *xsink)
{
   class CryptoHelper ch;

   if (ch.getSingleKey("CAST5-ENCRYPT-PARAM-ERROR", params, xsink)
       || ch.doCipher(EVP_cast5_cbc(), "CAST5", QC_ENCRYPT, xsink))
      return NULL;

   return new QoreNode(ch.getBinary());
}

static class QoreNode *f_cast5_decrypt_cbc(class QoreNode *params, ExceptionSink *xsink)
{
   class CryptoHelper ch;

   if (ch.getSingleKey("CAST5-DECRYPT-PARAM-ERROR", params, xsink)
       || ch.doCipher(EVP_cast5_cbc(), "CAST5", QC_DECRYPT, xsink))
      return NULL;

   return new QoreNode(ch.getBinary());
}

static class QoreNode *f_cast5_decrypt_cbc_to_string(class QoreNode *params, ExceptionSink *xsink)
{
   class CryptoHelper ch;

   if (ch.getSingleKey("CAST5-DECRYPT-PARAM-ERROR", params, xsink)
       || ch.doCipher(EVP_cast5_cbc(), "CAST5", QC_DECRYPT, xsink))
      return NULL;

   return new QoreNode(ch.getString());
}

#ifndef OPENSSL_NO_RC5
static class QoreNode *f_rc5_encrypt_cbc(class QoreNode *params, ExceptionSink *xsink)
{
   class CryptoHelper ch;

   if (ch.getSingleKey("RC5-ENCRYPT-PARAM-ERROR", params, xsink)
       || ch.doCipher(EVP_rc5_32_12_16_cbc(), "rc5", QC_ENCRYPT, xsink))
      return NULL;

   return new QoreNode(ch.getBinary());
}

static class QoreNode *f_rc5_decrypt_cbc(class QoreNode *params, ExceptionSink *xsink)
{
   class CryptoHelper ch;

   if (ch.getSingleKey("RC5-DECRYPT-PARAM-ERROR", params, xsink)
       || ch.doCipher(EVP_rc5_32_12_16_cbc(), "rc5", QC_DECRYPT, xsink))
      return NULL;

   return new QoreNode(ch.getBinary());
}

static class QoreNode *f_rc5_decrypt_cbc_to_string(class QoreNode *params, ExceptionSink *xsink)
{
   class CryptoHelper ch;

   if (ch.getSingleKey("RC5-DECRYPT-PARAM-ERROR", params, xsink)
       || ch.doCipher(EVP_rc5_32_12_16_cbc(), "rc5", QC_DECRYPT, xsink))
      return NULL;

   return new QoreNode(ch.getString());
}
#endif

#define MD2_ERR "MD2-DIGEST-ERROR"
static class QoreNode *f_MD2(class QoreNode *params, ExceptionSink *xsink)
{
   class DigestHelper dh;
   if (dh.getData(MD2_ERR, params, xsink) || dh.doDigest(MD2_ERR, EVP_md2(), xsink))
      return NULL;

   return new QoreNode(dh.getString());
}

#define MD4_ERR "MD4-DIGEST-ERROR"
static class QoreNode *f_MD4(class QoreNode *params, ExceptionSink *xsink)
{
   class DigestHelper dh;
   if (dh.getData(MD4_ERR, params, xsink) || dh.doDigest(MD4_ERR, EVP_md4(), xsink))
      return NULL;

   return new QoreNode(dh.getString());
}

#define MD5_ERR "MD5-DIGEST-ERROR"
static class QoreNode *f_MD5(class QoreNode *params, ExceptionSink *xsink)
{
   class DigestHelper dh;
   if (dh.getData(MD5_ERR, params, xsink) || dh.doDigest(MD5_ERR, EVP_md5(), xsink))
      return NULL;

   return new QoreNode(dh.getString());
}

#define SHA_ERR "SHA-DIGEST-ERROR"
static class QoreNode *f_SHA(class QoreNode *params, ExceptionSink *xsink)
{
   class DigestHelper dh;
   if (dh.getData(SHA_ERR, params, xsink) || dh.doDigest(SHA_ERR, EVP_sha(), xsink))
      return NULL;

   return new QoreNode(dh.getString());
}

#define SHA1_ERR "SHA1-DIGEST-ERROR"
static class QoreNode *f_SHA1(class QoreNode *params, ExceptionSink *xsink)
{
   class DigestHelper dh;
   if (dh.getData(SHA1_ERR, params, xsink) || dh.doDigest(SHA1_ERR, EVP_sha1(), xsink))
      return NULL;

   return new QoreNode(dh.getString());
}

#define DSS_ERR "DSS-DIGEST-ERROR"
static class QoreNode *f_DSS(class QoreNode *params, ExceptionSink *xsink)
{
   class DigestHelper dh;
   if (dh.getData(DSS_ERR, params, xsink) || dh.doDigest(DSS_ERR, EVP_dss(), xsink))
      return NULL;

   return new QoreNode(dh.getString());
}

#define DSS1_ERR "DSS1-DIGEST-ERROR"
static class QoreNode *f_DSS1(class QoreNode *params, ExceptionSink *xsink)
{
   class DigestHelper dh;
   if (dh.getData(DSS1_ERR, params, xsink) || dh.doDigest(DSS1_ERR, EVP_dss1(), xsink))
      return NULL;

   return new QoreNode(dh.getString());
}

#ifndef OPENSSL_NO_MDC2
#define MDC2_ERR "MDC2-DIGEST-ERROR"
static class QoreNode *f_MDC2(class QoreNode *params, ExceptionSink *xsink)
{
   class DigestHelper dh;
   if (dh.getData(MDC2_ERR, params, xsink) || dh.doDigest(MDC2_ERR, EVP_mdc2(), xsink))
      return NULL;

   return new QoreNode(dh.getString());
}
#endif

#define RIPEMD160_ERR "RIPEMD160-DIGEST-ERROR"
static class QoreNode *f_RIPEMD160(class QoreNode *params, ExceptionSink *xsink)
{
   class DigestHelper dh;
   if (dh.getData(RIPEMD160_ERR, params, xsink) || dh.doDigest(RIPEMD160_ERR, EVP_ripemd160(), xsink))
      return NULL;

   return new QoreNode(dh.getString());
}

static class QoreNode *f_des_random_key(class QoreNode *params, ExceptionSink *xsink)
{
   DES_cblock *db = (DES_cblock *)malloc(sizeof(DES_cblock));
   DES_random_key(db);
   return new QoreNode(new BinaryObject(db, sizeof(DES_cblock)));
}

void init_crypto_functions()
{
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

#ifndef OPENSSL_NO_RC5
   builtinFunctions.add("rc5_encrypt_cbc", f_rc5_encrypt_cbc);
   builtinFunctions.add("rc5_decrypt_cbc", f_rc5_decrypt_cbc);
   builtinFunctions.add("rc5_decrypt_cbc_to_string", f_rc5_decrypt_cbc_to_string);
#endif

   // digest functions
   builtinFunctions.add("MD2",       f_MD2);
   builtinFunctions.add("MD4",       f_MD4);
   builtinFunctions.add("MD5",       f_MD5);
   builtinFunctions.add("SHA",       f_SHA);
   builtinFunctions.add("SHA1",      f_SHA1);
   builtinFunctions.add("DSS",       f_DSS);
   builtinFunctions.add("DSS1",      f_DSS1);
#ifndef OPENSSL_NO_MDC2
   builtinFunctions.add("MSC2",      f_MDC2);
#endif
   builtinFunctions.add("RIPEMD160", f_RIPEMD160);

   // other functions
   builtinFunctions.add("des_random_key", f_des_random_key);

}
