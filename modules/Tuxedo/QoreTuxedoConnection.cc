/*
  modules/Tuxedo/QoreTuxedoConnection.cc

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
#include <qore/Exception.h>
#include <atmi.h>

#include "QoreTuxedoConnection.h"
#include "connection_parameters_helper.h"
#include <string>
#include "handle_error.h"

//------------------------------------------------------------------------------
QoreTuxedoConnection::QoreTuxedoConnection(const char* name, Tuxedo_connection_parameters& params, ExceptionSink* xsink)
: m_name(name && name[0] ? name : "<unnamed>"),
  m_was_closed(true)
{
#ifdef DEBUG
  if (m_name.find("test-fail-close") == 0) {
    return;
  }
  if (m_name.find("test-fail-open") == 0) {
    xsink->raiseException("QORE-TUXEDO-CONNECTION-CONSTRUCTOR", "[%s] object that fails in constructor.\nFile %s[%d].", 
      m_name.c_str(), __FILE__, __LINE__);
    return;
  }
  if (m_name == "test" || m_name.find("test-") == 0) {
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
    if (m_name == "test" || m_name.find("test-") == 0) {
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
  std::string func_name = "tpinit() of connection [";
  func_name += m_name;
  func_name += "]";
  if (tperrno == TPEPERM) {
    char buffer[100];
    sprintf(buffer, ". Authentification result = %ld", tpurcode);
    func_name += buffer;
  }
  handle_error(tperrno, "QORE-TUXEDO-CONNECTION-CONSTRUCTOR", func_name.c_str(), xsink);
}

//------------------------------------------------------------------------------
void QoreTuxedoConnection::handle_tpterm_error(ExceptionSink* xsink) const
{
  std::string func_name = "tpterm() of connection [";
  func_name += m_name;
  func_name += "]";
  handle_error(tperrno, "QORE-TUXEDO-CONNECTION_DESTRUCTOR", func_name.c_str(), xsink);
}

//------------------------------------------------------------------------------
void QoreTuxedoConnection::close_connection(ExceptionSink* xsink)
{
#ifdef DEBUG
  if (m_name.find("xtest-fail-close") == 0) {
    xsink->raiseException("QORE-TUXEDO-CONNECTION-DESTRUCTOR",
      "[%s] object that fails in destructor.", m_name.c_str());
    return;
  }
  if (m_name == "test" || m_name.find("test-") == 0) {
    return;
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

