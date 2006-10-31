/*
  modules/Tuxedo/QC_TuxedoQueueTransactionId.cc

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

#include "QC_TuxedoTransactionId.h"
#include "QoreTuxedoTransactionId.h"

int CID_TUXEDOTRANSACTIONID;

//------------------------------------------------------------------------------
static void getTuxedoTransactionId(void* obj)
{
  ((QoreTuxedoTransactionId*)obj)->ROreference();
}

static void releaseTuxedoTransactionId(void* obj)
{
  ((QoreTuxedoTransactionId*)obj)->deref();
}

//------------------------------------------------------------------------------
// No parameters.
static void TUXEDOTRID_constructor(Object *self, QoreNode *params, ExceptionSink *xsink)
{
  tracein("TUXEDOTRID_constructor");

  if (get_param(params, 0)) {
    xsink->raiseException("TUXEDO-QUEUE_TRANSACTION_ID-CONSTRUCTOR", "No parameters are allowed in the constructor.");
    return;
  }

  QoreTuxedoTransactionId* trid = new QoreTuxedoTransactionId();
  self->setPrivate(CID_TUXEDOTRANSACTIONID, trid, getTuxedoTransactionId, releaseTuxedoTransactionId);

  traceout("TUXEDOTRID_constructor");
}

//------------------------------------------------------------------------------
static void TUXEDOTRID_destructor(Object *self, QoreTuxedoTransactionId* trid, ExceptionSink *xsink)
{
  tracein("TUXEDOTRID_destructor");
  trid->deref();
  traceout("TUXEDOTRID_destructor");
}

//------------------------------------------------------------------------------
static void TUXEDOTRID_copy(Object *self, Object *old, QoreTuxedoTransactionId* ctl, ExceptionSink *xsink)
{
  xsink->raiseException("TUXEDO-TRANSACTION_ID-COPY", "copying Tuxedo::TuxedoTransactionId objects is not yet supported.");
}

//-----------------------------------------------------------------------------
class QoreClass* initTuxedoTransactionIdClass()
{
  tracein("initTuxedoTransactionIdClass");
  QoreClass* trid = new QoreClass(QDOM_NETWORK, strdup("TuxedoTransactionId"));
  CID_TUXEDOTRANSACTIONID = trid->getID();  

  trid->setConstructor((q_constructor_t)TUXEDOTRID_constructor);
  trid->setDestructor((q_destructor_t)TUXEDOTRID_destructor);
  trid->setCopy((q_copy_t)TUXEDOTRID_copy);

  traceout("initTuxedoTransactionIdClass");
  return trid;
}

//----------------------------------------------------------------------------
#ifdef DEBUG
TEST()
{
  // just test that it could be instantiated
  QoreTuxedoTransactionId id1;
  QoreTuxedoTransactionId id2;
}

TEST()
{
  // test from Qore
  char* cmd =
    "qore -e '%requires tuxedo\n"
    "$a = new Tuxedo::TuxedoTransactionId();\n"
    "exit(10);\n'";

  int res = system(cmd);
  res = WEXITSTATUS(res);
  assert(res == 10);

}
#endif

// EOF


