/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QC_DatasourcePool.h

  Qore Programming Language

  Copyright (C) 2003 - 2015 David Nichols

  The Datasource class provides the low-level interface to Qore DBI drivers.

  NOTE that this class is *not* thread-safe.  To use this class in a multi-
  threaded context, per-thread connection locking must be done at a level
  above this class...

  NOTE that 2 copies of connection values are kept in case
  the values are changed while a connection is in use

  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.

  Note that the Qore library is released under a choice of three open-source
  licenses: MIT (as above), LGPL 2+, or GPL 2+; see README-LICENSE for more
  information.
*/

#ifndef _QORUS_QC_DATASOURCEPOOL_H

#define _QORUS_QC_DATASOURCEPOOL_H

#include <qore/AbstractPrivateData.h>
#include "qore/intern/DatasourcePool.h"

#include <stdlib.h>
#include <string.h>

DLLEXPORT extern qore_classid_t CID_DATASOURCEPOOL;
DLLLOCAL extern QoreClass* QC_DATASOURCEPOOL;
DLLLOCAL extern QoreClass* QC_ABSTRACTDATASOURCE;

QoreClass *initDatasourcePoolClass(QoreNamespace& ns);

#endif
