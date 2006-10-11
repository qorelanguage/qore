/*
  modules/Tuxedo/low_level_api.cc

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
#include <qore/Exception.h>
#include <qore/Namespace.h>
#include <qore/BuiltinFunctionList.h>
#include <qore/params.h>

#include "low_level_api.h"
#include "QoreTuxedoTypedBuffer.h"
#include "QC_TuxedoTypedBuffer.h"

#include <atmi.h>

const int64 OK = 0;

//------------------------------------------------------------------------------
// n - known as object
static QoreTuxedoTypedBuffer* node2typed_buffer(QoreNode* n, char* func_name, ExceptionSink* xsink)
{
  if (!n->val.object) {
    xsink->raiseException(func_name, "Expected instance of Tuxedo::TuxedoTypedBuffer, NULL found.");
    return 0;
  }
  if (n->val.object->getClass()->getID() != CID_TUXEDOTYPEDBUFFER) {
    xsink->raiseException(func_name, "Type mismatch: expected instance of Tuxedo::TuxedoTypedBuffer class.");
    return 0;
  }
  // this should be safe now
  QoreTuxedoTypedBuffer* buff = (QoreTuxedoTypedBuffer*)(n->val.object);
  return buff;
}

//------------------------------------------------------------------------------
// http://edocs.bea.com/tuxedo/tux91/rf3c/rf3c28.htm#1040017
static QoreNode* f_tpchkauth(QoreNode* params, ExceptionSink* xsink)
{
  if (get_param(params, 0)) {
    xsink->raiseException("tpchkauth", "No parameters expected.");
    return 0;
  }
  if (tpchkauth() == -1) {
    return new QoreNode((int64)tperrno);
  } else {
    return new QoreNode(OK);
  }
}

//-----------------------------------------------------------------------------
// http://edocs.bea.com/tuxedo91/rf3c/rf3c23.htm#1021676
// The difference is that instead of returning char* it 
// fills instance of Tuxedo::TuxedoTypedBuffer in the last parameter.
// Returns 0 if all is OK or tperrno code. The last parameter
// needs to be passed by reference.
static QoreNode* f_tpalloc(QoreNode* params, ExceptionSink* xsink)
{
  for (int i = 0; i <= 4; ++i) {
    bool ok;
    if (i == 4) ok = !get_param(params, i);
    else ok = get_param(params, i);
    if (!ok) {
      xsink->raiseException("tpalloc", "Four parameters expected (type, subtype, size, out-typed-buffer).");
      return 0;
    }
  }

  QoreNode* n = test_param(params, NT_STRING, 0);
  if (!n) {
    xsink->raiseException("tpalloc", "The first parameter, type, needs to be a string.");
    return 0;
  }
  char* type = n->val.String->getBuffer();

  n = test_param(params, NT_STRING, 1);
  if (!n) {
    xsink->raiseException("tpalloc", "The second parameter, the subtype, needs to be a string, possibly empty.");
    return 0;
  }  
  char* subtype = n->val.String->getBuffer();
 
  n = test_param(params, NT_INT, 2);
  if (!n) {
    xsink->raiseException("tpalloc", "The third parameter, size, needs to be an integer.");
    return 0;
  }
  long size = (long)n->val.intval;

  n = test_param(params, NT_OBJECT, 3);
  if (!n) {
    xsink->raiseException("tpalloc", "The fourth parameter, typed buffer, needs to be an object.");
    return 0;
  }  
  QoreTuxedoTypedBuffer* buff = node2typed_buffer(n, "tpalloc", xsink);
  
  char* res = tpalloc(type, subtype, size);
  if (!res) {
    return new QoreNode((int64)tperrno);
  }
  
  if (buff->buffer) {
    tpfree(buff->buffer);
  }  
  buff->buffer = res;
  buff->size = size;
  return new QoreNode(OK);
}

//------------------------------------------------------------------------------
// http://edocs.bea.com/tux91/rf3c/rf3c55.htm#1022852
// Variant w/o parameters (tpinfo == 0)
static QoreNode* f_tpinit(QoreNode* params, ExceptionSink* xsink)
{
  if (get_param(params, 0)) {
    xsink->raiseException("tpinit", "No parameters expected. Use tpinit_params().");
    return 0;
  }
  if (tpinit(0) == -1) {
    return new QoreNode((int64)tperrno);
  } else {
    return new QoreNode(OK);
  }
}

//------------------------------------------------------------------------------
// tpinit() with all parameters:
// * user name (string, max 30 chars)
// * client name (string, max 30 chars)
// * password (string, max 30 chars)
// * group name (string, max 30 chars)
// * flags (integer)
// * additional data (could be empty)
static QoreNode* f_tpinit_params(QoreNode* params, ExceptionSink* xsink)
{
  for (int i = 0; i <= 6; ++i) {
    bool ok;
    if (i == 6) ok = !get_param(params, i);
    else ok = get_param(params, i);
    if (!ok) {
      xsink->raiseException("tpinit", "Six parameters expected: user name, client name, password, group name, flags, additional data.");
      return 0;
    }
  }

  QoreNode* n = test_param(params, NT_STRING, 0);
  if (!n) {
    xsink->raiseException("tpinit", "The first parameter, user name, needs to be a string.");
    return 0;
  }
  char* user_name = n->val.String->getBuffer();
  if (!user_name) user_name = "";
  if (strlen(user_name) > MAXTIDENT) {
    xsink->raiseException("tpinit", "The first parameter, user name string, could have max %d characters. It has %d.", MAXTIDENT, strlen(user_name));
    return 0;
  }

  n = test_param(params, NT_STRING, 1);
  if (!n) {
    xsink->raiseException("tpinit", "The second parameter, client name, needs to be a string.");
    return 0;
  }
  char* client_name = n->val.String->getBuffer();
  if (!client_name) client_name = "";
  if (strlen(client_name) > MAXTIDENT) {
    xsink->raiseException("tpinit", "The second parameter, client name string, could have max %d characters. It has %d.", MAXTIDENT, strlen(client_name));
    return 0;
  }

  n = test_param(params, NT_STRING, 2);
  if (!n) {
    xsink->raiseException("tpinit", "The third parameter, password, needs to be a string.");
    return 0;
  }
  char* password = n->val.String->getBuffer();
  if (!password) password = "";
  if (strlen(password) > MAXTIDENT) {
    xsink->raiseException("tpinit", "The third parameter, password string, could have max %d characters. It has %d.", MAXTIDENT, strlen(password));
    return 0;
  }

  n = test_param(params, NT_STRING, 3);
  if (!n) {
    xsink->raiseException("tpinit", "The fourth parameter, group name, needs to be a string.");
    return 0;
  }
  char* group_name = n->val.String->getBuffer();
  if (!group_name) group_name = "";
  if (strlen(group_name) > MAXTIDENT) {
    xsink->raiseException("tpinit", "The fourth parameter, group name string, could have max %d characters. It has %d.", MAXTIDENT, strlen(group_name));
    return 0;
  }

  n = test_param(params, NT_INT, 4);
  if (!n) {
    xsink->raiseException("tpinit", "The fifth parameter, flags, needs to be an integer.");
    return 0;
  }
  long flags = (long)n->val.intval;

  n = test_param(params, NT_BINARY, 5);
  if (!n) {
    xsink->raiseException("tpinit", "The sixth parameter, additional data, needs to be a binary (could be empty).");
    return 0;
  }
  void* data = n->val.bin->getPtr();
  int data_size = n->val.bin->size();

  long result_size = sizeof(TPINIT) + data_size;
  TPINIT* buffer = (TPINIT*)tpalloc("TPINIT", 0, result_size);
  if (!buffer) {
    return new QoreNode((int64)tperrno);
  }
  strcpy(buffer->usrname, user_name);
  strcpy(buffer->cltname, client_name);
  strcpy(buffer->grpname, group_name);
  strcpy(buffer->passwd, password);
  buffer->flags = flags;
  buffer->datalen = data_size;
  if (data_size) {
    memcpy(&buffer->data, data, data_size);
  }

  int64 retval;
  if (tpinit(buffer) == -1) {
    retval = tperrno;
  } else {
    retval = OK;
  }
  tpfree((char*)buffer);
  return new QoreNode(retval);
}

//------------------------------------------------------------------------------
// http://edocs.bea.com/tuxedo/tux91/rf3c/rf3c86.htm#1219084
static QoreNode* f_tpterm(QoreNode* params, ExceptionSink* xsink)
{
  if (get_param(params, 0)) {
    xsink->raiseException("tpterm", "No parameter expected.");
    return 0;
  }
  if (tpterm() == -1) {
    return new QoreNode((int64)tperrno);
  } else {
    return new QoreNode(OK);
  }
}

//-----------------------------------------------------------------------------
// http://edocs.bea.com/tuxedo/tux91/rf3c/rf3c87.htm#1045809
// Parameters:
// * TuxedoTypedBuffer - in
// * type string - out
// * subtype string - out
// * size - out
// Returns either tperrno integer or list:
//   size (integer), type (string), subtype(string)
static QoreNode* f_tptypes(QoreNode* params, ExceptionSink* xsink)
{
  for (int i = 0; i <= 4; ++i) {
    bool ok;
    if (i == 4) ok = !get_param(params, i);
    else ok = get_param(params, i);
    if (!ok) {
      xsink->raiseException("tptypes", "Four parameters required: typed-buffer-object, out-type_string, out-subtype-string, out-size_integer).");
      return 0;
    }
  }

  QoreNode* n = test_param(params, NT_OBJECT, 0);
  if (!n) {
    xsink->raiseException("tptypes", "The first parameter, typed buffer to be checked, needs to be Tuxedo::TuxedoTypedBuffer object.");
    return 0;
  }
  QoreTuxedoTypedBuffer* buff = node2typed_buffer(n, "tptypes", xsink);
  if (xsink->isException()) {
    return 0;
  }
  if (!buff->buffer || !buff->size) {
    // empty buffer is clearly invalid for getting a type
    return new QoreNode((int64)TPEINVAL);
  }

  n = test_param(params, NT_STRING, 1);
  if (!n) {
    xsink->raiseException("tptypes", "The second parameter, type,  needs to be a string (used as out).");
    return 0;
  }
  QoreString* out_type = n->val.String;

  n = test_param(params, NT_STRING, 2);
  if (!n) {
    xsink->raiseException("tptypes", "The third parameter, subtype,  needs to be a string (used as out).");
    return 0;
  }
  QoreString* out_subtype = n->val.String;

  n = test_param(params, NT_INT, 3);
  if (!n) {
    xsink->raiseException("tptypes", "The fourth parameter, typed buffer size, needs to be an integer (used as out).");
    return 0;
  }
  int64* out_size = &n->val.intval;
 
  const int MaxTypeSize = 8;  // see docs
  const int MaxSubtypeSize = 16; 
  char type[MaxTypeSize + 100];
  char subtype[MaxSubtypeSize + 100];

  long size =  tptypes(buff->buffer, type, subtype);
  if (size == -1) {
    return new QoreNode((int64)tperrno);
  }
  type[MaxTypeSize] = 0;
  subtype[MaxSubtypeSize] = 0;

  *out_size = size;
  out_type->set(type);
  out_subtype->set(subtype);

  return new QoreNode(OK);
}

//------------------------------------------------------------------------------
// http://edocs.bea.com/tuxedo/tux91/rf3c/rf3c26.htm#1021731
// Parameters:
// * service name (string)
// * input data (Tuxedo::TuxedoTypedBuffer)
// * output data (Tuxedo::TuxedoTypedBuffer)
// * flags (integer)
static QoreNode* f_tpcall(QoreNode* params, ExceptionSink* xsink)
{
  for (int i = 0; i <= 4; ++i) {
    bool ok;
    if (i == 4) ok = !get_param(params, i);
    else ok = get_param(params, i);
    if (!ok) {
      xsink->raiseException("tpcall", "Four parameters expected: service name string, input data typed buffer, output data typed buffer, integer flags.");
      return 0;
    }
  }

  QoreNode* n = test_param(params, NT_STRING, 0);
  if (!n) {
    xsink->raiseException("tpcall", "The first paramer (service name) needs to be an string.");
    return 0;
  }
  char* service_name = n->val.String->getBuffer();

  n = test_param(params, NT_OBJECT, 1);
  if (!n) {
    xsink->raiseException("tpcall", "The second parameter (input data) needs to be Tuxedo::TuxedoTypedBuffer instance.");    
    return 0;
  }
  QoreTuxedoTypedBuffer* in_buff = node2typed_buffer(n, "tpcall", xsink);
  if (xsink->isException()) {
    return 0;
  }
 
  n = test_param(params, NT_OBJECT, 2);
  if (!n) {
    xsink->raiseException("tpcall", "The third parameter (output data) needs to be Tuxedo::TuxedoTypedBuffer instance passed by reference.");
    return 0;
  }
  QoreTuxedoTypedBuffer* out_buff = node2typed_buffer(n, "tpcall", xsink);
  if (xsink->isException()) {
    return 0;
  }
  
  n = test_param(params, NT_INT, 3);
  if (!n) {
    xsink->raiseException("tpcall", "The fourth parameter, flags, needs to be an integer.");
    return 0;
  }
  long flags = (long)n->val.intval;


  if (tpcall(service_name, in_buff->buffer, in_buff->size, &out_buff->buffer, &out_buff->size, flags) == -1) {
    return new QoreNode((int64)tperrno);
  } else {
    return new QoreNode(OK);
  }
}

//------------------------------------------------------------------------------
// http://edocs.bea.com/tuxedo/tux91/rf3c/rf3c20.htm#1037129
// Parameters:
// * service name (string)
// * input buffer (Tuxedo::TuxedoTypedBuffer object)
// * flags (integer)
// * output handle (integer)
static QoreNode* f_tpacall(QoreNode* params, ExceptionSink* xsink)
{
  for (int i = 0; i <= 4; ++i) {
    bool ok;
    if (i == 4) ok = !get_param(params, i);
    else ok = get_param(params, i);
    if (!ok) {
      xsink->raiseException("tpacall", "Four parameters expected: service name string, input data TuxedoTypedBuffer, flags integer, handle integer passed by reference.");
      return 0;
    }
  }

  QoreNode* n = test_param(params, NT_STRING, 0);
  if (!n) {
    xsink->raiseException("tpacall", "The first parameter, service name, needs to be a string.");
    return 0;
  }
  char* service_name = n->val.String->getBuffer();

  n = test_param(params, NT_OBJECT, 1);
  if (!n) {
    xsink->raiseException("tpacall", "The second parameter, input data, needs to be instance of Tuxedo::TuxedoTypedBuffer.");
    return 0;
  }
  QoreTuxedoTypedBuffer* buff = node2typed_buffer(n, "tpacall", xsink);
  if (xsink->isException()) {
    return 0;
  }
  
  n = test_param(params, NT_INT, 2);
  if (!n) {
    xsink->raiseException("tpacall", "The third parameter, flags, needs to be an integer.");
    return 0;
  }
  long flags = n->val.intval;

  n = test_param(params, NT_INT, 3);
  if (!n) {
    xsink->raiseException("tpacall", "The fourth parameter, out handle, needs to be be an integer passed by reference.");
    return 0;
  }
  int64* out_handle = &n->val.intval;

  int res = tpacall(service_name, buff->buffer, buff->size, flags);
  if (res == -1) {
    return new QoreNode((int64)tperrno);
  }
  
  *out_handle = res;
  return new QoreNode(OK);
}

//------------------------------------------------------------------------------
// http://edocs.bea.com/tuxedo/tux91/Rf3c/rf3c27.htm#1039772
static QoreNode* f_tpcancel(QoreNode* params, ExceptionSink* xsink)
{
  for (int i = 0; i <= 1; ++i) {
    bool ok;
    if (i == 1) ok = !get_param(params, i); 
    else ok = get_param(params, i);
    if (!ok) {
      xsink->raiseException("tpcancel", "One parameter, integer handle, is expected.");
      return 0;
    }
  }

  QoreNode* n = test_param(params, NT_INT, 0);
  if (!n) {
    xsink->raiseException("tpcancel", "The first parameter, handle, needs to be an integer.");
    return 0;
  }
  int handle = (int)n->val.intval;

  if (tpcancel(handle) == -1) {
    return new QoreNode((int64)tperrno);
  } else {
    return new QoreNode(OK);
  }
}

//------------------------------------------------------------------------------
// http://edocs.bea.com/tuxedo/tux91/rf3c/rf3c52.htm#1021885
// Parameters:
// * integer handle, passed by reference
// * out-data TuxedoTypedBuffer passed by reference
// * flags integer
static QoreNode* f_tpgetrply(QoreNode* params, ExceptionSink* xsink)
{
  for (int i = 0; i <= 3; ++i) {
    bool ok;
    if (i == 3) ok = !get_param(params, i);
    else ok = get_param(params, i);
    if (!ok) {
      xsink->raiseException("tpgetrply", "Three parameters are expected: handle integer by reference, out data TuxedoTypedBuffer by reference, integer flags.");
      return 0;
    }
  }

  QoreNode* n = test_param(params, NT_INT, 0);
  if (!n) {
    xsink->raiseException("tpgetrply", "The first parameter, handle, needs to be an inteher passed by reference.");
    return 0;
  }
  int handle = (int)n->val.intval;
  int64* phandle = &n->val.intval;

  n = test_param(params, NT_OBJECT, 1);
  if (!n) {
    xsink->raiseException("tpgetrply", "The second parameter, output data, needs to be Tuxedo::TuxedoTypedClass passed by reference.");
    return 0;
  }
  QoreTuxedoTypedBuffer* buff = node2typed_buffer(n, "tpgetrply", xsink);
  if (xsink->isException()) {
    return 0;
  }

  n = test_param(params, NT_INT, 2);
  if (!n) {
    xsink->raiseException("tpgetrply", "The third parameter, flags, needs to be an integer.");
    return 0;
  }
  long flags = (long)n->val.intval;

  int res = tpgetrply(&handle, &buff->buffer, &buff->size, flags);
  *phandle = handle;
  
  if (res == -1) {
    return new QoreNode((int64)tperrno);
  } else {
    return new QoreNode(OK);
  }
}

//------------------------------------------------------------------------------
void tuxedo_low_level_init()
{
  builtinFunctions.add("tpchkauth", f_tpchkauth, QDOM_NETWORK);
  builtinFunctions.add("tpalloc", f_tpalloc, QDOM_NETWORK);
  builtinFunctions.add("tpinit", f_tpinit, QDOM_NETWORK);
  builtinFunctions.add("tpinitParams", f_tpinit_params, QDOM_NETWORK);
  builtinFunctions.add("tpterm", f_tpterm, QDOM_NETWORK);
  builtinFunctions.add("tptypes", f_tptypes, QDOM_NETWORK);
  builtinFunctions.add("tpcall", f_tpcall, QDOM_NETWORK);
  builtinFunctions.add("tpacall", f_tpacall, QDOM_NETWORK);
  builtinFunctions.add("tpcancel", f_tpcancel, QDOM_NETWORK);
  builtinFunctions.add("tpgetrply", f_tpgetrply, QDOM_NETWORK);
}

//-----------------------------------------------------------------------------
void tuxedo_low_level_ns_init(Namespace* ns)
{
  ns->addConstant("TPAPPAUTH", new QoreNode((int64)TPAPPAUTH));
  ns->addConstant("TPEBADDESC", new QoreNode((int64)TPEBADDESC));
  ns->addConstant("TPEBLOCK", new QoreNode((int64)TPEBLOCK));
  ns->addConstant("TPEINVAL", new QoreNode((int64)TPEINVAL));
  ns->addConstant("TPEITYPE", new QoreNode((int64)TPEITYPE));
  ns->addConstant("TPENOENT", new QoreNode((int64)TPENOENT));
  ns->addConstant("TPEOS", new QoreNode((int64)TPEOS));
  ns->addConstant("TPEOTYPE", new QoreNode((int64)TPEOTYPE));
  ns->addConstant("TPEPERM", new QoreNode((int64)TPEPERM));
  ns->addConstant("TPEPROTO", new QoreNode((int64)TPEPROTO));
  ns->addConstant("TPESVCFAIL", new QoreNode((int64)TPESVCFAIL));
  ns->addConstant("TPESVCERR", new QoreNode((int64)TPESVCERR));
  ns->addConstant("TPESYSTEM", new QoreNode((int64)TPESYSTEM));
  ns->addConstant("TPETIME", new QoreNode((int64)TPETIME));
  ns->addConstant("TPETRAN", new QoreNode((int64)TPETRAN));
  ns->addConstant("TPGETANY", new QoreNode((int64)TPGETANY));
  ns->addConstant("TPGOTSIG", new QoreNode((int64)TPGOTSIG));
  ns->addConstant("TPMULTICONTEXTS", new QoreNode((int64)TPMULTICONTEXTS));
  ns->addConstant("TPNOAUTH", new QoreNode((int64)TPNOAUTH));
  ns->addConstant("TPNOBLOCK", new QoreNode((int64)TPNOCHANGE));
  ns->addConstant("TPNOCHANGE", new QoreNode((int64)TPNOCHANGE));
  ns->addConstant("TPNOTIME", new QoreNode((int64)TPNOTIME));
  ns->addConstant("TPNOTRAN", new QoreNode((int64)TPNOTRAN));
  ns->addConstant("TPSA_FASTPATH", new QoreNode((int64)TPSA_FASTPATH));
  ns->addConstant("TPSA_PROTECTED", new QoreNode((int64)TPSA_PROTECTED));
  ns->addConstant("TPSIGRSTRT", new QoreNode((int64)TPSIGRSTRT));
  ns->addConstant("TPSYSAUTH", new QoreNode((int64)TPSYSAUTH));
  ns->addConstant("TPU_DIP", new QoreNode((int64)TPU_DIP));
  ns->addConstant("TPU_IGN", new QoreNode((int64)TPU_IGN));
  ns->addConstant("TPU_SIG", new QoreNode((int64)TPU_SIG));
  ns->addConstant("TPU_THREAD", new QoreNode((int64)TPU_THREAD));
}

// EOF

