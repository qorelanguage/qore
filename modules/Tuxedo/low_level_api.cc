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
#include "QC_TuxedoQueueControlParams.h"
#include "QoreTuxedoQueueControlParams.h"

#include <atmi.h>
#include <userlog.h>

const int64 OK = 0;

//------------------------------------------------------------------------------
// n - known as an object
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
// n - known as an object
static QoreTuxedoQueueControlParams* node2queue_control_params(QoreNode* n, char* func_name, ExceptionSink* xsink)
{
  if (!n->val.object) {
    xsink->raiseException(func_name, "Expected instance of Tuxedo::TuxedoQueueControlParams class.");
    return 0;
  }
  if (n->val.object->getClass()->getID() != CID_TUXEDOQUEUECONTROLPARAMS) {
    xsink->raiseException(func_name, "Type mismatch: expected instance of Tuxedo::TuxedoQueueControlParams class.");
    return 0;
  }
  // this should be safe now
  QoreTuxedoQueueControlParams* ctl = (QoreTuxedoQueueControlParams*)(n->val.object);
  return ctl;
}

//------------------------------------------------------------------------------
// http://edocs.bea.com/tuxedo/tux91/rf3c/rf3c28.htm#1040017
static QoreNode* f_tpchkauth(QoreNode* params, ExceptionSink* xsink)
{
  for (int i = 0; i <= 1; ++i) {
    bool ok;
    if (i == 1) ok = !get_param(params, i);
    else ok = get_param(params, i);
    if (!ok) {
      xsink->raiseException("tpchkauth", "One parameter expected: required authentification integer passed by reference.");
      return 0;
    }
  }

  QoreNode* n = test_param(params, NT_INT, 0);
  if (!n) {
    xsink->raiseException("tpchauth", "The first parameter, required authentification, needs to be an integer passed by reference.");
  }
  int64* pauth = &n->val.intval;

  int res = tpchkauth();
  if (res == -1) {
    return new QoreNode((int64)tperrno);
  } 
  
  *pauth = res;
  return new QoreNode(OK);
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
  if (subtype && !subtype[0]) subtype = 0;
 
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
// * additional data (could be empty), optional
static QoreNode* f_tpinit_params(QoreNode* params, ExceptionSink* xsink)
{
  for (int i = 0; i < 6; ++i) {
    if (!get_param(params, i)) {
      xsink->raiseException("tpinit", "File or six parameters expected: user name, client name, password, group name, flags, additional data (binary, optional).");
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

  void* data = 0;
  int data_size = 0;

  if (get_param(params, 5)) {
    n = test_param(params, NT_BINARY, 5);
    if (!n) {
      xsink->raiseException("tpinit", "The sixth parameter, additional data, needs to be a binary (could be empty).");
      return 0;
    }
    data = n->val.bin->getPtr();
    data_size = n->val.bin->size();
  }

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
      xsink->raiseException("tptypes", "Four parameters required: typed-buffer-object passed by reference, out-type_string, out-subtype-string, out-size_integer).");
      return 0;
    }
  }

  QoreNode* n = test_param(params, NT_OBJECT, 0);
  if (!n) {
    xsink->raiseException("tptypes", "The first parameter, typed buffer to be checked, needs to be Tuxedo::TuxedoTypedBuffer object passed by reference.");
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
    xsink->raiseException("tpcall", "The second parameter (input data) needs to be Tuxedo::TuxedoTypedBuffer instance, possibly passed by reference.");    
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
// http://edocs.bea.com/tuxedo/tux91/rf3c/rf3c37.htm#1042150
static QoreNode* f_tpdiscon(QoreNode* params, ExceptionSink* xsink)
{
  for (int i = 0; i <= 1; ++i) {
    bool ok;
    if (i == 1) ok = !get_param(params, i);
    else ok = get_param(params, i);
    if (!ok) {
      xsink->raiseException("tpdiscon", "One parameter is expected, the integer handle.");
      return 0;
    }
  }

  QoreNode* n = test_param(params, NT_INT, 0);
  if (!n) {
    xsink->raiseException("tpdiscon", "The first parameter, handle, needs to be an integer.");
    return 0;
  }
  int handle = (int)n->val.intval;

  if (tpdiscon(handle) == -1) {
    return new QoreNode((int64)tperrno);
  } else {
    return new QoreNode(OK);
  }
}

//------------------------------------------------------------------------------
// http://edocs.bea.com/tuxedo/tux91/rf3c/rf3c19.htm#1036832
static QoreNode* f_tpabort(QoreNode* params, ExceptionSink* xsink)
{
  if (get_param(params, 0)) {
    xsink->raiseException("tpabort", "No parameters expected.");
    return 0;
  }

  if (tpabort(0) == -1) {
    return new QoreNode((int64)tperrno);
  } else {
    return new QoreNode(OK);
  }
}

//-----------------------------------------------------------------------------
// http://edocs.bea.com/tuxedo/tux91/rf3c/rf3c24.htm#1039168
static QoreNode* f_tpbegin(QoreNode* params, ExceptionSink* xsink)
{
  for (int i = 0; i <= 1; ++i) {
    bool ok;
    if (i == 1) ok = !get_param(params, i);
    else ok = get_param(params, i);
    if (!ok) {
      xsink->raiseException("tpbegin", "Two parameters are expected: integer timeout.");
      return 0;
    }
  }
  
  QoreNode* n = test_param(params, NT_INT, 0);
  if (!n) {
    xsink->raiseException("tpbegin", "The first parameter, timeout, needs to be an integer.");
    return 0;
  }
  unsigned long timeout = (unsigned long)n->val.intval;

  if (tpbegin(timeout, 0) == -1) {
    return new QoreNode((int64)tperrno);
  } else {
    return new QoreNode(OK);
  }  
}

//-----------------------------------------------------------------------------
// http://edocs/bea.com/tuxedo/tux91/rf3c/rf3c61.htm#1043391
static QoreNode* f_tpopen(QoreNode* params, ExceptionSink* xsink)
{
  if (get_param(params, 0)) {
    xsink->raiseException("tpopen", "The function has no parameters.");
    return 0;
  }

  if (tpopen() == -1) {
    return new QoreNode((int64)tperrno);
  } else {
    return new QoreNode(OK);
  }
}

//-----------------------------------------------------------------------------
// http://edocs/bea.com/tuxedo/tux91/rf3c/rf3c30.htm#1040568
static QoreNode* f_tpclose(QoreNode* params, ExceptionSink* xsink)
{
  if (get_param(params, 0)) {
    xsink->raiseException("tpclose", "The function has no parameters.");
    return 0;
  }

  if (tpclose() == -1) {
    return new QoreNode((int64)tperrno);
  } else {
    return new QoreNode(OK);
  }
}

//-----------------------------------------------------------------------------
// http://edocs.bea.com/tuxedo/tux91/rf3c/rf3c31.htm#1040811
static QoreNode* f_tpcommit(QoreNode* params, ExceptionSink* xsink)
{
  if (get_param(params, 0)) {
    xsink->raiseException("tpcommit", "No parameters expected.");
    return 0;
  }

  if (tpcommit(0) == -1) {
    return new QoreNode((int64)tperrno);
  } else {
    return new QoreNode(OK);
  }
}

//-----------------------------------------------------------------------------
// http://edocs.bea.com/tuxedo/tux91/rf3c/rf3c32.htm#1041087
// Parameters:
// * string service name
// * input data TuxedoTypedBuffer (could be empty)
// * integer flags
// * out integer handle
static QoreNode* f_tpconnect(QoreNode* params, ExceptionSink* xsink)
{
  for (int i = 0; i <= 4; ++i) {
    bool ok;
    if (i == 1) ok = !get_param(params, i);
    else ok = get_param(params, i);
    if (!ok) {
      xsink->raiseException("tpconnect", "Four parameters are expected: string service name, input data Tuxedo::TuxedoTypedBuffer, integer flags, integer handle passed by reference.");
      return 0;
    }
  }

  QoreNode* n = test_param(params, NT_STRING, 0);
  if (!n) {
    xsink->raiseException("tpconnect", "The first parameter, service name, needs to be a string.");
    return 0;
  }
  char* service_name = n->val.String->getBuffer();

  n = test_param(params, NT_OBJECT, 1);
  if (!n) {
    xsink->raiseException("tpconnect", "The second parameter, input data, neesd to be instance of TuxedoTypedBuffer (could be empty).");
    return 0;
  }
  QoreTuxedoTypedBuffer* buff = node2typed_buffer(n, "tpgetrply", xsink);

  n = test_param(params, NT_INT, 2);
  if (!n) {
    xsink->raiseException("tpconnect", "The third parameter, flags, neesd to be an integer.");
    return 0;
  }
  long flags = (long)n->val.intval;

  n = test_param(params, NT_INT, 3);
  if (!n) {
    xsink->raiseException("tpconnect", "The fourth parameter, output handle, needs to be an integer passed by reference.");
    return 0;
  }
  int64* phandle = &n->val.intval;

  int res = tpconnect(service_name, buff->buffer, buff->size, flags);
  if (res == -1) {
    return new QoreNode((int64)tperrno);
  }
  *phandle = res;
  return new QoreNode(OK);
}

//-----------------------------------------------------------------------------
// http://edocs.bea.com/tuxedo/tux91/rf3c/rf3c70.htm#1023105
static QoreNode* f_tpsend(QoreNode* params, ExceptionSink* xsink)
{
  for (int i = 0; i <= 4; ++i) {
    bool ok;
    if (i == 1) ok = !get_param(params, i);
    else ok = get_param(params, i);
    if (!ok) {
      xsink->raiseException("tpsend", "Four parameters are expected: integer handle, input data Tuxedo::TuxedoTypedBuffer, integer flags, out integer event passed by reference.");
      return 0;
    }
  }

  QoreNode* n = test_param(params, NT_INT, 0);
  if (!n) {
    xsink->raiseException("tpsend", "The first parameter, handle, needs to be an integer.");
    return 0;
  }
  int handle = (int)n->val.intval;

  n = test_param(params, NT_OBJECT, 1);
  if (!n) {
    xsink->raiseException("tpsend", "The second paramener, data to be sent, needs to be TuxedoTypedBuffer instance.");
    return 0;
  }
  QoreTuxedoTypedBuffer* buff = node2typed_buffer(n, "tpsend", xsink);

  n = test_param(params, NT_INT, 2);
  if (!n) {
    xsink->raiseException("tpsend", "The third parameter, flags, needs to be an integer.");
    return 0;
  }
  long flags = (long)n->val.intval;

  n = test_param(params, NT_INT, 3);
  if (!n) {
    xsink->raiseException("tpsend", "The fourth parameter, event, needs to be an integer passed by reference.");
    return 0;
  }
  int64* pevent = &n->val.intval;
  
  long event = 0;
  int res = tpsend(handle, buff->buffer, buff->size, flags, &event);
  *pevent = event;

  if (res == -1) {
    return new QoreNode((int64)tperrno);
  } else {
    return new QoreNode(OK);
  }
}

//-----------------------------------------------------------------------------
// http://edocs.bea.com/tuxedo/tux91/rf3c/rf3c64.htm#1022967
static QoreNode* f_tprecv(QoreNode* params, ExceptionSink* xsink)
{
  for (int i = 0; i <= 4; ++i) {
    bool ok;
    if (i == 1) ok = !get_param(params, i);
    else ok = get_param(params, i);
    if (!ok) {
      xsink->raiseException("tpsend", "Four parameters are expected: integer handle, out data Tuxedo::TuxedoTypedBuffer passed by reference, integer flags, out integer event passed by reference.");
      return 0;
    }
  }

  QoreNode* n = test_param(params, NT_INT, 0);
  if (!n) {
    xsink->raiseException("tprecv", "The first parameter, handle, needs to be an integer.");
    return 0;
  }
  int handle = (int)n->val.intval;
  
  n = test_param(params, NT_OBJECT, 1);
  if (!n) {
    xsink->raiseException("tprecv", "The second parameter, out data, needs to be TuxedoTypedBuffer instance passed by reference.");
    return 0;
  }
  QoreTuxedoTypedBuffer* buff = node2typed_buffer(n, "tprecv", xsink);

  n = test_param(params, NT_INT, 2);
  if (!n) {
    xsink->raiseException("tprecv", "The third parameter, flags, needs to be an integer.");
    return 0;
  }
  long flags = (long)n->val.intval;

  n = test_param(params, NT_INT, 3);
  if (!n) {
    xsink->raiseException("tprecv", "The fourth parameter, out event, needs to be an integer passed by reference.");
    return 0;
  }
  int64* pevent = &n->val.intval;

  long event = 0;
  int res = tprecv(handle, &buff->buffer, &buff->size, flags, &event);
  *pevent = event;

  if (res == -1) {
    return new QoreNode((int64)tperrno);
  } else {
    return new QoreNode(OK);
  }
}

//-----------------------------------------------------------------------------
// http://edocs.bea.com/tuxedo/tux91/rf3c/rf3c38.htm#1042179
// Parameters:
// * queue space name string
// * queue name string
// * queue control parameters object TuxedoQueueControlParams passed as reference
// * data to be queued, TuxedoTypedBuffer instance
// * flags integer
static QoreNode* f_tpenqueue(QoreNode* params, ExceptionSink* xsink)
{
  for (int i = 0; i <= 5; ++i) {
    bool ok;
    if (i == 1) ok = !get_param(params, i);
    else ok = get_param(params, i);
    if (!ok) {
      xsink->raiseException("tpenqueue", "File parameters are expected: space name string, queue name string, TuxedoQueueControlParams object passed by reference, data to be queued as TuxedoTypedBuffer, integer flags.");
      return 0;
    }
  }

  QoreNode* n = test_param(params, NT_STRING, 0);
  if (!n) {
    xsink->raiseException("tpenqueue", "The first parameter, queue space name, needs to be a string.");
    return 0;
  }
  char* queue_space_name = n->val.String->getBuffer();

  n = test_param(params, NT_STRING, 1);
  if (!n) {
    xsink->raiseException("tpenqueue", "The second parameter, queue name, needs to be a string.");
    return 0;
  }
  char* queue_name = n->val.String->getBuffer();

  n = test_param(params, NT_OBJECT, 2);
  if (!n) {
    xsink->raiseException("tpenqueue", "The third parameter, queue control parameters, needs to instance of TuxedoQueueControlParams passed by reference.");
    return 0;
  }
  QoreTuxedoQueueControlParams* ctl = node2queue_control_params(n, "tpenqueue", xsink);
  if (xsink->isException()) {
    return 0;
  }

  n = test_param(params, NT_OBJECT, 3);
  if (!n) {
    xsink->raiseException("tpenqueue", "The fourth parameter, data to be send, needs to be instance of TuxedoTypedBuffer.");
    return 0;
  }
  QoreTuxedoTypedBuffer* buff = node2typed_buffer(n, "tpenqueue", xsink);
  if (xsink->isException()) {
    return 0;
  }

  n = test_param(params, NT_INT, 4);
  if (!n) {
    xsink->raiseException("tpenqueue", "The fifth parameter, flags, neesd to be an integer.");
    return 0;
  }
  long flags = (long)n->val.intval;

  if (tpenqueue(queue_space_name, queue_name, &ctl->ctl, buff->buffer, buff->size, flags) == -1) {
    return new QoreNode((int64)tperrno);
  } else {
    return new QoreNode(OK);
  }
}

//-----------------------------------------------------------------------------
// http://edocs.bea.com/tuxedo/tux91/rf3c/rf3c36.htm#1041783
// Parameters:
// * queue space name string
// * queue name string
// * queue control parameters object TuxedoQueueControlParams passed as reference
// * read data, TuxedoTypedBuffer instance passed by reference
// * flags integer
static QoreNode* f_tpdeque(QoreNode* params, ExceptionSink* xsink)
{
  for (int i = 0; i <= 5; ++i) {
    bool ok;
    if (i == 1) ok = !get_param(params, i);
    else ok = get_param(params, i);
    if (!ok) {
      xsink->raiseException("tpenqueue", "Five parameters are expected: space name string, queue name string, TuxedoQueueControlParams object passed by reference, data to be read as TuxedoTypedBuffer passed by reference, integer flags.");
      return 0;
    }
  }

 QoreNode* n = test_param(params, NT_STRING, 0);
  if (!n) {
    xsink->raiseException("tpdequeue", "The first parameter, queue space name, needs to be a string.");
    return 0;
  }
  char* queue_space_name = n->val.String->getBuffer();

  n = test_param(params, NT_STRING, 1);
  if (!n) {
    xsink->raiseException("tpdequeue", "The second parameter, queue name, needs to be a string.");
    return 0;
  }
  char* queue_name = n->val.String->getBuffer();

  n = test_param(params, NT_OBJECT, 2);
  if (!n) {
    xsink->raiseException("tpdequeue", "The third parameter, queue control parameters, needs to instance of TuxedoQueueControlParams passed by reference.");
    return 0;
  }
  QoreTuxedoQueueControlParams* ctl = node2queue_control_params(n, "tpdequeue", xsink);
  if (xsink->isException()) {
    return 0;
  }

  n = test_param(params, NT_OBJECT, 3);
  if (!n) {
    xsink->raiseException("tpdequeue", "The fourth parameter, data to be send, needs to be instance of TuxedoTypedBuffer passed by reference.");
    return 0;
  }
  QoreTuxedoTypedBuffer* buff = node2typed_buffer(n, "tpdequeue", xsink);
  if (xsink->isException()) {
    return 0;
  }

  n = test_param(params, NT_INT, 4);
  if (!n) {
    xsink->raiseException("tpdequeue", "The fifth parameter, flags, neesd to be an integer.");
    return 0;
  }
  long flags = (long)n->val.intval;

  if (tpdequeue(queue_space_name, queue_name, &ctl->ctl, &buff->buffer, &buff->size, flags) == -1) {
    return new QoreNode((int64)tperrno);
  } else {
    return new QoreNode(OK);
  }
}

//-----------------------------------------------------------------------------
// http://edocs.bea.com/tuxedo/tux91/rf3c/rf3c40.htm#1021798
// No parameters, unlike C function which always requires 0.
static QoreNode* f_tperrordetail(QoreNode* params, ExceptionSink* xsink)
{
  if (get_param(params, 0)) {
    xsink->raiseException("tperrordetail", "No parameter expected.");
    return 0;
  }

  int res = tperrordetail(0);
  return new QoreNode((int64)res);
}

//-----------------------------------------------------------------------------
// http://edocs.bea.com/tuxedo/tux91/rf3c/rf3c110.htm#1049646
static QoreNode* f_userlog(QoreNode* params, ExceptionSink* xsink)
{
  for (int i = 0; i <= 1; ++i) {
    bool ok;
    if (i == 1) ok = !get_param(params, i);
    else ok = get_param(params, i);
    if (!ok) {
      xsink->raiseException("TuxedoUserlog", "One parameter is expected: the string to be written into Tuxedo log.");
      return 0;
    }
  }
  
  QoreNode* n = test_param(params, NT_STRING, 0);
  if (!n) {
    xsink->raiseException("TuxedoUserlog", "The first parameter, logged text,  needs to be a string.");
    return 0;
  }
  char* text = n->val.String->getBuffer();
  if (!text) text = "";

  if (userlog(text) < 0) {
    return new QoreNode((int64)TPEINVAL);
  } else {
    return new QoreNode(OK);
  }
}

//-----------------------------------------------------------------------------
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
  builtinFunctions.add("tpdiscon", f_tpdiscon, QDOM_NETWORK);
  builtinFunctions.add("tpabort", f_tpabort, QDOM_NETWORK);
  builtinFunctions.add("tpbegin", f_tpbegin, QDOM_NETWORK);
  builtinFunctions.add("tpopen", f_tpopen, QDOM_NETWORK);
  builtinFunctions.add("tpclose", f_tpclose, QDOM_NETWORK);
  builtinFunctions.add("tpcommit", f_tpcommit, QDOM_NETWORK);
  builtinFunctions.add("tpconnect", f_tpconnect, QDOM_NETWORK);
  builtinFunctions.add("tpsend", f_tpsend, QDOM_NETWORK);
  builtinFunctions.add("tprecv", f_tprecv, QDOM_NETWORK);
  builtinFunctions.add("tpenqueue", f_tpenqueue, QDOM_NETWORK);
  builtinFunctions.add("tpdequeue", f_tpdeque, QDOM_NETWORK);
  builtinFunctions.add("tperrordetail", f_tperrordetail, QDOM_NETWORK);
  builtinFunctions.add("TuxedoUserlog", f_userlog, QDOM_NETWORK);

  // tpsuspend, tpresume, tpscmt, tpgetlev. tpsprio, tpgprio, tpopne, tpclose, tppost, tpstrerror
  // XML <-> FML
  // tuxputenv, tuxgetenv
  // tx_*
}

//-----------------------------------------------------------------------------
void tuxedo_low_level_ns_init(Namespace* ns)
{
  ns->addConstant("QMEABORTED", new QoreNode((int64)QMEABORTED));
  ns->addConstant("QMEBADQUEUE", new QoreNode((int64)QMEBADQUEUE));
  ns->addConstant("QMEBADRMID", new QoreNode((int64)QMEBADRMID));
  ns->addConstant("QMEBADMSGID", new QoreNode((int64)QMEBADMSGID));
  ns->addConstant("QMEINUSE", new QoreNode((int64)QMEINUSE));
  ns->addConstant("QMEINVAL", new QoreNode((int64)QMEINVAL));
  ns->addConstant("QMENOMSG", new QoreNode((int64)QMENOMSG));
  ns->addConstant("QMENOSPACE", new QoreNode((int64)QMENOSPACE));
  ns->addConstant("QMENOTOPEN", new QoreNode((int64)QMENOTOPEN));
  ns->addConstant("QMEOS", new QoreNode((int64)QMEOS));
  ns->addConstant("QMEPROTO", new QoreNode((int64)QMEPROTO));
  ns->addConstant("QMERELEASE", new QoreNode((int64)QMERELEASE));
  ns->addConstant("QMESHARE", new QoreNode((int64)QMESHARE));
  ns->addConstant("QMESYSTEM", new QoreNode((int64)QMESYSTEM));
  ns->addConstant("QMETRAN", new QoreNode((int64)QMETRAN));

  ns->addConstant("TP_CMT_COMPLETE", new QoreNode((int64)TP_CMT_COMPLETE));
  ns->addConstant("TP_CMT_LOGGED", new QoreNode((int64)TP_CMT_LOGGED));
  ns->addConstant("TPEV_DISCONIMM", new QoreNode((int64)TPEV_DISCONIMM));
  ns->addConstant("TPEV_SVCERR", new QoreNode((int64)TPEV_SVCERR));
  ns->addConstant("TPEV_SVCFAIL", new QoreNode((int64)TPEV_SVCFAIL));
  ns->addConstant("TPEV_SVCSUCC", new QoreNode((int64)TPEV_SVCSUCC));
  ns->addConstant("TPAPPAUTH", new QoreNode((int64)TPAPPAUTH));
  ns->addConstant("TPEABORT", new QoreNode((int64)TPEABORT));
  ns->addConstant("TPEBADDESC", new QoreNode((int64)TPEBADDESC));
  ns->addConstant("TPEBLOCK", new QoreNode((int64)TPEBLOCK));
  ns->addConstant("TPED_CLIENTDISCONNECTED", new QoreNode((int64)TPED_CLIENTDISCONNECTED));
  ns->addConstant("TPED_DECRYPTION_FAILURE", new QoreNode((int64)TPED_DECRYPTION_FAILURE));
  ns->addConstant("TPED_DOMAINUNREACHABLE", new QoreNode((int64)TPED_DOMAINUNREACHABLE));
  ns->addConstant("TPED_INVALID_CERTIFICATE", new QoreNode((int64)TPED_INVALID_CERTIFICATE));
  ns->addConstant("TPED_INVALID_SIGNATURE", new QoreNode((int64)TPED_INVALID_SIGNATURE));
  ns->addConstant("TPED_INVALIDCONTEXT", new QoreNode((int64)TPED_INVALIDCONTEXT));
  ns->addConstant("TPED_INVALID_XA_TRANSACTION", new QoreNode((int64)TPED_INVALID_XA_TRANSACTION));
  ns->addConstant("TPED_NOCLIENT", new QoreNode((int64)TPED_NOCLIENT));
  ns->addConstant("TPED_NOUNSOLHANDLER", new QoreNode((int64)TPED_NOUNSOLHANDLER));
  ns->addConstant("TPED_SVCTIMEOUT", new QoreNode((int64)TPED_SVCTIMEOUT));
  ns->addConstant("TPED_TERM", new QoreNode((int64)TPED_TERM));
  ns->addConstant("TPEDIAGNOSTIC", new QoreNode((int64)TPEDIAGNOSTIC));
  ns->addConstant("TPEVENT", new QoreNode((int64)TPEEVENT));
  ns->addConstant("TPEHAZARD", new QoreNode((int64)TPEHAZARD));
  ns->addConstant("TPEHEURISTIC", new QoreNode((int64)TPEHEURISTIC));
  ns->addConstant("TPEINVAL", new QoreNode((int64)TPEINVAL));
  ns->addConstant("TPEITYPE", new QoreNode((int64)TPEITYPE));
  ns->addConstant("TPELIMIT", new QoreNode((int64)TPELIMIT));
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
  ns->addConstant("TPNOFLAGS", new QoreNode((int64)TPNOFLAGS));
  ns->addConstant("TPNOREPLY", new QoreNode((int64)TPNOREPLY));
  ns->addConstant("TPNOTIME", new QoreNode((int64)TPNOTIME));
  ns->addConstant("TPNOTRAN", new QoreNode((int64)TPNOTRAN));
  ns->addConstant("TPQBEFOREMSGID", new QoreNode((int64)TPQBEFOREMSGID));
  ns->addConstant("TPQCORRID", new QoreNode((int64)TPQCORRID));
  ns->addConstant("TPQDELIVERYQOS", new QoreNode((int64)TPQDELIVERYQOS));
  ns->addConstant("TPEX_STRING", new QoreNode((int64)TPEX_STRING));
  ns->addConstant("TPQEXPTIME_ABS", new QoreNode((int64)TPQEXPTIME_ABS));
  ns->addConstant("TPQEXPTIME_NONE", new QoreNode((int64)TPQEXPTIME_NONE));
  ns->addConstant("TPQEXPTIME_REL", new QoreNode((int64)TPQEXPTIME_REL));
  ns->addConstant("TPQFAILUREQ", new QoreNode((int64)TPQFAILUREQ));
  ns->addConstant("TPQGETBYCORRID", new QoreNode((int64)TPQGETBYCORRID));
  ns->addConstant("TPQGETBYMSGID", new QoreNode((int64)TPQGETBYMSGID));
  ns->addConstant("TPQMSGID", new QoreNode((int64)TPQMSGID));
  ns->addConstant("TPQPEEK", new QoreNode((int64)TPQPEEK));
  ns->addConstant("TPQPRIORITY", new QoreNode((int64)TPQPRIORITY));
  ns->addConstant("TPQQOSDEFAULTPERSIST", new QoreNode((int64)TPQQOSDEFAULTPERSIST));
  ns->addConstant("TPQQOSNONPERSISTENT", new QoreNode((int64)TPQQOSNONPERSISTENT));
  ns->addConstant("TPQQOSPERSISTENT", new QoreNode((int64)TPQQOSPERSISTENT));
  ns->addConstant("TPREPLYQ", new QoreNode((int64)TPQREPLYQ));
  ns->addConstant("TPQREPLYQOS", new QoreNode((int64)TPQREPLYQOS));
  ns->addConstant("TPQTIME_ABS", new QoreNode((int64)TPQTIME_ABS));
  ns->addConstant("TPQTIME_REL", new QoreNode((int64)TPQTIME_REL));
  ns->addConstant("TPQTOP", new QoreNode((int64)TPQTOP));
  ns->addConstant("TPQWAIT", new QoreNode((int64)TPQWAIT));
  ns->addConstant("TPRECVONLY", new QoreNode((int64)TPRECVONLY));
  ns->addConstant("TPSA_FASTPATH", new QoreNode((int64)TPSA_FASTPATH));
  ns->addConstant("TPSA_PROTECTED", new QoreNode((int64)TPSA_PROTECTED));
  ns->addConstant("TPSENDONLY", new QoreNode((int64)TPSENDONLY));
  ns->addConstant("TPSIGRSTRT", new QoreNode((int64)TPSIGRSTRT));
  ns->addConstant("TPSYSAUTH", new QoreNode((int64)TPSYSAUTH));
  ns->addConstant("TPU_DIP", new QoreNode((int64)TPU_DIP));
  ns->addConstant("TPU_IGN", new QoreNode((int64)TPU_IGN));
  ns->addConstant("TPU_SIG", new QoreNode((int64)TPU_SIG));
  ns->addConstant("TPU_THREAD", new QoreNode((int64)TPU_THREAD));
}

// EOF

