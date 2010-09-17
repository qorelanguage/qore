/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreSQLStatement.h

  Qore Programming Language

  Copyright (C) 2006 - 2010 Qore Technologies, sro
  
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
#include <qore/intern/QC_SQLStatement.h>
#include <qore/intern/DatasourceStatementHelper.h>
#include <qore/intern/sql_statement_private.h>

const char *QoreSQLStatement::stmt_statuses[] = { "idle", "prepared", "defined", "executed", "deleted" };

struct DBActionHelper {
   QoreSQLStatement &stmt;
   bool valid;
   char cmd;
   bool first;

   DLLLOCAL DBActionHelper(QoreSQLStatement &n_stmt, ExceptionSink *xsink, char n_cmd = DAH_NONE, bool ignore_new_transaction = false) : stmt(n_stmt), valid(false), cmd(n_cmd), first(false) {
      bool nt = false;
      stmt.priv->ds = stmt.dsh->helperStartAction(xsink, cmd, &nt);

      if (cmd == DAH_ACQUIRE && stmt.trans_status == STMT_TRANS_UNKNOWN) {
         stmt.trans_status = nt ? STMT_TRANS_NEW : STMT_TRANS_EXISTED;
         // set first time acquisition status
         first = true;
      }

      valid = *xsink ? false : true;
   }

   DLLLOCAL ~DBActionHelper() {
      if (valid) {
         bool nt = stmt.trans_status == STMT_TRANS_NEW;

         if (stmt.priv->ds->wasConnectionAborted()) {
            cmd = DAH_RELEASE;
            // FIXME: do something else here?
         }
         if (cmd == DAH_RELEASE)
            stmt.trans_status = STMT_TRANS_UNKNOWN;

         stmt.priv->ds = stmt.dsh->helperEndAction(cmd, nt);
         //printd(0, "DBActionHelper::~DBActionHelper() ds=%p\n", stmt.priv->ds);
      }
   }

   DLLLOCAL operator bool() const {
      return valid;
   }

   // release the datasource in the destructor if an error occurs when the datasource is initially acquired
   DLLLOCAL void error() {
      if (cmd == DAH_ACQUIRE && first)
         cmd = DAH_RELEASE;
   }
};

QoreSQLStatement::~QoreSQLStatement() {
   assert(!priv->data);
}

void QoreSQLStatement::init(DatasourceStatementHelper *n_dsh) {
   dsh = n_dsh;
}

void QoreSQLStatement::deref(ExceptionSink *xsink) {
   if (ROdereference()) {
      {
         DBActionHelper dba(*this, xsink, DAH_RELEASE);
         if (dba)
            closeIntern(xsink);
      }

      dsh->helperDestructor(this, xsink);
      delete this;
   }
}

int QoreSQLStatement::closeIntern(ExceptionSink *xsink) {
   if (!priv->data)
      return 0;

   assert(priv->ds);

   int rc = priv->ds->getDriver()->stmt_close(this, xsink);
   assert(!priv->data);

   return rc;
}

int QoreSQLStatement::close(ExceptionSink *xsink) {
   DBActionHelper dba(*this, xsink, trans_status == STMT_TRANS_NEW ? DAH_RELEASE : DAH_NONE);
   if (!dba)
      return -1;

   return closeIntern(xsink);
}

int QoreSQLStatement::prepare(const QoreString &str, const QoreListNode *args, ExceptionSink *xsink) {
   DBActionHelper dba(*this, xsink);
   if (!dba)
      return -1;

   if (checkStatus(dba, STMT_IDLE, "prepare", xsink))
      return -1;

   int rc = priv->ds->getDriver()->stmt_prepare(this, str, args, xsink);
   if (!rc)
      status = STMT_PREPARED;
   return rc;
}

int QoreSQLStatement::prepareRaw(const QoreString &str, ExceptionSink *xsink) {
   DBActionHelper dba(*this, xsink);
   if (!dba)
      return -1;

   if (checkStatus(dba, STMT_IDLE, "prepareRaw", xsink))
      return -1;

   int rc = priv->ds->getDriver()->stmt_prepare_raw(this, str, xsink);
   if (!rc)
      status = STMT_PREPARED;
   return rc;
}

int QoreSQLStatement::bind(const QoreListNode &l, ExceptionSink *xsink) {
   DBActionHelper dba(*this, xsink);
   if (!dba)
      return -1;

   if (checkStatus(dba, STMT_PREPARED, "bind", xsink))
      return -1;

   return priv->ds->getDriver()->stmt_bind(this, l, xsink);
}

