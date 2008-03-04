/*
  modules/TIBCO/QoreTibrvTransport.cc

  TIBCO Rendezvous integration to QORE

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols

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

#include "QoreTibrvTransport.h"

#include <arpa/inet.h>
#include <stdlib.h>

QoreTibrvTransport::QoreTibrvTransport(const char *desc, const char *service, const char *network, const char *daemon, class ExceptionSink *xsink)
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

int QoreTibrvTransport::hashToMsg(TibrvMsg *msg, const QoreHashNode *hash, ExceptionSink *xsink)
{
   ConstHashIterator hi(hash);
   
   while (hi.next())
   {
      const char *key = hi.getKey();
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
int QoreTibrvTransport::valueToField(const char *key, const AbstractQoreNode *v, TibrvMsg *msg, class ExceptionSink *xsink)
{
   //printd(5, "adding %s (%s)\n", key, v ? v->getTypeName() : "null");
   if (is_nothing(v)) {
      msg->addString(key, NULL);
      return 0;
   }

   qore_type_t vtype = v->getType();
   if (vtype == NT_INT) {
      msg->addI64(key, reinterpret_cast<const QoreBigIntNode *>(v)->val);
      return 0;
   }

   if (vtype == NT_FLOAT) {
      msg->addF64(key, reinterpret_cast<const QoreFloatNode *>(v)->f);
      return 0;
   }

   if (vtype == NT_LIST) {
      const QoreListNode *l = reinterpret_cast<const QoreListNode *>(v);
      for (qore_size_t i = 0; i < l->size(); i++)
	 if (valueToField(key, l->retrieve_entry(i), msg, xsink))
	    return -1;
      return 0;
   }

   if (vtype == NT_STRING) {
      const QoreStringNode *str = reinterpret_cast<const QoreStringNode *>(v);
      TempEncodingHelper t(str, enc, xsink);
      if (*xsink)
	 return -1;

      msg->addString(key, t->getBuffer());
      return 0;
   }

   if (vtype == NT_DATE) {
      const DateTimeNode *date = reinterpret_cast<const DateTimeNode *>(v);
      TibrvMsgDateTime dt;
      dt.sec = date->getEpochSeconds();
      msg->addDateTime(key, dt);
      return 0;
   }

   if (vtype == NT_HASH) {
      const QoreHashNode *h = reinterpret_cast<const QoreHashNode *>(v);
      //check if it's a type-encoded hash
      const AbstractQoreNode *t;
      if (h->size() == 2 && (t = h->getKeyValue("^type^")) && (t->getType() == NT_STRING))
	 doEncodedType(msg, key, (reinterpret_cast<const QoreStringNode *>(t))->getBuffer(), h->getKeyValue("^value^"), xsink);
      else
      {
	 TibrvMsg m;
	 hashToMsg(&m, h, xsink);
	 if (xsink->isException())
	    return -1;
	 msg->addMsg(key, m);
      }
      return 0;
   }

   if (vtype == NT_BOOLEAN) {
      msg->addBool(key, (tibrv_bool)(reinterpret_cast<const QoreBoolNode *>(v)->getValue()));
      return 0;
   }
   
   if (vtype == NT_BINARY) {
      const BinaryNode *b = reinterpret_cast<const BinaryNode *>(v);
      msg->addOpaque(key, b->getPtr(), b->size());
      return 0;
   }

   xsink->raiseException("TIBRV-MARSHALLING-ERROR", "can't serialize type '%s'", v->getTypeName());
   return -1;
}

QoreHashNode *QoreTibrvTransport::parseMsg(TibrvMsg *msg, class ExceptionSink *xsink)
{
   ReferenceHolder<QoreHashNode> data(msgToHash(msg, xsink), xsink);
   if (xsink->isException())
      return 0;
   
   QoreHashNode *h = new QoreHashNode();
   h->setKeyValue("msg", data.release(), NULL);
   
   const char *str;
   TibrvStatus status = msg->getReplySubject(str);
   if (status == TIBRV_OK)
      h->setKeyValue("replySubject", new QoreStringNode(str), xsink);
   
   status = msg->getSendSubject(str);
   if (status == TIBRV_OK)
      h->setKeyValue("subject", new QoreStringNode(str), xsink);

   return h;
}

QoreHashNode *QoreTibrvTransport::msgToHash(TibrvMsg *msg, class ExceptionSink *xsink)
{
   tibrv_u32 len;
   TibrvStatus status = msg->getNumFields(len);
   if (status != TIBRV_OK)
   {
      xsink->raiseException("TIBRV-DEMARSHALLING-ERROR", status.getText());
      return 0;
   }

   TibrvMsgField field;

   ReferenceHolder<QoreHashNode> h(new QoreHashNode(), xsink);
   for (unsigned i = 0; i < len; i++)
   {
      status = msg->getFieldByIndex(field, i);
      if (status != TIBRV_OK)
      {
	 xsink->raiseException("TIBRV-DEMARSHALLING-ERROR", status.getText());
	 return 0;
      }

      const char *key = field.name;
      // if a null pointer is found, then change to a zero-length string
      if (!key)
	 key = "";

      AbstractQoreNode *val = fieldToNode(&field, xsink);
      if (xsink->isException())
	 return 0;

      bool exists;
      AbstractQoreNode *ev = h->getKeyValueExistence(key, exists);
      if (exists)
      {
	 AbstractQoreNode **evp = h->getKeyValuePtr(key);
	 QoreListNode *l = dynamic_cast<QoreListNode *>(ev);
	 if (!l) {
	    //printd(5, "QoreTibrvTransport::msgToHash() making list\n");
	    l = new QoreListNode();
	    l->push(ev);
	    ev = l;
	 }
	 l->push(val);
	 *evp = ev;
      }
      else
	 h->setKeyValue(key, val, NULL);
   }

   return h.release();
}

AbstractQoreNode *QoreTibrvTransport::fieldToNode(TibrvMsgField *field, class ExceptionSink *xsink)
{
   if (field->count > 1)
      return listToNode(field, xsink);

   tibrvLocalData data = field->getData();

   //printd(5, "QoreTibrvTransport::fieldToNode() type=%d\n", field->getType());

   switch (field->getType())
   {
      case TIBRVMSG_STRING:
	 return new QoreStringNode((char *)data.str, enc);

      case TIBRVMSG_I8:
	 return new QoreBigIntNode(data.i8);

      case TIBRVMSG_U8:
	 return new QoreBigIntNode(data.u8);

      case TIBRVMSG_I16:
	 return new QoreBigIntNode(data.i16);

      case TIBRVMSG_U16:
	 return new QoreBigIntNode(data.u16);

      case TIBRVMSG_I32:
	 return new QoreBigIntNode(data.i32);

      case TIBRVMSG_U32:
	 return new QoreBigIntNode(data.u32);

      case TIBRVMSG_I64:
	 return new QoreBigIntNode(data.i64);

      case TIBRVMSG_U64: // WARNING: we lose precision here!!!
	 return new QoreBigIntNode((int64)data.u64);

      case TIBRVMSG_F32:
	 return new QoreFloatNode((double)data.f32);
 
      case TIBRVMSG_F64:
	 return new QoreFloatNode(data.f64);

      case TIBRVMSG_IPPORT16:
	 return new QoreBigIntNode(ntohs(data.ipport16));

      case TIBRVMSG_IPADDR32:
      {
	 class QoreStringNode *addr = new QoreStringNode(enc);
	 unsigned int ia = ntohl(data.ipaddr32);
	 //printd(0, "ipaddr32: data=%u conv=%u\n", data.ipaddr32, ia);
	 unsigned char *uc = (unsigned char*)&ia;
	 addr->sprintf("%d.%d.%d.%d", (int)uc[3], (int)uc[2], (int)uc[1], (int)uc[0]);
	 //printd(5, "addr=%s\n", addr->getBuffer());
	 return addr;
      }

      case TIBRVMSG_BOOL:
	 return get_bool_node((bool)data.boolean);

      case TIBRVMSG_MSG:
      {
	 //printd(5, "TIBRVMSG_MSG size=%d\n", field->size);
	 TibrvMsg msg(data.msg, TIBRV_FALSE);
	 return msgToHash(&msg, xsink);
      }

      case TIBRVMSG_DATETIME:
	 return new DateTimeNode((int64)data.date.sec);

      case TIBRVMSG_OPAQUE:
      {
	 void *ptr = malloc(field->size);
	 memcpy(ptr, data.buf, field->size);
	 return new BinaryNode(ptr, field->size);
      }

      case TIBRVMSG_XML:
      {
	 class QoreStringNode *str = new QoreStringNode(enc);
	 // ensure that xml string is terminated with a null character in case it wasn't sent
	 int len = field->size;
	 if (!((char *)data.buf)[len - 1])
	    len--;
	 str->terminate(len);
	 memcpy((char *)str->getBuffer(), data.buf, len);
	 return str;
      }
   }

   xsink->raiseException("TIBRV-DEMARSHALLING-ERROR", "don't know how to convert type %d", field->getType());
   return 0;
}

class AbstractQoreNode *QoreTibrvTransport::listToNode(TibrvMsgField *field, class ExceptionSink *xsink)
{
   ReferenceHolder<QoreListNode> l(new QoreListNode(), xsink);
   tibrvLocalData data = field->getData();

   switch (field->getType())
   {
      case TIBRVMSG_I8:
      {
	 char *c = (char *)data.buf;
	 for (unsigned i = 0; i < field->count; i++)
	    l->push(new QoreBigIntNode(c[i]));
	 break;
      }

      case TIBRVMSG_U8:
      {
	 unsigned char *c = (unsigned char *)data.buf;
	 for (unsigned i = 0; i < field->count; i++)
	    l->push(new QoreBigIntNode(c[i]));
	 break;
      }
      case TIBRVMSG_I16:
      {
	 short *c = (short *)data.buf;
	 for (unsigned i = 0; i < field->count; i++)
	    l->push(new QoreBigIntNode(c[i]));
	 break;
      }

      case TIBRVMSG_U16:
      {
	 unsigned short *c = (unsigned short *)data.buf;
	 for (unsigned i = 0; i < field->count; i++)
	    l->push(new QoreBigIntNode(c[i]));
	 break;
      }

      case TIBRVMSG_I32:
      {
	 int *c = (int *)data.buf;
	 for (unsigned i = 0; i < field->count; i++)
	    l->push(new QoreBigIntNode(c[i]));
	 break;
      }

      case TIBRVMSG_U32:
      {
	 unsigned *c = (unsigned *)data.buf;
	 for (unsigned i = 0; i < field->count; i++)
	    l->push(new QoreBigIntNode(c[i]));
	 break;
      }

      case TIBRVMSG_U64:
      case TIBRVMSG_I64:
      {
	 int64 *c = (int64 *)data.buf;
	 for (unsigned i = 0; i < field->count; i++)
	    l->push(new QoreBigIntNode(c[i]));
	 break;
      }

      case TIBRVMSG_F32:
      {
	 float *c = (float *)data.buf;
	 for (unsigned i = 0; i < field->count; i++)
	    l->push(new QoreFloatNode((double)c[i]));
	 break;
      }

      case TIBRVMSG_F64:
      {
	 double *c = (double *)data.buf;
	 for (unsigned i = 0; i < field->count; i++)
	    l->push(new QoreFloatNode(c[i]));
	 break;
      }

      case TIBRVMSG_BOOL:
      {
	 tibrv_bool *c = (tibrv_bool *)data.buf;
	 for (unsigned i = 0; i < field->count; i++)
	    l->push(get_bool_node((bool)c[i]));
	 break;
      }

      default:
	 xsink->raiseException("TIBRV-DEMARSHALLING-ERROR", "don't know how to convert a list of type %d", field->getType());
	 return 0;
   }
   return l.release();
}

int QoreTibrvTransport::doEncodedType(TibrvMsg *msg, const char *key, const char *type, const AbstractQoreNode *val, class ExceptionSink *xsink)
{
   if (type[0] == 'i')
   {
      type++;
      if (!strcmp(type, "8"))
      {
	 msg->addI8(key, val ? val->getAsInt() : 0);
	 return 0;
      }
      if (!strcmp(type, "16"))
      {
	 msg->addI16(key, val ? val->getAsInt() : 0);
	 return 0;
      }
      if (!strcmp(type, "32"))
      {
	 msg->addI32(key, val ? val->getAsInt() : 0);
	 return 0;
      }
      if (!strcmp(type, "64"))
      {
	 msg->addI64(key, val ? val->getAsBigInt() : 0);
	 return 0;
      }
      else if (!strcmp(type, "pport16"))
      {
	 msg->addIPPort16(key, htons(val ? val->getAsInt() : 0));
	 return 0;
      }
      else if (!strcmp(type, "paddr32"))
      {
	 const QoreStringNode *vstr = dynamic_cast<const QoreStringNode *>(val);
	 if (!vstr) {
	    xsink->raiseException("TIBRV-MARSHALLING-ERROR", "can't serialize tibrv type 'ipaddr32' from qore type '%s' (expecting string)", val ? val->getTypeName() : "NOTHING");
	    return -1;
	 }

	 // assuming format "#.#.#.#"
	 int addr[4] = { 0, 0, 0, 0 };
	 int i = 0;
	 const char *c;
	 const char *buf = vstr->getBuffer();
	 QoreString str;
	 while ((c = strchr(buf, '.')))
	 {
	    str.terminate(0);
	    str.concat(buf, c - buf);
	    addr[i] = atoi(str.getBuffer());
	    //printd(5, "addr[%d]=%d\n", i, addr[i]);
	    i++;
	    if (i == 3)
	    {
	       addr[i] = atoi(c + 1);
	       //printd(5, "addr[%d]=%d\n", i, addr[i]);
	       break;
	    }
	    buf = c + 1;
	 }
	 // put it network byte order directly
	 i = addr[0] | addr[1] << 8 | addr[2] << 16 | addr[3] << 24;
	 msg->addIPAddr32(key, i);
	 
	 return 0;
      }
   }
   else if (type[0] == 'u')
   {   
      type++;
      if (!strcmp(type, "8"))
      {
	 msg->addU8(key, val ? val->getAsInt() : 0);
	 return 0;
      }
      
      if (!strcmp(type, "16"))
      {
	 msg->addU16(key, val ? val->getAsInt() : 0);
	 return 0;
      }
      
      if (!strcmp(type, "32"))
      {
	 msg->addU32(key, val ? val->getAsInt() : 0);
	 return 0;
      }
      
      if (!strcmp(type, "64"))
      {
	 msg->addU64(key, val ? val->getAsBigInt() : 0);
	 return 0;
      }
   }
   else if (type[0] == 'f')
   {
      type++;
      if (!strcmp(type, "32"))
      {
	 msg->addF32(key, val ? val->getAsFloat() : 0.0);
	 return 0;
      }
      
      if (!strcmp(type, "64"))
      {
	 msg->addF64(key, val ? val->getAsFloat() : 0.0);
	 return 0;
      }
   }
   else if (!strcmp(type, "bool"))
   {
      msg->addBool(key, (tibrv_bool)(val ? val->getAsBool() : false));
      return 0;
   }
   else if (!strcmp(type, "xml"))
   {
      const QoreStringNode *str = dynamic_cast<const QoreStringNode *>(val);
      if (!str)
      {
	 xsink->raiseException("TIBRV-MARSHALLING-ERROR", "can't serialize tibrv type 'xml' from qore type '%s' (need string)", val ? val->getTypeName() : "NOTHING");
	 return -1;
      }

      msg->addXml(key, str->getBuffer(), str->strlen() + 1);
      return 0;
   }

   xsink->raiseException("TIBRV-MARSHALLING-ERROR", "don't know how to serialize tibrv type '%s'", type);
   return -1;
}
