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

#include <openssl/evp.h>

#include <stdio.h>
#include <stdlib.h>

static unsigned char def_iv[] = { 0, 0, 0, 0, 0, 0, 0, 0 };

static unsigned char *encrypt_intern(const EVP_CIPHER *type, char *cipher, class QoreNode *params, int *size, ExceptionSink *xsink)
{
   unsigned char *buf = NULL;
   int len = 0;

   class QoreNode *pt = get_param(params, 0);
   if (pt)
      if (pt->type == NT_STRING)
      {
	 buf = (unsigned char *)pt->val.String->getBuffer();
	 len = pt->val.String->strlen() + 1;
      }
      else if (pt->type == NT_BINARY)
      {
	 buf = (unsigned char *)pt->val.bin->getPtr();
	 len = pt->val.bin->size();
      }
   
   if (!buf || !len)
      return NULL;

   // get key and IV
   unsigned char *key;
   int keylen = 0;
   pt = get_param(params, 1);
   if (!is_nothing(pt))
   {
      if (pt->type == NT_STRING)
      {
	 key = (unsigned char *)pt->val.String->getBuffer();
	 keylen = pt->val.String->strlen();
      }
      else if (pt->type == NT_BINARY)
      {
	 key = (unsigned char *)pt->val.bin->getPtr();
	 keylen = pt->val.bin->size();
      }
      else
      {
	 xsink->raiseException("ENCRYPT-ERROR", "can't use type '%s' as a key value for the %s cipher", pt->type->name, cipher);
	 return NULL;
      }
   }
   else
   {
      xsink->raiseException("ENCRYPT-ERROR", "missing key parameter for %s cipher", cipher);
      return NULL;
   }

   unsigned char *iv;
   pt = get_param(params, 2);
   if (!is_nothing(pt))
   {
      if (pt->type == NT_STRING)
      {
	 if (pt->val.String->strlen() < 8)
	 {
	    xsink->raiseException("ENCRYPT-ERROR", "%s input vector must be at least 8 bytes long (%d)", cipher, pt->val.String->strlen());
	    return NULL;
	 }
	 iv = (unsigned char *)pt->val.String->getBuffer();
      }
      else if (pt->type == NT_BINARY)
      {
	 if (pt->val.bin->size() < 16)
	 {
	    xsink->raiseException("ENCRYPT-ERROR", "%s input vector must be at least 8 bytes long (%d)", cipher, pt->val.bin->size());
	    return NULL;
	 }
	 iv = (unsigned char *)pt->val.bin->getPtr();	 
      }
      else
      {
	 xsink->raiseException("ENCRYPT-ERROR", "can't use type '%s' as an input vector value for the %s cipher", pt->type->name, cipher);
	 return NULL;
      }
   }
   else
      iv = def_iv;

   EVP_CIPHER_CTX ctx;
   EVP_CIPHER_CTX_init(&ctx);
   EVP_EncryptInit_ex(&ctx, type, NULL, NULL, NULL);
   if (key)
   {
      if (keylen > EVP_MAX_KEY_LENGTH)
	 keylen = EVP_MAX_KEY_LENGTH;

      if (!EVP_CIPHER_CTX_set_key_length(&ctx, keylen) || !EVP_EncryptInit_ex(&ctx, NULL, NULL, key, iv))
      {
	 xsink->raiseException("ENCRYPT-ERROR", "%s: error setting key length=%d", cipher, keylen);
	 EVP_CIPHER_CTX_cleanup(&ctx);
	 return NULL;
      }
   }

   int clen = len + (EVP_MAX_BLOCK_LENGTH * 2) - 1;
   unsigned char *cbuf = (unsigned char *)malloc(sizeof(char) * clen);
   
   if (!EVP_EncryptUpdate(&ctx, cbuf, &clen, buf, len))
   {
      xsink->raiseException("ENCRYPT-ERROR", "%s encryption error", cipher);
      EVP_CIPHER_CTX_cleanup(&ctx);
      free(cbuf);
      return NULL;
   }

   int tmplen;
   // Buffer passed to EVP_EncryptFinal() must be after data just encrypted to avoid overwriting it.
   if (!EVP_EncryptFinal_ex(&ctx, cbuf + clen, &tmplen))
   {
      xsink->raiseException("ENCRYPT-ERROR", "%s encryption error", cipher);
      EVP_CIPHER_CTX_cleanup(&ctx);
      free(cbuf);
      return NULL;
   }

   EVP_CIPHER_CTX_cleanup(&ctx);
   (*size) = clen + tmplen;
   //printd(5, "encrypt %s: in=%08x (%d) out=%08x (%d)\n", cipher, buf, len, cbuf, *size);
   return cbuf;
}

