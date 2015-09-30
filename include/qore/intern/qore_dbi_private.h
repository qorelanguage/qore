/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  qore_dbi_private.h

  Qore Programming Language

  Copyright (C) 2003 - 2015 David Nichols

  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.

  Note that the Qore library is released under a choice of three open-source
  licenses: MIT (as above), LGPL 2+, or GPL 2+; see README-LICENSE for more
  information.
*/

#ifndef _QORE_QORE_DBI_PRIVATE_H
#define _QORE_QORE_DBI_PRIVATE_H

#include <map>

// internal DBI definitions
typedef std::map<int, void*> dbi_method_list_t;
struct DbiOptInfo {
   const char* desc;
   const QoreTypeInfo *typeInfo;

   DLLLOCAL DbiOptInfo() : desc(0), typeInfo(0) {
   }

   DLLLOCAL DbiOptInfo(const char* d, const QoreTypeInfo* t) : desc(d), typeInfo(t) {
   }
};
typedef std::map<const char*, DbiOptInfo, ltcstrcase> dbi_opt_map_t;

struct dbi_driver_stmt {
   q_dbi_stmt_prepare_t prepare;
   q_dbi_stmt_prepare_raw_t prepare_raw;
   q_dbi_stmt_bind_t bind, bind_placeholders, bind_values;
   q_dbi_stmt_exec_t exec;
   q_dbi_stmt_fetch_row_t fetch_row;
   q_dbi_stmt_fetch_rows_t fetch_rows;
   q_dbi_stmt_fetch_columns_t fetch_columns;
   q_dbi_stmt_fetch_row_t describe;
   q_dbi_stmt_next_t next;
   q_dbi_stmt_define_t define;
   q_dbi_stmt_close_t close;
   q_dbi_stmt_affected_rows_t affected_rows;
   q_dbi_stmt_get_output_t get_output;
   q_dbi_stmt_get_output_rows_t get_output_rows;

   DLLLOCAL dbi_driver_stmt() : prepare(0), prepare_raw(0), bind(0), bind_placeholders(0),
                                bind_values(0), exec(0), fetch_row(0), fetch_rows(0),
                                fetch_columns(0), describe(0), next(0), define(0),
                                close(0), affected_rows(0), get_output(0), get_output_rows(0) {
   }
};

struct dbi_driver_opt {
   q_dbi_option_set_t set;
   q_dbi_option_get_t get;

   DLLLOCAL dbi_driver_opt() : set(0), get(0) {
   }
};

struct DBIDriverFunctions {
   q_dbi_open_t open;
   q_dbi_close_t close;
   q_dbi_select_t select;
   q_dbi_select_rows_t selectRows;
   q_dbi_select_row_t selectRow;
   q_dbi_exec_t execSQL;
   q_dbi_execraw_t execRawSQL;
   q_dbi_describe_t describe;
   q_dbi_commit_t commit;
   q_dbi_rollback_t rollback;
   q_dbi_begin_transaction_t begin_transaction; // for DBI drivers that require explicit transaction starts
   q_dbi_get_server_version_t get_server_version;
   q_dbi_get_client_version_t get_client_version;

   dbi_driver_stmt stmt;
   dbi_driver_opt opt;

   DLLLOCAL DBIDriverFunctions() : open(0), close(0), select(0), selectRows(0), selectRow(0),
                                   execSQL(0), execRawSQL(0), describe(0), commit(0), rollback(0),
                                   begin_transaction(0), get_server_version(0), get_client_version(0) {
   }
};

// helper class that will edit argument lists and convert number values to floats if the driver does not support the "number" type (QoreNumberNode)
class DbiArgHelper {
protected:
   const QoreListNode* orig;
   QoreListNode* nl;
   ExceptionSink* xsink;

public:
   DLLLOCAL DbiArgHelper(const QoreListNode* ol, bool numeric, ExceptionSink* xs);

