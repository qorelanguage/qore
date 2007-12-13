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

#include <qore/Qore.h>
#include <qore/minitest.hpp>

#include "tuxedo_module.h"
#include <atmi.h>
#include <fml32.h>
#include <fml.h>
#include <tx.h>

#include "QC_TuxedoAdapter.h"

#ifndef QORE_MONOLITHIC
DLLEXPORT char qore_module_name[] = "tuxedo";
DLLEXPORT char qore_module_version[] = "0.1";
DLLEXPORT char qore_module_description[] = "Connection to Tuxedo 9.1 (should be compatible with older versions).";
DLLEXPORT char qore_module_author[] = "Pavel Vozenilek";
DLLEXPORT char qore_module_url[] = "http://qore.sourceforge.net";
DLLEXPORT int qore_module_api_major = QORE_MODULE_API_MAJOR;
DLLEXPORT int qore_module_api_minor = QORE_MODULE_API_MINOR;
DLLEXPORT qore_module_init_t qore_module_init = tuxedo_module_init;
DLLEXPORT qore_module_ns_init_t qore_module_ns_init =tuxedo_module_ns_init;
DLLEXPORT qore_module_delete_t qore_module_delete = tuxedo_module_delete;
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
  
  xsink->raiseException("TUXEDO-TEST-FAILURE", "Tuxedo test in file %s, line %d threw an exception.", 
    res.failed_test_file, res.failed_test_line);
  return 0;
}

TEST()
{
  // just an example of empty test
}
#endif

static void add_constants(Namespace* ns)
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
  ns->addConstant("TPQREPLYQ", new QoreNode((int64)TPQREPLYQ));
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

  // FML/FML32 types
  ns->addConstant("FLD_SHORT", new QoreNode((int64)FLD_SHORT));
  ns->addConstant("FLD_LONG", new QoreNode((int64)FLD_LONG));
  ns->addConstant("FLD_CHAR", new QoreNode((int64)FLD_CHAR));
  ns->addConstant("FLD_FLOAT", new QoreNode((int64)FLD_FLOAT));
  ns->addConstant("FLD_DOUBLE", new QoreNode((int64)FLD_DOUBLE));
  ns->addConstant("FLD_STRING", new QoreNode((int64)FLD_STRING));
  ns->addConstant("FLD_CARRAY", new QoreNode((int64)FLD_CARRAY));
  ns->addConstant("FLD_PTR", new QoreNode((int64)FLD_PTR));
  ns->addConstant("FLD_FML32", new QoreNode((int64)FLD_FML32));
  ns->addConstant("FLD_VIEW32", new QoreNode((int64)FLD_VIEW32));
  ns->addConstant("FLD_MBSTRING", new QoreNode((int64)FLD_MBSTRING));

#if TUXEDO_VERSION_MAJOR > 8
  // XML parsing
  ns->addConstant("TPXPARSNEVER", new QoreNode((int64)TPXPARSNEVER));
  ns->addConstant("TPXPARSALWAYS", new QoreNode((int64)TPXPARSALWAYS));
  ns->addConstant("TPXPARSSCHFULL", new QoreNode((int64)TPXPARSSCHFULL));
  ns->addConstant("TPXPARSCONFATAL", new QoreNode((int64)TPXPARSCONFATAL));
  ns->addConstant("TPXPARSNSPACE", new QoreNode((int64)TPXPARSNSPACE));
  ns->addConstant("TPXPARSDOSCH", new QoreNode((int64)TPXPARSDOSCH));
  ns->addConstant("TPXPARSEREFN", new QoreNode((int64)TPXPARSEREFN));
  ns->addConstant("TPXPARSNOEXIT", new QoreNode((int64)TPXPARSNOEXIT));
  ns->addConstant("TPXPARSNOINCWS", new QoreNode((int64)TPXPARSNOINCWS));
  ns->addConstant("TPXPARSCACHERESET", new QoreNode((int64)TPXPARSCACHERESET));
  ns->addConstant("TPXPARSCACHESET", new QoreNode((int64)TPXPARSCACHESET));
#endif
}

static Namespace* tuxedons;
static void init_namespace()
{
   tuxedons = new Namespace("Tuxedo");

   add_constants(tuxedons);
   tuxedons->addSystemClass(initTuxedoAdapterClass());

#ifdef DEBUG
   tuxedons->addSystemClass(initDummyTestClass());
#endif
}

//------------------------------------------------------------------------------
QoreString* tuxedo_module_init()
{
  tracein("tuxedo_module_init");

  init_namespace();

#ifdef DEBUG
  builtinFunctions.add("runTuxedoTests", runTuxedoTests, QDOM_NETWORK);
#endif

  traceout("tuxedo_module_init");
  return NULL;
}

//------------------------------------------------------------------------------
void tuxedo_module_ns_init(Namespace* rns, Namespace* qns)
{
  tracein("tuxedo_module_ns_init");
  qns->addInitialNamespace(tuxedons->copy());
  traceout("tuxedo_module_ns_init");
}

//------------------------------------------------------------------------------
void tuxedo_module_delete()
{
  tracein("tuxedo_module_delete");
  delete tuxedons;
  traceout("tuxedo_module_delete");
}

// EOF

