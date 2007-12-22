/*
  QoreSSLCertificate.h

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols

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

struct qore_sslcert_private;

class QoreSSLCertificate : public AbstractPrivateData, public QoreSSLBase
{
   private:
      struct qore_sslcert_private *priv; // private implementation

      DLLLOCAL class QoreNode *doPurposeValue(int id, int ca) const;

      // not implemented
      DLLLOCAL QoreSSLCertificate(const QoreSSLCertificate&);
      DLLLOCAL QoreSSLCertificate& operator=(const QoreSSLCertificate&);

   protected:
      DLLLOCAL virtual ~QoreSSLCertificate();

   public:
      // caller owns the QoreString returned
      DLLEXPORT class QoreString *getPEM(class ExceptionSink *xsink) const;

      DLLLOCAL QoreSSLCertificate(X509 *c);
      DLLLOCAL QoreSSLCertificate(const char *fn, class ExceptionSink *xsink);
      // caller does NOT own the X509 pointer returned; "const" cannot be used because of the openssl API does not support it
      DLLLOCAL X509 *getData() const;
      // caller owns value returned
      DLLLOCAL class QoreHash *getSubjectHash() const;
      // caller owns value returned
      DLLLOCAL class QoreHash *getIssuerHash() const;
      DLLLOCAL int64 getSerialNumber() const;
      DLLLOCAL int64 getVersion() const;
      // caller owns value returned
      DLLLOCAL class QoreHash *getPurposeHash() const;
      // caller owns value returned
      DLLLOCAL class DateTime *getNotBeforeDate() const;
      // caller owns value returned
      DLLLOCAL class DateTime *getNotAfterDate() const;
      // caller owns value returned
      DLLLOCAL class QoreString *getSignatureType() const;
      // caller owns value returned
      DLLLOCAL class BinaryObject *getSignature() const;
      // caller owns value returned
      DLLLOCAL class QoreString *getPublicKeyAlgorithm() const;
      // caller owns value returned
      DLLLOCAL class BinaryObject *getPublicKey() const;
      // caller owns value returned
      DLLLOCAL class QoreHash *getInfo() const;
};

#endif
