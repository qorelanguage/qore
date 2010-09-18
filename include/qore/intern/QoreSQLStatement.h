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

#ifndef _QORE_QORESQLSTATEMENT_H
#define _QORE_QORESQLSTATEMENT_H

#include <qore/intern/sql_statement_private.h>

class DatasourceStatementHelper;

#define STMT_IDLE      0
#define STMT_PREPARED  1
#define STMT_DEFINED   2
#define STMT_EXECED    3
#define STMT_DELETED   4

#define STMT_TRANS_UNKNOWN 0
#define STMT_TRANS_NEW     1
#define STMT_TRANS_EXISTED 2

#define STMT_TRANS_TEXT(t) (t == STMT_TRANS_NEW ? "new" : (t == STMT_TRANS_EXISTED ? "existed" : "unknown"))

class DBActionHelper;

class QoreSQLStatement : public AbstractPrivateData, public SQLStatement {
   friend class DBActionHelper;

protected:
   DLLLOCAL static const char *stmt_statuses[];

   // helper object for acquiring a Datasource pointer
   DatasourceStatementHelper *dsh;
   // copy of SQL string
   QoreString str;
   // copy of prepare args
   QoreListNode *prepare_args;
   // status
   unsigned char status;
   // new transaction status
   char trans_status;
   // raw prepare flag
   bool raw;

   DLLLOCAL static int invalidException(ExceptionSink *xsink) {
      xsink->raiseException("SQLSTATMENT-ERROR", "TID %d attempted to acquire already deleted SQLStatement object", gettid());
      return -1;
   }

   DLLLOCAL int checkStatus(DBActionHelper &dba, int stat, const char *action, ExceptionSink *xsink);

   DLLLOCAL int closeIntern(ExceptionSink *xsink);
   DLLLOCAL int execIntern(DBActionHelper &dba, ExceptionSink *xsink);
   DLLLOCAL int defineIntern(ExceptionSink *xsink);
   DLLLOCAL int prepareIntern(ExceptionSink *xsink);
   DLLLOCAL int prepareArgs(bool n_raw, const QoreString &n_str, const QoreListNode *args, ExceptionSink *xsink);
      
public:
   DLLLOCAL QoreSQLStatement() : dsh(0), prepare_args(0), status(STMT_IDLE), trans_status(STMT_TRANS_UNKNOWN), raw(false) {
   }

   DLLLOCAL ~QoreSQLStatement();

   DLLLOCAL void init(DatasourceStatementHelper *n_dsh);

   DLLLOCAL virtual void deref(ExceptionSink *xsink);

   DLLLOCAL int prepare(const QoreString &str, const QoreListNode *args, ExceptionSink *xsink);

   DLLLOCAL int prepareRaw(const QoreString &str, ExceptionSink *xsink);

   DLLLOCAL int bind(const QoreListNode &l, ExceptionSink *xsink);
   DLLLOCAL int bindPlaceholders(const QoreListNode &l, ExceptionSink *xsink);
   DLLLOCAL int bindValues(const QoreListNode &l, ExceptionSink *xsink);

   DLLLOCAL int exec(ExceptionSink *xsink);

   DLLLOCAL int affectedRows(ExceptionSink *xsink);

   DLLLOCAL QoreHashNode *getOutput(ExceptionSink *xsink);

   DLLLOCAL QoreHashNode *getOutputRows(ExceptionSink *xsink);

   DLLLOCAL bool next(ExceptionSink *xsink);

   DLLLOCAL int define(ExceptionSink *xsink);

   DLLLOCAL QoreHashNode *fetchRow(ExceptionSink *xsink);
   DLLLOCAL QoreListNode *fetchRows(int rows, ExceptionSink *xsink);
   DLLLOCAL QoreHashNode *fetchColumns(int rows, ExceptionSink *xsink);

   DLLLOCAL int close(ExceptionSink *xsink);
   DLLLOCAL int commit(ExceptionSink *xsink);
   DLLLOCAL int rollback(ExceptionSink *xsink);
   DLLLOCAL int beginTransaction(ExceptionSink *xsink);

   DLLLOCAL bool active() const;
};

#endif
