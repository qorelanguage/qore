/*
  QC_XMPRPC.cc

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
#include <qore/QC_XMLRPC.h>
#include <qore/QoreClass.h>
#include <qore/params.h>
#include <qore/Exception.h>
#include <qore/support.h>
#include <qore/Object.h>

int CID_XMLRPCCLIENT, CID_XMLRPCSERVER;

static void XRS_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
}

static QoreNode *XRS_copy(class Object *self, class Object *old, ExceptionSink *xsink)
{
}

static QoreNode *XRS_call(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
}

static void XRC_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
}

static QoreNode *XRC_call(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
}

class QoreClass *initXMPRPCServer()
{
   tracein("initXMLRPCServer()");

   class QoreClass *QC_XMLRPCSERVER = new QoreClass(strdup("XMLRPCServer"));
   CID_XMLRPCSERVER = QC_XMLRPCSERVER->getID();
   QC_XMLRPCSERVER->setConstructor(XRS_constructor);

   traceout("initXMLRPCServer()");
   return QC_XMLRPCSERVER;
}

class QoreClass *initXMPRPCClient()
{
   tracein("initXMLRPCClient()");

   class QoreClass *QC_XMLRPCCLIENT = new QoreClass(strdup("XMLRPCClient"));
   CID_XMLRPCCLIENT = QC_XMLRPCCLIENT->getID();
   QC_XMLRPCCLIENT->setConstructor(XRC_constructor);
   QC_XMLRPCCLIENT->addMethod("call",          XRC_call);

   traceout("initXMLRPCClient()");
   return QC_XMLRPCCLIENT;
}
