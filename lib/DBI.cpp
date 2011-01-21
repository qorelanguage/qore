/*
  DBI.cpp

  Database Independent SQL Layer

  Qore Programming Language

  Copyright 2003 - 2010 David Nichols

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
#include <qore/intern/QC_Datasource.h>
#include <qore/intern/QC_DatasourcePool.h>
#include <qore/intern/QC_SQLStatement.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

#define NUM_DBI_CAPS 8

typedef safe_dslist<DBIDriver *> dbi_list_t;

// global qore library class for DBI driver management
DBIDriverList DBI;

struct dbi_cap_hash {
   int cap;
   const char *desc;
};

struct dbi_cap_hash dbi_cap_list[] =
{ { DBI_CAP_TIME_ZONE_SUPPORT,      "TimeZoneSupport" },
  { DBI_CAP_CHARSET_SUPPORT,        "CharsetSupport" },
  { DBI_CAP_TRANSACTION_MANAGEMENT, "TransactionManagement" },
  { DBI_CAP_STORED_PROCEDURES,      "StoredProcedures" },
  { DBI_CAP_LOB_SUPPORT,            "LargeObjectSupport" },
  { DBI_CAP_BIND_BY_VALUE,          "BindByValue" },
  { DBI_CAP_BIND_BY_PLACEHOLDER,    "BindByPlaceholder" },
  { DBI_CAP_HAS_EXECRAW,            "HasExecRaw" },
  { DBI_CAP_HAS_STATEMENT,          "HasStatementApi" },
  { DBI_CAP_HAS_SELECT_ROW,         "HasSelectRow" },
};

struct dbi_driver_stmt {
   q_dbi_stmt_prepare_t prepare;
   q_dbi_stmt_prepare_raw_t prepare_raw;
   q_dbi_stmt_bind_t bind, bind_placeholders, bind_values;
   q_dbi_stmt_exec_t exec;
   q_dbi_stmt_fetch_row_t fetch_row;
   q_dbi_stmt_fetch_rows_t fetch_rows;
   q_dbi_stmt_fetch_columns_t fetch_columns;
   q_dbi_stmt_next_t next;
   q_dbi_stmt_define_t define;
   q_dbi_stmt_close_t close;
   q_dbi_stmt_affected_rows_t affected_rows;
   q_dbi_stmt_get_output_t get_output;
   q_dbi_stmt_get_output_rows_t get_output_rows;
   
   DLLLOCAL dbi_driver_stmt() : prepare(0), prepare_raw(0), bind(0), bind_placeholders(0), 
				bind_values(0), exec(0), fetch_row(0), fetch_rows(0),
				fetch_columns(0), next(0), define(0),
				close(0), affected_rows(0), get_output(0), get_output_rows(0) {
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
   q_dbi_commit_t commit;
   q_dbi_rollback_t rollback;
   q_dbi_begin_transaction_t begin_transaction; // for DBI drivers that require explicit transaction starts
   q_dbi_abort_transaction_start_t abort_transaction_start;  // for DBI drivers that require a rollback in order to use
   // the connection after an exception as the first statement
   // in a transaction
   q_dbi_get_server_version_t get_server_version;
   q_dbi_get_client_version_t get_client_version;

   dbi_driver_stmt stmt;

   DLLLOCAL DBIDriverFunctions() : open(0), close(0), select(0), selectRows(0), selectRow(0), 
				   execSQL(0), execRawSQL(0), commit(0), rollback(0), 
				   begin_transaction(0), abort_transaction_start(0),
				   get_server_version(0), get_client_version(0) {
   }
};

struct qore_dbi_mlist_private {
      dbi_method_list_t l;
};

qore_dbi_method_list::qore_dbi_method_list() : priv(new qore_dbi_mlist_private) {
}

qore_dbi_method_list::~qore_dbi_method_list() {
   delete priv;
}

// covers open, commit, rollback, begin transaction, and abort transaction start
void qore_dbi_method_list::add(int code, q_dbi_open_t method) {
   assert(code == QDBI_METHOD_OPEN || code == QDBI_METHOD_COMMIT || code == QDBI_METHOD_ROLLBACK || code == QDBI_METHOD_BEGIN_TRANSACTION
      || code == QDBI_METHOD_ABORT_TRANSACTION_START);
   priv->l.push_back(std::make_pair(code, (void *)method));
}

// for close
void qore_dbi_method_list::add(int code, q_dbi_close_t method) {
   assert(code == QDBI_METHOD_CLOSE);
   priv->l.push_back(std::make_pair(code, (void *)method));
}

// covers select, select_rows. and exec
void qore_dbi_method_list::add(int code, q_dbi_select_t method) {
   assert(code == QDBI_METHOD_SELECT || code == QDBI_METHOD_SELECT_ROWS || code == QDBI_METHOD_EXEC);
   priv->l.push_back(std::make_pair(code, (void *)method));
}

// covers execRaw
void qore_dbi_method_list::add(int code, q_dbi_execraw_t method) {
   assert(code == QDBI_METHOD_EXECRAW);
   priv->l.push_back(std::make_pair(code, (void *)method));
}

// covers get_server_version
void qore_dbi_method_list::add(int code, q_dbi_get_server_version_t method) {
   assert(code == QDBI_METHOD_GET_SERVER_VERSION);
   priv->l.push_back(std::make_pair(code, (void *)method));
}

// covers get_client_version
void qore_dbi_method_list::add(int code, q_dbi_get_client_version_t method) {
   assert(code == QDBI_METHOD_GET_CLIENT_VERSION);
   priv->l.push_back(std::make_pair(code, (void *)method));
}

// covers stmt prepare
void qore_dbi_method_list::add(int code, q_dbi_stmt_prepare_t method) {
   assert(code == QDBI_METHOD_STMT_PREPARE);
   priv->l.push_back(std::make_pair(code, (void *)method));
}

// covers stmt prepare_raw
void qore_dbi_method_list::add(int code, q_dbi_stmt_prepare_raw_t method) {
   assert(code == QDBI_METHOD_STMT_PREPARE_RAW);
   priv->l.push_back(std::make_pair(code, (void *)method));
}

// covers stmt bind
void qore_dbi_method_list::add(int code, q_dbi_stmt_bind_t method) {
   assert(code == QDBI_METHOD_STMT_BIND || code == QDBI_METHOD_STMT_BIND_PLACEHOLDERS || code == QDBI_METHOD_STMT_BIND_VALUES);
   priv->l.push_back(std::make_pair(code, (void *)method));
}

// covers stmt exec, close, define, and affectedRows
void qore_dbi_method_list::add(int code, q_dbi_stmt_exec_t method) {
   assert(code == QDBI_METHOD_STMT_EXEC || code == QDBI_METHOD_STMT_CLOSE || code == QDBI_METHOD_STMT_DEFINE || code == QDBI_METHOD_STMT_AFFECTED_ROWS);
   priv->l.push_back(std::make_pair(code, (void *)method));
}

// covers stmt fetch_row, get_output and get_output_rows
void qore_dbi_method_list::add(int code, q_dbi_stmt_fetch_row_t method) {
   assert(code == QDBI_METHOD_STMT_FETCH_ROW || code == QDBI_METHOD_STMT_GET_OUTPUT_ROWS || code == QDBI_METHOD_STMT_GET_OUTPUT);
   priv->l.push_back(std::make_pair(code, (void *)method));
}

// covers stmt fetch_rows
void qore_dbi_method_list::add(int code, q_dbi_stmt_fetch_rows_t method) {
   assert(code == QDBI_METHOD_STMT_FETCH_ROWS);
   priv->l.push_back(std::make_pair(code, (void *)method));
}

// covers stmt fetch_columns
void qore_dbi_method_list::add(int code, q_dbi_stmt_fetch_columns_t method) {
   assert(code == QDBI_METHOD_STMT_FETCH_COLUMNS);
   priv->l.push_back(std::make_pair(code, (void *)method));
}

// covers stmt next
void qore_dbi_method_list::add(int code, q_dbi_stmt_next_t method) {
   assert(code == QDBI_METHOD_STMT_NEXT);
   priv->l.push_back(std::make_pair(code, (void *)method));
}

dbi_method_list_t *qore_dbi_method_list::getMethods() const {
   return &priv->l;
}

struct qore_dbi_private {
   DBIDriverFunctions f;
   int caps;
   const char *name;
   DLLLOCAL qore_dbi_private(const char *nme, const dbi_method_list_t &methods, int cps) {
      // add methods to internal data structure
      for (dbi_method_list_t::const_iterator i = methods.begin(), e = methods.end(); i != e; ++i) {
	 assert((*i).first > 0 && (*i).first <= QDBI_VALID_CODES);
	 switch ((*i).first) {
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
	    case QDBI_METHOD_SELECT_ROW:
	       assert(!f.selectRow);
	       f.selectRow = (q_dbi_select_row_t)(*i).second;
	       break;
	    case QDBI_METHOD_EXEC:
	       assert(!f.execSQL);
	       f.execSQL = (q_dbi_exec_t)(*i).second;
	       break;
	    case QDBI_METHOD_EXECRAW:
               assert(!f.execRawSQL);
               f.execRawSQL = (q_dbi_execraw_t)(*i).second;
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
	    case QDBI_METHOD_STMT_PREPARE:
	       assert(!f.stmt.prepare);
	       f.stmt.prepare = (q_dbi_stmt_prepare_t)(*i).second;
	       break;
	    case QDBI_METHOD_STMT_PREPARE_RAW:
	       assert(!f.stmt.prepare_raw);
	       f.stmt.prepare_raw = (q_dbi_stmt_prepare_raw_t)(*i).second;
	       break;
	    case QDBI_METHOD_STMT_BIND:
	       assert(!f.stmt.bind);
	       f.stmt.bind = (q_dbi_stmt_bind_t)(*i).second;
	       break;
	    case QDBI_METHOD_STMT_BIND_PLACEHOLDERS:
	       assert(!f.stmt.bind_placeholders);
	       f.stmt.bind_placeholders = (q_dbi_stmt_bind_t)(*i).second;
	       break;
	    case QDBI_METHOD_STMT_BIND_VALUES:
	       assert(!f.stmt.bind_values);
	       f.stmt.bind_values = (q_dbi_stmt_bind_t)(*i).second;
	       break;
	    case QDBI_METHOD_STMT_EXEC:
	       assert(!f.stmt.exec);
	       f.stmt.exec = (q_dbi_stmt_exec_t)(*i).second;
	       break;
	    case QDBI_METHOD_STMT_FETCH_ROW:
	       assert(!f.stmt.fetch_row);
	       f.stmt.fetch_row = (q_dbi_stmt_fetch_row_t)(*i).second;
	       break;
	    case QDBI_METHOD_STMT_FETCH_ROWS:
	       assert(!f.stmt.fetch_rows);
	       f.stmt.fetch_rows = (q_dbi_stmt_fetch_rows_t)(*i).second;
	       break;
	    case QDBI_METHOD_STMT_FETCH_COLUMNS:
	       assert(!f.stmt.fetch_columns);
	       f.stmt.fetch_columns = (q_dbi_stmt_fetch_columns_t)(*i).second;
	       break;
	    case QDBI_METHOD_STMT_NEXT:
	       assert(!f.stmt.next);
	       f.stmt.next = (q_dbi_stmt_next_t)(*i).second;
	       break;
	    case QDBI_METHOD_STMT_DEFINE:
	       assert(!f.stmt.define);
	       f.stmt.define = (q_dbi_stmt_define_t)(*i).second;
	       break;
	    case QDBI_METHOD_STMT_CLOSE:
	       assert(!f.stmt.close);
	       f.stmt.close = (q_dbi_stmt_close_t)(*i).second;
	       break;
	    case QDBI_METHOD_STMT_AFFECTED_ROWS:
	       assert(!f.stmt.affected_rows);
	       f.stmt.affected_rows = (q_dbi_stmt_affected_rows_t)(*i).second;
	       break;
	    case QDBI_METHOD_STMT_GET_OUTPUT:
	       assert(!f.stmt.get_output);
	       f.stmt.get_output = (q_dbi_stmt_get_output_t)(*i).second;
	       break;
	    case QDBI_METHOD_STMT_GET_OUTPUT_ROWS:
	       assert(!f.stmt.get_output_rows);
	       f.stmt.get_output_rows = (q_dbi_stmt_get_output_rows_t)(*i).second;
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

      // ensure either no or minimum stmt methods are defined
      assert(!f.stmt.prepare || (f.stmt.prepare_raw && f.stmt.bind && f.stmt.bind_values
				 && f.stmt.exec && f.stmt.fetch_row && f.stmt.fetch_rows
				 && f.stmt.fetch_columns && f.stmt.next && f.stmt.close
				 && f.stmt.affected_rows && f.stmt.get_output
				 && f.stmt.get_output_rows && f.stmt.define));
   
      name = nme;
      caps = cps;
      if (f.stmt.prepare)
	 caps |= DBI_CAP_HAS_STATEMENT;
#ifdef DEBUG
      else
	 assert(!(caps & DBI_CAP_HAS_STATEMENT));
#endif
   }
};

DBIDriver::DBIDriver(const char *nme, const dbi_method_list_t &methods, int cps) : priv(new qore_dbi_private(nme, methods, cps)) {
}

DBIDriver::~DBIDriver() {
   delete priv;
}

const char *DBIDriver::getName() const {
   return priv->name;
}

int DBIDriver::getCaps() const {
   return priv->caps;
}

QoreListNode *DBIDriver::getCapList() const {
   QoreListNode *l = new QoreListNode;
   for (int i = 0; i < NUM_DBI_CAPS; i++)
      if (priv->caps & dbi_cap_list[i].cap)
	 l->push(new QoreStringNode(dbi_cap_list[i].desc));
   return l;
}

int DBIDriver::init(Datasource *ds, ExceptionSink *xsink) const {
   return priv->f.open(ds, xsink);
}

int DBIDriver::close(Datasource *ds) const {
   return priv->f.close(ds);
}

AbstractQoreNode *DBIDriver::select(Datasource *ds, const QoreString *sql, const QoreListNode *args, ExceptionSink *xsink) const {
   return priv->f.select(ds, sql, args, xsink);
}

AbstractQoreNode *DBIDriver::selectRows(Datasource *ds, const QoreString *sql, const QoreListNode *args, ExceptionSink *xsink) const {
   return priv->f.selectRows(ds, sql, args, xsink);
}

QoreHashNode *DBIDriver::selectRow(Datasource *ds, const QoreString *sql, const QoreListNode *args, ExceptionSink *xsink) const {
   if (priv->f.selectRow)
      return priv->f.selectRow(ds, sql, args, xsink);

   ReferenceHolder<AbstractQoreNode> res(priv->f.selectRows(ds, sql, args, xsink), xsink);
   if (!res)
      return 0;

   if (res->getType() != NT_LIST) {
      xsink->raiseException("DBI-SELECT-ROW-ERROR", "the call to selectRow() did not return a single row; type returned: %s", res->getTypeName());
      return 0;
   }

   QoreListNode *l = reinterpret_cast<QoreListNode *>(*res);
   if (l->size() > 1) {
      xsink->raiseException("DBI-SELECT-ROW-ERROR", "the call to selectRow() returned %lld rows; SQL passed to this method must return not more than 1 row", l->size());
      return 0;
   }

   AbstractQoreNode *rv = l->shift();
   assert(!rv || rv->getType() == NT_HASH);
   return reinterpret_cast<QoreHashNode *>(rv);
}

AbstractQoreNode *DBIDriver::execSQL(Datasource *ds, const QoreString *sql, const QoreListNode *args, ExceptionSink *xsink) const {
   return priv->f.execSQL(ds, sql, args, xsink);
}

AbstractQoreNode *DBIDriver::execRawSQL(Datasource *ds, const QoreString *sql, ExceptionSink *xsink) const {
   if (!priv->f.execRawSQL) {
      xsink->raiseException("DBI-EXEC-RAW-SQL-ERROR", "this driver does not implement the Datasource::execRawSQL() method");
      return 0;
   }
   return priv->f.execRawSQL(ds, sql, xsink);
}

int DBIDriver::commit(Datasource *ds, ExceptionSink *xsink) const {
   return priv->f.commit(ds, xsink);
}

int DBIDriver::rollback(Datasource *ds, ExceptionSink *xsink) const {
   return priv->f.rollback(ds, xsink);
}

int DBIDriver::beginTransaction(Datasource *ds, ExceptionSink *xsink) const {
   if (priv->f.begin_transaction)
      return priv->f.begin_transaction(ds, xsink);
   return 0; // 0 = OK
}

int DBIDriver::autoCommit(Datasource *ds, ExceptionSink *xsink) const {
   // if the driver does not require explicit "begin" statements to
   // start a transaction, then we have to explicitly call "commit" here
   if (!priv->f.begin_transaction)
      return priv->f.commit(ds, xsink);

   return 0; // 0 = OK
}

int DBIDriver::abortTransactionStart(Datasource *ds, ExceptionSink *xsink) const {
   if (priv->f.abort_transaction_start)
      return priv->f.abort_transaction_start(ds, xsink);
   return 0; // 0 = OK
}

AbstractQoreNode *DBIDriver::getServerVersion(Datasource *ds, ExceptionSink *xsink) const {
   if (priv->f.get_server_version)
      return priv->f.get_server_version(ds, xsink);
   return 0;
}

AbstractQoreNode *DBIDriver::getClientVersion(const Datasource *ds, ExceptionSink *xsink) const {
   if (priv->f.get_client_version)
      return priv->f.get_client_version(ds, xsink);
   return 0;
}

int DBIDriver::stmt_prepare(SQLStatement *stmt, const QoreString &str, const QoreListNode *args, ExceptionSink *xsink) const {
   return priv->f.stmt.prepare(stmt, str, args, xsink);
}

int DBIDriver::stmt_prepare_raw(SQLStatement *stmt, const QoreString &str, ExceptionSink *xsink) const {
   return priv->f.stmt.prepare_raw(stmt, str, xsink);
}

int DBIDriver::stmt_bind(SQLStatement *stmt, const QoreListNode &l, ExceptionSink *xsink) const {
   return priv->f.stmt.bind(stmt, l, xsink);
}

int DBIDriver::stmt_bind_placeholders(SQLStatement *stmt, const QoreListNode &l, ExceptionSink *xsink) const {
   if (!priv->f.stmt.bind_placeholders) {
      xsink->raiseException("SQLSTATEMENT-BIND-PLACEHOLDERS-ERROR", "the '%s' driver does not require placeholder buffer specifications so the SQLStatement::bindPlaceholders() method is not supported", getName());
      return -1;
   }

   return priv->f.stmt.bind_placeholders(stmt, l, xsink);
}

int DBIDriver::stmt_bind_values(SQLStatement *stmt, const QoreListNode &l, ExceptionSink *xsink) const {
   return priv->f.stmt.bind_values(stmt, l, xsink);
}

int DBIDriver::stmt_define(SQLStatement *stmt, ExceptionSink *xsink) const {
   return priv->f.stmt.define(stmt, xsink);
}

int DBIDriver::stmt_exec(SQLStatement *stmt, ExceptionSink *xsink) const {
   return priv->f.stmt.exec(stmt, xsink);
}

int DBIDriver::stmt_affected_rows(SQLStatement *stmt, ExceptionSink *xsink) const {
   return priv->f.stmt.affected_rows(stmt, xsink);
}

QoreHashNode *DBIDriver::stmt_get_output(SQLStatement *stmt, ExceptionSink *xsink) const {
   return priv->f.stmt.get_output(stmt, xsink);
}

QoreHashNode *DBIDriver::stmt_get_output_rows(SQLStatement *stmt, ExceptionSink *xsink) const {
   return priv->f.stmt.get_output_rows(stmt, xsink);
}

QoreHashNode *DBIDriver::stmt_fetch_row(SQLStatement *stmt, ExceptionSink *xsink) const {
   return priv->f.stmt.fetch_row(stmt, xsink);
}

QoreListNode *DBIDriver::stmt_fetch_rows(SQLStatement *stmt, int rows, ExceptionSink *xsink) const {
   return priv->f.stmt.fetch_rows(stmt, rows, xsink);
}

QoreHashNode *DBIDriver::stmt_fetch_columns(SQLStatement *stmt, int rows, ExceptionSink *xsink) const {
   return priv->f.stmt.fetch_columns(stmt, rows, xsink);
}

bool DBIDriver::stmt_next(SQLStatement *stmt, ExceptionSink *xsink) const {
   return priv->f.stmt.next(stmt, xsink);
}

int DBIDriver::stmt_close(SQLStatement *stmt, ExceptionSink *xsink) const {
   return priv->f.stmt.close(stmt, xsink);
}

bool DBIDriver::hasStatementAPI() const {
   return priv->caps & DBI_CAP_HAS_STATEMENT;
}

/* it's not necessary to lock this object because it will only be written to in one thread at a time
   (within the module lock)
   note that a safe_dslist is used because it can be safely read in multiple threads while
   being written to (in the lock).  The list should never be that long so the penalty for searching
   a linked list with strcmp() against using a hash with explicit locking around all searches
   should be acceptable...
*/
struct qore_dbi_dlist_private {
   dbi_list_t l;

