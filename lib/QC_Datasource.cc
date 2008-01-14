/*
  QC_Datasource.cc

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
#include <qore/intern/QC_Datasource.h>

int CID_DATASOURCE;

// usage: Datasource(db name, [username, password, dbname])
static void DS_constructor(class QoreObject *self, const QoreNode *params, ExceptionSink *xsink)
{
   QoreStringNode *p = test_string_param(params, 0);
   if (!p || !p->strlen())
   {
      xsink->raiseException("DATASOURCE-PARAMETER-ERROR", "expecting database driver name as first parameter of ManagedDatasource() constructor");
      return;
   }
   DBIDriver *db_driver = DBI.find(p->getBuffer());
   if (!db_driver)
   {
      xsink->raiseException("DATASOURCE-UNSUPPORTED-DATABASE", "DBI driver '%s' cannot be loaded", p->getBuffer());
      return;
   }
   class ManagedDatasource *ds = new ManagedDatasource(db_driver);

   if ((p = test_string_param(params, 1)) && p->strlen())
      ds->setPendingUsername(p->getBuffer());

   if ((p = test_string_param(params, 2)) && p->strlen())
      ds->setPendingPassword(p->getBuffer());

   if ((p = test_string_param(params, 3)) && p->strlen())
      ds->setPendingDBName(p->getBuffer());
   
   if ((p = test_string_param(params, 4)) && p->strlen())
      ds->setPendingDBEncoding(p->getBuffer());
   
   if ((p = test_string_param(params, 5)) && p->strlen())
      ds->setPendingHostName(p->getBuffer());
   
   self->setPrivate(CID_DATASOURCE, ds);
}

static void DS_destructor(class QoreObject *self, class ManagedDatasource *ods, class ExceptionSink *xsink)
{
   ods->destructor(xsink);
   ods->deref(xsink);
}

static void DS_copy(class QoreObject *self, class QoreObject *old, class ManagedDatasource *ods, class ExceptionSink *xsink)
{
   self->setPrivate(CID_DATASOURCE, ods->copy());
}

static QoreNode *DS_open(class QoreObject *self, class ManagedDatasource *ds, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)ds->open(xsink));
}

static QoreNode *DS_close(class QoreObject *self, class ManagedDatasource *ds, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)ds->close());
}

static QoreNode *DS_commit(class QoreObject *self, class ManagedDatasource *ds, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)ds->commit(xsink));
}

static QoreNode *DS_rollback(class QoreObject *self, class ManagedDatasource *ds, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)ds->rollback(xsink));
}

static QoreNode *DS_setAutoCommit(class QoreObject *self, class ManagedDatasource *ds, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   ds->setAutoCommit(p ? p->getAsBool() : true);
   return NULL;
}

static QoreNode *DS_getAutoCommit(class QoreObject *self, class ManagedDatasource *ds, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(ds->getAutoCommit());
}

static QoreNode *DS_exec(class QoreObject *self, class ManagedDatasource *ds, const QoreNode *params, ExceptionSink *xsink)
{
   class QoreStringNode *p0;
   if (!(p0 = test_string_param(params, 0)))
      return NULL;

   QoreList *args = params->val.list->size() > 1 ? params->val.list->copyListFrom(1) : NULL;
   class QoreNode *rv = ds->exec(p0, args, xsink);
   if (args)
      args->derefAndDelete(xsink);
   return rv;
}

static QoreNode *DS_vexec(class QoreObject *self, class ManagedDatasource *ds, const QoreNode *params, ExceptionSink *xsink)
{
   class QoreStringNode *p0;
   if (!(p0 = test_string_param(params, 0)))
      return NULL;

   QoreNode *p1 = test_param(params, NT_LIST, 1);
   QoreList *args = p1 ? p1->val.list : NULL;
   return ds->exec(p0, args, xsink);
}

static QoreNode *DS_select(class QoreObject *self, class ManagedDatasource *ds, const QoreNode *params, ExceptionSink *xsink)
{
   QoreStringNode *p = test_string_param(params, 0);
   if (!p)
      return NULL;

   QoreList *args = params->val.list->size() > 1 ? params->val.list->copyListFrom(1) : NULL;
   class QoreNode *rv = ds->select(p, args, xsink);
   if (args)
      args->derefAndDelete(xsink);
   return rv;
}

static QoreNode *DS_selectRow(class QoreObject *self, class ManagedDatasource *ds, const QoreNode *params, ExceptionSink *xsink)
{
   QoreStringNode *p = test_string_param(params, 0);
   if (!p)
      return NULL;
   
   QoreList *args = params->val.list->size() > 1 ? params->val.list->copyListFrom(1) : NULL;
   class QoreNode *rv = ds->selectRow(p, args, xsink);
   if (args)
      args->derefAndDelete(xsink);
   return rv;
}

static QoreNode *DS_selectRows(class QoreObject *self, class ManagedDatasource *ds, const QoreNode *params, ExceptionSink *xsink)
{
   QoreStringNode *p = test_string_param(params, 0);
   if (!p)
      return NULL;

   QoreList *args = params->val.list->size() > 1 ? params->val.list->copyListFrom(1) : NULL;
   class QoreNode *rv = ds->selectRows(p, args, xsink);
   if (args)
      args->derefAndDelete(xsink);
   return rv;
}

static QoreNode *DS_vselect(class QoreObject *self, class ManagedDatasource *ds, const QoreNode *params, ExceptionSink *xsink)
{
   QoreStringNode *p0 = test_string_param(params, 0);
   if (!p0)
      return NULL;

   QoreNode *p1 = test_param(params, NT_LIST, 1);
   QoreList *args = p1 ? p1->val.list : NULL;
   return ds->select(p0, args, xsink);
}

static QoreNode *DS_vselectRow(class QoreObject *self, class ManagedDatasource *ds, const QoreNode *params, ExceptionSink *xsink)
{
   QoreStringNode *p0 = test_string_param(params, 0);
   if (!p0)
      return NULL;
   
   QoreNode *p1 = test_param(params, NT_LIST, 1);
   QoreList *args = p1 ? p1->val.list : NULL;
   return ds->selectRow(p0, args, xsink);
}

static QoreNode *DS_vselectRows(class QoreObject *self, class ManagedDatasource *ds, const QoreNode *params, ExceptionSink *xsink)
{
   QoreStringNode *p0 = test_string_param(params, 0);
   if (!p0)
      return NULL;

   QoreNode *p1 = test_param(params, NT_LIST, 1);
   QoreList *args = p1 ? p1->val.list : NULL;
   return ds->selectRows(p0, args, xsink);
}

/*
static QoreNode *DS_describe(class QoreObject *self, class ManagedDatasource *ds, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = test_string_param(params, 0);
   if (!p)
      return NULL;

   class QoreHash *h = ds->describe(p->getBuffer(), xsink);
   if (h)
	return new QoreNode(h);

   return NULL;
}
*/

