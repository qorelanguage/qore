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

#include <qore/Qore.h>
#include <qore/DBI.h>

#include "qore-mysql.h"
#include "qore-mysql-module.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <strings.h>

#ifndef QORE_MONOLITHIC
DLLEXPORT char qore_module_name[] = "mysql";
DLLEXPORT char qore_module_version[] = "0.2";
DLLEXPORT char qore_module_description[] = "MySQL database driver";
DLLEXPORT char qore_module_author[] = "David Nichols";
DLLEXPORT char qore_module_url[] = "http://qore.sourceforge.net";
DLLEXPORT int qore_module_api_major = QORE_MODULE_API_MAJOR;
DLLEXPORT int qore_module_api_minor = QORE_MODULE_API_MINOR;
DLLEXPORT qore_module_init_t qore_module_init = qore_mysql_module_init;
DLLEXPORT qore_module_ns_init_t qore_module_ns_init = qore_mysql_module_ns_init;
DLLEXPORT qore_module_delete_t qore_module_delete = qore_mysql_module_delete;
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
#ifdef HAVE_MYSQL_STMT
   | DBI_CAP_STORED_PROCEDURES | DBI_CAP_LOB_SUPPORT
#endif
;

class DBIDriver *DBID_MYSQL = NULL;

// this is the thread key that will tell us if the 
static pthread_key_t ptk_mysql;

class MySQLData {
   public:
      MYSQL *db;
      LockedObject lck;

      DLLLOCAL MySQLData(MYSQL *d) { db = d; }
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
   char *p = strchr(cs, '_');
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

   printd(5, "qore_mysql_init() datasource %08p for DB=%s\n", ds, 
	  ds->getDBName() ? ds->getDBName() : "unknown");

   if (!ds->getDBName())
   {
      xsink->raiseException("DATASOURCE-MISSING-DBNAME", "Datasource has an empty dbname parameter");
      traceout("qore_mysql_init()");
      return -1;
   }

   if (ds->getDBEncoding())
      ds->setQoreEncoding(get_qore_cs(ds->getDBEncoding()));
   else
   {
      char *enc = get_mysql_cs(QCS_DEFAULT);
      if (!enc)
      {
	 xsink->raiseException("DBI:MYSQL:UNKNOWN-CHARACTER-SET", "cannot find the mysql character set equivalent for '%s'", QCS_DEFAULT->code);
	 traceout("qore_mysql_init()");
	 return -1;
      }
      
      ds->setDBEncoding(enc);
      ds->setQoreEncoding(QCS_DEFAULT);
   }

   printd(3, "qore_mysql_init(): user=%s pass=%s db=%s (encoding=%s)\n",
	  ds->getUsername(), ds->getPassword(), ds->getDBName(), ds->getDBEncoding() ? ds->getDBEncoding() : "(none)");

   MYSQL *db = mysql_init(NULL);
   if (!mysql_real_connect(db, ds->getHostName(), ds->getUsername(), ds->getPassword(), ds->getDBName(), 0, NULL, 0))
   {
      xsink->raiseException("DBI:MYSQL:CONNECT-ERROR", "%s", mysql_error(db));
      traceout("qore_mysql_init()");
      return -1;
   }

   class MySQLData *d_mysql = new MySQLData(db);
   ds->setPrivateData((void *)d_mysql);

#ifdef HAVE_MYSQL_SET_CHARACTER_SET
   // set character set
   mysql_set_character_set(db, ds->getDBEncoding());
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
   MYSQL *db = ((MySQLData *)ds->getPrivateData())->db;
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
   MYSQL *db = ((MySQLData *)ds->getPrivateData())->db;
   if (mysql_rollback(db))
      xsink->raiseException("DBI:MYSQL:ROLLBACK-ERROR", (char *)mysql_error(db));
#else
   xsink->raiseException("DBI:MYSQL:NOT-IMPLEMENTED", "this version of the MySQL client API does not support transaction management");
#endif

   traceout("qore_mysql_rollback()");
   return 0;
}

#ifdef HAVE_MYSQL_STMT
void MyResult::bind(MYSQL_STMT *stmt)
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

class QoreNode *MyResult::getBoundColumnValue(class QoreEncoding *csid, int i)
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