   DLLLOCAL ~qore_dbi_dlist_private() {
      dbi_list_t::iterator i;
      while ((i = l.begin()) != l.end()) {
	 DBIDriver *driv = *i;
	 l.erase(i);
	 delete driv;
      }
   }

   DLLLOCAL DBIDriver *find_intern(const char *name) const {
      for (dbi_list_t::const_iterator i = l.begin(); i != l.end(); i++)
	 if (!strcmp(name, (*i)->getName()))
	    return *i;
	 
      return 0;
   }

   DLLLOCAL QoreListNode *getDriverList() const {
      if (l.empty())
	 return 0;
	 
      QoreListNode *lst = new QoreListNode;
	 
      for (dbi_list_t::const_iterator i = l.begin(); i != l.end(); i++)
	 lst->push(new QoreStringNode((*i)->getName()));
	 
      return lst;
   }
};

DBIDriverList::DBIDriverList() : priv(new qore_dbi_dlist_private) {
}

DBIDriverList::~DBIDriverList() {
   delete priv;
}

DBIDriver *DBIDriverList::find_intern(const char *name) const {
   return priv->find_intern(name);
}

DBIDriver *DBIDriverList::find(const char *name) const {
   DBIDriver *d = priv->find_intern(name);
   if (d)
      return d;

   // to to load the driver if it doesn't exist
   // ignore any exceptions
   ExceptionSink xs;
   MM.runTimeLoadModule(name, &xs);
   xs.clear();

   return priv->find_intern(name);
}

