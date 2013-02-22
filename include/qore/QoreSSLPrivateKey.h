/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QC_SSLPrivateKey.h
  
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

#ifndef _QORE_QORESSLPRIVATEKEY_H

#define _QORE_QORESSLPRIVATEKEY_H

#include <openssl/ssl.h>
#include <openssl/evp.h>

//! provides access to a private key data structure for SSL connections
class QoreSSLPrivateKey : public AbstractPrivateData {
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
   //! creates the object from a file name by reading in the file in PEM format
   /** @param fn the filename of the private key file to open (must be in PEM format)
       @param pp the pass phase for the key (if any; may be NULL)
       @param xsink Qore-language exceptions are raised here in case of errors
   */
   DLLEXPORT QoreSSLPrivateKey(const char *fn, const char *pp, ExceptionSink *xsink);

   //! creates the object from a pointer to a BinaryNode object (key data in DER format)
   /** @param bin a pointer to a BinaryNode object with the raw binary private key information
       @param xsink Qore-language exceptions are raised here in case of errors
   */
   DLLEXPORT QoreSSLPrivateKey(const BinaryNode *bin, ExceptionSink *xsink);

   //! create the object from a pointer to a QoreString representing the private key data in PEM format
   /** @param str a pointer to a QoreString with the private key data in PEM format
       @param pp the pass phase for the key (if any; may be NULL)
       @param xsink Qore-language exceptions are raised here in case of errors
   */
   DLLEXPORT QoreSSLPrivateKey(const QoreString *str, const char *pp, ExceptionSink *xsink);

   //! returns a string in PEM format representing the private key, caller owns the QoreString reference count returned
   /** @return a string in PEM format representing the private key, caller owns the QoreString reference count returned
    */
   DLLEXPORT QoreStringNode *getPEM(ExceptionSink *xsink) const;

   // caller does NOT own the EVP_PKEY returned; "const" cannot be used because of the openssl API does not support it
   DLLEXPORT EVP_PKEY *getData() const;

   DLLEXPORT const char *getType() const;

   //! returns a constant '1': do not use; only included for backwards-compatibility
   DLLEXPORT int64 getVersion() const;

   //! returns the length in bits
   DLLEXPORT int64 getBitLength() const;

   //! caller owns the QoreHashNode reference count returned
   DLLEXPORT QoreHashNode *getInfo() const;

   //! private constructor; not exported
   DLLLOCAL QoreSSLPrivateKey(EVP_PKEY *p);
};

#endif