   DLLLOCAL ~DbiArgHelper() {
      if (nl)
         nl->deref(xsink);
   }

   DLLLOCAL const QoreListNode* get() const {
      return nl ? nl : orig;
   }

   DLLLOCAL const QoreListNode* operator*() const {
      return nl ? nl : orig;
   }
};

struct OptInputHelper {
   ExceptionSink* xsink;
   AbstractQoreNode* val;
   bool tmp;

   DLLLOCAL OptInputHelper(ExceptionSink* xs, const qore_dbi_private& driver, const char* opt, bool set = false, const AbstractQoreNode* v = 0);

   DLLLOCAL ~OptInputHelper() {
      if (tmp)
         val->deref(xsink);
   }

   DLLLOCAL operator bool() const {
      return !*xsink;
   }
};

struct qore_dbi_private {
   DBIDriverFunctions f;
   int caps;
   const char* name;
   dbi_opt_map_t omap;

   DLLLOCAL qore_dbi_private(const char* nme, const qore_dbi_mlist_private& methods, int cps);

   DLLLOCAL bool hasStatementAPI() const {
      return caps & DBI_CAP_HAS_STATEMENT;
   }

   DLLLOCAL int init(Datasource* ds, ExceptionSink* xsink) const {
      int rc = f.open(ds, xsink);
      assert((!rc && !*xsink) || (rc && *xsink));
      // set option if init was successful
      if (!rc && f.opt.set) {
         ConstHashIterator hi(ds->getConnectOptions());
         while (hi.next())
            f.opt.set(ds, hi.getKey(), hi.getValue(), xsink);
      }
      return rc;
   }

   DLLLOCAL int close(Datasource* ds) const {
      return f.close(ds);
   }

   DLLLOCAL AbstractQoreNode* select(Datasource* ds, const QoreString* sql, const QoreListNode* args, ExceptionSink* xsink) const {
      DbiArgHelper dargs(args, (caps & DBI_CAP_HAS_NUMBER_SUPPORT), xsink);
      return f.select(ds, sql, *dargs, xsink);
   }

   DLLLOCAL AbstractQoreNode* selectRows(Datasource* ds, const QoreString* sql, const QoreListNode* args, ExceptionSink* xsink) const {
      DbiArgHelper dargs(args, (caps & DBI_CAP_HAS_NUMBER_SUPPORT), xsink);
      return f.selectRows(ds, sql, *dargs, xsink);
   }

   DLLLOCAL QoreHashNode* selectRow(Datasource* ds, const QoreString* sql, const QoreListNode* args, ExceptionSink* xsink) const {
      DbiArgHelper dargs(args, (caps & DBI_CAP_HAS_NUMBER_SUPPORT), xsink);

      if (f.selectRow)
         return f.selectRow(ds, sql, *dargs, xsink);

      ReferenceHolder<AbstractQoreNode> res(f.selectRows(ds, sql, *dargs, xsink), xsink);
      if (!res)
         return 0;

      if (res->getType() != NT_HASH) {
         assert(res->getType() == NT_LIST);
         QoreListNode* l = reinterpret_cast<QoreListNode*>(*res);
         assert(l->size() <= 1);
         AbstractQoreNode* n = l->shift();
         assert(!n || n->getType() == NT_HASH);
         return reinterpret_cast<QoreHashNode*>(n);
      }

      return reinterpret_cast<QoreHashNode*>(res.release());
   }

   DLLLOCAL AbstractQoreNode* execSQL(Datasource* ds, const QoreString* sql, const QoreListNode* args, ExceptionSink* xsink) const {
      DbiArgHelper dargs(args, (caps & DBI_CAP_HAS_NUMBER_SUPPORT), xsink);
      return f.execSQL(ds, sql, *dargs, xsink);
   }

