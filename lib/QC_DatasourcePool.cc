/*
  QC_DatasourcePool.cc

  Qore Programming Language

  Copyright 2003 - 2009 David Nichols

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

#include <qore/intern/QC_DatasourcePool.h>

qore_classid_t CID_DATASOURCEPOOL;

#define DP_MIN 5
#define DP_MAX 20

// usage: DatasourcePool(db name, [username, password, dbname, charset, hostname, min, max])
static void DSP_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *pstr = test_string_param(params, 0);
   if (!pstr) {
      xsink->raiseException("DATASOURCEPOOL-PARAM-ERROR", "expecting database type as first parameter of DatasourcePool() constructor");
      return;
   }
   DBIDriver *db_driver = DBI.find(pstr->getBuffer());
   if (!db_driver) {
      xsink->raiseException("DATASOURCEPOOL-UNSUPPORTED-DATABASE", "no DBI driver can be found for database type '%s'", pstr->getBuffer());
      return;
   }

   const char *user = 0, *pass = 0, *db = 0, *charset = 0, *host = 0;
   int min, max;
   if ((pstr = test_string_param(params, 1)))
      user = pstr->getBuffer();

   if ((pstr = test_string_param(params, 2)))
      pass = pstr->getBuffer();

   if ((pstr = test_string_param(params, 3)))
      db = pstr->getBuffer();
   
   if ((pstr = test_string_param(params, 4)))
      charset = pstr->getBuffer();

   if ((pstr = test_string_param(params, 5)))
      host = pstr->getBuffer();
   
   const AbstractQoreNode *p = get_param(params, 6);
   if (!is_nothing(p)) {
      min = p->getAsInt();
      if (min <= 0) {
	 xsink->raiseException("DATASOURCEPOOL-PARAM-ERROR", "minimum connections must be > 0 (value given: %d)", min);
	 return;
      }
   }
   else
      min = DP_MIN;

   p = get_param(params, 7);
   if (!is_nothing(p)) {
      max = p->getAsInt();
      if (max < min) {
	 xsink->raiseException("DATASOURCEPOOL-PARAM-ERROR", "maximum connections must be >= min(%d) (value given: %d)", min, max);
	 return;
      }
   }
   else 
      max = DP_MAX;
   
   int port = get_int_param(params, 8);

   DatasourcePool *ds = new DatasourcePool(db_driver, user, pass, db, charset, host, min, max, port, xsink);
   if (xsink->isException())
      ds->deref();
   else
      self->setPrivate(CID_DATASOURCEPOOL, ds);
}

static void DSP_destructor(QoreObject *self, DatasourcePool *ds, ExceptionSink *xsink) {
   ds->destructor(xsink);
   ds->deref();
}

static void DSP_copy(QoreObject *self, QoreObject *old, DatasourcePool *ods, ExceptionSink *xsink) {
   xsink->raiseException("COPY-ERROR", "DatasourcePool objects may not be copied");
}

static AbstractQoreNode *DSP_commit(QoreObject *self, DatasourcePool *ds, const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreBigIntNode(ds->commit(xsink));
}

static AbstractQoreNode *DSP_rollback(QoreObject *self, DatasourcePool *ds, const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreBigIntNode(ds->rollback(xsink));
}

static AbstractQoreNode *DSP_exec(QoreObject *self, DatasourcePool *ds, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0;
   if (!(p0 = test_string_param(params, 0)))
      return 0;

   ReferenceHolder<QoreListNode> args(params->size() > 1 ? params->copyListFrom(1) : 0, xsink);
   return ds->exec(p0, *args, xsink);
}

static AbstractQoreNode *DSP_vexec(QoreObject *self, DatasourcePool *ds, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0;
   if (!(p0 = test_string_param(params, 0)))
      return 0;
   
   const QoreListNode *args = test_list_param(params, 1);
   return ds->exec(p0, args, xsink);
}

static AbstractQoreNode *DSP_select(QoreObject *self, DatasourcePool *ds, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p = test_string_param(params, 0);
   if (!p)
      return 0;

   ReferenceHolder<QoreListNode> args(params->size() > 1 ? params->copyListFrom(1) : 0, xsink);
   return ds->select(p, *args, xsink);
}

static AbstractQoreNode *DSP_selectRow(QoreObject *self, DatasourcePool *ds, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p = test_string_param(params, 0);
   if (!p)
      return 0;
   
   ReferenceHolder<QoreListNode> args(params->size() > 1 ? params->copyListFrom(1) : 0, xsink);
   return ds->selectRow(p, *args, xsink);
}

static AbstractQoreNode *DSP_selectRows(QoreObject *self, DatasourcePool *ds, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p = test_string_param(params, 0);
   if (!p)
      return 0;

   ReferenceHolder<QoreListNode> args(params->size() > 1 ? params->copyListFrom(1) : 0, xsink);
   return ds->selectRows(p, *args, xsink);
}

static AbstractQoreNode *DSP_vselect(QoreObject *self, DatasourcePool *ds, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = test_string_param(params, 0);
   if (!p0)
      return 0;
   
   const QoreListNode *args = test_list_param(params, 1);
   return ds->select(p0, args, xsink);
}

static AbstractQoreNode *DSP_vselectRow(QoreObject *self, DatasourcePool *ds, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = test_string_param(params, 0);
   if (!p0)
      return 0;
   
   const QoreListNode *args = test_list_param(params, 1);
   return ds->selectRow(p0, args, xsink);
}

static AbstractQoreNode *DSP_vselectRows(QoreObject *self, DatasourcePool *ds, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = test_string_param(params, 0);
   if (!p0)
      return 0;
   
   const QoreListNode *args = test_list_param(params, 1);
   return ds->selectRows(p0, args, xsink);
}

static AbstractQoreNode *DSP_beginTransaction(QoreObject *self, DatasourcePool *ds, const QoreListNode *params, ExceptionSink *xsink) {
   ds->beginTransaction(xsink);
   return 0;
}

static AbstractQoreNode *DSP_getUserName(QoreObject *self, DatasourcePool *ds, const QoreListNode *params, ExceptionSink *xsink) {
   return ds->getPendingUsername();
}

static AbstractQoreNode *DSP_getPassword(QoreObject *self, DatasourcePool *ds, const QoreListNode *params, ExceptionSink *xsink) {
   return ds->getPendingPassword();
}

static AbstractQoreNode *DSP_getDBName(QoreObject *self, DatasourcePool *ds, const QoreListNode *params, ExceptionSink *xsink) {
   return ds->getPendingDBName();
}

static AbstractQoreNode *DSP_getDBCharset(QoreObject *self, DatasourcePool *ds, const QoreListNode *params, ExceptionSink *xsink) {
   return ds->getPendingDBEncoding();
}

static AbstractQoreNode *DSP_getOSCharset(QoreObject *self, DatasourcePool *ds, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreEncoding *enc = ds->getQoreEncoding();
   return new QoreStringNode(enc ? enc->getCode() : "(unknown)");
}

static AbstractQoreNode *DSP_getHostName(QoreObject *self, DatasourcePool *ds, const QoreListNode *params, ExceptionSink *xsink) {
   return ds->getPendingHostName();
}

static AbstractQoreNode *DSP_getPort(QoreObject *self, DatasourcePool *ds, const QoreListNode *params, ExceptionSink *xsink) {
   int port = ds->getPendingPort();
   return port ? new QoreBigIntNode(port) : 0;
}

static AbstractQoreNode *DSP_getDriverName(QoreObject *self, DatasourcePool *ds, const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreStringNode(ds->getDriverName());
}

static AbstractQoreNode *DSP_getMinimum(QoreObject *self, DatasourcePool *ds, const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreBigIntNode(ds->getMin());
}

static AbstractQoreNode *DSP_getMaximum(QoreObject *self, DatasourcePool *ds, const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreBigIntNode(ds->getMax());
}

static AbstractQoreNode *DSP_toString(QoreObject *self, DatasourcePool *ds, const QoreListNode *params, ExceptionSink *xsink) {
   return ds->toString();
}

static AbstractQoreNode *DSP_getServerVersion(QoreObject *self, DatasourcePool *ds, const QoreListNode *params, ExceptionSink *xsink) {
   return ds->getServerVersion(xsink);
}

static AbstractQoreNode *DSP_getClientVersion(QoreObject *self, DatasourcePool *ds, const QoreListNode *params, ExceptionSink *xsink) {
   return ds->getClientVersion(xsink);
}

static AbstractQoreNode *DSP_inTransaction(QoreObject *self, DatasourcePool *ds, const QoreListNode *params, ExceptionSink *xsink) {
   return get_bool_node(ds->inTransaction());
}

class QoreClass *initDatasourcePoolClass()
{
   QORE_TRACE("initDatasourcePoolClass()");

   class QoreClass *QC_DATASOURCEPOOL = new QoreClass("DatasourcePool", QDOM_DATABASE);
   CID_DATASOURCEPOOL = QC_DATASOURCEPOOL->getID();
   QC_DATASOURCEPOOL->setConstructor(DSP_constructor);
   QC_DATASOURCEPOOL->setDestructor((q_destructor_t)DSP_destructor);
   QC_DATASOURCEPOOL->setCopy((q_copy_t)DSP_copy);
   QC_DATASOURCEPOOL->addMethod("commit",            (q_method_t)DSP_commit);
   QC_DATASOURCEPOOL->addMethod("rollback",          (q_method_t)DSP_rollback);
   QC_DATASOURCEPOOL->addMethod("exec",              (q_method_t)DSP_exec);
   QC_DATASOURCEPOOL->addMethod("vexec",             (q_method_t)DSP_vexec);
   QC_DATASOURCEPOOL->addMethod("select",            (q_method_t)DSP_select);
   QC_DATASOURCEPOOL->addMethod("selectRow",         (q_method_t)DSP_selectRow);
   QC_DATASOURCEPOOL->addMethod("selectRows",        (q_method_t)DSP_selectRows);
   QC_DATASOURCEPOOL->addMethod("vselect",           (q_method_t)DSP_vselect);
   QC_DATASOURCEPOOL->addMethod("vselectRow",        (q_method_t)DSP_vselectRow);
   QC_DATASOURCEPOOL->addMethod("vselectRows",       (q_method_t)DSP_vselectRows);
   QC_DATASOURCEPOOL->addMethod("beginTransaction",  (q_method_t)DSP_beginTransaction);
   QC_DATASOURCEPOOL->addMethod("getUserName",       (q_method_t)DSP_getUserName);
   QC_DATASOURCEPOOL->addMethod("getPassword",       (q_method_t)DSP_getPassword);
   QC_DATASOURCEPOOL->addMethod("getDBName",         (q_method_t)DSP_getDBName);
   QC_DATASOURCEPOOL->addMethod("getDBCharset",      (q_method_t)DSP_getDBCharset);
   QC_DATASOURCEPOOL->addMethod("getOSCharset",      (q_method_t)DSP_getOSCharset);
   QC_DATASOURCEPOOL->addMethod("getHostName",       (q_method_t)DSP_getHostName);
   QC_DATASOURCEPOOL->addMethod("getPort",           (q_method_t)DSP_getPort);
   QC_DATASOURCEPOOL->addMethod("getDriverName",     (q_method_t)DSP_getDriverName);
   QC_DATASOURCEPOOL->addMethod("getMinimum",        (q_method_t)DSP_getMinimum);
   QC_DATASOURCEPOOL->addMethod("getMaximum",        (q_method_t)DSP_getMaximum);
   QC_DATASOURCEPOOL->addMethod("toString",          (q_method_t)DSP_toString);
   QC_DATASOURCEPOOL->addMethod("getServerVersion",  (q_method_t)DSP_getServerVersion);
   QC_DATASOURCEPOOL->addMethod("getClientVersion",  (q_method_t)DSP_getClientVersion);
   QC_DATASOURCEPOOL->addMethod("inTransaction",     (q_method_t)DSP_inTransaction);

   return QC_DATASOURCEPOOL;
}
