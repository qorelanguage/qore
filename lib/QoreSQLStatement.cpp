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
   DatasourceStatementHelper *dsh;
   bool valid;

   DLLLOCAL DBActionHelper(DatasourceStatementHelper *n_dsh, bool needs_transaction_lock, ExceptionSink *xsink) : dsh(n_dsh) {
      valid = !dsh->helperStartAction(needs_transaction_lock, xsink);
   }

   DLLLOCAL ~DBActionHelper() {
      if (valid)
	 dsh->helperEndAction();
   }
   
   DLLLOCAL operator bool() const {
      return valid;
   }
};

QoreSQLStatement::~QoreSQLStatement() {
   assert(!priv->data);
}

int QoreSQLStatement::init(DatasourceStatementHelper *n_dsh, ExceptionSink *xsink) {
   dsh = n_dsh;
   priv->ds = dsh->helperGetDatasource(xsink);
   return *xsink ? -1 : 0;
}

void QoreSQLStatement::deref(ExceptionSink *xsink) {
   if (ROdereference()) {
      closeIntern(xsink);
      dsh->helperDestructor(this, xsink);
      delete this;
   }
}

int QoreSQLStatement::closeIntern(ExceptionSink *xsink) {
   if (!priv->data)
      return 0;

   int rc = priv->ds->getDriver()->stmt_close(this, xsink);
   assert(!priv->data);

   dsh->helperReleaseDatasource();

   priv->reset();

   return rc;
}

int QoreSQLStatement::close(ExceptionSink *xsink) {
   DBActionHelper dba(dsh, false, xsink);
   if (!dba)
      return -1;

   return closeIntern(xsink);
}

int QoreSQLStatement::prepare(const QoreString &str, const QoreListNode *args, ExceptionSink *xsink) {
   DBActionHelper dba(dsh, false, xsink);
   if (!dba)
      return -1;

   if (checkStatus(STMT_IDLE, "prepare", xsink))
      return -1;

   int rc = priv->ds->getDriver()->stmt_prepare(this, str, args, xsink);
   if (!rc)
      status = STMT_PREPARED;
   return rc;
}

int QoreSQLStatement::prepareRaw(const QoreString &str, ExceptionSink *xsink) {
   DBActionHelper dba(dsh, false, xsink);
   if (!dba)
      return -1;

   if (checkStatus(STMT_IDLE, "prepareRaw", xsink))
      return -1;

   int rc = priv->ds->getDriver()->stmt_prepare_raw(this, str, xsink);
   if (!rc)
      status = STMT_PREPARED;
   return rc;
}

int QoreSQLStatement::bind(const QoreListNode &l, ExceptionSink *xsink) {
   DBActionHelper dba(dsh, false, xsink);
   if (!dba)
      return -1;

   if (checkStatus(STMT_PREPARED, "bind", xsink))
      return -1;

   return priv->ds->getDriver()->stmt_bind(this, l, xsink);
}

int QoreSQLStatement::bindPlaceholders(const QoreListNode &l, ExceptionSink *xsink) {
   DBActionHelper dba(dsh, false, xsink);
   if (!dba)
      return -1;

   if (checkStatus(STMT_PREPARED, "bindPlaceholders", xsink))
      return -1;

   return priv->ds->getDriver()->stmt_bind_placeholders(this, l, xsink);
}

int QoreSQLStatement::bindValues(const QoreListNode &l, ExceptionSink *xsink) {
   DBActionHelper dba(dsh, false, xsink);
   if (!dba)
      return -1;

   if (checkStatus(STMT_PREPARED, "bindValues", xsink))
      return -1;

   return priv->ds->getDriver()->stmt_bind_values(this, l, xsink);
}

int QoreSQLStatement::exec(ExceptionSink *xsink) {
   DBActionHelper dba(dsh, priv->trans, xsink);
   if (!dba)
      return -1;

   if (checkStatus(STMT_PREPARED, "exec", xsink))
      return -1;

   return execIntern(xsink);
}

int QoreSQLStatement::execIntern(ExceptionSink *xsink) {
   int rc = priv->ds->getDriver()->stmt_exec(this, xsink);
   if (!rc)
      status = STMT_EXECED;
   return rc;
}

int QoreSQLStatement::affectedRows(ExceptionSink *xsink) {
   DBActionHelper dba(dsh, false, xsink);
   if (!dba)
      return -1;

   if (checkStatus(STMT_EXECED, "affectedRows", xsink))
      return -1;

   return priv->ds->getDriver()->stmt_affected_rows(this, xsink);
}

QoreHashNode *QoreSQLStatement::getOutput(ExceptionSink *xsink) {
   DBActionHelper dba(dsh, false, xsink);
   if (!dba)
      return 0;

   if (checkStatus(STMT_EXECED, "getOutput", xsink))
      return 0;

   return priv->ds->getDriver()->stmt_get_output(this, xsink);
}

QoreHashNode *QoreSQLStatement::getOutputRows(ExceptionSink *xsink) {
   DBActionHelper dba(dsh, false, xsink);
   if (!dba)
      return 0;

   if (checkStatus(STMT_EXECED, "getOutputRows", xsink))
      return 0;

   return priv->ds->getDriver()->stmt_get_output_rows(this, xsink);
}

bool QoreSQLStatement::next(ExceptionSink *xsink) {
   DBActionHelper dba(dsh, false, xsink);
   if (!dba)
      return false;

   if (checkStatus(STMT_DEFINED, "next", xsink))
      return false;

   return priv->ds->getDriver()->stmt_next(this, xsink);
}

int QoreSQLStatement::define(ExceptionSink *xsink) {
   DBActionHelper dba(dsh, false, xsink);
   if (!dba)
      return false;

   if (checkStatus(STMT_EXECED, "define", xsink))
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
   DBActionHelper dba(dsh, false, xsink);
   if (!dba)
      return 0;

   if (checkStatus(STMT_DEFINED, "fetchRow", xsink))
      return 0;

   return priv->ds->getDriver()->stmt_fetch_row(this, xsink);
}

bool QoreSQLStatement::active() const {
   return status != STMT_IDLE;
}
