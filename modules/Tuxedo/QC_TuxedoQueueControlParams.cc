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
static void TUXEDOQCTL_constructor(Object *self, QoreNode *params, ExceptionSink *xsink)
{
  tracein("TUXEDOQCTL_constructor");

  if (get_param(params, 0)) {
    xsink->raiseException("TUXEDO-QUEUE_CONTROL_PARAMS-CONSTRUCTOR", "No parameters are allowed in the constructor.");
    return;
  }

  QoreTuxedoQueueControlParams* ctl = new QoreTuxedoQueueControlParams();
  self->setPrivate(CID_TUXEDOQUEUECONTROLPARAMS, ctl, getTuxedoQueueControlParams, releaseTuxedoQueueControlParams);

  traceout("TUXEDOQCTL_constructor");
}

//------------------------------------------------------------------------------
static void TUXEDOQCTL_destructor(Object *self, QoreTuxedoQueueControlParams* ctl, ExceptionSink *xsink)
{
  tracein("TUXEDOQCTL_destructor");
  ctl->deref();
  traceout("TUXEDOQCTL_destructor");
}

//------------------------------------------------------------------------------
static void TUXEDOQCTL_copy(Object *self, Object *old, QoreTuxedoQueueControlParams* ctl, ExceptionSink *xsink)
{
  xsink->raiseException("TUXEDO-QUEUE_CONTROL_PARAMS-COPY", "copying Tuxedo::TuxedoQueueControlParams objects is not yet supported.");
}

//------------------------------------------------------------------------------
static QoreNode* TUXEDOQCTL_setFlags(Object* self, QoreTuxedoQueueControlParams* ctl, QoreNode* params, ExceptionSink* xsink)
{
  for (int i = 0; i <= 1; ++i) {
    bool ok;
    if (i == 1) ok = !get_param(params, i);
    else ok = get_param(params, i);
    if (!ok) {
      xsink->raiseException("TuxedoQueueCtl::setFlags", "One parameter (flags) is required.");
      return 0;
    }
  }
  
  QoreNode* n = test_param(params, NT_INT, 0);
  if (!n) {
    xsink->raiseException("TuxedoQueueCtl::setFlags", "The parameter (flags) needs to be an integer.");
    return 0;
  }
  long flags = (long)n->val.intval;
  
  ctl->ctl.flags = flags;
  return 0;
}

//------------------------------------------------------------------------------
static QoreNode* TUXEDOQCTL_getFlags(Object* self, QoreTuxedoQueueControlParams* ctl, QoreNode* params, ExceptionSink* xsink)
{
  if (get_param(params, 0)) {
    xsink->raiseException("TuxedoQueueCtl::getFlags", "No parameter expected.");
    return 0;
  }

  return new QoreNode((int64)ctl->ctl.flags);
}

//------------------------------------------------------------------------------
static QoreNode* TUXEDOQCTL_setDeqTime(Object* self, QoreTuxedoQueueControlParams* ctl, QoreNode* params, ExceptionSink* xsink)
{
  for (int i = 0; i <= 1; ++i) {
    bool ok;
    if (i == 1) ok = !get_param(params, i);
    else ok = get_param(params, i);
    if (!ok) {
      xsink->raiseException("TuxedoQueueCtl::setDeqTime", "One parameter (deq_time) is required.");
      return 0;
    }
  }

  QoreNode* n = test_param(params, NT_INT, 0);
  if (!n) {
    xsink->raiseException("TuxedoQueueCtl::setDeqTime", "The parameter (deq_time) needs to be an integer.");
    return 0;
  }
  long deq_time = (long)n->val.intval;

  ctl->ctl.deq_time = deq_time;
  return 0;
}

//------------------------------------------------------------------------------
static QoreNode* TUXEDOQCTL_getDeqTime(Object* self, QoreTuxedoQueueControlParams* ctl, QoreNode* params, ExceptionSink* xsink)
{
  if (get_param(params, 0)) {
    xsink->raiseException("TuxedoQueueCtl::getDeqTime", "No parameter expected.");
    return 0;
  }

  return new QoreNode((int64)ctl->ctl.deq_time);
}

