/*
  tibae.h

  TIBCO integration to QORE

  Copyright 2003 - 2008 David Nichols

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

#ifndef QORE_TIBAE_H

#define QORE_TIBAE_H
#include <Maverick.h>

#define TIBCO_EXPLICIT_CREATE_SESSION 1
#define DEFAULT_SUBJECT "QORE.MESSAGE"

// TIBAE primitive type codes
#define TIBAE_BINARY    1
#define TIBAE_BOOLEAN   2
#define TIBAE_BYTE      3
#define TIBAE_CHAR      4
#define TIBAE_DATE      5
#define TIBAE_DATETIME  6
#define TIBAE_FIXED     7
#define TIBAE_I1        8
#define TIBAE_I2        9
#define TIBAE_I4        10
#define TIBAE_I8        11
#define TIBAE_INTERVAL  12
#define TIBAE_R4        13
#define TIBAE_R8        14
#define TIBAE_STRING    15
#define TIBAE_TIME      16
#define TIBAE_U1        17
#define TIBAE_U2        18
#define TIBAE_U4        19
#define TIBAE_U8        20

#define MAX_TIBAE_TYPE 20

#include "TibCommandLine.h"

DLLLOCAL class AbstractQoreNode *map_mdata_to_node(MData *md, class ExceptionSink *xsink);
DLLLOCAL void set_properties(MAppProperties *appProperties, const QoreHashNode *h, TibCommandLine &tcl, ExceptionSink *xsink);

#endif