inline int MyBindGroup::parse(class List *args, class ExceptionSink *xsink)
{
   char quote = 0;

   char *p = str->getBuffer();
   while (*p)
   {
      if (!quote && (*p) == '%') // found value marker
      {
	 char *w = p;

	 p++;
	 if ((*p) != 'v')
	 {
	    xsink->raiseException("DBI-EXEC-PARSE-EXCEPTION", "invalid value specification (expecting '%v', got %%%c)", *p);
	    return -1;
	 }
	 p++;
	 if (isalpha(*p))
	 {
	    xsink->raiseException("DBI-EXEC-PARSE-EXCEPTION", "invalid value specification (expecting '%v', got %%v%c*)", *p);
	    return -1;
	 }
	 if (!args || args->size() <= len)
	 {
	    xsink->raiseException("DBI-EXEC-PARSE-EXCEPTION", "too few arguments passed (%d) for value expression (%d)",
				  args ? args->size() : 0, len + 1);
	    return -1;
	 }
	 class QoreNode *v = args->retrieve_entry(len);

	 // replace value marker with "?"
	 // find byte offset in case string buffer is reallocated with replace()
	 int offset = w - str->getBuffer();
	 str->replace(offset, 2, "?");
	 p = str->getBuffer() + offset;

	 printd(5, "MyBindGroup::parse() newstr=%s\n", str->getBuffer());
	 printd(5, "MyBindGroup::parse() adding value type=%s\n",v ? v->type->getName() : "<NULL>");
	 add(v);
      }
      else if (!quote && (*p) == ':') // found placeholder marker
      {
	 char *w = p;

	 p++;
	 if (!isalpha(*p))
	    continue;

	 // get placeholder name
	 QoreString tstr;
	 while (isalnum(*p) || (*p) == '_')
	    tstr.concat(*(p++));

	 printd(5, "MyBindGroup::parse() adding placeholder for '%s'\n", tstr.getBuffer());
	 add(tstr.giveBuffer());

	 // substitute "@" for ":" in bind name
	 // find byte position of start of string
	 int offset = w - str->getBuffer();
	 str->replace(offset, 1, "@");

	 printd(5, "MyBindGroup::parse() offset=%d, new str=%s\n", offset, str->getBuffer());
      }
      else if (((*p) == '\'') || ((*p) == '\"'))
      {
	 if (!quote)
	    quote = *p;
	 else if (quote == (*p))
	    quote = 0;
	 p++;
      }
      else
	 p++;
   }

   return 0;
}

MyBindGroup::MyBindGroup(class Datasource *ods, class QoreString *ostr, class List *args, class ExceptionSink *xsink)
{
   head = tail = NULL;
   stmt = NULL;
   hasOutput = false;
   bind = NULL;
   len = 0;
   ds = ods;

   // create copy of string and convert encoding if necessary
   str = ostr->convertEncoding(ods->getQoreEncoding(), xsink);
   if (!str)
      return;

   // parse query and bind variables/placeholders, return on error
   if (parse(args, xsink))
      return;

   MySQLData *d_mysql =(MySQLData *)ds->getPrivateData();
   db = d_mysql->db;
   
   stmt = mysql_stmt_init(db);

   // prepare the statement for execution
   if (mysql_stmt_prepare(stmt, str->getBuffer(), str->strlen()))
   {
      xsink->raiseException("DBI:MYSQL:ERROR", (char *)mysql_error(db));
      return;
   }

   // if there is data to bind, then bind it
   if (len)
   {
      // allocate bind buffer
      bind = new MYSQL_BIND[len];
      // zero out bind memory
      memset(bind, 0, sizeof(MYSQL_BIND) * len);

      // bind all values/placeholders
      class MyBindNode *w = head;
      int pos = 0;
      while (w)
      {
	 printd(5, "MBG::MBG() binding value at position %d (%s)\n", pos, w->data.value->type->getName());
	 if (w->bindValue(ods->getQoreEncoding(), &bind[pos], xsink))
	    return;
	 pos++;
	 w = w->next;
      }
   }
   // now perform the bind
   if (mysql_stmt_bind_param(stmt, bind))
      xsink->raiseException("DBI:MYSQL-ERROR", (char *)mysql_error(db));
}

