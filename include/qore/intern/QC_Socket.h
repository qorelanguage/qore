/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QC_Socket.h

  Qore Programming Language

  Copyright (C) 2003 - 2015 David Nichols

  provides a thread-safe interface to the QoreSocket object

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

#ifndef _QORE_CLASS_SOCKET_H

#define _QORE_CLASS_SOCKET_H

DLLLOCAL QoreClass* initSocketClass(QoreNamespace& qorens);
DLLEXPORT extern qore_classid_t CID_SOCKET;
DLLEXPORT extern QoreClass* QC_SOCKET;

#include <qore/QoreSocket.h>
#include <qore/AbstractPrivateData.h>
#include <qore/QoreThreadLock.h>
#include <qore/QoreSocketObject.h>
#include <qore/intern/QC_SSLCertificate.h>
#include <qore/intern/QC_SSLPrivateKey.h>

class my_socket_priv {
public:
   QoreSocket* socket;
   QoreSSLCertificate* cert;
   QoreSSLPrivateKey* pk;
   mutable QoreThreadLock m;

   DLLLOCAL my_socket_priv(QoreSocket* s, QoreSSLCertificate* c = 0, QoreSSLPrivateKey* p = 0) : socket(s), cert(c), pk(p) {
   }

   DLLLOCAL my_socket_priv() : socket(new QoreSocket), cert(0), pk(0) {
   }

   DLLLOCAL ~my_socket_priv() {
      if (cert)
	 cert->deref();
      if (pk)
	 pk->deref();

      delete socket;
   }

   //! sets backwards-compatible members on accept in a new object - will be removed in a future version of qore
   DLLLOCAL void setAccept(QoreObject* o) {
      socket->setAccept(o);
   }

   DLLLOCAL static void setAccept(QoreSocketObject& sock, QoreObject* o) {
      sock.priv->setAccept(o);
   }
};

#endif // _QORE_CLASS_QORESOCKET_H
