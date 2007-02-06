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
#include <qore/Exception.h>
#include <qore/Qore.h>

#include <ctpublic.h>
#include <assert.h>

#include "sybase.h"

#ifndef QORE_MONOLITHIC
DLLEXPORT char qore_module_name[] = "sybase";
DLLEXPORT char qore_module_version[] = "1.0";
DLLEXPORT char qore_module_description[] = "Sybase database driver";
DLLEXPORT char qore_module_author[] = "Qore Technologies";
DLLEXPORT char qore_module_url[] = "http://qore.sourceforge.net";
DLLEXPORT int qore_module_api_major = QORE_MODULE_API_MAJOR;
DLLEXPORT int qore_module_api_minor = QORE_MODULE_API_MINOR;
DLLEXPORT qore_module_init_t qore_module_init = sybase_module_init;
DLLEXPORT qore_module_ns_init_t qore_module_ns_init = sybase_module_ns_init;
DLLEXPORT qore_module_delete_t qore_module_delete = sybase_module_delete;
#endif

//------------------------------------------------------------------------------
class sybase_connection
{
private:
  CS_CONTEXT* m_context;
  CS_CONNECTION* m_connection;

  static CS_RETCODE csmsg_callback();
  static CS_RETCODE clientmsg_callback();
  static CS_RETCODE servermsg_callback();

public:
  sybase_connection();
  ~sybase_connection();
  void init(char* username, char* password, ExceptionSink* xsink);
};

//------------------------------------------------------------------------------
sybase_connection::sybase_connection()
: m_context(0), m_connection(0)
{
}

//------------------------------------------------------------------------------
sybase_connection::~sybase_connection()
{
  CS_RETCODE ret = CS_SUCCEED;
  if (m_connection) {
    ret = ct_close(m_connection, CS_UNUSED);
    if (ret != CS_SUCCEED) {
      assert(false); // not much can be done here
    }
  }
  if (m_context) {
   CS_INT exit_type = ret == CS_SUCCEED ? CS_UNUSED : CS_FORCE_EXIT;
    ret = ct_exit(m_context, exit_type);
    if (ret != CS_SUCCEED) {
      assert(false); // not much can be done here
    } 
    ret = cs_ctx_drop(m_context);
    if (ret != CS_SUCCEED) {
      assert(false); // not much can be done here
    }
  }
}

//------------------------------------------------------------------------------
// Post-constructor initialization used as it fits (unfortunately) better
// with current programming model.
void sybase_connection::init(char* username, char* password, ExceptionSink* xsink)
{
  assert(!m_connection);
  assert(!m_context);
  CS_RETCODE ret = cs_ctx_alloc(CS_VERSION_100, &m_context);
  if (ret != CS_SUCCEED) {
    xsink->raiseException("DBI:SYBASE:CT-LIB-CANNOT-ALLOCATE-ERROR", "cs_ctx_alloc() failed with error %d", ret);
    return;
  }
  ret = ct_init(m_context, CS_VERSION_100);
  if (ret != CS_SUCCEED) {
    xsink->raiseException("DBI:SYBASE:CT-LIB-INIT-FAILED", "ct_init() failed with error %d", ret);
    return;  
  }
  
}

//------------------------------------------------------------------------------
CS_RETCODE sybase_connection::csmsg_callback()
{
  return 0; // TBD
}

//------------------------------------------------------------------------------
CS_RETCODE sybase_connection::clientmsg_callback()
{
  return 0; // TBD
}

//------------------------------------------------------------------------------
CS_RETCODE sybase_connection::servermsg_callback()
{
  return 0; // TBD
}

//------------------------------------------------------------------------------
int syb_commit_transaction(class Datasource *ds)
{
}

int syb_rollback_transaction(class Datasource *ds)
{
  return 0; // TBD
}

int syb_exec_sql(class Query *query, char *query_str)
{
  return 0; // TBD
}

class Query *syb_exec_query(class Query *query, char *query_str)
{
}

int syb_ds_init(class Datasource *ds)
{
   CS_RETCODE rc;

   tracein("syb_ds_init()");

//   ds->setPrivateData(new struct dst_sybase_s);
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

//------------------------------------------------------------------------------
QoreString *sybase_module_init()
{
   tracein("sybase_module_init()");
/*
   // register driver with DBI subsystem
   DBIDriverFunctions *ddf =
      new DBIDriverFunctions(oracle_open,
                             oracle_close,
                             oracle_select,
                             oracle_select_rows,
                             oracle_exec,
                             oracle_commit,
                             oracle_rollback);
   DBID_ORACLE = DBI.registerDriver("oracle", ddf, DBI_ORACLE_CAPS);
*/
   traceout("sybase_module_init()");
   return NULL;
}

//------------------------------------------------------------------------------
void oracle_module_ns_init(Namespace *rns, Namespace *qns)
{
   tracein("sybase_module_ns_init()");
   // nothing to do at the moment
   traceout("sybase_module_ns_init()");
}

//------------------------------------------------------------------------------
void sybase_module_delete()
{
   tracein("sybase_module_delete()");
   //DBI_deregisterDriver(DBID_SYBASE); - commented out because copied from oracle module
   traceout("sybase_module_delete()");
}






