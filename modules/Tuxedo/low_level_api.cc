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
#include <qore/QoreString.h>
#include <qore/minitest.hpp>
#include <qore/ScopeGuard.h>
#include <qore/VRMutex.h>

#include "low_level_api.h"
#include "QoreTuxedoTypedBuffer.h"
#include "QC_TuxedoTypedBuffer.h"
#include "QC_TuxedoQueueControlParams.h"
#include "QoreTuxedoQueueControlParams.h"
#include "QC_TuxedoTransactionId.h"
#include "QoreTuxedoTransactionId.h"
#include "QC_TuxedoContext.h"
#include "QoreTuxedoContext.h"
#include "hashed_parameters_helper.h"

#include <atmi.h>
#include <userlog.h>
#include <tx.h>

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
  if (n->val.object->getClass()->getID() != CID_TUXEDOTRANSACTIONID) {
    xsink->raiseException(func_name, "Type mismatch: expected instance of Tuxedo::TuxedoQueueControlParams class.");
    return 0;
  }
  // this should be safe now
  QoreTuxedoQueueControlParams* ctl = (QoreTuxedoQueueControlParams*)(n->val.object);
  return ctl;
}

//------------------------------------------------------------------------------
// n - known as an object
static QoreTuxedoTransactionId* node2transaction_id(QoreNode* n, char* func_name, ExceptionSink* xsink)
{
  if (!n->val.object) {
    xsink->raiseException(func_name, "Expected instance of Tuxedo::TuxedoTransactionId class.");
    return 0;
  }
  if (n->val.object->getClass()->getID() != CID_TUXEDOTRANSACTIONID) {
    xsink->raiseException(func_name, "Type mismatch: expected instance of Tuxedo::TuxedoTransactionId class.");
    return 0;
  }
  // this should be safe now
  QoreTuxedoTransactionId* trid = (QoreTuxedoTransactionId*)(n->val.object);
  return trid;
}

//------------------------------------------------------------------------------
// n - known as an object
static QoreTuxedoContext* node2context(QoreNode* n, char* func_name, ExceptionSink* xsink)
{
  if (!n->val.object) {
    xsink->raiseException(func_name, "Expected instance of Tuxedo::TuxedoContext class.");
    return 0;
  }
  if (n->val.object->getClass()->getID() != CID_TUXEDOCONTEXT) {
    xsink->raiseException(func_name, "Type mismatch: expected instance of Tuxedo::TuxedoContext class.");
    return 0;
  }
  // this should be safe now
  QoreTuxedoContext* ctx = (QoreTuxedoContext*)(n->val.object);
  return ctx;
}

//------------------------------------------------------------------------------
// http://edocs.bea.com/tuxedo/tux91/rf3c/rf3c28.htm#1040017
static QoreNode* f_tpchkauth(QoreNode* params, ExceptionSink* xsink)
{
  Tuxedo_hashed_parameters conn_params;

  if (get_param(params, 1)) {
    QoreNode* n = test_param(params, NT_HASH, 1);
    if (!n) {
      xsink->raiseException("tpchkauth", "The optional parameter, settings, needs to be a hash.");
      return 0;
    }
    conn_params.process_hash(n, xsink);
    if (*xsink) {
      return 0;
    }
  }
  int res = tpchkauth();
  List* l = new List;

  if (res == -1) {
    l->push(new QoreNode((int64)tperrno));
  } else {
    l->push(new QoreNode(OK));
    l->push(new QoreNode((int64)res));
  }
  return new QoreNode(l);
}

