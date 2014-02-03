/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QC_Socket.h

  Qore Programming Language

  Copyright 2003 - 2014 David Nichols

  provides a thread-safe interface to the QoreSocket object

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

#ifndef _QORE_CLASS_SOCKET_H

#define _QORE_CLASS_SOCKET_H

DLLLOCAL QoreClass* initSocketClass(QoreNamespace& qorens);
DLLEXPORT extern qore_classid_t CID_SOCKET;
DLLLOCAL extern QoreClass* QC_SOCKET;

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

   DLLLOCAL my_socket_priv(QoreSocket* s) : socket(s), cert(0), pk(0) {
   }

   DLLLOCAL my_socket_priv() : socket(new QoreSocket()), cert(0), pk(0) {
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
