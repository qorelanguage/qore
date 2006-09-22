#include <qore/config.h>
#include <qore/support.h>
#include <qore/QoreString.h>
#include <qore/Exception.h>
#include <qore/Namespace.h>
#include <qore/ModuleManager.h>
#include <qore/Object.h>

#include "tuxedo_module.h"
#include "TuxedoTest.h"
extern "C" {
#include "atmi.h"
}

#ifndef QORE_MONOLITHIC
char qore_module_name[] = "tuxedo";
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
class QoreString* tuxedo_module_init()
{
  tracein("tuxedo_module_init");
  printf("INITIALISING TUXEDO\n");

  if (tpinit((TPINIT*)0) == -1) {
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
  
  class Namespace* tuxedons = new Namespace("tuxedo");
  tuxedons->addSystemClass(initTuxedoTestClass());
  qns->addInitialNamespace(tuxedons);

  traceout("tuxedo_module_ns_init");
}

//------------------------------------------------------------------------------
void tuxedo_module_delete()
{
  tracein("tuxedo_module_delete");
  tpterm();
  traceout("tuxedo_module_delete");
}

// EOF

