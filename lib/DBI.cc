/*
  DBI.cc

  Database Independent SQL Layer

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
#include <qore/QC_Datasource.h>
#include <qore/QC_DatasourcePool.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define NUM_DBI_CAPS 4

// global qore library class for DBI driver management
DLLEXPORT DBIDriverList DBI;

struct dbi_cap_hash
{
   int cap;
   char *desc;
};

struct dbi_cap_hash dbi_cap_list[] =
{ { DBI_CAP_CHARSET_SUPPORT,        "CharsetSupport" },
  { DBI_CAP_TRANSACTION_MANAGEMENT, "TransactionManagement" },
  { DBI_CAP_STORED_PROCEDURES,      "StoredProcedures" },
  { DBI_CAP_LOB_SUPPORT,            "LargeObjectSupport" },
};

DBIDriver::DBIDriver(const char *nme, dbi_method_list_t &methods, int cps)
{
   // add methods to internal data structure
   for (dbi_method_list_t::iterator i = methods.begin(), e = methods.end(); i != e; ++i)
   {
      assert((*i).first >= 0 && (*i).first < QDBI_VALID_CODES);
      switch ((*i).first)
      {
	 case QDBI_METHOD_OPEN:
	    assert(!f.open);
	    f.open = (q_dbi_open_t)(*i).second;
	    break;
	 case QDBI_METHOD_CLOSE:
	    assert(!f.close);
	    f.close = (q_dbi_close_t)(*i).second;
	    break;
	 case QDBI_METHOD_SELECT:
	    assert(!f.select);
	    f.select = (q_dbi_select_t)(*i).second;
	    break;
	 case QDBI_METHOD_SELECT_ROWS:
	    assert(!f.selectRows);
	    f.selectRows = (q_dbi_select_rows_t)(*i).second;
	    break;
	 case QDBI_METHOD_EXEC:
	    assert(!f.execSQL);
	    f.execSQL = (q_dbi_exec_t)(*i).second;
	    break;
	 case QDBI_METHOD_COMMIT:
	    assert(!f.commit);
	    f.commit = (q_dbi_commit_t)(*i).second;
	    break;
	 case QDBI_METHOD_ROLLBACK:
	    assert(!f.rollback);
	    f.rollback = (q_dbi_rollback_t)(*i).second;
	    break;
	 case QDBI_METHOD_BEGIN_TRANSACTION:
	    assert(!f.begin_transaction);
	    f.begin_transaction = (q_dbi_begin_transaction_t)(*i).second;
	    break;
	 case QDBI_METHOD_AUTO_COMMIT:
	    assert(!f.auto_commit);
	    f.auto_commit = (q_dbi_auto_commit_t)(*i).second;
	    break;
	 case QDBI_METHOD_ABORT_TRANSACTION_START:
	    assert(!f.abort_transaction_start);
	    f.abort_transaction_start = (q_dbi_abort_transaction_start_t)(*i).second;
	    break;
	 case QDBI_METHOD_GET_SERVER_VERSION:
	    assert(!f.get_server_version);
	    f.get_server_version = (q_dbi_get_server_version_t)(*i).second;
	    break;
	 case QDBI_METHOD_GET_CLIENT_VERSION:
	    assert(!f.get_client_version);
	    f.get_client_version = (q_dbi_get_client_version_t)(*i).second;
	    break;
#ifdef DEBUG
	 default:
	    assert(false);
#endif
      }
   }
   // ensure minimum methods are defined
   assert(f.open);
   assert(f.close);
   assert(f.select);
   assert(f.selectRows);
   assert(f.execSQL);
   assert(f.commit);
   assert(f.rollback);
   
   name = nme;
   caps = cps;
}

DBIDriver::~DBIDriver()
{
}

const char *DBIDriver::getName() const
{
   return name;
}

int DBIDriver::getCaps() const
{
   return caps;
}

List *DBIDriver::getCapList() const
{
   List *l = new List();
   for (int i = 0; i < NUM_DBI_CAPS; i++)
      if (caps & dbi_cap_list[i].cap)
	 l->push(new QoreNode(dbi_cap_list[i].desc));
   return l;
}

int DBIDriver::init(class Datasource *ds, class ExceptionSink *xsink)
{
   return f.open(ds, xsink);
}

int DBIDriver::close(class Datasource *ds)
{
   return f.close(ds);
}

class QoreNode *DBIDriver::select(class Datasource *ds, class QoreString *sql, class List *args, class ExceptionSink *xsink)
{
   return f.select(ds, sql, args, xsink);
}

class QoreNode *DBIDriver::selectRows(class Datasource *ds, class QoreString *sql, class List *args, class ExceptionSink *xsink)
{
   return f.selectRows(ds, sql, args, xsink);
}

class QoreNode *DBIDriver::execSQL(class Datasource *ds, class QoreString *sql, class List *args, class ExceptionSink *xsink)
{
   return f.execSQL(ds, sql, args, xsink);
}

int DBIDriver::commit(class Datasource *ds, class ExceptionSink *xsink)
{
   return f.commit(ds, xsink);
}

int DBIDriver::rollback(class Datasource *ds, class ExceptionSink *xsink)
{
   return f.rollback(ds, xsink);
}

int DBIDriver::beginTransaction(class Datasource *ds, class ExceptionSink *xsink)
{
   if (f.begin_transaction)
      return f.begin_transaction(ds, xsink);
   return 0; // 0 = OK
}

int DBIDriver::autoCommit(class Datasource *ds, class ExceptionSink *xsink)
{
   if (f.auto_commit)
      return f.auto_commit(ds, xsink);
   return 0; // 0 = OK
}

int DBIDriver::abortTransactionStart(class Datasource *ds, class ExceptionSink *xsink)
{
   if (f.abort_transaction_start)
      return f.abort_transaction_start(ds, xsink);
   return 0; // 0 = OK
}

class QoreNode *DBIDriver::getServerVersion(class Datasource *ds, class ExceptionSink *xsink)
{
   if (f.get_server_version)
      return f.get_server_version(ds, xsink);
   return 0;
}

class QoreNode *DBIDriver::getClientVersion(class Datasource *ds, class ExceptionSink *xsink)
{
   if (f.get_client_version)
      return f.get_client_version(ds, xsink);
   return 0;
}

DBIDriverList::~DBIDriverList()
{
   dbi_list_t::iterator i;
   while ((i = begin()) != end())
   {
      class DBIDriver *driv = *i;
      erase(i);
      delete driv;
   }
}

DBIDriver *DBIDriverList::find_intern(const char *name) const
{
   for (dbi_list_t::const_iterator i = begin(); i != end(); i++)
      if (!strcmp(name, (*i)->getName()))
	 return *i;

   return NULL;
}

DBIDriver *DBIDriverList::find(const char *name) const
{
   DBIDriver *d = find_intern(name);
   if (d)
      return d;

   // to to load the driver if it doesn't exist
   // ignore any exceptions
   ExceptionSink xs;
   MM.runTimeLoadModule(name, &xs);
   xs.clear();

   return find_intern(name);
}

class DBIDriver *DBIDriverList::registerDriver(const char *name, dbi_method_list_t &methods, int caps)
{
   assert(!find_intern(name));
   
   DBIDriver *dd = new DBIDriver(name, methods, caps);
   push_back(dd);
   return dd;
}

class List *DBIDriverList::getDriverList() const
{
   if (empty())
      return NULL;

   class List *l = new List();
   
   for (dbi_list_t::const_iterator i = begin(); i != end(); i++)
      l->push(new QoreNode((*i)->getName()));

   return l;
}

/*
  parseDatasource()

  parses strings of the form: driver:user/pass@db(charset)%host

  driver, charset, and host are optional
*/

