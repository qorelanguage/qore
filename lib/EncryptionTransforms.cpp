/* indent-tabs-mode: nil -*- */
/*
  EncryptionTransforms.cpp

  Qore Programming Language

  Copyright (C) 2016 - 2017 Qore Technologies, s.r.o.

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

#include <zlib.h>
#include <bzlib.h>
#include <errno.h>

#include "qore/Qore.h"
#include "qore/intern/EncryptionTransforms.h"

#include <map>
#include <string>

#include <openssl/evp.h>
#include <openssl/des.h>
#include <openssl/hmac.h>

struct CryptoEntry {
   // the length of the key; 0 = variable length key
   unsigned key_len;
   // the OpenSSL cipher type
   const EVP_CIPHER* cipher_type;
   // the initialization vector length
   int iv_len;
   // does the algorithm use Galois Counter Mode (GCM)?
   bool gcm;
};

typedef std::map<std::string, CryptoEntry> crypto_map_t;

crypto_map_t crypto_map = {
   {"blowfish", {0, EVP_bf_cbc(), 8, false}},
   {"des", {8, EVP_des_cbc(), 8, false}},
   {"desede", {16, EVP_des_ede_cbc(), 8, false}},
   {"desede3", {24, EVP_des_ede3_cbc(), 8, false}},
   {"desx", {24, EVP_desx_cbc(), 8, false}},
   {"rc2", {0, EVP_rc2_cbc(), 8, false}},
   {"rc4", {0, EVP_rc4(), 8, false}},
#ifndef OPENSSL_NO_RC5
   {"rc5", {0, EVP_rc5_32_12_16_cbc(), 8, false}},
#endif
   {"cast5", {0, EVP_cast5_cbc(), 8, false}},

   // AES ciphers
   {"aes128", {16, EVP_aes_128_gcm(), 0, true}},
   {"aes192", {24, EVP_aes_192_gcm(), 0, true}},
   {"aes256", {32, EVP_aes_256_gcm(), 0, true}},
};

class CryptoTransform : public Transform {
public:
   DLLLOCAL CryptoTransform(const char* cipher, bool do_crypt, const char* key, unsigned key_len, const char* iv, unsigned iv_len, const char* mac, unsigned mac_len, unsigned tag_length, const ReferenceNode* mac_ref, const char* aad, unsigned aad_len, ExceptionSink* xsink) : cipher(cipher), err(do_crypt ? "ENCRYPT-ERROR" : "DECRYPT-ERROR"), tag_length(tag_length), mac_ref(mac_ref ? mac_ref->refRefSelf() : nullptr), do_crypt(do_crypt) {
      // if encrypting, we can't send a mac
      assert(!(do_crypt && (mac || mac_len)));
      // if decrypting, then we can't send tag or AAD
      assert(!(!do_crypt && (tag_length || mac_ref)));

      crypto_map_t::iterator i = crypto_map.find(cipher);
      if (i == crypto_map.end()) {
         xsink->raiseException(err, "unknown %scryption algorithm '%s'", do_crypt ? "en" : "de", cipher);
         return;
      }

      ce = i->second;

      if (iv && ce.iv_len && iv_len < ce.iv_len) {
         xsink->raiseException(err, "cannot create a %scryption transformation object for algorithm '%s' with an initialization vector of size %d; %d bytes are required", do_crypt ? "en" : "de", cipher, iv_len, ce.iv_len);
         return;
      }

      if (key_len < ce.key_len) {
         xsink->raiseException(err, "%scryption algorithm '%s' requires a key length of %d bytes; got %d byte%s", do_crypt ? "en" : "de", cipher, ce.key_len, key_len, key_len == 1 ? "" : "s");
         return;
      }

      if (ce.key_len && ce.key_len != key_len)
         key_len = ce.key_len;

      EVP_CIPHER_CTX_init(&ctx);
      state = STATE_OK;

      EVP_CipherInit_ex(&ctx, ce.cipher_type, nullptr, nullptr, nullptr, do_crypt);

      if (iv && ce.gcm) {
         if (!EVP_CIPHER_CTX_ctrl(&ctx, EVP_CTRL_GCM_SET_IVLEN, iv_len, nullptr)) {
	    xsink->raiseException(err, "error setting %s initialization vector length: %d", cipher, iv_len);
            state = STATE_ERROR;
	    return;
         }
      }

      if (key_len) {
	 if (key_len > EVP_MAX_KEY_LENGTH)
	    key_len = EVP_MAX_KEY_LENGTH;

	 if (!EVP_CIPHER_CTX_set_key_length(&ctx, key_len) || !EVP_CipherInit_ex(&ctx, nullptr, nullptr, (const unsigned char*)key, (const unsigned char*)iv, -1)) {
            // should not happen
	    xsink->raiseException(err, "error setting %s key length: %d", cipher, key_len);
            state = STATE_ERROR;
	    return;
	 }
      }

      // set AAD if applicable
      if (ce.gcm) {
         if (aad) {
            int outlen;
            if (!EVP_CipherUpdate(&ctx, nullptr, &outlen, (unsigned char*)aad, aad_len)) {
               xsink->raiseException(err, "error setting %s AAD length: %d", cipher, aad_len);
               state = STATE_ERROR;
               return;
            }
         }

         // set the MAC
         if (!do_crypt && mac) {
            if (!EVP_CIPHER_CTX_ctrl(&ctx, EVP_CTRL_GCM_SET_TAG, mac_len, (void*)mac)) {
               xsink->raiseException(err, "error setting %s tag (MAC) length: %d", cipher, mac_len);
               state = STATE_ERROR;
               return;
            }
         }
      }
   }

   DLLLOCAL ~CryptoTransform() {
      if (state != STATE_NOT_INIT) {
         EVP_CIPHER_CTX_cleanup(&ctx);
      }
      if (mac_ref) {
         ExceptionSink xsink;
         mac_ref->deref(&xsink);
         xsink.clear();
      }
   }

   DLLLOCAL std::pair<int64, int64> apply(const void* src, int64 srcLen, void* dst, int64 dstLen, ExceptionSink* xsink) {
      if (state == STATE_DONE)
         return std::make_pair(0, 0);

      if (state != STATE_OK) {
         xsink->raiseException(err, "invalid encryption stream state");
         return std::make_pair(0, 0);
      }

      if (!src) {
         // if the input is done
         int tmplen;
         if (!EVP_CipherFinal_ex(&ctx, (unsigned char*)dst, &tmplen)) {
            xsink->raiseException(err, "error %scrypting final %s block", do_crypt ? "en" : "de", cipher);
            state = STATE_ERROR;
            return std::make_pair(0, 0);
         }
         //printd(0, "src: %p srlen: %d dst: %p dstLen: %d tmplen: %d\n", src, srcLen, dst, dstLen, tmplen);

         // get mac and write to MAC reference, if any
         if (ce.gcm && do_crypt && mac_ref && tag_length) {
            SimpleRefHolder<BinaryNode> mac(new BinaryNode);
            mac->preallocate(tag_length);
            if (!EVP_CIPHER_CTX_ctrl(&ctx, EVP_CTRL_GCM_GET_TAG, tag_length, const_cast<void*>(mac->getPtr()))) {
               xsink->raiseException(err, "error getting %s tag (MAC) length: %d", cipher, tag_length);
               state = STATE_ERROR;
               return std::make_pair(0, 0);
            }

            // write to reference
            QoreTypeSafeReferenceHelper rh(mac_ref, xsink);
            if (rh)
               rh.assign(mac.release());
            if (*xsink) {
               state = STATE_ERROR;
               return std::make_pair(0, 0);
            }
         }

         state = STATE_DONE;
         return std::make_pair(0, tmplen);
      }
      assert(srcLen);

      int outlen = dstLen;
      if (!EVP_CipherUpdate(&ctx, (unsigned char*)dst, &outlen, (unsigned char*)src, srcLen)) {
         xsink->raiseException(err, "error %scrypting %s block", do_crypt ? "en" : "de", cipher);
         state = STATE_ERROR;
         return std::make_pair(0, 0);
      }

      return std::make_pair(srcLen, outlen);
   }

   DLLLOCAL virtual size_t outputBufferSize() {
      return BUFSIZE + (EVP_MAX_BLOCK_LENGTH * 2);
   }

   DLLLOCAL virtual size_t inputBufferSize() {
      return BUFSIZE;
   }

private:
   DLLLOCAL static constexpr size_t BUFSIZE = 4096;

   const char* cipher;
   CryptoEntry ce;
   EVP_CIPHER_CTX ctx;
   const char* err;
   enum State {
      STATE_OK, STATE_DONE, STATE_ERROR, STATE_NOT_INIT
   };

   State state = STATE_NOT_INIT;

   // number of bytes in MAC requested
   unsigned tag_length;
   // reference to store the MAC
   ReferenceNode* mac_ref;

   bool do_crypt;
};

Transform* EncryptionTransforms::getCryptoTransform(const char* cipher, bool do_crypt, const char* key, unsigned key_len, const char* iv, unsigned iv_len, const char* mac, unsigned mac_len, unsigned tag_length, const ReferenceNode* mac_ref, const char* aad, unsigned aad_len, ExceptionSink* xsink) {
   ReferenceHolder<Transform> rv(new CryptoTransform(cipher, do_crypt, key, key_len, iv, iv_len, mac, mac_len, tag_length, mac_ref, aad, aad_len, xsink), xsink);
   return *xsink ? nullptr : rv.release();
}
