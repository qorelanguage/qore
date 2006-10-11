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
// Returns 0 if all is OK or tperrno code.
static QoreNode* f_tpalloc(QoreNode* params, ExceptionSink* xsink)
{
  char* all_params_err = "Four parameters expected (type, subtype, size, typed-buffer).";

  if (!get_param(params, 0)) {
    xsink->raiseException("tpalloc", all_params_err);
    return 0;
  }
  QoreNode* n = test_param(params, NT_STRING, 0);
  if (!n) {
    xsink->raiseException("tpalloc", "The first parameter, type, needs to be a string.");
    return 0;
  }
  char* type = n->val.String->getBuffer();

  if (!get_param(params, 1)) {
    xsink->raiseException("tpalloc", all_params_err);
    return 0;
  }
  n = test_param(params, NT_STRING, 1);
  if (!n) {
    xsink->raiseException("tpalloc", "The second parameter, the subtype, needs to be a string, possibly empty.");
    return 0;
  }  
  char* subtype = n->val.String->getBuffer();
 
  if (!get_param(params, 2)) {
    xsink->raiseException("tpalloc", all_params_err);
    return 0;
  }
  n = test_param(params, NT_INT, 2);
  if (!n) {
    xsink->raiseException("tpalloc", "The third parameter, size, needs to be an integer.");
    return 0;
  }
  long size = (long)n->val.intval;

  if (!get_param(params, 3)) {
    xsink->raiseException("tpalloc", all_params_err);
    return 0;
  }
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
  for (int i = 0; i < 6; ++i) {
    if (!get_param(params, i)) {
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

//------------------------------------------------------------------------------
void tuxedo_low_level_init()
{
  builtinFunctions.add("tpchkauth", f_tpchkauth, QDOM_NETWORK);
  builtinFunctions.add("tpalloc", f_tpalloc, QDOM_NETWORK);
  builtinFunctions.add("tpinit", f_tpinit, QDOM_NETWORK);
  builtinFunctions.add("tpinitParams", f_tpinit_params, QDOM_NETWORK);
  builtinFunctions.add("tpterm", f_tpterm, QDOM_NETWORK);
}

//-----------------------------------------------------------------------------
void tuxedo_low_level_ns_init(Namespace* ns)
{
  ns->addConstant("TPAPPAUTH", new QoreNode((int64)TPAPPAUTH));
  ns->addConstant("TPEINVAL", new QoreNode((int64)TPEINVAL));
  ns->addConstant("TPENOENT", new QoreNode((int64)TPENOENT));
  ns->addConstant("TPEOS", new QoreNode((int64)TPEOS));
  ns->addConstant("TPEPERM", new QoreNode((int64)TPEPERM));
  ns->addConstant("TPEPROTO", new QoreNode((int64)TPEPROTO));
  ns->addConstant("TPESYSTEM", new QoreNode((int64)TPESYSTEM));
  ns->addConstant("TPMULTICONTEXTS", new QoreNode((int64)TPMULTICONTEXTS));
  ns->addConstant("TPNOAUTH", new QoreNode((int64)TPNOAUTH));
  ns->addConstant("TPSA_FASTPATH", new QoreNode((int64)TPSA_FASTPATH));
  ns->addConstant("TPSA_PROTECTED", new QoreNode((int64)TPSA_PROTECTED));
  ns->addConstant("TPSYSAUTH", new QoreNode((int64)TPSYSAUTH));
  ns->addConstant("TPU_DIP", new QoreNode((int64)TPU_DIP));
  ns->addConstant("TPU_IGN", new QoreNode((int64)TPU_IGN));
  ns->addConstant("TPU_SIG", new QoreNode((int64)TPU_SIG));
  ns->addConstant("TPU_THREAD", new QoreNode((int64)TPU_THREAD));
}

// EOF