inline class QoreNode *MyBindGroup::getOutputHash(class ExceptionSink *xsink)
{
   class Hash *h = new Hash();

   StringList::iterator sli = phl.begin();
   while (sli != phl.end())
   {
      // prepare statement to retrieve values
      mysql_stmt_close(stmt);
      stmt = mysql_stmt_init(db);

      QoreString qstr;
      qstr.sprintf("select @%s", *sli);

      // prepare the statement for execution
      if (mysql_stmt_prepare(stmt, qstr.getBuffer(), qstr.strlen()))
      {
	 h->derefAndDelete(xsink);
	 xsink->raiseException("DBI:MYSQL:ERROR", (char *)mysql_error(db));
	 return NULL;
      }

      class QoreNode *v = NULL;

      MYSQL_RES *res = mysql_stmt_result_metadata(stmt);
      if (res)
      {
	 class MyResult myres(res);

	 if (mysql_stmt_execute(stmt))
	 {
	    h->derefAndDelete(xsink);
	    xsink->raiseException("DBI:MYSQL:ERROR", (char *)mysql_error(db));
	    return NULL;
	 }

	 int rows = mysql_stmt_affected_rows(stmt);
	 if (rows)
	 {
	    myres.bind(stmt);

	    if (rows > 1)
	    {
	       List *l = new List();
	       while (!mysql_stmt_fetch(stmt))
		  l->push(myres.getBoundColumnValue(ds->getQoreEncoding(), 0));
	       v = new QoreNode(l);
	    }
	    else
	    {
	       mysql_stmt_fetch(stmt);
	       v = myres.getBoundColumnValue(ds->getQoreEncoding(), 0);
	    }
	 }
      }

      h->setKeyValue(*sli, v, NULL);
      sli++;
   }
   return new QoreNode(h);
}

class QoreNode *MyBindGroup::execIntern(class ExceptionSink *xsink)
{
   class QoreNode *rv;
   MYSQL_RES *res = mysql_stmt_result_metadata(stmt);
   if (res)
   {
      class MyResult myres(res);

      if (mysql_stmt_execute(stmt))
      {
	 xsink->raiseException("DBI:MYSQL:ERROR", (char *)mysql_error(db));
	 return NULL;
      }

      Hash *h = new Hash();
	 
      for (int i = 0; i < myres.getNumFields(); i++)
	 h->setKeyValue(myres.getFieldName(i), new QoreNode(new List()), NULL);
	 
      if (mysql_stmt_affected_rows(stmt))
      {
	 myres.bind(stmt);
	    
	 while (!mysql_stmt_fetch(stmt))
	    for (int i = 0; i < myres.getNumFields(); i++)
	       h->getKeyValue(myres.getFieldName(i))->val.list->push(myres.getBoundColumnValue(ds->getQoreEncoding(), i));
      }
      rv = new QoreNode(h);
   }
   else
   {
      // there is no result set
      if (mysql_stmt_execute(stmt))
      {
	 xsink->raiseException("DBI:MYSQL:ERROR", (char *)mysql_error(db));
	 return NULL;
      }

      if (!hasOutput)
	 rv = new QoreNode((int64)mysql_stmt_affected_rows(stmt));
      else
	 rv = getOutputHash(xsink);
   }

   return rv;
}

inline class QoreNode *MyBindGroup::exec(class ExceptionSink *xsink)
{
   class QoreNode *rv = execIntern(xsink);
   
#ifdef HAVE_MYSQL_COMMIT
   if (!xsink->isException() && ds->getAutoCommit())
      mysql_commit(db);
#endif
   
   return rv;
}

inline class QoreNode *MyBindGroup::select(class ExceptionSink *xsink)
{
   return execIntern(xsink);
}

