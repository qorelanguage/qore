/*
  modules/Tuxedo/QC_TuxedoTypedBuffer.cc

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

#include "QC_TuxedoTypedBuffer.h"
#include "QoreTuxedoTypedBuffer.h"

int CID_TUXEDOTYPEDBUFFER;

//------------------------------------------------------------------------------
static void getTuxedoTypedBuffer(void* obj)
{
  ((QoreTuxedoTypedBuffer*)obj)->ROreference();
}

static void releaseTuxedoTypedBuffer(void* obj)
{
  ((QoreTuxedoTypedBuffer*)obj)->deref();
}

//------------------------------------------------------------------------------
// No parameters.
static void TUXEDOTYPEDBUFFER_constructor(Object *self, QoreNode *params, ExceptionSink *xsink)
{
  tracein("TUXEDOTYPEDBUFFER_constructor");

  if (get_param(params, 0)) {
    xsink->raiseException("TUXEDO-TYPED_BUFFER-CONSTRUCTOR", "No parameters are allowed in the constructor.");
    return;
  }

  QoreTuxedoTypedBuffer* buff = new QoreTuxedoTypedBuffer();
  self->setPrivate(CID_TUXEDOTYPEDBUFFER, buff, getTuxedoTypedBuffer, releaseTuxedoTypedBuffer);

  traceout("TUXEDOTYPEDBUFFER_constructor");
}

//------------------------------------------------------------------------------
static void TUXEDOTYPEDBUFFER_destructor(Object *self, QoreTuxedoTypedBuffer* buff, ExceptionSink *xsink)
{
  tracein("TUXEDOTYPEDBUFFER_destructor");
  buff->deref();
  traceout("TUXEDOTYPEDBUFFER_destructor");
}

//------------------------------------------------------------------------------
static void TUXEDOTYPEDBUFFER_copy(Object *self, Object *old, QoreTuxedoTypedBuffer* buff, ExceptionSink *xsink)
{
  xsink->raiseException("TUXEDO-TYPED_BUFFER-COPY", "copying Tuxedo::TuxedoTypedBuffer objects is not supported.");
}

//-----------------------------------------------------------------------------
static QoreNode* TUXEDOTYPEDBUFFER_clear(Object* self, QoreTuxedoTypedBuffer* buff, QoreNode* params, ExceptionSink* xsink)
{
  if (get_param(params, 1)) {
    xsink->raiseException("TuxedoTypedBuffer::clear()", "No parameter expected.");
    return 0;
  }
  buff->clear();
}

//------------------------------------------------------------------------------
class QoreClass* initTuxedoTypedBufferClass()
{
  tracein("initTuxedoTypedBufferClass");
  QoreClass* buff = new QoreClass(QDOM_NETWORK, strdup("TuxedoTypedBuffer"));
  CID_TUXEDOTYPEDBUFFER = buff->getID();  

  buff->setConstructor((q_constructor_t)TUXEDOTYPEDBUFFER_constructor);
  buff->setDestructor((q_destructor_t)TUXEDOTYPEDBUFFER_destructor);
  buff->setCopy((q_copy_t)TUXEDOTYPEDBUFFER_copy);
  buff->addMethod("clear", (q_method_t)TUXEDOTYPEDBUFFER_clear);

  traceout("initTuxedoTypedBufferClass");
  return buff;
}


// EOF


