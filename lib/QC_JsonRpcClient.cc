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
#include <qore/QC_JsonRpcClient.h>
#include <qore/QC_HTTPClient.h>
#include <qore/QoreHTTPClient.h>
#include <qore/ReferenceHolder.h>
#include <qore/ql_json.h>

int CID_JSONRPCCLIENT;

static void JRC_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   // get HTTPClient object
   ReferenceHolder<QoreHTTPClient> client((QoreHTTPClient *)getStackObject()->getReferencedPrivateData(CID_HTTPCLIENT, xsink));
   if (!client)
      return;

   // set encoding to UTF-8
   client->setEncoding(QCS_UTF8);

   // set options for JSON-RPC communication
   client->setDefaultPath("JSON");
   client->default_headers["Content-Type"] = "application/json";
   client->default_headers["Accept"] = "application/json";
   client->default_headers["User-Agent"] = "Qore JSON-RPC Client v" PACKAGE_VERSION;

   client->addProtocol("jsonrpc", 80, false);
   client->addProtocol("jsonrpcs", 443, true);

   QoreNode* n = test_param(params, NT_HASH, 0);
   if (n && client->setOptions(n->val.hash, xsink))
      return;

   client->connect(xsink); 
}

static void JRC_copy(Object *self, Object *old, QoreHTTPClient* client, ExceptionSink *xsink)
{
   xsink->raiseException("JSONRPCCLIENT-COPY-ERROR", "copying JsonRpcClient objects is not yet supported.");
}

static class QoreNode *JRC_callArgs(Object *self, QoreHTTPClient *client, class QoreNode *params, ExceptionSink *xsink)
{
   client->connect(xsink);
   if (xsink->isEvent())
      return NULL;

   // create the outgoing message in JSON-RPC call format
   TempString msg(makeJSONRPC11RequestStringArgs(params, xsink));
   if (!msg)
      return NULL;
   // send the message to the server and get the response as an JSON string
   ReferenceHolder<QoreNode> ans(client->post(NULL, NULL, msg->getBuffer(), msg->strlen(), xsink), xsink);
   if (!ans)
      return NULL;
   // parse JSON-RPC response
   return parseJSONValue(ans->val.String, xsink);
}

static class QoreNode *JRC_call(Object *self, QoreHTTPClient *client, class QoreNode *params, ExceptionSink *xsink)
{
   client->connect(xsink);
   if (xsink->isEvent())
      return NULL;

   // create the outgoing message in JSON-RPC call format
   TempString msg(makeJSONRPC11RequestString(params, xsink));
   if (!msg)
      return NULL;
   // send the message to the server and get the response as an JSON string
   ReferenceHolder<QoreNode> ans(client->post(NULL, NULL, msg->getBuffer(), msg->strlen(), xsink), xsink);
   if (!ans)
      return NULL;
   // parse JSON-RPC response
   return parseJSONValue(ans->val.String, xsink);
}

QoreClass *initJsonRpcClientClass(class QoreClass *http_client)
{
    QoreClass* client = new QoreClass(QDOM_NETWORK, strdup("JsonRpcClient"));
    CID_JSONRPCCLIENT = client->getID();

    client->addDefaultBuiltinBaseClass(http_client);

    client->setConstructor(JRC_constructor);
    client->setCopy((q_copy_t)JRC_copy);
    client->addMethod("callArgs", (q_method_t)JRC_callArgs);
    client->addMethod("call",     (q_method_t)JRC_call);

    return client;
} 
