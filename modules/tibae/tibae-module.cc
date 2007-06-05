/*
  modules/TIBCO/tibco-module.cc

  TIBCO integration to QORE

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols

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
#include <qore/ModuleManager.h>

#include "tibae-module.h"
#include "QC_TibcoAdapter.h"

#include <string.h>

#ifndef QORE_MONOLITHIC
DLLEXPORT char qore_module_name[] = "tibae";
DLLEXPORT char qore_module_version[] = "0.2";
DLLEXPORT char qore_module_description[] = "TIBCO Active Enterprise module";
DLLEXPORT char qore_module_author[] = "David Nichols";
DLLEXPORT char qore_module_url[] = "http://qore.sourceforge.net";
DLLEXPORT int qore_module_api_major = QORE_MODULE_API_MAJOR;
DLLEXPORT int qore_module_api_minor = QORE_MODULE_API_MINOR;
DLLEXPORT qore_module_init_t qore_module_init = tibae_module_init;
DLLEXPORT qore_module_ns_init_t qore_module_ns_init = tibae_module_ns_init;
DLLEXPORT qore_module_delete_t qore_module_delete = tibae_module_delete;
#endif

static class QoreNode *f_tibae_type(class QoreNode *params, class ExceptionSink *xsink)
{
   class QoreNode *p = get_param(params, 0);
   int type = p->getAsInt();
   if (type < 1 || type > MAX_TIBAE_TYPE)
   {
      xsink->raiseException("TIBAE-TYPE-ERROR", "type %d is out of range (expecting 1 - %d)", type, MAX_TIBAE_TYPE);
      return 0;
   }
   class Hash *h = new Hash();
   h->setKeyValue("^type^", new QoreNode((int64)type), xsink);
   p = get_param(params, 1);
   if (p)
      p->ref();
   h->setKeyValue("^value^", p, xsink);
   return new QoreNode(h);
}

class QoreString *tibae_module_init()
{
   builtinFunctions.add("tibae_type", f_tibae_type);
   return NULL;
}

void tibae_module_ns_init(class Namespace *rns, class Namespace *qns)
{
   tracein("tibae_module_ns_init()");
   class Namespace *tibns = new Namespace("Tibae");
   tibns->addSystemClass(initTibcoAdapterClass());

   // add constants
   tibns->addConstant("TIBAE_BINARY",      new QoreNode((int64)TIBAE_BINARY));
   tibns->addConstant("TIBAE_BOOLEAN",     new QoreNode((int64)TIBAE_BOOLEAN));
   tibns->addConstant("TIBAE_BYTE",        new QoreNode((int64)TIBAE_BYTE));
   tibns->addConstant("TIBAE_CHAR",        new QoreNode((int64)TIBAE_CHAR));
   tibns->addConstant("TIBAE_DATE",        new QoreNode((int64)TIBAE_DATE));
   tibns->addConstant("TIBAE_DATETIME",    new QoreNode((int64)TIBAE_DATETIME));
   tibns->addConstant("TIBAE_FIXED",       new QoreNode((int64)TIBAE_FIXED));
   tibns->addConstant("TIBAE_I1",          new QoreNode((int64)TIBAE_I1));
   tibns->addConstant("TIBAE_I2",          new QoreNode((int64)TIBAE_I2));
   tibns->addConstant("TIBAE_I4",          new QoreNode((int64)TIBAE_I4));
   tibns->addConstant("TIBAE_I8",          new QoreNode((int64)TIBAE_I8));
   tibns->addConstant("TIBAE_INTERVAL",    new QoreNode((int64)TIBAE_INTERVAL));
   tibns->addConstant("TIBAE_R4",          new QoreNode((int64)TIBAE_R4));
   tibns->addConstant("TIBAE_R8",          new QoreNode((int64)TIBAE_R8));
   tibns->addConstant("TIBAE_STRING",      new QoreNode((int64)TIBAE_STRING));
   tibns->addConstant("TIBAE_TIME",        new QoreNode((int64)TIBAE_TIME));
   tibns->addConstant("TIBAE_U1",          new QoreNode((int64)TIBAE_U1));
   tibns->addConstant("TIBAE_U2",          new QoreNode((int64)TIBAE_U2));
   tibns->addConstant("TIBAE_U4",          new QoreNode((int64)TIBAE_U4));
   tibns->addConstant("TIBAE_U8",          new QoreNode((int64)TIBAE_U8));

   qns->addInitialNamespace(tibns);

   traceout("tibae_module_nsinit()");
}

void tibae_module_delete()
{
   tracein("tibae_module_delete()");
   traceout("tibae_module_delete()");
}

