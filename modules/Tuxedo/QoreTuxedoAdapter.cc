/*
  modules/Tuxedo/QoreTuxedoAdapter.cc

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

#include "QoreTuxedoAdapter.h"

//------------------------------------------------------------------------------
QoreTuxedoAdapter::QoreTuxedoAdapter(const char* name, Hash* params, ExceptionSink* xsink)
: m_name(name && name[0] ? name : "<unnamed>")
{
#ifdef DEBUG
  // TBD
#endif
}

//------------------------------------------------------------------------------
QoreTuxedoAdapter::~QoreTuxedoAdapter()
{
  // TBD - clean all pending requests
}

//------------------------------------------------------------------------------
void QoreTuxedoAdapter::close_adapter(ExceptionSink* xsink)
{
  // TBD
}

//------------------------------------------------------------------------------
Hash* QoreTuxedoAdapter::call(char* service_name, Hash* params, long flags, ExceptionSink* xsink)
{
  return 0;
}

//------------------------------------------------------------------------------
int QoreTuxedoAdapter::async_call(char* service_name, Hash* params, long flags, ExceptionSink* xsink)
{
  return 0; // TBD
}

//------------------------------------------------------------------------------
void QoreTuxedoAdapter::cancel_async(int handle, ExceptionSink* xsink)
{
  char* err = "QORE-TUXEDO-ADAPTER-CANCEL_ASYNC";

  for (std::list<int>::iterator it = m_pending_async_requests.begin(), end = m_pending_async_requests.end(); it != end; ++it) {
    if (*it == handle) {
      m_pending_async_requests.erase(it);
      if (tpcancel(handle) != -1) {
        return;
      }
      handle_tpcancel_error(err, tperrno, handle, xsink);
    }
  }
  xsink->raiseException(err, "Invalid handle value.");
}

//------------------------------------------------------------------------------
void QoreTuxedoAdapter::handle_tpcancel_error(char* err, int tperrnum, int handle, ExceptionSink* xsink)
{
  switch (tperrnum) {
  case TPEBADDESC:
    xsink->raiseException(err, "tpcancel() returned TPEBADDESC error. Value %d is not valid handle.", handle);
    return;
  case TPETRAN:
    xsink->raiseException(err, "tpcancel() returned TPETRAC error: the handle is associated with Tuxedo transaction.");
    return;
  case TPEPROTO:
    xsink->raiseException(err, "tpcancel() returned TPEPROTO error: called improperly.");
    return;
  case TPESYSTEM:
    xsink->raiseException(err, "tpcancel() returned TPESYSTEM error: Tuxedo problem, see Tuxedo log for details.");
    return;
  case TPEOS:
    xsink->raiseException(err, "tpcancel() returned TPEOS error: operating system error, errno = %d", errno);
    return;
  default:
    xsink->raiseException(err, "tpcancel() returned unknown error %d.", tperrnum);
    return;
  }
}

//------------------------------------------------------------------------------
Hash* QoreTuxedoAdapter::get_async_result(int handle, Hash* out, long flags, ExceptionSink* xsink)
{
  return 0; // TBD
}

//-----------------------------------------------------------------------------
char* QoreTuxedoAdapter::hash2buffer(Hash* hash, ExceptionSink* xsink)
{
  // TBD
  return 0;
}

//------------------------------------------------------------------------------
Hash* QoreTuxedoAdapter::buffer2hash(char* buffer, ExceptionSink* xsink)
{
  // TBD
  return 0;
}

// EOF

