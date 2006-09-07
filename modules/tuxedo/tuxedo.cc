/*
  tuxedo/tuxedo.cc

  Tuxedo integration to QORE

  Copyright (C) 2003, 2004, 2005, 2006 David Nichols

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

#include <qore/config.h>
#include <qore/support.h>
#include <qore/QoreNode.h>
#include <qore/QoreType.h>
#include <qore/Object.h>
#include <qore/Exception.h>
#include <qore/QoreString.h>
#include <qore/params.h>
#include <qore/QoreClass.h>

#include "tuxedo.h"

extern int CID_TUXEDOCLIENT;

// usage: new Tuxedo()
class QoreNode *TUXEDOCLIENT_constructor(class QoreNode *params, ExceptionSink *xsink)
{
   tracein("TUXEDOCLIENT_constructor");

   Object *self = getStackObject();

   //self->private_data = (void *) ... ;
   //printd(5, "TUXEDOCLIENT_constructor() this=%08p myQoreApp=%08p\n", self, myQoreApp);
   traceout("TUXEDOCLIENT_constructor");
   return NULL;
}

class QoreNode *TUXEDOCLIENT_destructor(class QoreNode *params, ExceptionSink *xsink)
{
   tracein("TUXEDOCLIENT_destructor()");
   // set adapter paramter
   Object *self = getStackObject();
   //QoreApp *myQoreApp = (QoreApp *)self->private_data;
   //myQoreApp->deref(xsink);
#ifdef DEBUG
   self->private_data = NULL;
#endif
   traceout("TUXEDOCLIENT_destructor()");
   return NULL;
}

/*
// make into untyped object (hash) on copy
static QoreNode *TUXEDOCLIENT_copy(QoreNode *params, ExceptionSink *xsink)
{
}
*/

class QoreClass *initTuxedoClientClass()
{
   tracein("initTuxedoClientClass()");

   class QoreClass *QC_TUXEDOCLIENT = new QoreClass(strdup("TuxedoClient"));
   CID_TUXEOCLIENT = QC_TUXEOCLIENT->getID();
   QC_TUXEDOCLIENT->setConstructor(TUXEDOCLIENT_constructor);
   QC_TUXEDOCLIENT->setDestructor( TUXEDOCLIENT_destructor);
   //QC_TUXEDOCLIENT->setCopy(TUXEDOCLIENT_copy);

   traceout("initTuxedoClientClass()");
   return QC_TUXEDOCLIENT;
}
