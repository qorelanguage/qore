/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreSQLStatement.h

  Qore Programming Language

  Copyright (C) 2006 - 2017 Qore Technologies, s.r.o.

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

#include <qore/Qore.h>
#include <qore/intern/QC_SQLStatement.h>
#include <qore/intern/DatasourceStatementHelper.h>
#include <qore/intern/sql_statement_private.h>
#include <qore/intern/qore_ds_private.h>
#include <qore/intern/qore_dbi_private.h>

const char* QoreSQLStatement::stmt_statuses[] = { "idle", "prepared", "executed", "defined" };

class DBActionHelper {
public:
   QoreSQLStatement& stmt;
   ExceptionSink* xsink;
   bool valid;
   char cmd;
   bool nt; // new transaction flag

   DLLLOCAL DBActionHelper(QoreSQLStatement& n_stmt, ExceptionSink* n_xsink, char n_cmd = DAH_NOCHANGE) : stmt(n_stmt), xsink(n_xsink), valid(false), cmd(n_cmd), nt(false) {
      Datasource* oldds = stmt.priv->ds;
      //printd(5, "DBActionHelper::DBActionHelper() old ds: %p cmd: %s\n", stmt.priv->ds, DAH_TEXT(cmd));
      stmt.priv->ds = stmt.dsh->helperStartAction(xsink, nt);

      //printd(5, "DBActionHelper::DBActionHelper() ds: %p cmd: %s nt: %d\n", stmt.priv->ds, DAH_TEXT(cmd), nt);

      // issue #1835: the helperStartAction() call can fail in case of a timeout, therefore we have to check below
      // that the call succeeded before raising an additional exception

      // if we are trying to start a new transaction on a new Datasource connection, but we have
      // already prepared the statement on another connection, we have to raise an exception
      // otherwise the statement will actually be executed on the original connection which is
      // cached in the driver (https://github.com/qorelanguage/qore/issues/465)
      if (oldds && stmt.priv->ds && stmt.priv->ds != oldds && cmd == DAH_ACQUIRE) {
         // issue #1836 we release the connection before marking the object as invalid
         stmt.priv->ds = stmt.dsh->helperEndAction(DAH_RELEASE, nt, xsink);
         xsink->raiseException("SQLSTATEMENT-ERROR", "statement was prepared on another Datasource; you must close the statement before executing this action on a new connection");
      }

      // ensure we either have a datasource or an exception
      assert((*xsink && !stmt.priv->ds) || (!*xsink && stmt.priv->ds));

      valid = *xsink ? false : true;
   }

   DLLLOCAL ~DBActionHelper() {
      if (!valid)
         return;

      /* release the Datasource if:
         1) the connection was lost (exception already raised)
         2) the Datasource was acquired for this call, and
              the command was NOCHANGE, meaning, leave the Datasource in the same state it was before the call
       */
      if (stmt.priv->ds->wasConnectionAborted() || (nt && (cmd == DAH_NOCHANGE)))
         cmd = DAH_RELEASE;

      //printd(5, "DBActionHelper::~DBActionHelper() ds: %p cmd: %s nt: %d xsink: %d\n", stmt.priv->ds, DAH_TEXT(cmd), nt, xsink->isEvent());

      // call end action with the command
      stmt.priv->ds = stmt.dsh->helperEndAction(cmd, nt, xsink);
   }

   DLLLOCAL operator bool() const {
      return valid;
   }

   DLLLOCAL bool inTransaction() const {
      return valid && !nt;
   }
};

QoreSQLStatement::~QoreSQLStatement() {
   assert(!priv->data);
}

void QoreSQLStatement::init(DatasourceStatementHelper* n_dsh) {
   dsh = n_dsh;
}

