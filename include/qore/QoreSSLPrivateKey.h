/*
  QC_SSLPrivateKey.h
  
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

#ifndef _QORE_QORESSLPRIVATEKEY_H

#define _QORE_QORESSLPRIVATEKEY_H

#include <openssl/ssl.h>
#include <openssl/evp.h>

struct qore_sslpk_private;

class QoreSSLPrivateKey : public AbstractPrivateData
{
   private:
      struct qore_sslpk_private *priv; // for private implementation

      // not implemented
      DLLLOCAL QoreSSLPrivateKey(const QoreSSLPrivateKey&);
      DLLLOCAL QoreSSLPrivateKey& operator=(const QoreSSLPrivateKey&);

   protected:
      DLLLOCAL virtual ~QoreSSLPrivateKey();

   public:
      // caller owns the QoreString returned
      DLLEXPORT class QoreStringNode *getPEM(class ExceptionSink *xsink) const;

      DLLLOCAL QoreSSLPrivateKey(EVP_PKEY *p);
      DLLLOCAL QoreSSLPrivateKey(const char *fn, char *pp, class ExceptionSink *xsink);
      // caller does NOT own the EVP_PKEY returned; "const" cannot be used because of the openssl API does not support it
      DLLLOCAL EVP_PKEY *getData() const;
      DLLLOCAL const char *getType() const;
      DLLLOCAL int64 getVersion() const;
      // returns the length in bits
      DLLLOCAL int64 getBitLength() const;
      // caller owns the QoreHashNode returned
      DLLLOCAL class QoreHashNode *getInfo() const;
};

#endif
