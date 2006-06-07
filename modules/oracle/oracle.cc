/*
  oracle.cc

  Oracle OCI Interface to Qore DBI layer

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

#include <qore/config.h>
#include <qore/support.h>
#include <qore/QoreNode.h>
#include <qore/QoreType.h>
#include <qore/Exception.h>
#include <qore/Object.h>
#include <qore/QoreString.h>
#include <qore/QoreClass.h>
#include <qore/charset.h>
#include <qore/QC_Datasource.h>
#include <qore/DBI.h>
#include <qore/module.h>
#include <qore/ModuleManager.h>

#include "oracle.h"
#include "oracle-module.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <strings.h>

#ifndef QORE_MONOLITHIC
char qore_module_name[] = "oracle";
char qore_module_version[] = "0.1";
char qore_module_description[] = "Oracle database driver";
char qore_module_author[] = "David Nichols";
char qore_module_url[] = "http://qore.sourceforge.net";
int qore_module_api_major = QORE_MODULE_API_MAJOR;
int qore_module_api_minor = QORE_MODULE_API_MINOR;
qore_module_init_t qore_module_init = oracle_module_init;
qore_module_ns_init_t qore_module_ns_init = oracle_module_ns_init;
qore_module_delete_t qore_module_delete = oracle_module_delete;
#endif

// capabilities of this driver
#define DBI_ORACLE_CAPS (DBI_CAP_TRANSACTION_MANAGEMENT | DBI_CAP_STORED_PROCEDURES | DBI_CAP_CHARSET_SUPPORT | DBI_CAP_LOB_SUPPORT)

class DBIDriver *DBID_ORACLE = NULL;

// maximum string size for an oracle number
#define ORACLE_NUMBER_STR_LEN 30

class OracleData {
   public:
      OCIEnv *envhp;
      OCIError *errhp;
      OCISvcCtx *svchp;
      ub2 charsetid;
};

static void ora_checkerr(OCIError *errhp, sword status, char *query_name, Datasource *ds, ExceptionSink *xsink)
{
   text errbuf[512];
   sb4 errcode = 0;

   //printd(5, "ora_checkerr(%08x, %d, %s, isEvent=%d)\n", errhp, status, query_name ? query_name : "none", xsink->isEvent());
   switch (status)
   {
      case OCI_SUCCESS:
      case OCI_SUCCESS_WITH_INFO:
	 // ignore SUCCESS_WITH_INFO codes
	 break;

      case OCI_ERROR:
	 OCIErrorGet((dvoid *)errhp, (ub4) 1, (text *) NULL, &errcode, errbuf, (ub4) sizeof(errbuf), OCI_HTYPE_ERROR);
	 if (query_name)
	    xsink->raiseException("DBI:ORACLE:OCI-ERROR", "%s@%s: %s: %s", ds->username, ds->dbname, query_name, remove_trailing_newlines((char *)errbuf));
	 else
	    xsink->raiseException("DBI:ORACLE:OCI-ERROR", "%s@%s: %s", ds->username, ds->dbname, remove_trailing_newlines((char *)errbuf));
	 break;
      case OCI_INVALID_HANDLE:
	 xsink->raiseException("DBI:ORACLE:OCI-INVALID-HANDLE", "%s@%s: %s", ds->username, ds->dbname, remove_trailing_newlines((char *)errbuf));
	 break;
      case OCI_NEED_DATA:
	 xsink->raiseException("DBI:ORACLE:OCI-NEED-DATA", "Oracle OCI error");
	 break;
      case OCI_NO_DATA:
	 xsink->raiseException("DBI:ORACLE:OCI-NODATA", "Oracle OCI error");
	 break;
      case OCI_STILL_EXECUTING:
	 xsink->raiseException("DBI:ORACLE:OCI-STILL-EXECUTING", "Oracle OCI error");
	 break;
      case OCI_CONTINUE:
	 xsink->raiseException("DBI:ORACLE:OCI-CONTINUE", "Oracle OCI error");
	 break;
      default:
	 xsink->raiseException("DBI:ORACLE:UNKNOWN-ERROR", "unknown OCI error code %d", status);
	 break;
   }
   //if (xsink->isEvent()) abort();
}

static int oracle_commit(class Datasource *ds, ExceptionSink *xsink)
{
   int status;

   class OracleData *d_ora = (OracleData *)ds->private_data;

   // commit transaction
   if ((status = OCITransCommit(d_ora->svchp, d_ora->errhp, (ub4) 0)))
      ora_checkerr(d_ora->errhp, status, "Oracle commit transaction", ds, xsink);
   return 0;
}

static int oracle_rollback(class Datasource *ds, ExceptionSink *xsink)
{
   int status;

   class OracleData *d_ora = (OracleData *)ds->private_data;
   if ((status = OCITransRollback(d_ora->svchp, d_ora->errhp, (ub4) 0)))
      ora_checkerr(d_ora->errhp, status, "Oracle rollback transaction", ds, xsink);
   return 0;
}

Columns::Columns(OCIStmt *stmthp, class Datasource *ds, char *str, ExceptionSink *xsink)
{
   tracein("Columns::Columns()");

   len = 0;
   head = tail = NULL;

   class OracleData *d_ora = (OracleData *)ds->private_data;

   // retrieve results, if any
   OCIParam *parmp;

   // get columns in output
   while (OCIParamGet(stmthp, OCI_HTYPE_STMT, d_ora->errhp, (dvoid **)&parmp, size() + 1) == OCI_SUCCESS)
   {
      ub2 dtype;
      text *col_name;
      int col_name_len;
      ub2 col_max_size;

      // get column type
      ora_checkerr(d_ora->errhp, 
		   OCIAttrGet(parmp, OCI_DTYPE_PARAM, &dtype, 0, OCI_ATTR_DATA_TYPE, d_ora->errhp), 
		   str, ds, xsink);
      if (xsink->isEvent()) return;
      // get column name
      ora_checkerr(d_ora->errhp, 
		   OCIAttrGet(parmp, OCI_DTYPE_PARAM, &col_name, (ub4 *)&col_name_len, OCI_ATTR_NAME, d_ora->errhp), 
		   str, ds, xsink);
      if (xsink->isEvent()) return;
      // get size
      ora_checkerr(d_ora->errhp, 
		   OCIAttrGet(parmp, OCI_DTYPE_PARAM, &col_max_size, 0, OCI_ATTR_DATA_SIZE, d_ora->errhp), 
		   str, ds, xsink);

      if (xsink->isEvent()) return;

      add((char *)col_name, col_name_len, col_max_size, dtype);
   }

   traceout("Columns::Columns()");
}

void Columns::define(OCIStmt *stmthp, class Datasource *ds, char *str, ExceptionSink *xsink)
{
   //tracein("Columne::define()");

   class OracleData *d_ora = (OracleData *)ds->private_data;

   // iterate column list
   class Column *w = head;
   int i = 0;
   while (w)
   {
      if (w->dtype == SQLT_DAT)
      {
	 w->val.ptr = malloc(sizeof(char) * 7);
	 ora_checkerr(d_ora->errhp,
		      OCIDefineByPos(stmthp, &w->defp, d_ora->errhp, i + 1, w->val.ptr, 7, SQLT_DAT, &w->ind, 0, 0, OCI_DEFAULT), 
		      str, ds, xsink);
	 if (xsink->isEvent()) return;
      }
      else if (w->dtype == SQLT_INT)
      {
	 w->val.i8 = 0;
	 ora_checkerr(d_ora->errhp,
		      OCIDefineByPos(stmthp, &w->defp, d_ora->errhp, i + 1, &w->val.i8, sizeof(int64), SQLT_INT, &w->ind, 0, 0, OCI_DEFAULT), 
		      str, ds, xsink);
	 if (xsink->isEvent()) return;
      }
      else if (w->dtype == SQLT_FLT)
      {
	 ora_checkerr(d_ora->errhp,
		      OCIDefineByPos(stmthp, &w->defp, d_ora->errhp, i + 1, &w->val.f8, sizeof(double), SQLT_FLT, &w->ind, 0, 0, OCI_DEFAULT), 
		      str, ds, xsink);
	 if (xsink->isEvent()) return;
      }
      else if (w->dtype == SQLT_BLOB || w->dtype == SQLT_CLOB)
      {
	 w->val.ptr = NULL;
	 ora_checkerr(d_ora->errhp, 
		      OCIDescriptorAlloc(d_ora->envhp, &w->val.ptr, OCI_DTYPE_LOB, 0, NULL), str, ds, xsink);
	 if (xsink->isEvent()) return;
	 printd(5, "Columns::define() got LOB locator handle %08x\n", w->val.ptr);
	 ora_checkerr(d_ora->errhp,
		      OCIDefineByPos(stmthp, &w->defp, d_ora->errhp, i + 1, &w->val.ptr, 0, w->dtype, &w->ind, 0, 0, OCI_DEFAULT), 
		      str, ds, xsink);
	 if (xsink->isEvent()) return;
      }
      else // treated as a string
      {
	 w->val.ptr = malloc(sizeof(char) * (w->maxsize + 1));
	 ora_checkerr(d_ora->errhp,
		      OCIDefineByPos(stmthp, &w->defp, d_ora->errhp, i + 1, w->val.ptr, w->maxsize + 1, SQLT_STR, &w->ind, 0, 0, OCI_DEFAULT), 
		      str, ds, xsink);
	 if (xsink->isEvent()) return;
      }
      w = w->next;
      i++;
   }
   //traceout("Columns::define()");
}

static class DateTime *convert_date_time(unsigned char *str)
{
   DateTime *dt = new DateTime;
   if ((str[0] < 100) || (str[1] < 100))
      dt->year = 9999; 
   else
      dt->year = (str[0] - 100) * 100 + (str[1] - 100);
   dt->month       = str[2];
   dt->day         = str[3];
   dt->hour        = str[4] - 1;
   dt->minute      = str[5] - 1;
   dt->second      = str[6] - 1;
   dt->millisecond = 0;
   dt->relative    = 0;
   printd(1, "convert_date_time(): %d %d = %04d-%02d-%02d %02d:%02d:%02d\n", str[0], str[1], dt->year, dt->month, dt->day, dt->hour, dt->minute, dt->second);
   return dt;
}

static void *make_oracle_date_time(class DateTime *d)
{
   char *buf = (char *)malloc(7 * sizeof(char));

   buf[0] = d->year / 100 + 100;
   buf[1] = d->year % 100 + 100;
   buf[2] = d->month;
   buf[3] = d->day;
   buf[4] = d->hour + 1;
   buf[5] = d->minute + 1;
   buf[6] = d->second + 1;
   printd(1, "make_oracle_date_time(): %04d-%02d-%02d %02d:%02d:%02d = %d %d\n", d->year, d->month, d->day, d->hour, d->minute, d->second, buf[0], buf[1]);
   return buf;
}

static sb4 read_clob_callback(void *sp, CONST dvoid *bufp, ub4 len, ub1 piece)
{
   printd(5, "read_clob_callback(%08x, %08x, %d, %d)\n", sp, bufp, len, piece);
   ((class QoreString *)sp)->concat((char *)bufp, len);
   return OCI_CONTINUE;
}

static sb4 read_blob_callback(void *bp, CONST dvoid *bufp, ub4 len, ub1 piece)
{
   printd(5, "read_blob_callback(%08x, %08x, %d, %d)\n", bp, bufp, len, piece);
   ((class BinaryObject *)bp)->append((char *)bufp, len);
   return OCI_CONTINUE;
}

class QoreNode *Column::getValue(class Datasource *ds, class ExceptionSink *xsink)
{
   //printd(5, "Column::getValue() dtype=%d, ind=%d, maxsize=%d\n", dtype, ind, maxsize);

   if (ind == -1)      // SQL NULL returned
      return null();

   if (dtype == SQLT_INT)
      return new QoreNode(val.i8);
   else if (dtype == SQLT_FLT)
      return new QoreNode(val.f8);
   else if (dtype == SQLT_DAT)
      return new QoreNode(convert_date_time((unsigned char *)val.ptr));
   else if (dtype == SQLT_CLOB || dtype == SQLT_BLOB)
   {
      // get oracle data
      class OracleData *d_ora = (OracleData *)ds->private_data;

      printd(5, "Columns::getValue() using LOB locator handle %08x\n", val.ptr);

      // retrieve *LOB data
      void *buf = malloc(LOB_BLOCK_SIZE);
      class QoreNode *rv;
      ub4 amt = 0;
      if (dtype == SQLT_CLOB)
      {
	 class QoreString *str = new QoreString(ds->qorecharset, "");
	 // read LOB data in streaming callback mode
	 ora_checkerr(d_ora->errhp,
		      OCILobRead(d_ora->svchp, d_ora->errhp, (OCILobLocator *)val.ptr, &amt, 1, buf, LOB_BLOCK_SIZE,
				 str, read_clob_callback, (ub2)d_ora->charsetid, (ub1)0), "oraReadCLOBCallback()", ds, xsink);
	 if (!xsink->isEvent())
	    rv = new QoreNode(str);
	 else
	    rv = NULL;
      }
      else
      {
	 BinaryObject *b = new BinaryObject();
	 // read LOB data in streaming callback mode
	 ora_checkerr(d_ora->errhp,
		      OCILobRead(d_ora->svchp, d_ora->errhp, (OCILobLocator *)val.ptr, &amt, 1, buf, LOB_BLOCK_SIZE,
				 b, read_blob_callback, (ub2)0, (ub1)0), "oraReadBLOBCallback()", ds, xsink);
	 if (!xsink->isEvent())
	    rv = new QoreNode(b);
	 else
	    rv = NULL;
      }
      free(buf);
      return rv;
   }

   // must be string data
   remove_trailing_blanks((char *)val.ptr);
   return new QoreNode(new QoreString(ds->qorecharset, (char *)val.ptr));
}

#define DEFAULT_STR_LEN 512

static class Hash *ora_fetch(OCIStmt *stmthp, class Datasource *ds, class ExceptionSink *xsink)
{
   class Hash *h = NULL;
   // retrieve results from statement and return hash
   
   // setup column structure for output columns
   class Columns *columns = new Columns(stmthp, ds, "ora_fetch()", xsink);

   if (!xsink->isEvent())
   {
      // allocate result hash for result value
      h = new Hash();
      
      // create hash elements for each column, assign empty list
      class Column *w = columns->getHead();
      while (w)
      {
	 printd(5, "ora_fetch() allocating list for '%s' column\n", w->name);
	 h->setKeyValue(w->name, new QoreNode(new List()), xsink);
	 w = w->next;
      }
      
      int num_rows = 0;
      
      // setup temporary row to accept values
      columns->define(stmthp, ds, "ora_fetch()", xsink);
      
      // now finally fetch the data
      while (!xsink->isEvent())
      {
	 int status;
	 class OracleData *d_ora = (OracleData *)ds->private_data;

	 if ((status = OCIStmtFetch(stmthp, d_ora->errhp, 1, OCI_FETCH_NEXT, OCI_DEFAULT)))
	 {
	    if (status == OCI_NO_DATA)
	       break;
	    else
	    {
	       ora_checkerr(d_ora->errhp, status, "ora_fetch()", ds, xsink);
	       if (xsink->isEvent())
		  break;
	    }
	 }

	 // copy data or perform per-value processing if needed
	 class Column *w = columns->getHead();
	 int i = 0;
	 while (w)
	 {
	    // get pointer to value of target node
	    QoreNode **v = h->getKeyValue(w->name)->val.list->get_entry_ptr(num_rows);
	    
	    // dereference node if already present
	    if (*v)
	    {
	       printd(1, "ora_fetch(): dereferencing result value (col=%s)\n", w->name);
	       (*v)->deref(xsink);
	    }
	    // FIXME: check for exception after this call]
	    (*v) = w->getValue(ds, xsink);
	    if (xsink->isEvent())
	       break;
	    w = w->next;
	    i++;
	 }
	 num_rows++;
      }
      printd(2, "ora_fetch(): %d column(s), %d row(s) retrieved as output\n", columns->size(), num_rows);
   }
   // free column structure
   if (columns)
      delete columns;

   return h;
}

// returns a list of hashes for a "horizontal" fetch
static class List *ora_fetch_horizontal(OCIStmt *stmthp, class Datasource *ds, class ExceptionSink *xsink)
{
   class List *l = NULL;
   // retrieve results from statement and return hash
   
   // setup column structure for output columns
   class Columns *columns = new Columns(stmthp, ds, "ora_fetch_horizontal()", xsink);

   if (!xsink->isEvent())
   {
      // allocate result hash for result value
      l = new List();

      // setup temporary row to accept values
      columns->define(stmthp, ds, "ora_fetch_horizontal()", xsink);

      class OracleData *d_ora = (OracleData *)ds->private_data;

      // now finally fetch the data
      while (!xsink->isEvent())
      {
	 int status;

	 if ((status = OCIStmtFetch(stmthp, d_ora->errhp, 1, OCI_FETCH_NEXT, OCI_DEFAULT)))
	 {
	    if (status == OCI_NO_DATA)
	       break;
	    else
	    {
	       ora_checkerr(d_ora->errhp, status, "ora_fetch_horizontal()", ds, xsink);
	       if (xsink->isEvent())
		  break;
	    }
	 }
	 //printd(5, "ora_fetch_horizontal(): l=%08x, %d column(s), got row %d\n", l, columns->size(), l->size());

	 // set up hash for row
	 class Hash *h = new Hash();

	 // copy data or perform per-value processing if needed
	 class Column *w = columns->getHead();
	 while (w)
	 {
	    // assign value to hash
	    h->setKeyValue(w->name, w->getValue(ds, xsink), xsink);
	    if (xsink->isEvent())
	       break;
	    w = w->next;
	 }
	 // add row to list
	 l->push(new QoreNode(h));
      }
      printd(2, "ora_fetch_horizontal(): %d column(s), %d row(s) retrieved as output\n", columns->size(), l->size());
   }
   // free column structure
   if (columns)
      delete columns;

   return l;
}

static class QoreNode *oracle_select(class Datasource *ds, QoreString *qstr, ExceptionSink *xsink)
{
   OCIStmt *stmthp;
   int status;
   class Hash *h = NULL;
   class QoreString *tqstr;

   tracein("oracle_select()");

   // convert character set if necessary
   if (qstr->getEncoding() != ds->qorecharset)
   {
      tqstr = qstr->convertEncoding(ds->qorecharset, xsink);
      if (xsink->isEvent())
	 return NULL;
   }
   else
      tqstr = qstr;

   printd(4, "oracle_select(%08x) \"%s\"\n", ds, tqstr->getBuffer());

   class OracleData *d_ora = (OracleData *)ds->private_data;

   ora_checkerr(d_ora->errhp, 
		OCIHandleAlloc(d_ora->envhp, (dvoid **)&stmthp, OCI_HTYPE_STMT,
			       0, 0), "oracle_select()", ds, xsink);
   if (xsink->isEvent())
   {
      if (tqstr != qstr)
	 delete tqstr;
      return NULL;
   }

   ora_checkerr(d_ora->errhp, 
		OCIStmtPrepare(stmthp, d_ora->errhp, (text *)tqstr->getBuffer(), tqstr->strlen(), OCI_NTV_SYNTAX, OCI_DEFAULT), 
		"oracle_select()", ds, xsink);

   if (!xsink->isEvent() && (status = OCIStmtExecute(d_ora->svchp, stmthp, d_ora->errhp, 0, 0, NULL, NULL, OCI_DEFAULT)))
      ora_checkerr(d_ora->errhp, status, "oracle_select()", ds, xsink);
   if (!xsink->isEvent())
      h = ora_fetch(stmthp, ds, xsink);

   if (tqstr != qstr) 
      delete tqstr;

   // free OCI handle
   OCIHandleFree(stmthp, OCI_HTYPE_STMT);

   traceout("oracle_select()");
   if (!h)
      return NULL;
   return new QoreNode(h);
}

static class QoreNode *oracle_select_rows(class Datasource *ds, QoreString *qstr, ExceptionSink *xsink)
{
   OCIStmt *stmthp;
   int status;
   class List *l = NULL;
   class QoreString *tqstr;

   tracein("oracle_select_rows()");

   // convert character set if necessary
   if (qstr->getEncoding() != ds->qorecharset)
   {
      tqstr = qstr->convertEncoding(ds->qorecharset, xsink);
      if (xsink->isEvent())
	 return NULL;
   }
   else
      tqstr = qstr;

   printd(5, "oracle_select_rows(%08x) '%s'\n", ds, tqstr->getBuffer());

   class OracleData *d_ora = (OracleData *)ds->private_data;

   ora_checkerr(d_ora->errhp, 
		OCIHandleAlloc(d_ora->envhp, (dvoid **)&stmthp, OCI_HTYPE_STMT,
			       0, 0), "oracle_select()", ds, xsink);
   if (xsink->isEvent())
   {
      if (tqstr != qstr)
	 delete tqstr;
      return NULL;
   }

   ora_checkerr(d_ora->errhp, 
		OCIStmtPrepare(stmthp, d_ora->errhp, (text *)tqstr->getBuffer(), tqstr->strlen(), OCI_NTV_SYNTAX, OCI_DEFAULT), 
		"oracle_select_rows()", ds, xsink);

   if (!xsink->isEvent() && (status = OCIStmtExecute(d_ora->svchp, stmthp, d_ora->errhp, 0, 0, NULL, NULL, OCI_DEFAULT)))
      ora_checkerr(d_ora->errhp, status, "oracle_select_rows()", ds, xsink);
   if (!xsink->isEvent())
      l = ora_fetch_horizontal(stmthp, ds, xsink);

   if (tqstr != qstr) 
      delete tqstr;

   // free OCI handle
   OCIHandleFree(stmthp, OCI_HTYPE_STMT);

   traceout("oracle_select_rows()");
   if (!l)
      return NULL;
   return new QoreNode(l);
}

// NOTE this method is disabled in the DBI driver
static class Hash *oracle_describe(class Datasource *ds, char *table_name, ExceptionSink *xsink)
{
   OCIStmt *stmthp;
   int status;
   Hash *h = NULL;

   tracein("oracle_describe()");
   printd(4, "oracle_describe(%08x) '%s'\n", ds, table_name);

   class OracleData *d_ora = (OracleData *)ds->private_data;

   ora_checkerr(d_ora->errhp, 
		OCIHandleAlloc(d_ora->envhp, (dvoid **)&stmthp, OCI_HTYPE_STMT, 0, 0), 
		"oracle_describe()", ds, xsink);
   if (xsink->isEvent())
      return NULL;

   QoreString qstr("select * from ");
   qstr.concat(table_name);

   ora_checkerr(d_ora->errhp, 
		OCIStmtPrepare(stmthp, d_ora->errhp, (text *)qstr.getBuffer(), qstr.strlen(), OCI_NTV_SYNTAX, OCI_DEFAULT), 
		"oracle_describe()", ds, xsink);

   if (!xsink->isEvent() && (status = OCIStmtExecute(d_ora->svchp, stmthp, d_ora->errhp,
				       0, 0, NULL, NULL, OCI_DEFAULT)))
      ora_checkerr(d_ora->errhp, status, "oracle_describe()", ds, xsink);
   if (!xsink->isEvent())
   {
      // now retrieve results from statement and return

      // setup column structure for output columns
      class Columns *columns = new Columns(stmthp, ds, "oracle_describe()", xsink);
      if (!xsink->isEvent())
      {
	 // allocate result hash for result value
	 h = new Hash();
	 
	 // create hash elements for each column, assign type
	 class Column *w = columns->getHead();
	 while (w)
	 {
	    QoreNode *n = new QoreNode((int64)-1); //w->type->getID());
	    h->setKeyValue(w->name, n, xsink);
	    w = w->next;
	 }
      }
      // free column structure
      if (columns)
	 delete columns;
   }
   // free OCI handle
   OCIHandleFree(stmthp, OCI_HTYPE_STMT);

   traceout("oracle_describe()");
   return h;
}

BindGroup::BindGroup(class Datasource *ods, class QoreString *ostr, class List *args, ExceptionSink *xsink)
{
   stmthp = NULL;
   hasOutput = false;
   head = tail = NULL;
   ds = ods;
   len = 0;

   // create copy of string and convert encoding if necessary
   str = ostr->convertEncoding(ds->qorecharset, xsink);
   if (xsink->isEvent())
      return;

   class OracleData *d_ora = (OracleData *)ds->private_data;
   printd(4, "BindGroup::BindGroup() ds=%08x, d_ora=%08x, SQL='%s', args=%d\n", ds, d_ora, str->getBuffer(), args ? args->size() : 0);

   // process query string and setup bind value list
   parseQuery(args, xsink);

   if (xsink->isEvent())
      return;
      
   ora_checkerr(d_ora->errhp, 
		OCIHandleAlloc(d_ora->envhp, (dvoid **)&stmthp, OCI_HTYPE_STMT, 0, 0), 
		"BindGroup::BindGroup():hndl", ds, xsink);
   if (xsink->isEvent())
      return;

   ora_checkerr(d_ora->errhp, 
		OCIStmtPrepare(stmthp, d_ora->errhp, (text *)str->getBuffer(), str->strlen(), OCI_NTV_SYNTAX, OCI_DEFAULT), 
		"BindGroup():prep", ds, xsink);

   if (xsink->isEvent())
      return;

   class BindNode *w = head;
   int pos = 1;
   while (w)
   {
      if (w->bindtype == BV_PLACEHOLDER)
	 w->bindPlaceholder(ds, stmthp, pos, xsink);
      else
	 w->bindValue(ds, stmthp, pos, xsink);
      
      if (xsink->isEvent())
	 return;

      pos++;
      w = w->next;
   }

}

#ifdef ORA_EXEC_COMPAT
void BindGroup::parseOld(class Hash *h, class ExceptionSink *xsink)
{
   printd(5, "parseOld() h=%08x (%d)\n", h, h->size());

   char quote = 0;

   char *p = str->getBuffer();
   while (*p)
   {
      if (!quote && (*p) == ':') // found placeholder marker
      {
	 p++;
	 if (!isalpha(*p))
	    continue;
	 // get name
	 QoreString tstr;
	 tstr.concat(':');
	 while (isalnum(*p) || (*p) == '_')
	    tstr.concat(*(p++));

	 // get hash value
	 class QoreNode *v = h->getKeyValue(&tstr, xsink);
	 if (!v)
	 {
	    xsink->raiseException("DBI-EXEC-COMPAT-PARSE-EXCEPTION", "no hash key found for '%s'", tstr.getBuffer());
	    break;
	 }
	 if (v->type == NT_STRING && v->val.String->strlen())
	    add(v, xsink);
	 else
	    add(tstr.giveBuffer(), -1, "string", xsink);
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
}
#endif

void BindGroup::parseQuery(class List *args, class ExceptionSink *xsink)
{
   printd(5, "parseQuery() args=%08x str=%s\n", args, str->getBuffer());

#ifdef ORA_EXEC_COMPAT
   // check if old-style hash given for exec
   if (args && args->size() == 1)
   {
      class QoreNode *h = args->retrieve_entry(0);
      if (h && h->type == NT_HASH)
      {
	 char *k = h->val.hash->getFirstKey();
	 if (k && k[0] == ':')
	 {
	    parseOld(h->val.hash, xsink);
	    return;
	 }
      }
   }
#endif

   char quote = 0;

   char *p = str->getBuffer();
   while (*p)
   {
      if (!quote && (*p) == '%') // found value marker
      {
	 p++;
	 if ((*p) != 'v')
	 {
	    xsink->raiseException("DBI-EXEC-PARSE-EXCEPTION", "invalid value specification (expecting '%v', got %%%c)", *p);
	    break;
	 }
	 p++;
	 if (isalpha(*p))
	 {
	    xsink->raiseException("DBI-EXEC-PARSE-EXCEPTION", "invalid value specification (expecting '%v', got %%v%c*)", *p);
	    break;
	 }
	 if (!args || args->size() <= len)
	 {
	    xsink->raiseException("DBI-EXEC-PARSE-EXCEPTION", "too few arguments passed (%d) for value expression (%d)",
			   args ? args->size() : 0, len + 1);
	    break;
	 }
	 class QoreNode *v = args->retrieve_entry(len);

	 // replace value marker with generated name
	 QoreString tn;
	 tn.sprintf(":qdodvrs___%d", len);
	 int offset = p - str->getBuffer() - 2;
	 str->replace(offset, 2, &tn);
	 p = str->getBuffer() + offset + tn.strlen();

	 printd(5, "BindGroup::parseQuery() newstr=%s\n", str->getBuffer());
	 printd(5, "BindGroup::parseQuery() adding value type=%s\n",v ? v->type->name : "<NULL>");
	 add(v, xsink);
      }
      else if (!quote && (*p) == ':') // found placeholder marker
      {
	 p++;
	 if (!isalpha(*p))
	    continue;

	 // get placeholder name
	 QoreString tstr;
	 while (isalnum(*p) || (*p) == '_')
	    tstr.concat(*(p++));

	 // check hash argument
	 class QoreNode *v;
	 // assume string if no argument passed
	 if (!args || args->size() <= len || !(v = args->retrieve_entry(len)))
	    add(tstr.giveBuffer(), -1, "string", xsink);
	 else if (v->type == NT_HASH)
	 {
	    // get and check data type
	    class QoreNode *t = v->val.hash->getKeyValue("type");
	    if (!t)
	    {
	       xsink->raiseException("DBI-EXEC-EXCEPTION", "missing 'type' key in placeholder hash");
	       break;	 
	    }
	    if (t->type != NT_STRING)
	    {
	       xsink->raiseException("DBI-EXEC-EXCEPTION", "expecting type name as value of 'type' key, got '%s'", t->type->name);
	       break;
	    }
	    
	    // get and check size
	    class QoreNode *sz = v->val.hash->getKeyValue("size");
	    int size = sz ? sz->getAsInt() : -1;
	    
	    printd(5, "BindGroup::parseQuery() adding placeholder name=%s, size=%d, type=%s\n", tstr.getBuffer(), size, t->val.String->getBuffer());
	    add(tstr.giveBuffer(), size, t->val.String->getBuffer(), xsink);
	 }
	 else if (v->type == NT_STRING)
	    add(tstr.giveBuffer(), -1, v->val.String->getBuffer(), xsink);
	 else if (v->type == NT_INT)
	    add(tstr.giveBuffer(), v->val.intval, "string", xsink);
	 else
	    xsink->raiseException("DBI-EXEC-EXCEPTION", "expecting string or hash for placeholder description, got '%s'", v->type->name);
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
}

void BindNode::bindValue(class Datasource *ds, OCIStmt *stmthp, int pos, class ExceptionSink *xsink)
{
   class OracleData *d_ora = (OracleData *)ds->private_data;
   OCIBind *bndp = NULL;
   ind = 0;

   //printd(5, "BindNode::bindValue() type=%s\n", data.v.value ? data.v.value->type->name : "NOTHING");

   // bind a NULL
   if (is_nothing(data.v.value) || is_null(data.v.value))
   {
      ora_checkerr(d_ora->errhp, OCIBindByPos(stmthp, &bndp, d_ora->errhp, pos, NULL, 0, SQLT_STR, (dvoid *)NULL, (ub2 *)NULL, (ub2 *)NULL, (ub4)0, (ub4 *)NULL, OCI_DEFAULT), 
		   "BindNode::bindValue()", ds, xsink);      
   }
   else if (data.v.value->type == NT_STRING)
   {
      buftype = SQLT_STR;

      class QoreString *bstr = data.v.value->val.String;
      // convert to the db charset if necessary
      if (bstr->getEncoding() != ds->qorecharset)
      {
	 bstr = bstr->convertEncoding(ds->qorecharset, xsink);
	 if (xsink->isEvent())
	    return;
	 // save temporary string for later deleting
	 data.v.tstr = bstr;
      }

      // bind value to buffer
      buf.ptr = bstr->getBuffer();

      // bind it
      ora_checkerr(d_ora->errhp, OCIBindByPos(stmthp, &bndp, d_ora->errhp, pos, buf.ptr, bstr->strlen() + 1, SQLT_STR, (dvoid *)NULL, (ub2 *)NULL, (ub2 *)NULL, (ub4)0, (ub4 *)NULL, OCI_DEFAULT), 
		   "BindNode::bindValue()", ds, xsink);
   }
   else if (data.v.value->type == NT_DATE)
   {
      buftype = SQLT_DAT;
      buf.ptr = make_oracle_date_time(data.v.value->val.date_time);
      // bind it
      ora_checkerr(d_ora->errhp, OCIBindByPos(stmthp, &bndp, d_ora->errhp, pos, buf.ptr, 7, SQLT_DAT, (dvoid *)NULL, (ub2 *)NULL, (ub2 *)NULL, (ub4)0, (ub4 *)NULL, OCI_DEFAULT), 
		   "BindNode::bindValue()", ds, xsink);      
   }
   else if (data.v.value->type == NT_BINARY)
   {
      printd(5, "BindNode::bindValue() BLOB ptr=%08x size=%d\n", data.v.value->val.bin->getPtr(), data.v.value->val.bin->size());
      ora_checkerr(d_ora->errhp, OCIBindByPos(stmthp, &bndp, d_ora->errhp, pos, data.v.value->val.bin->getPtr(), data.v.value->val.bin->size(), SQLT_BIN, (dvoid *)NULL, (ub2 *)NULL, (ub2 *)NULL, (ub4)0, (ub4 *)NULL, OCI_DEFAULT), 
		   "BindNode::bindValue()", ds, xsink);      
   }
   else if (data.v.value->type == NT_BOOLEAN)
   {
      buf.i4 = data.v.value->val.boolval;
      ora_checkerr(d_ora->errhp, OCIBindByPos(stmthp, &bndp, d_ora->errhp, pos, &buf.i4, sizeof(int), SQLT_INT, (dvoid *)NULL, (ub2 *)NULL, (ub2 *)NULL, (ub4)0, (ub4 *)NULL, OCI_DEFAULT), "BindNode::bindValue()", ds, xsink);
   }
   else if (data.v.value->type == NT_INT)
   {
      if (data.v.value->val.intval <= MAXINT32 && data.v.value->val.intval >= -MAXINT32)
      {
	 buf.i4 = data.v.value->val.intval;
	 ora_checkerr(d_ora->errhp, OCIBindByPos(stmthp, &bndp, d_ora->errhp, pos, &buf.i4, sizeof(int), SQLT_INT, (dvoid *)NULL, (ub2 *)NULL, (ub2 *)NULL, (ub4)0, (ub4 *)NULL, OCI_DEFAULT), "BindNode::bindValue()", ds, xsink);
      }
      else // bind as a string value
      {
	 buftype = SQLT_STR;

	 QoreString *tstr = new QoreString();
	 tstr->sprintf("%lld", data.v.value->val.intval);
	 data.v.tstr = tstr;

	 //printd(5, "binding number '%s'\n", buf.ptr);
	 ora_checkerr(d_ora->errhp, OCIBindByPos(stmthp, &bndp, d_ora->errhp, pos, (char *)tstr->getBuffer(), tstr->strlen() + 1, SQLT_STR, (dvoid *)NULL, (ub2 *)NULL, (ub2 *)NULL, (ub4)0, (ub4 *)NULL, OCI_DEFAULT), "BindNode::bindValue()", ds, xsink);
      }
   }
   else if (data.v.value->type == NT_FLOAT)
   {
      ora_checkerr(d_ora->errhp, 
		   OCIBindByPos(stmthp, &bndp, d_ora->errhp, pos, &data.v.value->val.floatval, sizeof(double), SQLT_FLT, (dvoid *)NULL, (ub2 *)NULL, (ub2 *)NULL, (ub4)0, (ub4 *)NULL, OCI_DEFAULT), "BindNode::bindValue()", ds, xsink);
   }
   else
      xsink->raiseException("DBI-EXEC-EXCEPTION", "type '%s' is not supported for SQL binding", data.v.value->type->name);
}

void BindNode::bindPlaceholder(class Datasource *ds, OCIStmt *stmthp, int pos, class ExceptionSink *xsink)
{
   class OracleData *d_ora = (OracleData *)ds->private_data;
   OCIBind *bndp = NULL;

   printd(5, "BindNode::bindPlaceholder(ds=%08x, pos=%d) type=%s, size=%d)\n", ds, pos, data.ph.type, data.ph.maxsize);

   if (!strcmp(data.ph.type, "string"))
   {
      if (data.ph.maxsize == -1)
	 data.ph.maxsize = DEFAULT_STR_LEN;

      // simply malloc some space for sending to the new node
      buftype = SQLT_STR;
      buf.ptr = malloc(sizeof(char) * (data.ph.maxsize + 1));
      ((char *)buf.ptr)[0] = '\0';

      ora_checkerr(d_ora->errhp, OCIBindByPos(stmthp, &bndp, d_ora->errhp, pos, buf.ptr, data.ph.maxsize + 1, SQLT_STR, &ind, (ub2 *)NULL, (ub2 *)NULL, (ub4)0, (ub4 *)NULL, OCI_DEFAULT), "BindNode::bindPlaceholder()", ds, xsink);
   }
   else if (!strcmp(data.ph.type, "date"))
   {
      // simply malloc some space for sending to the new node
      buftype = SQLT_DAT;
      buf.ptr = malloc(sizeof(char) * 7);

      ora_checkerr(d_ora->errhp, OCIBindByPos(stmthp, &bndp, d_ora->errhp, pos, buf.ptr, 7, SQLT_DAT, &ind, (ub2 *)NULL, (ub2 *)NULL, (ub4)0, (ub4 *)NULL, OCI_DEFAULT), "BindNode::bindPlaceholder()", ds, xsink);
   }
   else if (!strcmp(data.ph.type, "clob"))
   {
      buftype = SQLT_CLOB;
      buf.ptr = NULL;

      ora_checkerr(d_ora->errhp, OCIDescriptorAlloc(d_ora->envhp, &buf.ptr, OCI_DTYPE_LOB, 0, NULL), "BindNode::bindPlaceholder()", ds, xsink);

      if (xsink->isEvent()) return;
      printd(5, "bindPalceholder() got LOB locator handle %08x\n", buf.ptr);
      ora_checkerr(d_ora->errhp, OCIBindByPos(stmthp, &bndp, d_ora->errhp, pos, &buf.ptr, 0, SQLT_CLOB, &ind, (ub2 *)NULL, (ub2 *)NULL, (ub4)0, (ub4 *)NULL, OCI_DEFAULT), "BindNode::bindPlaceholder()", ds, xsink);
   }
   else if (!strcmp(data.ph.type, "binary"))
   {
      buftype = SQLT_BLOB;
      buf.ptr = NULL;

      ora_checkerr(d_ora->errhp, OCIDescriptorAlloc(d_ora->envhp, &buf.ptr, OCI_DTYPE_LOB, 0, NULL), "BindNode::bindPlaceholder()", ds, xsink);

      if (xsink->isEvent()) return;
      printd(5, "bindPalceholder() got LOB locator handle %08x\n", buf.ptr);
      ora_checkerr(d_ora->errhp, OCIBindByPos(stmthp, &bndp, d_ora->errhp, pos, &buf.ptr, 0, SQLT_BLOB, &ind, (ub2 *)NULL, (ub2 *)NULL, (ub4)0, (ub4 *)NULL, OCI_DEFAULT), "BindNode::bindPlaceholder()", ds, xsink);
   }
   else if (!strcmp(data.ph.type, "integer"))
   {
      buftype = SQLT_INT;
      ora_checkerr(d_ora->errhp, OCIBindByPos(stmthp, &bndp, d_ora->errhp, pos, &buf.i8, sizeof(int64), SQLT_INT, &ind, (ub2 *)NULL, (ub2 *)NULL, (ub4)0, (ub4 *)NULL, OCI_DEFAULT), "BindNode::bindPlaceholder()", ds, xsink);
   }
   else if (!strcmp(data.ph.type, "float"))
   {
      buftype = SQLT_FLT;
      ora_checkerr(d_ora->errhp, OCIBindByPos(stmthp, &bndp, d_ora->errhp, pos, &buf.f8, sizeof(double), SQLT_FLT, &ind, (ub2 *)NULL, (ub2 *)NULL, (ub4)0, (ub4 *)NULL, OCI_DEFAULT), "BindNode::bindPlaceholder()", ds, xsink);
   }
   else if (!strcmp(data.ph.type, "hash"))
   {
      buftype = SQLT_RSET;

      // allocate statement handle for result list
      ora_checkerr(d_ora->errhp, OCIHandleAlloc(d_ora->envhp, (dvoid **)&buf.ptr, OCI_HTYPE_STMT, 0, 0), "BindNode::bindPlaceHolder()", ds, xsink);

      if (!xsink->isEvent())
	 ora_checkerr(d_ora->errhp, OCIBindByPos(stmthp, &bndp, d_ora->errhp, pos, &buf.ptr, 0, SQLT_RSET, &ind, (ub2 *)NULL, (ub2 *)NULL, (ub4)0, (ub4 *)NULL, OCI_DEFAULT), "BindNode::bindPlaceholder()", ds, xsink);
      else
	 buf.ptr = NULL;
   }
   else
      xsink->raiseException("DBI-EXEC-EXCEPTION", "type '%s' is not supported for SQL binding", data.ph.type);
}

class QoreNode *BindNode::getValue(class Datasource *ds, class ExceptionSink *xsink)
{
   // if NULL, then return NULL
   if (ind == -1)
      return null();

   if (buftype == SQLT_STR)
   {
      // must be string data
      remove_trailing_blanks((char *)buf.ptr);
      class QoreString *str = new QoreString();
      str->take(ds->qorecharset, (char *)buf.ptr);
      buf.ptr = NULL;
      return new QoreNode(str);
   }
   else if (buftype == SQLT_DAT)
      return new QoreNode(convert_date_time((unsigned char *)buf.ptr));
   else if (buftype == SQLT_INT)
      return new QoreNode(buf.i8);
   else if (buftype == SQLT_FLT)
      return new QoreNode(buf.f8);
   else if (buftype == SQLT_RSET)
   {
      class Hash *h = ora_fetch((OCIStmt *)buf.ptr, ds, xsink);
      return h ? new QoreNode(h) : NULL;
   }
   else if (buftype == SQLT_CLOB || buftype == SQLT_BLOB)
   {
      // get oracle data
      class OracleData *d_ora = (OracleData *)ds->private_data;

      printd(5, "BindNode::getValue() using LOB locator handle %08x\n", buf.ptr);

      // retrieve *LOB data
      void *bbuf = malloc(LOB_BLOCK_SIZE);
      class QoreNode *rv;
      ub4 amt = 0;
      if (buftype == SQLT_CLOB)
      {
	 class QoreString *str = new QoreString(ds->qorecharset, "");
	 // read LOB data in streaming callback mode
	 ora_checkerr(d_ora->errhp,
		      OCILobRead(d_ora->svchp, d_ora->errhp, (OCILobLocator *)buf.ptr, &amt, 1, bbuf, LOB_BLOCK_SIZE,
				 str, read_clob_callback, (ub2)d_ora->charsetid, (ub1)0), "oraReadCLOBCallback()", ds, xsink);
	 rv = new QoreNode(str);
      }
      else
      {
	 BinaryObject *b = new BinaryObject();
	 // read LOB data in streaming callback mode
	 ora_checkerr(d_ora->errhp,
		      OCILobRead(d_ora->svchp, d_ora->errhp, (OCILobLocator *)buf.ptr, &amt, 1, bbuf, LOB_BLOCK_SIZE,
				 b, read_blob_callback, (ub2)0, (ub1)0), "oraReadBLOBCallback()", ds, xsink);
	 rv = new QoreNode(b);    
      }
      free(bbuf);
      return rv;
   }
   return NULL;
}

class QoreNode *BindGroup::getOutputHash(class ExceptionSink *xsink)
{
   class Hash *h = new Hash();
   
   class BindNode *w = head;
   while (w)
   {
      if (w->bindtype == BV_PLACEHOLDER)
	 h->setKeyValue(w->data.ph.name, w->getValue(ds, xsink), xsink);
      w = w->next;
   }
   return new QoreNode(h);
}

class QoreNode *BindGroup::exec(class ExceptionSink *xsink)
{
   class QoreNode *rv;

   class OracleData *d_ora = (OracleData *)ds->private_data;
   int status = OCIStmtExecute(d_ora->svchp, stmthp, d_ora->errhp, 1, 0, NULL, NULL, OCI_DEFAULT);
   if (status)
      ora_checkerr(d_ora->errhp, status, "BindGroup::exec()", ds, xsink);

   if (!xsink->isEvent())
   {
      // if there are output variables, then fix values if necessary and return
      if (hasOutput)
	 rv = getOutputHash(xsink);
      else // get row count
      {
	 int rc = 0;
	 ora_checkerr(d_ora->errhp, OCIAttrGet(stmthp, OCI_HTYPE_STMT, &rc, 0, OCI_ATTR_ROW_COUNT, d_ora->errhp), "BindGroup::exec():attr", ds, xsink);
	 rv = new QoreNode((int64)rc);
      }
      
      // commit transaction if autocommit set for datasource
      if (ds->getAutoCommit())
	 if ((status = OCITransCommit(d_ora->svchp, d_ora->errhp, (ub4) 0)))
	    ora_checkerr(d_ora->errhp, status, "BindGroup():commit", ds, xsink);
   }   
   else
      rv = NULL;

   return rv;
}

static class QoreNode *oracle_exec(class Datasource *ds, QoreString *qstr, List *args, class ExceptionSink *xsink)
{
   QoreNode *rv = NULL;

   tracein("oracle_exec()");

   class BindGroup *bg = new BindGroup(ds, qstr, args, xsink);

   if (!xsink->isEvent())
      rv = bg->exec(xsink);

   delete bg;

   traceout("oracle_exec()");
   return rv;
}

static int oracle_open(Datasource *ds, ExceptionSink *xsink)
{
   tracein("oracle_open()");

   printd(5, "oracle_open() datasource %08x for DB=%s open\n", ds, ds->dbname);

   if (!ds->username)
   {
      xsink->raiseException("DATASOURCE-MISSING-USERNAME", "Datasource has an empty username parameter");
      traceout("oracle_open()");
      return -1;
   }
   if (!ds->password)
   {
      xsink->raiseException("DATASOURCE-MISSING-PASSWORD", "Datasource has an empty password parameter");
      traceout("oracle_open()");
      return -1;
   }
   if (!ds->dbname)
   {
      xsink->raiseException("DATASOURCE-MISSING-DBNAME", "Datasource has an empty dbname parameter");
      traceout("oracle_open()");
      return -1;
   }
   printd(3, "oracle_open(): user=%s pass=%s db=%s (charset=%s)\n",
	  ds->username, ds->password, ds->dbname, ds->charset ? ds->charset : "(none)");

   class OracleData *d_ora = new OracleData;
   ds->private_data = (void *)d_ora;

   int oci_flags = OCI_DEFAULT|OCI_THREADED;

   char *charset;

   // FIXME: maybe I don't need a temporary environment handle?
   // create temporary environment handle
   OCIEnv *tmpenvhp;
   OCIEnvCreate(&tmpenvhp, oci_flags | OCI_NO_UCB, 0, 0, 0, 0, 0, 0);
   // declare temporary buffer
   char nbuf[OCI_NLS_MAXBUFSZ];
   int need_to_set_charset = 0;

   if (ds->charset && ds->charset[0])
   {
      charset = ds->charset;
      need_to_set_charset = 1;
   }
   else // get Oracle character set name from OS character set name
   {
      if ((OCINlsNameMap(tmpenvhp, (oratext *)nbuf, OCI_NLS_MAXBUFSZ, (oratext *)QCS_DEFAULT->code, OCI_NLS_CS_IANA_TO_ORA) != OCI_SUCCESS))
      {
	 OCIHandleFree(tmpenvhp, OCI_HTYPE_ENV);
	 xsink->raiseException("DBI:ORACLE:UNKNOWN-CHARACTER-SET", 
			"cannot map default OS charset '%s' to Oracle character set",
			QCS_DEFAULT->code);
	 delete d_ora;
	 ds->private_data = NULL;
	 traceout("oracle_open()");
	 return -1;
      }
      ds->qorecharset = QCS_DEFAULT;

      if (ds->charset)
	 free(ds->charset);

      ds->charset = strdup(nbuf);
      charset = nbuf;
      printd(5, "oracle_open() setting Oracle charset to '%s' from default OS charset '%s'\n",
	     charset, QCS_DEFAULT->code);
   }

#ifdef HAVE_OCIENVNLSCREATE
   // get character set ID
   d_ora->charsetid = OCINlsCharSetNameToId(tmpenvhp, (oratext *)charset);
   // delete temporary environmental handle
   OCIHandleFree(tmpenvhp, OCI_HTYPE_ENV);

   if (!d_ora->charsetid)
   {
      xsink->raiseException("DBI:ORACLE:UNKNOWN-CHARACTER-SET", "this installation of Oracle does not support the '%s' character set", 
		     ds->charset);
      delete d_ora;
      ds->private_data = NULL;
      traceout("oracle_open()");
      return -1;
   }
   printd(5, "Oracle character set '%s' has ID %d\n", charset, d_ora->charsetid);
   // create environment with default character set
   OCIEnvNlsCreate(&d_ora->envhp, oci_flags, 0, NULL, NULL, NULL, 0, NULL, d_ora->charsetid, d_ora->charsetid);
   // map the Oracle character set to a core character set

   if (need_to_set_charset)
   {
      // map Oracle character set name to QORE character set
      if ((OCINlsNameMap(d_ora->envhp, (oratext *)nbuf, OCI_NLS_MAXBUFSZ, (oratext *)ds->charset, OCI_NLS_CS_ORA_TO_IANA) == OCI_SUCCESS))
      {
	 printd(5, "oracle_open() Oracle character set '%s' mapped to '%s' character set\n", ds->charset, nbuf);
	 ds->qorecharset = QEM.findCreate(nbuf);
      }
#ifdef DEBUG
      else
      {
	 run_time_error("oracle_open(): can't map Oracle character set '%s' to OS character set\n", ds->charset);
	 traceout("oracle_open()");
	 leave(1);
      }
#endif
   }

#else // !HAVE_OCIENVNLSCREATE
#error need to define HAVE_OCIENVNLSCREATE (with Oracle 9i+)
/*
   d_ora->charsetid = 0;
   if (ds->charset)
   {
      xsink->raiseException("DBI:ORACLE:NO_OCIENVCREATE", "compile-time options do not support Oracle character set specifications");
      delete d_ora;
      ds->private_data = NULL;
      return -1;
   }
# ifdef HAVE_OCIENVCREATE
   OCIEnvCreate(&d_ora->envhp, oci_flags | OCI_NO_UCB, 0, 0, 0, 0, 0, 0);
# else
   OCIInitialize((ub4)oci_flags, 0, 0, 0, 0);
   OCIEnvInit(&d_ora->envhp, (ub4) OCI_DEFAULT, 0, 0);
# endif
*/
#endif // HAVE_OCIENVNLSCREATE
   OCIHandleAlloc(d_ora->envhp, (dvoid **) &d_ora->errhp, OCI_HTYPE_ERROR, 0, 0);
   ora_checkerr(d_ora->errhp, 
		OCILogon(d_ora->envhp, d_ora->errhp, &d_ora->svchp, (text *)ds->username, strlen(ds->username), (text *)ds->password, strlen(ds->password), (text *)ds->dbname, strlen(ds->dbname)), 
		"<open>", ds, xsink);
   if (xsink->isEvent())
   {
      delete d_ora;
      ds->private_data = NULL;
      traceout("oracle_open()");
      return -1;
   }

   printd(5, "oracle_open() datasource %08x for DB=%s open (envhp=%08x)\n", ds, ds->dbname, d_ora->envhp);

