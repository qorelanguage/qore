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

#include <qore/QC_DatasourcePool.h>

int CID_DATASOURCEPOOL;

#define DP_MIN 5
#define DP_MAX 20

// usage: DatasourcePool(db name, [username, password, dbname, charset, hostname, min, max])
static void DSP_constructor(class QoreObject *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_STRING, 0);
   if (!p)
   {
      xsink->raiseException("DATASOURCEPOOL-PARAM-ERROR", "expecting database type as first parameter of DatasourcePool() constructor");
      return;
   }
   DBIDriver *db_driver = DBI.find((const char*)p->val.String->getBuffer());
   if (!db_driver)
   {
      xsink->raiseException("DATASOURCEPOOL-UNSUPPORTED-DATABASE", "no DBI driver can be found for database type '%s'", p->val.String->getBuffer());
      return;
   }

   const char *user = NULL, *pass = NULL, *db = NULL, *charset = NULL, *host = NULL;
   int min, max;
   if ((p = test_param(params, NT_STRING, 1)))
      user = p->val.String->getBuffer();

   if ((p = test_param(params, NT_STRING, 2)))
      pass = p->val.String->getBuffer();

   if ((p = test_param(params, NT_STRING, 3)))
      db = p->val.String->getBuffer();
   
   if ((p = test_param(params, NT_STRING, 4)))
      charset = p->val.String->getBuffer();

   if ((p = test_param(params, NT_STRING, 5)))
      host = p->val.String->getBuffer();
   
   p = get_param(params, 6);
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

static QoreNode *DSP_commit(class QoreObject *self, class DatasourcePool *ds, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)ds->commit(xsink));
}

static QoreNode *DSP_rollback(class QoreObject *self, class DatasourcePool *ds, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)ds->rollback(xsink));
}

static QoreNode *DSP_exec(class QoreObject *self, class DatasourcePool *ds, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p0;
   if (!(p0 = test_param(params, NT_STRING, 0)))
      return NULL;

   QoreList *args = params->val.list->size() > 1 ? params->val.list->copyListFrom(1) : NULL;
   class QoreNode *rv = ds->exec(p0->val.String, args, xsink);
   if (args)
      args->derefAndDelete(xsink);
   return rv;
}

static QoreNode *DSP_vexec(class QoreObject *self, class DatasourcePool *ds, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p0;
   if (!(p0 = test_param(params, NT_STRING, 0)))
      return NULL;

   QoreNode *p1 = test_param(params, NT_LIST, 1);
   QoreList *args = p1 ? p1->val.list : NULL;
   return ds->exec(p0->val.String, args, xsink);
}

static QoreNode *DSP_select(class QoreObject *self, class DatasourcePool *ds, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_STRING, 0);
   if (!p)
      return NULL;

   QoreList *args = params->val.list->size() > 1 ? params->val.list->copyListFrom(1) : NULL;
   class QoreNode *rv = ds->select(p->val.String, args, xsink);
   if (args)
      args->derefAndDelete(xsink);
   return rv;
}

static QoreNode *DSP_selectRow(class QoreObject *self, class DatasourcePool *ds, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_STRING, 0);
   if (!p)
      return NULL;
   
   QoreList *args = params->val.list->size() > 1 ? params->val.list->copyListFrom(1) : NULL;
   class QoreNode *rv = ds->selectRow(p->val.String, args, xsink);
   if (args)
      args->derefAndDelete(xsink);
   return rv;
}

static QoreNode *DSP_selectRows(class QoreObject *self, class DatasourcePool *ds, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_STRING, 0);
   if (!p)
      return NULL;

   QoreList *args = params->val.list->size() > 1 ? params->val.list->copyListFrom(1) : NULL;
   class QoreNode *rv = ds->selectRows(p->val.String, args, xsink);
   if (args)
      args->derefAndDelete(xsink);
   return rv;
}

static QoreNode *DSP_vselect(class QoreObject *self, class DatasourcePool *ds, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = test_param(params, NT_STRING, 0);
   if (!p0)
      return NULL;
   
   QoreNode *p1 = test_param(params, NT_LIST, 1);
   QoreList *args = p1 ? p1->val.list : NULL;
   return ds->select(p0->val.String, args, xsink);
}

static QoreNode *DSP_vselectRow(class QoreObject *self, class DatasourcePool *ds, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = test_param(params, NT_STRING, 0);
   if (!p0)
      return NULL;
   
   QoreNode *p1 = test_param(params, NT_LIST, 1);
   QoreList *args = p1 ? p1->val.list : NULL;
   return ds->selectRow(p0->val.String, args, xsink);
}

static QoreNode *DSP_vselectRows(class QoreObject *self, class DatasourcePool *ds, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = test_param(params, NT_STRING, 0);
   if (!p0)
      return NULL;
   
   QoreNode *p1 = test_param(params, NT_LIST, 1);
   QoreList *args = p1 ? p1->val.list : NULL;
   return ds->selectRows(p0->val.String, args, xsink);
}

static QoreNode *DSP_beginTransaction(class QoreObject *self, class DatasourcePool *ds, class QoreNode *params, ExceptionSink *xsink)
{
   ds->beginTransaction(xsink);
   return NULL;
}

static QoreNode *DSP_getUserName(class QoreObject *self, class DatasourcePool *ds, class QoreNode *params, ExceptionSink *xsink)
{
   return ds->getPendingUsername();
}

static QoreNode *DSP_getPassword(class QoreObject *self, class DatasourcePool *ds, class QoreNode *params, ExceptionSink *xsink)
{
   return ds->getPendingPassword();
}

static QoreNode *DSP_getDBName(class QoreObject *self, class DatasourcePool *ds, class QoreNode *params, ExceptionSink *xsink)
{
   return ds->getPendingDBName();
}

static QoreNode *DSP_getDBCharset(class QoreObject *self, class DatasourcePool *ds, class QoreNode *params, ExceptionSink *xsink)
{
   return ds->getPendingDBEncoding();
}

static QoreNode *DSP_getOSCharset(class QoreObject *self, class DatasourcePool *ds, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreEncoding *enc = ds->getQoreEncoding();
   return new QoreNode(enc ? enc->getCode() : "(unknown)");
}

static QoreNode *DSP_getHostName(class QoreObject *self, class DatasourcePool *ds, class QoreNode *params, ExceptionSink *xsink)
{
   return ds->getPendingHostName();
}

static QoreNode *DSP_getDriverName(class QoreObject *self, class DatasourcePool *ds, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(ds->getDriverName());
}

static QoreNode *DSP_getMinimum(class QoreObject *self, class DatasourcePool *ds, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)ds->getMin());
}

static QoreNode *DSP_getMaximum(class QoreObject *self, class DatasourcePool *ds, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)ds->getMax());
}

static QoreNode *DSP_toString(class QoreObject *self, class DatasourcePool *ds, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(ds->toString());
}

static QoreNode *DSP_getServerVersion(class QoreObject *self, class DatasourcePool *ds, class QoreNode *params, ExceptionSink *xsink)
{
   return ds->getServerVersion(xsink);
}

static QoreNode *DSP_getClientVersion(class QoreObject *self, class DatasourcePool *ds, class QoreNode *params, ExceptionSink *xsink)
{
   return ds->getClientVersion(xsink);
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

   traceout("initDatasourcePoolClass()");
   return QC_DATASOURCEPOOL;
}