DBIDriver *DBIDriverList::find(const char *name, ExceptionSink *xsink) const {
   DBIDriver *d = priv->find_intern(name);
   if (d)
      return d;

   // to to load the driver if it doesn't exist
   if (MM.runTimeLoadModule(name, xsink))
      return 0;

   return priv->find_intern(name);
}

DBIDriver *DBIDriverList::registerDriver(const char *name, const struct qore_dbi_method_list &methods, int caps) {
   assert(!priv->find_intern(name));
   
   DBIDriver *dd = new DBIDriver(name, *(methods.getMethods()), caps);
   priv->l.push_back(dd);
   return dd;
}

QoreListNode *DBIDriverList::getDriverList() const {
   return priv->getDriverList();
}

void DBI_concat_numeric(QoreString *str, const AbstractQoreNode *v) {
   if (is_nothing(v) || is_null(v)) {
      str->concat("null");
      return;
   }

   if (v->getType() == NT_FLOAT || (v->getType() == NT_STRING && strchr((reinterpret_cast<const QoreStringNode *>(v))->getBuffer(), '.'))) {
      str->sprintf("%g", v->getAsFloat());
      return;
   }
   str->sprintf("%lld", v->getAsBigInt());
}

int DBI_concat_string(QoreString *str, const AbstractQoreNode *v, ExceptionSink *xsink) {
   if (is_nothing(v) || is_null(v))
      return 0;

   QoreStringValueHelper tstr(v, str->getEncoding(), xsink);
   if (*xsink)
      return -1;

   str->concat(*tstr, xsink);
   return *xsink;
}

