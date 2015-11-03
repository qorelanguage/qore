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

static void XRC_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink) {
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

   // do not connect immediately if the second argument is True
   bool no_connect = get_bool_param(params, 1);
   if (!no_connect)
      client->connect(xsink);
}

static void XRC_copy(QoreObject *self, QoreObject *old, QoreHTTPClient* client, ExceptionSink *xsink) {
   xsink->raiseException("XMLRPCCLIENT-COPY-ERROR", "copying XmlRpcClient objects is not yet supported.");
}

static QoreHashNode *make_xmlrpc_call(QoreHTTPClient *client, QoreStringNode *msg, QoreHashNode *info, ExceptionSink *xsink) {
   ReferenceHolder<QoreHashNode> response(client->send("POST", 0, 0, msg->getBuffer(), msg->strlen(), true, info, xsink), xsink);
   if (!response)
      return 0;

   ReferenceHolder<AbstractQoreNode> ans(response->takeKeyValue("body"), xsink);
   if (!ans)
      return 0;
   
   if (ans->getType() != NT_STRING) {
      xsink->raiseException("XMLRPCCLIENT-RESPONSE-ERROR", "undecoded binary response received from remote server");
      return 0;
   }

   // parse XML-RPC response   
   QoreHashNode *rv = parseXMLRPCResponse(reinterpret_cast<QoreStringNode *>(*ans), QCS_DEFAULT, xsink);
   if (!rv)
      return 0;

   if (info) {
      info->setKeyValue("response_headers", response.release(), xsink);
      info->setKeyValue("response", ans.release(), xsink);
   }

   return rv;
}

static AbstractQoreNode *XRC_callArgs(QoreObject *self, QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink) {
   // create the outgoing message in XML-RPC call format
   QoreStringNodeHolder msg(makeXMLRPCCallStringArgs(client->getEncoding(), 0, params, xsink));
   if (!msg)
      return 0;

   // send the message to the server and get the response as an XML string
   return make_xmlrpc_call(client, *msg, 0, xsink);
}

static AbstractQoreNode *XRC_call(QoreObject *self, QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink) {
   // create the outgoing message in XML-RPC call format
   QoreStringNodeHolder msg(makeXMLRPCCallString(client->getEncoding(), 0, params, xsink));
   if (!msg)
      return 0;

   // send the message to the server and get the response as an XML string
   return make_xmlrpc_call(client, *msg, 0, xsink);
}

static AbstractQoreNode *XRC_callArgsWithInfo(QoreObject *self, QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink) {
   // get info reference
   const ReferenceNode *ref = test_reference_param(params, 0);
   if (!ref) {
      xsink->raiseException("XMLRPC-CALLWITHINFO-ERROR", "expecting a reference as the first parameter to XmlRpcClient::callArgsWithInfo()");
      return 0;
   }

   // get arguments
   ReferenceHolder<QoreListNode> args(params->copyListFrom(1), xsink);

   // create the outgoing message in XML-RPC call format
   QoreStringNodeHolder msg(makeXMLRPCCallStringArgs(client->getEncoding(), 0, *args, xsink));
   if (!msg)
      return 0;

   ReferenceHolder<QoreHashNode> info(new QoreHashNode, xsink);

   // send the message to the server and get the response as an XML string
   ReferenceHolder<QoreHashNode> rv(make_xmlrpc_call(client, *msg, *info, xsink), xsink);
   if (!rv)
      return 0;

   // set request key
   info->setKeyValue("request", msg.release(), xsink);

   // write info hash to reference
   AutoVLock vl(xsink);
   ReferenceHelper rh(ref, vl, xsink);
   if (!rh)
      return 0;

   if (rh.assign(info.release(), xsink))
      return 0;   

   return rv.release();
}

static AbstractQoreNode *XRC_callWithInfo(QoreObject *self, QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink) {
   // get info reference
   const ReferenceNode *ref = test_reference_param(params, 0);
   if (!ref) {
      xsink->raiseException("XMLRPC-CALLWITHINFO-ERROR", "expecting a reference as the first parameter to XmlRpcClient::callWithInfo()");
      return 0;
   }

   // get arguments
   ReferenceHolder<QoreListNode> args(params->copyListFrom(1), xsink);

   // create the outgoing message in XML-RPC call format
   QoreStringNodeHolder msg(makeXMLRPCCallString(client->getEncoding(), 0, *args, xsink));
   if (!msg)
      return 0;

   ReferenceHolder<QoreHashNode> info(new QoreHashNode, xsink);

   // send the message to the server and get the response as an XML string
   ReferenceHolder<QoreHashNode> rv(make_xmlrpc_call(client, *msg, *info, xsink), xsink);
   if (!rv)
      return 0;

   // set request key
   info->setKeyValue("request", msg.release(), xsink);

   // write info hash to reference
   AutoVLock vl(xsink);
   ReferenceHelper rh(ref, vl, xsink);
   if (!rh)
      return 0;

   if (rh.assign(info.release(), xsink))
      return 0;   

   return rv.release();
}

static AbstractQoreNode *XRC_setEventQueue(QoreObject *self, QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink) {
    const QoreObject *o = test_object_param(params, 0);
    Queue *q = o ? (Queue *)o->getReferencedPrivateData(CID_QUEUE, xsink) : 0;
    if (*xsink)
        return 0;
    // pass reference from QoreObject::getReferencedPrivateData() to function
    client->setEventQueue(q, xsink);
    return 0;
}

QoreClass *initXmlRpcClientClass(class QoreClass *http_client) {
   QoreClass* client = new QoreClass("XmlRpcClient", QDOM_NETWORK); 
   CID_XMLRPCCLIENT = client->getID();

   client->addDefaultBuiltinBaseClass(http_client);

   client->setConstructor(XRC_constructor);
   client->setCopy((q_copy_t)XRC_copy);
   client->addMethod("callArgs",         (q_method_t)XRC_callArgs);
   client->addMethod("call",             (q_method_t)XRC_call);
   client->addMethod("callArgsWithInfo", (q_method_t)XRC_callArgsWithInfo);
   client->addMethod("callWithInfo",     (q_method_t)XRC_callWithInfo);
   client->addMethod("setEventQueue",    (q_method_t)XRC_setEventQueue);

   return client;
} 
