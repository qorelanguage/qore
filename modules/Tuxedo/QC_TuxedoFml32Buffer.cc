/*
  modules/Tuxedo/QC_TuxedoFml32Buffer.cc

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

#include "QC_TuxedoFml32Buffer.h"
#include "QoreTuxedoFml32Buffer.h"

int CID_TUXEDOFML32BUFFER;

//------------------------------------------------------------------------------
static void getTuxedoFml32Buffer(void* obj)
{
  ((QoreTuxedoFml32Buffer*)obj)->ROreference();
}

static void releaseTuxedoFml32Buffer(void* obj)
{
  ((QoreTuxedoFml32Buffer*)obj)->deref();
}

//------------------------------------------------------------------------------
// No parameters.
static void TUXEDOFML32_constructor(Object *self, QoreNode *params, ExceptionSink *xsink)
{
  tracein("TUXEDOFML32_constructor");

  if (get_param(params, 0)) {
    xsink->raiseException("TUXEDO-FML32_BUFFER-CONSTRUCTOR", "No parameters are allowed in the constructor.");
    return;
  }

  QoreTuxedoFml32Buffer* buff = new QoreTuxedoFml32Buffer();
  self->setPrivate(CID_TUXEDOFML32BUFFER, buff, getTuxedoFml32Buffer, releaseTuxedoFml32Buffer);

  traceout("TUXEDOFML32_constructor");
}

//------------------------------------------------------------------------------
static void TUXEDOFML32_destructor(Object *self, QoreTuxedoFml32Buffer* buff, ExceptionSink *xsink)
{
  tracein("TUXEDOFML32_destructor");
  buff->deref();
  traceout("TUXEDOFML32_destructor");
}

//------------------------------------------------------------------------------
static void TUXEDOFML32_copy(Object *self, Object *old, QoreTuxedoFml32Buffer* buff, ExceptionSink *xsink)
{
  xsink->raiseException("TUXEDO-FML32_BUFFER-COPY", "copying Tuxedo::TuxedoFml32Buffer objects is not yet supported.");
}

//------------------------------------------------------------------------------
class QoreClass* initTuxedoFml32BufferClass()
{
  tracein("initTuxedoFml32BufferClass");
  QoreClass* buff = new QoreClass(QDOM_NETWORK, strdup("TuxedoFml32Buffer"));
  CID_TUXEDOFML32BUFFER = buff->getID();  

  buff->setConstructor((q_constructor_t)TUXEDOFML32_constructor);
  buff->setDestructor((q_destructor_t)TUXEDOFML32_destructor);
  buff->setCopy((q_copy_t)TUXEDOFML32_copy);

  traceout("initTuxedoFml32BufferClass");
  return buff;
}


// EOF


