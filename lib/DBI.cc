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

DBIDriver::DBIDriver(char *nme, DBIDriverFunctions *funcs, int cps)
{
   name = nme;
   f = funcs;   
   caps = cps;
}

DBIDriver::~DBIDriver()
{
   delete f;
}

char *DBIDriver::getName() const
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
   return f->init(ds, xsink);
}

int DBIDriver::close(class Datasource *ds)
{
   return f->close(ds);
}

class QoreNode *DBIDriver::select(class Datasource *ds, class QoreString *sql, class List *args, class ExceptionSink *xsink)
{
   return f->select(ds, sql, args, xsink);
}

class QoreNode *DBIDriver::selectRows(class Datasource *ds, class QoreString *sql, class List *args, class ExceptionSink *xsink)
{
   return f->selectRows(ds, sql, args, xsink);
}

class QoreNode *DBIDriver::execSQL(class Datasource *ds, class QoreString *sql, class List *args, class ExceptionSink *xsink)
{
   return f->execSQL(ds, sql, args, xsink);
}

int DBIDriver::commit(class Datasource *ds, class ExceptionSink *xsink)
{
   return f->commit(ds, xsink);
}

int DBIDriver::rollback(class Datasource *ds, class ExceptionSink *xsink)
{
   return f->rollback(ds, xsink);
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

DBIDriver *DBIDriverList::find(char *name) const
{
   for (dbi_list_t::const_iterator i = begin(); i != end(); i++)
      if (!strcmp(name, (*i)->getName()))
	 return *i;

   return NULL;
}

class DBIDriver *DBIDriverList::registerDriver(char *name, DBIDriverFunctions *f, int caps)
{
   assert(!find(name));

   DBIDriver *dd = new DBIDriver(name, f, caps);
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

class Hash *parseDatasource(char *ds, class ExceptionSink *xsink)
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

void datasource_thread_lock_cleanup(void *ptr, class ExceptionSink *xsink)
{
   //printd(5, "datasource_thread_lock_cleanup(ds=%08p)\n", ptr);
   xsink->raiseException("DATASOURCE-LOCK-EXCEPTION", "TID %d terminated while in a transaction; transaction will be automatically rolled back and the lock released", gettid());
   class Datasource *ds = (Datasource *)ptr;
   ds->rollback(xsink);
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

   // datasource type constants
   SQLNS->addConstant("DSOracle",   new QoreNode("oracle"));
   SQLNS->addConstant("DSMySQL",    new QoreNode("mysql"));
   // the following have no drivers yet
   SQLNS->addConstant("DSPGSQL",    new QoreNode("pgsql"));
   SQLNS->addConstant("DSSybase",   new QoreNode("sybase"));
   SQLNS->addConstant("DSDB2",      new QoreNode("db2"));
   SQLNS->addConstant("DSInformix", new QoreNode("informix"));

   // for DBI driver capabilities
   SQLNS->addConstant("DBI_CAP_CHARSET_SUPPORT",        new QoreNode((int64)DBI_CAP_CHARSET_SUPPORT));
   SQLNS->addConstant("DBI_CAP_TRANSACTION_MANAGEMENT", new QoreNode((int64)DBI_CAP_TRANSACTION_MANAGEMENT));
   SQLNS->addConstant("DBI_CAP_STORED_PROCEDURES",      new QoreNode((int64)DBI_CAP_STORED_PROCEDURES));
   SQLNS->addConstant("DBI_CAP_LOB_SUPPORT",            new QoreNode((int64)DBI_CAP_LOB_SUPPORT));

   // for column types for binding
   SQLNS->addConstant("VARCHAR", new QoreNode("string"));
   SQLNS->addConstant("NUMBER",  new QoreNode("string"));
   SQLNS->addConstant("CLOB",    new QoreNode("clob"));
   SQLNS->addConstant("BLOB",    new QoreNode("binary"));
   SQLNS->addConstant("DATE",    new QoreNode("date"));

   return SQLNS;
}

