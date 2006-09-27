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
#include "connection_parameters_helper.h"

//------------------------------------------------------------------------------
QoreTuxedoConnection::QoreTuxedoConnection(const char* name, Tuxedo_connection_parameters& params, ExceptionSink* xsink)
: m_name(name && name[0] ? name : "<unnamed>"),
  m_was_closed(true)
{
#ifdef DEBUG
  if (strstr(name, "test-fail-close") == name) {
    return;
  }
  if (strstr(name, "test-fail-open") == name) {
    xsink->raiseException("QORE-TUXEDO-CONNECTION-CONSTRUCTOR", "[%s] object that fails in constructor", name);
    return;
  }
  if (strstr(name, "test-") == name || !strcmp(name, "test")) {
    return;
  }
#endif

  int res = tpinit(params.get_tpinit_data());
  if (res == -1) {
    handle_tpinit_error(params, xsink);
  } else {
    m_was_closed = false;
  }
}

//------------------------------------------------------------------------------
QoreTuxedoConnection::~QoreTuxedoConnection()
{
#ifdef DEBUG
  if (!m_name.empty()) {
    if (m_name == "test" || m_name.find_first_of("test-") == 0) {
      return;
    }
  }
#endif
  if (!m_was_closed) {
    close_connection(0);
  }
}

//------------------------------------------------------------------------------
void QoreTuxedoConnection::handle_tpinit_error(Tuxedo_connection_parameters& params, ExceptionSink* xsink) const
{
  switch (tperrno) {
  case TPEINVAL: 
    xsink->raiseException("QORE-TUXEDO-CONNECTION-CONSTRUCTOR", 
       "connection named [%s]: tpinit() failed with TPEINVAL result (invalid arguments).", m_name.c_str());
    break;
  case TPENOENT: 
    xsink->raiseException("QORE-TUXEDO-CONNECTION-CONSTRUCTOR", 
      "connection named [%s]: tpinit() failed with TPENOENT result (space limitations).", m_name.c_str());
    break;
  case TPEPERM: 
    xsink->raiseException("QORE-TUXEDO-CONNECTION-CONSTRUCTOR",
      "connection named [%s]: tpinit() failed with TPEPERM (permission denied). Authentification result = %d.",
      m_name.c_str(), tpurcode);
    break;
  case TPEPROTO:
    xsink->raiseException("QORE-CONNECTION-CONSTRUCTOR",
      "connection named [%s]: tpinit() failed with TPEPROTO (called improperly). See tpinit() documentation for more.",
      m_name.c_str());
    break;
  case TPESYSTEM:
    xsink->raiseException("QORE-TUXEDO-CONNECTION-CONSTRUCTOR",
      "connection named [%s]: tpinit() failed with TPESYSTEM (Tuxedo error). See Tuxedo log file for details.", m_name.c_str());
    break;
  case TPEOS:
    xsink->raiseException("QORE-TUXEDO-CONNECTION-CONSTRUCTOR",
      "connection named [%s]: tpinit() failed with TPEOS (OS error). errno = %d.", m_name.c_str(), errno);
    break;
  default:
    xsink->raiseException("QORE-TUXEDO-CONNECTION-CONSTRUCTOR",
      "connection named [%s]: tpinit() failed with unrecognized error code %d.", m_name.c_str(), tperrno);
    break;
  }
}

//------------------------------------------------------------------------------
void QoreTuxedoConnection::handle_tpterm_error(ExceptionSink* xsink) const
{
  switch (tperrno) {
  case TPEPROTO:
    xsink->raiseException("QORE-TUXEDO-CONNECTION-DESTRUCTOR",
      "connection named [%s]: tpterm() failed with error TPEPROTO (improper context).", m_name.c_str());
    break;
  case TPESYSTEM:
    xsink->raiseException("QORE-TUXEDO-CONNECTION-DESTRUCTOR",
      "connection named [%s]: tpterm() failed with error TPESYSTEM (Tuxedo error). See Tuxedo log for details.", m_name.c_str());
    break;
  case TPEOS:
    xsink->raiseException("QORE-TUXEDO-CONNECTION-DESTRUCTOR", 
      "connection named [%s]: tpterm() failed with error TPEOS (OS error). errno = %d.", m_name.c_str(), errno);
    break;
  default:
    xsink->raiseException("QORE-TUXEDO-CONNECTION-DESTRUCTOR",
      "connection named [%s]: tpterm() failed with unrecognised error %d.", tperrno);
    break;
  }
}

//------------------------------------------------------------------------------
void QoreTuxedoConnection::close_connection(ExceptionSink* xsink)
{
#ifdef DEBUG
  if (!m_name.empty()) {
    if (m_name.find_first_of("test-fail-close") == 0) {
      xsink->raiseException("QORE-TUXEDO-CONNECTION-DESTRUCTOR",
        "[%s] object that fails in destructor.", m_name.c_str());
      return;
    }
    if (m_name == "test" || m_name.find_first_of("test-") == 0) {
      return;
    }
  }
#endif

  int res = tpterm();
  if (res == -1) {
    if (xsink) {
      handle_tpterm_error(xsink);
    }
  } else {
    m_was_closed = true;
  }
}

// EOF