/*
   OCIStmt *stmthp;
   char *date_query = "alter session set nls_date_format = 'YYYYMMDDHH24MISS'";

   // set default date format YYYYMMDDHH24MISS
   ora_checkerr(d_ora->errhp, 
		OCIHandleAlloc(d_ora->envhp, (dvoid **)&stmthp, OCI_HTYPE_STMT, 0, 0), 
		"<open>", ds, xsink);
   if (xsink->isEvent())
   {
      delete d_ora;
      ds->private_data = NULL;
      traceout("oracle_open()");
      return -1;
   }
   ora_checkerr(d_ora->errhp, 
		OCIStmtPrepare(stmthp, d_ora->errhp, (text *)date_query, strlen(date_query), OCI_NTV_SYNTAX, OCI_DEFAULT), 
		"<open>", ds, xsink);
   if (xsink->isEvent())
   {
      delete d_ora;
      ds->private_data = NULL;
   }
   else
   {
      ora_checkerr(d_ora->errhp, 
		   OCIStmtExecute(d_ora->svchp, stmthp, d_ora->errhp, 1, 0, NULL, NULL, OCI_DEFAULT), 
		   "<open>", ds, xsink);
   }
   OCIHandleFree(stmthp, OCI_HTYPE_STMT);
*/
   
   traceout("oracle_open()");
   return 0;
}

