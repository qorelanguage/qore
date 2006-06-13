/*
  mysql.cc

  MySQL Interface to Qore DBI layer

  Qore Programming Language

  0.4.0 changes: 
  * multi-threaded access added
  * transaction management added
  * character set support added

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

#include <qore/config.h>
#include <qore/support.h>
#include <qore/QoreNode.h>
#include <qore/QoreType.h>
#include <qore/Exception.h>
#include <qore/Object.h>
#include <qore/QoreString.h>
#include <qore/QoreClass.h>
#include <qore/charset.h>
#include <qore/LockedObject.h>
#include <qore/QC_Datasource.h>
#include <qore/DBI.h>
#include <qore/module.h>
#include <qore/ModuleManager.h>

#include "qore-mysql-module.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <strings.h>

#include <mysql.h>

#ifndef QORE_MONOLITHIC
char qore_module_name[] = "mysql";
char qore_module_version[] = "0.2";
char qore_module_description[] = "MySQL database driver";
char qore_module_author[] = "David Nichols";
char qore_module_url[] = "http://qore.sourceforge.net";
int qore_module_api_major = QORE_MODULE_API_MAJOR;
int qore_module_api_minor = QORE_MODULE_API_MINOR;
qore_module_init_t qore_module_init = qore_mysql_module_init;
qore_module_ns_init_t qore_module_ns_init = qore_mysql_module_ns_init;
qore_module_delete_t qore_module_delete = qore_mysql_module_delete;
#endif

// driver capabilities
// FIXME: implement support for newer MySQL features!
static int mysql_caps = DBI_CAP_NONE
#ifdef HAVE_MYSQL_COMMIT
   | DBI_CAP_TRANSACTION_MANAGEMENT
#endif
#ifdef HAVE_MYSQL_SET_CHARACTER_SET
   | DBI_CAP_CHARSET_SUPPORT
#endif
;

class DBIDriver *DBID_MYSQL = NULL;

// this is the thread key that will tell us if the 
static pthread_key_t ptk_mysql;

class MySQLData {
   public:
      MYSQL *db;
      LockedObject lck;

      MySQLData(MYSQL *d) { db = d; }
};

static struct mapEntry {
      char *qore;
      char *mysql;
      class QoreEncoding *id;
} mapList[] = 
{
   { "utf-8", "utf8", QCS_UTF8 },
   { "iso-8859-1", "latin1", QCS_ISO_8859_1 },
   { "iso-8859-2", "latin2", QCS_ISO_8859_2 },
   { "us-ascii", "ascii", QCS_USASCII },
   { "koi8-r", "koi8r", QCS_KOI8_R },
   { "koi8-u", "koi8u", QCS_KOI8_U },
   { "iso-8859-7", "greek", QCS_ISO_8859_7 },
   { "iso-8859-8", "hebrew", QCS_ISO_8859_8 },
   { "iso-8858-9", "latin5", QCS_ISO_8859_9 },
   { "iso-8859-13", "latin7", QCS_ISO_8859_13 },
   //{ "", "big5_chinese_ci" },
   //{ "", "dec8_swedish_ci" },
   //{ "", "cp850_general_ci" },
   //{ "", "hp8_english_ci" },
   //{ "", "swe7_swedish_ci" },
   //{ "", "ujis_japanese_ci" },
   //{ "", "sjis_japanese_ci" },
   //{ "", "tis620_thai_ci" },
   //{ "", "euckr_korean_ci" },
   //{ "", "gb2312_chinese_ci" },
   //{ "", "cp1250_general_ci" },
   //{ "", "gbk_chinese_ci" },
   //{ "", "armscii8_general_ci" },
   //{ "", "ucs2_general_ci" },
   //{ "", "cp866_general_ci" },
   //{ "", "keybcs2_general_ci" },
   //{ "", "macce_general_ci" },
   //{ "", "macroman_general_ci" },
   //{ "", "cp852_general_ci" },
   //{ "", "cp1251_general_ci" },
   //{ "", "cp1256_general_ci" }, iso-8859-6 ?
   //{ "", "cp1257_general_ci" },
   //{ "", "binary" },
   //{ "", "geostd8_general_ci" },
   //{ "", "cp932_japanese_ci" },
};

#define NUM_CHARMAPS (sizeof(mapList) / sizeof(struct mapEntry))

static class QoreEncoding *get_qore_cs(char *cs)
{
   int end;
   // get end of charset name
   char *p = index(cs, '_');
   if (p)
      end = p - cs;
   else
      end = strlen(cs);

   for (unsigned i = 0; i < NUM_CHARMAPS; i++)
      if (!strncasecmp(cs, mapList[i].mysql, end))
	 return mapList[i].id;

   QoreString cset;
   cset.concat(cs, end);
   return QEM.findCreate(&cset);
}

static char *get_mysql_cs(class QoreEncoding *id)
{
   for (unsigned i = 0; i < NUM_CHARMAPS; i++)
      if (mapList[i].id == id)
	 return mapList[i].mysql;

   return NULL;
}

static inline void checkInit()
{
   if (!pthread_getspecific(ptk_mysql))
   {
      mysql_thread_init();
      pthread_setspecific(ptk_mysql, (void *)1);
   }
}

static void mysql_thread_cleanup(void *unused)
{
   if (pthread_getspecific(ptk_mysql))
      mysql_thread_end();
}

static int qore_mysql_init(Datasource *ds, ExceptionSink *xsink)
{
   tracein("qore_mysql_init()");

   printd(5, "qore_mysql_init() datasource %08x for DB=%s\n", ds, 
	  ds->dbname ? ds->dbname : "unknown");

   if (!ds->dbname)
   {
      xsink->raiseException("DATASOURCE-MISSING-DBNAME", "Datasource has an empty dbname parameter");
      traceout("qore_mysql_init()");
      return -1;
   }

   if (ds->charset)
      ds->qorecharset = get_qore_cs(ds->charset);
   else
   {
      ds->qorecharset = QCS_DEFAULT;
      ds->charset = get_mysql_cs(QCS_DEFAULT);

      if (!ds->charset)
      {
	 xsink->raiseException("DBI:MYSQL:UNKNOWN-CHARACTER-SET", "cannot find the mysql character set equivalent for '%s'", QCS_DEFAULT->code);
	 traceout("qore_mysql_init()");
	 return -1;
      }
      ds->charset = strdup(ds->charset);
   }

   printd(3, "qore_mysql_init(): user=%s pass=%s db=%s (charset=%s)\n",
	  ds->username, ds->password, ds->dbname, ds->charset ? ds->charset : "(none)");

   MYSQL *db = mysql_init(NULL);
   if (!mysql_real_connect(db, ds->hostname, ds->username, ds->password, ds->dbname, 0, NULL, 0))
   {
      xsink->raiseException("DBI:MYSQL:CONNECT-ERROR", "%s", mysql_error(db));
      traceout("qore_mysql_init()");
      return -1;
   }

   class MySQLData *d_mysql = new MySQLData(db);
   ds->private_data = (void *)d_mysql;

#ifdef HAVE_MYSQL_SET_CHARACTER_SET
   // set character set
   mysql_set_character_set(db, ds->charset);
#endif

#ifdef HAVE_MYSQL_COMMIT
   // autocommits are handled by this driver, not by MySQL
   mysql_autocommit(db, false);

   // set transaction handling
   if (mysql_query(db, "set transaction isolation level read committed"))
   {
      xsink->raiseException("DBI:MYSQL:INIT-ERROR", (char *)mysql_error(db));
      traceout("qore_mysql_init()");
      return -1;
   }

#endif

   traceout("qore_mysql_init()");
   return 0;
}

static int qore_mysql_commit(class Datasource *ds, ExceptionSink *xsink)
{
   tracein("qore_mysql_commit()");

#ifdef HAVE_MYSQL_COMMIT
   checkInit();
   MYSQL *db = ((MySQLData *)ds->private_data)->db;
   if (mysql_commit(db))
      xsink->raiseException("DBI:MYSQL:COMMIT-ERROR", (char *)mysql_error(db));
#else
   xsink->raiseException("DBI:MYSQL:NOT-IMPLEMENTED", "this version of the MySQL client API does not support transaction management");
#endif
   traceout("qore_mysql_commit()");
   return 0;
}

static int qore_mysql_rollback(class Datasource *ds, ExceptionSink *xsink)
{
   tracein("qore_mysql_rollback()");

#ifdef HAVE_MYSQL_COMMIT
   checkInit();
   MYSQL *db = ((MySQLData *)ds->private_data)->db;
   if (mysql_rollback(db))
      xsink->raiseException("DBI:MYSQL:ROLLBACK-ERROR", (char *)mysql_error(db));
#else
   xsink->raiseException("DBI:MYSQL:NOT-IMPLEMENTED", "this version of the MySQL client API does not support transaction management");
#endif

   traceout("qore_mysql_rollback()");
   return 0;
}

#ifdef HAVE_MYSQL_STMT
class MyResult {
   private:
      MYSQL_FIELD *field;
      int num_fields;
      int type;
      MYSQL_BIND *bindbuf;
      struct bindInfo {
	    my_bool mnull;
	    long unsigned int mlen;
      } *bi;

   public:
      inline MyResult(MYSQL_RES *res)
      {
	 field = mysql_fetch_fields(res);
	 num_fields = mysql_num_fields(res);
	 mysql_free_result(res);

	 bindbuf = NULL;
      }

      inline ~MyResult()
      {
	 if (bindbuf)
	 {
	    // delete buffer
	    for (int i = 0; i < num_fields; i++)
	       if (bindbuf[i].buffer_type == MYSQL_TYPE_DOUBLE || bindbuf[i].buffer_type == MYSQL_TYPE_LONGLONG)
		  free(bindbuf[i].buffer);
	       else if (bindbuf[i].buffer_type == MYSQL_TYPE_STRING)
		  delete [] (char *)bindbuf[i].buffer;
	       else if (bindbuf[i].buffer_type == MYSQL_TYPE_DATETIME)
		  delete (MYSQL_TIME *)bindbuf[i].buffer;
	    delete [] bindbuf;
	 }
      }

      inline char *getFieldName(int i)
      {
	 return field[i].name;
      }

      inline int getNumFields()
      {
	 return num_fields;
      }

      void bind(MYSQL_STMT *stmt)
      {
	 bindbuf = new MYSQL_BIND[num_fields];
	 bi      = new bindInfo[num_fields];

	 // zero out bind memory
	 memset(bindbuf, 0, sizeof(MYSQL_BIND) * num_fields);

	 for (int i = 0; i < num_fields; i++)
	 {
	    // setup bind structure
	    switch (field[i].type)
	    {
	       // for integer values
	       case FIELD_TYPE_DECIMAL:
	       case FIELD_TYPE_SHORT:
	       case FIELD_TYPE_LONG:
	       case FIELD_TYPE_LONGLONG:
	       case FIELD_TYPE_INT24:
	       case FIELD_TYPE_TINY:
		  bindbuf[i].buffer_type = MYSQL_TYPE_LONGLONG;
		  bindbuf[i].buffer = malloc(sizeof(int64));
		  break;
		  
		  // for floating point values
	       case FIELD_TYPE_FLOAT:
	       case FIELD_TYPE_DOUBLE:
		  bindbuf[i].buffer_type = MYSQL_TYPE_DOUBLE;
		  bindbuf[i].buffer = malloc(sizeof(double));
		  break;
		  
		  // for datetime values
	       case FIELD_TYPE_DATETIME:
	       case FIELD_TYPE_DATE:
	       case FIELD_TYPE_TIME:
	       case FIELD_TYPE_TIMESTAMP:
		  bindbuf[i].buffer_type = MYSQL_TYPE_DATETIME;
		  bindbuf[i].buffer = new MYSQL_TIME;
		  break;
		  
		  // for all other types (treated as string)
	       default:
		  bindbuf[i].buffer_type = MYSQL_TYPE_STRING;
		  bindbuf[i].buffer = new char[field[i].length + 1];
		  bindbuf[i].buffer_length = field[i].length + 1;
		  break;
	    }
	    bindbuf[i].is_null = &bi[i].mnull;
	    bindbuf[i].length = &bi[i].mlen;
	 }

	 // FIXME: check for errors here
	 mysql_stmt_bind_result(stmt, bindbuf);
      }

      class QoreNode *getBoundColumnValue(class QoreEncoding *csid, int i)
      {
	 class QoreNode *n = NULL;
	 
	 if (bi[i].mnull)
	    n = null();
	 else if (bindbuf[i].buffer_type == MYSQL_TYPE_LONGLONG)
	    n = new QoreNode(*((int64 *)bindbuf[i].buffer));
	 else if (bindbuf[i].buffer_type == MYSQL_TYPE_DOUBLE)
	    n = new QoreNode(*((double *)bindbuf[i].buffer));
	 else if (bindbuf[i].buffer_type == MYSQL_TYPE_STRING)
	 {
	    //printf("string (%d): '%s'\n", mlen[i], (char *)bindbuf[i].buffer);
	    n = new QoreNode(new QoreString((char *)bindbuf[i].buffer, csid));
	 }
	 else if (bindbuf[i].buffer_type == MYSQL_TYPE_DATETIME)
	 {
	    MYSQL_TIME *t = (MYSQL_TIME *)bindbuf[i].buffer;
	    n = new QoreNode(new DateTime(t->year, t->month, t->day, t->hour, t->minute, t->second));
	 }
	 
	 return n;
      }
};

static class QoreNode *qore_mysql_do_sql(class Datasource *ds, QoreString *qstr, ExceptionSink *xsink)
{
   tracein("qore_mysql_do_sql()");

   // convert string if necessary
   class QoreString *tqstr;

   if (qstr->getEncoding() && qstr->getEncoding() != ds->qorecharset && ds->qorecharset)
   {
      tqstr = qstr->convertEncoding(ds->qorecharset, xsink);
      if (xsink->isEvent())
      {
	 traceout("qore_mysql_do_sql()");
         return NULL;
      }
   }
   else
      tqstr = qstr;

   MySQLData *d_mysql =(MySQLData *)ds->private_data;
   MYSQL *db = d_mysql->db;
   
   MYSQL_STMT *stmt = mysql_stmt_init(db);

   class QoreNode *rv = NULL;

   if (!mysql_stmt_prepare(stmt, tqstr->getBuffer(), tqstr->strlen()))
   {
      MYSQL_RES *res = mysql_stmt_result_metadata(stmt);
      if (res)
      {
	 class MyResult myres(res);

	 if (!mysql_stmt_execute(stmt))
	 {
	    Hash *h = new Hash();

	    for (int i = 0; i < myres.getNumFields(); i++)
	       h->setKeyValue(myres.getFieldName(i), new QoreNode(new List()), NULL);

	    if (mysql_stmt_affected_rows(stmt))
	    {
	       myres.bind(stmt);

	       while (!mysql_stmt_fetch(stmt))
		  for (int i = 0; i < myres.getNumFields(); i++)
		     h->getKeyValue(myres.getFieldName(i))->val.list->push(myres.getBoundColumnValue(ds->qorecharset, i));
	    }
	    rv = new QoreNode(h);
	 }
	 else
	    xsink->raiseException("DBI:MYSQL:ERROR", (char *)mysql_error(db));
      }
      else
      {
	 if (!mysql_stmt_execute(stmt))
	    rv = new QoreNode((int64)mysql_stmt_affected_rows(stmt));
	 else
	    xsink->raiseException("DBI:MYSQL:ERROR", (char *)mysql_error(db));
      }
   }
   else
      xsink->raiseException("DBI:MYSQL:ERROR", (char *)mysql_error(db));

   mysql_stmt_close(stmt);

   if (tqstr != qstr)
      delete tqstr;

   traceout("qore_mysql_do_sql()");
   return rv;
}

static class QoreNode *qore_mysql_do_sql_horizontal(class Datasource *ds, QoreString *qstr, ExceptionSink *xsink)
{
   tracein("qore_mysql_select_horizontal()");

   // convert string if necessary
   class QoreString *tqstr;

   if (qstr->getEncoding() && qstr->getEncoding() != ds->qorecharset  && ds->qorecharset)
   {
      tqstr = qstr->convertEncoding(ds->qorecharset, xsink);
      if (xsink->isEvent())
      {
	 traceout("qore_mysql_select_horizontal()");
         return NULL;
      }
   }
   else
      tqstr = qstr;

   MySQLData *d_mysql =(MySQLData *)ds->private_data;
   MYSQL *db = d_mysql->db;
   
   MYSQL_STMT *stmt = mysql_stmt_init(db);

   class QoreNode *rv = NULL;

   if (!mysql_stmt_prepare(stmt, tqstr->getBuffer(), tqstr->strlen()))
   {
      MYSQL_RES *res = mysql_stmt_result_metadata(stmt);
      if (res)
      {
	 class MyResult myres(res);

	 if (!mysql_stmt_execute(stmt))
	 {
	    List *l = new List();

	    if (mysql_stmt_affected_rows(stmt))
	    {
	       myres.bind(stmt);

	       while (!mysql_stmt_fetch(stmt))
	       {
		  class Hash *h = new Hash();

		  for (int i = 0; i < myres.getNumFields(); i++)
		     h->setKeyValue(myres.getFieldName(i), myres.getBoundColumnValue(ds->qorecharset, i), NULL);

		  l->push(new QoreNode(h));
	       }
	    }

	    rv = new QoreNode(l);
	 }
	 else
	    xsink->raiseException("DBI:MYSQL:ERROR", (char *)mysql_error(db));
      }
      else
      {
	 if (!mysql_stmt_execute(stmt))
	    rv = new QoreNode((int64)mysql_stmt_affected_rows(stmt));
	 else
	    xsink->raiseException("DBI:MYSQL:ERROR", (char *)mysql_error(db));
      }
   }
   else
      xsink->raiseException("DBI:MYSQL:ERROR", (char *)mysql_error(db));

   mysql_stmt_close(stmt);

   if (tqstr != qstr)
      delete tqstr;

   traceout("qore_mysql_select_horizontal()");
   return rv;
}

#else  // !HAVE_MYSQL_STMT
static class Hash *get_result_set(class Datasource *ds, MYSQL_RES *res)
{
   MYSQL_ROW row;
   int num_fields = mysql_num_fields(res);
   class Hash *h = new Hash();
   
   // get column names and set up column lists
   MYSQL_FIELD *field = mysql_fetch_fields(res);
   
   for (int i = 0; i < num_fields; i++)
      h->setKeyValue(field[i].name, new QoreNode(new List()), NULL);
   
   int rn = 0;
   while ((row = mysql_fetch_row(res)))
   {
      for (int i = 0; i < num_fields; i++)
      {
	 QoreNode *n;
	 // some basic type checking
	 switch (field[i].type)
	 {
	    // for integer values
	    case FIELD_TYPE_DECIMAL:
	    case FIELD_TYPE_SHORT:
	    case FIELD_TYPE_LONG:
	    case FIELD_TYPE_INT24:
	    case FIELD_TYPE_TINY:
	       n = new QoreNode(NT_INT, atoi(row[i]));
	       break;
	       
	       // for floating point values
	    case FIELD_TYPE_FLOAT:
	    case FIELD_TYPE_DOUBLE:
	       n = new QoreNode(atof(row[i]));
	       break;
	       
	       // for datetime values
	    case FIELD_TYPE_DATETIME:
	    {
	       class DateTime *d = new DateTime();
	       row[i][4]  = '\0';
	       row[i][7]  = '\0';
	       row[i][10] = '\0';
	       row[i][13] = '\0';
	       row[i][16] = '\0';
	       d->year    = atoi(row[i]);
	       d->month   = atoi(row[i] + 5);
	       d->day     = atoi(row[i] + 8);
	       d->hour    = atoi(row[i] + 11);
	       d->minute  = atoi(row[i] + 14);
	       d->second  = atoi(row[i] + 17);
	       n = new QoreNode(d);
	       break;
	    }

	    // for date values
	    case FIELD_TYPE_DATE:
	    {
	       class DateTime *d = new DateTime();
	       row[i][4] = '\0';
	       row[i][7] = '\0';
	       d->year   = atoi(row[i]);
	       d->month  = atoi(row[i] + 5);
	       d->day    = atoi(row[i] + 8);
	       n = new QoreNode(d);
	       break;
	    }
	    
	    // for time values
	    case FIELD_TYPE_TIME:
	    {
	       class DateTime *d = new DateTime();
	       row[i][2] = '\0';
	       row[i][5] = '\0';
	       d->hour   = atoi(row[i]);
	       d->minute = atoi(row[i] + 3);
	       d->second = atoi(row[i] + 6);
	       n = new QoreNode(d);
	       break;
	    }

	    case FIELD_TYPE_TIMESTAMP:
	       n = new QoreNode(new DateTime(row[i]));
	       break;
	    
	    // the rest defaults to string
	    default:
	       n = new QoreNode(new QoreString(ds->qorecharset, row[i]));
	       break;
	 }
	 //printd(5, "get_result_set() row %d col %d: %s (type=%d)=\"%s\"\n", rn, i, field[i].name, field[i].type, row[i]);
	 h->getKeyValue(field[i].name)->val.list->push(n);
      }
      rn++;
   }
   return h;
}

static class QoreNode *qore_mysql_do_sql(class Datasource *ds, QoreString *qstr, ExceptionSink *xsink)
{
   tracein("qore_mysql_do_sql()");

   // convert string if necessary
   class QoreString *tqstr;

   if (qstr->getEncoding() && qstr->getEncoding() != ds->qorecharset && ds->qorecharset)
   {
      tqstr = qstr->convertEncoding(ds->qorecharset, xsink);
      if (xsink->isEvent())
         return NULL;
   }
   else
      tqstr = qstr;

   MySQLData *d_mysql =(MySQLData *)ds->private_data;
   MYSQL *db = d_mysql->db;
   
   d_mysql->lck.lock();
   if (mysql_query(db, qstr->getBuffer()))
   {
      xsink->raiseException("DBI:MYSQL:SELECT-ERROR", (char *)mysql_error(db));
      d_mysql->lck.unlock();
      if (tqstr != qstr)
         delete tqstr;
      return NULL;
   }

   class QoreNode *rv;
   if (mysql_field_count(db) > 0)
   {
      MYSQL_RES *res = mysql_store_result(db);
      d_mysql->lck.unlock();

      if (!res)
      {
	 xsink->raiseException("DBI:MYSQL:SELECT-ERROR", (char *)mysql_error(db));
	 if (tqstr != qstr)
	    delete tqstr;
	 return NULL;
      }
      class Hash *h = get_result_set(ds, res);
      rv = new QoreNode(h);
      mysql_free_result(res);
   }
   else
   {
      rv = new QoreNode(NT_INT, mysql_affected_rows(db));
      d_mysql->lck.unlock();
   }
#ifdef HAVE_MYSQL_COMMIT
   if (ds->getAutoCommit())
      mysql_commit(db);
#endif

   if (tqstr != qstr)
      delete tqstr;

   traceout("qore_mysql_do_sql()");
   return rv;
}

static class QoreNode *qore_mysql_do_sql_horizontal(class Datasource *ds, QoreString *qstr, ExceptionSink *xsink)
{
   xsink->raiseException("MYSQL-UNSUPPORTED", "row retrieval not yet implemented for old versions of MySQL without a prepared statement interface");
   return NULL;
}
#endif // HAVE_MYSQL_STMT

static class Hash *qore_mysql_describe(class Datasource *ds, char *table_name, ExceptionSink *xsink)
{
   tracein("qore_mysql_describe()");

   checkInit();
   xsink->raiseException("DBI:MYSQL:NOT-IMPLEMENTED", "sorry, not implemented yet");

   traceout("qore_mysql_describe()");
   return NULL;
}

static class QoreNode *qore_mysql_exec(class Datasource *ds, QoreString *qstr, class List *args, class ExceptionSink *xsink)
{
   tracein("qore_mysql_exec()");

   checkInit();
   QoreNode *rv = qore_mysql_do_sql(ds, qstr, xsink);

   traceout("qore_mysql_exec()");
   return rv;
}

static int qore_mysql_init_datasource(Datasource *ds, ExceptionSink *xsink)
{
   tracein("qore_mysql_init_datasource()");

   checkInit();

   int rc = qore_mysql_init(ds, xsink);

   traceout("qore_mysql_init_datasource()");
   return rc;
}

static int qore_mysql_close_datasource(class Datasource *ds)
{
   tracein("qore_mysql_close_datasource()");

   checkInit();

   class MySQLData *d_mysql = (MySQLData *)ds->private_data;
   
   printd(3, "qore_mysql_close_datasource(): connection to %s closed.\n", ds->dbname);
   
   mysql_close(d_mysql->db);
   
   delete d_mysql;
   ds->private_data = NULL;
   
   traceout("qore_mysql_close_datasource()");

   return 0;
}

char *qore_mysql_module_init()
{
   tracein("qore_mysql_module_init()");

   // initialize thread key to test for mysql_thread_init()
   pthread_key_create(&ptk_mysql, NULL);
   tclist.push(mysql_thread_cleanup, NULL);
   my_init();

   // register database functions with DBI subsystem
   DBIDriverFunctions *ddf = 
      new DBIDriverFunctions(qore_mysql_init_datasource, 
			     qore_mysql_close_datasource,
			     qore_mysql_do_sql, 
			     qore_mysql_do_sql_horizontal, 
			     qore_mysql_exec, 
			     qore_mysql_describe,
			     qore_mysql_commit, 
			     qore_mysql_rollback);

   DBID_MYSQL = DBI.registerDriver("mysql", ddf, mysql_caps);

   traceout("qore_mysql_module_init()");
   return NULL;
}

void qore_mysql_module_ns_init(class Namespace *rns, class Namespace *qns)
{
   tracein("qore_mysql_module_ns_init()");
   // nothing to do at the moment
   traceout("qore_mysql_module_ns_init()");
}

void qore_mysql_module_delete()
{
   tracein("qore_mysql_module_delete()");

   //printf("mysql delete\n");

   // cleanup any thread data
   tclist.pop(1);
   // delete thread key
   pthread_key_delete(ptk_mysql);

   //DBI_deregisterDriver(DBID_MYSQL);
   traceout("qore_mysql_module_delete()");
}


