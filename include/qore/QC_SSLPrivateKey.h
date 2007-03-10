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

#ifndef _QORE_CLASS_SSLPRIVATEKEY_H

#define _QORE_CLASS_SSLPRIVATEKEY_H

DLLEXPORT extern int CID_SSLPRIVATEKEY;
DLLLOCAL class QoreClass *initSSLPrivateKeyClass();

#include <qore/AbstractPrivateData.h>
#include <qore/Exception.h>

#include <openssl/ssl.h>
#include <openssl/evp.h>

#include <stdio.h>
#include <errno.h>

class QoreSSLPrivateKey : public AbstractPrivateData
{
   private:
      EVP_PKEY *pk;

   protected:
      DLLLOCAL virtual ~QoreSSLPrivateKey();

   public:
      DLLEXPORT class QoreString *getPEM(class ExceptionSink *xsink) const;

      DLLLOCAL QoreSSLPrivateKey(EVP_PKEY *p);
      DLLLOCAL QoreSSLPrivateKey(const char *fn, char *pp, class ExceptionSink *xsink);
      DLLLOCAL EVP_PKEY *getData() const;
      DLLLOCAL char *getType() const;
      DLLLOCAL int64 getVersion() const;
      // returns the length in bits
      DLLLOCAL int64 getBitLength() const;
      DLLLOCAL class Hash *getInfo() const;
};

#endif // _QORE_CLASS_SSLPRIVATEKEY_H
