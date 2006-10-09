/*
  modules/Tuxedo/handle_error.cc

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

#include "handle_error.h"
#include <atmi.h>

//------------------------------------------------------------------------------
void handle_error(int tperrnum, char* err, const char* func, ExceptionSink* xsink)
{
  switch (tperrnum) {
  case TPEINVAL:
    xsink->raiseException(err, "%s fails with error TPEINVAL (invalid arguments).", func);
    break;
  case TPENOENT:
    xsink->raiseException(err, "%s failed with error TPENOENT (no match for the type or subtype).", func);
    break;
  case TPEPROTO:
    xsink->raiseException(err, "%s failed with error TPEPROTO (called improperly).", func);
    break;
  case TPESYSTEM:
    xsink->raiseException(err, "%s failed with error TPESYSTEM (Tuxedo error, see Tuxedo log).", func);
    break;
  case TPEOS:
    xsink->raiseException(err, "%s failed with error TPEOS (OS error, errno = %d).", func, errno);
    break;
  case TPEPERM:
    xsink->raiseException(err, "%s failed with error TPEPERM (permission denied).", func);
    break;
  case TPEITYPE:
    xsink->raiseException(err, "%s returned TPEITYPE (invalid input type or subtype).", func);
    break;
  case TPEOTYPE:
    xsink->raiseException(err, "%s returned TPEOTYPE (invalid output type or subtype).", func);
    break;
  case TPETRAN:
    xsink->raiseException(err, "%s returned TPETRAN (transaction not supported).", func);
    break;
  case TPETIME:
    xsink->raiseException(err, "%s returned TPETIME (timeout or transcation rolled back).", func);
    break;
  case TPESVCFAIL:
    xsink->raiseException(err, "%s returned TPESVCFAIL (service failed).", func);
    break;
  case TPESVCERR:
    xsink->raiseException(err, "%s returned TPESVCERR (service error).", func);
    break;
  case TPEBLOCK:
    xsink->raiseException(err, "%s returned TPEBLOCK (blocking condition).", func);
    break;
  case TPGOTSIG:
    xsink->raiseException(err, "%s returned TPGOTSIG (signal received).", func);
    break;
  case TPEBADDESC:
    xsink->raiseException(err, "%s returned TPEBADDESC (invalid descriptor).", func);
    return;
  default:
    xsink->raiseException(err, "%s failed with unknown error %d", tperrnum);
    break;
  }
}

// EOF