//------------------------------------------------------------------------------
static QoreNode* TUXEDOQCTL_setPriority(Object* self, QoreTuxedoQueueControlParams* ctl, QoreNode* params, ExceptionSink* xsink)
{
  for (int i = 0; i <= 1; ++i) {
    bool ok;
    if (i == 1) ok = !get_param(params, i);
    else ok = get_param(params, i);
    if (!ok) {
      xsink->raiseException("TuxedoQueueCtl::setPriority", "One parameter (priority) is required.");
      return 0;
    }
  }

  QoreNode* n = test_param(params, NT_INT, 0);
  if (!n) {
    xsink->raiseException("TuxedoQueueCtl::setPriority", "The parameter (priority) needs to be an integer.");
    return 0;
  }
  long priority = (long)n->val.intval;

  ctl->ctl.priority = priority;
  return 0;
}

//------------------------------------------------------------------------------
static QoreNode* TUXEDOQCTL_getPriority(Object* self, QoreTuxedoQueueControlParams* ctl, QoreNode* params, ExceptionSink* xsink)
{
  if (get_param(params, 0)) {
    xsink->raiseException("TuxedoQueueCtl::getPriority", "No parameter expected.");
    return 0;
  }

  return new QoreNode((int64)ctl->ctl.priority);
}

//------------------------------------------------------------------------------
static QoreNode* TUXEDOQCTL_setDiagnostic(Object* self, QoreTuxedoQueueControlParams* ctl, QoreNode* params, ExceptionSink* xsink) 
{
  for (int i = 0; i <= 1; ++i) {
    bool ok;
    if (i == 1) ok = !get_param(params, i);
    else ok = get_param(params, i);
    if (!ok) {
      xsink->raiseException("TuxedoQueueCtl::setDiagnostic", "One parameter (diagnostic) is required.");
      return 0;
    }
  }

  QoreNode* n = test_param(params, NT_INT, 0);
  if (!n) {
    xsink->raiseException("TuxedoQueueCtl::setDiagnostic", "The parameter (diagnostic) needs to be an integer.");
    return 0;
  }
  long diagnostic = (long)n->val.intval;

  ctl->ctl.diagnostic = diagnostic;
  return 0;
}

//------------------------------------------------------------------------------
static QoreNode* TUXEDOQCTL_getDiagnostic(Object* self, QoreTuxedoQueueControlParams* ctl, QoreNode* params, ExceptionSink* xsink)
{
  if (get_param(params, 0)) {
    xsink->raiseException("TuxedoQueueCtl::getDiagnostic", "No parameter expected.");
    return 0;
  }

  return new QoreNode((int64)ctl->ctl.diagnostic);
}

//------------------------------------------------------------------------------
static QoreNode* TUXEDOQCTL_setExpTime(Object* self, QoreTuxedoQueueControlParams* ctl, QoreNode* params, ExceptionSink* xsink)
{
  for (int i = 0; i <= 1; ++i) {
    bool ok;
    if (i == 1) ok = !get_param(params, i);
    else ok = get_param(params, i);
    if (!ok) {
      xsink->raiseException("TuxedoQueueCtl::setExpTime", "One parameter (expiration time) is required.");
      return 0;
    }
  }

  QoreNode* n = test_param(params, NT_INT, 0);
  if (!n) {
    xsink->raiseException("TuxedoQueueCtl::setExpTime", "The parameter (expiration time) needs to be an integer.");
    return 0;
  }
  long exp_time = (long)n->val.intval;

  ctl->ctl.exp_time = exp_time;
  return 0;
}

//------------------------------------------------------------------------------
static QoreNode* TUXEDOQCTL_getExpTime(Object* self, QoreTuxedoQueueControlParams* ctl, QoreNode* params, ExceptionSink* xsink)
{
  if (get_param(params, 0)) {
    xsink->raiseException("TuxedoQueueCtl::getExpTime", "No parameter expected.");
    return 0;
  }

  return new QoreNode((int64)ctl->ctl.exp_time);
}

