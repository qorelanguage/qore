/*
  modules/Tuxedo/QC_TuxedoConnection.cc

  Tuxedo integration to QORE

  Qore Programming Language

  Copyright (C) 2006 Pavel Vozenilek

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

#include "QC_TuxedoConnection.h"
#include "QoreTuxedoConnection.h"

// Provides a class that allows a client to connect to a Tuxedo enabled server(s).
// Every thread could instantiate single instance of this class.
//
// For more information see:
// * tpchkauth() documentation: http://edocs.bea.com/tuxedo/tux91/rf3c/rf3c28.htm#1040017
// * tpinit() documentation:  http://edocs.bea.com/tuxedo/tux91/rf3c/rf3c55.htm#1022852
//     (Note that the initialisation may be a simple call without
//      parameters or parameters may be provided.)
// * tpterm() documentation: http://edocs.bea.com/tuxedo/tux91/rf3c/rf3c86.htm#1219084
// 

int CID_TUXEDOCONNECTION;

//------------------------------------------------------------------------------
static void getTuxedoConnection(void* obj)
{
  ((QoreTuxedoConnection*)obj)->ROreference();
}

static void releaseTuxedoConnection(void* obj)
{
  ((QoreTuxedoConnection*)obj)->deref();
}

//------------------------------------------------------------------------------
static void TUXEDOCONNECTION_constructor(class Object *self, class QoreNode *params, class ExceptionSink *xsink)
{
  tracein("TUXEDOCONNECTION _constructor");
  QoreTuxedoConnection* conn = new QoreTuxedoConnection("", xsink);
  if (xsink->isException()) {
    conn->deref();
  } else {
    self->setPrivate(CID_TUXEDOCONNECTION, conn, getTuxedoConnection, releaseTuxedoConnection);
  }
  traceout("TUXEDOCONNECTION _constructor");
}

//------------------------------------------------------------------------------
static void TUXEDOCONNECTION_destructor(class Object *self, class QoreTuxedoConnection* conn, class ExceptionSink *xsink)
{
  tracein("TUXEDOCONNECTION_destructor");
  conn->close_connection(xsink);
  conn->deref();
  traceout("TUXEDOCONNECTION_destructor");
}

//------------------------------------------------------------------------------
static void TUXEDOCONNECTION_copy(class Object *self, class Object *old, class QoreTuxedoConnection* conn, ExceptionSink *xsink)
{
  xsink->raiseException("TUXEDOCONNECTION-COPY-ERROR", "copying Tuxedo::Connection objects is not supported.");
}

//------------------------------------------------------------------------------
class QoreClass* initTuxedoConnectionClass()
{
  tracein("initTuxedoConnectionClass");
  QoreClass* tuxedo_connection = new QoreClass(QDOM_NETWORK, strdup("Connection"));
  CID_TUXEDOCONNECTION = tuxedo_connection->getID();  

  tuxedo_connection->setConstructor((q_constructor_t)TUXEDOCONNECTION_constructor);
  tuxedo_connection->setDestructor((q_destructor_t)TUXEDOCONNECTION_destructor);
  tuxedo_connection->setCopy((q_copy_t)TUXEDOCONNECTION_copy);

  traceout("initTuxedoConnectionClass");
  return tuxedo_connection;
}


// EOF


