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
QoreNode* runTuxedoTests(const QoreListNode* params, ExceptionSink* xsink)
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

static void add_constants(QoreNamespace* ns)
{
  // Queueing errors and constants
  ns->addConstant("QMEABORTED", new QoreBigIntNode(QMEABORTED));
  ns->addConstant("QMEBADQUEUE", new QoreBigIntNode(QMEBADQUEUE));
  ns->addConstant("QMEBADRMID", new QoreBigIntNode(QMEBADRMID));
  ns->addConstant("QMEBADMSGID", new QoreBigIntNode(QMEBADMSGID));
  ns->addConstant("QMEINUSE", new QoreBigIntNode(QMEINUSE));
  ns->addConstant("QMEINVAL", new QoreBigIntNode(QMEINVAL));
  ns->addConstant("QMENOMSG", new QoreBigIntNode(QMENOMSG));
  ns->addConstant("QMENOSPACE", new QoreBigIntNode(QMENOSPACE));
  ns->addConstant("QMENOTOPEN", new QoreBigIntNode(QMENOTOPEN));
  ns->addConstant("QMEOS", new QoreBigIntNode(QMEOS));
  ns->addConstant("QMEPROTO", new QoreBigIntNode(QMEPROTO));
  ns->addConstant("QMERELEASE", new QoreBigIntNode(QMERELEASE));
  ns->addConstant("QMESHARE", new QoreBigIntNode(QMESHARE));
  ns->addConstant("QMESYSTEM", new QoreBigIntNode(QMESYSTEM));
  ns->addConstant("QMETRAN", new QoreBigIntNode(QMETRAN));

  // ATMI errors and constants
  ns->addConstant("TP_CMT_COMPLETE", new QoreBigIntNode(TP_CMT_COMPLETE));
  ns->addConstant("TP_CMT_LOGGED", new QoreBigIntNode(TP_CMT_LOGGED));
  ns->addConstant("TPEV_DISCONIMM", new QoreBigIntNode(TPEV_DISCONIMM));
  ns->addConstant("TPEV_SVCERR", new QoreBigIntNode(TPEV_SVCERR));
  ns->addConstant("TPEV_SVCFAIL", new QoreBigIntNode(TPEV_SVCFAIL));
  ns->addConstant("TPEV_SVCSUCC", new QoreBigIntNode(TPEV_SVCSUCC));
  ns->addConstant("TPABSOLUTE", new QoreBigIntNode(TPABSOLUTE));
  ns->addConstant("TPAPPAUTH", new QoreBigIntNode(TPAPPAUTH));
  ns->addConstant("TPEABORT", new QoreBigIntNode(TPEABORT));
  ns->addConstant("TPEBADDESC", new QoreBigIntNode(TPEBADDESC));
  ns->addConstant("TPEBLOCK", new QoreBigIntNode(TPEBLOCK));
  ns->addConstant("TPED_CLIENTDISCONNECTED", new QoreBigIntNode(TPED_CLIENTDISCONNECTED));
  ns->addConstant("TPED_DECRYPTION_FAILURE", new QoreBigIntNode(TPED_DECRYPTION_FAILURE));
  ns->addConstant("TPED_DOMAINUNREACHABLE", new QoreBigIntNode(TPED_DOMAINUNREACHABLE));
  ns->addConstant("TPED_INVALID_CERTIFICATE", new QoreBigIntNode(TPED_INVALID_CERTIFICATE));
  ns->addConstant("TPED_INVALID_SIGNATURE", new QoreBigIntNode(TPED_INVALID_SIGNATURE));
  ns->addConstant("TPED_INVALIDCONTEXT", new QoreBigIntNode(TPED_INVALIDCONTEXT));
  ns->addConstant("TPED_INVALID_XA_TRANSACTION", new QoreBigIntNode(TPED_INVALID_XA_TRANSACTION));
  ns->addConstant("TPED_NOCLIENT", new QoreBigIntNode(TPED_NOCLIENT));
  ns->addConstant("TPED_NOUNSOLHANDLER", new QoreBigIntNode(TPED_NOUNSOLHANDLER));
  ns->addConstant("TPED_SVCTIMEOUT", new QoreBigIntNode(TPED_SVCTIMEOUT));
  ns->addConstant("TPED_TERM", new QoreBigIntNode(TPED_TERM));
  ns->addConstant("TPEDIAGNOSTIC", new QoreBigIntNode(TPEDIAGNOSTIC));
  ns->addConstant("TPEVENT", new QoreBigIntNode(TPEEVENT));
  ns->addConstant("TPEHAZARD", new QoreBigIntNode(TPEHAZARD));
  ns->addConstant("TPEHEURISTIC", new QoreBigIntNode(TPEHEURISTIC));
  ns->addConstant("TPEINVAL", new QoreBigIntNode(TPEINVAL));
  ns->addConstant("TPEITYPE", new QoreBigIntNode(TPEITYPE));
  ns->addConstant("TPELIMIT", new QoreBigIntNode(TPELIMIT));
  ns->addConstant("TPENOENT", new QoreBigIntNode(TPENOENT));
  ns->addConstant("TPEOS", new QoreBigIntNode(TPEOS));
  ns->addConstant("TPEOTYPE", new QoreBigIntNode(TPEOTYPE));
  ns->addConstant("TPEPERM", new QoreBigIntNode(TPEPERM));
  ns->addConstant("TPEPROTO", new QoreBigIntNode(TPEPROTO));
  ns->addConstant("TPERMERR", new QoreBigIntNode(TPERMERR));
  ns->addConstant("TPESVCFAIL", new QoreBigIntNode(TPESVCFAIL));
  ns->addConstant("TPESVCERR", new QoreBigIntNode(TPESVCERR));
  ns->addConstant("TPESYSTEM", new QoreBigIntNode(TPESYSTEM));
  ns->addConstant("TPETIME", new QoreBigIntNode(TPETIME));
  ns->addConstant("TPETRAN", new QoreBigIntNode(TPETRAN));
  ns->addConstant("TPGETANY", new QoreBigIntNode(TPGETANY));
  ns->addConstant("TPGOTSIG", new QoreBigIntNode(TPGOTSIG));
  ns->addConstant("TPMULTICONTEXTS", new QoreBigIntNode(TPMULTICONTEXTS));
  ns->addConstant("TPNOAUTH", new QoreBigIntNode(TPNOAUTH));
  ns->addConstant("TPNOBLOCK", new QoreBigIntNode(TPNOCHANGE));
  ns->addConstant("TPNOCHANGE", new QoreBigIntNode(TPNOCHANGE));
  ns->addConstant("TPNOFLAGS", new QoreBigIntNode(TPNOFLAGS));
  ns->addConstant("TPNOREPLY", new QoreBigIntNode(TPNOREPLY));
  ns->addConstant("TPNOTIME", new QoreBigIntNode(TPNOTIME));
  ns->addConstant("TPNOTRAN", new QoreBigIntNode(TPNOTRAN));
  ns->addConstant("TPQBEFOREMSGID", new QoreBigIntNode(TPQBEFOREMSGID));
  ns->addConstant("TPQCORRID", new QoreBigIntNode(TPQCORRID));
  ns->addConstant("TPQDELIVERYQOS", new QoreBigIntNode(TPQDELIVERYQOS));
  ns->addConstant("TPEX_STRING", new QoreBigIntNode(TPEX_STRING));
  ns->addConstant("TPQEXPTIME_ABS", new QoreBigIntNode(TPQEXPTIME_ABS));
  ns->addConstant("TPQEXPTIME_NONE", new QoreBigIntNode(TPQEXPTIME_NONE));
  ns->addConstant("TPQEXPTIME_REL", new QoreBigIntNode(TPQEXPTIME_REL));
  ns->addConstant("TPQFAILUREQ", new QoreBigIntNode(TPQFAILUREQ));
  ns->addConstant("TPQGETBYCORRID", new QoreBigIntNode(TPQGETBYCORRID));
  ns->addConstant("TPQGETBYMSGID", new QoreBigIntNode(TPQGETBYMSGID));
  ns->addConstant("TPQMSGID", new QoreBigIntNode(TPQMSGID));
  ns->addConstant("TPQPEEK", new QoreBigIntNode(TPQPEEK));
  ns->addConstant("TPQPRIORITY", new QoreBigIntNode(TPQPRIORITY));
  ns->addConstant("TPQQOSDEFAULTPERSIST", new QoreBigIntNode(TPQQOSDEFAULTPERSIST));
  ns->addConstant("TPQQOSNONPERSISTENT", new QoreBigIntNode(TPQQOSNONPERSISTENT));
  ns->addConstant("TPQQOSPERSISTENT", new QoreBigIntNode(TPQQOSPERSISTENT));
  ns->addConstant("TPQREPLYQ", new QoreBigIntNode(TPQREPLYQ));
  ns->addConstant("TPQREPLYQOS", new QoreBigIntNode(TPQREPLYQOS));
  ns->addConstant("TPQTIME_ABS", new QoreBigIntNode(TPQTIME_ABS));
  ns->addConstant("TPQTIME_REL", new QoreBigIntNode(TPQTIME_REL));
  ns->addConstant("TPQTOP", new QoreBigIntNode(TPQTOP));
  ns->addConstant("TPQWAIT", new QoreBigIntNode(TPQWAIT));
  ns->addConstant("TPRECVONLY", new QoreBigIntNode(TPRECVONLY));
  ns->addConstant("TPSA_FASTPATH", new QoreBigIntNode(TPSA_FASTPATH));
  ns->addConstant("TPSA_PROTECTED", new QoreBigIntNode(TPSA_PROTECTED));
  ns->addConstant("TPSENDONLY", new QoreBigIntNode(TPSENDONLY));
  ns->addConstant("TPSIGRSTRT", new QoreBigIntNode(TPSIGRSTRT));
  ns->addConstant("TPSYSAUTH", new QoreBigIntNode(TPSYSAUTH));
  ns->addConstant("TPU_DIP", new QoreBigIntNode(TPU_DIP));
  ns->addConstant("TPU_IGN", new QoreBigIntNode(TPU_IGN));
  ns->addConstant("TPU_SIG", new QoreBigIntNode(TPU_SIG));
  ns->addConstant("TPU_THREAD", new QoreBigIntNode(TPU_THREAD));

  // X/Open errors and constants
  ns->addConstant("TX_CHAINED", new QoreBigIntNode(TX_CHAINED));
  ns->addConstant("TX_COMMITTED", new QoreBigIntNode(TX_COMMITTED));
  ns->addConstant("TX_COMMITTED_NO_BEGIN", new QoreBigIntNode(TX_COMMITTED_NO_BEGIN));
  ns->addConstant("TX_COMMIT_COMPLETED", new QoreBigIntNode(TX_COMMIT_COMPLETED));
  ns->addConstant("TX_COMMIT_DECISION_LOGGED", new QoreBigIntNode(TX_COMMIT_DECISION_LOGGED));
  ns->addConstant("TX_EINVAL", new QoreBigIntNode(TX_EINVAL));
  ns->addConstant("TX_ERROR", new QoreBigIntNode(TX_ERROR));
  ns->addConstant("TX_FAIL", new QoreBigIntNode(TX_FAIL));
  ns->addConstant("TX_HAZARD", new QoreBigIntNode(TX_HAZARD));
  ns->addConstant("TX_HAZARD_NO_BEGIN", new QoreBigIntNode(TX_HAZARD_NO_BEGIN));
  ns->addConstant("TX_MIXED", new QoreBigIntNode(TX_MIXED));
  ns->addConstant("TX_MIXED_NO_BEGIN", new QoreBigIntNode(TX_MIXED_NO_BEGIN));
  ns->addConstant("TX_NO_BEGIN", new QoreBigIntNode(TX_NO_BEGIN));
  ns->addConstant("TX_OK", new QoreBigIntNode(TX_OK));
  ns->addConstant("TX_OUTSIDE", new QoreBigIntNode(TX_OUTSIDE));
  ns->addConstant("TX_PROTOCOL_ERROR", new QoreBigIntNode(TX_PROTOCOL_ERROR));
  ns->addConstant("TX_ROLLBACK", new QoreBigIntNode(TX_ROLLBACK));
  ns->addConstant("TX_ROLLBACK_NO_BEGIN", new QoreBigIntNode(TX_ROLLBACK_NO_BEGIN));
  ns->addConstant("TX_UNCHAINED", new QoreBigIntNode(TX_UNCHAINED));

  // FML/FML32 types
  ns->addConstant("FLD_SHORT", new QoreBigIntNode(FLD_SHORT));
  ns->addConstant("FLD_LONG", new QoreBigIntNode(FLD_LONG));
  ns->addConstant("FLD_CHAR", new QoreBigIntNode(FLD_CHAR));
  ns->addConstant("FLD_FLOAT", new QoreBigIntNode(FLD_FLOAT));
  ns->addConstant("FLD_DOUBLE", new QoreBigIntNode(FLD_DOUBLE));
  ns->addConstant("FLD_STRING", new QoreBigIntNode(FLD_STRING));
  ns->addConstant("FLD_CARRAY", new QoreBigIntNode(FLD_CARRAY));
  ns->addConstant("FLD_PTR", new QoreBigIntNode(FLD_PTR));
  ns->addConstant("FLD_FML32", new QoreBigIntNode(FLD_FML32));
  ns->addConstant("FLD_VIEW32", new QoreBigIntNode(FLD_VIEW32));
  ns->addConstant("FLD_MBSTRING", new QoreBigIntNode(FLD_MBSTRING));

#if TUXEDO_VERSION_MAJOR > 8
  // XML parsing
  ns->addConstant("TPXPARSNEVER", new QoreBigIntNode(TPXPARSNEVER));
  ns->addConstant("TPXPARSALWAYS", new QoreBigIntNode(TPXPARSALWAYS));
  ns->addConstant("TPXPARSSCHFULL", new QoreBigIntNode(TPXPARSSCHFULL));
  ns->addConstant("TPXPARSCONFATAL", new QoreBigIntNode(TPXPARSCONFATAL));
  ns->addConstant("TPXPARSNSPACE", new QoreBigIntNode(TPXPARSNSPACE));
  ns->addConstant("TPXPARSDOSCH", new QoreBigIntNode(TPXPARSDOSCH));
  ns->addConstant("TPXPARSEREFN", new QoreBigIntNode(TPXPARSEREFN));
  ns->addConstant("TPXPARSNOEXIT", new QoreBigIntNode(TPXPARSNOEXIT));
  ns->addConstant("TPXPARSNOINCWS", new QoreBigIntNode(TPXPARSNOINCWS));
  ns->addConstant("TPXPARSCACHERESET", new QoreBigIntNode(TPXPARSCACHERESET));
  ns->addConstant("TPXPARSCACHESET", new QoreBigIntNode(TPXPARSCACHESET));
#endif
}

static QoreNamespace* tuxedons;
static void init_namespace()
{
   tuxedons = new QoreNamespace("Tuxedo");

   add_constants(tuxedons);
   tuxedons->addSystemClass(initTuxedoAdapterClass());

#ifdef DEBUG
   tuxedons->addSystemClass(initDummyTestClass());
#endif
}

//------------------------------------------------------------------------------
QoreStringNode* tuxedo_module_init()
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
void tuxedo_module_ns_init(QoreNamespace* rns, QoreNamespace* qns)
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

