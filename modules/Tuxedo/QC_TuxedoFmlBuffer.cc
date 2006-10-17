/*
  modules/Tuxedo/QC_TuxedoFmlBuffer.cc

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

#include "QC_TuxedoFmlBuffer.h"
#include "QoreTuxedoFmlBuffer.h"

int CID_TUXEDOFMLBUFFER;

//------------------------------------------------------------------------------
static void getTuxedoFmlBuffer(void* obj)
{
  ((QoreTuxedoFmlBuffer*)obj)->ROreference();
}

static void releaseTuxedoFmlBuffer(void* obj)
{
  ((QoreTuxedoFmlBuffer*)obj)->deref();
}

//------------------------------------------------------------------------------
// No parameters.
static void TUXEDOFML_constructor(Object *self, QoreNode *params, ExceptionSink *xsink)
{
  tracein("TUXEDOFML_constructor");

  if (get_param(params, 0)) {
    xsink->raiseException("TUXEDO-FML_BUFFER-CONSTRUCTOR", "No parameters are allowed in the constructor.");
    return;
  }

  QoreTuxedoFmlBuffer* buff = new QoreTuxedoFmlBuffer();
  self->setPrivate(CID_TUXEDOFMLBUFFER, buff, getTuxedoFmlBuffer, releaseTuxedoFmlBuffer);

  traceout("TUXEDOFML_constructor");
}

//------------------------------------------------------------------------------
static void TUXEDOFML_destructor(Object *self, QoreTuxedoFmlBuffer* buff, ExceptionSink *xsink)
{
  tracein("TUXEDOFML_destructor");
  buff->deref();
  traceout("TUXEDOFML_destructor");
}

//------------------------------------------------------------------------------
static void TUXEDOFML_copy(Object *self, Object *old, QoreTuxedoFmlBuffer* buff, ExceptionSink *xsink)
{
  xsink->raiseException("TUXEDO-FML_BUFFER-COPY", "copying Tuxedo::TuxedoFmlBuffer objects is not yet supported.");
}

//------------------------------------------------------------------------------
class QoreClass* initTuxedoFmlBufferClass()
{
  tracein("initTuxedoFmlBufferClass");
  QoreClass* buff = new QoreClass(QDOM_NETWORK, strdup("TuxedoFmlBuffer"));
  CID_TUXEDOFMLBUFFER = buff->getID();  

  buff->setConstructor((q_constructor_t)TUXEDOFML_constructor);
  buff->setDestructor((q_destructor_t)TUXEDOFML_destructor);
  buff->setCopy((q_copy_t)TUXEDOFML_copy);

  traceout("initTuxedoFmlBufferClass");
  return buff;
}


// EOF


