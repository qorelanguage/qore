/*
  QoreSSLBase.cpp
 
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

#include <qore/Qore.h>
#include <qore/QoreSSLBase.h>

#define OBJ_BUF_LEN 80

// static method
QoreHashNode *QoreSSLBase::X509_NAME_to_hash(X509_NAME *n) {
   QoreHashNode *h = new QoreHashNode();
   for (int i = 0; i < X509_NAME_entry_count(n); i++) {
      X509_NAME_ENTRY *e = X509_NAME_get_entry(n, i);
      
      ASN1_OBJECT *ko = X509_NAME_ENTRY_get_object(e);
      char key[OBJ_BUF_LEN + 1];
      
      OBJ_obj2txt(key, OBJ_BUF_LEN, ko, 0);
      ASN1_STRING *val = X509_NAME_ENTRY_get_data(e);
      //printd(5, "do_X509_name() %s=%s\n", key, ASN1_STRING_data(val));
      h->setKeyValue(key, new QoreStringNode((const char *)ASN1_STRING_data(val)), 0);
   }
   return h;
}

// static method
DateTimeNode *QoreSSLBase::ASN1_TIME_to_DateTime(ASN1_STRING *t) {
   // FIXME: check ASN1_TIME format if this algorithm is always correct
   QoreString str("20");
   str.concat((char *)ASN1_STRING_data(t));
   str.terminate(14);
   return new DateTimeNode(str.getBuffer());
}

// static method
QoreStringNode *QoreSSLBase::ASN1_OBJECT_to_QoreStringNode(ASN1_OBJECT *o) {
   BIO *bp = BIO_new(BIO_s_mem());
   i2a_ASN1_OBJECT(bp, o);
   char *buf;
   long len = BIO_get_mem_data(bp, &buf);
   QoreStringNode *str = new QoreStringNode(buf, (int)len);
   BIO_free(bp);
   return str;
}
