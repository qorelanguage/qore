/*
  QC_DatasourcePool.h

 Qore Programming Language
 
 Copyright 2003 - 2009 David Nichols
 
 The Datasource class provides the low-level interface to Qore DBI drivers.
 
 NOTE that this class is *not* thread-safe.  To use this class in a multi-
 threaded context, per-thread connection locking must be done at a level
 above this class...
 
 NOTE that 2 copies of connection values are kept in case
 the values are changed while a connection is in use
 
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
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA*/

#ifndef _QORUS_QC_DATASOURCEPOOL_H

#define _QORUS_QC_DATASOURCEPOOL_H

#include <qore/AbstractPrivateData.h>
#include <qore/intern/DatasourcePool.h>

#include <stdlib.h>
#include <string.h>

extern qore_classid_t CID_DATASOURCEPOOL;

class QoreClass *initDatasourcePoolClass();

#endif
