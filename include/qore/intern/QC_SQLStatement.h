/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QC_SQLStatement.h

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

#ifndef _QORE_CLASS_SQLSTATEMENT_H
#define _QORE_CLASS_SQLSTATEMENT_H

#include <qore/AbstractThreadResource.h>
#include <qore/intern/sql_statement_private.h>
#include <qore/intern/DatasourceStatementHelper.h>

DLLEXPORT extern qore_classid_t CID_SQLSTATEMENT;
DLLLOCAL QoreClass *initSQLStatementClass(QoreClass *QC_Datasource, QoreClass *QC_DatasourcePool);

class QoreSQLStatement : public AbstractThreadResource, public SQLStatement {
protected:
   // mutex
   QoreThreadLock m;
   // helper object for acquiring a Datasource pointer
   DatasourceStatementHelper *dsh;
   // tid that currently owns the object
   int tid;
   // valid flag
   bool valid;

   DLLLOCAL static int invalidException(ExceptionSink *xsink) {
      xsink->raiseException("SQLSTATMENT-ERROR", "TID %d attempted to acquire already deleted SQLStatement object", gettid());
      return -1;
   }

   DLLLOCAL int checkOwner(const char *action, ExceptionSink *xsink) {
      AutoLocker al(m);
      if (tid != gettid()) {
         xsink->raiseException("SQLSTATMENT-ERROR", "SQLStatement::%s() called without first calling SQLStatement::prepare()", action);
         return -1;
      }
      if (!valid)
         return invalidException(xsink);

      return 0;
   }

   DLLLOCAL int acquireObject(ExceptionSink *xsink) {
      int mtid = gettid();

      AutoLocker al(m);
      if (tid != mtid) {
         if (tid != -1) {
            xsink->raiseException("SQLSTATMENT-ERROR", "cannot acquire SQLStatement object in TID %d while active in TID %d", mtid, tid);
            return -1;
         }
         if (!valid)
            return invalidException(xsink);

	 if (priv->ds && closeInternUnlocked(xsink))
	    return -1;	    

         tid = mtid;
      }

      priv->ds = dsh->helperGetDatasource(xsink);
      if (!priv->ds) {
         tid = -1;
         return -1;
      }

      set_thread_resource(this);

      return 0;
   }

   DLLLOCAL void releaseObject(ExceptionSink *xsink) {
      AutoLocker al(m);
      dsh->helperReleaseDatasource(priv->trans, xsink);
      priv->ds = 0;

      remove_thread_resource(this);

      tid = -1;
   }

   DLLLOCAL int closeIntern(ExceptionSink *xsink) {
      AutoLocker al(m);
      return closeInternUnlocked(xsink);
   }

   DLLLOCAL int closeInternUnlocked(ExceptionSink *xsink) {
      if (tid == -1)
         return 0;

      tid = -1;

      int rc = priv->ds->getDriver()->stmt_close(this, xsink);

      releaseObject(xsink);

      return rc;
   }

public:
   DLLLOCAL QoreSQLStatement(DatasourceStatementHelper *n_dsh) : dsh(n_dsh), tid(-1), valid(true) {
   }

   DLLLOCAL ~QoreSQLStatement() {
      assert(!priv->ds);
      assert(!priv->data);
   }


   DLLLOCAL virtual void cleanup(ExceptionSink *xsink) {
      AutoLocker al(m);

      assert(valid);
      assert(tid == gettid());

      xsink->raiseException("SQLSTATMENT-ERROR", "TID %d terminated with an active SQLStatement object; the statement will be automatically canceled and closed", gettid());

      closeIntern(xsink);
   }

   DLLLOCAL virtual void deref(ExceptionSink *xsink) {
      if (ROdereference()) {
         closeIntern(xsink);
         dsh->helperDestructor(xsink);
         delete this;
      }
   }

   DLLLOCAL void destructor(ExceptionSink *xsink) {
      AutoLocker al(m);
      if (!valid)
         return;

      valid = false;

      if (tid != -1) {
         if (tid != gettid()) {
            xsink->raiseException("SQLSTATMENT-ERROR", "SQLStatement object deleted while owned by TID %d", tid);
            return;
         }

         closeInternUnlocked(xsink);
      }
   }

   DLLLOCAL int prepare(QoreString &str, ExceptionSink *xsink) {
      if (acquireObject(xsink))
         return -1;

      int rc;
      {
         OptLocker ol(dsh->helperGetConnectionLock());
         rc = priv->ds->getDriver()->stmt_prepare(this, str, xsink);
      }
      
      if (rc) {
         releaseObject(xsink);
         return -1;
      }

      return 0;
   }

   DLLLOCAL int bind(QoreListNode &l, ExceptionSink *xsink) {
      if (checkOwner("bind", xsink))
         return -1;

      return priv->ds->getDriver()->stmt_bind(this, l, xsink);
   }

   DLLLOCAL int exec(ExceptionSink *xsink) {
      if (checkOwner("exec", xsink))
         return -1;

      return priv->ds->getDriver()->stmt_exec(this, xsink);
   }

   DLLLOCAL int close(ExceptionSink *xsink) {
      if (checkOwner("close", xsink))
         return -1;

      return closeIntern(xsink);
   }

   DLLLOCAL bool next(ExceptionSink *xsink) {
      if (checkOwner("next", xsink))
         return -1;

      return priv->ds->getDriver()->stmt_next(this, xsink);
   }

   DLLLOCAL QoreListNode *fetchRow(ExceptionSink *xsink) {
      if (checkOwner("fetchRow", xsink))
         return 0;

      return priv->ds->getDriver()->stmt_fetch_row(this, xsink);
   }

   DLLLOCAL bool active() const {
      return priv->ds;
   }

   DLLLOCAL bool inTransaction() const {
      return priv->trans;
   }
};

#endif