static int oracle_close(class Datasource *ds)
{
   tracein("oracle_close()");

   class OracleData *d_ora = (OracleData *)ds->private_data;

   printd(3, "oracle_close(): connection to %s closed.\n", ds->dbname);
   OCILogoff(d_ora->svchp, d_ora->errhp);
   OCIHandleFree(d_ora->svchp, OCI_HTYPE_SVCCTX);
   OCIHandleFree(d_ora->errhp, OCI_HTYPE_ERROR);
   OCIHandleFree(d_ora->envhp, OCI_HTYPE_ENV);
   delete d_ora;
   ds->private_data = NULL;

   traceout("oracle_close()");
   return 0;
}

char *oracle_module_init()
{
   tracein("oracle_module_init()");

   // register driver with DBI subsystem
   DBIDriverFunctions *ddf = 
      new DBIDriverFunctions(oracle_open, 
			     oracle_close,
			     oracle_select, 
			     oracle_select_rows, 
			     oracle_exec, 
			     oracle_describe,
			     oracle_commit, 
			     oracle_rollback);
   DBID_ORACLE = DBI.registerDriver("oracle", ddf, DBI_ORACLE_CAPS);

   traceout("oracle_module_init()");
   return NULL;
}

void oracle_module_ns_init(class Namespace *rns, class Namespace *qns)
{
   tracein("oracle_module_ns_init()");
   // nothing to do at the moment
   traceout("oracle_module_ns_init()");
}


void oracle_module_delete()
{
   tracein("oracle_module_delete()");
   //DBI_deregisterDriver(DBID_ORACLE);
   traceout("oracle_module_delete()");
}
