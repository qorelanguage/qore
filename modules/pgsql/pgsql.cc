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

static class QoreStringNode *pgsql_module_init();
static void pgsql_module_ns_init(class QoreNamespace *rns, class QoreNamespace *qns);
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

static class AbstractQoreNode *qore_pgsql_select_rows(class Datasource *ds, const QoreString *qstr, const QoreListNode *args, class ExceptionSink *xsink)
{
   QorePGConnection *pc = (QorePGConnection *)ds->getPrivateData();

   return pc->select_rows(ds, qstr, args, xsink);
}

static class AbstractQoreNode *qore_pgsql_select(class Datasource *ds, const QoreString *qstr, const QoreListNode *args, class ExceptionSink *xsink)
{
   QorePGConnection *pc = (QorePGConnection *)ds->getPrivateData();

   return pc->select(ds, qstr, args, xsink);
}

static class AbstractQoreNode *qore_pgsql_exec(class Datasource *ds, const QoreString *qstr, const QoreListNode *args, class ExceptionSink *xsink)
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
      const QoreEncoding *enc = QorePGMapper::getQoreEncoding(ds->getDBEncoding());
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

static class AbstractQoreNode *qore_pgsql_get_server_version(class Datasource *ds, class ExceptionSink *xsink)
{
   QorePGConnection *pc = (QorePGConnection *)ds->getPrivateData();
   return new QoreBigIntNode(pc->get_server_version());
}

static class AbstractQoreNode *f_pgsql_bind(const QoreListNode *params, class ExceptionSink *xsink)
{
   class AbstractQoreNode *p = get_param(params, 0);
   int type = p? p->getAsInt() : 0;
   if (!type)
   {
      xsink->raiseException("PGSQL-BIND-ERROR", "expecting OID (type number) as first parameter to pgsql_bind()");
      return NULL;
   }
   p = get_param(params, 1);
   class QoreHashNode *h = new QoreHashNode();
   h->setKeyValue("^pgtype^", new QoreBigIntNode(type), xsink);
   h->setKeyValue("^value^", p ? p->RefSelf() : NULL, xsink);
   return h;
}

static class QoreNamespace *pgsql_ns;

