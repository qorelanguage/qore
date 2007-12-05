/*
  QC_SSLCertificate.h
  
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

#ifndef _QORE_CLASS_SSLCERTIFICATE_H

#define _QORE_CLASS_SSLCERTIFICATE_H

DLLEXPORT extern int CID_SSLCERTIFICATE;
DLLLOCAL class QoreClass *initSSLCertificateClass();

#include <qore/AbstractPrivateData.h>
#include <qore/QoreSSLBase.h>

#include <openssl/ssl.h>
#include <openssl/pem.h>

#include <errno.h>

class QoreSSLCertificate : public AbstractPrivateData, public QoreSSLBase
{
   private:
      X509 *cert;

      DLLLOCAL class QoreNode *doPurposeValue(int id, int ca) const;

   protected:
      DLLLOCAL virtual ~QoreSSLCertificate();

   public:
      DLLEXPORT class QoreString *getPEM(class ExceptionSink *xsink) const;

      DLLLOCAL QoreSSLCertificate(X509 *c);
      DLLLOCAL QoreSSLCertificate(const char *fn, class ExceptionSink *xsink);
      DLLLOCAL X509 *getData() const;
      DLLLOCAL class QoreHash *getSubjectHash() const;
      DLLLOCAL class QoreHash *getIssuerHash() const;
      DLLLOCAL int64 getSerialNumber() const;
      DLLLOCAL int64 getVersion() const;
      DLLLOCAL class QoreHash *getPurposeHash() const;
      DLLLOCAL class DateTime *getNotBeforeDate() const;
      DLLLOCAL class DateTime *getNotAfterDate() const;
      DLLLOCAL class QoreString *getSignatureType() const;
      DLLLOCAL class BinaryObject *getSignature() const;
      DLLLOCAL class QoreString *getPublicKeyAlgorithm() const;
      DLLLOCAL class BinaryObject *getPublicKey() const;
      DLLLOCAL class QoreHash *getInfo() const;
};

#endif // _QORE_CLASS_SSLCERTIFICATE_H