/*
  parseDatasource()
  parses strings of the form: driver:user/pass@db(encoding)%host:post
  driver, encoding, host, and port are optional
*/
QoreHashNode *parseDatasource(const char *ds, ExceptionSink *xsink) {
   static const char *DATASOURCE_PARSE_ERROR = "DATASOURCE-PARSE-ERROR";

   if (!ds || !ds[0]) {
      xsink->raiseException(DATASOURCE_PARSE_ERROR, "empty text passed to parseDatasource()");
      return 0;
   }

   // use a QoreString to create a temporary buffer
   QoreString tmp(ds);
   tmp.trim();
   char *str = const_cast<char *>(tmp.getBuffer());

   ReferenceHolder<QoreHashNode> h(new QoreHashNode, xsink);
   char *p = strchr(str, ':');
   if (p) {
      *p = '\0';
      h->setKeyValue("type", new QoreStringNode(str), 0);
      str = p + 1;
   }

   bool has_pass = false;
   p = strchr(str, '/');
   if (p) {
      *p = '\0';
      if (*str)
	 h->setKeyValue("user", new QoreStringNode(str), 0);
      str = p + 1;
      has_pass = true;
   }

   // take last '@' sign in string in case there's one in the password
   p = strrchr(str, '@');
   if (!p) {
      xsink->raiseException(DATASOURCE_PARSE_ERROR, "missing database name delimited by '@' in '%s'", ds);
      return 0;
   }

   *p = '\0';
   if (p != str) {
      if (has_pass)
	 h->setKeyValue("pass", new QoreStringNode(str), 0);
      else
	 h->setKeyValue("user", new QoreStringNode(str), 0);
   }
   str = p + 1;

   char *db = str;
   p = strchr(str, '(');
   if (p) {
      char *end = strchr(p, ')');
      if (!end) {
	 xsink->raiseException(DATASOURCE_PARSE_ERROR, "missing closing parenthesis in charset specification in '%s'", ds);
	 return 0;
      }
      *p = '\0';  // terminate for db
      *end = '\0';  // terminate charset
      p++;
      h->setKeyValue("charset", new QoreStringNode(p), 0);
      str = end + 1;
   }

   // get dbname
   p = strchrs(str, "%:{");
   if (!p)
      h->setKeyValue("db", new QoreStringNode(db), 0);
   else {
      char tok = *p;
      *p = '\0';

      h->setKeyValue("db", new QoreStringNode(db), 0);
      str = p + 1;

      if (tok == '%') {	 
	 p = strchrs(str, ":{");

	 if ((p == str) || !*str) {
	    xsink->raiseException(DATASOURCE_PARSE_ERROR, "missing hostname string after '%%' delimeter in '%s'", ds);
	    return 0;
	 }

	 const char *hstr = str;

	 if (p) {
	    tok = *p;
	    *p = '\0';
	    str = p + 1;
	 }
	 else
	    str += strlen(str);

	 h->setKeyValue("host", new QoreStringNode(hstr), 0);
      }

      if (tok == ':') {
	 int port = atoi(str);
	 if (!port) {
	    xsink->raiseException(DATASOURCE_PARSE_ERROR, "invalid port number in datasource string");
	    return 0;
	 }
	 h->setKeyValue("port", new QoreBigIntNode(port), 0);

	 const char *pstr = str;
	 p = strchr(str, '{');
	 if (p) {
	    tok = *p;
	    *p = '\0';
	    str = p + 1;
	 }
	 else
	    str += strlen(str);
	 
	 while (isdigit(*pstr))
	    ++pstr;
	 
	 if (*pstr) {
	    xsink->raiseException(DATASOURCE_PARSE_ERROR, "invalid characters present in port number in '%s'", ds);
	    return 0;
	 }
      }

      if (tok == '{') {
	 char *end = strchr(str, '}');
	 if (!end) {
	    xsink->raiseException(DATASOURCE_PARSE_ERROR, "missing closing curly bracket '}' in option specification in '%s'", ds);
	    return 0;
	 }
	 *end = '\0';
	 p = str;
	 str = end + 1;

	 // parse option hash
	 ReferenceHolder<QoreHashNode> opt(new QoreHashNode, xsink);

	 while (true) {
	    if (!*p)
	       break;
	    char *eq = strchr(p, '=');
	    if (!eq) {
	       xsink->raiseException(DATASOURCE_PARSE_ERROR, "missing '=' in option specification in '%s'", ds);
	       return 0;
	    }
	    if (eq == p) {
	       xsink->raiseException(DATASOURCE_PARSE_ERROR, "missing value before '=' in option specification in '%s'", ds);
	       return 0;
	    }
	    *eq = '\0';
	    ++eq;
	    char *oend = strchr(eq, ',');
	    qore_size_t len = oend ? oend - eq : strlen(eq);
	    if (opt->existsKey(p)) {
	       xsink->raiseException(DATASOURCE_PARSE_ERROR, "option '%s' repeated in '%s'", p, ds);
	       return 0;
	    }
	    
	    QoreString key(p);
	    key.trim();
	    
	    QoreString value(eq, len);
	    value.trim();
	    
	    opt->setKeyValue(key.getBuffer(), new QoreStringNode(value), 0);
	    
	    p = eq + len;
	    if (oend)
	       ++p;
	 }

	 h->setKeyValue("options", opt.release(), 0);
      }

      if (*str) {
	 xsink->raiseException(DATASOURCE_PARSE_ERROR, "unrecognized characters at end of datasource definition in '%s'", ds);
	 return 0;
      }
   }

   return h.release();
}