//------------------------------------------------------------------------------
// http://edocs.bea.com/tux91/rf3c/rf3c55.htm#1022852
// Variant with tpinfo == 0
// Parameters:
// * optional hash with environment variables
static QoreNode* f_tpinit(QoreNode* params, ExceptionSink* xsink)
{
  Tuxedo_hashed_parameters conn_params;

  if (get_param(params, 0)) {
    QoreNode* n = test_param(params, NT_HASH, 0);
    if (!n) {
      xsink->raiseException("tpinit", "The first optional parameter needs to be a hash with environment variables to set.");
      return 0;
    }
    conn_params.process_hash(n, xsink);
    if (xsink->isException()) {
      return 0;
    }

    if (get_param(params, 1)) {
      xsink->raiseException("tpinit", "Up to one parameter expected (hash with environment variables).");
      return 0;
    }
  }
  
  int res = tpinit(conn_params.get_tpinit_data());
  if (res == -1) {
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
// * optional hash with environment variable to be set
static QoreNode* f_tpinit_params(QoreNode* params, ExceptionSink* xsink)
{
  char* all_params_err = "Five to seven parameters expected: user name, client name, password, group name, flags, additional data (binary, optional), environment variables (hash, optional).";

  for (int i = 0; i < 5; ++i) {
    if (!get_param(params, i)) {
      xsink->raiseException("tpinit", all_params_err);
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

  int next_index = 5;
  if (get_param(params, 5)) {
    n = test_param(params, NT_BINARY, 5);
    if (n) {
      data = n->val.bin->getPtr();
      data_size = n->val.bin->size();
      ++next_index;
    }
  }

  Tuxedo_hashed_parameters conn_params;
  if (get_param(params, next_index)) {
    n = test_param(params, NT_HASH, next_index);
    if (!n) {
      xsink->raiseException("tpinit", "The sixth or seventh parameter, connection settings, needs to be a hash.");
      return 0;
    }
    conn_params.process_hash(n, xsink);
    if (xsink->isException()) {
      return 0;
    }
    ++next_index;
  } 
  
  if (get_param(params, next_index)) {
    xsink->raiseException("tpinit", all_params_err);
    return 0;
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

  char* par3_err = "The third parameter (output data) needs to be Tuxedo::TuxedoTypedBuffer instance passed by reference.";
  n = test_param(params, NT_REFERENCE, 2);
  if (!n) {
    xsink->raiseException("tpcall", par3_err);
    return 0;
  } 
  VLock vl;
  QoreNode **vp = get_var_value_ptr(n->val.lvexp, &vl, xsink);
  if (*xsink || !*vp) {
    xsink->raiseException("tpcall", par3_err);
    return 0;
  }
  if ((*vp)->type != NT_OBJECT) {
    xsink->raiseException("tpcall", par3_err);
    return 0;
  }
  
  QoreTuxedoTypedBuffer* out_buff = node2typed_buffer(*vp, "tpcall", xsink);
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
static QoreNode* f_tpacall(QoreNode* params, ExceptionSink* xsink)
{
  for (int i = 0; i <= 3; ++i) {
    bool ok;
    if (i == 3) ok = !get_param(params, i);
    else ok = get_param(params, i);
    if (!ok) {
      xsink->raiseException("tpacall", "Three parameters expected: service name string, input data TuxedoTypedBuffer, flags integer.");
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

  List* l = new List;
  int res = tpacall(service_name, buff->buffer, buff->size, flags);
  if (res == -1) {
    l->push(new QoreNode((int64)tperrno));
  } else {
    l->push(new QoreNode((int64)0));
    l->push(new QoreNode((int64)res));
  }
  
  return new QoreNode(l);
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
static QoreNode* f_tpdequeue(QoreNode* params, ExceptionSink* xsink)
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
// http://edocs.bea.com/tuxedo/tux91/rf3c/rf3c77.htm#1045232
static QoreNode* f_tpsprio(QoreNode* params, ExceptionSink* xsink)
{
  for (int i = 0; i <= 2; ++i) {
    bool ok;
    if (i == 2) ok = !get_param(params, i);
    else ok = get_param(params, i);
    if (!ok) {
      xsink->raiseException("tpsprio", "Two parameters expected: integer priority, integer flags.");
      return 0;
    }
  }
  
  QoreNode* n = test_param(params, NT_INT, 0);
  if (!n) {
    xsink->raiseException("tpsprio", "The first parameter, priority, needs to be an integer.");
    return 0;
  }
  int priority = (int)n->val.intval;

  n = test_param(params, NT_INT, 1);
  if (!n) {
    xsink->raiseException("tpsprio", "The second parameter, flags, needs to be an integer.");
    return 0;
  }
  long flags = (long)n->val.intval;

  if (tpsprio(priority, flags) == -1) {
    return new QoreNode((int64)tperrno);
  } else {
    return new QoreNode(OK);
  }
}

//-----------------------------------------------------------------------------
// http://edocs.bea.com/tuxedo/tux91/rf3c/rf3c53.htm#1042995
static QoreNode* f_tpgprio(QoreNode* params, ExceptionSink* xsink)
{
  if (get_param(params, 0)) {
    xsink->raiseException("tpgprio", "No parameter expected.");
    return 0;
  }

  int res = tpgprio();
  List* l = new List;
  if (res == -1) {
    l->push(new QoreNode((int64)tperrno));
  } else {
    l->push(new QoreNode((int64)0));
    l->push(new QoreNode((int64)res));
  }
  return new QoreNode(l);
}

//-----------------------------------------------------------------------------
// http://edocs.bea.com/tuxedo/tux91/rf3c/rf3c81.htm#1045633
static QoreNode* f_tpsuspend(QoreNode* params, ExceptionSink* xsink)
{
  for (int i = 0; i <= 2; ++i) {
    bool ok;
    if (i == 2) ok = !get_param(params, i);
    else ok = get_param(params, i);
    if (!ok) {
      xsink->raiseException("tpsuspend", "Two parameters expected: instance of TuxedoTranscationId passed by  reference and integer flags.");
      return 0;
    }
  }
  
  QoreNode* n = test_param(params, NT_OBJECT, 0);
  if (!n) {
    xsink->raiseException("tpsuspend", "The first parameter, transaction ID, needs to be instance of TuxedoTransactionId passed by reference.");
    return 0;
  }
  QoreTuxedoTransactionId* trid = node2transaction_id(n, "tpsuspend", xsink);
  if (xsink->isException()) {
    return 0;
  }

  n = test_param(params, NT_INT, 1);
  if (!n) {
    xsink->raiseException("tpsuspend", "The second parameter, flags, needs to be an integer.");
    return 0;
  }
  long flags = (long)n->val.intval;

  if (tpsuspend(&trid->trid, flags) == -1) {
    return new QoreNode((int64)tperrno);
  } else {
    return new QoreNode(OK);
  }
}

//-----------------------------------------------------------------------------
// http://edocs.bea.com/tuxedo/tux91/rf3c/rf3c65.htm#1044625
static QoreNode* f_tpresume(QoreNode* params, ExceptionSink* xsink)
{
  for (int i = 0; i <= 2; ++i) {
    bool ok;
    if (i == 2) ok = !get_param(params, i);
    else ok = get_param(params, i);
    if (!ok) {
      xsink->raiseException("tpresume", "Two parameters expected: instance of TuxedoTranscationId passed by  reference and integer flags.");
      return 0;
    }
  }

  QoreNode* n = test_param(params, NT_OBJECT, 0);
  if (!n) {
    xsink->raiseException("tpresume", "The first parameter, transaction ID, needs to be instance of TuxedoTransactionId passed by reference.");
    return 0;
  }
  QoreTuxedoTransactionId* trid = node2transaction_id(n, "tpresume", xsink);
  if (xsink->isException()) {
    return 0;
  }

  n = test_param(params, NT_INT, 1);
  if (!n) {
    xsink->raiseException("tpresume", "The second parameter, flags, needs to be an integer.");
    return 0;
  }
  long flags = (long)n->val.intval;

  if (tpresume(&trid->trid, flags) == -1) {
    return new QoreNode((int64)tperrno);
  } else {
    return new QoreNode(OK);
  }
}

//-----------------------------------------------------------------------------
// http://edocs.bea.com/tuxedo/tux91/rf3c/rf3c68.htm#1044795
static QoreNode* f_tpscmt(QoreNode* params, ExceptionSink* xsink)
{
  for (int i = 0; i <= 1; ++i) {
    bool ok;
    if (i == 1) ok = !get_param(params, i);
    else ok = get_param(params, i);
    if (!ok) {
      xsink->raiseException("tpscmt", "One parameter expected: integer flags.");
      return 0;
    }
  }

  QoreNode* n = test_param(params, NT_INT, 0);
  if (!n) {
    xsink->raiseException("tpscmt", "The first parameter, flags, needs to be an integer.");
    return 0;
  }
  long flags = (long)n->val.intval;

  if (tpscmt(flags) == -1) {
    return new QoreNode((int64)tperrno);
  } else {
    return new QoreNode(OK);
  }
}

//-----------------------------------------------------------------------------
// http://edocs.bea.com/tuxedo/tux91/rf3c/rf3c49.htm#122812
static QoreNode* f_tpgetlev(QoreNode* params, ExceptionSink* xsink)
{
  for (int i = 0; i <= 1; ++i) {
    bool ok;
    if (i == 1) ok = !get_param(params, i);
    else ok = get_param(params, i);
    if (!ok) {
      xsink->raiseException("tpgetlev", "One parameter expected, transaction indicator as  integer passed by reference.");
      return 0;
    }
  }

  QoreNode* n = test_param(params, NT_INT, 0);
  if (!n) {
    xsink->raiseException("tpgetlev", "The first parameter, transaction indicator, needs to be an integer passed by reference.");
    return 0;
  }
  int64* pindicator = &n->val.intval;

  int res = tpgetlev();
  if (res == -1) {
    return new QoreNode((int64)tperrno);
  }

  *pindicator = res;
  return new QoreNode(OK);
}

//-----------------------------------------------------------------------------
// http://edocs.bea.com/tuxedo/tux91/rf3c/rf3c78.htm#1023204
static QoreNode* f_tpstrerror(QoreNode* params, ExceptionSink* xsink)
{
  for (int i = 0; i <= 2; ++i) {
    bool ok;
    if (i == 2) ok = !get_param(params, i);
    else ok = get_param(params, i);
    if (!ok) {
      xsink->raiseException("tpstrerror", "Two parameters expected, integer, error,  out string passed by reference.");
      return 0;
    }
  }

  QoreNode* n = test_param(params, NT_INT, 0);
  if (!n) {
    xsink->raiseException("tpstrerror", "The first parameter, error, needs to be an integer.");
    return 0;
  }
  int err = (int)n->val.intval;

  n = test_param(params, NT_STRING, 1);
  if (!n) {
    xsink->raiseException("tpstrerror", "The second parameter, out error text, needs to be an string passed by reference.");
    return 0;
  }
  QoreString* s = n->val.String;

  char* out = tpstrerror(err);
  if (!out || !out[0]) {
    return new QoreNode((int64)TPEINVAL); // no tperrno set
  }
  if (s) s->set(out);
  return new QoreNode(OK);
}

//-----------------------------------------------------------------------------
// http://edocs.bea.com/tuxedo/tux91/rf3c/rfc362.htm#1044210
static QoreNode* f_tppost(QoreNode* params, ExceptionSink* xsink)
{
  for (int i = 0; i <= 3; ++i) {
    bool ok;
    if (i == 3) ok = !get_param(params, i);
    else ok = get_param(params, i);
    if (!ok) {
      xsink->raiseException("tppost", "Three parameters expected, string event name, data in TuxedoTypedBuffer instance passed by reference, integer flags.");
      return 0;
    }
  }
  
  QoreNode* n = test_param(params, NT_STRING, 0);
  if (!n) {
    xsink->raiseException("tppost", "The first parameter, event name,  needs to be a string.");
    return 0;
  }
  char* event_name = n->val.String->getBuffer();
  if (!event_name) event_name = "";

  n = test_param(params, NT_OBJECT, 1);
  if (!n) {
    xsink->raiseException("tppost", "The second parameter, event data, needs to instance of TuxedoQueueControlParams passed by reference.");
    return 0;
  }
  QoreTuxedoTypedBuffer* data = node2typed_buffer(n, "tppost", xsink);
  if (xsink->isException()) {
    return 0;
  }

  n = test_param(params, NT_INT, 2);
  if (!n) {
    xsink->raiseException("tppost", "The third parameter, flags, needs to be an integer.");
    return 0;
  }
  long flags = (long)n->val.intval;

  if (tppost(event_name, data->buffer, data->size, flags) == -1) {
    return new QoreNode((int64)tperrno);
  } else {
    return new QoreNode(OK);
  }
}

//-----------------------------------------------------------------------------
// http://edocs.bea.com/tuxedo/tux91/rf3c/rf3c101.htm#1046168
static QoreNode* f_tx_begin(QoreNode* params, ExceptionSink* xsink)
{
  if (get_param(params, 0)) {
    xsink->raiseException("tx_begin", "No parameters expected.");
    return 0;
  }

  int res = tx_begin();
  if (res == TX_OK) {
    return new QoreNode(OK);
  }
  return new QoreNode((int64)res);
}

//-----------------------------------------------------------------------------
// http://edocs.bea.com/tuxedo/tux91/rf3c/rf3c102.htm#1046217
static QoreNode* f_tx_close(QoreNode* params, ExceptionSink* xsink)
{
  if (get_param(params, 0)) {
    xsink->raiseException("tx_close", "No parameters expected.");
    return 0;
  }

  int res = tx_close();
  if (res == TX_OK) {
    return new QoreNode(OK);
  }
  return new QoreNode((int64)res);
}

//-----------------------------------------------------------------------------
// http://edocs.bea.com/tuxedo/tux91/rf3c/rf3c103.htm#1046266
static QoreNode* f_tx_commit(QoreNode* params, ExceptionSink* xsink)
{
  if (get_param(params, 0)) {
    xsink->raiseException("tx_commit", "No parameters expected.");
    return 0;
  }

  int res = tx_commit();
  if (res == TX_OK) {
    return new QoreNode(OK);
  }
  return new QoreNode((int64)res);
}

//-----------------------------------------------------------------------------
// http://edocs.bea.com/tuxedo/tux91/rf3c/rf3c105.htm#1046379
static QoreNode* f_tx_open(QoreNode* params, ExceptionSink* xsink)
{
  if (get_param(params, 0)) {
    xsink->raiseException("tx_open", "No parameters expected.");
    return 0;
  }

  int res = tx_open();
  if (res == TX_OK) {
    return new QoreNode(OK);
  }
  return new QoreNode((int64)res);
}

//-----------------------------------------------------------------------------
// http://edocs.bea.com/tuxedo/tux91/rf3c/rf3c106.htm#1046426
static QoreNode* f_tx_rollback(QoreNode* params, ExceptionSink* xsink)
{
  if (get_param(params, 0)) {
    xsink->raiseException("tx_rollback", "No parameters expected.");
    return 0;
  }

  int res = tx_rollback();
  if (res == TX_OK) {
    return new QoreNode(OK);
  }
  return new QoreNode((int64)res);
}

//-----------------------------------------------------------------------------
// http://edocs.bea.com/tuxedo/tux91/rf3c/rf3c48.htm#1226460
static QoreNode* f_tpgetctxt(QoreNode* params, ExceptionSink* xsink)
{
  for (int i = 0; i <= 1; ++i) {
    bool ok;
    if (i == 1) ok = !get_param(params, i);
    else ok = get_param(params, i);
    if (!ok) {
      xsink->raiseException("tpgetctxt", "One parameter expected, instance of TuxedoContext passed by reference.");
      return 0;
    }
  }
  
  QoreNode* n = test_param(params, NT_OBJECT, 0);
  if (!n) {
    xsink->raiseException("tpgetctxt", "The first parameter needs to be instance of TuxedoContext passed by reference.");
    return 0;
  }
  QoreTuxedoContext* ctx = node2context(n, "tpgetctxt", xsink);
  if (xsink->isException()) {
    return 0;
  }
  int res = tpgetctxt(&ctx->ctx, 0);
  if (res == -1) {
    return new QoreNode((int64)tperrno);
  }
  if (ctx->ctx == TPINVALIDCONTEXT) {
    return new QoreNode((int64)TPINVALIDCONTEXT);
  }
  if (ctx->ctx == TPNULLCONTEXT) {
    return new QoreNode((int64)TPNULLCONTEXT);
  }

  return new QoreNode(OK);
}

//-----------------------------------------------------------------------------
// http://edocs.bea.com/tuxedo/tux91/rf3c/rf3c72.htm#1223538
static QoreNode* f_tpsetctxt(QoreNode* params, ExceptionSink* xsink)
{
  for (int i = 0; i <= 1; ++i) {
    bool ok;
    if (i == 1) ok = !get_param(params, i);
    else ok = get_param(params, i);
    if (!ok) {
      xsink->raiseException("tpsetctxt", "One parameter expected, instance of TuxedoContext possibly by reference.");
      return 0;
    }
  }

  QoreNode* n = test_param(params, NT_OBJECT, 0);
  if (!n) {
    xsink->raiseException("tpsetctxt", "The first parameter needs to be instance of TuxedoContext possibly passed by reference.");
    return 0;
  }
  QoreTuxedoContext* ctx = node2context(n, "tpsetctxt", xsink);
  if (xsink->isException()) {
    return 0;
  }
  
  if (tpsetctxt(ctx->ctx, 0) == -1) {
    return new QoreNode((int64)tperrno);
  } else {
    return new QoreNode(OK);
  }
}

//-----------------------------------------------------------------------------
// http://edocs.bea.com/tuxedo/tux91/rf3c/rf3c107.htm#1046490
static QoreNode* f_tx_set_commit_return(QoreNode* params, ExceptionSink* xsink)
{
  for (int i = 0; i <= 1; ++i) {
    bool ok;
    if (i == 1) ok = !get_param(params, i);
    else ok = get_param(params, i);
    if (!ok) {
      xsink->raiseException("tx_set_commit_return", "One parameter expected, integer flags");
      return 0;
    }
  }
  
  QoreNode* n = test_param(params, NT_INT, 0);
  if (!n) {
    xsink->raiseException("tx_set_commit_return", "The first parameter, flags, needs to be an integer.");
    return 0;
  }
  long flags = (long)n->val.intval;

  int res = tx_set_commit_return(flags);
  if (res == TX_OK) {
    return new QoreNode(OK);
  }
  return new QoreNode((int64)res);
}

//-----------------------------------------------------------------------------
// http://edocs.bea.com/tuxedo/tux91/rf3c/rf3c108.htm#1046542
static QoreNode* f_tx_set_transaction_control(QoreNode* params, ExceptionSink* xsink)
{
  for (int i = 0; i <= 1; ++i) {
    bool ok;
    if (i == 1) ok = !get_param(params, i);
    else ok = get_param(params, i);
    if (!ok) {
      xsink->raiseException("tx_set_transaction_control", "One parameter expected, integer flags");
      return 0;
    }
  }

  QoreNode* n = test_param(params, NT_INT, 0);
  if (!n) {
    xsink->raiseException("tx_set_transaction_control", "The first parameter, flags, needs to be an integer.");
    return 0;
  }
  long flags = (long)n->val.intval;

  int res = tx_set_transaction_control(flags);
  if (res == TX_OK) {
    return new QoreNode(OK);
  }
  return new QoreNode((int64)res);
}

//-----------------------------------------------------------------------------
// http://edocs.bea.com/tuxedo/tux91/rf3c/rf3c109.htm#1046597
static QoreNode* f_tx_set_transaction_timeout(QoreNode* params, ExceptionSink* xsink)
{
  for (int i = 0; i <= 1; ++i) {
    bool ok;
    if (i == 1) ok = !get_param(params, i);
    else ok = get_param(params, i);
    if (!ok) {
      xsink->raiseException("tx_set_transaction_timeout", "One parameter expected, integer timeout in seconds");
      return 0;
    }
  }

  QoreNode* n = test_param(params, NT_INT, 0);
  if (!n) {
    xsink->raiseException("tx_set_transaction_timeout", "The first parameter, timeout in seconds, needs to be an integer.");
    return 0;
  }
  long timeout = (long)n->val.intval;

  int res = tx_set_transaction_timeout(timeout);
  if (res == TX_OK) {
    return new QoreNode(OK);
  }
  return new QoreNode((int64)res);
}

//-----------------------------------------------------------------------------
void tuxedo_low_level_init()
{
  builtinFunctions.add("tpchkauth", f_tpchkauth, QDOM_NETWORK);
  builtinFunctions.add("tpinit", f_tpinit, QDOM_NETWORK);
  builtinFunctions.add("tpinitParams", f_tpinit_params, QDOM_NETWORK);
  builtinFunctions.add("tpterm", f_tpterm, QDOM_NETWORK);
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
  builtinFunctions.add("tpdequeue", f_tpdequeue, QDOM_NETWORK);
  builtinFunctions.add("tperrordetail", f_tperrordetail, QDOM_NETWORK);
  builtinFunctions.add("TuxedoUserlog", f_userlog, QDOM_NETWORK);
  builtinFunctions.add("tpsprio", f_tpsprio, QDOM_NETWORK);
  builtinFunctions.add("tpgprio", f_tpgprio, QDOM_NETWORK);
  builtinFunctions.add("tpsuspend", f_tpsuspend, QDOM_NETWORK);
  builtinFunctions.add("tpresume", f_tpresume, QDOM_NETWORK);
  builtinFunctions.add("tpscmt", f_tpscmt, QDOM_NETWORK);
  builtinFunctions.add("tpgetlev", f_tpgetlev, QDOM_NETWORK);
  builtinFunctions.add("tpstrerror", f_tpstrerror, QDOM_NETWORK);
  builtinFunctions.add("tppost", f_tppost, QDOM_NETWORK);
  builtinFunctions.add("tx_begin", f_tx_begin, QDOM_NETWORK);
  builtinFunctions.add("tx_close", f_tx_close, QDOM_NETWORK);
  builtinFunctions.add("tx_commit", f_tx_commit, QDOM_NETWORK);
  builtinFunctions.add("tx_open", f_tx_open, QDOM_NETWORK);
  builtinFunctions.add("tx_rollback", f_tx_rollback, QDOM_NETWORK);
  builtinFunctions.add("tpgetctxt", f_tpgetctxt, QDOM_NETWORK);
  builtinFunctions.add("tpsetctxt", f_tpsetctxt, QDOM_NETWORK);
  builtinFunctions.add("tx_set_commit_return", f_tx_set_commit_return, QDOM_NETWORK);
  builtinFunctions.add("tx_set_transaction_control", f_tx_set_transaction_control, QDOM_NETWORK);
  builtinFunctions.add("tx_set_transaction_timeout", f_tx_set_transaction_timeout, QDOM_NETWORK);
}

//-----------------------------------------------------------------------------
void tuxedo_low_level_ns_init(Namespace* ns)
{
  // Queueing errors and constants
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

  // ATMI errors and constants
  ns->addConstant("TP_CMT_COMPLETE", new QoreNode((int64)TP_CMT_COMPLETE));
  ns->addConstant("TP_CMT_LOGGED", new QoreNode((int64)TP_CMT_LOGGED));
  ns->addConstant("TPEV_DISCONIMM", new QoreNode((int64)TPEV_DISCONIMM));
  ns->addConstant("TPEV_SVCERR", new QoreNode((int64)TPEV_SVCERR));
  ns->addConstant("TPEV_SVCFAIL", new QoreNode((int64)TPEV_SVCFAIL));
  ns->addConstant("TPEV_SVCSUCC", new QoreNode((int64)TPEV_SVCSUCC));
  ns->addConstant("TPABSOLUTE", new QoreNode((int64)TPABSOLUTE));
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
  ns->addConstant("TPERMERR", new QoreNode((int64)TPERMERR));
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

  // X/Open errors and constants
  ns->addConstant("TX_CHAINED", new QoreNode((int64)TX_CHAINED));
  ns->addConstant("TX_COMMITTED", new QoreNode((int64)TX_COMMITTED));
  ns->addConstant("TX_COMMITTED_NO_BEGIN", new QoreNode((int64)TX_COMMITTED_NO_BEGIN));
  ns->addConstant("TX_COMMIT_COMPLETED", new QoreNode((int64)TX_COMMIT_COMPLETED));
  ns->addConstant("TX_COMMIT_DECISION_LOGGED", new QoreNode((int64)TX_COMMIT_DECISION_LOGGED));
  ns->addConstant("TX_EINVAL", new QoreNode((int64)TX_EINVAL));
  ns->addConstant("TX_ERROR", new QoreNode((int64)TX_ERROR));
  ns->addConstant("TX_FAIL", new QoreNode((int64)TX_FAIL));
  ns->addConstant("TX_HAZARD", new QoreNode((int64)TX_HAZARD));
  ns->addConstant("TX_HAZARD_NO_BEGIN", new QoreNode((int64)TX_HAZARD_NO_BEGIN));
  ns->addConstant("TX_MIXED", new QoreNode((int64)TX_MIXED));
  ns->addConstant("TX_MIXED_NO_BEGIN", new QoreNode((int64)TX_MIXED_NO_BEGIN));
  ns->addConstant("TX_NO_BEGIN", new QoreNode((int64)TX_NO_BEGIN));
  ns->addConstant("TX_OK", new QoreNode((int64)TX_OK));
  ns->addConstant("TX_OUTSIDE", new QoreNode((int64)TX_OUTSIDE));
  ns->addConstant("TX_PROTOCOL_ERROR", new QoreNode((int64)TX_PROTOCOL_ERROR));
  ns->addConstant("TX_ROLLBACK", new QoreNode((int64)TX_ROLLBACK));
  ns->addConstant("TX_ROLLBACK_NO_BEGIN", new QoreNode((int64)TX_ROLLBACK_NO_BEGIN));
  ns->addConstant("TX_UNCHAINED", new QoreNode((int64)TX_UNCHAINED));
}

//-----------------------------------------------------------------------------
#ifdef DEBUG
TEST()
{
  // Test tpchkauth() - assumes a server is running 
  // and $TUXDIR and $TUXCONFIG set. 
  // Should work for any test on server side.
  char* tuxconfig = getenv("TUXCONFIG");
  if (!tuxconfig || !tuxconfig[0]) {
    return;
  }

  char* cmd = "qore -e '%requires tuxedo\n"
    "$res = tpchkauth();\n"
    "if ($res[0] != 0) exit(11);\n"
    "switch ($res[1]) {\n"
    "  case Tuxedo::TPNOAUTH:\n"
    "  case Tuxedo::TPSYSAUTH:\n"
    "  case Tuxedo::TPAPPAUTH:\n"
    "    break;\n"
    "  default: exit(11);\n"
    "}\n"
    "exit(10);'\n";

  int res = system(cmd);
  res = WEXITSTATUS(res);
  assert(res == 10);

  // test it with settings hash
  char buffer[1024];
  sprintf(buffer, "qore -e '%%requires tuxedo\n"
    "$settings = (\"TUXCONFIG\" : \"%s\");\n"
    "$res = tpchkauth($settings);\n"
    "if ($res[0] != 0) exit(11);\n"
    "switch ($res[1]) {\n"
    "  case Tuxedo::TPNOAUTH:\n"
    "  case Tuxedo::TPSYSAUTH:\n"
    "  case Tuxedo::TPAPPAUTH:\n"
    "    break;\n"
    "  default: exit(11);\n"
    "}\n"
    "exit(10);'\n",
    tuxconfig);

  res = system(buffer);
  res = WEXITSTATUS(res);
  assert(res == 10);
}

TEST()
{
  // Test tpinit() - assumes a server is running
  // and $TUXDIR and $TUXCONFIG set.
  // Should work for any test on server side.
  char* tuxconfig = getenv("TUXCONFIG");
  if (!tuxconfig || !tuxconfig[0]) {
    return;
  }
  
  // use env. variables for everything
  char* cmd = "qore -e '%requires tuxedo\n"
    "$res = tpinit();\n"
    "if ($res != 0) exit(11);\n"
    "$res = tpterm();\n"
    "if ($res != 0) exit(11);\n"
    "exit(10);'\n";

  int res = system(cmd);
  res = WEXITSTATUS(res);
  assert(res == 10);

  // tpinit() with settings hash
  char buffer[1024];
  sprintf(buffer, "qore -e '%%requires tuxedo\n"
    "$settings = (\"TUXCONFIG\" : \"%s\");\n"
    "$res = tpinit($settings);\n"
    "if ($res != 0) exit(11);\n"
    "$res = tpterm();\n"
    "if ($res != 0) exit(11);\n"
    "exit(10);'\n",
    tuxconfig);

  res = system(buffer);
  res = WEXITSTATUS(res);
  assert(res == 10);
}

TEST()
{
  // Test tpinitParams() - assumes a server is running
  // and $TUXDIR and $TUXCONFIG set.
  // Should work for any test on server side.
  char* tuxconfig = getenv("TUXCONFIG");
  if (!tuxconfig || !tuxconfig[0]) {
    return;
  }
  
  char* cmd = "qore -e '%requires tuxedo\n"
    "$res = tpinitParams(\"\", \"\", \"\", \"\", 0);\n"
    "if ($res != 0) exit(11);\n"
    "$res = tpterm();\n"
    "if ($res != 0) exit(11);\n"
    "exit(10);'\n";

  int res = system(cmd);
  res = WEXITSTATUS(res);
  assert(res == 10);

  // add settings hash
  char buffer[1024];
  sprintf(buffer, "qore -e '%%requires tuxedo\n"
    "$settings = (\"TUXCONFIG\" : \"%s\");\n"
    "$res = tpinitParams(\"\", \"\", \"\", \"\", 0, $settings);\n"
    "if ($res != 0) exit(11);\n"
    "$res = tpterm();\n"
    "if ($res != 0) exit(11);\n"
    "exit(10);'\n",
     tuxconfig);

  res = system(buffer);
  res = WEXITSTATUS(res);
  assert(res == 10);
}

TEST()
{
  // Test tpgprio() and tpsprio() - assumes a server is running
  // and $TUXDIR and $TUXCONFIG set.
  // Should work for any test on server side.
  char* tuxconfig = getenv("TUXCONFIG");
  if (!tuxconfig || !tuxconfig[0]) {
    return;
  }

  if (tpinit(0) == -1) {
    assert(false);
  }
  ON_BLOCK_EXIT(tpterm);

  ExceptionSink xsink;
  List* l = new List;
  QoreNode* params = new QoreNode(l);

  // this will FAIL as there's no tpacall() yet
  QoreNode* res = f_tpgprio(params, &xsink);
  assert(!xsink);
  assert(res);
  assert(res->type == NT_LIST);
  assert(res->val.list->size() == 1); // the error

  QoreNode* code = test_param(res, NT_INT, 0);
  assert(code);
  int errcode = (int)code->val.intval;
  assert(errcode != 0); // an error code expected

  res->deref(&xsink);
  assert(!xsink);

  params->deref(&xsink);
  assert(!xsink);
}

TEST()
{
  // test tpgprio() and tpsprio() in Qore
  char* tuxconfig = getenv("TUXCONFIG");
  if (!tuxconfig || !tuxconfig[0]) {
    return;
  }
/*###
  char* cmd = "qore -e '%requires tuxedo\n"
    "$res = tpinit();\n"
    "if ($res != 0) {printf(\"tpinit err = %d\n\", $res); exit(11);}\n"
    "$data = new Tuxedo::TuxedoTypedBuffer();\n"
    "$data.setString(\"abcd\");\n"
    "$res = tpacall(\"TOUPPER\", $data, Tuxedo::TPNOTRAN);\n" // the prior tpacall() is needed
    "if ($res[0] != 0) {printf(\"tpacall err = %d\n\", $res[0]); exit(11);}\n"
    "$handle = $res[1];\n"
//    "$res = tpgprio();\n"
//  "if ($res[0] != 0) exit(11);\n"   - this still returns TPENOENT. No idea what does it need.
//    "$res = tpsprio(60, Tuxedo::TPABSOLUTE);\n"
    "$res = tpcancel($handle);\n"
    "if ($res != 0) {printf(\"tpcancel err = %d\n\", $res); exit(11);}\n"
    "$res = tpterm();\n"
    "if ($res != 0) {printf(\"tpterm err = %d\n\", $res); exit(10);}\n"
    "exit(10);'\n";

  int res = system(cmd);
  res = WEXITSTATUS(res);
  assert(res == 10);
*/ 
}

#endif // DEBUG

// EOF

