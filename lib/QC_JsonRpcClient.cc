/*
  QC_JsonRpcClient.cc

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
#include <qore/intern/QC_JsonRpcClient.h>
#include <qore/intern/QC_HTTPClient.h>
#include <qore/QoreHTTPClient.h>
#include <qore/ReferenceHolder.h>
#include <qore/intern/ql_json.h>

qore_classid_t CID_JSONRPCCLIENT;

static void JRC_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink) {
   // get HTTPClient object
   ReferenceHolder<QoreHTTPClient> client((QoreHTTPClient *)getStackObject()->getReferencedPrivateData(CID_HTTPCLIENT, xsink), xsink);
   if (!client)
      return;

   // set encoding to UTF-8
   client->setEncoding(QCS_UTF8);

   // set options for JSON-RPC communication
   client->setDefaultPath("JSON");
   client->setDefaultHeaderValue("Content-Type", "application/json");
   client->setDefaultHeaderValue("Accept", "application/json");
   client->setDefaultHeaderValue("User-Agent", "Qore JSON-RPC Client v" PACKAGE_VERSION);

   client->addProtocol("jsonrpc", 80, false);
   client->addProtocol("jsonrpcs", 443, true);

   const QoreHashNode* n = test_hash_param(params, 0);
   if (n && client->setOptions(n, xsink))
      return;

   // do not connect immediately if the second argument is True
   bool no_connect = get_bool_param(params, 1);
   if (!no_connect)
      client->connect(xsink); 
}

static void JRC_copy(QoreObject *self, QoreObject *old, QoreHTTPClient* client, ExceptionSink *xsink) {
   xsink->raiseException("JSONRPCCLIENT-COPY-ERROR", "copying JsonRpcClient objects is not yet supported.");
}

static AbstractQoreNode *make_jsonrpc_call(QoreHTTPClient *client, QoreStringNode *msg, QoreHashNode *info, ExceptionSink *xsink) {
   ReferenceHolder<QoreHashNode> response(client->send("POST", 0, 0, msg->getBuffer(), msg->strlen(), true, info, xsink), xsink);
   if (!response)
      return 0;

   ReferenceHolder<AbstractQoreNode> ans(response->takeKeyValue("body"), xsink);

   if (!ans)
      return 0;
   
   if (ans->getType() != NT_STRING) {
      xsink->raiseException("JSONRPCCLIENT-RESPONSE-ERROR", "undecoded binary response received from remote server");
      return 0;
   }

   // parse JSON-RPC response   
   AbstractQoreNode *rv = parseJSONValue(reinterpret_cast<QoreStringNode *>(*ans), xsink);
   if (!rv)
      return 0;

   if (info) {
      info->setKeyValue("response_headers", response.release(), xsink);
      info->setKeyValue("response", ans.release(), xsink);
   }

   return rv;
}

static AbstractQoreNode *JRC_callArgs(QoreObject *self, QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink) {
   // create the outgoing message in JSON-RPC call format
   SimpleRefHolder<QoreStringNode> msg(makeJSONRPC11RequestStringArgs(params, xsink));
   if (!msg)
      return 0;

   return make_jsonrpc_call(client, *msg, 0, xsink);
}

static AbstractQoreNode *JRC_call(QoreObject *self, QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink) {
   // create the outgoing message in JSON-RPC call format
   SimpleRefHolder<QoreStringNode> msg(makeJSONRPC11RequestString(params, xsink));
   if (!msg)
      return 0;

   return make_jsonrpc_call(client, *msg, 0, xsink);
}

static AbstractQoreNode *JRC_callArgsWithInfo(QoreObject *self, QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink) {
   // get info reference
   const ReferenceNode *ref = test_reference_param(params, 0);
   if (!ref) {
      xsink->raiseException("JSONRPC-CALLWITHINFO-ERROR", "expecting a reference as the first parameter to JsonRpcClient::callArgsWithInfo()");
      return 0;
   }

   // get arguments
   ReferenceHolder<QoreListNode> args(params->copyListFrom(1), xsink);

   // create the outgoing message in JSON-RPC call format
   SimpleRefHolder<QoreStringNode> msg(makeJSONRPC11RequestStringArgs(*args, xsink));
   if (!msg)
      return 0;

   ReferenceHolder<QoreHashNode> info(new QoreHashNode, xsink);

   // send the message to the server and get the response as an XML string
   ReferenceHolder<AbstractQoreNode> rv(make_jsonrpc_call(client, *msg, *info, xsink), xsink);
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

static AbstractQoreNode *JRC_callWithInfo(QoreObject *self, QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink)
{
   // get info reference
   const ReferenceNode *ref = test_reference_param(params, 0);
   if (!ref) {
      xsink->raiseException("JSONRPC-CALLWITHINFO-ERROR", "expecting a reference as the first parameter to JsonRpcClient::callArgsWithInfo()");
      return 0;
   }

   // get arguments
   ReferenceHolder<QoreListNode> args(params->copyListFrom(1), xsink);

   // create the outgoing message in JSON-RPC call format
   SimpleRefHolder<QoreStringNode> msg(makeJSONRPC11RequestString(*args, xsink));
   if (!msg)
      return 0;

   ReferenceHolder<QoreHashNode> info(new QoreHashNode, xsink);

   // send the message to the server and get the response as an XML string
   ReferenceHolder<AbstractQoreNode> rv(make_jsonrpc_call(client, *msg, *info, xsink), xsink);
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

static AbstractQoreNode *JRC_setEventQueue(QoreObject *self, QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink) {
    const QoreObject *o = test_object_param(params, 0);
    Queue *q = o ? (Queue *)o->getReferencedPrivateData(CID_QUEUE, xsink) : 0;
    if (*xsink)
        return 0;
    // pass reference from QoreObject::getReferencedPrivateData() to function
    client->setEventQueue(q, xsink);
    return 0;
}

QoreClass *initJsonRpcClientClass(class QoreClass *http_client) {
    QoreClass* client = new QoreClass("JsonRpcClient", QDOM_NETWORK);
    CID_JSONRPCCLIENT = client->getID();

    client->addDefaultBuiltinBaseClass(http_client);

    client->setConstructor(JRC_constructor);
    client->setCopy((q_copy_t)JRC_copy);
    client->addMethod("callArgs",         (q_method_t)JRC_callArgs);
    client->addMethod("call",             (q_method_t)JRC_call);
    client->addMethod("callArgsWithInfo", (q_method_t)JRC_callArgsWithInfo);
    client->addMethod("callWithInfo",     (q_method_t)JRC_callWithInfo);
    client->addMethod("setEventQueue",    (q_method_t)JRC_setEventQueue);

    return client;
} 
