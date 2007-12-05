/*
  oracle.h

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006

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

#ifndef QORE_ORACLE_H

#define QORE_ORACLE_H

#include <qore/common.h>
#include <qore/dbi.h>
#include <qore/Exception.h>

// exported function prototypes
int ora_ds_init(class Datasource *ds, ExceptionSink *xsink);
int ora_ds_close(class Datasource *ds);
int ora_commit_transaction(class Datasource *ds, ExceptionSink *xsink);
int ora_rollback_transaction(class Datasource *ds, ExceptionSink *xsink);
class QoreNode *ora_exec_select(class Datasource *dsx, char *query_str, ExceptionSink *xsink);
class QoreNode *ora_exec_sql(class Datasource *dsx, char *query_str, class QoreObject *vmap,
			     ExceptionSink *xsink);
class Query *ora_exec_query(class Query *query, char *query_str, ExceptionSink *xsink);

#endif
