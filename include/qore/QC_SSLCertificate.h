/*
  QC_SSLCertificate.h
  
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

#ifndef _QORE_CLASS_SSLCERTIFICATE_H

#define _QORE_CLASS_SSLCERTIFICATE_H

extern int CID_SSLCERTIFICATE;
class QoreClass *initSSLCertificateClass();

#include <qore/ReferenceObject.h>
#include <qore/Exception.h>

#include <openssl/ssl.h>

#include <errno.h>

class QoreSSLCertificate : public ReferenceObject
{
   private:
      X509 *cert;

   protected:
      inline ~QoreSSLCertificate()
      {
	 if (cert)
	    X509_free(cert);
      }

   public:
      inline QoreSSLCertificate(X509 *c) : cert(c) {}
      inline QoreSSLCertificate(char *fn, class ExceptionSink *xsink)
      {
	 cert = NULL;
	 FILE *fp = fopen(fn, "r");
	 if (!fp)
	 {
	    xsink->raiseException("SSLCERTIFICATE-CONSTRUCTOR-ERROR", "'%s': %s", fn, strerror(errno));
	    return;
	 }

	 PEM_read_X509(fp, &cert, NULL, NULL);
	 fclose(fp);
	 if (!cert)
	    xsink->raiseException("SSLCERTIFICATE-CONSTRUCTOR-ERROR", "error parsing certificate file '%s'", fn);
      }
      inline X509 *getData() { return cert; }
      inline void deref()
      {
	 if (ROdereference())
	    delete this;
      }
      //inline class Hash *getInfo();
};

#include <qore/Hash.h>
#include <qore/support.h>

#if 0
inline Hash *QoreSSLCertificate::getInfo()
{
   X509_NAME *sn = X509_get_subject_name(cert);

   class Hash *h = new Hash();

   for (int i = 0; i < X509_NAME_entry_count(sn); i++)
   {
      X509_NAME_ENTRY *e = X509_NAME_get_entry(sn, i);

      ASN1_STRING *astr = X509_NAME_ENTRY_get_data(e);
      printd(0, "astr=%s\n", ASN1_STRING_data(astr));
   }

   return h;
}
#endif 

#endif // _QORE_CLASS_SSLCERTIFICATE_H
