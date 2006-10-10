/*
  modules/Tuxedo/QC_TuxedoConnection.cc

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
#include <string>

#include "QC_TuxedoConnection.h"
#include "QoreTuxedoConnection.h"
#include "connection_parameters_helper.h"

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
// []
// [string-name]
// [string-name, parameters-hash]
static void TUXEDOCONNECTION_constructor(Object *self, QoreNode *params, ExceptionSink *xsink)
{
  tracein("TUXEDOCONNECTION_constructor");

  const char* connection_name = 0;
  Tuxedo_connection_parameters Tuxedo_params;

  if (get_param(params, 0)) {
    QoreNode* pt = test_param(params, NT_STRING, 0);
    if (pt) {
      connection_name = pt->val.String->getBuffer();
    } else {
      xsink->raiseException("QORE-TUXEDO-CONNECTION-CONSTRUCTOR",
        "The first parameter of Tuxedo::Connection constructor (if any) needs to be a string (could be empty). "
        "The string is symbolic name of the connection.");
      return;
    }
  }
  if (get_param(params, 1)) {
    QoreNode* pt = test_param(params, NT_HASH, 1);
    if (pt) {
      Tuxedo_params.process_parameters(pt, xsink);
      if (xsink->isException()) {
        return;
      }
    } else {
      xsink->raiseException("QORE-TUXEDO-CONNECTION-CONSTRUCTOR",
        "The second parameter of Tuxedo::Connection constructor (if any) needs to be hash of connection parameters. "
        "See documentation for details on this hash.");
      return;
    }
  }
  if (get_param(params, 2)) {
    xsink->raiseException("QORE-TUXEDO-CONNECTION-CONSTRUCTOR",
      "Tuxedo::Connection constructor could have maximally two parameters.");
    return;
  }
  QoreTuxedoConnection* conn = new QoreTuxedoConnection(connection_name, Tuxedo_params, xsink);
  if (xsink->isException()) {
    conn->deref();
  } else {
    self->setPrivate(CID_TUXEDOCONNECTION, conn, getTuxedoConnection, releaseTuxedoConnection);
  }

  traceout("TUXEDOCONNECTION_constructor");
}

//------------------------------------------------------------------------------
static void TUXEDOCONNECTION_destructor(Object *self, QoreTuxedoConnection* conn, ExceptionSink *xsink)
{
  tracein("TUXEDOCONNECTION_destructor");
#ifdef DEBUG
  std::string name = conn->get_name();
#endif

  // clumsy, used to keep the deref() the same as in other modules
  conn->close_connection(xsink);
  conn->deref();

#ifdef DEBUG
  if (name.find("test-fail-close") == 0) {
    xsink->raiseException("QORE-TUXEDO-CONNECTION-DESTRUCTOR",
      "Test object: exception asked to be thrown.");
  }
#endif

  traceout("TUXEDOCONNECTION_destructor");
}

//------------------------------------------------------------------------------
static void TUXEDOCONNECTION_copy(Object *self, Object *old, QoreTuxedoConnection* conn, ExceptionSink *xsink)
{
  xsink->raiseException("QORE-TUXEDO-CONNECTION-COPY", "copying Tuxedo::Connection objects is not supported.");
}

//------------------------------------------------------------------------------
class QoreClass* initTuxedoConnectionClass()
{
  tracein("initTuxedoConnectionClass");
  QoreClass* tuxedo_connection = new QoreClass(QDOM_NETWORK, strdup("TuxedoConnection"));
  CID_TUXEDOCONNECTION = tuxedo_connection->getID();  

  tuxedo_connection->setConstructor((q_constructor_t)TUXEDOCONNECTION_constructor);
  tuxedo_connection->setDestructor((q_destructor_t)TUXEDOCONNECTION_destructor);
  tuxedo_connection->setCopy((q_copy_t)TUXEDOCONNECTION_copy);

  traceout("initTuxedoConnectionClass");
  return tuxedo_connection;
}


// EOF


