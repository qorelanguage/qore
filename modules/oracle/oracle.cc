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

#include <qore/Qore.h>
#include <qore/DBI.h>

#include "oracle.h"
#include "oracle-module.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <strings.h>
#include <assert.h>

#ifndef QORE_MONOLITHIC
DLLEXPORT char qore_module_name[] = "oracle";
DLLEXPORT char qore_module_version[] = "0.6";
DLLEXPORT char qore_module_description[] = "Oracle database driver";
DLLEXPORT char qore_module_author[] = "David Nichols";
DLLEXPORT char qore_module_url[] = "http://qore.sourceforge.net";
DLLEXPORT int qore_module_api_major = QORE_MODULE_API_MAJOR;
DLLEXPORT int qore_module_api_minor = QORE_MODULE_API_MINOR;
DLLEXPORT qore_module_init_t qore_module_init = oracle_module_init;
DLLEXPORT qore_module_ns_init_t qore_module_ns_init = oracle_module_ns_init;
DLLEXPORT qore_module_delete_t qore_module_delete = oracle_module_delete;
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

static void ora_checkerr(OCIError *errhp, sword status, const char *query_name, Datasource *ds, ExceptionSink *xsink)
{
   text errbuf[512];
   sb4 errcode = 0;

   //printd(5, "ora_checkerr(%08p, %d, %s, isEvent=%d)\n", errhp, status, query_name ? query_name : "none", xsink->isEvent());
   switch (status)
   {
      case OCI_SUCCESS:
      case OCI_SUCCESS_WITH_INFO:
	 // ignore SUCCESS_WITH_INFO codes
	 break;

      case OCI_ERROR:
	 OCIErrorGet((dvoid *)errhp, (ub4) 1, (text *) NULL, &errcode, errbuf, (ub4) sizeof(errbuf), OCI_HTYPE_ERROR);
	 if (query_name)
	    xsink->raiseException("DBI:ORACLE:OCI-ERROR", "%s@%s: %s: %s", ds->getUsername(), ds->getDBName(), query_name, remove_trailing_newlines((char *)errbuf));
	 else
	    xsink->raiseException("DBI:ORACLE:OCI-ERROR", "%s@%s: %s", ds->getUsername(), ds->getDBName(), remove_trailing_newlines((char *)errbuf));
	 break;
      case OCI_INVALID_HANDLE:
	 xsink->raiseException("DBI:ORACLE:OCI-INVALID-HANDLE", "%s@%s: %s", ds->getUsername(), ds->getDBName(), remove_trailing_newlines((char *)errbuf));
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

   class OracleData *d_ora = (OracleData *)ds->getPrivateData();

   // commit transaction
   if ((status = OCITransCommit(d_ora->svchp, d_ora->errhp, (ub4) 0)))
      ora_checkerr(d_ora->errhp, status, "Oracle commit transaction", ds, xsink);
   return 0;
}

static int oracle_rollback(class Datasource *ds, ExceptionSink *xsink)
{
   int status;

   class OracleData *d_ora = (OracleData *)ds->getPrivateData();
   if ((status = OCITransRollback(d_ora->svchp, d_ora->errhp, (ub4) 0)))
      ora_checkerr(d_ora->errhp, status, "Oracle rollback transaction", ds, xsink);
   return 0;
}

OraColumns::OraColumns(OCIStmt *stmthp, class Datasource *ds, const char *str, ExceptionSink *xsink)
{
   tracein("OraColumns::OraColumns()");

   len = 0;
   head = tail = NULL;

   class OracleData *d_ora = (OracleData *)ds->getPrivateData();

   // retrieve results, if any
   //OCIParam *parmp;
   void *parmp;

   // get columns in output
   while (OCIParamGet(stmthp, OCI_HTYPE_STMT, d_ora->errhp, &parmp, size() + 1) == OCI_SUCCESS)
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

   traceout("OraColumns::OraColumns()");
}

void OraColumns::define(OCIStmt *stmthp, class Datasource *ds, const char *str, ExceptionSink *xsink)
{
   //tracein("OraColumne::define()");

   class OracleData *d_ora = (OracleData *)ds->getPrivateData();

   // iterate column list
   class OraColumn *w = head;
   int i = 0;
   while (w)
   {
      //printd(5, "w->dtype=%d\n", w->dtype);
      switch (w->dtype)
      {
	 case SQLT_INT:
	 case SQLT_UIN:
	    w->val.i8 = 0;
	    ora_checkerr(d_ora->errhp,
			 OCIDefineByPos(stmthp, &w->defp, d_ora->errhp, i + 1, &w->val.i8, sizeof(int64), SQLT_INT, &w->ind, 0, 0, OCI_DEFAULT), 
			 str, ds, xsink);
	    break;

	 case SQLT_FLT:
	 case SQLT_BFLOAT:
	 case SQLT_BDOUBLE:
	 case SQLT_IBFLOAT:
	 case SQLT_IBDOUBLE:
	    ora_checkerr(d_ora->errhp,
			 OCIDefineByPos(stmthp, &w->defp, d_ora->errhp, i + 1, &w->val.f8, sizeof(double), SQLT_BDOUBLE, &w->ind, 0, 0, OCI_DEFAULT), 
			 str, ds, xsink);
	    break;

	 case SQLT_DAT:
	    ora_checkerr(d_ora->errhp,
			 OCIDefineByPos(stmthp, &w->defp, d_ora->errhp, i + 1, w->val.date, 7, SQLT_DAT, &w->ind, 0, 0, OCI_DEFAULT), 
			 str, ds, xsink);
	    break;

	 case SQLT_TIMESTAMP:
	 case SQLT_TIMESTAMP_TZ:
	 case SQLT_TIMESTAMP_LTZ:
	 case SQLT_DATE:
	    w->val.odt = NULL;
	    ora_checkerr(d_ora->errhp,
			 OCIDescriptorAlloc(d_ora->envhp, (dvoid **)&w->val.odt, OCI_DTYPE_TIMESTAMP, 0, NULL), str, ds, xsink);
	    if (*xsink)
	       return;
	    //printd(5, "OraColumns::define() got TIMESTAMP handle %08p\n", w->val.odt);
	    ora_checkerr(d_ora->errhp,
			 OCIDefineByPos(stmthp, &w->defp, d_ora->errhp, i + 1, &w->val.odt, sizeof(w->val.odt), SQLT_TIMESTAMP, &w->ind, 0, 0, OCI_DEFAULT), 
			 str, ds, xsink);
	    break;

	 case SQLT_INTERVAL_YM:
	    w->val.oi = NULL;
	    ora_checkerr(d_ora->errhp,
			 OCIDescriptorAlloc(d_ora->envhp, (dvoid **)&w->val.oi, OCI_DTYPE_INTERVAL_YM, 0, NULL), str, ds, xsink);
	    if (*xsink)
	       return;
	    //printd(5, "OraColumns::define() got TIMESTAMP handle %08p\n", w->val.odt);
	    ora_checkerr(d_ora->errhp,
			 OCIDefineByPos(stmthp, &w->defp, d_ora->errhp, i + 1, &w->val.oi, sizeof(w->val.oi), SQLT_INTERVAL_YM, &w->ind, 0, 0, OCI_DEFAULT), 
			 str, ds, xsink);
	    break;

	 case SQLT_INTERVAL_DS:
	    w->val.oi = NULL;
	    ora_checkerr(d_ora->errhp,
			 OCIDescriptorAlloc(d_ora->envhp, (dvoid **)&w->val.oi, OCI_DTYPE_INTERVAL_DS, 0, NULL), str, ds, xsink);
	    if (*xsink)
	       return;
	    //printd(5, "OraColumns::define() got TIMESTAMP handle %08p\n", w->val.odt);
	    ora_checkerr(d_ora->errhp,
			 OCIDefineByPos(stmthp, &w->defp, d_ora->errhp, i + 1, &w->val.oi, sizeof(w->val.oi), SQLT_INTERVAL_DS, &w->ind, 0, 0, OCI_DEFAULT), 
			 str, ds, xsink);
	    break;

	 // handle raw data
	 case SQLT_BIN:
	 case SQLT_LBI:
	 {
	    int size = w->maxsize + sizeof(int);
	    w->val.ptr = malloc(size);
	    w->dtype = SQLT_LVB;
	    ora_checkerr(d_ora->errhp,
			 OCIDefineByPos(stmthp, &w->defp, d_ora->errhp, i + 1, w->val.ptr, size, SQLT_LVB, &w->ind, 0, 0, OCI_DEFAULT), 
			 str, ds, xsink);
	    break;
	 }

	 case SQLT_BLOB:
	 case SQLT_CLOB:
	    w->val.ptr = NULL;
	    ora_checkerr(d_ora->errhp, 
			 OCIDescriptorAlloc(d_ora->envhp, &w->val.ptr, OCI_DTYPE_LOB, 0, NULL), str, ds, xsink);
	    if (xsink->isEvent()) return;
	    printd(5, "OraColumns::define() got LOB locator handle %08p\n", w->val.ptr);
	    ora_checkerr(d_ora->errhp,
			 OCIDefineByPos(stmthp, &w->defp, d_ora->errhp, i + 1, &w->val.ptr, 0, w->dtype, &w->ind, 0, 0, OCI_DEFAULT), 
			 str, ds, xsink);
	    break;

	 default: // treated as a string
	    w->val.ptr = malloc(sizeof(char) * (w->maxsize + 1));
	    ora_checkerr(d_ora->errhp,
			 OCIDefineByPos(stmthp, &w->defp, d_ora->errhp, i + 1, w->val.ptr, w->maxsize + 1, SQLT_STR, &w->ind, 0, 0, OCI_DEFAULT), 
			 str, ds, xsink);
      }
      if (*xsink) return;
      w = w->next;
      i++;
   }
   //traceout("OraColumns::define()");
}

static class DateTime *convert_date_time(unsigned char *str)
{
   int year;
   if ((str[0] < 100) || (str[1] < 100))
      year = 9999; 
   else
      year = (str[0] - 100) * 100 + (str[1] - 100);

   //printd(5, "convert_date_time(): %d %d = %04d-%02d-%02d %02d:%02d:%02d\n", str[0], str[1], dt->year, dt->month, dt->day, dt->hour, dt->minute, dt->second);
   return new DateTime(year, str[2], str[3], str[4] - 1, str[5] - 1, str[6] - 1);
}

/*
static void *make_oracle_date_time(class DateTime *d)
{
   char *buf = (char *)malloc(7 * sizeof(char));

   buf[0] = d->getYear() / 100 + 100;
   buf[1] = d->getYear() % 100 + 100;
   buf[2] = d->getMonth();
   buf[3] = d->getDay();
   buf[4] = d->getHour() + 1;
   buf[5] = d->getMinute() + 1;
   buf[6] = d->getSecond() + 1;
   //printd(5, "make_oracle_date_time(): %04d-%02d-%02d %02d:%02d:%02d = %d %d\n", d->year, d->month, d->day, d->hour, d->minute, d->second, buf[0], buf[1]);
   return buf;
}
*/

extern "C" sb4 read_clob_callback(void *sp, CONST dvoid *bufp, ub4 len, ub1 piece)
{
   printd(5, "read_clob_callback(%08p, %08p, %d, %d)\n", sp, bufp, len, piece);
   ((class QoreString *)sp)->concat((char *)bufp, len);
   return OCI_CONTINUE;
}

extern "C" sb4 read_blob_callback(void *bp, CONST dvoid *bufp, ub4 len, ub1 piece)
{
   printd(5, "read_blob_callback(%08p, %08p, %d, %d)\n", bp, bufp, len, piece);
   ((class BinaryObject *)bp)->append((char *)bufp, len);
   return OCI_CONTINUE;
}


class QoreNode *get_oracle_timestamp(Datasource *ds, OCIDateTime *odt, class ExceptionSink *xsink)
{
   class OracleData *d_ora = (OracleData *)ds->getPrivateData();
   //printd(5, "OraColumn::getValue() using TIMESTAMP handle %08p\n", val.odt);

   sb2 year;
   ub1 month, day;
   ora_checkerr(d_ora->errhp, 
		OCIDateTimeGetDate(d_ora->envhp, d_ora->errhp, odt, &year, &month, &day),
		"OCIDateTimeGetDate()", ds, xsink);

   if (*xsink)
      return NULL;

   ub1 hour, minute, second;
   ub4 us; // microseconds
   ora_checkerr(d_ora->errhp, 
		OCIDateTimeGetTime(d_ora->envhp, d_ora->errhp, odt, &hour, &minute, &second, &us),
		"OCIDateTimeGetTime()", ds, xsink);
   if (*xsink)
      return NULL;

   return new QoreNode(new DateTime(year, month, day, hour, minute, second, us / 1000));
}

class QoreNode *OraColumn::getValue(class Datasource *ds, class ExceptionSink *xsink)
{
   //printd(5, "OraColumn::getValue() dtype=%d, ind=%d, maxsize=%d\n", dtype, ind, maxsize);

   if (ind == -1)      // SQL NULL returned
      return null();

   switch (dtype)
   {
      case SQLT_INT:
      case SQLT_UIN:
	 return new QoreNode(val.i8);

      case SQLT_FLT:
      case SQLT_BFLOAT:
      case SQLT_BDOUBLE:
      case SQLT_IBFLOAT:
      case SQLT_IBDOUBLE:
	 return new QoreNode(val.f8);

      case SQLT_DAT:
	 return new QoreNode(convert_date_time(val.date));

      case SQLT_TIMESTAMP:
      case SQLT_TIMESTAMP_TZ:
      case SQLT_TIMESTAMP_LTZ:
      case SQLT_DATE:
	 return get_oracle_timestamp(ds, val.odt, xsink);

      case SQLT_INTERVAL_YM:
      {
	 // get oracle data
	 class OracleData *d_ora = (OracleData *)ds->getPrivateData();

	 //printd(5, "OraColumn::getValue() using INTERVAL_YM handle %08p\n", val.oi);

	 sb4 year, month;
	 ora_checkerr(d_ora->errhp, 
		      OCIIntervalGetYearMonth(d_ora->envhp, d_ora->errhp, &year, &month, val.oi),
		      "OCIIntervalGetYearMonth()", ds, xsink);
	 
	 return (*xsink ? NULL :
		 new QoreNode(new DateTime(year, month, 0, 0, 0, 0, 0, true)));
      }

      case SQLT_INTERVAL_DS:
      {
	 // get oracle data
	 class OracleData *d_ora = (OracleData *)ds->getPrivateData();

	 //printd(5, "OraColumn::getValue() using INTERVAL_DS handle %08p\n", val.oi);

	 sb4 day, hour, minute, second, microsecond;
	 ora_checkerr(d_ora->errhp, 
		      OCIIntervalGetDaySecond(d_ora->envhp, d_ora->errhp, &day, &hour, &minute, &second, &microsecond, val.oi),
		      "OCIIntervalGetDaySecond()", ds, xsink);
	 
	 return (*xsink ? NULL :
		 new QoreNode(new DateTime(0, 0, day, hour, minute, second, microsecond / 1000, true)));
      }

      case SQLT_LVB:
      {
	 // get oracle data
	 class OracleData *d_ora = (OracleData *)ds->getPrivateData();
	 class BinaryObject *b = new BinaryObject();
	 b->append(OCIRawPtr(d_ora->envhp, (OCIRaw *)val.ptr), OCIRawSize(d_ora->envhp, (OCIRaw *)val.ptr));
	 return new QoreNode(b);
      }

      case SQLT_CLOB:
      case SQLT_BLOB:
      {
	 // get oracle data
	 class OracleData *d_ora = (OracleData *)ds->getPrivateData();
	 
	 printd(5, "OraColumns::getValue() using LOB locator handle %08p\n", val.ptr);
	 
	 // retrieve *LOB data
	 void *buf = malloc(LOB_BLOCK_SIZE);
	 class QoreNode *rv;
	 ub4 amt = 0;
	 if (dtype == SQLT_CLOB)
	 {
	    class QoreString *str = new QoreString(ds->getQoreEncoding());
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

      default:
	 //printd(5, "type=%d\n", dtype);
	 // must be string data
	 remove_trailing_blanks((char *)val.ptr);
	 return new QoreNode(new QoreString((char *)val.ptr, ds->getQoreEncoding()));
   }
   // to avoid a warning
   return NULL;
}

static class Hash *ora_fetch(OCIStmt *stmthp, class Datasource *ds, class ExceptionSink *xsink)
{
   class Hash *h = NULL;
   // retrieve results from statement and return hash
   
   // setup column structure for output columns
   OraColumns columns(stmthp, ds, "ora_fetch()", xsink);

   if (!xsink->isEvent())
   {
      // allocate result hash for result value
      h = new Hash();
      
      // create hash elements for each column, assign empty list
      class OraColumn *w = columns.getHead();
      while (w)
      {
	 printd(5, "ora_fetch() allocating list for '%s' column\n", w->name);
	 h->setKeyValue(w->name, new QoreNode(new List()), xsink);
	 w = w->next;
      }
      
      int num_rows = 0;
      
      // setup temporary row to accept values
      columns.define(stmthp, ds, "ora_fetch()", xsink);
      
      // now finally fetch the data
      while (!xsink->isEvent())
      {
	 int status;
	 class OracleData *d_ora = (OracleData *)ds->getPrivateData();

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
	 class OraColumn *w = columns.getHead();
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
	    // FIXME: check for exception after this call
	    (*v) = w->getValue(ds, xsink);
	    if (xsink->isEvent())
	       break;
	    w = w->next;
	    i++;
	 }
	 num_rows++;
      }
      printd(2, "ora_fetch(): %d column(s), %d row(s) retrieved as output\n", columns.size(), num_rows);
   }
   return h;
}

// returns a list of hashes for a "horizontal" fetch
static class List *ora_fetch_horizontal(OCIStmt *stmthp, class Datasource *ds, class ExceptionSink *xsink)
{
   class List *l = NULL;
   // retrieve results from statement and return hash
   
   // setup column structure for output columns
   OraColumns columns(stmthp, ds, "ora_fetch_horizontal()", xsink);

   if (!xsink->isEvent())
   {
      // allocate result hash for result value
      l = new List();

      // setup temporary row to accept values
      columns.define(stmthp, ds, "ora_fetch_horizontal()", xsink);

      class OracleData *d_ora = (OracleData *)ds->getPrivateData();

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
	 //printd(5, "ora_fetch_horizontal(): l=%08p, %d column(s), got row %d\n", l, columns.size(), l->size());

	 // set up hash for row
	 class Hash *h = new Hash();

	 // copy data or perform per-value processing if needed
	 class OraColumn *w = columns.getHead();
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
      printd(2, "ora_fetch_horizontal(): %d column(s), %d row(s) retrieved as output\n", columns.size(), l->size());
   }
   return l;
}

OraBindGroup::OraBindGroup(class Datasource *ods, class QoreString *ostr, class List *args, ExceptionSink *xsink)
{
   stmthp = NULL;
   hasOutput = false;
   head = tail = NULL;
   ds = ods;
   len = 0;

   // create copy of string and convert encoding if necessary
   str = ostr->convertEncoding(ds->getQoreEncoding(), xsink);
   if (xsink->isEvent())
      return;

   class OracleData *d_ora = (OracleData *)ds->getPrivateData();
   printd(4, "OraBindGroup::OraBindGroup() ds=%08p, d_ora=%08p, SQL='%s', args=%d\n", ds, d_ora, str->getBuffer(), args ? args->size() : 0);

   // process query string and setup bind value list
   parseQuery(args, xsink);

   if (xsink->isEvent())
      return;
      
   ora_checkerr(d_ora->errhp, 
		OCIHandleAlloc(d_ora->envhp, (dvoid **)&stmthp, OCI_HTYPE_STMT, 0, 0), 
		"OraBindGroup::OraBindGroup():hndl", ds, xsink);
   if (xsink->isEvent())
      return;

   ora_checkerr(d_ora->errhp, 
		OCIStmtPrepare(stmthp, d_ora->errhp, (text *)str->getBuffer(), str->strlen(), OCI_NTV_SYNTAX, OCI_DEFAULT), 
		"OraBindGroup():prep", ds, xsink);

   if (xsink->isEvent())
      return;

   class OraBindNode *w = head;
   int pos = 1;
   while (w)
   {
      if (w->bindtype == BN_PLACEHOLDER)
	 w->bindPlaceholder(ds, stmthp, pos, xsink);
      else
	 w->bindValue(ds, stmthp, pos, xsink);
      
      if (xsink->isEvent())
	 return;

      pos++;
      w = w->next;
   }
}

void OraBindGroup::parseQuery(class List *args, class ExceptionSink *xsink)
{
   printd(5, "parseQuery() args=%08p str=%s\n", args, str->getBuffer());

   char quote = 0;

   const char *p = str->getBuffer();
   int index = 0;
   QoreString tmp(ds->getQoreEncoding());
   while (*p)
   {
      if (!quote && (*p) == '%') // found value marker
      {
	 int offset = p - str->getBuffer();
	 class QoreNode *v = args ? args->retrieve_entry(index++) : NULL;

	 p++;
	 if ((*p) == 'd')
	 {
	    // add integer value or NULL
	    if (is_nothing(v) || is_null(v))
	    {
	       str->replace(offset, 2, "null");
	       p = str->getBuffer() + offset + 4;
	    }
	    else
	    {
	       tmp.sprintf("%lld", v->getAsBigInt());
	       str->replace(offset, 2, &tmp);
	       p = str->getBuffer() + offset + tmp.strlen();
	       tmp.clear();
	    }
	    continue;
	 }
	 if ((*p) != 'v')
	 {
	    xsink->raiseException("DBI-EXEC-PARSE-EXCEPTION", "invalid value specification (expecting '%v' or '%%d', got %%%c)", *p);
	    break;
	 }
	 p++;
	 if (isalpha(*p))
	 {
	    xsink->raiseException("DBI-EXEC-PARSE-EXCEPTION", "invalid value specification (expecting '%v' or '%%d', got %%v%c*)", *p);
	    break;
	 }

	 // replace value marker with generated name
	 tmp.sprintf(":qdodvrs___%d", len);
	 str->replace(offset, 2, &tmp);
	 p = str->getBuffer() + offset + tmp.strlen();
	 tmp.clear();

	 printd(5, "OraBindGroup::parseQuery() newstr=%s\n", str->getBuffer());
	 printd(5, "OraBindGroup::parseQuery() adding value type=%s\n",v ? v->type->getName() : "<NULL>");
	 add(v);
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
	 if (!args || args->size() <= index || !(v = args->retrieve_entry(index++)))
	    add(tstr.giveBuffer(), -1, "string");
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
	       xsink->raiseException("DBI-EXEC-EXCEPTION", "expecting type name as value of 'type' key, got '%s'", t->type->getName());
	       break;
	    }
	    
	    // get and check size
	    class QoreNode *sz = v->val.hash->getKeyValue("size");
	    int size = sz ? sz->getAsInt() : -1;
	    
	    printd(5, "OraBindGroup::parseQuery() adding placeholder name=%s, size=%d, type=%s\n", tstr.getBuffer(), size, t->val.String->getBuffer());
	    add(tstr.giveBuffer(), size, t->val.String->getBuffer());
	 }
	 else if (v->type == NT_STRING)
	    add(tstr.giveBuffer(), -1, v->val.String->getBuffer());
	 else if (v->type == NT_INT)
	    add(tstr.giveBuffer(), v->val.intval, "string");
	 else
	    xsink->raiseException("DBI-EXEC-EXCEPTION", "expecting string or hash for placeholder description, got '%s'", v->type->getName());
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

void OraBindNode::bindValue(class Datasource *ds, OCIStmt *stmthp, int pos, class ExceptionSink *xsink)
{
   class OracleData *d_ora = (OracleData *)ds->getPrivateData();
   OCIBind *bndp = NULL;
   ind = 0;

   //printd(5, "OraBindNode::bindValue() type=%s\n", data.v.value ? data.v.value->type->getName() : "NOTHING");

   // bind a NULL
   if (is_nothing(data.v.value) || is_null(data.v.value))
   {
      ora_checkerr(d_ora->errhp, OCIBindByPos(stmthp, &bndp, d_ora->errhp, pos, NULL, 0, SQLT_STR, (dvoid *)NULL, (ub2 *)NULL, (ub2 *)NULL, (ub4)0, (ub4 *)NULL, OCI_DEFAULT), 
		   "OraBindNode::bindValue()", ds, xsink);      
   }
   else if (data.v.value->type == NT_STRING)
   {
      buftype = SQLT_STR;

      class QoreString *bstr = data.v.value->val.String;
      // convert to the db charset if necessary
      if (bstr->getEncoding() != ds->getQoreEncoding())
      {
	 bstr = bstr->convertEncoding(ds->getQoreEncoding(), xsink);
	 if (xsink->isEvent())
	    return;
	 // save temporary string for later deleting
	 data.v.tstr = bstr;
      }

      // bind value to buffer
      buf.ptr = (char *)bstr->getBuffer();

      // bind it
      ora_checkerr(d_ora->errhp, OCIBindByPos(stmthp, &bndp, d_ora->errhp, pos, buf.ptr, bstr->strlen() + 1, SQLT_STR, (dvoid *)NULL, (ub2 *)NULL, (ub2 *)NULL, (ub4)0, (ub4 *)NULL, OCI_DEFAULT), 
		   "OraBindNode::bindValue()", ds, xsink);
   }
   else if (data.v.value->type == NT_DATE)
   {
      class DateTime *d = data.v.value->val.date_time;
      buftype = SQLT_DATE;
      buf.odt = NULL;
      ora_checkerr(d_ora->errhp,
		   OCIDescriptorAlloc(d_ora->envhp, (dvoid **)&buf.odt, OCI_DTYPE_TIMESTAMP, 0, NULL), "OraBindNode::bindValue() TIMESTAMP", ds, xsink);
      if (!*xsink)
      {
	 ora_checkerr(d_ora->errhp, 
		      OCIDateTimeConstruct (d_ora->envhp, d_ora->errhp, buf.odt, (sb2)d->getYear(), (ub1)d->getMonth(), (ub1)d->getDay(),
					    (ub1)d->getHour(), (ub1)d->getMinute(), (ub1)d->getSecond(),
					    (ub4)(d->getMillisecond() * 1000), NULL, 0), "OraBindNode::bindValue() TIMESTAMP", ds, xsink);
	 
	 // bind it
	 ora_checkerr(d_ora->errhp, OCIBindByPos(stmthp, &bndp, d_ora->errhp, pos, &buf.odt, 0, SQLT_TIMESTAMP, (dvoid *)NULL, (ub2 *)NULL, (ub2 *)NULL, (ub4)0, (ub4 *)NULL, OCI_DEFAULT), 
		      "OraBindNode::bindValue()", ds, xsink);
	 
      }
/*
      buftype = SQLT_DAT;
      make_oracle_date_time(data.v.value->val.date_time, buf.date);
      // bind it
      ora_checkerr(d_ora->errhp, OCIBindByPos(stmthp, &bndp, d_ora->errhp, pos, buf.date, 7, SQLT_DAT, (dvoid *)NULL, (ub2 *)NULL, (ub2 *)NULL, (ub4)0, (ub4 *)NULL, OCI_DEFAULT), 
		   "OraBindNode::bindValue()", ds, xsink);
*/
   }
   else if (data.v.value->type == NT_BINARY)
   {
      printd(5, "OraBindNode::bindValue() BLOB ptr=%08p size=%d\n", data.v.value->val.bin->getPtr(), data.v.value->val.bin->size());
      ora_checkerr(d_ora->errhp, OCIBindByPos(stmthp, &bndp, d_ora->errhp, pos, (void *)data.v.value->val.bin->getPtr(), data.v.value->val.bin->size(), SQLT_BIN, (dvoid *)NULL, (ub2 *)NULL, (ub2 *)NULL, (ub4)0, (ub4 *)NULL, OCI_DEFAULT), 
		   "OraBindNode::bindValue()", ds, xsink);      
   }
   else if (data.v.value->type == NT_BOOLEAN)
   {
      buf.i4 = data.v.value->val.boolval;
      ora_checkerr(d_ora->errhp, OCIBindByPos(stmthp, &bndp, d_ora->errhp, pos, &buf.i4, sizeof(int), SQLT_INT, (dvoid *)NULL, (ub2 *)NULL, (ub2 *)NULL, (ub4)0, (ub4 *)NULL, OCI_DEFAULT), "OraBindNode::bindValue()", ds, xsink);
   }
   else if (data.v.value->type == NT_INT)
   {
      if (data.v.value->val.intval <= MAXINT32 && data.v.value->val.intval >= -MAXINT32)
      {
	 buf.i4 = data.v.value->val.intval;
	 ora_checkerr(d_ora->errhp, OCIBindByPos(stmthp, &bndp, d_ora->errhp, pos, &buf.i4, sizeof(int), SQLT_INT, (dvoid *)NULL, (ub2 *)NULL, (ub2 *)NULL, (ub4)0, (ub4 *)NULL, OCI_DEFAULT), "OraBindNode::bindValue()", ds, xsink);
      }
      else // bind as a string value
      {
	 buftype = SQLT_STR;

	 QoreString *tstr = new QoreString(ds->getQoreEncoding());
	 tstr->sprintf("%lld", data.v.value->val.intval);
	 data.v.tstr = tstr;

	 //printd(5, "binding number '%s'\n", buf.ptr);
	 ora_checkerr(d_ora->errhp, OCIBindByPos(stmthp, &bndp, d_ora->errhp, pos, (char *)tstr->getBuffer(), tstr->strlen() + 1, SQLT_STR, (dvoid *)NULL, (ub2 *)NULL, (ub2 *)NULL, (ub4)0, (ub4 *)NULL, OCI_DEFAULT), "OraBindNode::bindValue()", ds, xsink);
      }
   }
   else if (data.v.value->type == NT_FLOAT)
   {
      ora_checkerr(d_ora->errhp, 
		   OCIBindByPos(stmthp, &bndp, d_ora->errhp, pos, &data.v.value->val.floatval, sizeof(double), SQLT_BDOUBLE, (dvoid *)NULL, (ub2 *)NULL, (ub2 *)NULL, (ub4)0, (ub4 *)NULL, OCI_DEFAULT), "OraBindNode::bindValue()", ds, xsink);
   }
   else
      xsink->raiseException("ORACLE-BIND-PLACEHOLDER-ERROR", "type '%s' is not supported for SQL binding", data.v.value->type->getName());
}

void OraBindNode::bindPlaceholder(class Datasource *ds, OCIStmt *stmthp, int pos, class ExceptionSink *xsink)
{
   class OracleData *d_ora = (OracleData *)ds->getPrivateData();
   OCIBind *bndp = NULL;

   printd(5, "OraBindNode::bindPlaceholder(ds=%08p, pos=%d) type=%s, size=%d)\n", ds, pos, data.ph.type, data.ph.maxsize);

   if (!strcmp(data.ph.type, "string"))
   {
      if (data.ph.maxsize < 0)
	 data.ph.maxsize = DBI_DEFAULT_STR_LEN;

      // simply malloc some space for sending to the new node
      buftype = SQLT_STR;
      buf.ptr = malloc(sizeof(char) * (data.ph.maxsize + 1));
      ((char *)buf.ptr)[0] = '\0';

      ora_checkerr(d_ora->errhp, OCIBindByPos(stmthp, &bndp, d_ora->errhp, pos, buf.ptr, data.ph.maxsize + 1, SQLT_STR, &ind, (ub2 *)NULL, (ub2 *)NULL, (ub4)0, (ub4 *)NULL, OCI_DEFAULT), "OraBindNode::bindPlaceholder()", ds, xsink);
   }
   else if (!strcmp(data.ph.type, "date"))
   {
      //printd(5, "oraBindNode::bindPlaceholder() this=%08p, DATE buftype=%d\n", this, SQLT_DATE);
      buftype = SQLT_DATE;
      buf.odt = NULL;
      ora_checkerr(d_ora->errhp,
		   OCIDescriptorAlloc(d_ora->envhp, (dvoid **)&buf.odt, OCI_DTYPE_TIMESTAMP, 0, NULL), "OraBindNode::bindPlaceholder()", ds, xsink);
      if (*xsink)
	 return;

      ora_checkerr(d_ora->errhp, OCIBindByPos(stmthp, &bndp, d_ora->errhp, pos, &buf.odt, 0, SQLT_TIMESTAMP, &ind, (ub2 *)NULL, (ub2 *)NULL, (ub4)0, (ub4 *)NULL, OCI_DEFAULT), "OraBindNode::bindPlaceholder()", ds, xsink);
   }
   else if (!strcmp(data.ph.type, "binary"))
   {
      buftype = SQLT_LVB;
      data.ph.maxsize = ORA_RAW_SIZE;
      buf.ptr = malloc(ORA_RAW_SIZE);
      // set varbin length to zero
      ub4 *bs = (ub4 *)buf.ptr;
      *bs = 0;

      ora_checkerr(d_ora->errhp, OCIBindByPos(stmthp, &bndp, d_ora->errhp, pos, buf.ptr, ORA_RAW_SIZE, SQLT_LVB, &ind, (ub2 *)NULL, (ub2 *)NULL, (ub4)0, (ub4 *)NULL, OCI_DEFAULT), "OraBindNode::bindPlaceholder()", ds, xsink);
   }
   else if (!strcmp(data.ph.type, "clob"))
   {
      buftype = SQLT_CLOB;
      buf.ptr = NULL;

      ora_checkerr(d_ora->errhp, OCIDescriptorAlloc(d_ora->envhp, &buf.ptr, OCI_DTYPE_LOB, 0, NULL), "OraBindNode::bindPlaceholder()", ds, xsink);

      if (xsink->isEvent()) return;
      printd(5, "bindPalceholder() got LOB locator handle %08p\n", buf.ptr);
      ora_checkerr(d_ora->errhp, OCIBindByPos(stmthp, &bndp, d_ora->errhp, pos, &buf.ptr, 0, SQLT_CLOB, &ind, (ub2 *)NULL, (ub2 *)NULL, (ub4)0, (ub4 *)NULL, OCI_DEFAULT), "OraBindNode::bindPlaceholder()", ds, xsink);
   }
   else if (!strcmp(data.ph.type, "blob"))
   {
      buftype = SQLT_BLOB;
      buf.ptr = NULL;

      ora_checkerr(d_ora->errhp, OCIDescriptorAlloc(d_ora->envhp, &buf.ptr, OCI_DTYPE_LOB, 0, NULL), "OraBindNode::bindPlaceholder()", ds, xsink);

      if (xsink->isEvent()) return;
      printd(5, "bindPalceholder() got LOB locator handle %08p\n", buf.ptr);
      ora_checkerr(d_ora->errhp, OCIBindByPos(stmthp, &bndp, d_ora->errhp, pos, &buf.ptr, 0, SQLT_BLOB, &ind, (ub2 *)NULL, (ub2 *)NULL, (ub4)0, (ub4 *)NULL, OCI_DEFAULT), "OraBindNode::bindPlaceholder()", ds, xsink);
   }
   else if (!strcmp(data.ph.type, "integer"))
   {
      buftype = SQLT_INT;
      ora_checkerr(d_ora->errhp, OCIBindByPos(stmthp, &bndp, d_ora->errhp, pos, &buf.i8, sizeof(int64), SQLT_INT, &ind, (ub2 *)NULL, (ub2 *)NULL, (ub4)0, (ub4 *)NULL, OCI_DEFAULT), "OraBindNode::bindPlaceholder()", ds, xsink);
   }
   else if (!strcmp(data.ph.type, "float"))
   {
      buftype = SQLT_BDOUBLE;
      ora_checkerr(d_ora->errhp, OCIBindByPos(stmthp, &bndp, d_ora->errhp, pos, &buf.f8, sizeof(double), SQLT_BDOUBLE, &ind, (ub2 *)NULL, (ub2 *)NULL, (ub4)0, (ub4 *)NULL, OCI_DEFAULT), "OraBindNode::bindPlaceholder()", ds, xsink);
   }
   else if (!strcmp(data.ph.type, "hash"))
   {
      buftype = SQLT_RSET;

      // allocate statement handle for result list
      ora_checkerr(d_ora->errhp, OCIHandleAlloc(d_ora->envhp, (dvoid **)&buf.ptr, OCI_HTYPE_STMT, 0, 0), "OraBindNode::bindPlaceHolder()", ds, xsink);

      if (!xsink->isEvent())
	 ora_checkerr(d_ora->errhp, OCIBindByPos(stmthp, &bndp, d_ora->errhp, pos, &buf.ptr, 0, SQLT_RSET, &ind, (ub2 *)NULL, (ub2 *)NULL, (ub4)0, (ub4 *)NULL, OCI_DEFAULT), "OraBindNode::bindPlaceholder()", ds, xsink);
      else
	 buf.ptr = NULL;
   }
   else
      xsink->raiseException("DBI-EXEC-EXCEPTION", "type '%s' is not supported for SQL binding", data.ph.type);
}

class QoreNode *OraBindNode::getValue(class Datasource *ds, class ExceptionSink *xsink)
{
   // if NULL, then return NULL
   if (ind == -1)
      return null();

   //printd(5, "buftype: %d\n", buftype);
   if (buftype == SQLT_STR)
   {
      // must be string data
      remove_trailing_blanks((char *)buf.ptr);
      class QoreString *str = new QoreString();
      str->take((char *)buf.ptr, ds->getQoreEncoding());
      buf.ptr = NULL;
      return new QoreNode(str);
   }
   else if (buftype == SQLT_DAT)
      return new QoreNode(convert_date_time(buf.date));
   else if (buftype == SQLT_DATE)
      return get_oracle_timestamp(ds, buf.odt, xsink);
   else if (buftype == SQLT_INT)
      return new QoreNode(buf.i8);
   else if (buftype == SQLT_BDOUBLE)
      return new QoreNode(buf.f8);
   else if (buftype == SQLT_RSET)
   {
      class Hash *h = ora_fetch((OCIStmt *)buf.ptr, ds, xsink);
      return h ? new QoreNode(h) : NULL;
   }
   else if (buftype == SQLT_LVB)
   {
      // get oracle data
      class OracleData *d_ora = (OracleData *)ds->getPrivateData();
      class BinaryObject *b = new BinaryObject();
      b->append(OCIRawPtr(d_ora->envhp, (OCIRaw *)buf.ptr), OCIRawSize(d_ora->envhp, (OCIRaw *)buf.ptr));
      return new QoreNode(b);
   }
   else if (buftype == SQLT_CLOB || buftype == SQLT_BLOB)
   {
      // get oracle data
      class OracleData *d_ora = (OracleData *)ds->getPrivateData();

      printd(5, "OraBindNode::getValue() using LOB locator handle %08p\n", buf.ptr);

      // retrieve *LOB data
      void *bbuf = malloc(LOB_BLOCK_SIZE);
      class QoreNode *rv;
      ub4 amt = 0;
      if (buftype == SQLT_CLOB)
      {
	 class QoreString *str = new QoreString(ds->getQoreEncoding());
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
      OCIDescriptorFree(buf.ptr, OCI_DTYPE_LOB);
      free(bbuf);
      return rv;
   }
   return NULL;
}

class QoreNode *OraBindGroup::getOutputHash(class ExceptionSink *xsink)
{
   class Hash *h = new Hash();
   
   class OraBindNode *w = head;
   while (w)
   {
      if (w->bindtype == BN_PLACEHOLDER)
	 h->setKeyValue(w->data.ph.name, w->getValue(ds, xsink), xsink);
      w = w->next;
   }
   return new QoreNode(h);
}

class QoreNode *OraBindGroup::exec(class ExceptionSink *xsink)
{
   class QoreNode *rv;

   class OracleData *d_ora = (OracleData *)ds->getPrivateData();
   int status = OCIStmtExecute(d_ora->svchp, stmthp, d_ora->errhp, 1, 0, NULL, NULL, OCI_DEFAULT);
   if (status)
      ora_checkerr(d_ora->errhp, status, "OraBindGroup::exec()", ds, xsink);

   if (!xsink->isEvent())
   {
      // if there are output variables, then fix values if necessary and return
      if (hasOutput)
	 rv = getOutputHash(xsink);
      else // get row count
      {
	 int rc = 0;
	 ora_checkerr(d_ora->errhp, OCIAttrGet(stmthp, OCI_HTYPE_STMT, &rc, 0, OCI_ATTR_ROW_COUNT, d_ora->errhp), "OraBindGroup::exec():attr", ds, xsink);
	 rv = new QoreNode((int64)rc);
      }
      
      // commit transaction if autocommit set for datasource
      if (ds->getAutoCommit())
	 if ((status = OCITransCommit(d_ora->svchp, d_ora->errhp, (ub4) 0)))
	    ora_checkerr(d_ora->errhp, status, "OraBindGroup():commit", ds, xsink);
   }   
   else
      rv = NULL;

   return rv;
}

class QoreNode *OraBindGroup::select(class ExceptionSink *xsink)
{
   class OracleData *d_ora = (OracleData *)ds->getPrivateData();

   int status = OCIStmtExecute(d_ora->svchp, stmthp, d_ora->errhp, 0, 0, NULL, NULL, OCI_DEFAULT);
   if (status)
      ora_checkerr(d_ora->errhp, status, "OraBindGroup::select()", ds, xsink);

   class Hash *h = NULL;
   if (!xsink->isEvent())
      h = ora_fetch(stmthp, ds, xsink);

   // commit transaction if autocommit set for datasource
   if (!xsink->isEvent() && ds->getAutoCommit())
      if ((status = OCITransCommit(d_ora->svchp, d_ora->errhp, (ub4) 0)))
	 ora_checkerr(d_ora->errhp, status, "OraBindGroup():commit", ds, xsink);

   if (!h)
      return NULL;
   return new QoreNode(h);
}

class QoreNode *OraBindGroup::selectRows(class ExceptionSink *xsink)
{
   class OracleData *d_ora = (OracleData *)ds->getPrivateData();

   int status = OCIStmtExecute(d_ora->svchp, stmthp, d_ora->errhp, 0, 0, NULL, NULL, OCI_DEFAULT);
   if (status)
      ora_checkerr(d_ora->errhp, status, "OraBindGroup::select()", ds, xsink);

   class List *l = NULL;
   if (!xsink->isEvent())
      l = ora_fetch_horizontal(stmthp, ds, xsink);

   // commit transaction if autocommit set for datasource
   if (!xsink->isEvent() && ds->getAutoCommit())
      if ((status = OCITransCommit(d_ora->svchp, d_ora->errhp, (ub4) 0)))
	 ora_checkerr(d_ora->errhp, status, "OraBindGroup():commit", ds, xsink);

   if (!l)
      return NULL;
   return new QoreNode(l);
}

static class QoreNode *oracle_exec(class Datasource *ds, QoreString *qstr, List *args, class ExceptionSink *xsink)
{
   class OraBindGroup bg(ds, qstr, args, xsink);

   if (xsink->isException())
      return NULL;

   return bg.exec(xsink);
}

static class QoreNode *oracle_select(class Datasource *ds, QoreString *qstr, List *args, class ExceptionSink *xsink)
{
   class OraBindGroup bg(ds, qstr, args, xsink);

   if (xsink->isException())
      return NULL;

   return bg.select(xsink);
}

static class QoreNode *oracle_select_rows(class Datasource *ds, QoreString *qstr, List *args, class ExceptionSink *xsink)
{
   class OraBindGroup bg(ds, qstr, args, xsink);

   if (xsink->isException())
      return NULL;

   return bg.selectRows(xsink);
}

static int oracle_open(Datasource *ds, ExceptionSink *xsink)
{
   tracein("oracle_open()");

   printd(5, "oracle_open() datasource %08p for DB=%s open\n", ds, ds->getDBName());

   if (!ds->getUsername())
   {
      xsink->raiseException("DATASOURCE-MISSING-USERNAME", "Datasource has an empty username parameter");
      traceout("oracle_open()");
      return -1;
   }
   if (!ds->getPassword())
   {
      xsink->raiseException("DATASOURCE-MISSING-PASSWORD", "Datasource has an empty password parameter");
      traceout("oracle_open()");
      return -1;
   }
   if (!ds->getDBName())
   {
      xsink->raiseException("DATASOURCE-MISSING-DBNAME", "Datasource has an empty dbname parameter");
      traceout("oracle_open()");
      return -1;
   }
   printd(3, "oracle_open(): user=%s pass=%s db=%s (oracle encoding=%s)\n",
	  ds->getUsername(), ds->getPassword(), ds->getDBName(), ds->getDBEncoding() ? ds->getDBEncoding() : "(none)");

   class OracleData *d_ora = new OracleData;
   ds->setPrivateData((void *)d_ora);

   // locking is done on the level above with the Datasource class
   int oci_flags = OCI_DEFAULT|OCI_THREADED|OCI_NO_MUTEX;

   const char *charset;

   // FIXME: maybe I don't need a temporary environment handle?
   // create temporary environment handle
   OCIEnv *tmpenvhp;
   OCIEnvCreate(&tmpenvhp, oci_flags | OCI_NO_UCB, 0, 0, 0, 0, 0, 0);
   // declare temporary buffer
   char nbuf[OCI_NLS_MAXBUFSZ];
   int need_to_set_charset = 0;

   if (ds->getDBEncoding())
   {
      charset = ds->getDBEncoding();
      need_to_set_charset = 1;
   }
   else // get Oracle character set name from OS character set name
   {
      if ((OCINlsNameMap(tmpenvhp, (oratext *)nbuf, OCI_NLS_MAXBUFSZ, (oratext *)QCS_DEFAULT->getCode(), OCI_NLS_CS_IANA_TO_ORA) != OCI_SUCCESS))
      {
	 OCIHandleFree(tmpenvhp, OCI_HTYPE_ENV);
	 xsink->raiseException("DBI:ORACLE:UNKNOWN-CHARACTER-SET", 
			"cannot map default OS encoding '%s' to Oracle character encoding",
			QCS_DEFAULT->getCode());
	 delete d_ora;
	 ds->setPrivateData(NULL);
	 traceout("oracle_open()");
	 return -1;
      }
      ds->setDBEncoding(nbuf);
      ds->setQoreEncoding(QCS_DEFAULT);
      charset = nbuf;
      printd(5, "oracle_open() setting Oracle encoding to '%s' from default OS encoding '%s'\n",
	     charset, QCS_DEFAULT->getCode());
   }

#ifdef HAVE_OCIENVNLSCREATE
   // get character set ID
   d_ora->charsetid = OCINlsCharSetNameToId(tmpenvhp, (oratext *)charset);
   // delete temporary environmental handle
   OCIHandleFree(tmpenvhp, OCI_HTYPE_ENV);

   if (!d_ora->charsetid)
   {
      xsink->raiseException("DBI:ORACLE:UNKNOWN-CHARACTER-SET", "this installation of Oracle does not support the '%s' character encoding", 
			    ds->getDBEncoding());
      delete d_ora;
      ds->setPrivateData(NULL);
      traceout("oracle_open()");
      return -1;
   }
   printd(5, "Oracle character encoding '%s' has ID %d\n", charset, d_ora->charsetid);
   // create environment with default character set
   OCIEnvNlsCreate(&d_ora->envhp, oci_flags, 0, NULL, NULL, NULL, 0, NULL, d_ora->charsetid, d_ora->charsetid);
   // map the Oracle character set to a core character set

   if (need_to_set_charset)
   {
      // map Oracle character encoding name to QORE/OS character encoding name
      if ((OCINlsNameMap(d_ora->envhp, (oratext *)nbuf, OCI_NLS_MAXBUFSZ, (oratext *)ds->getDBEncoding(), OCI_NLS_CS_ORA_TO_IANA) == OCI_SUCCESS))
      {
	 printd(5, "oracle_open() Oracle character set '%s' mapped to '%s' character set\n", ds->getDBEncoding(), nbuf);
	 ds->setQoreEncoding(nbuf);
      }
#ifdef DEBUG
      else
      {
	 printd(5, "oracle_open(): can't map Oracle character set '%s' to OS character set\n", ds->getDBEncoding());
	 assert(false);
      }
#endif
   }

#else // !HAVE_OCIENVNLSCREATE
#error need to define HAVE_OCIENVNLSCREATE (with Oracle 9i+)
/*
   d_ora->charsetid = 0;
   if (ds->getDBEncoding())
   {
      xsink->raiseException("DBI:ORACLE:NO_OCIENVCREATE", "compile-time options do not support Oracle character set specifications");
      delete d_ora;
      ds->setPrivateData(NULL);
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
		OCILogon(d_ora->envhp, d_ora->errhp, &d_ora->svchp, (text *)ds->getUsername(), strlen(ds->getUsername()), (text *)ds->getPassword(), strlen(ds->getPassword()), (text *)ds->getDBName(), strlen(ds->getDBName())), 
		"<open>", ds, xsink);
   if (xsink->isEvent())
   {
      delete d_ora;
      ds->setPrivateData(NULL);
      traceout("oracle_open()");
      return -1;
   }

   printd(5, "oracle_open() datasource %08p for DB=%s open (envhp=%08p)\n", ds, ds->getDBName(), d_ora->envhp);
   
   traceout("oracle_open()");
   return 0;
}

static int oracle_close(class Datasource *ds)
{
   tracein("oracle_close()");

   class OracleData *d_ora = (OracleData *)ds->getPrivateData();

   printd(3, "oracle_close(): connection to %s closed.\n", ds->getDBName());
   OCILogoff(d_ora->svchp, d_ora->errhp);
   OCIHandleFree(d_ora->svchp, OCI_HTYPE_SVCCTX);
   OCIHandleFree(d_ora->errhp, OCI_HTYPE_ERROR);
   OCIHandleFree(d_ora->envhp, OCI_HTYPE_ENV);
   delete d_ora;
   ds->setPrivateData(NULL);

   traceout("oracle_close()");
   return 0;
}

#define VERSION_BUF_SIZE 512
static class QoreNode *oracle_get_server_version(class Datasource *ds, class ExceptionSink *xsink)
{
   class OracleData *d_ora = (OracleData *)ds->getPrivateData();
   char version_buf[VERSION_BUF_SIZE + 1];

   ora_checkerr(d_ora->errhp, 
		OCIServerVersion (d_ora->svchp, d_ora->errhp, (OraText *)version_buf, VERSION_BUF_SIZE, OCI_HTYPE_SVCCTX),
		"oracle_get_server_version", ds, xsink);
   if (xsink->isEvent())
      return NULL;
   
   return new QoreNode(version_buf);   
}

#ifdef HAVE_OCICLIENTVERSION
static class QoreNode *oracle_get_client_version()
{
   sword major, minor, update, patch, port_update;

   OCIClientVersion(&major, &minor, &update, &patch, &port_update);
   class Hash *h = new Hash();
   h->setKeyValue("major", new QoreNode((int64)major), NULL);
   h->setKeyValue("minor", new QoreNode((int64)minor), NULL);
   h->setKeyValue("update", new QoreNode((int64)update), NULL);
   h->setKeyValue("patch", new QoreNode((int64)patch), NULL);
   h->setKeyValue("port_update", new QoreNode((int64)port_update), NULL);
   return new QoreNode(h);
}
#endif

class QoreString *oracle_module_init()
{
   tracein("oracle_module_init()");

   // register driver with DBI subsystem
   class qore_dbi_method_list methods;
   methods.add(QDBI_METHOD_OPEN, oracle_open);
   methods.add(QDBI_METHOD_CLOSE, oracle_close);
   methods.add(QDBI_METHOD_SELECT, oracle_select);
   methods.add(QDBI_METHOD_SELECT_ROWS, oracle_select_rows);
   methods.add(QDBI_METHOD_EXEC, oracle_exec);
   methods.add(QDBI_METHOD_COMMIT, oracle_commit);
   methods.add(QDBI_METHOD_ROLLBACK, oracle_rollback);
   methods.add(QDBI_METHOD_AUTO_COMMIT, oracle_commit);
   methods.add(QDBI_METHOD_GET_SERVER_VERSION, oracle_get_server_version);
#ifdef HAVE_OCICLIENTVERSION
   methods.add(QDBI_METHOD_GET_CLIENT_VERSION, oracle_get_client_version);
#endif
   
   DBID_ORACLE = DBI.registerDriver("oracle", methods, DBI_ORACLE_CAPS);

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