static void init_namespace()
{
   pgsql_ns = new QoreNamespace("PGSQL");

   pgsql_ns->addConstant("PG_TYPE_BOOL",                new QoreBigIntNode(BOOLOID));
   pgsql_ns->addConstant("PG_TYPE_BYTEA",               new QoreBigIntNode(BYTEAOID));
   pgsql_ns->addConstant("PG_TYPE_CHAR",                new QoreBigIntNode(CHAROID));
   pgsql_ns->addConstant("PG_TYPE_NAME",                new QoreBigIntNode(NAMEOID));
   pgsql_ns->addConstant("PG_TYPE_INT8",                new QoreBigIntNode(INT8OID));
   pgsql_ns->addConstant("PG_TYPE_INT2",                new QoreBigIntNode(INT2OID));
   pgsql_ns->addConstant("PG_TYPE_INT2VECTOR",          new QoreBigIntNode(INT2VECTOROID));
   pgsql_ns->addConstant("PG_TYPE_INT4",                new QoreBigIntNode(INT4OID));
   pgsql_ns->addConstant("PG_TYPE_REGPROC",             new QoreBigIntNode(REGPROCOID));
   pgsql_ns->addConstant("PG_TYPE_TEXT",                new QoreBigIntNode(TEXTOID));
   pgsql_ns->addConstant("PG_TYPE_OID",                 new QoreBigIntNode(OIDOID));
   pgsql_ns->addConstant("PG_TYPE_TID",                 new QoreBigIntNode(TIDOID));
   pgsql_ns->addConstant("PG_TYPE_XID",                 new QoreBigIntNode(XIDOID));
   pgsql_ns->addConstant("PG_TYPE_CID",                 new QoreBigIntNode(CIDOID));
   pgsql_ns->addConstant("PG_TYPE_VECTOROID",           new QoreBigIntNode(OIDVECTOROID));
#ifdef PG_TYPE_RELTYPE_OID
   pgsql_ns->addConstant("PG_TYPE_TYPE_RELTYPE",        new QoreBigIntNode(PG_TYPE_RELTYPE_OID));
#endif
#ifdef PG_ATTRIBUTE_RELTYPE_OID
   pgsql_ns->addConstant("PG_TYPE_ATTRIBUTE_RELTYPE",   new QoreBigIntNode(PG_ATTRIBUTE_RELTYPE_OID));
#endif
#ifdef PG_PROC_RELTYPE_OID
   pgsql_ns->addConstant("PG_TYPE_PROC_RELTYPE",        new QoreBigIntNode(PG_PROC_RELTYPE_OID));
#endif
#ifdef PG_CLASS_RELTYPE_OID
   pgsql_ns->addConstant("PG_TYPE_CLASS_RELTYPE",       new QoreBigIntNode(PG_CLASS_RELTYPE_OID));
#endif
   pgsql_ns->addConstant("PG_TYPE_POINT",               new QoreBigIntNode(POINTOID));
   pgsql_ns->addConstant("PG_TYPE_LSEG",                new QoreBigIntNode(LSEGOID));
   pgsql_ns->addConstant("PG_TYPE_PATH",                new QoreBigIntNode(PATHOID));
   pgsql_ns->addConstant("PG_TYPE_BOX",                 new QoreBigIntNode(BOXOID));
   pgsql_ns->addConstant("PG_TYPE_POLYGON",             new QoreBigIntNode(POLYGONOID));
   pgsql_ns->addConstant("PG_TYPE_LINE",                new QoreBigIntNode(LINEOID));
   pgsql_ns->addConstant("PG_TYPE_FLOAT4",              new QoreBigIntNode(FLOAT4OID));
   pgsql_ns->addConstant("PG_TYPE_FLOAT8",              new QoreBigIntNode(FLOAT8OID));
   pgsql_ns->addConstant("PG_TYPE_ABSTIME",             new QoreBigIntNode(ABSTIMEOID));
   pgsql_ns->addConstant("PG_TYPE_RELTIME",             new QoreBigIntNode(RELTIMEOID));
   pgsql_ns->addConstant("PG_TYPE_TINTERVAL",           new QoreBigIntNode(TINTERVALOID));
   pgsql_ns->addConstant("PG_TYPE_UNKNOWN",             new QoreBigIntNode(UNKNOWNOID));
   pgsql_ns->addConstant("PG_TYPE_CIRCLE",              new QoreBigIntNode(CIRCLEOID));
   pgsql_ns->addConstant("PG_TYPE_CASH",                new QoreBigIntNode(CASHOID));
   pgsql_ns->addConstant("PG_TYPE_MACADDR",             new QoreBigIntNode(MACADDROID));
   pgsql_ns->addConstant("PG_TYPE_INET",                new QoreBigIntNode(INETOID));
   pgsql_ns->addConstant("PG_TYPE_CIDR",                new QoreBigIntNode(CIDROID));
   pgsql_ns->addConstant("PG_TYPE_ACLITEM",             new QoreBigIntNode(ACLITEMOID));
   pgsql_ns->addConstant("PG_TYPE_BPCHAR",              new QoreBigIntNode(BPCHAROID));
   pgsql_ns->addConstant("PG_TYPE_VARCHAR",             new QoreBigIntNode(VARCHAROID));
   pgsql_ns->addConstant("PG_TYPE_DATE",                new QoreBigIntNode(DATEOID));
   pgsql_ns->addConstant("PG_TYPE_TIME",                new QoreBigIntNode(TIMEOID));
   pgsql_ns->addConstant("PG_TYPE_TIMESTAMP",           new QoreBigIntNode(TIMESTAMPOID));
   pgsql_ns->addConstant("PG_TYPE_TIMESTAMPTZ",         new QoreBigIntNode(TIMESTAMPTZOID));
   pgsql_ns->addConstant("PG_TYPE_INTERVAL",            new QoreBigIntNode(INTERVALOID));
   pgsql_ns->addConstant("PG_TYPE_TIMETZ",              new QoreBigIntNode(TIMETZOID));
   pgsql_ns->addConstant("PG_TYPE_BIT",                 new QoreBigIntNode(BITOID));
   pgsql_ns->addConstant("PG_TYPE_VARBIT",              new QoreBigIntNode(VARBITOID));
   pgsql_ns->addConstant("PG_TYPE_NUMERIC",             new QoreBigIntNode(NUMERICOID));
   pgsql_ns->addConstant("PG_TYPE_REFCURSOR",           new QoreBigIntNode(REFCURSOROID));
   pgsql_ns->addConstant("PG_TYPE_REGPROCEDURE",        new QoreBigIntNode(REGPROCEDUREOID));
   pgsql_ns->addConstant("PG_TYPE_REGOPER",             new QoreBigIntNode(REGOPEROID));
   pgsql_ns->addConstant("PG_TYPE_REGOPERATOR",         new QoreBigIntNode(REGOPERATOROID));
   pgsql_ns->addConstant("PG_TYPE_REGCLASS",            new QoreBigIntNode(REGCLASSOID));
   pgsql_ns->addConstant("PG_TYPE_REGTYPE",             new QoreBigIntNode(REGTYPEOID));
   pgsql_ns->addConstant("PG_TYPE_RECORD",              new QoreBigIntNode(RECORDOID));
   pgsql_ns->addConstant("PG_TYPE_CSTRING",             new QoreBigIntNode(CSTRINGOID));
   pgsql_ns->addConstant("PG_TYPE_ANY",                 new QoreBigIntNode(ANYOID));
   pgsql_ns->addConstant("PG_TYPE_VOID",                new QoreBigIntNode(VOIDOID));
   pgsql_ns->addConstant("PG_TYPE_TRIGGER",             new QoreBigIntNode(TRIGGEROID));
   pgsql_ns->addConstant("PG_TYPE_LANGUAGE_HANDLER",    new QoreBigIntNode(LANGUAGE_HANDLEROID));
   pgsql_ns->addConstant("PG_TYPE_INTERNAL",            new QoreBigIntNode(INTERNALOID));
   pgsql_ns->addConstant("PG_TYPE_OPAQUE",              new QoreBigIntNode(OPAQUEOID));
   pgsql_ns->addConstant("PG_TYPE_ANYELEMENT",          new QoreBigIntNode(ANYELEMENTOID));

   // array types
   pgsql_ns->addConstant("PG_TYPE_INT4ARRAY",           new QoreBigIntNode(QPGT_INT4ARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_CIRCLEARRAY",         new QoreBigIntNode(QPGT_CIRCLEARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_MONEYARRAY",          new QoreBigIntNode(QPGT_MONEYARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_BOOLARRAY",           new QoreBigIntNode(QPGT_BOOLARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_BYTEAARRAY",          new QoreBigIntNode(QPGT_BYTEAARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_NAMEARRAY",           new QoreBigIntNode(QPGT_NAMEARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_INT2ARRAY",           new QoreBigIntNode(QPGT_INT2ARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_TEXTARRAY",           new QoreBigIntNode(QPGT_TEXTARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_OIDARRAY",            new QoreBigIntNode(QPGT_OIDARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_TIDARRAY",            new QoreBigIntNode(QPGT_TIDARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_XIDARRAY",            new QoreBigIntNode(QPGT_XIDARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_CIDARRAY",            new QoreBigIntNode(QPGT_CIDARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_BPCHARARRAY",         new QoreBigIntNode(QPGT_BPCHARARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_VARCHARARRAY",        new QoreBigIntNode(QPGT_VARCHARARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_INT8ARRAY",           new QoreBigIntNode(QPGT_INT8ARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_POINTARRAY",          new QoreBigIntNode(QPGT_POINTARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_LSEGARRAY",           new QoreBigIntNode(QPGT_LSEGARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_PATHARRAY",           new QoreBigIntNode(QPGT_PATHARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_BOXARRAY",            new QoreBigIntNode(QPGT_BOXARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_FLOAT4ARRAY",         new QoreBigIntNode(QPGT_FLOAT4ARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_FLOAT8ARRAY",         new QoreBigIntNode(QPGT_FLOAT8ARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_ABSTIMEARRAY",        new QoreBigIntNode(QPGT_ABSTIMEARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_RELTIMEARRAY",        new QoreBigIntNode(QPGT_RELTIMEARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_TINTERVALARRAY",      new QoreBigIntNode(QPGT_TINTERVALARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_POLYGONARRAY",        new QoreBigIntNode(QPGT_POLYGONARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_MACADDRARRAY",        new QoreBigIntNode(QPGT_MACADDRARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_INETARRAY",           new QoreBigIntNode(QPGT_INETARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_CIDRARRAY",           new QoreBigIntNode(QPGT_CIDRARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_TIMESTAMPARRAY",      new QoreBigIntNode(QPGT_TIMESTAMPARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_DATEARRAY",           new QoreBigIntNode(QPGT_DATEARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_TIMEARRAY",           new QoreBigIntNode(QPGT_TIMEARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_TIMESTAMPTZARRAY",    new QoreBigIntNode(QPGT_TIMESTAMPTZARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_INTERVALARRAY",       new QoreBigIntNode(QPGT_INTERVALARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_NUMERICARRAY",        new QoreBigIntNode(QPGT_NUMERICARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_TIMETZARRAY",         new QoreBigIntNode(QPGT_TIMETZARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_BITARRAY",            new QoreBigIntNode(QPGT_BITARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_VARBITARRAY",         new QoreBigIntNode(QPGT_VARBITARRAYOID));
   pgsql_ns->addConstant("PG_TYPE_ANYARRAY",            new QoreBigIntNode(ANYARRAYOID));
}

static class QoreStringNode *pgsql_module_init()
{
#ifdef HAVE_PQISTHREADSAFE
   if (!PQisthreadsafe())
      return QoreStringNode("cannot load pgsql module; the PostgreSQL library on this system is not thread-safe");
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

static void pgsql_module_ns_init(class QoreNamespace *rns, class QoreNamespace *qns)
{
   qns->addInitialNamespace(pgsql_ns->copy());
}

static void pgsql_module_delete()
{
   delete pgsql_ns;
}