class QoreNode *MyBindGroup::selectRows(class ExceptionSink *xsink)
{
   class QoreNode *rv;
   MYSQL_RES *res = mysql_stmt_result_metadata(stmt);
   if (res)
   {
      class MyResult myres(res);

      if (mysql_stmt_execute(stmt))
      {
	 xsink->raiseException("DBI:MYSQL:ERROR", (char *)mysql_error(db));
	 return NULL;
      }

      List *l = new List();

      if (mysql_stmt_affected_rows(stmt))
      {
	 myres.bind(stmt);

	 while (!mysql_stmt_fetch(stmt))
	 {
	    class Hash *h = new Hash();

	    for (int i = 0; i < myres.getNumFields(); i++)
	       h->setKeyValue(myres.getFieldName(i), myres.getBoundColumnValue(ds->getQoreEncoding(), i), NULL);

	    l->push(new QoreNode(h));
	 }
      }

      rv = new QoreNode(l);
   }
   else
   {
      // there is no result set
      if (mysql_stmt_execute(stmt))
      {
	 xsink->raiseException("DBI:MYSQL:ERROR", (char *)mysql_error(db));
	 return NULL;
      }

      if (!hasOutput)
	 rv = new QoreNode((int64)mysql_stmt_affected_rows(stmt));
      else
	 rv = getOutputHash(xsink);
   }

   return rv;
}

int MyBindNode::bindValue(class QoreEncoding *enc, MYSQL_BIND *buf, class ExceptionSink *xsink)
{
   //printd(5, "MyBindNode::bindValue() type=%s\n", data.value ? data.value->type->getName() : "NOTHING");

   // bind a NULL value
   if (is_nothing(data.value) || is_null(data.value))
   {
      buf->buffer_type = MYSQL_TYPE_NULL;
      return 0;
   }
   else if (data.value->type == NT_STRING)
   {
      // convert to the db charset if necessary
      class QoreString *bstr = data.value->val.String;
      if (bstr->getEncoding() != enc)
      {
	 bstr = bstr->convertEncoding(enc, xsink);
	 if (!bstr) // exception was thrown
	    return -1;
	 // save temporary string for later deleting
	 data.tstr = bstr;
      }

      len = bstr->strlen();

      buf->buffer_type = MYSQL_TYPE_STRING;
      buf->buffer = bstr->getBuffer();
      buf->buffer_length = len + 1;
      buf->length = &len;
      return 0;
   }
   else if (data.value->type == NT_DATE)
   {
      vbuf.assign(data.value->val.date_time);

      buf->buffer_type = MYSQL_TYPE_DATETIME;
      buf->buffer = &vbuf.time;
      return 0;
   }
   else if (data.value->type == NT_BINARY)
   {
      len = data.value->val.bin->size();
      buf->buffer_type = MYSQL_TYPE_BLOB;
      buf->buffer = data.value->val.bin->getPtr();
      buf->buffer_length = len;
      buf->length = &len;
      return 0;
   }
   else if (data.value->type == NT_BOOLEAN)
   {
      vbuf.i4 = data.value->val.intval;
      buf->buffer_type = MYSQL_TYPE_LONG;
      buf->buffer = (char *)&vbuf.i4;
      return 0;
   }
   else if (data.value->type == NT_INT)
   {
      buf->buffer_type = MYSQL_TYPE_LONGLONG;
      buf->buffer = (char *)&data.value->val.intval;
      return 0;
   }
   else if (data.value->type == NT_FLOAT)
   {
      buf->buffer_type = MYSQL_TYPE_DOUBLE;
      buf->buffer = (char *)&data.value->val.floatval;
      return 0;
   }

   xsink->raiseException("DBI-EXEC-EXCEPTION", "type '%s' is not supported for SQL binding", data.value->type->getName());
   return -1;
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
	       row[i][4]  = '\0';
	       row[i][7]  = '\0';
	       row[i][10] = '\0';
	       row[i][13] = '\0';
	       row[i][16] = '\0';

	       n = new QoreNode(new DateTime(atoi(row[i]), atoi(row[i] + 5), atoi(row[i] + 8), atoi(row[i] + 11), atoi(row[i] + 14), atoi(row[i] + 17)));
	       break;
	    }

	    // for date values
	    case FIELD_TYPE_DATE:
	    {
	       row[i][4] = '\0';
	       row[i][7] = '\0';
	       n = new QoreNode(new DateTime(atoi(row[i]), atoi(row[i] + 5), atoi(row[i] + 8), 0, 0, 0));
	       break;
	    }
	    
	    // for time values
	    case FIELD_TYPE_TIME:
	    {
	       row[i][2] = '\0';
	       row[i][5] = '\0';
	       n = new QoreNode(new DateTime(0, 0, 0, atoi(row[i]), atoi(row[i] + 3), atoi(row[i] + 6)));
	       break;
	    }

	    case FIELD_TYPE_TIMESTAMP:
	       n = new QoreNode(new DateTime(row[i]));
	       break;
	    
	    // the rest defaults to string
	    default:
	       n = new QoreNode(new QoreString(row[i], ds->getQoreEncoding()));
	       break;
	 }
	 //printd(5, "get_result_set() row %d col %d: %s (type=%d)=\"%s\"\n", rn, i, field[i].name, field[i].type, row[i]);
	 h->getKeyValue(field[i].name)->val.list->push(n);
      }
      rn++;
   }
   return h;
}