//------------------------------------------------------------------------------
static QoreNode* TUXEDOQCTL_setReplyQOS(Object* self, QoreTuxedoQueueControlParams* ctl, QoreNode* params, ExceptionSink* xsink)
{
  for (int i = 0; i <= 1; ++i) {
    bool ok;
    if (i == 1) ok = !get_param(params, i);
    else ok = get_param(params, i);
    if (!ok) {
      xsink->raiseException("TuxedoQueueCtl::setReplyQOS", "One parameter (reply quality of service) is required.");
      return 0;
    }
  }

  QoreNode* n = test_param(params, NT_INT, 0);
  if (!n) {
    xsink->raiseException("TuxedoQueueCtl::setReplyQOS", "The parameter (reply quality of service) needs to be an integer.");
    return 0;
  }
  long reply_qos = (long)n->val.intval;

  ctl->ctl.reply_qos = reply_qos;
  return 0;
}

//------------------------------------------------------------------------------
static QoreNode* TUXEDOQCTL_getReplyQOS(Object* self, QoreTuxedoQueueControlParams* ctl, QoreNode* params, ExceptionSink* xsink)
{
  if (get_param(params, 0)) {
    xsink->raiseException("TuxedoQueueCtl::getReplyQOS", "No parameter expected.");
    return 0;
  }

  return new QoreNode((int64)ctl->ctl.reply_qos);
}

//------------------------------------------------------------------------------
static QoreNode* TUXEDOQCTL_setDeliveryQOS(Object* self, QoreTuxedoQueueControlParams* ctl, QoreNode* params, ExceptionSink* xsink)
{
  for (int i = 0; i <= 1; ++i) {
    bool ok;
    if (i == 1) ok = !get_param(params, i);
    else ok = get_param(params, i);
    if (!ok) {
      xsink->raiseException("TuxedoQueueCtl::setDeliveryQOS", "One parameter (delivery quality of service) is required.");
      return 0;
    }
  }

  QoreNode* n = test_param(params, NT_INT, 0);
  if (!n) {
    xsink->raiseException("TuxedoQueueCtl::setDeliveryQOS", "The parameter (delivery quality of service) needs to be an integer.");
    return 0;
  }
  long delivery_qos = (long)n->val.intval;

  ctl->ctl.delivery_qos = delivery_qos;
  return 0;
}

//------------------------------------------------------------------------------
static QoreNode* TUXEDOQCTL_getDeliveryQOS(Object* self, QoreTuxedoQueueControlParams* ctl, QoreNode* params, ExceptionSink* xsink)
{
  if (get_param(params, 0)) {
    xsink->raiseException("TuxedoQueueCtl::getDeliveryQOS", "No parameter expected.");
    return 0;
  }

  return new QoreNode((int64)ctl->ctl.delivery_qos);
}

//------------------------------------------------------------------------------
static QoreNode* TUXEDOQCTL_setAppkey(Object* self, QoreTuxedoQueueControlParams* ctl, QoreNode* params, ExceptionSink* xsink) 
{
  for (int i = 0; i <= 1; ++i) {
    bool ok;
    if (i == 1) ok = !get_param(params, i);
    else ok = get_param(params, i);
    if (!ok) {
      xsink->raiseException("TuxedoQueueCtl::setAppkey", "One parameter (appkey) is required.");
      return 0;
    }
  }

  QoreNode* n = test_param(params, NT_INT, 0);
  if (!n) {
    xsink->raiseException("TuxedoQueueCtl::setAppkey", "The parameter (appkey) needs to be an integer.");
    return 0;
  }
  long appkey = (long)n->val.intval;

  ctl->ctl.appkey = appkey;
  return 0;
}

//------------------------------------------------------------------------------
static QoreNode* TUXEDOQCTL_getAppkey(Object* self, QoreTuxedoQueueControlParams* ctl, QoreNode* params, ExceptionSink* xsink)
{
  if (get_param(params, 0)) {
    xsink->raiseException("TuxedoQueueCtl::getAppkey", "No parameter expected.");
    return 0;
  }

  return new QoreNode((int64)ctl->ctl.appkey);
}

