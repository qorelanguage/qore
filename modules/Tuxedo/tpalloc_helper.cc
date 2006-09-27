/*
  modules/Tuxedo/tpalloc_helper.cc

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
#include <string>

#include <atmi.h>
#include "tpalloc_helper.h"

//------------------------------------------------------------------------------
// See tpalloc() documentation on http://edocs.bea.com/tux91/rf3c/rf3c23.htm#1021676
//
char* tpalloc_helper(char* type, char* subtype, unsigned size, 
  char* exception_name, ExceptionSink* xsink)
{
  char* res = tpalloc(type, subtype, size);
  if (res) {
    return res;
  }

  std::string details;
  switch (tperrno) {
  case TPEINVAL:
    details = "TPEINVAL (invalid argument)";
    break;
  case TPENOENT:
    details = "TPENOENT (no match for type or subtype)";
    break;
  case TPEPROTO:
    details = "TPEPROTO (called improperly)";
    break;
  case TPESYSTEM:
    details = "TPESYSTEM (Tuxedo error, see Tuxedo log for details)";
    break;
  case TPEOS:
    {
      char buffer[100];
      sprintf(buffer, "TPEOS (OS error, errno = %d)", errno);
      details = buffer;
    }
    break;
  default:
    {
      char buffer[100];
      sprintf(buffer, "unexpected error %d", tperrno);
      details = buffer;
    }
    break;
  }

  std::string error_text;
  {
    char buffer[100];
    sprintf(buffer, "Allocating %d bytes by tpalloc() failed with error ", size);
    error_text = buffer;
  }
  error_text += details;
  error_text += ". Type = ";
  if (type && type[0]) {
    error_text += type;
  } else {
    error_text += "<NULL>";
  }
  error_text += ". Subtype = ";
  if (subtype && subtype[0]) {
    error_text += subtype;
  } else {
    error_text += "<NULL>";
  }
  xsink->raiseException(exception_name, "%s.", error_text.c_str());
  return 0;
}

// EOF

