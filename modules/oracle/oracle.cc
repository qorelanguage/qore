/*
  oracle.cc

  Oracle OCI Interface to Qore DBI layer

  Qore Programming Language

  Copyright 2003 - 2008 David Nichols

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

#include <memory>

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
DLLEXPORT qore_license_t qore_module_license = QL_LGPL;
#endif

// capabilities of this driver
#define DBI_ORACLE_CAPS (DBI_CAP_TRANSACTION_MANAGEMENT | DBI_CAP_STORED_PROCEDURES | DBI_CAP_CHARSET_SUPPORT | DBI_CAP_LOB_SUPPORT | DBI_CAP_BIND_BY_VALUE | DBI_CAP_BIND_BY_PLACEHOLDER)

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

static int ora_checkerr(OCIError *errhp, sword status, const char *query_name, Datasource *ds, ExceptionSink *xsink)
{
   text errbuf[512];
   sb4 errcode = 0;

   //printd(5, "ora_checkerr(%08p, %d, %s, isEvent=%d)\n", errhp, status, query_name ? query_name : "none", xsink->isEvent());
   switch (status)
   {
      case OCI_SUCCESS:
      case OCI_SUCCESS_WITH_INFO:
	 // ignore SUCCESS_WITH_INFO codes
	 return 0;

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
   return -1;
}

static int oracle_commit(Datasource *ds, ExceptionSink *xsink)
{
   int status;

   OracleData *d_ora = (OracleData *)ds->getPrivateData();

   // commit transaction
   if ((status = OCITransCommit(d_ora->svchp, d_ora->errhp, (ub4) 0)))
      ora_checkerr(d_ora->errhp, status, "Oracle commit transaction", ds, xsink);
   return 0;
}

static int oracle_rollback(Datasource *ds, ExceptionSink *xsink)
{
   int status;

   OracleData *d_ora = (OracleData *)ds->getPrivateData();
   if ((status = OCITransRollback(d_ora->svchp, d_ora->errhp, (ub4) 0)))
      ora_checkerr(d_ora->errhp, status, "Oracle rollback transaction", ds, xsink);
   return 0;
}

OraColumns::OraColumns(OCIStmt *stmthp, Datasource *ds, const char *str, ExceptionSink *xsink)
{
   tracein("OraColumns::OraColumns()");

   len = 0;
   head = tail = NULL;

   OracleData *d_ora = (OracleData *)ds->getPrivateData();

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

void OraColumns::define(OCIStmt *stmthp, Datasource *ds, const char *str, ExceptionSink *xsink)
{
   //tracein("OraColumne::define()");

   OracleData *d_ora = (OracleData *)ds->getPrivateData();

   // iterate column list
   OraColumn *w = head;
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

static class DateTimeNode *convert_date_time(unsigned char *str)
{
   int year;
   if ((str[0] < 100) || (str[1] < 100))
      year = 9999; 
   else
      year = (str[0] - 100) * 100 + (str[1] - 100);

   //printd(5, "convert_date_time(): %d %d = %04d-%02d-%02d %02d:%02d:%02d\n", str[0], str[1], dt->year, dt->month, dt->day, dt->hour, dt->minute, dt->second);
   return new DateTimeNode(year, str[2], str[3], str[4] - 1, str[5] - 1, str[6] - 1);
}

extern "C" sb4 read_clob_callback(void *sp, CONST dvoid *bufp, ub4 len, ub1 piece)
{
   printd(5, "read_clob_callback(%08p, %08p, %d, %d)\n", sp, bufp, len, piece);
   (reinterpret_cast<QoreStringNode *>(sp))->concat((char *)bufp, len);
   return OCI_CONTINUE;
}

extern "C" sb4 read_blob_callback(void *bp, CONST dvoid *bufp, ub4 len, ub1 piece)
{
   printd(5, "read_blob_callback(%08p, %08p, %d, %d)\n", bp, bufp, len, piece);
   ((class BinaryNode *)bp)->append((char *)bufp, len);
   return OCI_CONTINUE;
}


AbstractQoreNode *get_oracle_timestamp(Datasource *ds, OCIDateTime *odt, ExceptionSink *xsink)
{
   OracleData *d_ora = (OracleData *)ds->getPrivateData();
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

   return new DateTimeNode(year, month, day, hour, minute, second, us / 1000);
}

AbstractQoreNode *OraColumn::getValue(Datasource *ds, ExceptionSink *xsink)
{
   //printd(5, "OraColumn::getValue() dtype=%d, ind=%d, maxsize=%d\n", dtype, ind, maxsize);

   if (ind == -1)      // SQL NULL returned
      return null();

   switch (dtype)
   {
      case SQLT_INT:
      case SQLT_UIN:
	 return new QoreBigIntNode(val.i8);

      case SQLT_FLT:
      case SQLT_BFLOAT:
      case SQLT_BDOUBLE:
      case SQLT_IBFLOAT:
      case SQLT_IBDOUBLE:
	 return new QoreFloatNode(val.f8);

      case SQLT_DAT:
	 return convert_date_time(val.date);

      case SQLT_TIMESTAMP:
      case SQLT_TIMESTAMP_TZ:
      case SQLT_TIMESTAMP_LTZ:
      case SQLT_DATE:
	 return get_oracle_timestamp(ds, val.odt, xsink);

      case SQLT_INTERVAL_YM:
      {
	 // get oracle data
	 OracleData *d_ora = (OracleData *)ds->getPrivateData();

	 //printd(5, "OraColumn::getValue() using INTERVAL_YM handle %08p\n", val.oi);

	 sb4 year, month;
	 ora_checkerr(d_ora->errhp, 
		      OCIIntervalGetYearMonth(d_ora->envhp, d_ora->errhp, &year, &month, val.oi),
		      "OCIIntervalGetYearMonth()", ds, xsink);
	 
	 return (*xsink ? NULL :
		 new DateTimeNode(year, month, 0, 0, 0, 0, 0, true));
      }

      case SQLT_INTERVAL_DS:
      {
	 // get oracle data
	 OracleData *d_ora = (OracleData *)ds->getPrivateData();

	 //printd(5, "OraColumn::getValue() using INTERVAL_DS handle %08p\n", val.oi);

	 sb4 day, hour, minute, second, microsecond;
	 ora_checkerr(d_ora->errhp, 
		      OCIIntervalGetDaySecond(d_ora->envhp, d_ora->errhp, &day, &hour, &minute, &second, &microsecond, val.oi),
		      "OCIIntervalGetDaySecond()", ds, xsink);
	 
	 return (*xsink ? NULL :
		 new DateTimeNode(0, 0, day, hour, minute, second, microsecond / 1000, true));
      }

      case SQLT_LVB:
      {
	 // get oracle data
	 OracleData *d_ora = (OracleData *)ds->getPrivateData();
	 BinaryNode *b = new BinaryNode();
	 b->append(OCIRawPtr(d_ora->envhp, (OCIRaw *)val.ptr), OCIRawSize(d_ora->envhp, (OCIRaw *)val.ptr));
	 return b;
      }

      case SQLT_CLOB:
      case SQLT_BLOB:
      {
	 // get oracle data
	 OracleData *d_ora = (OracleData *)ds->getPrivateData();
	 
	 printd(5, "OraColumns::getValue() using LOB locator handle %08p\n", val.ptr);
	 
	 // retrieve *LOB data
	 void *buf = malloc(LOB_BLOCK_SIZE);
	 AbstractQoreNode *rv;
	 ub4 amt = 0;
	 if (dtype == SQLT_CLOB)
	 {
	    QoreStringNodeHolder str(new QoreStringNode(ds->getQoreEncoding()));
	    // read LOB data in streaming callback mode
	    ora_checkerr(d_ora->errhp,
			 OCILobRead(d_ora->svchp, d_ora->errhp, (OCILobLocator *)val.ptr, &amt, 1, buf, LOB_BLOCK_SIZE,
				    *str, read_clob_callback, (ub2)d_ora->charsetid, (ub1)0), "oraReadCLOBCallback()", ds, xsink);
	    rv = *xsink ? 0 : str.release();
	 }
	 else
	 {
	    SimpleRefHolder<BinaryNode> b(new BinaryNode());
	    // read LOB data in streaming callback mode
	    ora_checkerr(d_ora->errhp,
			 OCILobRead(d_ora->svchp, d_ora->errhp, (OCILobLocator *)val.ptr, &amt, 1, buf, LOB_BLOCK_SIZE,
				    *b, read_blob_callback, (ub2)0, (ub1)0), "oraReadBLOBCallback()", ds, xsink);
	    rv = *xsink ? 0 : b.release();
	 }
	 free(buf);
	 return rv;
      }

      default:
	 //printd(5, "type=%d\n", dtype);
	 // must be string data
	 remove_trailing_blanks((char *)val.ptr);
	 return new QoreStringNode((char *)val.ptr, ds->getQoreEncoding());
   }
   // to avoid a warning
   return NULL;
}

static QoreHashNode *ora_fetch(OCIStmt *stmthp, Datasource *ds, ExceptionSink *xsink)
{
   // retrieve results from statement and return hash
   
   // setup column structure for output columns
   OraColumns columns(stmthp, ds, "ora_fetch()", xsink);
   if (*xsink)
      return 0;

   // allocate result hash for result value
   QoreHashNode *h = new QoreHashNode();
      
   // create hash elements for each column, assign empty list
   OraColumn *w = columns.getHead();
   while (w)
   {
      printd(5, "ora_fetch() allocating list for '%s' column\n", w->name);
      h->setKeyValue(w->name, new QoreListNode(), xsink);
      w = w->next;
   }
   
   int num_rows = 0;
   
   // setup temporary row to accept values
   columns.define(stmthp, ds, "ora_fetch()", xsink);
   
   // now finally fetch the data
   while (!xsink->isEvent())
   {
      int status;
      OracleData *d_ora = (OracleData *)ds->getPrivateData();
      
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
      OraColumn *w = columns.getHead();
      int i = 0;
      while (w)
      {
	 // get pointer to value of target node
	 QoreListNode *l = reinterpret_cast<QoreListNode *>(h->getKeyValue(w->name));
	 AbstractQoreNode **v = l->get_entry_ptr(num_rows);
	 
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

   return h;
}

// returns a list of hashes for a "horizontal" fetch
static QoreListNode *ora_fetch_horizontal(OCIStmt *stmthp, Datasource *ds, ExceptionSink *xsink)
{
   QoreListNode *l = NULL;
   // retrieve results from statement and return hash
   
   // setup column structure for output columns
   OraColumns columns(stmthp, ds, "ora_fetch_horizontal()", xsink);

   if (!xsink->isEvent())
   {
      // allocate result hash for result value
      l = new QoreListNode();

      // setup temporary row to accept values
      columns.define(stmthp, ds, "ora_fetch_horizontal()", xsink);

      OracleData *d_ora = (OracleData *)ds->getPrivateData();

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
	 QoreHashNode *h = new QoreHashNode();

	 // copy data or perform per-value processing if needed
	 OraColumn *w = columns.getHead();
	 while (w)
	 {
	    // assign value to hash
	    h->setKeyValue(w->name, w->getValue(ds, xsink), xsink);
	    if (xsink->isEvent())
	       break;
	    w = w->next;
	 }
	 // add row to list
	 l->push(h);
      }
      printd(2, "ora_fetch_horizontal(): %d column(s), %d row(s) retrieved as output\n", columns.size(), l->size());
   }
   return l;
}

OraBindGroup::OraBindGroup(Datasource *ods, const QoreString *ostr, const QoreListNode *args, ExceptionSink *xsink)
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

   OracleData *d_ora = (OracleData *)ds->getPrivateData();
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

   OraBindNode *w = head;
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

void OraBindGroup::parseQuery(const QoreListNode *args, ExceptionSink *xsink)
{
   printd(5, "parseQuery() args=%08p str=%s\n", args, str->getBuffer());

   char quote = 0;

   const char *p = str->getBuffer();
   unsigned index = 0;
   QoreString tmp(ds->getQoreEncoding());
   while (*p)
   {
      if (!quote && (*p) == '%') // found value marker
      {
	 int offset = p - str->getBuffer();
	 const AbstractQoreNode *v = args ? args->retrieve_entry(index++) : NULL;

	 p++;
	 if ((*p) == 'd')
	 {
	    DBI_concat_numeric(&tmp, v);
	    str->replace(offset, 2, &tmp);
	    p = str->getBuffer() + offset + tmp.strlen();
	    tmp.clear();
	    continue;
	 }
	 if ((*p) == 's')
	 {
	    if (DBI_concat_string(&tmp, v, xsink))
	       break;
	    str->replace(offset, 2, &tmp);
	    p = str->getBuffer() + offset + tmp.strlen();
	    tmp.clear();
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
	 printd(5, "OraBindGroup::parseQuery() adding value type=%s\n",v ? v->getTypeName() : "<NULL>");
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
	 const AbstractQoreNode *v;
	 // assume string if no argument passed
	 if (!args || args->size() <= index || !(v = args->retrieve_entry(index++)))
	    add(tstr.giveBuffer(), -1, "string");
	 else {
	    qore_type_t vtype = v->getType();
	    if (vtype == NT_HASH)
	    {
	       const QoreHashNode *h = reinterpret_cast<const QoreHashNode *>(v);
	       // get and check data type
	       const AbstractQoreNode *t = h->getKeyValue("type");
	       if (!t)
	       {
		  xsink->raiseException("DBI-EXEC-EXCEPTION", "missing 'type' key in placeholder hash");
		  break;	 
	       }
	       const QoreStringNode *str = dynamic_cast<const QoreStringNode *>(t);
	       if (!str)
	       {
		  xsink->raiseException("DBI-EXEC-EXCEPTION", "expecting type name as value of 'type' key, got '%s'", t->getTypeName());
		  break;
	       }
	       
	       // get and check size
	       const AbstractQoreNode *sz = h->getKeyValue("size");
	       int size = sz ? sz->getAsInt() : -1;
	       
	       printd(5, "OraBindGroup::parseQuery() adding placeholder name=%s, size=%d, type=%s\n", tstr.getBuffer(), size, str->getBuffer());
	       add(tstr.giveBuffer(), size, str->getBuffer());
	    }
	    else if (vtype == NT_STRING)
	       add(tstr.giveBuffer(), -1, (reinterpret_cast<const QoreStringNode *>(v))->getBuffer());
	    else if (vtype == NT_INT)
	       add(tstr.giveBuffer(), (reinterpret_cast<const QoreBigIntNode *>(v))->val, "string");
	    else
	       xsink->raiseException("DBI-EXEC-EXCEPTION", "expecting string or hash for placeholder description, got '%s'", v->getTypeName());
	 }
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

void OraBindNode::bindValue(Datasource *ds, OCIStmt *stmthp, int pos, ExceptionSink *xsink)
{
   OracleData *d_ora = (OracleData *)ds->getPrivateData();
   OCIBind *bndp = NULL;
   ind = 0;

   //printd(5, "OraBindNode::bindValue() type=%s\n", data.v.value ? data.v.value->getTypeName() : "NOTHING");

   // bind a NULL
   if (is_nothing(data.v.value) || is_null(data.v.value))
   {
      ora_checkerr(d_ora->errhp, OCIBindByPos(stmthp, &bndp, d_ora->errhp, pos, NULL, 0, SQLT_STR, (dvoid *)NULL, (ub2 *)NULL, (ub2 *)NULL, (ub4)0, (ub4 *)NULL, OCI_DEFAULT), 
		   "OraBindNode::bindValue()", ds, xsink);
      return;
   }

   qore_type_t ntype = data.v.value->getType();
   
   if (ntype == NT_STRING) {
      const QoreStringNode *bstr = reinterpret_cast<const QoreStringNode *>(data.v.value);
      buftype = SQLT_STR;

      // convert to the db charset if necessary
      if (bstr->getEncoding() != ds->getQoreEncoding())
      {
	 QoreString *nstr = bstr->QoreString::convertEncoding(ds->getQoreEncoding(), xsink);
	 if (*xsink)
	    return;
	 // save temporary string for later deleting
	 data.v.tstr = nstr;
	 buf.ptr = (char *)nstr->getBuffer();
      }
      else // bind value to buffer
	 buf.ptr = (char *)bstr->getBuffer();
      
      // bind it
      ora_checkerr(d_ora->errhp, OCIBindByPos(stmthp, &bndp, d_ora->errhp, pos, buf.ptr, bstr->strlen() + 1, SQLT_STR, (dvoid *)NULL, (ub2 *)NULL, (ub2 *)NULL, (ub4)0, (ub4 *)NULL, OCI_DEFAULT), 
		   "OraBindNode::bindValue()", ds, xsink);
      return;
   }

   if (ntype == NT_DATE) {
      const DateTimeNode *d = reinterpret_cast<const DateTimeNode *>(data.v.value);
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
      return;
   }

   if (ntype == NT_BINARY) {
      const BinaryNode *b = reinterpret_cast<const BinaryNode *>(data.v.value);
      printd(5, "OraBindNode::bindValue() BLOB ptr=%08p size=%d\n", b->getPtr(), b->size());
      ora_checkerr(d_ora->errhp, OCIBindByPos(stmthp, &bndp, d_ora->errhp, pos, (void *)b->getPtr(), b->size(), SQLT_BIN, (dvoid *)NULL, (ub2 *)NULL, (ub2 *)NULL, (ub4)0, (ub4 *)NULL, OCI_DEFAULT), 
		   "OraBindNode::bindValue()", ds, xsink);      
      return;
   }

   if (ntype == NT_BOOLEAN)
   {
      buf.i4 = reinterpret_cast<const QoreBoolNode *>(data.v.value)->getValue();
      ora_checkerr(d_ora->errhp, OCIBindByPos(stmthp, &bndp, d_ora->errhp, pos, &buf.i4, sizeof(int), SQLT_INT, (dvoid *)NULL, (ub2 *)NULL, (ub2 *)NULL, (ub4)0, (ub4 *)NULL, OCI_DEFAULT), "OraBindNode::bindValue()", ds, xsink);
      return;
   }

   if (ntype == NT_INT)
   {
      const QoreBigIntNode *b = reinterpret_cast<const QoreBigIntNode *>(data.v.value);
      if (b->val <= MAXINT32 && b->val >= -MAXINT32)
      {
	 buf.i4 = b->val;
	 ora_checkerr(d_ora->errhp, OCIBindByPos(stmthp, &bndp, d_ora->errhp, pos, &buf.i4, sizeof(int), SQLT_INT, (dvoid *)NULL, (ub2 *)NULL, (ub2 *)NULL, (ub4)0, (ub4 *)NULL, OCI_DEFAULT), "OraBindNode::bindValue()", ds, xsink);
      }
      else // bind as a string value
      {
	 buftype = SQLT_STR;

	 QoreString *tstr = new QoreString(ds->getQoreEncoding());
	 tstr->sprintf("%lld", b->val);
	 data.v.tstr = tstr;

	 //printd(5, "binding number '%s'\n", buf.ptr);
	 ora_checkerr(d_ora->errhp, OCIBindByPos(stmthp, &bndp, d_ora->errhp, pos, (char *)tstr->getBuffer(), tstr->strlen() + 1, SQLT_STR, (dvoid *)NULL, (ub2 *)NULL, (ub2 *)NULL, (ub4)0, (ub4 *)NULL, OCI_DEFAULT), "OraBindNode::bindValue()", ds, xsink);
      }
      return;
   }

   if (ntype == NT_FLOAT)
   {
      ora_checkerr(d_ora->errhp, 
		   OCIBindByPos(stmthp, &bndp, d_ora->errhp, pos, &(reinterpret_cast<QoreFloatNode *>(const_cast<AbstractQoreNode *>(data.v.value))->f), sizeof(double), SQLT_BDOUBLE, (dvoid *)NULL, (ub2 *)NULL, (ub2 *)NULL, (ub4)0, (ub4 *)NULL, OCI_DEFAULT), "OraBindNode::bindValue()", ds, xsink);
      return;
   }

   xsink->raiseException("ORACLE-BIND-PLACEHOLDER-ERROR", "type '%s' is not supported for SQL binding", data.v.value->getTypeName());
}

void OraBindNode::bindPlaceholder(Datasource *ds, OCIStmt *stmthp, int pos, ExceptionSink *xsink)
{
   OracleData *d_ora = (OracleData *)ds->getPrivateData();
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

AbstractQoreNode *OraBindNode::getValue(Datasource *ds, ExceptionSink *xsink)
{
   // if NULL, then return NULL
   if (ind == -1)
      return null();

   //printd(5, "buftype: %d\n", buftype);
   if (buftype == SQLT_STR)
   {
      // must be string data
      remove_trailing_blanks((char *)buf.ptr);
      int len = strlen((char *)buf.ptr);
      QoreStringNode *str = new QoreStringNode((char *)buf.ptr, len, len + 1, ds->getQoreEncoding());
      buf.ptr = 0;
      return str;
   }
   else if (buftype == SQLT_DAT)
      return convert_date_time(buf.date);
   else if (buftype == SQLT_DATE)
      return get_oracle_timestamp(ds, buf.odt, xsink);
   else if (buftype == SQLT_INT)
      return new QoreBigIntNode(buf.i8);
   else if (buftype == SQLT_BDOUBLE)
      return new QoreFloatNode(buf.f8);
   else if (buftype == SQLT_RSET)
      return ora_fetch((OCIStmt *)buf.ptr, ds, xsink);
   else if (buftype == SQLT_LVB)
   {
      // get oracle data
      OracleData *d_ora = (OracleData *)ds->getPrivateData();
      BinaryNode *b = new BinaryNode();
      b->append(OCIRawPtr(d_ora->envhp, (OCIRaw *)buf.ptr), OCIRawSize(d_ora->envhp, (OCIRaw *)buf.ptr));
      return b;
   }
   else if (buftype == SQLT_CLOB || buftype == SQLT_BLOB)
   {
      // get oracle data
      OracleData *d_ora = (OracleData *)ds->getPrivateData();

      printd(5, "OraBindNode::getValue() using LOB locator handle %08p\n", buf.ptr);

      // retrieve *LOB data
      void *bbuf = malloc(LOB_BLOCK_SIZE);
      AbstractQoreNode *rv;
      ub4 amt = 0;
      if (buftype == SQLT_CLOB)
      {
	 QoreStringNodeHolder str(new QoreStringNode(ds->getQoreEncoding()));
	 // read LOB data in streaming callback mode
	 ora_checkerr(d_ora->errhp,
		      OCILobRead(d_ora->svchp, d_ora->errhp, (OCILobLocator *)buf.ptr, &amt, 1, bbuf, LOB_BLOCK_SIZE,
				 *str, read_clob_callback, (ub2)d_ora->charsetid, (ub1)0), "oraReadCLOBCallback()", ds, xsink);
	 rv = *xsink ? 0 : str.release();
      }
      else
      {
	 BinaryNode *b = new BinaryNode();
	 // read LOB data in streaming callback mode
	 ora_checkerr(d_ora->errhp,
		      OCILobRead(d_ora->svchp, d_ora->errhp, (OCILobLocator *)buf.ptr, &amt, 1, bbuf, LOB_BLOCK_SIZE,
				 b, read_blob_callback, (ub2)0, (ub1)0), "oraReadBLOBCallback()", ds, xsink);
	 rv = b;    
      }
      OCIDescriptorFree(buf.ptr, OCI_DTYPE_LOB);
      free(bbuf);
      return rv;
   }
   return 0;
}

QoreHashNode *OraBindGroup::getOutputHash(ExceptionSink *xsink)
{
   QoreHashNode *h = new QoreHashNode();
   OraBindNode *w = head;
   while (w)
   {
      if (w->bindtype == BN_PLACEHOLDER)
	 h->setKeyValue(w->data.ph.name, w->getValue(ds, xsink), xsink);
      w = w->next;
   }
   return h;
}

int OraBindGroup::oci_exec(char *who, ub4 iters, ExceptionSink *xsink)
{
   OracleData *d_ora = (OracleData *)ds->getPrivateData();

   int status = OCIStmtExecute(d_ora->svchp, stmthp, d_ora->errhp, iters, 0, 0, 0, OCI_DEFAULT);

   //printd(5, "oci_exec() status=%d (OCI_ERROR=%d)\n", status, OCI_ERROR);
   if (status == OCI_ERROR) {
      // see if server is connected
      ub4 server_status = 0;

      // get server handle
      OCIServer *svrh;
      if (ora_checkerr(d_ora->errhp, OCIAttrGet(d_ora->svchp, OCI_HTYPE_SVCCTX, &svrh, 0, OCI_ATTR_SERVER, d_ora->errhp), who, ds, xsink))
	 return -1;

      printd(5, "oci_exec() got server handle: %08p\n", svrh);

      if (ora_checkerr(d_ora->errhp, OCIAttrGet(svrh, OCI_HTYPE_SERVER, (dvoid *)&server_status, 0, OCI_ATTR_SERVER_STATUS, d_ora->errhp), who, ds, xsink))
	 return -1;

      printd(5, "oci_exec() server_status=%d (OCI_SERVER_NOT_CONNECTED=%d)\n", server_status, OCI_SERVER_NOT_CONNECTED);

      if (server_status == OCI_SERVER_NOT_CONNECTED) {
	 // check if a transaction was in progress
	 if (ds->isInTransaction()) {
	    ds->connectionAborted();
	    xsink->raiseException("DBI:ORACLE:TRANSACTION-ERROR", "connection to Oracle database server lost while in a transaction; transaction has been lost");
	    return -1;
	 }

	 // otherwise try to reconnect
	 OCILogoff(d_ora->svchp, d_ora->errhp);

	 printd(5, "oci_exec() about to execute OCILogon() for reconnect\n");
	 if (ora_checkerr(d_ora->errhp, OCILogon(d_ora->envhp, d_ora->errhp, &d_ora->svchp, (text *)ds->getUsername(), strlen(ds->getUsername()), (text *)ds->getPassword(), strlen(ds->getPassword()), (text *)ds->getDBName(), strlen(ds->getDBName())), who, ds, xsink))
	    return -1;

	 printd(5, "oci_exec() returned from OCILogon() status=%d\n", status);
	 status = OCIStmtExecute(d_ora->svchp, stmthp, d_ora->errhp, iters, 0, 0, 0, OCI_DEFAULT);
	 if (status && ora_checkerr(d_ora->errhp, status, who, ds, xsink))
	    return -1;
      }
      else {
	 ora_checkerr(d_ora->errhp, status, who, ds, xsink);
	 return -1;
      }
   }
   else if (status && ora_checkerr(d_ora->errhp, status, who, ds, xsink))
      return -1;

   return 0;
}

AbstractQoreNode *OraBindGroup::exec(ExceptionSink *xsink)
{
   if (oci_exec("OraBindGroup::exec", 1, xsink))
      return 0;

   OracleData *d_ora = (OracleData *)ds->getPrivateData();

   int status;
   /*
   status = OCIStmtExecute(d_ora->svchp, stmthp, d_ora->errhp, 1, 0, NULL, NULL, OCI_DEFAULT);
   if (status)
      ora_checkerr(d_ora->errhp, status, "OraBindGroup::exec()", ds, xsink);
   */

   AbstractQoreNode *rv;
   if (!xsink->isEvent())
   {
      // if there are output variables, then fix values if necessary and return
      if (hasOutput)
	 rv = getOutputHash(xsink);
      else // get row count
      {
	 int rc = 0;
	 ora_checkerr(d_ora->errhp, OCIAttrGet(stmthp, OCI_HTYPE_STMT, &rc, 0, OCI_ATTR_ROW_COUNT, d_ora->errhp), "OraBindGroup::exec():attr", ds, xsink);
	 rv = new QoreBigIntNode(rc);
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

AbstractQoreNode *OraBindGroup::select(ExceptionSink *xsink)
{
   if (oci_exec("OraBindGroup::select", 0, xsink))
      return 0;

   OracleData *d_ora = (OracleData *)ds->getPrivateData();

   int status;
   /*
   status = OCIStmtExecute(d_ora->svchp, stmthp, d_ora->errhp, 0, 0, NULL, NULL, OCI_DEFAULT);
   if (status)
      ora_checkerr(d_ora->errhp, status, "OraBindGroup::select()", ds, xsink);
   */

   QoreHashNode *h = 0;
   if (!xsink->isEvent())
      h = ora_fetch(stmthp, ds, xsink);

   // commit transaction if autocommit set for datasource
   if (!xsink->isEvent() && ds->getAutoCommit())
      if ((status = OCITransCommit(d_ora->svchp, d_ora->errhp, (ub4) 0)))
	 ora_checkerr(d_ora->errhp, status, "OraBindGroup():commit", ds, xsink);

   return h;
}

AbstractQoreNode *OraBindGroup::selectRows(ExceptionSink *xsink)
{
   if (oci_exec("OraBindGroup::selectRows", 0, xsink))
      return 0;

   OracleData *d_ora = (OracleData *)ds->getPrivateData();

   int status;
   /*
   status = OCIStmtExecute(d_ora->svchp, stmthp, d_ora->errhp, 0, 0, NULL, NULL, OCI_DEFAULT);
   if (status)
      ora_checkerr(d_ora->errhp, status, "OraBindGroup::select()", ds, xsink);
   */

   QoreListNode *l = NULL;
   if (!xsink->isEvent())
      l = ora_fetch_horizontal(stmthp, ds, xsink);

   // commit transaction if autocommit set for datasource
   if (!xsink->isEvent() && ds->getAutoCommit())
      if ((status = OCITransCommit(d_ora->svchp, d_ora->errhp, (ub4) 0)))
	 ora_checkerr(d_ora->errhp, status, "OraBindGroup():commit", ds, xsink);

   return l;
}

static AbstractQoreNode *oracle_exec(Datasource *ds, const QoreString *qstr, const QoreListNode *args, ExceptionSink *xsink)
{
   OraBindGroup bg(ds, qstr, args, xsink);

   if (xsink->isException())
      return NULL;

   return bg.exec(xsink);
}

static AbstractQoreNode *oracle_select(Datasource *ds, const QoreString *qstr, const QoreListNode *args, ExceptionSink *xsink)
{
   OraBindGroup bg(ds, qstr, args, xsink);

   if (xsink->isException())
      return NULL;

   return bg.select(xsink);
}

static AbstractQoreNode *oracle_select_rows(Datasource *ds, const QoreString *qstr, const QoreListNode *args, ExceptionSink *xsink)
{
   OraBindGroup bg(ds, qstr, args, xsink);

   if (xsink->isException())
      return NULL;

   return bg.selectRows(xsink);
}

static int oracle_open(Datasource *ds, ExceptionSink *xsink)
{
   printd(5, "oracle_open() datasource %08p for DB=%s open\n", ds, ds->getDBName());

   if (!ds->getUsername()) {
      xsink->raiseException("DATASOURCE-MISSING-USERNAME", "Datasource has an empty username parameter");
      return -1;
   }
   if (!ds->getPassword()) {
      xsink->raiseException("DATASOURCE-MISSING-PASSWORD", "Datasource has an empty password parameter");
      return -1;
   }
   if (!ds->getDBName()) {
      xsink->raiseException("DATASOURCE-MISSING-DBNAME", "Datasource has an empty dbname parameter");
      return -1;
   }
   printd(5, "oracle_open(): user=%s pass=%s db=%s (oracle encoding=%s)\n",
	  ds->getUsername(), ds->getPassword(), ds->getDBName(), ds->getDBEncoding() ? ds->getDBEncoding() : "(none)");

   std::auto_ptr<OracleData> d_ora(new OracleData);

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

   if (ds->getDBEncoding()) {
      charset = ds->getDBEncoding();
      need_to_set_charset = 1;
   }
   else { // get Oracle character set name from OS character set name
      if ((OCINlsNameMap(tmpenvhp, (oratext *)nbuf, OCI_NLS_MAXBUFSZ, (oratext *)QCS_DEFAULT->getCode(), OCI_NLS_CS_IANA_TO_ORA) != OCI_SUCCESS)) {
	 OCIHandleFree(tmpenvhp, OCI_HTYPE_ENV);
	 xsink->raiseException("DBI:ORACLE:UNKNOWN-CHARACTER-SET", 
			       "cannot map default OS encoding '%s' to Oracle character encoding",
			       QCS_DEFAULT->getCode());
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

   if (!d_ora->charsetid) {
      xsink->raiseException("DBI:ORACLE:UNKNOWN-CHARACTER-SET", "this installation of Oracle does not support the '%s' character encoding", 
			    ds->getDBEncoding());
      return -1;
   }

   printd(5, "Oracle character encoding '%s' has ID %d, oci_flags=%d\n", charset, d_ora->charsetid, oci_flags);
   // create environment with default character set
   if (OCIEnvNlsCreate(&d_ora->envhp, oci_flags, 0, NULL, NULL, NULL, 0, NULL, d_ora->charsetid, d_ora->charsetid) != OCI_SUCCESS) {
      xsink->raiseException("DBI:ORACLE:OPEN-ERROR", "error creating new environment handle with encoding '%s'", ds->getDBEncoding());
      return -1;
   }

   // map the Oracle character set to a qore character set
   if (need_to_set_charset)
   {
      // map Oracle character encoding name to QORE/OS character encoding name
      if ((OCINlsNameMap(d_ora->envhp, (oratext *)nbuf, OCI_NLS_MAXBUFSZ, (oratext *)ds->getDBEncoding(), OCI_NLS_CS_ORA_TO_IANA) == OCI_SUCCESS)) {
	 printd(5, "oracle_open() Oracle character set '%s' mapped to '%s' character set\n", ds->getDBEncoding(), nbuf);
	 ds->setQoreEncoding(nbuf);
      }
      else {
	 xsink->raiseException("DBI:ORACLE:OPEN-ERROR", "error mapping Oracle encoding '%s' to a qore encoding: unknown encoding", ds->getDBEncoding());
	 return -1;
      }
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

   if (OCIHandleAlloc(d_ora->envhp, (dvoid **) &d_ora->errhp, OCI_HTYPE_ERROR, 0, 0) != OCI_SUCCESS) {
      OCIHandleFree(d_ora->envhp, OCI_HTYPE_ENV);
      xsink->raiseException("DBI:ORACLE:OPEN-ERROR", "failed to allocate error handle for connection");
      return -1;
   }
   //printd(5, "oracle_open() about to call OCILogon()\n");
   ora_checkerr(d_ora->errhp, 
		OCILogon(d_ora->envhp, d_ora->errhp, &d_ora->svchp, (text *)ds->getUsername(), strlen(ds->getUsername()), (text *)ds->getPassword(), strlen(ds->getPassword()), (text *)ds->getDBName(), strlen(ds->getDBName())), 
		"<open>", ds, xsink);
   if (xsink->isEvent()) {
      OCIHandleFree(d_ora->errhp, OCI_HTYPE_ERROR);
      OCIHandleFree(d_ora->envhp, OCI_HTYPE_ENV);
      return -1;
   }

   printd(5, "oracle_open() datasource %08p for DB=%s open (envhp=%08p)\n", ds, ds->getDBName(), d_ora->envhp);

   ds->setPrivateData((void *)d_ora.release());

   return 0;
}

static int oracle_close(Datasource *ds)
{
   tracein("oracle_close()");

   OracleData *d_ora = (OracleData *)ds->getPrivateData();

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
static AbstractQoreNode *oracle_get_server_version(Datasource *ds, ExceptionSink *xsink)
{
   // get private data structure for connection
   OracleData *d_ora = (OracleData *)ds->getPrivateData();
   
   // buffer for version information
   char version_buf[VERSION_BUF_SIZE + 1];
   
   // execute OCIServerVersion and check status code
   ora_checkerr(d_ora->errhp, 
   OCIServerVersion(d_ora->svchp, d_ora->errhp, (OraText *)version_buf, VERSION_BUF_SIZE, OCI_HTYPE_SVCCTX),
   "oracle_get_server_version", ds, xsink);
   if (*xsink)
      return 0;
   
   return new QoreStringNode(version_buf);   
}

#ifdef HAVE_OCICLIENTVERSION
static AbstractQoreNode *oracle_get_client_version(const Datasource *ds, ExceptionSink *xsink)
{
   sword major, minor, update, patch, port_update;

   OCIClientVersion(&major, &minor, &update, &patch, &port_update);
   QoreHashNode *h = new QoreHashNode();
   h->setKeyValue("major", new QoreBigIntNode(major), NULL);
   h->setKeyValue("minor", new QoreBigIntNode(minor), NULL);
   h->setKeyValue("update", new QoreBigIntNode(update), NULL);
   h->setKeyValue("patch", new QoreBigIntNode(patch), NULL);
   h->setKeyValue("port_update", new QoreBigIntNode(port_update), NULL);
   return h;
}
#endif

QoreStringNode *oracle_module_init()
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
   methods.add(QDBI_METHOD_GET_SERVER_VERSION, oracle_get_server_version);
#ifdef HAVE_OCICLIENTVERSION
   methods.add(QDBI_METHOD_GET_CLIENT_VERSION, oracle_get_client_version);
#endif
   
   DBID_ORACLE = DBI.registerDriver("oracle", methods, DBI_ORACLE_CAPS);

   traceout("oracle_module_init()");
   return NULL;
}

void oracle_module_ns_init(QoreNamespace *rns, QoreNamespace *qns)
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
