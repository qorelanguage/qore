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

//------------------------------------------------------------------------------
// http://edocs.bea.com/tuxedo/tux91/rf3c/rf3c28.htm#1040017
static QoreNode* f_tpchkauth(QoreNode* params, ExceptionSink* xsink)
{
  if (get_param(params, 0)) {
    xsink->raiseException("tpchkauth", "tpchkauth() has no parameters.");
    return 0;
  }
  int res = tpchkauth();
  return new QoreNode((int64)res);
}

//-----------------------------------------------------------------------------
// http://edocs.bea.com/tuxedo91/rf3c/rf3c23.htm#1021676
// The difference is that instead of returning char* it 
// fills instance of Tuxedo::TuxedoTypedBuffer in the last parameter.
// Returns 0 if all is OK or tperrno code.
static QoreNode* f_tpalloc(QoreNode* params, ExceptionSink* xsink)
{
  char* all_params_err = "tpalloc() requires four parameters (type, subtype, size, typed-buffer).";

  if (!get_param(params, 0)) {
    xsink->raiseException("tpalloc", all_params_err);
    return 0;
  }
  QoreNode* n = test_param(params, NT_STRING, 0);
  if (!n) {
    xsink->raiseException("tpalloc", "The first parameter of tpalloc(), type, needs to be a string.");
    return 0;
  }
  char* type = n->val.String->getBuffer();

  if (!get_param(params, 1)) {
    xsink->raiseException("tpalloc", all_params_err);
    return 0;
  }
  n = test_param(params, NT_STRING, 1);
  if (!n) {
    xsink->raiseException("tpalloc", "The second parameter of tpalloc(), the subtype, needs to be a string, possibly empty.");
    return 0;
  }  
  char* subtype = n->val.String->getBuffer();
 
  if (!get_param(params, 2)) {
    xsink->raiseException("tpalloc", all_params_err);
    return 0;
  }
  n = test_param(params, NT_INT, 2);
  if (!n) {
    xsink->raiseException("tpalloc", "The third parameter of tpalloc(), size, needs to be an integer.");
    return 0;
  }
  long size = (long)n->val.intval;

  if (!get_param(params, 3)) {
    xsink->raiseException("tpalloc", all_params_err);
    return 0;
  }
  n = test_param(params, NT_OBJECT, 3);
  if (!n) {
    xsink->raiseException("tpalloc", "The fourth parameter of tpalloc(), typed buffer, needs to be an object.");
    return 0;
  }  
  if (!n->val.object) {
    xsink->raiseException("tpalloc", "The fourth parameter of tpalloc(), typed buffer, is NULL.");
  }
  if (n->val.object->getClass()->getID() != CID_TUXEDOTYPEDBUFFER) {
    xsink->raiseException("tpcall", "The fourth parameter of tpalloc() is not Tuxedo::TuxedoTypedBuffer instance.");
    return 0;
  }
  // this should be safe now
  QoreTuxedoTypedBuffer* buff = (QoreTuxedoTypedBuffer*)(n->val.object);
  
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
// http://edocs.bea.com/tux91/rf3c/rf3c55.htm#1022852
// Variant w/o parameters (tpinfo == 0)
static QoreNode* f_tpinit(QoreNode* params, ExceptionSink* xsink)
{
  int res = tpinit(0);
  return new QoreNode((int64)res);
}

//------------------------------------------------------------------------------
void tuxedo_low_level_init()
{
  builtinFunctions.add("tpchkauth", f_tpchkauth, QDOM_NETWORK);
  builtinFunctions.add("tpalloc", f_tpalloc, QDOM_NETWORK);
  builtinFunctions.add("tpinit", f_tpinit, QDOM_NETWORK);
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

