/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QC_HTTPClient.h

  Qore Programming Language

  Copyright (C) 2006 - 2010 Qore Technologies

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
#ifndef QC_HTTP_CLIENT_H_
#define QC_HTTP_CLIENT_H_

DLLEXPORT extern qore_classid_t CID_HTTPCLIENT;
DLLLOCAL class QoreClass *initHTTPClientClass();

class HTTPInfoRefHelper {
protected:
   const ReferenceNode *ref;
   ExceptionSink *xsink;
   ReferenceHolder<QoreHashNode> info;

public:
   DLLLOCAL HTTPInfoRefHelper(const ReferenceNode *n_ref, QoreStringNode *msg, ExceptionSink *n_xsink) : ref(n_ref), xsink(n_xsink), info(new QoreHashNode, xsink) {
      info->setKeyValue("request", msg, xsink);
   }
   DLLLOCAL ~HTTPInfoRefHelper() {
      // we have to create a temporary ExceptionSink if there is
      // an active exception, otherwise writing back the reference will fail
      ExceptionSink *txsink = *xsink ? new ExceptionSink : xsink;
      
      // write info hash to reference
      AutoVLock vl(txsink);
      QoreTypeSafeReferenceHelper rh(ref, vl, txsink);
      if (!rh)
         return;

      if (rh.assign(info.release(), txsink))
         return;   

      if (txsink != xsink)
         xsink->assimilate(txsink);
   }
   DLLLOCAL QoreHashNode *operator*() {
      return *info;
   }
};

#endif

// EOF


