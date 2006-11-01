/*
  modules/Tuxedo/QC_TuxedoAdapter.cc

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

#include <qore/common.h>
#include <qore/support.h>
#include <qore/QoreClass.h>
#include <qore/params.h>
#include <qore/minitest.hpp>

#include "QC_TuxedoAdapter.h"
#include "QoreTuxedoAdapter.h"

int CID_TUXEDOADAPTER;

//------------------------------------------------------------------------------
static void getTuxedoAdapter(void* obj)
{
  ((QoreTuxedoAdapter*)obj)->ROreference();
}

static void releaseTuxedoAdapter(void* obj)
{
  ((QoreTuxedoAdapter*)obj)->deref();
}

//------------------------------------------------------------------------------
// No parameters.
static void TUXEDO_constructor(Object *self, QoreNode *params, ExceptionSink *xsink)
{
  tracein("TUXEDO_constructor");

  QoreTuxedoAdapter* adapter = new QoreTuxedoAdapter();
  self->setPrivate(CID_TUXEDOADAPTER, adapter, getTuxedoAdapter, releaseTuxedoAdapter);

  traceout("TUXEDO_constructor");
}

//------------------------------------------------------------------------------
static void TUXEDO_destructor(Object *self, QoreTuxedoAdapter* adapter, ExceptionSink *xsink)
{
  tracein("TUXEDO_destructor");
  adapter->deref();
  traceout("TUXEDO_destructor");
}

//------------------------------------------------------------------------------
static void TUXEDO_copy(Object *self, Object *old, QoreTuxedoAdapter* adapter, ExceptionSink *xsink)
{
  xsink->raiseException("TUXEDO-ADAPTER-COPY", "copying Tuxedo::TuxedoAdapter objects is not yet supported.");
}

//-----------------------------------------------------------------------------
class QoreClass* initTuxedoAdapterClass()
{
  tracein("initTuxedoAdapterClass");
  QoreClass* adapter = new QoreClass(QDOM_NETWORK, strdup("TuxedoAdapter"));
  CID_TUXEDOADAPTER = adapter->getID();  

  adapter->setConstructor((q_constructor_t)TUXEDO_constructor);
  adapter->setDestructor((q_destructor_t)TUXEDO_destructor);
  adapter->setCopy((q_copy_t)TUXEDO_copy);

  traceout("initTuxedoAdapterClass");
  return adapter;
}

//----------------------------------------------------------------------------
#ifdef DEBUG
TEST()
{
  // just test that it could be instantiated
  QoreTuxedoAdapter ad1;
  QoreTuxedoAdapter ad2;
}

TEST()
{
  // test from Qore
  char* cmd =
    "qore -e '%requires tuxedo\n"
    "$a = new Tuxedo::TuxedoAdapter();\n"
    "exit(10);\n'";

  int res = system(cmd);
  res = WEXITSTATUS(res);
  assert(res == 10);

}
#endif

// EOF