static QoreNode *DS_beginTransaction(class QoreObject *self, class ManagedDatasource *ds, const QoreNode *params, ExceptionSink *xsink)
{
   ds->beginTransaction(xsink);
   return NULL;
}

static QoreNode *DS_reset(class QoreObject *self, class ManagedDatasource *ds, const QoreNode *params, ExceptionSink *xsink)
{
   ds->reset(xsink);
   return NULL;
}

static QoreNode *DS_getCapabilities(class QoreObject *self, class ManagedDatasource *ds, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)ds->getCapabilities());
}

static QoreNode *DS_getCapabilityList(class QoreObject *self, class ManagedDatasource *ds, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(ds->getCapabilityList());
}

static QoreNode *DS_setUserName(class QoreObject *self, class ManagedDatasource *ds, const QoreNode *params, ExceptionSink *xsink)
{
   QoreStringNode *p = test_string_param(params, 0);
   if (!p)
      return NULL;

   ds->setPendingUsername(p->getBuffer());
   return NULL;
}

static QoreNode *DS_setPassword(class QoreObject *self, class ManagedDatasource *ds, const QoreNode *params, ExceptionSink *xsink)
{
   QoreStringNode *p = test_string_param(params, 0);
   if (!p)
      return NULL;

   ds->setPendingPassword(p->getBuffer());
   return NULL;
}