//------------------------------------------------------------------------------
static QoreNode* TUXEDOQCTL_setUrcode(Object* self, QoreTuxedoQueueControlParams* ctl, QoreNode* params, ExceptionSink* xsink
)
{
  for (int i = 0; i <= 1; ++i) {
    bool ok;
    if (i == 1) ok = !get_param(params, i);
    else ok = get_param(params, i);
    if (!ok) {
      xsink->raiseException("TuxedoQueueCtl::setUrcode", "One parameter (return user code) is required.");
      return 0;
    }
  }

  QoreNode* n = test_param(params, NT_INT, 0);
  if (!n) {
    xsink->raiseException("TuxedoQueueCtl::setUrcode", "The parameter (return user code) needs to be an integer.");
    return 0;
  }
  long urcode = (long)n->val.intval;

  ctl->ctl.urcode = urcode;
  return 0;
}

//------------------------------------------------------------------------------
static QoreNode* TUXEDOQCTL_getUrcode(Object* self, QoreTuxedoQueueControlParams* ctl, QoreNode* params, ExceptionSink* xsink
)
{
  if (get_param(params, 0)) {
    xsink->raiseException("TuxedoQueueCtl::getUrcode", "No parameter expected.");
    return 0;
  }

  return new QoreNode((int64)ctl->ctl.urcode);
}

//------------------------------------------------------------------------------
class QoreClass* initTuxedoQueueControlParamsClass()
{
  tracein("initTuxedoQueueControlParamsClass");
  QoreClass* ctl = new QoreClass(QDOM_NETWORK, strdup("TuxedoQueueCtl"));
  CID_TUXEDOQUEUECONTROLPARAMS = ctl->getID();  

  ctl->setConstructor((q_constructor_t)TUXEDOQCTL_constructor);
  ctl->setDestructor((q_destructor_t)TUXEDOQCTL_destructor);
  ctl->setCopy((q_copy_t)TUXEDOQCTL_copy);

  // all getters and setters
  ctl->addMethod("getFlags", (q_method_t)TUXEDOQCTL_getFlags);
  ctl->addMethod("setFlags", (q_method_t)TUXEDOQCTL_setFlags);
  ctl->addMethod("getDeqTime", (q_method_t)TUXEDOQCTL_getDeqTime);
  ctl->addMethod("setDeqTime", (q_method_t)TUXEDOQCTL_setDeqTime);
  ctl->addMethod("getPriority", (q_method_t)TUXEDOQCTL_getPriority);
  ctl->addMethod("setPriority", (q_method_t)TUXEDOQCTL_setPriority);
  ctl->addMethod("getDiagnostic", (q_method_t)TUXEDOQCTL_getDiagnostic);
  ctl->addMethod("setDiagnostic", (q_method_t)TUXEDOQCTL_setDiagnostic);
  ctl->addMethod("getExpTime", (q_method_t)TUXEDOQCTL_getExpTime);
  ctl->addMethod("setExpTime", (q_method_t)TUXEDOQCTL_setExpTime);
  ctl->addMethod("getReplyQOS", (q_method_t)TUXEDOQCTL_getReplyQOS);
  ctl->addMethod("setReplyQOS", (q_method_t)TUXEDOQCTL_setReplyQOS);
  ctl->addMethod("getDeliveryQOS", (q_method_t)TUXEDOQCTL_getDeliveryQOS);
  ctl->addMethod("setDeliveryQOS", (q_method_t)TUXEDOQCTL_setDeliveryQOS);
  ctl->addMethod("getAppkey", (q_method_t)TUXEDOQCTL_getAppkey);
  ctl->addMethod("setAppkey", (q_method_t)TUXEDOQCTL_setAppkey);
  ctl->addMethod("getUrcode", (q_method_t)TUXEDOQCTL_getUrcode);
  ctl->addMethod("setUrcode", (q_method_t)TUXEDOQCTL_setUrcode);

  traceout("initTuxedoQueueControlParamsClass");
  return ctl;
}


// EOF


