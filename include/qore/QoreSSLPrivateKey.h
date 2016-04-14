/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QC_SSLPrivateKey.h
  
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
   DLLEXPORT QoreSSLPrivateKey(const char* fn, const char* pp, ExceptionSink* xsink);

   //! creates the object from a pointer to a BinaryNode object (key data in DER format)
   /** @param bin a pointer to a BinaryNode object with the raw binary private key information
       @param xsink Qore-language exceptions are raised here in case of errors
   */
   DLLEXPORT QoreSSLPrivateKey(const BinaryNode* bin, ExceptionSink* xsink);

   //! create the object from a pointer to a QoreString representing the private key data in PEM format
   /** @param str a pointer to a QoreString with the private key data in PEM format
       @param pp the pass phase for the key (if any; may be NULL)
       @param xsink Qore-language exceptions are raised here in case of errors
   */
   DLLEXPORT QoreSSLPrivateKey(const QoreString* str, const char* pp, ExceptionSink* xsink);

   //! returns a string in PEM format representing the private key, caller owns the QoreString reference count returned
   /** @return a string in PEM format representing the private key, caller owns the QoreString reference count returned
    */
   DLLEXPORT QoreStringNode* getPEM(ExceptionSink* xsink) const;

   // caller does NOT own the EVP_PKEY returned; "const" cannot be used because of the openssl API does not support it
   DLLEXPORT EVP_PKEY* getData() const;

   DLLEXPORT const char* getType() const;

   //! returns a constant '1': do not use; only included for backwards-compatibility
   DLLEXPORT int64 getVersion() const;

   //! returns the length in bits
   DLLEXPORT int64 getBitLength() const;

   //! caller owns the QoreHashNode reference count returned
   DLLEXPORT QoreHashNode* getInfo() const;

   //! caller owns reference returned
   DLLEXPORT QoreSSLPrivateKey* pkRefSelf() const;
   
   //! private constructor; not exported
   DLLLOCAL QoreSSLPrivateKey(EVP_PKEY* p);
};

#endif
