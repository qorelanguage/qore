/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreSQLStatement.h

  Qore Programming Language

  Copyright (C) 2006 - 2014 Qore Technologies, sro
  
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

#ifndef _QORE_QORESQLSTATEMENT_H
#define _QORE_QORESQLSTATEMENT_H

#include <qore/intern/sql_statement_private.h>

class DatasourceStatementHelper;

#define STMT_IDLE      0
#define STMT_PREPARED  1
#define STMT_EXECED    2
#define STMT_DEFINED   3

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
   // raw prepare flag
   bool raw;
   // valid flag
   bool validp;

   DLLLOCAL int checkStatus(DBActionHelper &dba, int stat, const char *action, ExceptionSink *xsink);

   DLLLOCAL int closeIntern(ExceptionSink *xsink);
   DLLLOCAL int execIntern(DBActionHelper &dba, ExceptionSink *xsink);
   DLLLOCAL int defineIntern(ExceptionSink *xsink);
   DLLLOCAL int prepareIntern(ExceptionSink *xsink);
   DLLLOCAL int prepareArgs(bool n_raw, const QoreString &n_str, const QoreListNode *args, ExceptionSink *xsink);
      
public:
   DLLLOCAL QoreSQLStatement() : dsh(0), prepare_args(0), status(STMT_IDLE), raw(false), validp(false) {
   }

   DLLLOCAL ~QoreSQLStatement();

   DLLLOCAL void init(DatasourceStatementHelper *n_dsh);

   using AbstractPrivateData::deref;
   DLLLOCAL virtual void deref(ExceptionSink *xsink);

   DLLLOCAL int prepare(const QoreString &n_str, const QoreListNode *args, ExceptionSink *xsink);

   DLLLOCAL int prepareRaw(const QoreString &n_str, ExceptionSink *xsink);

   DLLLOCAL int bind(const QoreListNode &l, ExceptionSink *xsink);
   DLLLOCAL int bindPlaceholders(const QoreListNode &l, ExceptionSink *xsink);
   DLLLOCAL int bindValues(const QoreListNode &l, ExceptionSink *xsink);

   DLLLOCAL int exec(const QoreListNode *args, ExceptionSink *xsink);

   DLLLOCAL int affectedRows(ExceptionSink *xsink);

   DLLLOCAL QoreHashNode *getOutput(ExceptionSink *xsink);

   DLLLOCAL QoreHashNode *getOutputRows(ExceptionSink *xsink);

   DLLLOCAL bool next(ExceptionSink *xsink);
   DLLLOCAL bool valid();

   DLLLOCAL int define(ExceptionSink *xsink);

   DLLLOCAL QoreHashNode *fetchRow(ExceptionSink *xsink);

   DLLLOCAL QoreListNode *fetchRows(int rows, ExceptionSink *xsink);
   DLLLOCAL QoreHashNode *fetchColumns(int rows, ExceptionSink *xsink);

   DLLLOCAL QoreHashNode *describe(ExceptionSink *xsink);

   DLLLOCAL int close(ExceptionSink *xsink);
   DLLLOCAL int commit(ExceptionSink *xsink);
   DLLLOCAL int rollback(ExceptionSink *xsink);
   DLLLOCAL int beginTransaction(ExceptionSink *xsink);

   DLLLOCAL bool active() const;

   DLLLOCAL QoreStringNode *getSQL(ExceptionSink *xsink);
};

#endif
