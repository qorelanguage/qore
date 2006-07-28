/*
  modules/TIBCO/QoreTibrvSender.cc

  TIBCO Rendezvous integration to QORE

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006 David Nichols

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

#include <qore/common.h>
#include <qore/support.h>
#include <qore/Exception.h>

#include "QoreTibrvSender.h"

QoreTibrvSender::QoreTibrvSender(char *service, char *network, char *daemon, char *desc, class ExceptionSink *xsink)
{
   enc = QCS_DEFAULT;
   // create transport (connect to rvd daemon)
   TibrvStatus status = transport.create(service, network, daemon);
   if (status != TIBRV_OK)
   {
      xsink->raiseException("TIBRVSENDER-CONSTRUCTOR-ERROR", "cannot create Rendezvous transport: %s", status.getText());
      return;
   }
   transport.setDescription(desc);
}

void QoreTibrvSender::sendSubject(char *subject, class Hash *data, class ExceptionSink *xsink)
{
   TibrvMsg msg;

   TibrvStatus status = msg.setSendSubject(subject);
   if (status != TIBRV_OK)
   {
      xsink->raiseException("TIBCO-INVALID-SUBJECT", "'%s': %s", subject, (char *)status.getText());
      return;
   }

   if (hashToMsg(&msg, data, xsink))
      return;

   status = transport.send(msg);
   if (status != TIBRV_OK)
   {
      xsink->raiseException("TIBRVSENDER-ERROR", "%s", (char *)status.getText());
      return;
   }
   //printd(0, "subject: %s msg=%08x sent OK\n", subject, data);
}

int QoreTibrvSender::hashToMsg(TibrvMsg *msg, class Hash *hash, class ExceptionSink *xsink)
{
   class HashIterator hi(hash);
   
   while (hi.next())
   {
      char *key = hi.getKey();
      if (key && key[0] == '_')
      {
	 xsink->raiseException("TIBRV-MARSHALLING-ERROR", "field names may not begin with '_' (%s)", key);
	 return -1;
      }
      if (valueToField(key, hi.getValue(), msg, xsink))
	 return -1;
   }
   return 0;
}

// add fields to message - we add fields using the name so we can have arrays with different typed elements, etc
// returns 0 for success, -1 for error
int QoreTibrvSender::valueToField(char *key, class QoreNode *v, TibrvMsg *msg, class ExceptionSink *xsink)
{
   //printd(5, "adding %s (%s)\n", key, v ? v->type->name : "null");
   if (is_nothing(v))
      msg->addString(key, NULL);
   else if (v->type == NT_INT)
      msg->addI64(key, v->val.intval);
   else if (v->type == NT_FLOAT)
      msg->addF64(key, v->val.floatval);
   else if (v->type == NT_LIST)
   {
      for (int i = 0; i < v->val.list->size(); i++)
	 if (valueToField(key, v->val.list->retrieve_entry(i), msg, xsink))
	    return -1;
   }
   else if (v->type == NT_STRING)
   {
      // convert string to sender's encoding if necessary
      class QoreString *t;
      if (v->val.String->getEncoding() != enc)
      {
	 t = v->val.String->convertEncoding(enc, xsink);
	 if (!t)
	    return -1;
      }
      else
	 t = v->val.String;
      msg->addString(key, t->getBuffer());
      if (t != v->val.String)
	 delete t;
   }
   else if (v->type == NT_DATE)
   {
      TibrvMsgDateTime dt;
      dt.sec = v->val.date_time->getSeconds();
      msg->addDateTime(key, dt);
   }
   else if (v->type == NT_HASH)
   {
      TibrvMsg m;
      hashToMsg(&m, v->val.hash, xsink);
      if (xsink->isException())
	 return -1;
      msg->addMsg(key, m);
   }
   else if (v->type == NT_BOOLEAN)
      msg->addBool(key, (tibrv_bool)v->val.boolval);
   else if (v->type == NT_BINARY)
      msg->addOpaque(key, v->val.bin->getPtr(), v->val.bin->size());
   else
   {
      xsink->raiseException("TIBRV-MARSHALLING-ERROR", "can't serialize type '%s'", v->type->name);
      return -1;
   }

   return 0;
}
