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

#include "tibae-module.h"
#include "QC_TibcoAdapter.h"

#include <string.h>

static class QoreNamespace *tibns; // Tibae namespace

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
DLLEXPORT qore_license_t qore_module_license = QL_LGPL;
#endif

static class AbstractQoreNode *f_tibae_type(const QoreListNode *params, class ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int type = p->getAsInt();
   if (type < 1 || type > MAX_TIBAE_TYPE)
   {
      xsink->raiseException("TIBAE-TYPE-ERROR", "type %d is out of range (expecting 1 - %d)", type, MAX_TIBAE_TYPE);
      return 0;
   }
   QoreHashNode *h = new QoreHashNode();
   h->setKeyValue("^type^", new QoreBigIntNode(type), xsink);
   p = get_param(params, 1);
   h->setKeyValue("^value^", p ? p->refSelf() : 0, xsink);
   return h;
}

static void setup_namespace()
{
   // setup static "master" namespace
   tibns = new QoreNamespace("Tibae");
   tibns->addSystemClass(initTibcoAdapterClass());

   // add constants
   tibns->addConstant("TIBAE_BINARY",      new QoreBigIntNode(TIBAE_BINARY));
   tibns->addConstant("TIBAE_BOOLEAN",     new QoreBigIntNode(TIBAE_BOOLEAN));
   tibns->addConstant("TIBAE_BYTE",        new QoreBigIntNode(TIBAE_BYTE));
   tibns->addConstant("TIBAE_CHAR",        new QoreBigIntNode(TIBAE_CHAR));
   tibns->addConstant("TIBAE_DATE",        new QoreBigIntNode(TIBAE_DATE));
   tibns->addConstant("TIBAE_DATETIME",    new QoreBigIntNode(TIBAE_DATETIME));
   tibns->addConstant("TIBAE_FIXED",       new QoreBigIntNode(TIBAE_FIXED));
   tibns->addConstant("TIBAE_I1",          new QoreBigIntNode(TIBAE_I1));
   tibns->addConstant("TIBAE_I2",          new QoreBigIntNode(TIBAE_I2));
   tibns->addConstant("TIBAE_I4",          new QoreBigIntNode(TIBAE_I4));
   tibns->addConstant("TIBAE_I8",          new QoreBigIntNode(TIBAE_I8));
   tibns->addConstant("TIBAE_INTERVAL",    new QoreBigIntNode(TIBAE_INTERVAL));
   tibns->addConstant("TIBAE_R4",          new QoreBigIntNode(TIBAE_R4));
   tibns->addConstant("TIBAE_R8",          new QoreBigIntNode(TIBAE_R8));
   tibns->addConstant("TIBAE_STRING",      new QoreBigIntNode(TIBAE_STRING));
   tibns->addConstant("TIBAE_TIME",        new QoreBigIntNode(TIBAE_TIME));
   tibns->addConstant("TIBAE_U1",          new QoreBigIntNode(TIBAE_U1));
   tibns->addConstant("TIBAE_U2",          new QoreBigIntNode(TIBAE_U2));
   tibns->addConstant("TIBAE_U4",          new QoreBigIntNode(TIBAE_U4));
   tibns->addConstant("TIBAE_U8",          new QoreBigIntNode(TIBAE_U8));
}

class QoreStringNode *tibae_module_init()
{
   setup_namespace();

   // add builtin functions
   builtinFunctions.add("tibae_type", f_tibae_type);
   return NULL;
}

void tibae_module_ns_init(class QoreNamespace *rns, class QoreNamespace *qns)
{
   QORE_TRACE("tibae_module_ns_init()");

   qns->addInitialNamespace(tibns->copy());


}

void tibae_module_delete()
{
   QORE_TRACE("tibae_module_delete()");
   delete tibns;

}