class Hash *parseDatasource(const char *ds, class ExceptionSink *xsink)
{
   if (!ds || !ds[0])
      return NULL;

   char *str = strdup(ds);
   char *ostr = str;
   
   class Hash *h = new Hash();
   char *p = strchr(str, ':');
   if (p)
   {
      *p = '\0';
      h->setKeyValue("type", new QoreNode(str), NULL);
      str = p + 1;
   }

   bool has_pass = false;
   p = strchr(str, '/');
   if (p)
   {
      *p = '\0';
      h->setKeyValue("user", new QoreNode(str), NULL);
      str = p + 1;
      has_pass = true;
   }

   p = strrchr(str, '@');
   if (!p)
   {
      free(ostr);
      xsink->raiseException("DATASOURCE-PARSE-ERROR", "missing database name delimited by '@' in '%s'", ds);
      h->derefAndDelete(xsink);
      return NULL;
   }

   *p = '\0';
   if (p != str)
   {
      if (has_pass)
	 h->setKeyValue("pass", new QoreNode(str), NULL);
      else
	 h->setKeyValue("user", new QoreNode(str), NULL);
   }
   str = p + 1;

   char *db = str;
   p = strchr(str, '(');
   if (p)
   {
      char *end = strchr(p, ')');
      if (!end) 
      {
	 free(ostr);
	 xsink->raiseException("DATASOURCE-PARSE-ERROR", "missing closing parenthesis in charset specification in '%s'", ds);
	 h->derefAndDelete(xsink);
	 return NULL;
      }
      *p = '\0';  // terminate for db
      *end = '\0';  // terminate charset
      p++;
      h->setKeyValue("charset", new QoreNode(p), NULL);
      str = end + 1;
   }
   
