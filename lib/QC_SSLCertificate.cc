/*
  QC_SSLCertificate.cc

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

#include <qore/config.h>
#include <qore/common.h>
#include <qore/QC_SSLCertificate.h>
#include <qore/QoreClass.h>
#include <qore/params.h>
#include <qore/Exception.h>
#include <qore/support.h>
#include <qore/Object.h>

#include <openssl/ssl.h>
#include <openssl/pem.h>

int CID_SSLCERTIFICATE;

static inline void *getSSLCertificate(void *obj)
{
   ((QoreSSLCertificate *)obj)->ROreference();
   return obj;
}

/*
void createSSLCertificateObject(class Object *self, X509 *cert)
{
   self->setPrivate(CID_SSLCERTIFICATE, new QoreSSLCertificate(cert), getSSLCertificate);
}
*/

// syntax: SSLCertificate(filename)
static QoreNode *SSLCERT_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p0 = get_param(params, 0);
   if (!p0 || p0->type != NT_STRING)
   {
      xsink->raiseException("SSLCERTIFICATE-CONSTRUCTOR-ERROR", "expecting file name as argument");
      return NULL;
   }
   
   QoreSSLCertificate *qc = new QoreSSLCertificate(p0->val.String->getBuffer(), xsink);
   if (xsink->isEvent())
      qc->deref();
   else
      self->setPrivate(CID_SSLCERTIFICATE, qc, getSSLCertificate);

   return NULL;
}

static QoreNode *SSLCERT_destructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreSSLCertificate *s = (QoreSSLCertificate *)self->getAndClearPrivateData(CID_SSLCERTIFICATE);
   if (s)
      s->deref();
   return NULL;
}

static QoreNode *SSLCERT_copy(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   xsink->raiseException("SSLCERTIFICATE-COPY-ERROR", "SSLCertificate objects cannot be copied");
   return NULL;
}

#if 0
static QoreNode *SSLCERT_getInfo(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreSSLCertificate *s = (QoreSSLCertificate *)self->getReferencedPrivateData(CID_SSLCERTIFICATE);
   class QoreNode *rv;
   if (s)
   {
      rv = new QoreNode(s->getInfo());
      s->deref();
   }
   else
   {
      alreadyDeleted(xsink, "SSLCertificate::getInfo");
      rv = NULL;
   }
   return rv;

}
#endif

class QoreClass *initSSLCertificateClass()
{
   tracein("initSSLCertificateClass()");

   class QoreClass *QC_SSLCERTIFICATE = new QoreClass(strdup("SSLCertificate"));
   CID_SSLCERTIFICATE = QC_SSLCERTIFICATE->getID();
   QC_SSLCERTIFICATE->addMethod("constructor",           SSLCERT_constructor);
   QC_SSLCERTIFICATE->addMethod("destructor",            SSLCERT_destructor);
   QC_SSLCERTIFICATE->addMethod("copy",                  SSLCERT_copy);
   //QC_SSLCERTIFICATE->addMethod("getInfo",               SSLCERT_getInfo);

   traceout("initSSLCertificateClass()");
   return QC_SSLCERTIFICATE;
}