static unsigned char *decrypt_intern(const EVP_CIPHER *type, char *cipher, class QoreNode *params, int *size, ExceptionSink *xsink)
{
   unsigned char *buf = NULL;
   int len = 0;
   
   class QoreNode *pt = get_param(params, 0);
   if (pt)
      if (pt->type == NT_STRING)
      {
	 buf = (unsigned char *)pt->val.String->getBuffer();
	 len = pt->val.String->strlen() + 1;
      }
      else if (pt->type == NT_BINARY)
      {
	 buf = (unsigned char *)pt->val.bin->getPtr();
	 len = pt->val.bin->size();
      }
   
   if (!buf || !len)
      return NULL;

   // get key and IV
   unsigned char *key = NULL;
   int keylen = 0;
   pt = get_param(params, 1);
   if (!is_nothing(pt))
   {
      if (pt->type == NT_STRING)
      {
	 key = (unsigned char *)pt->val.String->getBuffer();
	 keylen = pt->val.String->strlen();
      }
      else if (pt->type == NT_BINARY)
      {
	 key = (unsigned char *)pt->val.bin->getPtr();
	 keylen = pt->val.bin->size();
      }
      else
      {
	 xsink->raiseException("DECRYPT-ERROR", "can't use type '%s' as a key value for the %s cipher", pt->type->name, cipher);
	 return NULL;
      }
   }
   else
   {
      xsink->raiseException("ENCRYPT-ERROR", "missing key parameter for %s cipher", cipher);
      return NULL;
   }

   unsigned char *iv;
   pt = get_param(params, 2);
   if (!is_nothing(pt))
   {
      if (pt->type == NT_STRING)
      {
	 if (pt->val.String->strlen() < 8)
	 {
	    xsink->raiseException("DECRYPT-ERROR", "%s input vector must be at least 8 bytes long (%d)", cipher, pt->val.String->strlen());
	    return NULL;
	 }
	 iv = (unsigned char *)pt->val.String->getBuffer();
      }
      else if (pt->type == NT_BINARY)
      {
	 if (pt->val.bin->size() < 16)
	 {
	    xsink->raiseException("DECRYPT-ERROR", "%s input vector must be at least 8 bytes long (%d)", cipher, pt->val.bin->size());
	    return NULL;
	 }
	 iv = (unsigned char *)pt->val.bin->getPtr();	 
      }
      else
      {
	 xsink->raiseException("DECRYPT-ERROR", "can't use type '%s' as an input vector value for the %s cipher", pt->type->name, cipher);
	 return NULL;
      }
   }
   else
      iv = def_iv;

   EVP_CIPHER_CTX ctx;
   EVP_CIPHER_CTX_init(&ctx);
   EVP_DecryptInit_ex(&ctx, type, NULL, NULL, NULL);
   if (key)
   {
      if (keylen > EVP_MAX_KEY_LENGTH)
	 keylen = EVP_MAX_KEY_LENGTH;

      if (!EVP_CIPHER_CTX_set_key_length(&ctx, keylen) || !EVP_DecryptInit_ex(&ctx, NULL, NULL, key, iv))
      {
	 xsink->raiseException("DECRYPT-ERROR", "%s: error setting key length=%d", cipher, keylen);
	 EVP_CIPHER_CTX_cleanup(&ctx);
	 return NULL;
      }
   }

   int clen = len + (EVP_MAX_BLOCK_LENGTH * 2);
   unsigned char *cbuf = (unsigned char *)malloc(sizeof(char) * clen);

   if (!EVP_DecryptUpdate(&ctx, cbuf, &clen, buf, len))
   {
      xsink->raiseException("DECRYPT-ERROR", "error decrypting %s data", cipher);
      EVP_CIPHER_CTX_cleanup(&ctx);
      free(cbuf);
      return NULL;
   }

   int tmplen = 0;
   // Buffer passed to EVP_DecryptFinal() must be after data just decrypted to avoid overwriting it.
   if (!EVP_DecryptFinal_ex(&ctx, cbuf + clen, &tmplen))
   {
      xsink->raiseException("DECRYPT-ERROR", "error decrypting %s data", cipher);
      EVP_CIPHER_CTX_cleanup(&ctx);
      free(cbuf);
      return NULL;
   }

   EVP_CIPHER_CTX_cleanup(&ctx);
   (*size) = clen + tmplen;
   //printd(5, "decrypt %s: in=%08x (%d) out=%08x (%d)\n", cipher, buf, len, cbuf, *size);
   return cbuf;
}


// params (data, [key, [input_vector]])
static class QoreNode *f_blowfish_encrypt_cbc(class QoreNode *params, ExceptionSink *xsink)
{
   int size;
   unsigned char *buf = encrypt_intern(EVP_bf_cbc(), "blowfish", params, &size, xsink);
   if (!buf)
      return NULL;

   return new QoreNode(new BinaryObject(buf, size));
}

static class QoreNode *f_blowfish_decrypt_cbc(class QoreNode *params, ExceptionSink *xsink)
{
   int size;
   unsigned char *buf = decrypt_intern(EVP_bf_cbc(), "blowfish", params, &size, xsink);
   if (!buf)
      return NULL;

   return new QoreNode(new BinaryObject(buf, size));
}

static class QoreNode *f_blowfish_decrypt_cbc_to_string(class QoreNode *params, ExceptionSink *xsink)
{
   int size;
   unsigned char *buf = decrypt_intern(EVP_bf_cbc(), "blowfish", params, &size, xsink);
   if (!buf)
      return NULL;

   class QoreString *str = new QoreString();
   str->take((char *)buf);
   return new QoreNode(str);
}

void init_crypto_functions()
{
   builtinFunctions.add("blowfish_encrypt_cbc", f_blowfish_encrypt_cbc);
   builtinFunctions.add("blowfish_decrypt_cbc", f_blowfish_decrypt_cbc);
   builtinFunctions.add("blowfish_decrypt_cbc_to_string", f_blowfish_decrypt_cbc_to_string);
}
