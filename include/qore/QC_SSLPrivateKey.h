/*
  QC_SSLPrivateKey.h
  
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

#ifndef _QORE_CLASS_SSLPRIVATEKEY_H

#define _QORE_CLASS_SSLPRIVATEKEY_H

extern int CID_SSLPRIVATEKEY;
class QoreClass *initSSLPrivateKeyClass();

#include <qore/ReferenceObject.h>
#include <qore/Exception.h>

#include <openssl/ssl.h>

#include <stdio.h>
#include <errno.h>

class QoreSSLPrivateKey : public ReferenceObject
{
   private:
      EVP_PKEY *pk;

   protected:
      inline ~QoreSSLPrivateKey()
      {
	 if (pk)
	    EVP_PKEY_free(pk);
      }

   public:
      inline QoreSSLPrivateKey(EVP_PKEY *p) : pk(p) {}
      inline QoreSSLPrivateKey(char *fn, class ExceptionSink *xsink)
      {
	 pk = NULL;
	 FILE *fp = fopen(fn, "r");
	 if (!fp)
	 {
	    xsink->raiseException("SSLPRIVATEKEY-CONSTRUCTOR-ERROR", "'%s': %s", fn, strerror(errno));
	    return;
	 }
	 PEM_read_PrivateKey(fp, &pk, NULL, NULL);
	 fclose(fp);
	 if (!pk)
	    xsink->raiseException("SSLPRIVATEKEY-CONSTRUCTOR-ERROR", "error parsing private key file '%s'", fn);
      }
      inline EVP_PKEY *getData() { return pk; }
      inline void deref()
      {
	 if (ROdereference())
	    delete this;
      }
};

#endif // _QORE_CLASS_SSLPRIVATEKEY_H