static QoreNode *DS_setDBName(class QoreObject *self, class ManagedDatasource *ds, const QoreNode *params, ExceptionSink *xsink)
{
   QoreStringNode *p = test_string_param(params, 0);
   if (!p)
      return NULL;

   ds->setPendingDBName(p->getBuffer());
   return NULL;
}

static QoreNode *DS_setDBCharset(class QoreObject *self, class ManagedDatasource *ds, const QoreNode *params, ExceptionSink *xsink)
{
   QoreStringNode *p = test_string_param(params, 0);
   if (!p)
      return NULL;

   ds->setPendingDBEncoding(p->getBuffer());
   return NULL;
}

static QoreNode *DS_setHostName(class QoreObject *self, class ManagedDatasource *ds, const QoreNode *params, ExceptionSink *xsink)
{
   QoreStringNode *p = test_string_param(params, 0);
   if (!p)
      return NULL;

   ds->setPendingHostName(p->getBuffer());
   return NULL;
}

static QoreNode *DS_getUserName(class QoreObject *self, class ManagedDatasource *ds, const QoreNode *params, ExceptionSink *xsink)
{
   return ds->getPendingUsername();
}

static QoreNode *DS_getPassword(class QoreObject *self, class ManagedDatasource *ds, const QoreNode *params, ExceptionSink *xsink)
{
   return ds->getPendingPassword();
}

static QoreNode *DS_getDBName(class QoreObject *self, class ManagedDatasource *ds, const QoreNode *params, ExceptionSink *xsink)
{
   return ds->getPendingDBName();
}

static QoreNode *DS_getDBCharset(class QoreObject *self, class ManagedDatasource *ds, const QoreNode *params, ExceptionSink *xsink)
{
   return ds->getPendingDBEncoding();
}

static QoreNode *DS_getOSCharset(class QoreObject *self, class ManagedDatasource *ds, const QoreNode *params, ExceptionSink *xsink)
{
   const QoreEncoding *enc = ds->getQoreEncoding();
   return new QoreStringNode(enc ? enc->getCode() : "(unknown)");
}

static QoreNode *DS_getHostName(class QoreObject *self, class ManagedDatasource *ds, const QoreNode *params, ExceptionSink *xsink)
{
   return ds->getPendingHostName();
}

static QoreNode *DS_setTransactionLockTimeout(class QoreObject *self, class ManagedDatasource *ds, const QoreNode *params, ExceptionSink *xsink)
{
   ds->setTransactionLockTimeout(getMsZeroInt(get_param(params, 0)));
   return NULL;
}

static QoreNode *DS_getTransactionLockTimeout(class QoreObject *self, class ManagedDatasource *ds, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)ds->getTransactionLockTimeout());
}

static QoreNode *DS_getDriverName(class QoreObject *self, class ManagedDatasource *ds, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(ds->getDriver()->getName());
}

static QoreNode *DS_getServerVersion(class QoreObject *self, class ManagedDatasource *ds, const QoreNode *params, ExceptionSink *xsink)
{
   return ds->getServerVersion(xsink);
}

static QoreNode *DS_getClientVersion(class QoreObject *self, class ManagedDatasource *ds, const QoreNode *params, ExceptionSink *xsink)
{
   return ds->getClientVersion(xsink);
}

static QoreNode *DS_inTransaction(class QoreObject *self, class ManagedDatasource *ds, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(ds->isInTransaction());
}

class QoreClass *initDatasourceClass()
{
   tracein("initDatasourceClass()");