static class QoreNode *qore_mysql_do_sql(class Datasource *ds, QoreString *qstr, class List *args, ExceptionSink *xsink)
{
   tracein("qore_mysql_do_sql()");

   // convert string if necessary
   class QoreString *tqstr;

   if (qstr->getEncoding() != ds->getQoreEncoding())
   {
      tqstr = qstr->convertEncoding(ds->getQoreEncoding(), xsink);
      if (xsink->isEvent())
         return NULL;
   }
   else
      tqstr = qstr;

   MySQLData *d_mysql =(MySQLData *)ds->getPrivateData();
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

static class QoreNode *qore_mysql_do_sql_horizontal(class Datasource *ds, QoreString *qstr, class List *args, ExceptionSink *xsink)
{
   xsink->raiseException("MYSQL-UNSUPPORTED", "row retrieval not yet implemented for old versions of MySQL without a prepared statement interface");
   return NULL;
}
#endif // HAVE_MYSQL_STMT

/*
static class Hash *qore_mysql_describe(class Datasource *ds, char *table_name, ExceptionSink *xsink)
{
   tracein("qore_mysql_describe()");

   checkInit();
   xsink->raiseException("DBI:MYSQL:NOT-IMPLEMENTED", "sorry, not implemented yet");

   traceout("qore_mysql_describe()");
   return NULL;
}
*/

static class QoreNode *qore_mysql_select_rows(class Datasource *ds, QoreString *qstr, class List *args, class ExceptionSink *xsink)
{
   checkInit();
#ifdef HAVE_MYSQL_STMT
   class MyBindGroup bg(ds, qstr, args, xsink);

   if (xsink->isException())
      return NULL;

   return bg.selectRows(xsink);
#else
   return qore_mysql_do_sql_horizontal(ds, qstr, args, xsink);
#endif
}

static class QoreNode *qore_mysql_select(class Datasource *ds, QoreString *qstr, class List *args, class ExceptionSink *xsink)
{
   checkInit();
#ifdef HAVE_MYSQL_STMT
   class MyBindGroup bg(ds, qstr, args, xsink);

   if (xsink->isException())
      return NULL;

   return bg.select(xsink);
#else
   return qore_mysql_do_sql(ds, qstr, args, xsink);
#endif
}

static class QoreNode *qore_mysql_exec(class Datasource *ds, QoreString *qstr, class List *args, class ExceptionSink *xsink)
{
   checkInit();
#ifdef HAVE_MYSQL_STMT
   class MyBindGroup bg(ds, qstr, args, xsink);

   if (xsink->isException())
      return NULL;

   return bg.exec(xsink);
#else
   return qore_mysql_do_sql(ds, qstr, args, xsink);
#endif
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

   class MySQLData *d_mysql = (MySQLData *)ds->getPrivateData();
   
   printd(3, "qore_mysql_close_datasource(): connection to %s closed.\n", ds->getDBName());
   
   mysql_close(d_mysql->db);
   
   delete d_mysql;
   ds->setPrivateData(NULL);
   
   traceout("qore_mysql_close_datasource()");

   return 0;
}

class QoreString *qore_mysql_module_init()
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
			     qore_mysql_select, 
			     qore_mysql_select_rows, 
			     qore_mysql_exec, 
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