int QoreSQLStatement::checkStatus(ExceptionSink* xsink, DBActionHelper& dba, int stat, const char* action) {
   //printd(5, "QoreSQLStatement::checkStatus() this: %p stat: %d status: %d action: '%s' ssize: %d\n", this, stat, status, action, ssize);

   if (stat != status) {
      if (stat == STMT_IDLE)
         return closeIntern(xsink);

      if (stat > STMT_IDLE && status == STMT_IDLE && str.strlen()) {
         if (prepareIntern(xsink))
            return -1;

         if (stat == status)
            return 0;
      }

      if (stat == STMT_PREPARED && status == STMT_EXECED)
         return 0;

      if (stat == STMT_PREPARED && status == STMT_DEFINED) {
         if (closeIntern(xsink))
            return -1;
         return prepareIntern(xsink);
      }

      if ((stat == STMT_EXECED || stat == STMT_DEFINED) && status == STMT_PREPARED) {
         if (execIntern(dba, xsink))
            return -1;

         if (stat == status)
            return 0;
      }

      if (stat == STMT_DEFINED && status == STMT_EXECED)
         return defineIntern(xsink);

      xsink->raiseException("SQLSTATEMENT-ERROR", "SQLStatement::%s() called expecting status '%s', but statement has status '%s'", action, stmt_statuses[stat], stmt_statuses[status]);
      return -1;
   }

   return 0;
}

void QoreSQLStatement::deref(ExceptionSink* xsink) {
   if (ROdereference()) {
      char cmd = DAH_NOCHANGE;
      //printd(5, "QoreSQLStatement::deref() deleting this=%p cmd=%s\n", this, DAH_TEXT(cmd));
      {
         DBActionHelper dba(*this, xsink, cmd);
         if (dba)
            closeIntern(xsink);
      }

      dsh->helperDestructor(this, xsink);

      if (prepare_args)
         prepare_args->deref(xsink);

      delete this;
   }
}

int QoreSQLStatement::closeIntern(ExceptionSink* xsink) {
   if (!priv->data)
      return 0;

   assert(priv->ds);
   int rc = qore_dbi_private::get(*priv->ds->getDriver())->stmt_close(this, xsink);
   assert(!priv->data);
   status = STMT_IDLE;

   return rc;
}

int QoreSQLStatement::prepareArgs(bool n_raw, const QoreString& n_str, const QoreListNode* args, ExceptionSink* xsink) {
   raw = n_raw;
   str = n_str;

   if (prepare_args) {
      prepare_args->deref(xsink);
      if (*xsink) {
         prepare_args = 0;
         return -1;
      }
   }

   prepare_args = args ? args->listRefSelf() : 0;
   return 0;
}

int QoreSQLStatement::prepareIntern(ExceptionSink* xsink) {
   //assert(!stmtds);
   int rc = qore_dbi_private::get(*priv->ds->getDriver())->stmt_prepare(this, str, prepare_args, xsink);
   if (!rc) {
      status = STMT_PREPARED;
      //stmtds = ds;
   }
   else
      closeIntern(xsink);
   return rc;
}

int QoreSQLStatement::prepare(const QoreString& n_str, const QoreListNode* args, ExceptionSink* xsink) {
   DBActionHelper dba(*this, xsink);
   if (!dba)
      return -1;

   if (checkStatus(xsink, dba, STMT_IDLE, "prepare"))
      return -1;

   if (prepareArgs(false, n_str, args, xsink))
      return -1;

   return 0;
}

int QoreSQLStatement::prepareRaw(const QoreString& n_str, ExceptionSink* xsink) {
   DBActionHelper dba(*this, xsink);
   if (!dba)
      return -1;

   if (checkStatus(xsink, dba, STMT_IDLE, "prepareRaw"))
      return -1;

   if (prepareArgs(true, n_str, 0, xsink))
      return -1;

   return 0;
}

int QoreSQLStatement::bind(const QoreListNode& l, ExceptionSink* xsink) {
   DBActionHelper dba(*this, xsink, DAH_ACQUIRE);
   if (!dba)
      return -1;

   if (checkStatus(xsink, dba, STMT_PREPARED, "bind"))
      return -1;

   return qore_dbi_private::get(*priv->ds->getDriver())->stmt_bind(this, l, xsink);
}

