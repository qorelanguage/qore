/*
  modules/Tuxedo/QC_TuxedoFml32Id.cc

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

#include "QC_TuxedoFml32Id.h"
#include "QoreTuxedoFml32Id.h"

int CID_TUXEDOFMLID32;

//------------------------------------------------------------------------------
static void getTuxedoFml32Id(void* obj)
{
  ((QoreTuxedoFml32Id*)obj)->ROreference();
}

static void releaseTuxedoFml32Id(void* obj)
{
  ((QoreTuxedoFml32Id*)obj)->deref();
}

//------------------------------------------------------------------------------
// No parameters.
static void TUXEDOFML32ID_constructor(Object *self, QoreNode *params, ExceptionSink *xsink)
{
  tracein("TUXEDOFML32ID_constructor");

  if (get_param(params, 0)) {
    xsink->raiseException("TUXEDO-FML32_ID-CONSTRUCTOR", "No parameters are allowed in the constructor.");
    return;
  }

  QoreTuxedoFml32Id* id = new QoreTuxedoFml32Id();
  self->setPrivate(CID_TUXEDOFML32ID, id, getTuxedoFml32Id, releaseTuxedoFml32Id);

  traceout("TUXEDOFML32ID_constructor");
}

//------------------------------------------------------------------------------
static void TUXEDOFML32ID_destructor(Object *self, QoreTuxedoFml32Id* id, ExceptionSink *xsink)
{
  tracein("TUXEDOFML32ID_destructor");
  id->deref();
  traceout("TUXEDOFML32ID_destructor");
}

//------------------------------------------------------------------------------
static void TUXEDOFML32ID_copy(Object *self, Object *old, QoreTuxedoFml32Id* id, ExceptionSink *xsink)
{
  xsink->raiseException("TUXEDO-FML32_ID-COPY", "copying Tuxedo::TuxedoFml32Id objects is not yet supported.");
}

//------------------------------------------------------------------------------
class QoreClass* initTuxedoFml32IdClass()
{
  tracein("initTuxedoFml32IdClass");
  QoreClass* id = new QoreClass(QDOM_NETWORK, strdup("TuxedoFml32Id"));
  CID_TUXEDOFML32ID = id->getID();  

  id->setConstructor((q_constructor_t)TUXEDOFML32ID_constructor);
  id->setDestructor((q_destructor_t)TUXEDOFML32ID_destructor);
  id->setCopy((q_copy_t)TUXEDOFML32ID_copy);

  traceout("initTuxedoFml32IdClass");
  return id;
}


// EOF


