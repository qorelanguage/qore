#include <qore/config.h>
#include <qore/support.h>
#include <qore/QoreString.h>
#include <qore/Exception.h>
#include <qore/Namespace.h>
#include <qore/ModuleManager.h>
#include <qore/Object.h>
#include <qore/BuiltinFunctionList.h>

#include "tuxedo_module.h"
#include "TuxedoTest.h" // to be deleted later

#include "QC_TuxedoConnection.h"
#include "QC_TuxedoTypedBuffer.h"


#include "atmi.h"

#ifndef QORE_MONOLITHIC
char qore_module_name[] = "Tuxedo";
char qore_module_version[] = "0.1";
char qore_module_description[] = "Conection to Tuxedo 9.1";
char qore_module_author[] = "Pavel Vozenilek";
char qore_module_url[] = "http://qore.sourceforge.net";
int qore_module_api_major = QORE_MODULE_API_MAJOR;
int qore_module_api_minor = QORE_MODULE_API_MINOR;
qore_module_init_t qore_module_init = tuxedo_module_init;
qore_module_ns_init_t qore_module_ns_init =tuxedo_module_ns_init;
qore_module_delete_t qore_module_delete = tuxedo_module_delete;
#endif

//------------------------------------------------------------------------------
// Find out what kind of authentification Tuxedo requires. Return strings
// "none", "system-only", "system-and-app" or throw exception on error.
// 
// Standalone function to be used before Tuxedo::Connection is created.
//
static class QoreNode* f_authentification_required_by_Tuxedo(class QoreNode* params, class ExceptionSink* xsink)
{
  int res = tpchkauth();
  switch (res) {
  case TPNOAUTH: return new QoreNode("none");
  case TPSYSAUTH: return new QoreNode("system-only");
  case TPAPPAUTH: return new QoreNode("system-and-app");
  }
  if (res == -1) {
    // Tuxedo error
    switch (tperrno) {
    case TPESYSTEM: 
      xsink->raiseException("authentification_required_by_Tuxedo()", "tpchkauth() failed due to Tuxedo system error. See Tuxedo log file for details.\n");
      break;
    case TPEOS:
      xsink->raiseException("authentification_required_by_Tuxedo()", "tpchkauth() failed due to OS error.\n");
      break;
    default:
      xsink->raiseException("authentification_required_by_Tuxedo()", "tpchkauth() failed due to unknown reason.\n");
    }
  } else {
    // undocumented return value
    xsink->raiseException("authentification_required_by_Tuxedo()", "tpchkauth() returned unexpected result %d", res);
  }
  return 0;
}

//------------------------------------------------------------------------------
class QoreString* tuxedo_module_init()
{
  tracein("tuxedo_module_init");
  printf("INITIALISING TUXEDO\n");

  if (tpinit((TPINIT*)0) == -1) { // to be deleted later
printf("TUXEDO INIT failed\n");
    QoreString* err = new QoreString();
    err->sprintf("Failed to initialize Tibco, tpinit(NULL) returned -1\n");
    return err;
  }
  printf("TUXEDO initializsed OK\n");

  traceout("tuxedo_module_init");
  return NULL;
}

//------------------------------------------------------------------------------
void tuxedo_module_ns_init(class Namespace* rns, class Namespace* qns)
{
  tracein("tuxedo_module_ns_init");
  builtinFunctions.add("authentification_required_by_Tuxedo", f_authentification_required_by_Tuxedo, QDOM_NETWORK);
  
  class Namespace* tuxedons = new Namespace("Tuxedo");
  
  tuxedons->addSystemClass(initTuxedoTestClass()); // to be deleted later
//  tuxedons->addSystemClass(initTuxedoTypedBufferClass());
 
  qns->addInitialNamespace(tuxedons);

  traceout("tuxedo_module_ns_init");
}

//------------------------------------------------------------------------------
void tuxedo_module_delete()
{
  tracein("tuxedo_module_delete");
  tpterm(); // to be deleted later
  traceout("tuxedo_module_delete");
}

// EOF

