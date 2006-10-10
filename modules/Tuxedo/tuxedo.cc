/*
  modules/Tuxedo/tuxedo.cc

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

#include <qore/config.h>
#include <qore/support.h>
#include <qore/QoreString.h>
#include <qore/Exception.h>
#include <qore/Namespace.h>
#include <qore/ModuleManager.h>
#include <qore/Object.h>
#include <qore/BuiltinFunctionList.h>

#include "tuxedo_module.h"

#include "QC_TuxedoConnection.h"
#include "QC_TuxedoAdapter.h"

#include <atmi.h>
#include "handle_error.h"

#ifndef QORE_MONOLITHIC
char qore_module_name[] = "tuxedo";
char qore_module_version[] = "0.1";
char qore_module_description[] = "Connection to Tuxedo 9.1 (should be compatible with solder versions)";
char qore_module_author[] = "Pavel Vozenilek";
char qore_module_url[] = "http://qore.sourceforge.net";
int qore_module_api_major = QORE_MODULE_API_MAJOR;
int qore_module_api_minor = QORE_MODULE_API_MINOR;
qore_module_init_t qore_module_init = tuxedo_module_init;
qore_module_ns_init_t qore_module_ns_init =tuxedo_module_ns_init;
qore_module_delete_t qore_module_delete = tuxedo_module_delete;
#endif

//------------------------------------------------------------------------------
// values returned by Qore function tpchkauth()
const int64 NO_AUTHENTICATION             = 1;
const int64 SYSTEM_AUTHENTICATION         = 2;
const int64 SYSTEM_AND_APP_AUTHENTICATION = 3;

//------------------------------------------------------------------------------
// Find out what kind of authentication Tuxedo requires. 
// Returns constants Tuxedo::NO_AUTHENTICATION, Tuxedo::SYSTEM_AUTHENTICATION
// or Tuxedo::SYSTEM_AND_APP_AUTHENTICATION. On error it throws.
//
// See Tuxedo tpchkauth() documentation for more: http://edocs.bea.com/tuxedo/tux91/rf3c/rf3c28.htm#1040017
//
static class QoreNode* f_authentication_required_by_Tuxedo(class QoreNode* params, class ExceptionSink* xsink)
{
  int res = tpchkauth();
  switch (res) {
  case TPNOAUTH: return new QoreNode(NO_AUTHENTICATION);
  case TPSYSAUTH: return new QoreNode(SYSTEM_AUTHENTICATION);
  case TPAPPAUTH: return new QoreNode(SYSTEM_AND_APP_AUTHENTICATION);
  }
  if (res == -1) {
    handle_error(tperrno, "TPCHKAUTH", "tpchkauth()", xsink);
  } else {
    // undocumented return value
    xsink->raiseException("TPCHKAUTH", "Tuxedo tpchkauth() returned unexpected result %d.", res);
  }
  return 0;
}

//------------------------------------------------------------------------------
class QoreString* tuxedo_module_init()
{
  tracein("tuxedo_module_init");

  // Standalone method tpchkauth() returning info what kind
  // of authentication is required.
  //
  builtinFunctions.add("tpchkauth", f_authentication_required_by_Tuxedo, QDOM_NETWORK);

  traceout("tuxedo_module_init");
  return NULL;
}

//------------------------------------------------------------------------------
void tuxedo_module_ns_init(class Namespace* rns, class Namespace* qns)
{
  tracein("tuxedo_module_ns_init");

  class Namespace* tuxedons = new Namespace("Tuxedo");

  // returned values of tpchkauth()
  tuxedons->addConstant("NO_AUTHENTICATION", new QoreNode(NO_AUTHENTICATION));
  tuxedons->addConstant("SYSTEM_AUTHENTICATION", new QoreNode(SYSTEM_AUTHENTICATION));
  tuxedons->addConstant("SYSTEM_AND_APP_AUTHENTICATION", new QoreNode(SYSTEM_AND_APP_AUTHENTICATION));
  
  
  // Tuxedo::Connection class
  tuxedons->addSystemClass(initTuxedoConnectionClass());
  // Tuxedo:Adapter class
  tuxedons->addSystemClass(initTuxedoAdapterClass());
  
  // misc Tuxedo constants
  tuxedons->addConstant("TPNOTRAN", new QoreNode((int64)TPNOTRAN));
  tuxedons->addConstant("TPNOCHANGE", new QoreNode((int64)TPNOCHANGE));
  tuxedons->addConstant("TPNOBLOCK", new QoreNode((int64)TPNOBLOCK));
  tuxedons->addConstant("TPNOTIME", new QoreNode((int64)TPNOTIME));
  tuxedons->addConstant("TPSIGRSTRT", new QoreNode((int64)TPSIGRSTRT));
  tuxedons->addConstant("TPNOREPLY", new QoreNode((int64)TPNOREPLY));
  tuxedons->addConstant("TPGETANY", new QoreNode((int64)TPGETANY));
  tuxedons->addConstant("TPSENDONLY", new QoreNode((int64)TPSENDONLY));
  tuxedons->addConstant("TPRECVONLY", new QoreNode((int64)TPRECVONLY));
 
  qns->addInitialNamespace(tuxedons);

  traceout("tuxedo_module_ns_init");
}

//------------------------------------------------------------------------------
void tuxedo_module_delete()
{
  tracein("tuxedo_module_delete");
  traceout("tuxedo_module_delete");
}

// EOF

