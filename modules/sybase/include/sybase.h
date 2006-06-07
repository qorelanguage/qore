/*
  sybase.h

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006 David Nichols

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

#ifndef QORE_SYBASE_H

#define QORE_SYBASE_H

#include <qore/dbi.h>

// exported function prototypes
int syb_ds_init(class Datasource *ds);
int syb_ds_close(class Datasource *ds);
int syb_commit_transaction(class Datasource *ds);
int syb_rollback_transaction(class Datasource *ds);
int syb_exec_sql(class Datasource *dsx, char *query_str);
class Query *syb_exec_query(class Query *query, char *query_str);

#endif
