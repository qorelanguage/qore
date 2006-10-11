/*
  modules/Tuxedo/QC_TuxedoQueueControlParams.cc

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

#include "QC_TuxedoQueueControlParams.h"
#include "QoreTuxedoQueueControlParams.h"

int CID_TUXEDOQUEUECONTROLPARAMS;

//------------------------------------------------------------------------------
static void getTuxedoQueueControlParams(void* obj)
{
  ((QoreTuxedoQueueControlParams*)obj)->ROreference();
}

static void releaseTuxedoQueueControlParams(void* obj)
{
  ((QoreTuxedoQueueControlParams*)obj)->deref();
}

//------------------------------------------------------------------------------
// No parameters.
static void TUXEDOQUEUECONTROLPARAMS_constructor(Object *self, QoreNode *params, ExceptionSink *xsink)
{
  tracein("TUXEDOQUEUECONTROLPARAMS_constructor");

  if (get_param(params, 0)) {
    xsink->raiseException("TUXEDO-QUEUE_CONTROL_PARAMS-CONSTRUCTOR", "No parameters are allowed in the constructor.");
    return;
  }

  QoreTuxedoQueueControlParams* ctl = new QoreTuxedoQueueControlParams();
  self->setPrivate(CID_TUXEDOQUEUECONTROLPARAMS, ctl, getTuxedoQueueControlParams, releaseTuxedoQueueControlParams);

  traceout("TUXEDOQUEUECONTROLPARAMS_constructor");
}

//------------------------------------------------------------------------------
static void TUXEDOQUEUECONTROLPARAMS_destructor(Object *self, QoreTuxedoQueueControlParams* ctl, ExceptionSink *xsink)
{
  tracein("TUXEDOQUEUECONTROLPARAMS_destructor");
  ctl->deref();
  traceout("TUXEDOQUEUECONTROLPARAMS_destructor");
}

//------------------------------------------------------------------------------
static void TUXEDOQUEUECONTROLPARAMS_copy(Object *self, Object *old, QoreTuxedoQueueControlParams* ctl, ExceptionSink *xsink)
{
  xsink->raiseException("TUXEDO-QUEUE_CONTROL_PARAMS-COPY", "copying Tuxedo::TuxedoQueueControlParams objects is not yet supported.");
}

//------------------------------------------------------------------------------
class QoreClass* initTuxedoQueueControlParamsClass()
{
  tracein("initTuxedoQueueControlParamsClass");
  QoreClass* ctl = new QoreClass(QDOM_NETWORK, strdup("TuxedoQueueControlParams"));
  CID_TUXEDOQUEUECONTROLPARAMS = ctl->getID();  

  ctl->setConstructor((q_constructor_t)TUXEDOQUEUECONTROLPARAMS_constructor);
  ctl->setDestructor((q_destructor_t)TUXEDOQUEUECONTROLPARAMS_destructor);
  ctl->setCopy((q_copy_t)TUXEDOQUEUECONTROLPARAMS_copy);

  traceout("initTuxedoQueueControlParamsClass");
  return ctl;
}


// EOF


