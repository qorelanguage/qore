/*
  sybase.cc

  Sybase DB layer for QORE
  uses Sybase OpenClient C library

  Qore Programming language

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

#include <qore/config.h>
#include <qore/support.h>
#include <qore/DBI.h>
#include <qore/QoreNode.h>

#include <ctpublic.h>

#include "sybase.h"

struct dst_sybase_s {
      CS_CONTEXT *context;
      CS_CONNECTION *connection;
};

// Callback routines for library errors and server messages.
static CS_RETCODE csmsg_callback()
{
}

static CS_RETCODE clientmsg_callback()
{
}

static CS_RETCODE servermsg_callback()
{
}

int syb_commit_transaction(class Datasource *ds)
{
}

int syb_rollback_transaction(class Datasource *ds)
{
}

int syb_exec_sql(class Query *query, char *query_str)
{
}

class Query *syb_exec_query(class Query *query, char *query_str)
{
}

int syb_ds_init(class Datasource *ds)
{
   CS_RETCODE rc;

   tracein("syb_ds_init()");

   ds->setPrivateData(new struct dst_sybase_s);
/*
   // if datasource is already initialized
   if (ds->status == DSS_OPEN)
   {
#ifdef DEBUG
      printe("syb_ds_init(): datasource \"%s\" is already open!\n", ds->name);
#endif      
      return 0;
   }
   printd(3, "syb_ds_init(): user=%s pass=%s db=%s\n",
          ds->username, ds->password, ds->dbname);

   // allocate context structure
   ds->d.sybase->context = NULL;
   rc = cs_ctx_alloc(CS_VERSION_100, &ds->d.sybase->context);
   // check return code...

   ds->status = DSS_OPEN;
*/
   traceout("syb_ds_init()");
   return 0;
}

int syb_ds_close(class Datasource *ds)
{
   
}

