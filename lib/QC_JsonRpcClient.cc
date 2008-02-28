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

static void JRC_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
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

   client->connect(xsink); 
}

static void JRC_copy(QoreObject *self, QoreObject *old, QoreHTTPClient* client, ExceptionSink *xsink)
{
   xsink->raiseException("JSONRPCCLIENT-COPY-ERROR", "copying JsonRpcClient objects is not yet supported.");
}

static AbstractQoreNode *JRC_callArgs(QoreObject *self, QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink)
{
   client->connect(xsink);
   if (xsink->isEvent())
      return NULL;

   // create the outgoing message in JSON-RPC call format
   SimpleRefHolder<QoreStringNode> msg(makeJSONRPC11RequestStringArgs(params, xsink));
   if (!msg)
      return NULL;
   // send the message to the server and get the response as an JSON string
   ReferenceHolder<AbstractQoreNode> ans(client->post(NULL, NULL, msg->getBuffer(), msg->strlen(), xsink), xsink);
   if (!ans)
      return NULL;

   QoreStringNode *str = dynamic_cast<QoreStringNode *>(*ans);
   if (!str) {
      xsink->raiseException("JSONRPCCLIENT-RESPONSE-ERROR", "undecoded binary response received from remote server");
      return 0;
   }

   // parse JSON-RPC response
   return parseJSONValue(str, xsink);
}

static AbstractQoreNode *JRC_call(QoreObject *self, QoreHTTPClient *client, const QoreListNode *params, ExceptionSink *xsink)
{
   client->connect(xsink);
   if (xsink->isEvent())
      return NULL;

   // create the outgoing message in JSON-RPC call format
   SimpleRefHolder<QoreStringNode> msg(makeJSONRPC11RequestStringArgs(params, xsink));
   if (!msg)
      return NULL;
   // send the message to the server and get the response as an JSON string
   ReferenceHolder<AbstractQoreNode> ans(client->post(NULL, NULL, msg->getBuffer(), msg->strlen(), xsink), xsink);
   if (!ans)
      return NULL;

   QoreStringNode *str = dynamic_cast<QoreStringNode *>(*ans);
   if (!str) {
      xsink->raiseException("JSONRPCCLIENT-RESPONSE-ERROR", "undecoded binary response received from remote server");
      return 0;
   }

   // parse JSON-RPC response
   return parseJSONValue(str, xsink);
}

QoreClass *initJsonRpcClientClass(class QoreClass *http_client)
{
    QoreClass* client = new QoreClass("JsonRpcClient", QDOM_NETWORK);
    CID_JSONRPCCLIENT = client->getID();

    client->addDefaultBuiltinBaseClass(http_client);

    client->setConstructor(JRC_constructor);
    client->setCopy((q_copy_t)JRC_copy);
    client->addMethod("callArgs", (q_method_t)JRC_callArgs);
    client->addMethod("call",     (q_method_t)JRC_call);

    return client;
} 