   class QoreClass *QC_DATASOURCE = new QoreClass("Datasource", QDOM_DATABASE);
   CID_DATASOURCE = QC_DATASOURCE->getID();
   QC_DATASOURCE->setConstructor(DS_constructor);
   QC_DATASOURCE->setDestructor((q_destructor_t)DS_destructor);
   QC_DATASOURCE->setCopy((q_copy_t)DS_copy);
   QC_DATASOURCE->addMethod("open",              (q_method_t)DS_open);
   QC_DATASOURCE->addMethod("close",             (q_method_t)DS_close);
   QC_DATASOURCE->addMethod("commit",            (q_method_t)DS_commit);
   QC_DATASOURCE->addMethod("rollback",          (q_method_t)DS_rollback);
   QC_DATASOURCE->addMethod("exec",              (q_method_t)DS_exec);
   QC_DATASOURCE->addMethod("select",            (q_method_t)DS_select);
   QC_DATASOURCE->addMethod("selectRow",         (q_method_t)DS_selectRow);
   QC_DATASOURCE->addMethod("selectRows",        (q_method_t)DS_selectRows);
   QC_DATASOURCE->addMethod("vexec",             (q_method_t)DS_vexec);
   QC_DATASOURCE->addMethod("vselect",           (q_method_t)DS_vselect);
   QC_DATASOURCE->addMethod("vselectRow",        (q_method_t)DS_vselectRow);
   QC_DATASOURCE->addMethod("vselectRows",       (q_method_t)DS_vselectRows);
   //QC_DATASOURCE->addMethod("describe",          (q_method_t)DS_describe);
   QC_DATASOURCE->addMethod("beginTransaction",  (q_method_t)DS_beginTransaction);
   QC_DATASOURCE->addMethod("reset",             (q_method_t)DS_reset);
   QC_DATASOURCE->addMethod("getCapabilities",   (q_method_t)DS_getCapabilities);
   QC_DATASOURCE->addMethod("getCapabilityList", (q_method_t)DS_getCapabilityList);
   QC_DATASOURCE->addMethod("setAutoCommit",     (q_method_t)DS_setAutoCommit);
   QC_DATASOURCE->addMethod("setUserName",       (q_method_t)DS_setUserName);
   QC_DATASOURCE->addMethod("setPassword",       (q_method_t)DS_setPassword);
   QC_DATASOURCE->addMethod("setDBName",         (q_method_t)DS_setDBName);
   QC_DATASOURCE->addMethod("setDBCharset",      (q_method_t)DS_setDBCharset);
   QC_DATASOURCE->addMethod("setHostName",       (q_method_t)DS_setHostName);
   QC_DATASOURCE->addMethod("getAutoCommit",     (q_method_t)DS_getAutoCommit);
   QC_DATASOURCE->addMethod("getUserName",       (q_method_t)DS_getUserName);
   QC_DATASOURCE->addMethod("getPassword",       (q_method_t)DS_getPassword);
   QC_DATASOURCE->addMethod("getDBName",         (q_method_t)DS_getDBName);
   QC_DATASOURCE->addMethod("getDBCharset",      (q_method_t)DS_getDBCharset);
   QC_DATASOURCE->addMethod("getOSCharset",      (q_method_t)DS_getOSCharset);
   QC_DATASOURCE->addMethod("getHostName",       (q_method_t)DS_getHostName);
   QC_DATASOURCE->addMethod("getDriverName",     (q_method_t)DS_getDriverName);
   QC_DATASOURCE->addMethod("getServerVersion",  (q_method_t)DS_getServerVersion);
   QC_DATASOURCE->addMethod("getClientVersion",  (q_method_t)DS_getClientVersion);
   QC_DATASOURCE->addMethod("inTransaction",     (q_method_t)DS_inTransaction);

   QC_DATASOURCE->addMethod("setTransactionLockTimeout", (q_method_t)DS_setTransactionLockTimeout);
   QC_DATASOURCE->addMethod("getTransactionLockTimeout", (q_method_t)DS_getTransactionLockTimeout);

   traceout("initDatasourceClass()");
   return QC_DATASOURCE;
}
