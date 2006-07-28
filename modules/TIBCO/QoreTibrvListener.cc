/*
  modules/TIBCO/QoreTibrvListener.cc

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

#include "QoreTibrvListener.h"

QoreTibrvListener::QoreTibrvListener(char *subject, char *service, char *network, char *daemon, char *desc, class ExceptionSink *xsink)
{
   // set default encoding
   enc = QCS_DEFAULT;
   callback = new QoreTibrvMsgCallback(this);

   // create queue
   TibrvStatus status = queue.create();
   if (status != TIBRV_OK)
   {
      xsink->raiseException("TIBRVLISTENER-CONSTRUCTOR-ERROR", "cannot create queue: %s", status.getText());
      return;
   }
   
   // create transport (connect to rvd daemon)
   status = transport.create(service, network, daemon);
   if (status != TIBRV_OK)
   {
      xsink->raiseException("TIBRVLISTENER-CONSTRUCTOR-ERROR", "cannot create Rendezvous transport: %s", status.getText());
      return;
   }
   transport.setDescription(desc);
   
   // create listener
   status = listener.create(&queue, callback, &transport, subject);
   if (status != TIBRV_OK)
   {
      xsink->raiseException("TIBRVLISTENER-CONSTRUCTOR-ERROR", "cannot create listener on %s: %s", subject, status.getText());
      return;
   }  
}

class Hash *QoreTibrvListener::getMessage(int64 timeout, class ExceptionSink *xsink)
{
   TibrvStatus status = queue.timedDispatch(timeout);

   if (status == TIBRV_TIMEOUT)
      return NULL;

   if (status != TIBRV_OK)
   {
      xsink->raiseException("TIBRVLISTENER-GETMESSAGE-ERROR", (char *)status.getText());
      return NULL;
   }

   return callback->getMessage(xsink);
}

class Hash *QoreTibrvListener::msgToHash(TibrvMsg *msg, class ExceptionSink *xsink)
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

class QoreNode *QoreTibrvListener::fieldToNode(TibrvMsgField *field, class ExceptionSink *xsink)
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

      default:
	 xsink->raiseException("TIBRV-DEMARSHALLING-ERROR", "don't know how to convert type %d", field->getType());
	 break;
   }
   return val;
}

class QoreNode *QoreTibrvListener::listToNode(TibrvMsgField *field, class ExceptionSink *xsink)
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
