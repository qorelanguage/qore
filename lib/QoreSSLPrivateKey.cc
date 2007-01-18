/*
 QoreSSLPrivateKey.h
 
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

#include <qore/config.h>
#include <qore/common.h>
#include <qore/QC_SSLPrivateKey.h>
#include <qore/Hash.h>
#include <qore/Exception.h>
#include <qore/support.h>
#include <qore/BinaryObject.h>

QoreSSLPrivateKey::QoreSSLPrivateKey(EVP_PKEY *p) : pk(p) 
{
}

QoreSSLPrivateKey::~QoreSSLPrivateKey()
{
   if (pk)
      EVP_PKEY_free(pk);
}

QoreSSLPrivateKey::QoreSSLPrivateKey(char *fn, char *pp, class ExceptionSink *xsink)
{
   pk = NULL;
   FILE *fp = fopen(fn, "r");
   if (!fp)
   {
      xsink->raiseException("SSLPRIVATEKEY-CONSTRUCTOR-ERROR", "'%s': %s", fn, strerror(errno));
      return;
   }
   PEM_read_PrivateKey(fp, &pk, NULL, pp ? pp : (void *)"_none_");
   fclose(fp);
   if (!pk)
      xsink->raiseException("SSLPRIVATEKEY-CONSTRUCTOR-ERROR", "error parsing private key file '%s'", fn);
}

EVP_PKEY *QoreSSLPrivateKey::getData() const 
{ 
   return pk; 
}

class QoreString *QoreSSLPrivateKey::getPEM(class ExceptionSink *xsink) const
{
   BIO *bp = BIO_new(BIO_s_mem());
   if (!PEM_write_bio_PrivateKey(bp, pk, NULL, NULL, 0, NULL, NULL))
   {
      BIO_free(bp);
      xsink->raiseException("SSLPRIVATEKEY-ERROR", "could not create PEM string from private key data");
      return NULL;
   }
   char *buf;
   long len = BIO_get_mem_data(bp, &buf);
   
   class QoreString *str = new QoreString(buf, (int)len);
   BIO_free(bp);
   return str;
}

char *QoreSSLPrivateKey::getType() const
{
   switch (EVP_PKEY_type(pk->type))
   {
#ifndef OPENSSL_NO_RSA
      case EVP_PKEY_RSA:
	 return "RSA";
      case EVP_PKEY_RSA2:
	 return "RSA2";
#endif
#ifndef OPENSSL_NO_DSA
      case EVP_PKEY_DSA:
	 return "DSA";
      case EVP_PKEY_DSA1:
	 return "DSA1";
      case EVP_PKEY_DSA2:
	 return "DSA2";
      case EVP_PKEY_DSA3:
	 return "DSA3";
      case EVP_PKEY_DSA4:
	 return "DSA4";
#endif
#ifndef OPENSSL_NO_DH
      case EVP_PKEY_DH:
	 return "DH";
#endif
      default:
	 return "unknown";
   }
}

int64 QoreSSLPrivateKey::getVersion() const
{
   switch (EVP_PKEY_type(pk->type))
   {
#ifndef OPENSSL_NO_RSA
      case EVP_PKEY_RSA:
	 return (int64)pk->pkey.rsa->version + 1;
#endif
#ifndef OPENSSL_NO_DSA
      case EVP_PKEY_DSA:
	 return (int64)pk->pkey.dsa->version + 1;
#endif
#ifndef OPENSSL_NO_DH
      case EVP_PKEY_DH:
	 return (int64)pk->pkey.dh->version + 1;
#endif
      default:
	 return 0;
   }
}

// returns the length in bits
int64 QoreSSLPrivateKey::getBitLength() const
{
   switch (EVP_PKEY_type(pk->type))
   {
#ifndef OPENSSL_NO_RSA
      case EVP_PKEY_RSA:
	 return (int64)RSA_size(pk->pkey.rsa) * 8;
#endif
#ifndef OPENSSL_NO_DSA
      case EVP_PKEY_DSA:
	 return (int64)DSA_size(pk->pkey.dsa) * 8;
#endif
#ifndef OPENSSL_NO_DH
      case EVP_PKEY_DH:
	 return (int64)DH_size(pk->pkey.dh) * 8;
#endif
      default:
	 return 0;
   }
}

class Hash *QoreSSLPrivateKey::getInfo() const
{
   class Hash *h = new Hash();
   h->setKeyValue("type", new QoreNode(getType()), NULL);
   h->setKeyValue("version", new QoreNode(getVersion()), NULL);
   h->setKeyValue("bitLength", new QoreNode(getBitLength()), NULL);
   return h;
}
