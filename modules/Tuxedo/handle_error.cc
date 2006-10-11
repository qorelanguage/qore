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
void handle_error(int tperrnum, char*name,  const char* func_name, ExceptionSink* xsink)
{
  switch (tperrnum) {
  case TPEINVAL:
    xsink->raiseException(name, "%s fails with error TPEINVAL (invalid arguments).", func_name);
    break;
  case TPENOENT:
    xsink->raiseException(name, "%s failed with error TPENOENT (no match for the type or subtype).", func_name);
    break;
  case TPEPROTO:
    xsink->raiseException(name, "%s failed with error TPEPROTO (called improperly).", func_name);
    break;
  case TPESYSTEM:
    xsink->raiseException(name, "%s failed with error TPESYSTEM (Tuxedo error, see Tuxedo log).", func_name);
    break;
  case TPEOS:
    xsink->raiseException(name, "%s failed with error TPEOS (OS error, errno = %d).", func_name, errno);
    break;
  case TPEPERM:
    xsink->raiseException(name, "%s failed with error TPEPERM (permission denied).", func_name);
    break;
  case TPEITYPE:
    xsink->raiseException(name, "%s returned TPEITYPE (invalid input type or subtype).", func_name);
    break;
  case TPEOTYPE:
    xsink->raiseException(name, "%s returned TPEOTYPE (invalid output type or subtype).", func_name);
    break;
  case TPETRAN:
    xsink->raiseException(name, "%s returned TPETRAN (transaction not supported).", func_name);
    break;
  case TPETIME:
    xsink->raiseException(name, "%s returned TPETIME (timeout or transcation rolled back).", func_name);
    break;
  case TPESVCFAIL:
    xsink->raiseException(name, "%s returned TPESVCFAIL (service failed).", func_name);
    break;
  case TPESVCERR:
    xsink->raiseException(name, "%s returned TPESVCERR (service error).", func_name);
    break;
  case TPEBLOCK:
    xsink->raiseException(name, "%s returned TPEBLOCK (blocking condition).", func_name);
    break;
  case TPGOTSIG:
    xsink->raiseException(name, "%s returned TPGOTSIG (signal received).", func_name);
    break;
  case TPEBADDESC:
    xsink->raiseException(name, "%s returned TPEBADDESC (invalid descriptor).", func_name);
    return;
  default:
    xsink->raiseException(name, "%s failed with unknown error %d", tperrnum);
    break;
  }
}

// EOF

