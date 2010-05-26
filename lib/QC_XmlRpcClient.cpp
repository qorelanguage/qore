/*
  QC_XmlRpcClient.cpp

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

#include <qore/Qore.h>
#include <qore/intern/QC_XmlRpcClient.h>
#include <qore/intern/QC_HTTPClient.h>
#include <qore/intern/QC_JsonRpcClient.h>
#include <qore/intern/QC_Queue.h>
#include <qore/QoreHTTPClient.h>
#include <qore/ReferenceHolder.h>
#include <qore/intern/ql_xml.h>

qore_classid_t CID_XMLRPCCLIENT;

typedef ReferenceHolder<QoreHTTPClient> safe_httpclient_t;

static void set_xrc_defaults(QoreHTTPClient &client) {
   // set options for XML-RPC communication
   client.setDefaultPath("RPC2");
   client.setDefaultHeaderValue("Content-Type", "text/xml");
   client.setDefaultHeaderValue("Accept", "text/xml");
   client.setDefaultHeaderValue("User-Agent", "Qore XML-RPC Client v" PACKAGE_VERSION);

   client.addProtocol("xmlrpc", 80, false);
   client.addProtocol("xmlrpcs", 443, true);
}

static void XRC_constructor_bool(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink) {
   // get HTTPClient object
   safe_httpclient_t client((QoreHTTPClient *)getStackObject()->getReferencedPrivateData(CID_HTTPCLIENT, xsink), xsink);
   if (!client)
      return;

   set_xrc_defaults(*(*client));

   if (!HARD_QORE_BOOL(params, 0))
      client->connect(xsink);
}

static void XRC_constructor_hash_bool(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink) {
   // get HTTPClient object
   safe_httpclient_t client((QoreHTTPClient *)getStackObject()->getReferencedPrivateData(CID_HTTPCLIENT, xsink), xsink);
   if (!client)
      return;

   set_xrc_defaults(*(*client));

   const QoreHashNode* n = HARD_QORE_HASH(params, 0);
   if (n && client->setOptions(n, xsink))
      return;

   // do not connect immediately if the second argument is True
   if (!HARD_QORE_BOOL(params, 1))
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

   AbstractQoreNode *ah = *ans;
   if (info) {
      info->setKeyValue("response", ans.release(), xsink);
      info->setKeyValue("response_headers", response.release(), xsink);
   }
   
   if (ah->getType() != NT_STRING) {
      xsink->raiseException("XMLRPCCLIENT-RESPONSE-ERROR", "undecoded binary response received from remote server");
      return 0;
   }

   // parse XML-RPC response   
   return parseXMLRPCResponse(reinterpret_cast<QoreStringNode *>(ah), QCS_DEFAULT, xsink);
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
   const ReferenceNode *ref = HARD_QORE_REF(params, 0);

   // get arguments
   ReferenceHolder<QoreListNode> args(params->copyListFrom(1), xsink);

   // create the outgoing message in XML-RPC call format
   QoreStringNode *msg = makeXMLRPCCallStringArgs(client->getEncoding(), 0, *args, xsink);
   if (!msg)
      return 0;

   HTTPInfoRefHelper irh(ref, msg, xsink);

   // send the message to the server and get the response as an XML string
   return make_xmlrpc_call(client, msg, *irh, xsink);
}

static AbstractQoreNode *XRC_callWithInfo(QoreObject *self, QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink) {
   // get info reference
   const ReferenceNode *ref = HARD_QORE_REF(params, 0);

   // get arguments
   ReferenceHolder<QoreListNode> args(params->copyListFrom(1), xsink);

   // create the outgoing message in XML-RPC call format
   QoreStringNode *msg = makeXMLRPCCallString(client->getEncoding(), 0, *args, xsink);
   if (!msg)
      return 0;

   HTTPInfoRefHelper irh(ref, msg, xsink);

   // send the message to the server and get the response as an XML string
   return make_xmlrpc_call(client, msg, *irh, xsink);
}

static AbstractQoreNode *XRC_setEventQueue_nothing(QoreObject *self, QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink) {
   client->setEventQueue(0, xsink);
   return 0;
}

static AbstractQoreNode *XRC_setEventQueue_queue(QoreObject *self, QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_OBJ_DATA(q, Queue, params, 0, CID_QUEUE, "Queue", "XmlRpcClient::setEventQueue", xsink);
   if (*xsink)
      return 0;
   // pass reference from QoreObject::getReferencedPrivateData() to function
   client->setEventQueue(q, xsink);
   return 0;
}

QoreClass *initXmlRpcClientClass(QoreClass *http_client) {
   assert(QC_QUEUE);

   QoreClass* client = new QoreClass("XmlRpcClient", QDOM_NETWORK); 
   CID_XMLRPCCLIENT = client->getID();

   client->addDefaultBuiltinBaseClass(http_client);

   client->setConstructorExtended(XRC_constructor_bool, false, QC_NO_FLAGS, QDOM_DEFAULT, 1, softBoolTypeInfo, &False);
   client->setConstructorExtended(XRC_constructor_hash_bool, false, QC_NO_FLAGS, QDOM_DEFAULT, 2, hashTypeInfo, QORE_PARAM_NO_ARG, softBoolTypeInfo, &False);

   client->setCopy((q_copy_t)XRC_copy);
   client->addMethodExtended("callArgs",         (q_method_t)XRC_callArgs, false, QC_NO_FLAGS, QDOM_DEFAULT, hashTypeInfo, 2, stringTypeInfo, QORE_PARAM_NO_ARG, anyTypeInfo, QORE_PARAM_NO_ARG);
   client->addMethodExtended("call",             (q_method_t)XRC_call, false, QC_USES_EXTRA_ARGS, QDOM_DEFAULT, hashTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   client->addMethodExtended("callArgsWithInfo", (q_method_t)XRC_callArgsWithInfo, false, QC_NO_FLAGS, QDOM_DEFAULT, hashTypeInfo, 3, referenceTypeInfo, QORE_PARAM_NO_ARG, stringTypeInfo, QORE_PARAM_NO_ARG, anyTypeInfo, QORE_PARAM_NO_ARG);
   client->addMethodExtended("callWithInfo",     (q_method_t)XRC_callWithInfo, false, QC_USES_EXTRA_ARGS, QDOM_DEFAULT, hashTypeInfo, 2, referenceTypeInfo, QORE_PARAM_NO_ARG, stringTypeInfo, QORE_PARAM_NO_ARG);

   // XmlRpcClient::setEventQueue() returns nothing
   client->addMethodExtended("setEventQueue",    (q_method_t)XRC_setEventQueue_nothing, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo);
   // XmlRpcClient::setEventQueue(Queue $queue) returns nothing
   client->addMethodExtended("setEventQueue",    (q_method_t)XRC_setEventQueue_queue, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 1, QC_QUEUE->getTypeInfo(), QORE_PARAM_NO_ARG);

   return client;
} 
