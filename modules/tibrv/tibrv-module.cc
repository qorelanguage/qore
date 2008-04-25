/*
  modules/TIBCO/tibco-module.cc

  TIBCO integration to QORE

  Qore Programming Language

  Copyright 2003 - 2008 David Nichols

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

#include "qore_tibrv.h"
#include "QC_TibrvListener.h"
#include "QC_TibrvSender.h"
#include "QC_TibrvFtMember.h"
#include "QC_TibrvFtMonitor.h"
#include "QC_TibrvCmSender.h"
#include "QC_TibrvCmListener.h"
#include "QC_TibrvDistributedQueue.h"
#include "tibrv-module.h"

#include <tibrv/ftcpp.h>

#include <string.h>

#ifndef QORE_MONOLITHIC
DLLEXPORT char qore_module_name[] = "tibrv";
DLLEXPORT char qore_module_version[] = "0.1";
DLLEXPORT char qore_module_description[] = "TIBCO Rendezvous module";
DLLEXPORT char qore_module_author[] = "David Nichols";
DLLEXPORT char qore_module_url[] = "http://qore.sourceforge.net";
DLLEXPORT int qore_module_api_major = QORE_MODULE_API_MAJOR;
DLLEXPORT int qore_module_api_minor = QORE_MODULE_API_MINOR;
DLLEXPORT qore_module_init_t qore_module_init = tibrv_module_init;
DLLEXPORT qore_module_ns_init_t qore_module_ns_init = tibrv_module_ns_init;
DLLEXPORT qore_module_delete_t qore_module_delete = tibrv_module_delete;
#endif

static QoreNamespace *tibns;

static void init_namespace()
{
   tibns = new QoreNamespace("Tibrv");
   tibns->addSystemClass(initTibrvListenerClass());
   tibns->addSystemClass(initTibrvSenderClass());
   tibns->addSystemClass(initTibrvFtMemberClass());
   tibns->addSystemClass(initTibrvFtMonitorClass());
   tibns->addSystemClass(initTibrvCmSenderClass());
   tibns->addSystemClass(initTibrvCmListenerClass());
   tibns->addSystemClass(initTibrvDistributedQueueClass());

   // add constants for fault tolerant events
   tibns->addConstant("TIBRVFT_PREPARE_TO_ACTIVATE", new QoreBigIntNode(TIBRVFT_PREPARE_TO_ACTIVATE));
   tibns->addConstant("TIBRVFT_ACTIVATE", new QoreBigIntNode(TIBRVFT_ACTIVATE));
   tibns->addConstant("TIBRVFT_DEACTIVATE", new QoreBigIntNode(TIBRVFT_DEACTIVATE));
   tibns->addConstant("TIBRVFT_QORE_STOP", new QoreBigIntNode(-1));
}

class QoreStringNode *tibrv_module_init()
{
   // initialize rendezvous
   TibrvStatus status = Tibrv::open();
   if (status != TIBRV_OK)
   {
      class QoreStringNode *err = new QoreStringNode;
      err->sprintf("cannot initialize TIB/RV library: status=%d: %s\n", (int)status, status.getText());
      return err;
   }
   
   init_namespace();

   init_tibrv_functions();
   return NULL;
}

void tibrv_module_ns_init(class QoreNamespace *rns, class QoreNamespace *qns)
{
   tracein("tibrv_module_ns_init()");
   qns->addInitialNamespace(tibns->copy());
   traceout("tibrv_module_nsinit()");
}

void tibrv_module_delete()
{
   tracein("tibrv_module_delete()");
   Tibrv::close();
   delete tibns;
   traceout("tibrv_module_delete()");
}
