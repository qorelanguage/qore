/*
 QoreSSLBase.cc
 
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

#include <qore/Qore.h>
#include <qore/QoreSSLBase.h>

#define OBJ_BUF_LEN 80

// static method
class QoreHash *QoreSSLBase::X509_NAME_to_hash(X509_NAME *n)
{
   class QoreHash *h = new QoreHash();
   for (int i = 0; i < X509_NAME_entry_count(n); i++)
   {
      X509_NAME_ENTRY *e = X509_NAME_get_entry(n, i);
      
      ASN1_OBJECT *ko = X509_NAME_ENTRY_get_object(e);
      char key[OBJ_BUF_LEN + 1];
      
      OBJ_obj2txt(key, OBJ_BUF_LEN, ko, 0);
      ASN1_STRING *val = X509_NAME_ENTRY_get_data(e);
      //printd(5, "do_X509_name() %s=%s\n", key, ASN1_STRING_data(val));
      h->setKeyValue(key, new QoreNode((char *)ASN1_STRING_data(val)), NULL);
   }
   return h;
}

// static method
class DateTime *QoreSSLBase::ASN1_TIME_to_DateTime(ASN1_STRING *t)
{
   // FIXME: check ASN1_TIME format if this algorithm is always correct
   QoreString str("20");
   str.concat((char *)ASN1_STRING_data(t));
   str.terminate(14);
   return new DateTime(str.getBuffer());
}

// static method
class QoreString *QoreSSLBase::ASN1_OBJECT_to_QoreString(ASN1_OBJECT *o)
{
   BIO *bp = BIO_new(BIO_s_mem());
   i2a_ASN1_OBJECT(bp, o);
   char *buf;
   long len = BIO_get_mem_data(bp, &buf);
   class QoreString *str = new QoreString(buf, (int)len);
   BIO_free(bp);
   return str;
}
