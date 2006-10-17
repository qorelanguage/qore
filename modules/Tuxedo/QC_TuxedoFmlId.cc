/*
  modules/Tuxedo/QC_TuxedoFmlId.cc

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

#include "QC_TuxedoFmlId.h"
#include "QoreTuxedoFmlId.h"

int CID_TUXEDOFMLID;

//------------------------------------------------------------------------------
static void getTuxedoFmlId(void* obj)
{
  ((QoreTuxedoFmlId*)obj)->ROreference();
}

static void releaseTuxedoFmlId(void* obj)
{
  ((QoreTuxedoFmlId*)obj)->deref();
}

//------------------------------------------------------------------------------
// No parameters.
static void TUXEDOFMLID_constructor(Object *self, QoreNode *params, ExceptionSink *xsink)
{
  tracein("TUXEDOFMLID_constructor");

  if (get_param(params, 0)) {
    xsink->raiseException("TUXEDO-FML_ID-CONSTRUCTOR", "No parameters are allowed in the constructor.");
    return;
  }

  QoreTuxedoFmlId* id = new QoreTuxedoFmlId();
  self->setPrivate(CID_TUXEDOFMLID, id, getTuxedoFmlId, releaseTuxedoFmlId);

  traceout("TUXEDOFMLID_constructor");
}

//------------------------------------------------------------------------------
static void TUXEDOFMLID_destructor(Object *self, QoreTuxedoFmlId* id, ExceptionSink *xsink)
{
  tracein("TUXEDOFMLID_destructor");
  id->deref();
  traceout("TUXEDOFMLID_destructor");
}

//------------------------------------------------------------------------------
static void TUXEDOFMLID_copy(Object *self, Object *old, QoreTuxedoFmlId* id, ExceptionSink *xsink)
{
  xsink->raiseException("TUXEDO-FML_ID-COPY", "copying Tuxedo::TuxedoFmlId objects is not yet supported.");
}

//------------------------------------------------------------------------------
class QoreClass* initTuxedoFmlIdClass()
{
  tracein("initTuxedoFmlIdClass");
  QoreClass* id = new QoreClass(QDOM_NETWORK, strdup("TuxedoFmlId"));
  CID_TUXEDOFMLID = id->getID();  

  id->setConstructor((q_constructor_t)TUXEDOFMLID_constructor);
  id->setDestructor((q_destructor_t)TUXEDOFMLID_destructor);
  id->setCopy((q_copy_t)TUXEDOFMLID_copy);

  traceout("initTuxedoFmlIdClass");
  return id;
}


// EOF


