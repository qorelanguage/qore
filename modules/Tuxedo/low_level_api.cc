/*
  modules/Tuxedo/low_level_api.cc

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
#include <qore/Namespace.h>
#include <qore/BuiltinFunctionList.h>
#include <qore/params.h>

#include "low_level_api.h"
#include <atmi.h>

//------------------------------------------------------------------------------
// http://edocs.bea.com/tuxedo/tux91/rf3c/rf3c28.htm#1040017
static class QoreNode* f_tpchkauth(class QoreNode* params, class ExceptionSink* xsink)
{
  if (get_param(params, 0)) {
    xsink->raiseException("tpchkauth", "tpchkauth() has no parameters.");
    return 0;
  }

  int res = tpchkauth();
  if (res == -1) {
    xsink->raiseException("tpchkauth", "Tuxedo function tpchkauth() failed.");
    return 0;
  }
  return new QoreNode((int64)res);
}

//------------------------------------------------------------------------------
void tuxedo_low_level_init()
{
  builtinFunctions.add("tpchkauth", f_tpchkauth, QDOM_NETWORK);
}

//-----------------------------------------------------------------------------
void tuxedo_low_level_ns_init(Namespace* ns)
{
  ns->addConstant("TPNOAUTH", new QoreNode((int64)TPNOAUTH));
  ns->addConstant("TPSYSAUTH", new QoreNode((int64)TPSYSAUTH));
  ns->addConstant("TPAPPAUTH", new QoreNode((int64)TPAPPAUTH));
}

// EOF