   DLLLOCAL AbstractQoreNode* execRawSQL(Datasource* ds, const QoreString* sql, ExceptionSink* xsink) const {
      if (!f.execRawSQL) {
         xsink->raiseException("DBI-EXEC-RAW-SQL-ERROR", "this driver does not implement the Datasource::execRawSQL() method");
         return 0;
      }
      return f.execRawSQL(ds, sql, xsink);
   }

   DLLLOCAL QoreHashNode* describe(Datasource* ds, const QoreString* sql, const QoreListNode* args, ExceptionSink* xsink) {
      if (!f.describe) {
         xsink->raiseException("DBI-DESCRIBE-ERROR", "this driver does not implement the Datasource::describe() method");
         return 0;
      }
      DbiArgHelper dargs(args, (caps & DBI_CAP_HAS_NUMBER_SUPPORT), xsink);
      return f.describe(ds, sql, *dargs, xsink);
   }

   DLLLOCAL int commit(Datasource* ds, ExceptionSink* xsink) const {
      return f.commit(ds, xsink);
   }

   DLLLOCAL int rollback(Datasource* ds, ExceptionSink* xsink) const {
      return f.rollback(ds, xsink);
   }

   DLLLOCAL int beginTransaction(Datasource* ds, ExceptionSink* xsink) const {
      if (f.begin_transaction)
         return f.begin_transaction(ds, xsink);
      return 0; // 0 = OK
   }

   DLLLOCAL int autoCommit(Datasource* ds, ExceptionSink* xsink) const {
      // if the driver does not require explicit "begin" statements to
      // start a transaction, then we have to explicitly call "commit" here
      if (!f.begin_transaction)
         return f.commit(ds, xsink);

      return 0; // 0 = OK
   }

   DLLLOCAL AbstractQoreNode* getServerVersion(Datasource* ds, ExceptionSink* xsink) const {
      if (f.get_server_version)
         return f.get_server_version(ds, xsink);
      return 0;
   }

   DLLLOCAL AbstractQoreNode* getClientVersion(const Datasource* ds, ExceptionSink* xsink) const {
      if (f.get_client_version)
         return f.get_client_version(ds, xsink);
      return 0;
   }

   DLLLOCAL int getCaps() const {
      return caps;
   }

   DLLLOCAL QoreListNode* getCapList() const;

   DLLLOCAL int stmt_prepare(SQLStatement* stmt, const QoreString& str, const QoreListNode* args, ExceptionSink* xsink) const {
      DbiArgHelper dargs(args, (caps & DBI_CAP_HAS_NUMBER_SUPPORT), xsink);
      return f.stmt.prepare(stmt, str, *dargs, xsink);
   }

   DLLLOCAL int stmt_prepare_raw(SQLStatement* stmt, const QoreString& str, ExceptionSink* xsink) const {
      return f.stmt.prepare_raw(stmt, str, xsink);
   }

   DLLLOCAL int stmt_bind(SQLStatement* stmt, const QoreListNode& l, ExceptionSink* xsink) const {
      return f.stmt.bind(stmt, l, xsink);
   }

   DLLLOCAL int stmt_bind_placeholders(SQLStatement* stmt, const QoreListNode& l, ExceptionSink* xsink) const {
      if (!f.stmt.bind_placeholders) {
         xsink->raiseException("SQLSTATEMENT-BIND-PLACEHOLDERS-ERROR", "the '%s' driver does not require placeholder buffer specifications so the SQLStatement::bindPlaceholders() method is not supported", name);
         return -1;
      }

      return f.stmt.bind_placeholders(stmt, l, xsink);
   }

   DLLLOCAL int stmt_bind_values(SQLStatement* stmt, const QoreListNode& l, ExceptionSink* xsink) const {
      return f.stmt.bind_values(stmt, l, xsink);
   }

   DLLLOCAL int stmt_define(SQLStatement* stmt, ExceptionSink* xsink) const {
      return f.stmt.define(stmt, xsink);
   }

   DLLLOCAL int stmt_exec(SQLStatement* stmt, ExceptionSink* xsink) const {
      return f.stmt.exec(stmt, xsink);
   }

