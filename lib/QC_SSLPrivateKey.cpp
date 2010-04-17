/*
  QC_SSLPrivateKey.cpp

  Qore Programming Language

  Copyright 2003 - 2010 David Nichols

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
#include <qore/intern/QC_SSLPrivateKey.h>

#include <openssl/ssl.h>
#include <openssl/pem.h>

qore_classid_t CID_SSLPRIVATEKEY;

// syntax: SSLPrivateKey(PEM_string, passphrase)
static void SSLPKEY_constructor_str(QoreObject *self, const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p0, const QoreStringNode, args, 0);
   HARD_QORE_PARAM(p1, const QoreStringNode, args, 1);

   SimpleRefHolder<QoreSSLPrivateKey> qpk;

   // FIXME: this class should not read file - we have to check the parse option PO_NO_FILESYSTEM at runtime
   if (p0->strlen() < 120) {
      if (getProgram()->getParseOptions() & PO_NO_FILESYSTEM) {
	 xsink->raiseException("INVALID-FILESYSTEM-ACCESS", "passing a filename to SSLPrivateKey::constructor() violates parse option NO-FILESYSTEM");
	 return;
      }

      qpk = new QoreSSLPrivateKey(p0->getBuffer(), p1->getBuffer(), xsink);
   }
   else
      qpk = new QoreSSLPrivateKey(p0, p1->getBuffer(), xsink);

   if (!*xsink)
      self->setPrivate(CID_SSLPRIVATEKEY, qpk.release());
}

// syntax: SSLPrivateKey(binary, passphrase)
static void SSLPKEY_constructor_bin(QoreObject *self, const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_PARAM(bin, const BinaryNode, args, 0);

   SimpleRefHolder<QoreSSLPrivateKey> qc(new QoreSSLPrivateKey(bin, xsink));
   if (!*xsink)
      self->setPrivate(CID_SSLPRIVATEKEY, qc.release());           
}

static void SSLPKEY_copy(QoreObject *self, QoreObject *old, QoreSSLPrivateKey *pk, ExceptionSink *xsink) {
   xsink->raiseException("SSLPRIVATEKEY-COPY-ERROR", "SSLPrivateKey objects cannot be copied");
}

static QoreHashNode *SSLPKEY_getInfo(QoreObject *self, QoreSSLPrivateKey *pk, const QoreListNode *args, ExceptionSink *xsink) {
   return pk->getInfo();
}

static QoreStringNode *SSLPKEY_getType(QoreObject *self, QoreSSLPrivateKey *pk, const QoreListNode *args, ExceptionSink *xsink) {
   return new QoreStringNode(pk->getType());
}

static QoreBigIntNode *SSLPKEY_getVersion(QoreObject *self, QoreSSLPrivateKey *pk, const QoreListNode *args, ExceptionSink *xsink) {
   return new QoreBigIntNode(pk->getVersion());
}

static QoreBigIntNode *SSLPKEY_getBitLength(QoreObject *self, QoreSSLPrivateKey *pk, const QoreListNode *args, ExceptionSink *xsink) {
   return new QoreBigIntNode(pk->getBitLength());
}

QoreClass *initSSLPrivateKeyClass() {
   QORE_TRACE("initSSLPrivateKeyClass()");

   QoreClass *QC_SSLPRIVATEKEY = new QoreClass("SSLPrivateKey");
   CID_SSLPRIVATEKEY = QC_SSLPRIVATEKEY->getID();

   QC_SSLPRIVATEKEY->setConstructorExtended(SSLPKEY_constructor_str, false, QC_NO_FLAGS, QDOM_DEFAULT, 2, stringTypeInfo, QORE_PARAM_NO_ARG, stringTypeInfo, null_string());
   QC_SSLPRIVATEKEY->setConstructorExtended(SSLPKEY_constructor_bin, false, QC_NO_FLAGS, QDOM_DEFAULT, 2, binaryTypeInfo, QORE_PARAM_NO_ARG, stringTypeInfo, null_string());

   QC_SSLPRIVATEKEY->setCopy((q_copy_t)SSLPKEY_copy);

   QC_SSLPRIVATEKEY->addMethodExtended("getType",          (q_method_t)SSLPKEY_getType, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, stringTypeInfo);
   QC_SSLPRIVATEKEY->addMethodExtended("getVersion",       (q_method_t)SSLPKEY_getVersion, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, bigIntTypeInfo);
   QC_SSLPRIVATEKEY->addMethodExtended("getBitLength",     (q_method_t)SSLPKEY_getBitLength, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, bigIntTypeInfo);
   QC_SSLPRIVATEKEY->addMethodExtended("getInfo",          (q_method_t)SSLPKEY_getInfo, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, hashTypeInfo);

   return QC_SSLPRIVATEKEY;
}
