/*
  QC_DatasourcePool.cc

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols

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

int CID_DATASOURCEPOOL;

#define DP_MIN 5
#define DP_MAX 20

// usage: DatasourcePool(db name, [username, password, dbname, charset, hostname, min, max])
static void DSP_constructor(class QoreObject *self, const QoreList *params, ExceptionSink *xsink)
{
   QoreStringNode *pstr = test_string_param(params, 0);
   if (!pstr)
   {
      xsink->raiseException("DATASOURCEPOOL-PARAM-ERROR", "expecting database type as first parameter of DatasourcePool() constructor");
      return;
   }
   DBIDriver *db_driver = DBI.find(pstr->getBuffer());
   if (!db_driver)
   {
      xsink->raiseException("DATASOURCEPOOL-UNSUPPORTED-DATABASE", "no DBI driver can be found for database type '%s'", pstr->getBuffer());
      return;
   }

   const char *user = NULL, *pass = NULL, *db = NULL, *charset = NULL, *host = NULL;
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
   
   QoreNode *p = get_param(params, 6);
   if (!is_nothing(p))
   {
      min = p->getAsInt();
      if (min <= 0)
      {
	 xsink->raiseException("DATASOURCEPOOL-PARAM-ERROR", "minimum connections must be > 0 (value given: %d)", min);
	 return;
      }
   }
   else 
      min = DP_MIN;

   p = get_param(params, 7);
   if (!is_nothing(p))
   {
      max = p->getAsInt();
      if (max < min)
      {
	 xsink->raiseException("DATASOURCEPOOL-PARAM-ERROR", "maximum connections must be >= min(%d) (value given: %d)", min, max);
	 return;
      }
   }
   else 
      max = DP_MAX;
   
   class DatasourcePool *ds = new DatasourcePool(db_driver, user, pass, db, charset, host, min, max, xsink);
   if (xsink->isException())
      ds->deref();
   else
      self->setPrivate(CID_DATASOURCEPOOL, ds);
}

static void DSP_destructor(class QoreObject *self, class DatasourcePool *ds, ExceptionSink *xsink)
{
   ds->destructor(xsink);
   ds->deref();
}

static void DSP_copy(class QoreObject *self, class QoreObject *old, class DatasourcePool *ods, class ExceptionSink *xsink)
{
   xsink->raiseException("COPY-ERROR", "DatasourcePool objects may not be copied");
}

static QoreNode *DSP_commit(class QoreObject *self, class DatasourcePool *ds, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)ds->commit(xsink));
}

static QoreNode *DSP_rollback(class QoreObject *self, class DatasourcePool *ds, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)ds->rollback(xsink));
}

static QoreNode *DSP_exec(class QoreObject *self, class DatasourcePool *ds, const QoreList *params, ExceptionSink *xsink)
{
   class QoreStringNode *p0;
   if (!(p0 = test_string_param(params, 0)))
      return NULL;

   ReferenceHolder<QoreList> args(params->size() > 1 ? params->copyListFrom(1) : 0, xsink);
   return ds->exec(p0, *args, xsink);
}

static QoreNode *DSP_vexec(class QoreObject *self, class DatasourcePool *ds, const QoreList *params, ExceptionSink *xsink)
{
   class QoreStringNode *p0;
   if (!(p0 = test_string_param(params, 0)))
      return NULL;
   
   QoreList *args = test_list_param(params, 1);
   return ds->exec(p0, args, xsink);
}

static QoreNode *DSP_select(class QoreObject *self, class DatasourcePool *ds, const QoreList *params, ExceptionSink *xsink)
{
   QoreStringNode *p = test_string_param(params, 0);
   if (!p)
      return NULL;

   ReferenceHolder<QoreList> args(params->size() > 1 ? params->copyListFrom(1) : 0, xsink);
   return ds->select(p, *args, xsink);
}

static QoreNode *DSP_selectRow(class QoreObject *self, class DatasourcePool *ds, const QoreList *params, ExceptionSink *xsink)
{
   QoreStringNode *p = test_string_param(params, 0);
   if (!p)
      return NULL;
   
   ReferenceHolder<QoreList> args(params->size() > 1 ? params->copyListFrom(1) : 0, xsink);
   return ds->selectRow(p, *args, xsink);
}

static QoreNode *DSP_selectRows(class QoreObject *self, class DatasourcePool *ds, const QoreList *params, ExceptionSink *xsink)
{
   QoreStringNode *p = test_string_param(params, 0);
   if (!p)
      return NULL;

   ReferenceHolder<QoreList> args(params->size() > 1 ? params->copyListFrom(1) : 0, xsink);
   return ds->selectRows(p, *args, xsink);
}

static QoreNode *DSP_vselect(class QoreObject *self, class DatasourcePool *ds, const QoreList *params, ExceptionSink *xsink)
{
   QoreStringNode *p0 = test_string_param(params, 0);
   if (!p0)
      return NULL;
   
   QoreList *args = test_list_param(params, 1);
   return ds->select(p0, args, xsink);
}

static QoreNode *DSP_vselectRow(class QoreObject *self, class DatasourcePool *ds, const QoreList *params, ExceptionSink *xsink)
{
   QoreStringNode *p0 = test_string_param(params, 0);
   if (!p0)
      return NULL;
   
   QoreList *args = test_list_param(params, 1);
   return ds->selectRow(p0, args, xsink);
}

static QoreNode *DSP_vselectRows(class QoreObject *self, class DatasourcePool *ds, const QoreList *params, ExceptionSink *xsink)
{
   QoreStringNode *p0 = test_string_param(params, 0);
   if (!p0)
      return NULL;
   
   QoreList *args = test_list_param(params, 1);
   return ds->selectRows(p0, args, xsink);
}

static QoreNode *DSP_beginTransaction(class QoreObject *self, class DatasourcePool *ds, const QoreList *params, ExceptionSink *xsink)
{
   ds->beginTransaction(xsink);
   return NULL;
}

static QoreNode *DSP_getUserName(class QoreObject *self, class DatasourcePool *ds, const QoreList *params, ExceptionSink *xsink)
{
   return ds->getPendingUsername();
}

static QoreNode *DSP_getPassword(class QoreObject *self, class DatasourcePool *ds, const QoreList *params, ExceptionSink *xsink)
{
   return ds->getPendingPassword();
}

static QoreNode *DSP_getDBName(class QoreObject *self, class DatasourcePool *ds, const QoreList *params, ExceptionSink *xsink)
{
   return ds->getPendingDBName();
}

static QoreNode *DSP_getDBCharset(class QoreObject *self, class DatasourcePool *ds, const QoreList *params, ExceptionSink *xsink)
{
   return ds->getPendingDBEncoding();
}

static QoreNode *DSP_getOSCharset(class QoreObject *self, class DatasourcePool *ds, const QoreList *params, ExceptionSink *xsink)
{
   const QoreEncoding *enc = ds->getQoreEncoding();
   return new QoreStringNode(enc ? enc->getCode() : "(unknown)");
}

static QoreNode *DSP_getHostName(class QoreObject *self, class DatasourcePool *ds, const QoreList *params, ExceptionSink *xsink)
{
   return ds->getPendingHostName();
}

static QoreNode *DSP_getDriverName(class QoreObject *self, class DatasourcePool *ds, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreStringNode(ds->getDriverName());
}

static QoreNode *DSP_getMinimum(class QoreObject *self, class DatasourcePool *ds, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)ds->getMin());
}

static QoreNode *DSP_getMaximum(class QoreObject *self, class DatasourcePool *ds, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)ds->getMax());
}

static QoreNode *DSP_toString(class QoreObject *self, class DatasourcePool *ds, const QoreList *params, ExceptionSink *xsink)
{
   return ds->toString();
}

static QoreNode *DSP_getServerVersion(class QoreObject *self, class DatasourcePool *ds, const QoreList *params, ExceptionSink *xsink)
{
   return ds->getServerVersion(xsink);
}

static QoreNode *DSP_getClientVersion(class QoreObject *self, class DatasourcePool *ds, const QoreList *params, ExceptionSink *xsink)
{
   return ds->getClientVersion(xsink);
}

static QoreNode *DSP_inTransaction(class QoreObject *self, class DatasourcePool *ds, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode(ds->inTransaction());
}

class QoreClass *initDatasourcePoolClass()
{
   tracein("initDatasourcePoolClass()");

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
   QC_DATASOURCEPOOL->addMethod("getDriverName",     (q_method_t)DSP_getDriverName);
   QC_DATASOURCEPOOL->addMethod("getMinimum",        (q_method_t)DSP_getMinimum);
   QC_DATASOURCEPOOL->addMethod("getMaximum",        (q_method_t)DSP_getMaximum);
   QC_DATASOURCEPOOL->addMethod("toString",          (q_method_t)DSP_toString);
   QC_DATASOURCEPOOL->addMethod("getServerVersion",  (q_method_t)DSP_getServerVersion);
   QC_DATASOURCEPOOL->addMethod("getClientVersion",  (q_method_t)DSP_getClientVersion);
   QC_DATASOURCEPOOL->addMethod("inTransaction",     (q_method_t)DSP_inTransaction);

   traceout("initDatasourcePoolClass()");
   return QC_DATASOURCEPOOL;
}