   p = strchr(str, '%');
   if (p)
   {
      *p = '\0';
      p++;
      if (!*p)
      {
	 free(ostr);
	 xsink->raiseException("DATASOURCE-PARSE-ERROR", "missing hostname string after '%' delimeter in '%s'", ds);
	 h->derefAndDelete(xsink);
	 return NULL;
      }
      h->setKeyValue("host", new QoreNode(p), NULL);
   }

   h->setKeyValue("db", new QoreNode(db), NULL);
   free(ostr);
   return h;
}

class QoreNode *f_parseDatasource(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0;

   if (!(p0 = test_param(params, NT_STRING, 0)))
      return NULL;

   class Hash *h = parseDatasource(p0->val.String->getBuffer(), xsink);

   return h ? new QoreNode(h) : NULL;
}

class QoreNode *f_getDBIDriverList(class QoreNode *params, ExceptionSink *xsink)
{
   class List *l = DBI.getDriverList();
   if (l)
      return new QoreNode(l);
   
   return NULL;
}

class QoreNode *f_getDBIDriverCapabilityList(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0;

   if (!(p0 = test_param(params, NT_STRING, 0)))
      return NULL;

   DBIDriver *dd = DBI.find(p0->val.String->getBuffer());
   if (!dd)
      return NULL;

   List *l = dd->getCapList();

   if (l)
      return new QoreNode(l);

   return NULL;
}

class QoreNode *f_getDBIDriverCapabilities(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0;

   if (!(p0 = test_param(params, NT_STRING, 0)))
      return NULL;

   DBIDriver *dd = DBI.find(p0->val.String->getBuffer());
   if (!dd)
      return NULL;

   return new QoreNode(NT_INT, dd->getCaps());
}

void init_dbi_functions()
{
   builtinFunctions.add("getDBIDriverList", f_getDBIDriverList);
   builtinFunctions.add("getDBIDriverCapabilityList", f_getDBIDriverCapabilityList);
   builtinFunctions.add("getDBIDriverCapabilities", f_getDBIDriverCapabilities);
   builtinFunctions.add("parseDatasource", f_parseDatasource);
}

class Namespace *getSQLNamespace()
{
   // create Qore::SQL namespace
   class Namespace *SQLNS = new Namespace("SQL");

   SQLNS->addSystemClass(initDatasourceClass());
   SQLNS->addSystemClass(initDatasourcePoolClass());

   // datasource type constants
   SQLNS->addConstant("DSOracle",   new QoreNode("oracle"));
   SQLNS->addConstant("DSMySQL",    new QoreNode("mysql"));
   SQLNS->addConstant("DSSybase",   new QoreNode("sybase"));
   SQLNS->addConstant("DSPGSQL",    new QoreNode("pgsql"));
   SQLNS->addConstant("DSMSSQL",    new QoreNode("mssql"));
   // the following have no drivers yet
   SQLNS->addConstant("DSDB2",      new QoreNode("db2"));
   SQLNS->addConstant("DSInformix", new QoreNode("informix"));

   // for DBI driver capabilities
   SQLNS->addConstant("DBI_CAP_CHARSET_SUPPORT",        new QoreNode((int64)DBI_CAP_CHARSET_SUPPORT));
   SQLNS->addConstant("DBI_CAP_TRANSACTION_MANAGEMENT", new QoreNode((int64)DBI_CAP_TRANSACTION_MANAGEMENT));
   SQLNS->addConstant("DBI_CAP_STORED_PROCEDURES",      new QoreNode((int64)DBI_CAP_STORED_PROCEDURES));
   SQLNS->addConstant("DBI_CAP_LOB_SUPPORT",            new QoreNode((int64)DBI_CAP_LOB_SUPPORT));
   SQLNS->addConstant("DBI_CAP_BIND_BY_VALUE",          new QoreNode((int64)DBI_CAP_BIND_BY_VALUE));
   SQLNS->addConstant("DBI_CAP_BIND_BY_PLACEHOLDER",    new QoreNode((int64)DBI_CAP_BIND_BY_PLACEHOLDER));

   // for column types for binding
   SQLNS->addConstant("VARCHAR", new QoreNode("string"));
   SQLNS->addConstant("NUMBER",  new QoreNode("string"));
   SQLNS->addConstant("CLOB",    new QoreNode("clob"));
   SQLNS->addConstant("BLOB",    new QoreNode("blob"));
   SQLNS->addConstant("DATE",    new QoreNode("date"));

   return SQLNS;
}

