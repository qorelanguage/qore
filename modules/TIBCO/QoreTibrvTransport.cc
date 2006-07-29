/*
  modules/TIBCO/QoreTibrvTransport.cc

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

#include "QoreTibrvTransport.h"

#include <arpa/inet.h>

QoreTibrvTransport::QoreTibrvTransport(char *service, char *network, char *daemon, char *desc, class ExceptionSink *xsink)
{
   enc = QCS_DEFAULT;
   // create transport (connect to rvd daemon)
   TibrvStatus status = transport.create(service, network, daemon);
   if (status != TIBRV_OK)
   {
      xsink->raiseException("TIBRVTRANSPORT-CONSTRUCTOR-ERROR", "cannot create Rendezvous transport: %s", status.getText());
      return;
   }
   transport.setDescription(desc);
}

int QoreTibrvTransport::hashToMsg(TibrvMsg *msg, class Hash *hash, class ExceptionSink *xsink)
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
int QoreTibrvTransport::valueToField(char *key, class QoreNode *v, TibrvMsg *msg, class ExceptionSink *xsink)
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
      // convert string to transport's encoding if necessary
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

class Hash *QoreTibrvTransport::parseMsg(TibrvMsg *msg, class ExceptionSink *xsink)
{
   class Hash *data = msgToHash(msg, xsink);
   if (xsink->isException())
   {
      if (data)
      {
	 data->dereference(xsink);
	 delete data;
      }
      return NULL;
   }
   
   class Hash *h = new Hash();
   h->setKeyValue("msg", new QoreNode(data), NULL);
   
   const char *str;
   TibrvStatus status = msg->getReplySubject(str);
   if (status == TIBRV_OK)
      h->setKeyValue("replySubject", new QoreNode(str), xsink);
   
   status = msg->getSendSubject(str);
   if (status == TIBRV_OK)
      h->setKeyValue("subject", new QoreNode(str), xsink);

   return h;
}

class Hash *QoreTibrvTransport::msgToHash(TibrvMsg *msg, class ExceptionSink *xsink)
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

      char *key = (char *)field.name;
      // if a null pointer is found, then change to a zero-length string
      if (!key)
	 key = "";

      class QoreNode *val = fieldToNode(&field, xsink);
      if (xsink->isException())
      {
	 h->dereference(xsink);
	 delete h;	 
	 return NULL;
      }
      class QoreNode *ev = h->getKeyValueExistence(key);
      if (ev != (QoreNode *)-1)
      {
	 class QoreNode **evp = h->getKeyValuePtr(key);
	 if (ev->type != NT_LIST)
	 {
	    printf("making list\n");
	    class List *l = new List();
	    l->push(ev);
	    ev = new QoreNode(l);
	 }
	 ev->val.list->push(val);
	 *evp = ev;
      }
      else
	 h->setKeyValue(key, val, NULL);
   }

   return h;
}

class QoreNode *QoreTibrvTransport::fieldToNode(TibrvMsgField *field, class ExceptionSink *xsink)
{
   if (field->count > 1)
      return listToNode(field, xsink);

   class QoreNode *val;
   tibrvLocalData data = field->getData();

   switch (field->getType())
   {
      case TIBRVMSG_STRING:
	 //printd(5, "data.str=%08x\n", data.str);
	 val = new QoreNode(new QoreString((char *)data.str, enc));
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

      case TIBRVMSG_IPPORT16:
	 val = new QoreNode((int64)ntohs(data.ipport16));
	 break;

      case TIBRVMSG_IPADDR32:
      {
	 class QoreString *addr = new QoreString(enc);
	 unsigned int ia = ntohl(data.ipaddr32);
	 unsigned char *uc = (unsigned char*)&ia;
	 addr->sprintf("%d.%d.%d.%d", uc[0], uc[1], uc[2], uc[3]);
	 val = new QoreNode(addr);
	 break;
      }

      case TIBRVMSG_BOOL:
	 val = new QoreNode((bool)data.boolean);
	 break;

      case TIBRVMSG_MSG:
      {
	 //printd(5, "TIBRVMSG_MSG size=%d\n", field->size);
	 TibrvMsg msg(data.msg, TIBRV_FALSE);
	 val = new QoreNode(msgToHash(&msg, xsink));
	 break;
      }

      case TIBRVMSG_DATETIME:
	 val = new QoreNode(new DateTime(data.date.sec));
	 break;

      case TIBRVMSG_OPAQUE:
      {
	 void *ptr = malloc(field->size);
	 memcpy(ptr, data.buf, field->size);
	 val = new QoreNode(new BinaryObject(ptr, field->size));
	 break;
      }

      case TIBRVMSG_XML:
      {
	 class QoreString *str = new QoreString(enc);
	 str->allocate(field->size + 1);
	 memcpy(str->getBuffer(), data.buf, field->size);
	 val = new QoreNode(str);
	 break;
      }

      default:
	 xsink->raiseException("TIBRV-DEMARSHALLING-ERROR", "don't know how to convert type %d", field->getType());
	 break;
   }
   return val;
}

class QoreNode *QoreTibrvTransport::listToNode(TibrvMsgField *field, class ExceptionSink *xsink)
{
   class List *l = new List();
   tibrvLocalData data = field->getData();

   switch (field->getType())
   {
      case TIBRVMSG_I8:
      {
	 char *c = (char *)data.buf;
	 for (unsigned i = 0; i < field->count; i++)
	    l->push(new QoreNode((int64)c[i]));
	 break;
      }

      case TIBRVMSG_U8:
      {
	 unsigned char *c = (unsigned char *)data.buf;
	 for (unsigned i = 0; i < field->count; i++)
	    l->push(new QoreNode((int64)c[i]));
	 break;
      }
      case TIBRVMSG_I16:
      {
	 short *c = (short *)data.buf;
	 for (unsigned i = 0; i < field->count; i++)
	    l->push(new QoreNode((int64)c[i]));
	 break;
      }

      case TIBRVMSG_U16:
      {
	 unsigned short *c = (unsigned short *)data.buf;
	 for (unsigned i = 0; i < field->count; i++)
	    l->push(new QoreNode((int64)c[i]));
	 break;
      }

      case TIBRVMSG_I32:
      {
	 int *c = (int *)data.buf;
	 for (unsigned i = 0; i < field->count; i++)
	    l->push(new QoreNode((int64)c[i]));
	 break;
      }

      case TIBRVMSG_U32:
      {
	 unsigned *c = (unsigned *)data.buf;
	 for (unsigned i = 0; i < field->count; i++)
	    l->push(new QoreNode((int64)c[i]));
	 break;
      }

      case TIBRVMSG_U64:
      case TIBRVMSG_I64:
      {
	 int64 *c = (int64 *)data.buf;
	 for (unsigned i = 0; i < field->count; i++)
	    l->push(new QoreNode(c[i]));
	 break;
      }

      case TIBRVMSG_F32:
      {
	 float *c = (float *)data.buf;
	 for (unsigned i = 0; i < field->count; i++)
	    l->push(new QoreNode((double)c[i]));
	 break;
      }

      case TIBRVMSG_F64:
      {
	 double *c = (double *)data.buf;
	 for (unsigned i = 0; i < field->count; i++)
	    l->push(new QoreNode(c[i]));
	 break;
      }

      case TIBRVMSG_BOOL:
      {
	 tibrv_bool *c = (tibrv_bool *)data.buf;
	 for (unsigned i = 0; i < field->count; i++)
	    l->push(new QoreNode((bool)c[i]));
	 break;
      }

      default:
	 xsink->raiseException("TIBRV-DEMARSHALLING-ERROR", "don't know how to convert a list of type %d", field->getType());
	 delete l;
	 return NULL;
   }
   return new QoreNode(l);
}
