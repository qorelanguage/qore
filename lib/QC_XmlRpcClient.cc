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
#include <qore/intern/QC_XmlRpcClient.h>
#include <qore/intern/QC_HTTPClient.h>
#include <qore/QoreHTTPClient.h>
#include <qore/ReferenceHolder.h>
#include <qore/intern/ql_xml.h>

qore_classid_t CID_XMLRPCCLIENT;

typedef ReferenceHolder<QoreHTTPClient> safe_httpclient_t;

static void XRC_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   // get HTTPClient object
   safe_httpclient_t client((QoreHTTPClient *)getStackObject()->getReferencedPrivateData(CID_HTTPCLIENT, xsink), xsink);
   if (!client)
      return;

   // set options for XML-RPC communication
   client->setDefaultPath("RPC2");
   client->setDefaultHeaderValue("Content-Type", "text/xml");
   client->setDefaultHeaderValue("Accept", "text/xml");
   client->setDefaultHeaderValue("User-Agent", "Qore XML-RPC Client v" PACKAGE_VERSION);

   client->addProtocol("xmlrpc", 80, false);
   client->addProtocol("xmlrpcs", 443, true);

   const QoreHashNode* n = test_hash_param(params, 0);
   if (n && client->setOptions(n, xsink))
      return;

   client->connect(xsink); 
}

static void XRC_copy(QoreObject *self, QoreObject *old, QoreHTTPClient* client, ExceptionSink *xsink)
{
   xsink->raiseException("XMLRPCCLIENT-COPY-ERROR", "copying XmlRpcClient objects is not yet supported.");
}

static AbstractQoreNode *XRC_callArgs(QoreObject *self, QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink)
{
   client->connect(xsink);
   if (xsink->isEvent())
      return NULL;

   // create the outgoing message in XML-RPC call format
   QoreStringNodeHolder msg(makeXMLRPCCallStringArgs(client->getEncoding(), params, xsink));
   if (!msg)
      return NULL;
   // send the message to the server and get the response as an XML string
   ReferenceHolder<AbstractQoreNode> ans(client->post(NULL, NULL, msg->getBuffer(), msg->strlen(), xsink), xsink);
   if (!ans)
      return NULL;
   
   QoreStringNode *str = dynamic_cast<QoreStringNode *>(*ans);
   if (!str) {
      xsink->raiseException("XMLRPCCLIENT-RESPONSE-ERROR", "undecoded binary response received from remote server");
      return 0;
   }

   // parse XML-RPC response   
   return parseXMLRPCResponse(str, QCS_DEFAULT, xsink);
}

static AbstractQoreNode *XRC_call(QoreObject *self, QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink)
{
   client->connect(xsink);
   if (xsink->isEvent())
      return NULL;

   // create the outgoing message in XML-RPC call format
   QoreStringNodeHolder msg(makeXMLRPCCallString(client->getEncoding(), params, xsink));
   if (!msg)
      return NULL;
   // send the message to the server and get the response as an XML string
   ReferenceHolder<AbstractQoreNode> ans(client->post(NULL, NULL, msg->getBuffer(), msg->strlen(), xsink), xsink);
   if (!ans)
      return NULL;

   QoreStringNode *str = dynamic_cast<QoreStringNode *>(*ans);
   if (!str) {
      xsink->raiseException("XMLRPCCLIENT-RESPONSE-ERROR", "undecoded binary response received from remote server");
      return 0;
   }

   // parse XML-RPC response
   return parseXMLRPCResponse(str, QCS_DEFAULT, xsink);
}

class QoreClass *initXmlRpcClientClass(class QoreClass *http_client)
{
   QoreClass* client = new QoreClass("XmlRpcClient", QDOM_NETWORK); 
   CID_XMLRPCCLIENT = client->getID();

   client->addDefaultBuiltinBaseClass(http_client);

   client->setConstructor(XRC_constructor);
   client->setCopy((q_copy_t)XRC_copy);
   client->addMethod("callArgs", (q_method_t)XRC_callArgs);
   client->addMethod("call",     (q_method_t)XRC_call);

   return client;
} 

