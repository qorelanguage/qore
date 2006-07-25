/*
  modules/TIBCO/rv.cc

  TIBCO Rendezvous integration to QORE

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

#include <qore/config.h>
#include <qore/support.h>

#include "qore-rv.h"

#include <stdio.h>

class QoreNode *tibrvmsg_field_to_node(TibrvMsgField *field, class ExceptionSink *xsink)
{
   class QoreNode *val;

   tibrvLocalData data = field->getData();
   switch (field->getType())
   {
      case TIBRVMSG_STRING:
	 val = new QoreNode(data.str);
	 break;

      case TIBRVMSG_I8:
	 val = new QoreNode((int64)data.i8);
	 break;

      case TIBRVMSG_U8:
	 val = new QoreNode((int64)data.u8);
	 break;

      case TIBRVMSG_I16:
	 val = new QoreNode((int64)data.i16);
	 break;

      case TIBRVMSG_U16:
	 val = new QoreNode((int64)data.u16);
	 break;

      case TIBRVMSG_I32:
	 val = new QoreNode((int64)data.i32);
	 break;

      case TIBRVMSG_U32:
	 val = new QoreNode((int64)data.u32);
	 break;

      case TIBRVMSG_I64:
	 val = new QoreNode((int64)data.i64);
	 break;

      case TIBRVMSG_U64:
	 val = new QoreNode((int64)data.u64);
	 break;

      case TIBRVMSG_F32:
	 val = new QoreNode((double)data.f32);
	 break;

      case TIBRVMSG_F64:
	 val = new QoreNode(data.f64);
	 break;

      case TIBRVMSG_BOOL:
	 val = new QoreNode((bool)data.boolean);
	 break;

      case TIBRVMSG_MSG:
      {
	 TibrvMsg msg(data.msg, TIBRV_FALSE);
	 val = new QoreNode(tibrvmsg_to_hash(&msg, xsink));
	 break;
      }

      case TIBRVMSG_DATETIME:
	 val = new QoreNode(new DateTime(data.date.sec));
	 break;

      default:
	 xsink->raiseException("TIBRV-DEMARSHALLING-ERROR", "don't know how to convert type %d", field->getType());
	 break;
   }
   return val;
}

class Hash *tibrvmsg_to_hash(TibrvMsg *msg, class ExceptionSink *xsink)
{
   tibrv_u32 len;
   TibrvStatus status = msg->getNumFields(len);
   if (status != TIBRV_OK)
   {
      xsink->raiseException("TIBRV-DEMARSHALLING-ERROR", (char *)status.getText());
      return NULL;
   }

   TibrvMsgField field;

   class Hash *h = new Hash();
   for (unsigned i = 0; i < len; i++)
   {
      status = msg->getFieldByIndex(field, i);
      if (status != TIBRV_OK)
      {
	 h->dereference(xsink);
	 delete h;
	 xsink->raiseException("TIBRV-DEMARSHALLING-ERROR", (char *)status.getText());
	 return NULL;
      }

      class QoreNode *val = tibrvmsg_field_to_node(&field, xsink);
      if (xsink->isException())
      {
	 h->dereference(xsink);
	 delete h;	 
	 return NULL;
      }
      class QoreNode *ev = h->getKeyValueExistence((char *)field.name);
      if (ev != (QoreNode *)-1)
      {
	 class QoreNode **evp = h->getKeyValuePtr((char *)field.name);
	 if (ev->type != NT_LIST)
	 {
	    class List *l = new List();
	    l->push(ev);
	    ev = new QoreNode(l);
	 }
	 ev->val.list->push(val);
	 *evp = ev;
      }
      else
	 h->setKeyValue((char *)field.name, val, NULL);
   }

   return h;
}

void hash_to_tibrvmsg(TibrvMsg *msg, class Hash *hash, class ExceptionSink *xsink)
{
   class HashIterator hi(hash);
   
   while (hi.next())
   {
      char *key = hi.getKey();
      class QoreNode *v = hi.getValue();
      if (!v)
	 msg->updateString(key, NULL);
      else if (v->type == NT_STRING)
	 msg->updateString(key, v->val.String->getBuffer());
      else if (v->type == NT_INT)
	 msg->updateI64(key, v->val.intval);
      else if (v->type == NT_FLOAT)
	 msg->updateF64(key, v->val.floatval);
      else if (v->type == NT_DATE)
      {
	 TibrvMsgDateTime dt;
	 dt.sec = v->val.date_time->getSeconds();
	 msg->updateDateTime(key, dt);
      }
      else if (v->type == NT_HASH)
      {
	 TibrvMsg m;
	 hash_to_tibrvmsg(&m, v->val.hash, xsink);
	 if (xsink->isException())
	    return;
	 msg->updateMsg(key, m);
      }
      else if (v->type == NT_BOOLEAN)
	 msg->updateBool(key, (tibrv_bool)v->val.boolval);
      else
      {
	 xsink->raiseException("TIBRV-MARSHALLING-ERROR", "can't serialize type '%s'", v->type->name);
	 return;
      }
   }
}
