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
#include <qore/ScopeGuard.h>
#include <qore/minitest.hpp>

#include "QC_TuxedoTypedBuffer.h"
#include "QoreTuxedoTypedBuffer.h"

#include <atmi.h>

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
  if (get_param(params, 0)) {
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
    if (!copy) {
      xsink->outOfMemory();
      return 0;
    }
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
  char* s = n->val.String->getBuffer();
  if (!s) s = "";
  QoreEncoding *enc = QEM.findCreate(s);
  if (!enc) {
    xsink->raiseException("TuxedoTypedBuffer::setStringEncoding()", "Invalid encoding name [ %s ].", s);
    return 0;
  }
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
  if (!copy) {
    xsink->outOfMemory();
    return 0;
  }
  ON_BLOCK_EXIT(free, copy);

  memcpy(copy, buff->buffer, buff->size);
  copy[buff->size] = 0;
  QoreString* s = new QoreString(copy, buff->string_encoding);

  return new QoreNode(s);
}

//------------------------------------------------------------------------------
// http://edocs.bea.com/tuxedo91/rf3c/rf3c23.htm#1021676
// Returns 0 if all is OK or tperrno code.
// Any previous memory block is freed before.
static QoreNode* TUXEDOTYPEDBUFFER_alloc(Object* self, QoreTuxedoTypedBuffer* buff, QoreNode* params, ExceptionSink* xsink)
{
  for (int i = 0; i <= 3; ++i) {
    bool ok;
    if (i == 3) ok = !get_param(params, i);
    else ok = get_param(params, i);
    if (!ok) {
      xsink->raiseException("TuxedoTypedBuffer::alloc", "Three parameters expected (type, subtype, size).");
      return 0;
    }
  }

  QoreNode* n = test_param(params, NT_STRING, 0);
  if (!n) {
    xsink->raiseException("TuxedoTypedBuffer::alloc", "The first parameter, type, needs to be a string.");
    return 0;
  }
  char* type = n->val.String->getBuffer();

  n = test_param(params, NT_STRING, 1);
  if (!n) {
    xsink->raiseException("TuxedoTypedBuffer::alloc", "The second parameter, the subtype, needs to be a string, possibly empty.");
    return 0;
  }
  char* subtype = n->val.String->getBuffer();
  if (subtype && !subtype[0]) subtype = 0;

  n = test_param(params, NT_INT, 2);
  if (!n) {
    xsink->raiseException("TuxedoTypedBuffer::alloc", "The third parameter, size, needs to be an integer.");
    return 0;
  }
  long size = (long)n->val.intval;

  char* res = tpalloc(type, subtype, size);
  if (!res) {
    return new QoreNode((int64)tperrno);
  }
  if (buff->buffer) {
    tpfree(buff->buffer);
  }
  buff->buffer = res;
  buff->size = size;
  return new QoreNode((int64)0);
}

//------------------------------------------------------------------------------
// http://edocs.bea.com/tuxedo/tux91/rf3c/rfc363.htm#1044439
static QoreNode* TUXEDOTYPEDBUFFER_realloc(Object* self, QoreTuxedoTypedBuffer* buff, QoreNode* params, ExceptionSink* xsink)
{
  if (!get_param(params, 0)) {
    xsink->raiseException("TuxedoTypedBuffer::realloc", "One parameter, new size, expected.");
    return 0;
  }
  QoreNode* n = test_param(params, NT_INT, 0);
  if (!n) {
    xsink->raiseException("TuxedoTypedBuffer::realloc", "The parameter, new size, needs to be an integer.");
    return 0;
  }
  long size = (long)n->val.intval;

  if (get_param(params, 1)) {
    xsink->raiseException("TuxedoTypedBuffer::realloc", "Only one parameter expected.");
    return 0;
  }
  if (!buff->buffer) {
    xsink->raiseException("TuxedoTypedBuffer::realloc", "There is no existing buffer to be reallocated.");
    return 0;
  }
  char* res = tprealloc(buff->buffer, size);
  if (!res) {
    return new QoreNode((int64)tperrno);
  }
  buff->buffer = res;
  buff->size = size;
  return new QoreNode((int64)0);
}

//-----------------------------------------------------------------------------
// http://edocs.bea.com/tuxedo/tux91/rf3c/rf3c87.htm#1045809
static QoreNode* TUXEDOTYPEDBUFFER_types(Object* self, QoreTuxedoTypedBuffer* buff, QoreNode* params, ExceptionSink* xsink)
{
  if (get_param(params, 0)) {
    xsink->raiseException("TuxedoTypedBuffer::types", "No parameters expected");
    return 0;
  }

  const int MaxTypeSize = 8;  // see docs
  const int MaxSubtypeSize = 16;
  char type[MaxTypeSize + 100];
  char subtype[MaxSubtypeSize + 100];

  long size =  tptypes(buff->buffer, type, subtype);

  List* l = new List;
  if (size == -1) {
    l->push(new QoreNode((int64)tperrno));
  } else {
    type[MaxTypeSize] = 0;
    subtype[MaxSubtypeSize] = 0;
    l->push(new QoreNode((int64)0));
    l->push(new QoreNode(type));
    l->push(new QoreNode(subtype));
    l->push(new QoreNode((int64)size));
  }

  return new QoreNode(l);
}

