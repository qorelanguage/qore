/*
  modules/TIBCO/tibco-module.cc

  TIBCO integration to QORE

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006 David Nichols

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
#include <qore/common.h>
#include <qore/support.h>
#include <qore/Namespace.h>
#include <qore/module.h>
#include <qore/ModuleManager.h>

#include "QC_TibrvListener.h"
#include "tibco-module.h"
#include "tibco.h"

#include <string.h>

#ifndef QORE_MONOLITHIC
char qore_module_name[] = "tibco";
char qore_module_version[] = "0.1";
char qore_module_description[] = "TIBCO integration module";
char qore_module_author[] = "David Nichols";
char qore_module_url[] = "http://qore.sourceforge.net";
int qore_module_api_major = QORE_MODULE_API_MAJOR;
int qore_module_api_minor = QORE_MODULE_API_MINOR;
qore_module_init_t qore_module_init = tibco_module_init;
qore_module_ns_init_t qore_module_ns_init = tibco_module_ns_init;
qore_module_delete_t qore_module_delete = tibco_module_delete;
#endif

int tibco_module_init()
{
   // initialize rendezvous
   TibrvStatus status = Tibrv::open();
   if (status != TIBRV_OK)
   {
       fprintf(stderr, "ERROR: cannot open TIB/RV: status=%d: %s\n", (int)status, status.getText());
       return 1;
   }
   return 0;
}

void tibco_module_ns_init(class Namespace *rns, class Namespace *qns)
{
   tracein("tibco_module_ns_init()");
   class Namespace *tibns = new Namespace("TIBCO");
   tibns->addSystemClass(initTibcoAdapterClass());
   tibns->addSystemClass(initTibrvListenerClass());
   qns->addInitialNamespace(tibns);

   traceout("tibco_module_nsinit()");
}

void tibco_module_delete()
{
   tracein("tibco_module_delete()");
   Tibrv::close();
   traceout("tibco_module_delete()");
}