AbstractQoreNode *f_parseDatasource(const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(params, 0);
   return parseDatasource(p0->getBuffer(), xsink);
}

// *list getDBIDriverList()  
AbstractQoreNode *f_getDBIDriverList(const QoreListNode *params, ExceptionSink *xsink) {
   return DBI.getDriverList();
}

// *list getDBIDriverCapabilityList(string $driver)  
AbstractQoreNode *f_getDBIDriverCapabilityList(const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(params, 0);

   DBIDriver *dd = DBI.find(p0->getBuffer());
   return !dd ? 0 : dd->getCapList();
}

// *int getDBIDriverCapabilities(string $driver)  
AbstractQoreNode *f_getDBIDriverCapabilities(const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(params, 0);

   DBIDriver *dd = DBI.find(p0->getBuffer());
   return !dd ? 0 : new QoreBigIntNode(dd->getCaps());
}

void init_dbi_functions() {
   // *list getDBIDriverList()  
   builtinFunctions.add2("getDBIDriverList", f_getDBIDriverList, QC_RET_VALUE_ONLY, QDOM_DEFAULT, listOrNothingTypeInfo);

   builtinFunctions.add2("getDBIDriverCapabilityList", f_noop, QC_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   // *list getDBIDriverCapabilityList(string $driver)  
   builtinFunctions.add2("getDBIDriverCapabilityList", f_getDBIDriverCapabilityList, QC_RET_VALUE_ONLY, QDOM_DEFAULT, listOrNothingTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("getDBIDriverCapabilities", f_noop, QC_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   // *int getDBIDriverCapabilities(string $driver)  
   builtinFunctions.add2("getDBIDriverCapabilities", f_getDBIDriverCapabilities, QC_RET_VALUE_ONLY, QDOM_DEFAULT, bigIntOrNothingTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("parseDatasource", f_noop, QC_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("parseDatasource", f_parseDatasource, QC_RET_VALUE_ONLY, QDOM_DEFAULT, hashTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
}

QoreNamespace *getSQLNamespace() {
   // create Qore::SQL namespace
   QoreNamespace *SQLNS = new QoreNamespace("SQL");

   QoreClass *ds, *dsp;
   SQLNS->addSystemClass((ds = initDatasourceClass()));
   SQLNS->addSystemClass((dsp = initDatasourcePoolClass()));
   SQLNS->addSystemClass(initSQLStatementClass(ds, dsp));

   // datasource type/driver constants
   SQLNS->addConstant("DSOracle",   new QoreStringNode("oracle"));
   SQLNS->addConstant("DSMySQL",    new QoreStringNode("mysql"));
   SQLNS->addConstant("DSSybase",   new QoreStringNode("sybase"));
   SQLNS->addConstant("DSPGSQL",    new QoreStringNode("pgsql"));
   SQLNS->addConstant("DSMSSQL",    new QoreStringNode("freetds"));
   SQLNS->addConstant("DSFreeTDS",  new QoreStringNode("freetds"));
   SQLNS->addConstant("DSSQLite3",  new QoreStringNode("sqlite3"));
   SQLNS->addConstant("DSDB2",      new QoreStringNode("db2"));
   // the following have no drivers yet
   SQLNS->addConstant("DSInformix", new QoreStringNode("informix"));
   SQLNS->addConstant("DSTimesTen", new QoreStringNode("timesten"));

   // for DBI driver capabilities
   SQLNS->addConstant("DBI_CAP_TIME_ZONE_SUPPORT",      new QoreBigIntNode(DBI_CAP_TIME_ZONE_SUPPORT));
   SQLNS->addConstant("DBI_CAP_CHARSET_SUPPORT",        new QoreBigIntNode(DBI_CAP_CHARSET_SUPPORT));
   SQLNS->addConstant("DBI_CAP_TRANSACTION_MANAGEMENT", new QoreBigIntNode(DBI_CAP_TRANSACTION_MANAGEMENT));
   SQLNS->addConstant("DBI_CAP_STORED_PROCEDURES",      new QoreBigIntNode(DBI_CAP_STORED_PROCEDURES));
   SQLNS->addConstant("DBI_CAP_LOB_SUPPORT",            new QoreBigIntNode(DBI_CAP_LOB_SUPPORT));
   SQLNS->addConstant("DBI_CAP_BIND_BY_VALUE",          new QoreBigIntNode(DBI_CAP_BIND_BY_VALUE));
   SQLNS->addConstant("DBI_CAP_BIND_BY_PLACEHOLDER",    new QoreBigIntNode(DBI_CAP_BIND_BY_PLACEHOLDER));
   SQLNS->addConstant("DBI_CAP_HAS_EXECRAW",            new QoreBigIntNode(DBI_CAP_HAS_EXECRAW));

   // for column types for binding
   SQLNS->addConstant("VARCHAR",  new QoreStringNode("string"));
   SQLNS->addConstant("NUMBER",   new QoreStringNode("string"));
   SQLNS->addConstant("NUMERIC",  new QoreStringNode("string"));
   SQLNS->addConstant("DECIMAL",  new QoreStringNode("string"));
   SQLNS->addConstant("CLOB",     new QoreStringNode("clob"));
   SQLNS->addConstant("BLOB",     new QoreStringNode("blob"));
   SQLNS->addConstant("DATE",     new QoreStringNode("date"));

   return SQLNS;
}