//------------------------------------------------------------------------------
static QoreNode* TUXEDOTYPEDBUFFER_dump(Object* self, QoreTuxedoTypedBuffer* buff, QoreNode* params, ExceptionSink* xsink)
{
  printf("TuxedoTypedBuffer %p, buffer %p, size %d\n", buff, buff->buffer, buff->size);
  return 0;
}

//------------------------------------------------------------------------------
QoreClass* initTuxedoTypedBufferClass()
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
  buff->addMethod("alloc", (q_method_t)TUXEDOTYPEDBUFFER_alloc);
  buff->addMethod("realloc", (q_method_t)TUXEDOTYPEDBUFFER_realloc);
  buff->addMethod("types", (q_method_t)TUXEDOTYPEDBUFFER_types);
  buff->addMethod("dump", (q_method_t)TUXEDOTYPEDBUFFER_dump);

  traceout("initTuxedoTypedBufferClass");
  return buff;
}

//-----------------------------------------------------------------------------
#ifdef DEBUG
TEST()
{
  // test clear method
  QoreTuxedoTypedBuffer buff;
  ExceptionSink xsink;

  List* l = new List();
  QoreNode* params = new QoreNode(l);

  QoreNode* res = TUXEDOTYPEDBUFFER_clear(0, &buff, params, &xsink);
  assert(!res);
  assert(!xsink.isException());

  params->deref(&xsink);
  assert(!xsink.isException());
}

TEST()
{
  // clear method with wrong parameters
  QoreTuxedoTypedBuffer buff;
  ExceptionSink xsink;

  List* l = new List();
  l->push(new QoreNode("aaa"));
  QoreNode* params = new QoreNode(l);

  QoreNode* res = TUXEDOTYPEDBUFFER_clear(0, &buff, params, &xsink);
  assert(!res);
  assert(xsink.isException());
  xsink.clear();

  params->deref(&xsink);
  assert(!xsink.isException());
}

TEST()
{
  // test get/set binary
  QoreTuxedoTypedBuffer buff;
  ExceptionSink xsink;

  List* l = new List();
  BinaryObject* bin = new BinaryObject(strdup("abcd"), 5);
  l->push(new QoreNode(bin));

  QoreNode* params = new QoreNode(l);

  QoreNode* res = TUXEDOTYPEDBUFFER_setBinary(0, &buff, params, &xsink);
  assert(!res);
  assert(!xsink.isException());

  params->deref(&xsink);
  assert(!xsink.isException());

  params = new QoreNode(new List);
  
  res = TUXEDOTYPEDBUFFER_getBinary(0, &buff, params, &xsink);
  assert(res);
  assert(!xsink.isException());

  params->deref(&xsink);
  assert(!xsink.isException());

  assert(res->type == NT_BINARY);
  BinaryObject* bin2 = res->val.bin;
  assert(bin2->size() == 5);
  if (memcmp(bin2->getPtr(), "abcd", 5)) {
    assert(0);
  }

  res->deref(&xsink);
  assert(!xsink.isException());
}

TEST()
{
  // binary get/set - invalid argument type
  QoreTuxedoTypedBuffer buff;
  ExceptionSink xsink;

  List* l = new List();
  l->push(new QoreNode("aaa"));

  QoreNode* params = new QoreNode(l);

  QoreNode* res = TUXEDOTYPEDBUFFER_setBinary(0, &buff, params, &xsink);
  assert(!res);
  assert(xsink.isException());
  xsink.clear();

  res = TUXEDOTYPEDBUFFER_getBinary(0, &buff, params, &xsink);
  assert(!res);
  assert(xsink.isException());
  xsink.clear();

  params->deref(&xsink);
  assert(!xsink.isException());
}

TEST()
{
  // binary set/get - too many parameters
  QoreTuxedoTypedBuffer buff;
  ExceptionSink xsink;

  List* l = new List();
  l->push(new QoreNode("aaa"));
  l->push(new QoreNode((int64)123));

  QoreNode* params = new QoreNode(l);

  QoreNode* res = TUXEDOTYPEDBUFFER_setBinary(0, &buff, params, &xsink);
  assert(!res);
  assert(xsink.isException());
  xsink.clear();

  res = TUXEDOTYPEDBUFFER_getBinary(0, &buff, params, &xsink);
  assert(!res);
  assert(xsink.isException());
  xsink.clear();

  params->deref(&xsink);
  assert(!xsink.isException());
}

