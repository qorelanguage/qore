/*
  modules/Tuxedo/QoreTuxedoConnection.cc

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
#include <qore/Exception.h>
#include <atmi.h>

#include "QoreTuxedoConnection.h"

//------------------------------------------------------------------------------
QoreTuxedoConnection::QoreTuxedoConnection(const char* name, ExceptionSink* xsink)
: m_name(name),
  m_was_closed(true)
{
  if (m_name.empty()) {
    m_name = "<unnamed>";
  }
  int res = tpinit(0);
  if (res == -1) {
    handle_tpinit_error(xsink);
  } else {
    m_was_closed = false;
printf("CONNECTION IS OPENED\n");
  }
}

//------------------------------------------------------------------------------
QoreTuxedoConnection::~QoreTuxedoConnection()
{
  if (!m_was_closed) {
    close_connection(0);
  }
}

//------------------------------------------------------------------------------
void QoreTuxedoConnection::handle_tpinit_error(ExceptionSink* xsink) const
{
  switch (tperrno) {
  case TPEINVAL: 
    xsink->raiseException("QORE-CONNECTION-CONSTRUCTOR", 
       "connection named [%s]: tpinit(NULL) failed with TPEINVAL result (invalid arguments).", m_name.c_str());
    break;
  case TPENOENT: 
    xsink->raiseException("QORE-CONNECTION-CONSTRUCTOR", 
      "connection named [%s]: tpinit(NULL) failed with TPENOENT result (space limitations).", m_name.c_str());
    break;
  case TPEPERM: 
    xsink->raiseException("QORE-CONNECTION-CONSTRUCTOR",
      "connection named [%s]: tpinit(NULL) failed with TPEPERM (permission denied). Authentification result = %d.",
      m_name.c_str(), tpurcode);
    break;
  case TPEPROTO:
    xsink->raiseException("QORE-CONNECTION-CONSTRUCTOR",
      "connection named [%s]: tpinit(NULL) failed with TPEPROTO (called improperly). See tpinit() documentation for more.",
      m_name.c_str());
    break;
  case TPESYSTEM:
    xsink->raiseException("QORE-CONNECTION-CONSTRUCTOR",
      "connection named [%s]: tpinit(NULL) failed with TPESYSTEM (Tuxedo error). See Tuxedo log file for details.", m_name.c_str());
    break;
  case TPEOS:
    xsink->raiseException("QORE-CONNECTION-CONSTRUCTOR",
      "connection named [%s]: tpinit(NULL) failed with TPEOS (OS error). errno = %d.", m_name.c_str(), errno);
    break;
  default:
    xsink->raiseException("QORE-CONNECTION-CONSTRUCTOR",
      "connection named [%s]: tpinit(NULL) failed with unrecognized error code %d.", m_name.c_str(), tperrno);
    break;
  }
}

//------------------------------------------------------------------------------
void QoreTuxedoConnection::handle_tpterm_error(ExceptionSink* xsink) const
{
  switch (tperrno) {
  case TPEPROTO:
    xsink->raiseException("QORE-CONNECTION-DESTRUCTOR",
      "connection named [%s]: tpterm() failed with error TPEPROTO (improper context).", m_name.c_str());
    break;
  case TPESYSTEM:
    xsink->raiseException("QORE-CONNECTION-DESTRUCTOR",
      "connection named [%s]: tpterm() failed with error TPESYSTEM (Tuxedo error). See Tuxedo log for details.", m_name.c_str());
    break;
  case TPEOS:
    xsink->raiseException("QORE-CONNECTION-DESTRUCTOR", 
      "connection named [%s]: tpterm() failed with error TPEOS (OS error). errno = %d.", m_name.c_str(), errno);
    break;
  default:
    xsink->raiseException("QORE-CONNECTION-DESTRUCTOR",
      "connection named [%s]: tpterm() failed with unrecognised error %d.", tperrno);
    break;
  }
}

//------------------------------------------------------------------------------
void QoreTuxedoConnection::close_connection(ExceptionSink* xsink)
{
  int res = tpterm();
  if (res == -1) {
    if (xsink) {
      handle_tpterm_error(xsink);
    }
  } else {
    m_was_closed = true;
printf("###CONNECTION IS CLOSED\n");
  }
}

// EOF

