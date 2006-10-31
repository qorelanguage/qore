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
#include <qore/minitest.hpp>
#include <qore/BuiltinFunctionList.h>

#include "tuxedo_module.h"

#include "QC_TuxedoTypedBuffer.h"
#include "low_level_api.h"
#include "QC_TuxedoTransactionId.h"
#include "QC_TuxedoQueueControlParams.h"
#include "QC_TuxedoContext.h"
#include "fml_api.h"

#include <atmi.h>

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
#ifdef DEBUG
// exported
QoreNode* runTuxedoTests(QoreNode* params, ExceptionSink* xsink)
{
  minitest::result res = minitest::execute_all_tests();
  if (res.all_tests_succeeded) {
    printf("Tuxedo module: %d tests succeeded\n", res.sucessful_tests_count);
    return 0;
  }
  
  xsink->raiseException("Tuxedo test failed", "Tuxedo test in file %s, line %d threw an exception.", 
    res.failed_test_file, res.failed_test_line);
  return 0;
}

TEST()
{
  // just an example of empty test
}
#endif

//------------------------------------------------------------------------------
class QoreString* tuxedo_module_init()
{
  tracein("tuxedo_module_init");
  tuxedo_low_level_init();
  tuxedo_fml_init();

#ifdef DEBUG
  builtinFunctions.add("runTuxedoTests", runTuxedoTests, QDOM_NETWORK);
#endif

  traceout("tuxedo_module_init");
  return NULL;
}

//------------------------------------------------------------------------------
void tuxedo_module_ns_init(class Namespace* rns, class Namespace* qns)
{
  tracein("tuxedo_module_ns_init");

  class Namespace* tuxedons = new Namespace("Tuxedo");
  tuxedo_low_level_ns_init(tuxedons);
  tuxedo_fml_ns_init(tuxedons);

  // Tuxedo::TuxedoTypedBuffer class
  tuxedons->addSystemClass(initTuxedoTypedBufferClass());
  // Tuxedo::TuxedoQueueCtl class
  tuxedons->addSystemClass(initTuxedoQueueControlParamsClass());
  // Tuxedo::TuxedoTransactionId class
  tuxedons->addSystemClass(initTuxedoTransactionIdClass());
  // Tuxedo::TuxedoContext class
  tuxedons->addSystemClass(initTuxedoContextClass());

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