TEST()
{
  // test get/set binary
  QoreTuxedoTypedBuffer buff;
  ExceptionSink xsink;

  List* l = new List();
  l->push(new QoreNode("abcdefgh"));

  QoreNode* params = new QoreNode(l);

  QoreNode* res = TUXEDOTYPEDBUFFER_setString(0, &buff, params, &xsink);
  assert(!res);
  assert(!xsink.isException());

  params->deref(&xsink);
  assert(!xsink.isException());

  params = new QoreNode(new List);

  res = TUXEDOTYPEDBUFFER_getString(0, &buff, params, &xsink);
  assert(res);
  assert(!xsink.isException());

  params->deref(&xsink);
  assert(!xsink.isException());

  assert(res->type == NT_STRING);
  char* s = res->val.String->getBuffer();
  assert(s);
  if (strcmp(s, "abcdefgh")) {
    assert(false);
  }

  res->deref(&xsink);
  assert(!xsink.isException());
}

TEST()
{
  // test string encoding set
  QoreTuxedoTypedBuffer buff;
  ExceptionSink xsink;
  
  List* l = new List();
  l->push(new QoreNode("ISO-8859-5")); // Cyrilic

  QoreNode* params = new QoreNode(l);

  QoreNode* res = TUXEDOTYPEDBUFFER_setStringEncoding(0, &buff, params, &xsink);
  assert(!res);
  assert(!xsink.isException());

  params->deref(&xsink);
  assert(!xsink.isException());
}

TEST()
{
  // string encoding set - wrong number of parameters
  QoreTuxedoTypedBuffer buff;
  ExceptionSink xsink;

  List* l = new List();
  QoreNode* params = new QoreNode(l);

  QoreNode* res = TUXEDOTYPEDBUFFER_setStringEncoding(0, &buff, params, &xsink);
  assert(!res);
  assert(xsink.isException());
  xsink.clear();

  params->deref(&xsink);
  assert(!xsink.isException());
}

TEST()
{
  // string encoding set - wrong type
  QoreTuxedoTypedBuffer buff;
  ExceptionSink xsink;

  List* l = new List();
  l->push(new QoreNode((int64)123));
  QoreNode* params = new QoreNode(l);

  QoreNode* res = TUXEDOTYPEDBUFFER_setStringEncoding(0, &buff, params, &xsink);
  assert(!res);
  assert(xsink.isException());
  xsink.clear();

  params->deref(&xsink);
  assert(!xsink.isException());
}

TEST()
{
/* this currently does not fail but IMHO it should
  // string necoding set - invalid encoding name
  QoreTuxedoTypedBuffer buff;
  ExceptionSink xsink;

  List* l = new List();
  l->push(new QoreNode("nonexistent"));
  QoreNode* params = new QoreNode(l);

  QoreNode* res = TUXEDOTYPEDBUFFER_setStringEncoding(0, &buff, params, &xsink);
  assert(!res);
  assert(xsink.isException());
  xsink.clear();

  params->deref(&xsink);
  assert(!xsink.isException());
*/
}

TEST()
{

  // string get/set with an encoding
  QoreTuxedoTypedBuffer buff;
  ExceptionSink xsink;

  List* l = new List();
  l->push(new QoreNode("ISO-8859-6")); // Arabic
  QoreNode* params = new QoreNode(l);
  QoreNode* res = TUXEDOTYPEDBUFFER_setStringEncoding(0, &buff, params, &xsink);
  assert(!res);
  assert(!xsink.isException());
  
  params->deref(&xsink);
  assert(!xsink.isException());


  char in_string[] = "abcdefgh";
  in_string[1] = 0xFF;
  in_string[2] = 0xF2;
  in_string[3] = 0xC7;

  // set/get the string
  l = new List();
  l->push(new QoreNode(in_string));

  params = new QoreNode(l);

  res = TUXEDOTYPEDBUFFER_setString(0, &buff, params, &xsink);
  assert(!res);
  assert(!xsink.isException());

  params->deref(&xsink);
  assert(!xsink.isException());

  params = new QoreNode(new List);

  res = TUXEDOTYPEDBUFFER_getString(0, &buff, params, &xsink);
  assert(res);
  assert(!xsink.isException());

  params->deref(&xsink);
  assert(!xsink.isException());

  assert(res->type == NT_STRING);
  char* s = res->val.String->getBuffer();
  assert(s);
  if (strcmp(s, in_string)) {
    assert(false);
  }

  res->deref(&xsink);
  assert(!xsink.isException());

  // change encoding
  l = new List();
  l->push(new QoreNode("UTF-8"));
  params = new QoreNode(l);
  res = TUXEDOTYPEDBUFFER_setStringEncoding(0, &buff, params, &xsink);
  assert(!res);
  assert(!xsink.isException());

  params->deref(&xsink);
  assert(!xsink.isException());

  // set/get the string again
  l = new List();
  l->push(new QoreNode(in_string));

  params = new QoreNode(l);

  res = TUXEDOTYPEDBUFFER_setString(0, &buff, params, &xsink);
  assert(!res);
  assert(!xsink.isException());

  params->deref(&xsink);
  assert(!xsink.isException());

  params = new QoreNode(new List);

  res = TUXEDOTYPEDBUFFER_getString(0, &buff, params, &xsink);
  assert(res);
  assert(!xsink.isException());

  params->deref(&xsink);
  assert(!xsink.isException());

  assert(res->type == NT_STRING);
  s = res->val.String->getBuffer();
  assert(s);
  if (strcmp(s, in_string)) {
    assert(false);
  }

  res->deref(&xsink);
  assert(!xsink.isException());
}

