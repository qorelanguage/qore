/*
  modules/Tuxedo/QC_TuxedoTypedBuffer.cc

  Tuxedo integration to QORE

  Qore Programming Language

  Copyright (C) 2006 Qore Technologies

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
#include <qore/QoreClass.h>
#include <qore/params.h>
#include <qore/charset.h>

#include "QC_TuxedoTypedBuffer.h"
#include "QoreTuxedoTypedBuffer.h"

int CID_TUXEDOTYPEDBUFFER;

//------------------------------------------------------------------------------
static void getTuxedoTypedBuffer(void* obj)
{
  ((QoreTuxedoTypedBuffer*)obj)->ROreference();
}

static void releaseTuxedoTypedBuffer(void* obj)
{
  ((QoreTuxedoTypedBuffer*)obj)->deref();
}

//------------------------------------------------------------------------------
// No parameters.
static void TUXEDOTYPEDBUFFER_constructor(Object *self, QoreNode *params, ExceptionSink *xsink)
{
  tracein("TUXEDOTYPEDBUFFER_constructor");

  if (get_param(params, 0)) {
    xsink->raiseException("TUXEDO-TYPED_BUFFER-CONSTRUCTOR", "No parameters are allowed in the constructor.");
    return;
  }

  QoreTuxedoTypedBuffer* buff = new QoreTuxedoTypedBuffer();
  self->setPrivate(CID_TUXEDOTYPEDBUFFER, buff, getTuxedoTypedBuffer, releaseTuxedoTypedBuffer);

  traceout("TUXEDOTYPEDBUFFER_constructor");
}

//------------------------------------------------------------------------------
static void TUXEDOTYPEDBUFFER_destructor(Object *self, QoreTuxedoTypedBuffer* buff, ExceptionSink *xsink)
{
  tracein("TUXEDOTYPEDBUFFER_destructor");
  buff->deref();
  traceout("TUXEDOTYPEDBUFFER_destructor");
}

//------------------------------------------------------------------------------
static void TUXEDOTYPEDBUFFER_copy(Object *self, Object *old, QoreTuxedoTypedBuffer* buff, ExceptionSink *xsink)
{
  xsink->raiseException("TUXEDO-TYPED_BUFFER-COPY", "copying Tuxedo::TuxedoTypedBuffer objects is not supported.");
}

//-----------------------------------------------------------------------------
static QoreNode* TUXEDOTYPEDBUFFER_clear(Object* self, QoreTuxedoTypedBuffer* buff, QoreNode* params, ExceptionSink* xsink)
{
  if (get_param(params, 1)) {
    xsink->raiseException("TuxedoTypedBuffer::clear()", "No parameter expected.");
    return 0;
  }
  buff->clear();
  return 0;
}

//------------------------------------------------------------------------------
static QoreNode* TUXEDOTYPEDBUFFER_setBinary(Object* self, QoreTuxedoTypedBuffer* buff, QoreNode* params, ExceptionSink* xsink)
{
  char* all_params_err = "One to three parameters expected: binary object passed by reference, optional type string, optional subtype string.";
  if (!get_param(params, 0)) {
    xsink->raiseException("TuxedoTypedBuffer::setBinary()", all_params_err);
    return 0;
  }

  QoreNode* n = test_param(params, NT_BINARY, 0);
  if (!n) {
    xsink->raiseException("TuxedoTypedBuffer::setBinary()", "The first parameter needs to be instance of Binary passed by reference.");
    return 0;
  }
  BinaryObject* bin = n->val.bin;

  char* type = "CARRAY";
  char* subtype = 0;
  if (get_param(params, 1)) {
    n = test_param(params, NT_STRING, 1);
    if (!n) {
      xsink->raiseException("TuxedoTypedBuffer::setBinary()", "The second parameter, type, needs to be a string.");
      return 0;
    }
    type = n->val.String->getBuffer();

    if (get_param(params, 2)) {
      n = test_param(params, NT_STRING, 2);
      if (!n) {
        xsink->raiseException("TuxedoTypedBuffer::setBinary()", "The third parameter needs to be a string.");
        return 0;
      }
      subtype = n->val.String->getBuffer();
    }
  }

  buff->setBinary(bin, type, subtype, xsink);
  return 0;
}

//------------------------------------------------------------------------------
static QoreNode* TUXEDOTYPEDBUFFER_getBinary(Object* self, QoreTuxedoTypedBuffer* buff, QoreNode* params,ExceptionSink* xsink)
{
  if (get_param(params, 0)) {
    xsink->raiseException("TuxedoTypedBuffer::getBinary()", "No parameter expected.");
    return 0;
  }
  void* copy = 0;
  long size = 0;
  if (buff->buffer) {
    copy = malloc(buff->size);
    memcpy(copy, buff->buffer, buff->size);
    size = buff->size;
  }
  BinaryObject* bin = new BinaryObject(copy, size);
  return new QoreNode(bin);
}

//------------------------------------------------------------------------------
static QoreNode* TUXEDOTYPEDBUFFER_setStringEncoding(Object* self, QoreTuxedoTypedBuffer* buff, QoreNode* params, ExceptionSink* xsink)
{
  for (int i = 0; i <= 1; ++i) {
    bool ok;
    if (i == 1) ok = !get_param(params, i);
    else ok = get_param(params, i);
    if (!ok) {
      xsink->raiseException("TuxedoTypedBuffer::setStringEncoding()", "One parameter expected: string encoding as string.");
      return 0;
    }
  }

  QoreNode* n = test_param(params, NT_STRING, 0);
  if (!n) {
    xsink->raiseException("TuxedoTypedBuffer::setStringEncoding()", "The first parameter, string encoding, needs to be a string.");
    return 0;
  }
  QoreEncoding *enc = QEM.findCreate(n->val.String->getBuffer());
  buff->setStringEncoding(enc);
  return 0;
}

//------------------------------------------------------------------------------
static QoreNode* TUXEDOTYPEDBUFFER_setString(Object* self, QoreTuxedoTypedBuffer* buff, QoreNode* params, ExceptionSink* xsink)
{
  char* all_params_err = "One to three parameters expected: string possibly passed by reference, optional type string, optional subtype string.";
  if (!get_param(params, 0)) {
    xsink->raiseException("TuxedoTypedBuffer::setString()", all_params_err);
    return 0;
  }

  QoreNode* n = test_param(params, NT_STRING, 0);
  if (!n) {
    xsink->raiseException("TuxedoTypedBuffer::setString()", "The first parameter needs to be an string, possibly passed by reference.");
    return 0;
  }
  QoreString encoded_string(n->val.String->getBuffer(), buff->string_encoding);
  char* s = encoded_string.getBuffer();
  if (!s) s = "";

  char* type = "STRING";
  char* subtype = 0;
  if (get_param(params, 1)) {
    n = test_param(params, NT_STRING, 1);
    if (!n) {
      xsink->raiseException("TuxedoTypedBuffer::setString()", "The second parameter, type, needs to be a string.");
      return 0;
    }
    type = n->val.String->getBuffer();

    if (get_param(params, 2)) {
      n = test_param(params, NT_STRING, 2);
      if (!n) {
        xsink->raiseException("TuxedoTypedBuffer::setString()", "The third parameter needs to be a string.");
        return 0;
      }
      subtype = n->val.String->getBuffer();
    }
  }

  buff->setString(s, type, subtype, xsink);
  return 0;

}

//-----------------------------------------------------------------------------
static QoreNode* TUXEDOTYPEDBUFFER_getString(Object* self, QoreTuxedoTypedBuffer* buff, QoreNode* params, ExceptionSink* xsink)
{
  if (get_param(params, 0)) {
    xsink->raiseException("TuxedoTypedBuffer::getString()", "No parameters expected.");
    return 0;
  }
  if (!buff->buffer) {
    xsink->raiseException("TuxedoTypedBuffer::getString()", "Buffer not allocated.");
    return 0;
  }
  if (buff->size == 0) {
    return new QoreNode("");
  }
  char* copy = (char*)malloc(buff->size + 1);
  memcpy(copy, buff->buffer, buff->size);
  copy[buff->size] = 0;
  QoreString* s = new QoreString(copy, buff->string_encoding);
  free(copy);

  return new QoreNode(s);
}


//------------------------------------------------------------------------------
class QoreClass* initTuxedoTypedBufferClass()
{
  tracein("initTuxedoTypedBufferClass");
  QoreClass* buff = new QoreClass(QDOM_NETWORK, strdup("TuxedoTypedBuffer"));
  CID_TUXEDOTYPEDBUFFER = buff->getID();  

  buff->setConstructor((q_constructor_t)TUXEDOTYPEDBUFFER_constructor);
  buff->setDestructor((q_destructor_t)TUXEDOTYPEDBUFFER_destructor);
  buff->setCopy((q_copy_t)TUXEDOTYPEDBUFFER_copy);
  buff->addMethod("clear", (q_method_t)TUXEDOTYPEDBUFFER_clear);
  buff->addMethod("setStringEncoding", (q_method_t)TUXEDOTYPEDBUFFER_setStringEncoding);
  buff->addMethod("setBinary", (q_method_t)TUXEDOTYPEDBUFFER_setBinary);
  buff->addMethod("getBinary", (q_method_t)TUXEDOTYPEDBUFFER_getBinary);
  buff->addMethod("setString", (q_method_t)TUXEDOTYPEDBUFFER_setString);
  buff->addMethod("getString", (q_method_t)TUXEDOTYPEDBUFFER_getString);

  traceout("initTuxedoTypedBufferClass");
  return buff;
}


// EOF