   DLLLOCAL int stmt_affected_rows(SQLStatement* stmt, ExceptionSink* xsink) const {
      return f.stmt.affected_rows(stmt, xsink);
   }

   DLLLOCAL QoreHashNode* stmt_get_output(SQLStatement* stmt, ExceptionSink* xsink) const {
      return f.stmt.get_output(stmt, xsink);
   }

   DLLLOCAL QoreHashNode* stmt_get_output_rows(SQLStatement* stmt, ExceptionSink* xsink) const {
      return f.stmt.get_output_rows(stmt, xsink);
   }

   DLLLOCAL QoreHashNode* stmt_fetch_row(SQLStatement* stmt, ExceptionSink* xsink) const {
      return f.stmt.fetch_row(stmt, xsink);
   }

   DLLLOCAL QoreListNode* stmt_fetch_rows(SQLStatement* stmt, int rows, ExceptionSink* xsink) const {
      return f.stmt.fetch_rows(stmt, rows, xsink);
   }

   DLLLOCAL QoreHashNode* stmt_fetch_columns(SQLStatement* stmt, int rows, ExceptionSink* xsink) const {
      return f.stmt.fetch_columns(stmt, rows, xsink);
   }

   DLLLOCAL QoreHashNode* stmt_describe(SQLStatement* stmt, ExceptionSink* xsink) const {
      if (!f.stmt.describe) {
         xsink->raiseException("DBI-DESCRIBE-ERROR", "this driver does not implement the SQLStatement::describe() method");
         return 0;
      }
      return f.stmt.describe(stmt, xsink);
   }

   DLLLOCAL bool stmt_next(SQLStatement* stmt, ExceptionSink* xsink) const {
      return f.stmt.next(stmt, xsink);
   }

   DLLLOCAL int stmt_close(SQLStatement* stmt, ExceptionSink* xsink) const {
      return f.stmt.close(stmt, xsink);
   }

   DLLLOCAL int opt_set(Datasource* ds, const char* opt, const AbstractQoreNode* val, ExceptionSink* xsink) {
      OptInputHelper oh(xsink, *this, opt, true, val);
      if (!oh)
         return -1;

      return f.opt.set(ds, opt, oh.val, xsink);
   }

   DLLLOCAL AbstractQoreNode* opt_get(const Datasource* ds, const char* opt, ExceptionSink* xsink) {
      OptInputHelper oh(xsink, *this, opt);
      if (!oh)
         return 0;

      return f.opt.get(ds, opt);
   }

   DLLLOCAL QoreHashNode* getOptionHash(const Datasource* ds) const {
      QoreHashNode* rv = new QoreHashNode;

      for (dbi_opt_map_t::const_iterator i = omap.begin(), e = omap.end(); i != e; ++i) {
         QoreHashNode* h = new QoreHashNode;
         h->setKeyValue("desc", new QoreStringNode(i->second.desc), 0);
         h->setKeyValue("type", new QoreStringNode(i->second.typeInfo->getName()), 0);
         h->setKeyValue("value", f.opt.get(ds, i->first), 0);

         rv->setKeyValue(i->first, h, 0);
      }
      return rv;
   }

   DLLLOCAL QoreHashNode* getOptionHash() const {
      QoreHashNode* rv = new QoreHashNode;

      for (dbi_opt_map_t::const_iterator i = omap.begin(), e = omap.end(); i != e; ++i) {
         QoreHashNode* h = new QoreHashNode;
         h->setKeyValue("desc", new QoreStringNode(i->second.desc), 0);
         h->setKeyValue("type", new QoreStringNode(i->second.typeInfo->getName()), 0);

         rv->setKeyValue(i->first, h, 0);
      }
      return rv;
   }

   DLLLOCAL static qore_dbi_private* get(const DBIDriver& d) {
      return d.priv;
   }
};

#endif