TEST()
{
  // test Qore code
  char* cmd =
    "qore -e '%requires tuxedo\n"
    "$a = new Tuxedo::TuxedoTypedBuffer();\n"

    "$a.setStringEncoding(\"UTF-8\");\n"
    "$a.setString(\"xyz\");\n"
    "$val = $a.getString();\n"
    "if ($val != \"xyz\") exit(11);\n"

    "$bin = $a.getBinary();\n" // get these data as binary
    "$a.setBinary($bin);\n"

    "$a.clear();\n"
    "exit(10);\n'";

  int res = system(cmd);
  res = WEXITSTATUS(res);
  assert(res == 10);
}

TEST()
{
  // alloc(), realloc(), types()
  QoreTuxedoTypedBuffer buff;
  ExceptionSink xsink;

  List* l = new List;
  l->push(new QoreNode("STRING"));
  l->push(new QoreNode(""));
  l->push(new QoreNode((int64)100));
  QoreNode* params = new QoreNode(l);

  QoreNode* res = TUXEDOTYPEDBUFFER_alloc(0, &buff, params, &xsink);
  assert(!xsink);
  assert(res);
  assert(buff.buffer);
  assert(buff.size == 100);
  assert(res->type == NT_INT);
  assert(res->val.intval == 0);

  res->deref(&xsink);
  assert(!xsink);

  params->deref(&xsink);
  assert(!xsink);

  // realloc it
  l = new List;
  l->push(new QoreNode((int64)200));
  params = new QoreNode(l);

  res = TUXEDOTYPEDBUFFER_realloc(0, &buff, params, &xsink);
  assert(!xsink);
  assert(res);
  assert(buff.buffer);
  assert(buff.size == 200);
  assert(res->type == NT_INT);
  assert(res->val.intval == 0);

  res->deref(&xsink);
  assert(!xsink);

  params->deref(&xsink);
  assert(!xsink);

  // use types() to find out
  l = new List;
  params = new QoreNode(l);
  
  res = TUXEDOTYPEDBUFFER_types(0, &buff, params, &xsink);
  assert(!xsink);
  assert(res);
  assert(res->type == NT_LIST);

  QoreNode* item = res->val.list->retrieve_entry(0);
  assert(item);
  assert(item->type == NT_INT);
  assert(item->val.intval == 0);
  assert(res->val.list->size() == 4);

  item = res->val.list->retrieve_entry(1);
  assert(item);
  assert(item->type == NT_STRING);
  char* s = item->val.String->getBuffer();
  if (strcmp(s, "STRING")) {
    assert(false);
  }

  item = res->val.list->retrieve_entry(2);
  assert(item);
  assert(item->type == NT_STRING);
  char* ss = item->val.String->getBuffer();
  assert(!ss || !ss[0]);

  item = res->val.list->retrieve_entry(3);
  assert(item);
  assert(item->type == NT_INT);
  // assert(item->val.intval == 200); - the actual value may be different from what was asked for, e.g. 512

  res->deref(&xsink);
  assert(!xsink);

  params->deref(&xsink);
  assert(!xsink);
}

TEST()
{
  // test alloc(), realloc(), types() members in Qore
  char* cmd = "qore -e '%requires tuxedo\n"
    "$a = new Tuxedo::TuxedoTypedBuffer();\n"
    "$res = $a.alloc(\"STRING\", \"\", 100);\n"
    "if ($res != 0) exit(11);\n"
    "$res = $a.realloc(300);\n"
    "if ($res != 0) exit(11);\n"
    "$res = $a.types();\n"
    "if ($res[0] != 0) exit(11);\n"
    "if ($res[1] != \"STRING\") exit(11);\n"
    "if ($res[2] != \"\") exit(11);\n"
    "exit(10);'";

  int res = system(cmd);
  res = WEXITSTATUS(res);
  assert(res == 10);
}

#endif // DEBUG

// EOF


