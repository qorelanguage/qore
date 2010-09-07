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

const char *QoreSQLStatement::stmt_statuses[] = { "idle", "prepared", "execed", "deleted" };

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
   assert(!priv->ds);
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

   return rc;
}

int QoreSQLStatement::close(ExceptionSink *xsink) {
   DBActionHelper dba(dsh, false, xsink);
   if (!dba)
      return -1;

   return closeIntern(xsink);
}

int QoreSQLStatement::prepare(QoreString &str, ExceptionSink *xsink) {
   DBActionHelper dba(dsh, false, xsink);
   if (!dba)
      return -1;

   if (checkStatus(STMT_IDLE, "prepare", xsink))
      return -1;

   int rc = priv->ds->getDriver()->stmt_prepare(this, str, xsink);
   if (!rc)
      status = STMT_PREPARED;
   return rc;
}

int QoreSQLStatement::bind(QoreListNode &l, ExceptionSink *xsink) {
   DBActionHelper dba(dsh, false, xsink);
   if (!dba)
      return -1;

   if (checkStatus(STMT_PREPARED, "bind", xsink))
      return -1;

   return priv->ds->getDriver()->stmt_bind(this, l, xsink);
}

int QoreSQLStatement::exec(ExceptionSink *xsink) {
   DBActionHelper dba(dsh, priv->trans, xsink);
   if (!dba)
      return -1;

   if (checkStatus(STMT_PREPARED, "exec", xsink))
      return -1;

   int rc = priv->ds->getDriver()->stmt_exec(this, xsink);
   if (!rc)
      status = STMT_EXECED;
   return rc;
}

bool QoreSQLStatement::next(ExceptionSink *xsink) {
   DBActionHelper dba(dsh, false, xsink);
   if (!dba)
      return false;

   if (checkStatus(STMT_EXECED, "next", xsink))
      return false;

   return priv->ds->getDriver()->stmt_next(this, xsink);
}

QoreListNode *QoreSQLStatement::fetchRow(ExceptionSink *xsink) {
   DBActionHelper dba(dsh, false, xsink);
   if (!dba)
      return 0;

   if (checkStatus(STMT_EXECED, "fetchRow", xsink))
      return 0;

   return priv->ds->getDriver()->stmt_fetch_row(this, xsink);
}

bool QoreSQLStatement::active() const {
   return status != STMT_IDLE;
}