int QoreSQLStatement::bindPlaceholders(const QoreListNode& l, ExceptionSink* xsink) {
   DBActionHelper dba(*this, xsink, DAH_ACQUIRE);
   if (!dba)
      return -1;

   if (checkStatus(xsink, dba, STMT_PREPARED, "bindPlaceholders"))
      return -1;

   return qore_dbi_private::get(*priv->ds->getDriver())->stmt_bind_placeholders(this, l, xsink);
}

int QoreSQLStatement::bindValues(const QoreListNode& l, ExceptionSink* xsink) {
   DBActionHelper dba(*this, xsink, DAH_ACQUIRE);
   if (!dba)
      return -1;

   if (checkStatus(xsink, dba, STMT_PREPARED, "bindValues"))
      return -1;

   return qore_dbi_private::get(*priv->ds->getDriver())->stmt_bind_values(this, l, xsink);
}

int QoreSQLStatement::exec(const QoreListNode* args, ExceptionSink* xsink) {
   DBActionHelper dba(*this, xsink, DAH_ACQUIRE);
   if (!dba)
      return -1;

   if (checkStatus(xsink, dba, STMT_PREPARED, "exec"))
      return -1;

   if (args && args->size() && qore_dbi_private::get(*priv->ds->getDriver())->stmt_bind(this, *args, xsink))
      return -1;

   return execIntern(dba, xsink);
}

int QoreSQLStatement::execIntern(DBActionHelper& dba, ExceptionSink* xsink) {
   int rc = qore_dbi_private::get(*priv->ds->getDriver())->stmt_exec(this, xsink);
   if (!rc)
      status = STMT_EXECED;

   //printd(5, "QoreSQLStatement::execIntern() this: %p ds: %p: %s@%s: %s\n", this, priv->ds, priv->ds->getUsername(), priv->ds->getDBName(), str.getBuffer());

   priv->ds->priv->statementExecuted(rc, xsink);
   return rc;
}

int QoreSQLStatement::affectedRows(ExceptionSink* xsink) {
   DBActionHelper dba(*this, xsink, DAH_ACQUIRE);
   if (!dba)
      return -1;

   if (checkStatus(xsink, dba, STMT_EXECED, "affectedRows"))
      return -1;

   return qore_dbi_private::get(*priv->ds->getDriver())->stmt_affected_rows(this, xsink);
}

QoreHashNode* QoreSQLStatement::getOutput(ExceptionSink* xsink) {
   DBActionHelper dba(*this, xsink, DAH_ACQUIRE);
   if (!dba)
      return 0;

   if (checkStatus(xsink, dba, STMT_EXECED, "getOutput"))
      return 0;

   return qore_dbi_private::get(*priv->ds->getDriver())->stmt_get_output(this, xsink);
}

QoreHashNode* QoreSQLStatement::getOutputRows(ExceptionSink* xsink) {
   DBActionHelper dba(*this, xsink, DAH_ACQUIRE);
   if (!dba)
      return 0;

   if (checkStatus(xsink, dba, STMT_EXECED, "getOutputRows"))
      return 0;

   return qore_dbi_private::get(*priv->ds->getDriver())->stmt_get_output_rows(this, xsink);
}

bool QoreSQLStatement::next(ExceptionSink* xsink) {
   DBActionHelper dba(*this, xsink, DAH_ACQUIRE);
   if (!dba)
      return (validp = false);

   if (checkStatus(xsink, dba, STMT_DEFINED, "next"))
      return (validp = false);

   return (validp = qore_dbi_private::get(*priv->ds->getDriver())->stmt_next(this, xsink));
}

bool QoreSQLStatement::valid() {
   return validp;
}

int QoreSQLStatement::define(ExceptionSink* xsink) {
   DBActionHelper dba(*this, xsink, DAH_ACQUIRE);
   if (!dba)
      return false;

   if (checkStatus(xsink, dba, STMT_EXECED, "define"))
      return false;

   return defineIntern(xsink);
}

