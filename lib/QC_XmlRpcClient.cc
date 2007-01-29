/*
  QC_XmlRpcClient.cc

  Qore Programming Language

  Copyright (C) 2006, 2007 Qore Technologies

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
#include <qore/QC_XmlRpcClient.h>
#include <qore/QC_HTTPClient.h>
#include <qore/QoreHTTPClient.h>
#include <qore/ReferenceHolder.h>
#include <qore/ql_xml.h>

int CID_XMLRPCCLIENT;

typedef ReferenceHolder<QoreHTTPClient> safe_httpclient_t;

static void XRC_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   // get HTTPClient object
   safe_httpclient_t client((QoreHTTPClient *)getStackObject()->getReferencedPrivateData(CID_HTTPCLIENT));

   // set options for XML-RPC communication
   client->setDefaultPath("RPC2");
   client->default_headers["Content-Type"] = "text/xml";
   client->default_headers["Accept"] = "text/xml";
   client->default_headers["User-Agent"] = "Qore XML-RPC Client v" PACKAGE_VERSION;

   client->addProtocol("xmlrpc", 80, false);
   client->addProtocol("xmlrpcs", 443, false);

   QoreNode* n = test_param(params, NT_HASH, 0);
   if (n && client->setOptions(n->val.hash, xsink))
      return;

   client->connect(xsink); 
}

static void XRC_copy(Object *self, Object *old, QoreHTTPClient* client, ExceptionSink *xsink)
{
   xsink->raiseException("XMLRPCCLIENT-COPY-ERROR", "copying XmlRpcClient objects is not yet supported.");
}

static class QoreNode *XRC_callArgs(Object *self, QoreHTTPClient *client, class QoreNode *params, ExceptionSink *xsink)
{
   client->connect(xsink);
   if (xsink->isEvent())
      return NULL;

   // create the outgoing message in XML-RPC call format
   TempString msg(makeXMLRPCCallStringArgs(client->getEncoding(), params, xsink));
   if (!msg)
      return NULL;
   // send the message to the server and get the response as an XML string
   ReferenceHolder<QoreNode> ans(client->post(NULL, NULL, msg->getBuffer(), msg->strlen(), xsink), xsink);
   if (!ans)
      return NULL;
   // parse XML-RPC response
   return parseXMLRPCResponse(ans->val.String, QCS_DEFAULT, xsink);
}

static class QoreNode *XRC_call(Object *self, QoreHTTPClient *client, class QoreNode *params, ExceptionSink *xsink)
{
   client->connect(xsink);
   if (xsink->isEvent())
      return NULL;

   // create the outgoing message in XML-RPC call format
   TempString msg(makeXMLRPCCallString(client->getEncoding(), params, xsink));
   if (!msg)
      return NULL;
   // send the message to the server and get the response as an XML string
   ReferenceHolder<QoreNode> ans(client->post(NULL, NULL, msg->getBuffer(), msg->strlen(), xsink), xsink);
   if (!ans)
      return NULL;
   // parse XML-RPC response
   return parseXMLRPCResponse(ans->val.String, QCS_DEFAULT, xsink);
}

class QoreClass *initXmlRpcClientClass(class QoreClass *http_client)
{
    QoreClass* client = new QoreClass(QDOM_NETWORK, strdup("XmlRpcClient"));
    CID_XMLRPCCLIENT = client->getID();

    client->addDefaultBuiltinBaseClass(http_client);

    client->setConstructor(XRC_constructor);
    client->setCopy((q_copy_t)XRC_copy);
    client->addMethod("callArgs", (q_method_t)XRC_callArgs);
    client->addMethod("call",     (q_method_t)XRC_call);

    return client;
} 