int QoreSQLStatement::bindPlaceholders(const QoreListNode &l, ExceptionSink *xsink) {
   DBActionHelper dba(*this, xsink);
   if (!dba)
      return -1;

   if (checkStatus(dba, STMT_PREPARED, "bindPlaceholders", xsink))
      return -1;

   return priv->ds->getDriver()->stmt_bind_placeholders(this, l, xsink);
}

int QoreSQLStatement::bindValues(const QoreListNode &l, ExceptionSink *xsink) {
   DBActionHelper dba(*this, xsink);
   if (!dba)
      return -1;

   if (checkStatus(dba, STMT_PREPARED, "bindValues", xsink))
      return -1;

   return priv->ds->getDriver()->stmt_bind_values(this, l, xsink);
}

int QoreSQLStatement::exec(ExceptionSink *xsink) {
   DBActionHelper dba(*this, xsink, DAH_ACQUIRE);
   if (!dba)
      return -1;

   if (checkStatus(dba, STMT_PREPARED, "exec", xsink))
      return -1;

   return execIntern(dba, xsink);
}

int QoreSQLStatement::execIntern(DBActionHelper &dba, ExceptionSink *xsink) {
   int rc = priv->ds->getDriver()->stmt_exec(this, xsink);
   if (!rc)
      status = STMT_EXECED;
   else
      dba.error();
   return rc;
}

int QoreSQLStatement::affectedRows(ExceptionSink *xsink) {
   DBActionHelper dba(*this, xsink, DAH_ACQUIRE);
   if (!dba)
      return -1;

   if (checkStatus(dba, STMT_EXECED, "affectedRows", xsink))
      return -1;

   return priv->ds->getDriver()->stmt_affected_rows(this, xsink);
}

QoreHashNode *QoreSQLStatement::getOutput(ExceptionSink *xsink) {
   DBActionHelper dba(*this, xsink, DAH_ACQUIRE);
   if (!dba)
      return 0;

   if (checkStatus(dba, STMT_EXECED, "getOutput", xsink))
      return 0;

   return priv->ds->getDriver()->stmt_get_output(this, xsink);
}

QoreHashNode *QoreSQLStatement::getOutputRows(ExceptionSink *xsink) {
   DBActionHelper dba(*this, xsink, DAH_ACQUIRE);
   if (!dba)
      return 0;

   if (checkStatus(dba, STMT_EXECED, "getOutputRows", xsink))
      return 0;

   return priv->ds->getDriver()->stmt_get_output_rows(this, xsink);
}

bool QoreSQLStatement::next(ExceptionSink *xsink) {
   DBActionHelper dba(*this, xsink, DAH_ACQUIRE);
   if (!dba)
      return false;

   if (checkStatus(dba, STMT_DEFINED, "next", xsink))
      return false;

   return priv->ds->getDriver()->stmt_next(this, xsink);
}

int QoreSQLStatement::define(ExceptionSink *xsink) {
   DBActionHelper dba(*this, xsink, DAH_ACQUIRE);
   if (!dba)
      return false;

   if (checkStatus(dba, STMT_EXECED, "define", xsink))
      return false;

   return defineIntern(xsink);
}

int QoreSQLStatement::defineIntern(ExceptionSink *xsink) {
   int rc = priv->ds->getDriver()->stmt_define(this, xsink);
   if (!rc)
      status = STMT_DEFINED;
   return rc;
}

QoreHashNode *QoreSQLStatement::fetchRow(ExceptionSink *xsink) {
   DBActionHelper dba(*this, xsink, DAH_ACQUIRE);
   if (!dba)
      return 0;

   if (checkStatus(dba, STMT_DEFINED, "fetchRow", xsink))
      return 0;

   return priv->ds->getDriver()->stmt_fetch_row(this, xsink);
}

QoreListNode *QoreSQLStatement::fetchRows(int rows, ExceptionSink *xsink) {
   DBActionHelper dba(*this, xsink, DAH_ACQUIRE);
   if (!dba)
      return 0;

   if (checkStatus(dba, STMT_DEFINED, "fetchRows", xsink))
      return 0;

   return priv->ds->getDriver()->stmt_fetch_rows(this, rows, xsink);
}

QoreHashNode *QoreSQLStatement::fetchColumns(int rows, ExceptionSink *xsink) {
   DBActionHelper dba(*this, xsink, DAH_ACQUIRE);
   if (!dba)
      return 0;

   if (checkStatus(dba, STMT_DEFINED, "fetchColumns", xsink))
      return 0;

   return priv->ds->getDriver()->stmt_fetch_columns(this, rows, xsink);
}

bool QoreSQLStatement::active() const {
   return status != STMT_IDLE;
}
