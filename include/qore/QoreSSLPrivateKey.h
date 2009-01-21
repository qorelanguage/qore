/*
  QC_SSLPrivateKey.h
  
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

#ifndef _QORE_QORESSLPRIVATEKEY_H

#define _QORE_QORESSLPRIVATEKEY_H

#include <openssl/ssl.h>
#include <openssl/evp.h>

//! provides access to a private key data structure for SSL connections
class QoreSSLPrivateKey : public AbstractPrivateData
{
   private:
      // the private implementation of the class
      struct qore_sslpk_private *priv; 

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL QoreSSLPrivateKey(const QoreSSLPrivateKey&);

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL QoreSSLPrivateKey& operator=(const QoreSSLPrivateKey&);

   protected:
      DLLLOCAL virtual ~QoreSSLPrivateKey();

   public:
      //! returns a string in PEM format representing the private key, caller owns the QoreString reference count returned
      /** @return a string in PEM format representing the private key, caller owns the QoreString reference count returned
       */
      DLLEXPORT class QoreStringNode *getPEM(class ExceptionSink *xsink) const;

      DLLLOCAL QoreSSLPrivateKey(EVP_PKEY *p);
      DLLLOCAL QoreSSLPrivateKey(const char *fn, char *pp, class ExceptionSink *xsink);

      // caller does NOT own the EVP_PKEY returned; "const" cannot be used because of the openssl API does not support it
      DLLLOCAL EVP_PKEY *getData() const;
      DLLLOCAL const char *getType() const;
      DLLLOCAL int64 getVersion() const;

      // returns the length in bits
      DLLLOCAL int64 getBitLength() const;

      // caller owns the QoreHashNode reference count returned
      DLLLOCAL class QoreHashNode *getInfo() const;
};

#endif
