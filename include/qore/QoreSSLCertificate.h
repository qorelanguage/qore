/*
  QoreSSLCertificate.h

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

#ifndef _QORE_QORESSLCERTIFICATE_H

#define _QORE_QORESSLCERTIFICATE_H

#include <qore/QoreSSLBase.h>

#include <openssl/ssl.h>
#include <openssl/pem.h>

//! represents an X509 certificate, reference-counted, dynamically-allocated only
class QoreSSLCertificate : public AbstractPrivateData, public QoreSSLBase
{
   private:
      //! private implementation of the class
      struct qore_sslcert_private *priv; 

      DLLLOCAL AbstractQoreNode *doPurposeValue(int id, int ca) const;

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL QoreSSLCertificate(const QoreSSLCertificate&);

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL QoreSSLCertificate& operator=(const QoreSSLCertificate&);

   protected:
      //! the destructor is protected to ensure that it's only dynamically allocated (use deref() to delete)
      DLLLOCAL virtual ~QoreSSLCertificate();

   public:
      //! creates the object from a pointer to an X509 data structure, the QoreSSLCertificate object takes ownership of the X509 pointer
      /** @param c a pointer to an X509 data structure, the QoreSSLCertificate object takes ownership of the X509 pointer
       */
      DLLEXPORT QoreSSLCertificate(X509 *c);

      //! creates the object from a pointer to a BinaryNode object
      /** @param bin a pointer to a BinaryNode object with the raw binary certificate information
	  @param xsink Qore-language exceptions are raised here in case of errors
       */
      DLLEXPORT QoreSSLCertificate(const BinaryNode *bin, ExceptionSink *xsink);

      //! create the object from a pointer to a QoreString representing the X.509 certificate in PEM format
      /** @param str a pointer to a QoreString with the certificatge in PEM format
	  @param xsink Qore-language exceptions are raised here in case of errors
       */
      DLLEXPORT QoreSSLCertificate(const QoreString *str, ExceptionSink *xsink);

      //! creates the object from a filename
      /** @param fn the filename of the certificate file in PEM format
	  @param xsink Qore-language exceptions are raised here in case of errors
      */
      DLLLOCAL QoreSSLCertificate(const char *fn, ExceptionSink *xsink);

      //! returns true if the object is valid, false if not
      DLLEXPORT operator bool() const;

      //! returns a string in PEM format representing the certificate; caller owns the QoreStringNode reference returned
      /** @return a string in PEM format representing the certificate; caller owns the QoreStringNode reference returned
       */
      DLLEXPORT QoreStringNode *getPEM(ExceptionSink *xsink) const;

      // caller does NOT own the X509 pointer returned; "const" cannot be used because of the openssl API does not support it
      DLLEXPORT X509 *getData() const;

      // caller owns value returned
      DLLEXPORT QoreHashNode *getSubjectHash() const;

      // caller owns value returned
      DLLEXPORT QoreHashNode *getIssuerHash() const;
      DLLEXPORT int64 getSerialNumber() const;
      DLLEXPORT int64 getVersion() const;

      // caller owns value returned
      DLLEXPORT QoreHashNode *getPurposeHash() const;

      // caller owns value returned
      DLLEXPORT DateTimeNode *getNotBeforeDate() const;

      // caller owns value returned
      DLLEXPORT DateTimeNode *getNotAfterDate() const;

      // caller owns value returned
      DLLEXPORT QoreStringNode *getSignatureType() const;

      // caller owns value returned
      DLLEXPORT BinaryNode *getSignature() const;

      // caller owns value returned
      DLLEXPORT QoreStringNode *getPublicKeyAlgorithm() const;

      // caller owns value returned
      DLLEXPORT BinaryNode *getPublicKey() const;

      // caller owns value returned
      DLLEXPORT QoreHashNode *getInfo() const;
};

#endif
