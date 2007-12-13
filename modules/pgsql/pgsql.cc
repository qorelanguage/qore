/*
  pgsql.cc
  
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

#include <qore/Qore.h>

#include "QorePGConnection.h"
#include "QorePGMapper.h"

#include <libpq-fe.h>

static class QoreString *pgsql_module_init();
static void pgsql_module_ns_init(class Namespace *rns, class Namespace *qns);
static void pgsql_module_delete();

#ifndef QORE_MONOLITHIC
DLLEXPORT char qore_module_name[] = "pgsql";
DLLEXPORT char qore_module_version[] = "0.1";
DLLEXPORT char qore_module_description[] = "PostgreSQL module";
DLLEXPORT char qore_module_author[] = "David Nichols";
DLLEXPORT char qore_module_url[] = "http://qore.sourceforge.net";
DLLEXPORT int qore_module_api_major = QORE_MODULE_API_MAJOR;
DLLEXPORT int qore_module_api_minor = QORE_MODULE_API_MINOR;
DLLEXPORT qore_module_init_t qore_module_init = pgsql_module_init;
DLLEXPORT qore_module_ns_init_t qore_module_ns_init = pgsql_module_ns_init;
DLLEXPORT qore_module_delete_t qore_module_delete = pgsql_module_delete;
#endif

static int pgsql_caps = DBI_CAP_TRANSACTION_MANAGEMENT 
   | DBI_CAP_CHARSET_SUPPORT
   | DBI_CAP_STORED_PROCEDURES 
   | DBI_CAP_LOB_SUPPORT
   | DBI_CAP_BIND_BY_VALUE;

class DBIDriver *DBID_PGSQL = NULL;

static int qore_pgsql_commit(class Datasource *ds, ExceptionSink *xsink)
{
   QorePGConnection *pc = (QorePGConnection *)ds->getPrivateData();

   return pc->commit(ds, xsink);
}

static int qore_pgsql_rollback(class Datasource *ds, ExceptionSink *xsink)
{
   QorePGConnection *pc = (QorePGConnection *)ds->getPrivateData();

   return pc->rollback(ds, xsink);
}

static int qore_pgsql_begin_transaction(class Datasource *ds, ExceptionSink *xsink)
{
   QorePGConnection *pc = (QorePGConnection *)ds->getPrivateData();

   return pc->begin_transaction(ds, xsink);
}

static class QoreNode *qore_pgsql_select_rows(class Datasource *ds, QoreString *qstr, class List *args, class ExceptionSink *xsink)
{
   QorePGConnection *pc = (QorePGConnection *)ds->getPrivateData();

   return pc->select_rows(ds, qstr, args, xsink);
}

static class QoreNode *qore_pgsql_select(class Datasource *ds, QoreString *qstr, class List *args, class ExceptionSink *xsink)
{
   QorePGConnection *pc = (QorePGConnection *)ds->getPrivateData();

   return pc->select(ds, qstr, args, xsink);
}

static class QoreNode *qore_pgsql_exec(class Datasource *ds, QoreString *qstr, class List *args, class ExceptionSink *xsink)
{
   QorePGConnection *pc = (QorePGConnection *)ds->getPrivateData();

   return pc->exec(ds, qstr, args, xsink);
}

static int qore_pgsql_open(Datasource *ds, ExceptionSink *xsink)
{
   printd(5, "qore_mysql_init() datasource %08p for DB=%s\n", ds, ds->getDBName() ? ds->getDBName() : "unknown");

   // string for connection arguments
   QoreString lstr;
   if (ds->getUsername())
      lstr.sprintf("user='%s' ", ds->getUsername());

   if (ds->getPassword())
      lstr.sprintf("password='%s' ", ds->getPassword());

   if (ds->getDBName())
      lstr.sprintf("dbname='%s' ", ds->getDBName());
   
   if (ds->getHostName())
      lstr.sprintf("host='%s' ", ds->getHostName());

   if (ds->getDBEncoding())
   {
      class QoreEncoding *enc = QorePGMapper::getQoreEncoding(ds->getDBEncoding());
      ds->setQoreEncoding(enc);
   }
   else
   {
      char *enc = (char *)QorePGMapper::getPGEncoding(QCS_DEFAULT);
      if (!enc)
      {
         xsink->raiseException("DBI:PGSQL:UNKNOWN-CHARACTER-SET", "cannot find the PostgreSQL character encoding equivalent for '%s'", QCS_DEFAULT->getCode());
         return -1;
      }
      ds->setDBEncoding(enc);
      ds->setQoreEncoding(QCS_DEFAULT);
   }

   class QorePGConnection *pc = new QorePGConnection(lstr.getBuffer(), xsink);

   if (*xsink || pc->setPGEncoding(ds->getDBEncoding(), xsink))
   {
      delete pc;
      return -1;
   }

   ds->setPrivateData((void *)pc);
   return 0;
}

static int qore_pgsql_close(class Datasource *ds)
{
   QorePGConnection *pc = (QorePGConnection *)ds->getPrivateData();

   delete pc;
   ds->setPrivateData(NULL);
   return 0;
}

static class QoreNode *qore_pgsql_get_server_version(class Datasource *ds, class ExceptionSink *xsink)
{
   QorePGConnection *pc = (QorePGConnection *)ds->getPrivateData();
   return new QoreNode((int64)pc->get_server_version());
}

static class QoreNode *f_pgsql_bind(class QoreNode *params, class ExceptionSink *xsink)
{
   class QoreNode *p = get_param(params, 0);
   int type = p? p->getAsInt() : 0;
   if (!type)
   {
      xsink->raiseException("PGSQL-BIND-ERROR", "expecting OID (type number) as first parameter to pgsql_bind()");
      return NULL;
   }
   p = get_param(params, 1);
   class Hash *h = new Hash();
   h->setKeyValue("^pgtype^", new QoreNode((int64)type), xsink);
   h->setKeyValue("^value^", p ? p->RefSelf() : NULL, xsink);
   return new QoreNode(h);
}

static class Namespace *pgsql_ns;

static void init_namespace()
{
   pgsql_ns = new Namespace("PGSQL");

   pgsql_ns->addConstant("PG_TYPE_BOOL",                new QoreNode((int64)BOOLOID));
   pgsql_ns->addConstant("PG_TYPE_BYTEA",               new QoreNode((int64)BYTEAOID));
   pgsql_ns->addConstant("PG_TYPE_CHAR",                new QoreNode((int64)CHAROID));
   pgsql_ns->addConstant("PG_TYPE_NAME",                new QoreNode((int64)NAMEOID));
   pgsql_ns->addConstant("PG_TYPE_INT8",                new QoreNode((int64)INT8OID));
   pgsql_ns->addConstant("PG_TYPE_INT2",                new QoreNode((int64)INT2OID));
   pgsql_ns->addConstant("PG_TYPE_INT2VECTOR",          new QoreNode((int64)INT2VECTOROID));
   pgsql_ns->addConstant("PG_TYPE_INT4",                new QoreNode((int64)INT4OID));
   pgsql_ns->addConstant("PG_TYPE_REGPROC",             new QoreNode((int64)REGPROCOID));
   pgsql_ns->addConstant("PG_TYPE_TEXT",                new QoreNode((int64)TEXTOID));
   pgsql_ns->addConstant("PG_TYPE_OID",                 new QoreNode((int64)OIDOID));
   pgsql_ns->addConstant("PG_TYPE_TID",                 new QoreNode((int64)TIDOID));
   pgsql_ns->addConstant("PG_TYPE_XID",                 new QoreNode((int64)XIDOID));
   pgsql_ns->addConstant("PG_TYPE_CID",                 new QoreNode((int64)CIDOID));
   pgsql_ns->addConstant("PG_TYPE_VECTOROID",           new QoreNode((int64)OIDVECTOROID));
#ifdef PG_TYPE_RELTYPE_OID
   pgsql_ns->addConstant("PG_TYPE_TYPE_RELTYPE",        new QoreNode((int64)PG_TYPE_RELTYPE_OID));
#endif
#ifdef PG_ATTRIBUTE_RELTYPE_OID
   pgsql_ns->addConstant("PG_TYPE_ATTRIBUTE_RELTYPE",   new QoreNode((int64)PG_ATTRIBUTE_RELTYPE_OID));
#endif
#ifdef PG_PROC_RELTYPE_OID
   pgsql_ns->addConstant("PG_TYPE_PROC_RELTYPE",        new QoreNode((int64)PG_PROC_RELTYPE_OID));
#endif
#ifdef PG_CLASS_RELTYPE_OID
   pgsql_ns->addConstant("PG_TYPE_CLASS_RELTYPE",       new QoreNode((int64)PG_CLASS_RELTYPE_OID));
#endif
   pgsql_ns->addConstant("PG_TYPE_POINT",               new QoreNode((int64)POINTOID));
   pgsql_ns->addConstant("PG_TYPE_LSEG",                new QoreNode((int64)LSEGOID));
   pgsql_ns->addConstant("PG_TYPE_PATH",                new QoreNode((int64)PATHOID));
   pgsql_ns->addConstant("PG_TYPE_BOX",                 new QoreNode((int64)BOXOID));
   pgsql_ns->addConstant("PG_TYPE_POLYGON",             new QoreNode((int64)POLYGONOID));
   pgsql_ns->addConstant("PG_TYPE_LINE",                new QoreNode((int64)LINEOID));
   pgsql_ns->addConstant("PG_TYPE_FLOAT4",              new QoreNode((int64)FLOAT4OID));
   pgsql_ns->addConstant("PG_TYPE_FLOAT8",              new QoreNode((int64)FLOAT8OID));
   pgsql_ns->addConstant("PG_TYPE_ABSTIME",             new QoreNode((int64)ABSTIMEOID));
   pgsql_ns->addConstant("PG_TYPE_RELTIME",             new QoreNode((int64)RELTIMEOID));
   pgsql_ns->addConstant("PG_TYPE_TINTERVAL",           new QoreNode((int64)TINTERVALOID));
   pgsql_ns->addConstant("PG_TYPE_UNKNOWN",             new QoreNode((int64)UNKNOWNOID));
   pgsql_ns->addConstant("PG_TYPE_CIRCLE",              new QoreNode((int64)CIRCLEOID));
   pgsql_ns->addConstant("PG_TYPE_CASH",                new QoreNode((int64)CASHOID));
   pgsql_ns->addConstant("PG_TYPE_MACADDR",             new QoreNode((int64)MACADDROID));
   pgsql_ns->addConstant("PG_TYPE_INET",                new QoreNode((int64)INETOID));
   pgsql_ns->addConstant("PG_TYPE_CIDR",                new QoreNode((int64)CIDROID));
   pgsql_ns->addConstant("PG_TYPE_ACLITEM",             new QoreNode((int64)ACLITEMOID));
   pgsql_ns->addConstant("PG_TYPE_BPCHAR",              new QoreNode((int64)BPCHAROID));
   pgsql_ns->addConstant("PG_TYPE_VARCHAR",             new QoreNode((int64)VARCHAROID));
   pgsql_ns->addConstant("PG_TYPE_DATE",                new QoreNode((int64)DATEOID));
   pgsql_ns->addConstant("PG_TYPE_TIME",                new QoreNode((int64)TIMEOID));
   pgsql_ns->addConstant("PG_TYPE_TIMESTAMP",           new QoreNode((int64)TIMESTAMPOID));
   pgsql_ns->addConstant("PG_TYPE_TIMESTAMPTZ",         new QoreNode((int64)TIMESTAMPTZOID));
   pgsql_ns->addConstant("PG_TYPE_INTERVAL",            new QoreNode((int64)INTERVALOID));
   pgsql_ns->addConstant("PG_TYPE_TIMETZ",              new QoreNode((int64)TIMETZOID));
   pgsql_ns->addConstant("PG_TYPE_BIT",                 new QoreNode((int64)BITOID));
   pgsql_ns->addConstant("PG_TYPE_VARBIT",              new QoreNode((int64)VARBITOID));
   pgsql_ns->addConstant("PG_TYPE_NUMERIC",             new QoreNode((int64)NUMERICOID));
   pgsql_ns->addConstant("PG_TYPE_REFCURSOR",           new QoreNode((int64)REFCURSOROID));
   pgsql_ns->addConstant("PG_TYPE_REGPROCEDURE",        new QoreNode((int64)REGPROCEDUREOID));
   pgsql_ns->addConstant("PG_TYPE_REGOPER",             new QoreNode((int64)REGOPEROID));
   pgsql_ns->addConstant("PG_TYPE_REGOPERATOR",         new QoreNode((int64)REGOPERATOROID));
   pgsql_ns->addConstant("PG_TYPE_REGCLASS",            new QoreNode((int64)REGCLASSOID));
   pgsql_ns->addConstant("PG_TYPE_REGTYPE",             new QoreNode((int64)REGTYPEOID));
   pgsql_ns->addConstant("PG_TYPE_RECORD",              new QoreNode((int64)RECORDOID));
   pgsql_ns->addConstant("PG_TYPE_CSTRING",             new QoreNode((int64)CSTRINGOID));
   pgsql_ns->addConstant("PG_TYPE_ANY",                 new QoreNode((int64)ANYOID));
   pgsql_ns->addConstant("PG_TYPE_VOID",                new QoreNode((int64)VOIDOID));
   pgsql_ns->addConstant("PG_TYPE_TRIGGER",             new QoreNode((int64)TRIGGEROID));
   pgsql_ns->addConstant("PG_TYPE_LANGUAGE_HANDLER",    new QoreNode((int64)LANGUAGE_HANDLEROID));
   pgsql_ns->addConstant("PG_TYPE_INTERNAL",            new QoreNode((int64)INTERNALOID));
   pgsql_ns->addConstant("PG_TYPE_OPAQUE",              new QoreNode((int64)OPAQUEOID));
   pgsql_ns->addConstant("PG_TYPE_ANYELEMENT",          new QoreNode((int64)ANYELEMENTOID));

   // array types
   pgsql_ns->addConstant("PG_TYPE_INT4ARRAY",           new QoreNode((int64)QPGT_INT4ARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_CIRCLEARRAY",         new QoreNode((int64)QPGT_CIRCLEARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_MONEYARRAY",          new QoreNode((int64)QPGT_MONEYARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_BOOLARRAY",           new QoreNode((int64)QPGT_BOOLARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_BYTEAARRAY",          new QoreNode((int64)QPGT_BYTEAARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_NAMEARRAY",           new QoreNode((int64)QPGT_NAMEARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_INT2ARRAY",           new QoreNode((int64)QPGT_INT2ARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_TEXTARRAY",           new QoreNode((int64)QPGT_TEXTARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_OIDARRAY",            new QoreNode((int64)QPGT_OIDARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_TIDARRAY",            new QoreNode((int64)QPGT_TIDARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_XIDARRAY",            new QoreNode((int64)QPGT_XIDARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_CIDARRAY",            new QoreNode((int64)QPGT_CIDARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_BPCHARARRAY",         new QoreNode((int64)QPGT_BPCHARARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_VARCHARARRAY",        new QoreNode((int64)QPGT_VARCHARARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_INT8ARRAY",           new QoreNode((int64)QPGT_INT8ARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_POINTARRAY",          new QoreNode((int64)QPGT_POINTARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_LSEGARRAY",           new QoreNode((int64)QPGT_LSEGARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_PATHARRAY",           new QoreNode((int64)QPGT_PATHARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_BOXARRAY",            new QoreNode((int64)QPGT_BOXARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_FLOAT4ARRAY",         new QoreNode((int64)QPGT_FLOAT4ARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_FLOAT8ARRAY",         new QoreNode((int64)QPGT_FLOAT8ARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_ABSTIMEARRAY",        new QoreNode((int64)QPGT_ABSTIMEARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_RELTIMEARRAY",        new QoreNode((int64)QPGT_RELTIMEARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_TINTERVALARRAY",      new QoreNode((int64)QPGT_TINTERVALARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_POLYGONARRAY",        new QoreNode((int64)QPGT_POLYGONARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_MACADDRARRAY",        new QoreNode((int64)QPGT_MACADDRARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_INETARRAY",           new QoreNode((int64)QPGT_INETARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_CIDRARRAY",           new QoreNode((int64)QPGT_CIDRARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_TIMESTAMPARRAY",      new QoreNode((int64)QPGT_TIMESTAMPARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_DATEARRAY",           new QoreNode((int64)QPGT_DATEARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_TIMEARRAY",           new QoreNode((int64)QPGT_TIMEARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_TIMESTAMPTZARRAY",    new QoreNode((int64)QPGT_TIMESTAMPTZARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_INTERVALARRAY",       new QoreNode((int64)QPGT_INTERVALARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_NUMERICARRAY",        new QoreNode((int64)QPGT_NUMERICARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_TIMETZARRAY",         new QoreNode((int64)QPGT_TIMETZARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_BITARRAY",            new QoreNode((int64)QPGT_BITARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_VARBITARRAY",         new QoreNode((int64)QPGT_VARBITARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_ANYARRAY",            new QoreNode((int64)ANYARRAYOID));
}

static class QoreString *pgsql_module_init()
{
#ifdef HAVE_PQISTHREADSAFE
   if (!PQisthreadsafe())
   {
      class QoreString *err = new QoreString("cannot load pgsql module; the PostgreSQL library on this system is not thread-safe");
      return err;
   }
#endif

   init_namespace();

   QorePGMapper::static_init();
   QorePGResult::static_init();

   // register database functions with DBI subsystem
   class qore_dbi_method_list methods;
   methods.add(QDBI_METHOD_OPEN, qore_pgsql_open);
   methods.add(QDBI_METHOD_CLOSE, qore_pgsql_close);
   methods.add(QDBI_METHOD_SELECT, qore_pgsql_select);
   methods.add(QDBI_METHOD_SELECT_ROWS, qore_pgsql_select_rows);
   methods.add(QDBI_METHOD_EXEC, qore_pgsql_exec);
   methods.add(QDBI_METHOD_COMMIT, qore_pgsql_commit);
   methods.add(QDBI_METHOD_ROLLBACK, qore_pgsql_rollback);
   methods.add(QDBI_METHOD_BEGIN_TRANSACTION, qore_pgsql_begin_transaction);
   methods.add(QDBI_METHOD_ABORT_TRANSACTION_START, qore_pgsql_rollback);
   methods.add(QDBI_METHOD_GET_SERVER_VERSION, qore_pgsql_get_server_version);

   DBID_PGSQL = DBI.registerDriver("pgsql", methods, pgsql_caps);

   // add pgsql_bind() function
   builtinFunctions.add("pgsql_bind", f_pgsql_bind);

   return NULL;
}

static void pgsql_module_ns_init(class Namespace *rns, class Namespace *qns)
{
   qns->addInitialNamespace(pgsql_ns->copy());
}

static void pgsql_module_delete()
{
   delete pgsql_ns;
}