int QoreSQLStatement::defineIntern(ExceptionSink* xsink) {
   int rc = qore_dbi_private::get(*priv->ds->getDriver())->stmt_define(this, xsink);
   if (!rc)
      status = STMT_DEFINED;
   return rc;
}

QoreHashNode* QoreSQLStatement::fetchRow(ExceptionSink* xsink) {
   DBActionHelper dba(*this, xsink, DAH_ACQUIRE);
   if (!dba)
      return 0;

   if (checkStatus(xsink, dba, STMT_DEFINED, "fetchRow"))
      return 0;

   return qore_dbi_private::get(*priv->ds->getDriver())->stmt_fetch_row(this, xsink);
}

QoreListNode* QoreSQLStatement::fetchRows(int rows, ExceptionSink* xsink) {
   DBActionHelper dba(*this, xsink, DAH_ACQUIRE);
   if (!dba)
      return 0;

   if (checkStatus(xsink, dba, STMT_DEFINED, "fetchRows"))
      return 0;

   return qore_dbi_private::get(*priv->ds->getDriver())->stmt_fetch_rows(this, rows, xsink);
}

QoreHashNode* QoreSQLStatement::fetchColumns(int rows, ExceptionSink* xsink) {
   DBActionHelper dba(*this, xsink, DAH_ACQUIRE);
   if (!dba)
      return 0;

   if (checkStatus(xsink, dba, STMT_DEFINED, "fetchColumns"))
      return 0;

   return qore_dbi_private::get(*priv->ds->getDriver())->stmt_fetch_columns(this, rows, xsink);
}

QoreHashNode* QoreSQLStatement::describe(ExceptionSink* xsink) {
    DBActionHelper dba(*this, xsink, DAH_ACQUIRE);
    if (!dba)
       return 0;

    if (checkStatus(xsink, dba, STMT_DEFINED, "describe"))
       return 0;

    return qore_dbi_private::get(*priv->ds->getDriver())->stmt_describe(this, xsink);
}

bool QoreSQLStatement::active() const {
   return status != STMT_IDLE;
}

bool QoreSQLStatement::currentThreadInTransaction(ExceptionSink* xsink) {
    DBActionHelper dba(*this, xsink, DAH_NOCHANGE);
    if (!dba)
       return false;

    return dba.inTransaction();
}

int QoreSQLStatement::close(ExceptionSink* xsink) {
   DBActionHelper dba(*this, xsink, DAH_NOCHANGE);
   if (!dba)
      return -1;

   return closeIntern(xsink);
}

int QoreSQLStatement::commit(ExceptionSink* xsink) {
   DBActionHelper dba(*this, xsink, DAH_RELEASE);
   if (!dba)
      return -1;

   int rc = closeIntern(xsink);
   rc = priv->ds->commit(xsink);
   //printd(5, "QoreSQLStatement::commit() this: %p ds: %p rc: %d\n", this, priv->ds, rc);
   return rc;
}

int QoreSQLStatement::rollback(ExceptionSink* xsink) {
   DBActionHelper dba(*this, xsink, DAH_RELEASE);
   if (!dba)
      return -1;

   int rc = closeIntern(xsink);
   rc = priv->ds->rollback(xsink);
   //printd(5, "QoreSQLStatement::rollback() this: %p ds: %p rc: %d\n", this, priv->ds, rc);
   return rc;
}

int QoreSQLStatement::beginTransaction(ExceptionSink* xsink) {
   DBActionHelper dba(*this, xsink, DAH_ACQUIRE);
   if (!dba)
      return -1;

   return priv->ds->beginTransaction(xsink);
}

QoreStringNode* QoreSQLStatement::getSQL(ExceptionSink* xsink) {
   // we have to acquire the datasource in order to use the thread lock to access the SQL string
   DBActionHelper dba(*this, xsink, DAH_NOCHANGE);
   if (!dba)
      return 0;

   return str.empty() ? 0 : new QoreStringNode(str);
}
