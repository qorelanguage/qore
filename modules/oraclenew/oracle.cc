/*
  oracle.cc

  Oracle OCI Interface to Qore DBI layer

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols

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
#include <oracle-config.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <occi.h>
#include <oracle.h>
#include <qore/support.h>
#include <SQL/DBI.h>
#include <qore/Query.h>
#include <qore/dbi.h>
#include <qore/QoreNode.h>
#include <qore/option.h>
#include <qore/QoreType.h>
#include <qore/Exception.h>
#include <qore/Object.h>
#include <qore/QoreString.h>

static Environment *env = NULL;
Connection *conn = NULL;

struct dst_oracle_s {
      Connection *conn;
      Statement *stmt;
};

#if 1
#include <ctype.h>
int __ctype_toupper(int c)
{
   return toupper(c);
}
#endif

extern char null_str[];
extern int null_str_len;

extern struct s_qore_options qore_options;

static DateTime *convert_date_time(unsigned char *str)
{
   DateTime *dt = new DateTime;
   if ((str[0] < 100) || (str[1] < 100))
      dt->year = 9999; 
   else
      dt->year = (str[0] - 100) * 100 + 
	 (str[1] - 100);
   dt->month       = str[2];
   dt->day         = str[3];
   dt->hour        = str[4] - 1;
   dt->minute      = str[5] - 1;
   dt->second      = str[6] - 1;
   dt->millisecond = 0;
   dt->relative    = 0;
   printd(1, "convert_date_time(): %d %d = %04d-%02d-%02d %02d:%02d:%02d\n", 
	  str[0], str[1], dt->year, dt->month, dt->day, dt->hour, dt->minute,
	  dt->second);
   return dt;
}

int ora_commit_transaction(class Datasource *ds, ExceptionSink *xsink)
{
   return 0;
}

int ora_rollback_transaction(class Datasource *ds, ExceptionSink *xsink)
{
   return 0;
}

class QoreNode *ora_exec_select(class Datasource *ds, char *query_str, ExceptionSink *xsink)
{
   return 0;
}

class QoreNode *ora_exec_sql(class Datasource *ds, char *query_str, QoreObject *vmap,
			     ExceptionSink *xsink)
{
   return 0;
}

class Query *ora_exec_query(class Query *query, char *query_str, ExceptionSink *xsink)
{
   return 0;
}

int ora_ds_init(class Datasource *ds, ExceptionSink *xsink)
{
   char *date_query = "alter session set nls_date_format = 'YYYYMMDDHH24MISS'";

   tracein("ora_ds_init()");

   // if datasource is already initialized
   if (ds->status == DSS_OPEN)
   {
#ifdef DEBUG
      printe("ora_ds_init(): datasource \"%s\" is already open!\n", ds->name);
#endif      
      return 0;
   }
   if (!ds->username)
   {
      (xsink->isEvent()) = new QoreException("DBI:MISSING-USERNAME", "datasource \"%s\" has an empty username parameter", ds->name);
      return 0;
   }
   if (!ds->password)
   {
      (xsink->isEvent()) = new QoreException("DBI:MISSING-PASSWORD", "datasource \"%s\" has an empty password parameter", ds->name);
      return 0;
   }
   if (!ds->dbname)
   {
      (xsink->isEvent()) = new QoreException("DBI:MISSING-DBNAME", "datasource \"%s\" has an empty dbname parameter", ds->name);
      return 0;
   }
   printd(3, "ora_ds_init(): user=%s pass=%s db=%s\n",
	  ds->username, ds->password, ds->dbname);

   ds->d.oracle = new struct dst_oracle_s;

   // connect to database
   ds->d.oracle->conn = env->createConnection(ds->username, ds->password, ds->dbname);
   if (!ds->d.oracle->conn)
   {
      // handle error
      printe("OH SHIT"); leave(1);
   }

   // execute initialization statements
   ds->d.oracle->stmt = ds->d.oracle->conn->createStatement(date_query);

   ds->d.oracle->conn->executeUpdate();

   traceout("ora_ds_init()");
   return 0;
}

int ora_ds_close(class Datasource *ds)
{
   tracein("ora_ds_close()");

   // if datasource is already closed
   if (ds->status == DSS_CLOSED)
      return 0;
   printd(3, "ora_ds_close(): closing connection to %s\n", ds->dbname);

   // ...
   ds->d.oracle->conn->terminateStatement(ds->d.oracle->stmt)
   env->terminateConnection(ds->d.oracle->conn);

   delete ds->d.oracle;
   ds->d.oracle = NULL;

   ds->status = DSS_CLOSED;
   traceout("ora_ds_close()");
   return 0;
}

void oracle_module_init()
{
   tracein("oracle_module_init()");
   env = Environment::createEnvironment();
   traceout("oracle_module_init()");
}

void oracle_module_delete()
{
   tracein("oracle_module_delete()");
   Environment::terminateEnvironment(env);
   traceout("oracle_module_delete()");
}
