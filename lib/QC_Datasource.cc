/*
  QC_Datasource.cc

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
#include <qore/intern/QC_Datasource.h>

qore_classid_t CID_DATASOURCE;

// usage: Datasource(db name, [username, password, dbname])
static void DS_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p = test_string_param(params, 0);
   if (!p || !p->strlen()) {
      xsink->raiseException("DATASOURCE-PARAMETER-ERROR", "expecting database driver name as first parameter of Datasource() constructor");
      return;
   }
   DBIDriver *db_driver = DBI.find(p->getBuffer(), xsink);
   if (!db_driver) {
      if (!*xsink)
	 xsink->raiseException("DATASOURCE-UNSUPPORTED-DATABASE", "DBI driver '%s' cannot be loaded", p->getBuffer());
      return;
   }

   ManagedDatasource *ds = new ManagedDatasource(db_driver);

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
   
   int port = get_int_param(params, 6);
   if (port)
      ds->setPendingPort(port);

   self->setPrivate(CID_DATASOURCE, ds);
}

static void DS_destructor(QoreObject *self, ManagedDatasource *ods, ExceptionSink *xsink) {
   ods->destructor(xsink);
   ods->deref(xsink);
}

static void DS_copy(QoreObject *self, QoreObject *old, ManagedDatasource *ods, ExceptionSink *xsink) {
   self->setPrivate(CID_DATASOURCE, ods->copy());
}

static AbstractQoreNode *DS_open(QoreObject *self, ManagedDatasource *ds, const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreBigIntNode(ds->open(xsink));
}

static AbstractQoreNode *DS_close(QoreObject *self, ManagedDatasource *ds, const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreBigIntNode(ds->close(xsink));
}

static AbstractQoreNode *DS_commit(QoreObject *self, ManagedDatasource *ds, const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreBigIntNode(ds->commit(xsink));
}

static AbstractQoreNode *DS_rollback(QoreObject *self, ManagedDatasource *ds, const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreBigIntNode(ds->rollback(xsink));
}

static AbstractQoreNode *DS_setAutoCommit(QoreObject *self, ManagedDatasource *ds, const QoreListNode *params, ExceptionSink *xsink) {
   const AbstractQoreNode *p = get_param(params, 0);
   ds->setAutoCommit(p ? p->getAsBool() : true);
   return 0;
}

static AbstractQoreNode *DS_getAutoCommit(QoreObject *self, ManagedDatasource *ds, const QoreListNode *params, ExceptionSink *xsink) {
   return get_bool_node(ds->getAutoCommit());
}

static AbstractQoreNode *DS_exec(QoreObject *self, ManagedDatasource *ds, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0;
   if (!(p0 = test_string_param(params, 0)))
      return 0;

   ReferenceHolder<QoreListNode> args((params->size() > 1 ? params->copyListFrom(1) : 0), xsink);
   return ds->exec(p0, *args, xsink);
}

static AbstractQoreNode *DS_vexec(QoreObject *self, ManagedDatasource *ds, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0;
   if (!(p0 = test_string_param(params, 0)))
      return 0;

   const QoreListNode *args = test_list_param(params, 1);
   return ds->exec(p0, args, xsink);
}

static AbstractQoreNode *DS_select(QoreObject *self, ManagedDatasource *ds, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p = test_string_param(params, 0);
   if (!p)
      return 0;

   ReferenceHolder<QoreListNode> args((params->size() > 1 ? params->copyListFrom(1) : 0), xsink);
   return ds->select(p, *args, xsink);
}

static AbstractQoreNode *DS_selectRow(QoreObject *self, ManagedDatasource *ds, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p = test_string_param(params, 0);
   if (!p)
      return 0;
   
   ReferenceHolder<QoreListNode> args((params->size() > 1 ? params->copyListFrom(1) : 0), xsink);
   return ds->selectRow(p, *args, xsink);
}

static AbstractQoreNode *DS_selectRows(QoreObject *self, ManagedDatasource *ds, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p = test_string_param(params, 0);
   if (!p)
      return 0;

   ReferenceHolder<QoreListNode> args((params->size() > 1 ? params->copyListFrom(1) : 0), xsink);
   return ds->selectRows(p, *args, xsink);
}

static AbstractQoreNode *DS_vselect(QoreObject *self, ManagedDatasource *ds, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = test_string_param(params, 0);
   if (!p0)
      return 0;

   const QoreListNode *args = test_list_param(params, 1);
   return ds->select(p0, args, xsink);
}

static AbstractQoreNode *DS_vselectRow(QoreObject *self, ManagedDatasource *ds, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = test_string_param(params, 0);
   if (!p0)
      return 0;
   
   const QoreListNode *args = test_list_param(params, 1);
   return ds->selectRow(p0, args, xsink);
}

static AbstractQoreNode *DS_vselectRows(QoreObject *self, ManagedDatasource *ds, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = test_string_param(params, 0);
   if (!p0)
      return 0;

   const QoreListNode *args = test_list_param(params, 1);
   return ds->selectRows(p0, args, xsink);
}

/*
static AbstractQoreNode *DS_describe(QoreObject *self, ManagedDatasource *ds, const QoreListNode *params, ExceptionSink *xsink) {
   const Abstracconst tQoreNode *p = test_string_param(params, 0);
   if (!p)
      return 0;

   return ds->describe(p->getBuffer(), xsink);
}
*/

static AbstractQoreNode *DS_beginTransaction(QoreObject *self, ManagedDatasource *ds, const QoreListNode *params, ExceptionSink *xsink) {
   ds->beginTransaction(xsink);
   return 0;
}

static AbstractQoreNode *DS_reset(QoreObject *self, ManagedDatasource *ds, const QoreListNode *params, ExceptionSink *xsink) {
   ds->reset(xsink);
   return 0;
}

static AbstractQoreNode *DS_getCapabilities(QoreObject *self, ManagedDatasource *ds, const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreBigIntNode(ds->getCapabilities());
}

static AbstractQoreNode *DS_getCapabilityList(QoreObject *self, ManagedDatasource *ds, const QoreListNode *params, ExceptionSink *xsink) {
   return ds->getCapabilityList();
}

static AbstractQoreNode *DS_setUserName(QoreObject *self, ManagedDatasource *ds, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p = test_string_param(params, 0);
   if (!p)
      return 0;

   ds->setPendingUsername(p->getBuffer());
   return 0;
}

static AbstractQoreNode *DS_setPassword(QoreObject *self, ManagedDatasource *ds, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p = test_string_param(params, 0);
   if (!p)
      return 0;

   ds->setPendingPassword(p->getBuffer());
   return 0;
}

static AbstractQoreNode *DS_setDBName(QoreObject *self, ManagedDatasource *ds, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p = test_string_param(params, 0);
   if (!p)
      return 0;

   ds->setPendingDBName(p->getBuffer());
   return 0;
}

static AbstractQoreNode *DS_setDBCharset(QoreObject *self, ManagedDatasource *ds, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p = test_string_param(params, 0);
   if (!p)
      return 0;

   ds->setPendingDBEncoding(p->getBuffer());
   return 0;
}

static AbstractQoreNode *DS_setHostName(QoreObject *self, ManagedDatasource *ds, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p = test_string_param(params, 0);
   if (!p)
      return 0;

   ds->setPendingHostName(p->getBuffer());
   return 0;
}

static AbstractQoreNode *DS_setPort(QoreObject *self, ManagedDatasource *ds, const QoreListNode *params, ExceptionSink *xsink) {
   ds->setPendingPort(get_int_param(params, 0));
   return 0;
}

static AbstractQoreNode *DS_getUserName(QoreObject *self, ManagedDatasource *ds, const QoreListNode *params, ExceptionSink *xsink) {
   return ds->getPendingUsername();
}

static AbstractQoreNode *DS_getPassword(QoreObject *self, ManagedDatasource *ds, const QoreListNode *params, ExceptionSink *xsink) {
   return ds->getPendingPassword();
}

static AbstractQoreNode *DS_getDBName(QoreObject *self, ManagedDatasource *ds, const QoreListNode *params, ExceptionSink *xsink) {
   return ds->getPendingDBName();
}

static AbstractQoreNode *DS_getDBCharset(QoreObject *self, ManagedDatasource *ds, const QoreListNode *params, ExceptionSink *xsink) {
   return ds->getPendingDBEncoding();
}

static AbstractQoreNode *DS_getOSCharset(QoreObject *self, ManagedDatasource *ds, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreEncoding *enc = ds->getQoreEncoding();
   return new QoreStringNode(enc ? enc->getCode() : "(unknown)");
}

static AbstractQoreNode *DS_getHostName(QoreObject *self, ManagedDatasource *ds, const QoreListNode *params, ExceptionSink *xsink) {
   return ds->getPendingHostName();
}

static AbstractQoreNode *DS_getPort(QoreObject *self, ManagedDatasource *ds, const QoreListNode *params, ExceptionSink *xsink) {
   int port = ds->getPort();
   return port ? new QoreBigIntNode(port) : 0;
}

static AbstractQoreNode *DS_setTransactionLockTimeout(QoreObject *self, ManagedDatasource *ds, const QoreListNode *params, ExceptionSink *xsink) {
   ds->setTransactionLockTimeout(getMsZeroInt(get_param(params, 0)));
   return 0;
}

static AbstractQoreNode *DS_getTransactionLockTimeout(QoreObject *self, ManagedDatasource *ds, const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreBigIntNode(ds->getTransactionLockTimeout());
}

static AbstractQoreNode *DS_getDriverName(QoreObject *self, ManagedDatasource *ds, const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreStringNode(ds->getDriver()->getName());
}

static AbstractQoreNode *DS_getServerVersion(QoreObject *self, ManagedDatasource *ds, const QoreListNode *params, ExceptionSink *xsink) {
   return ds->getServerVersion(xsink);
}

static AbstractQoreNode *DS_getClientVersion(QoreObject *self, ManagedDatasource *ds, const QoreListNode *params, ExceptionSink *xsink) {
   return ds->getClientVersion(xsink);
}

static AbstractQoreNode *DS_inTransaction(QoreObject *self, ManagedDatasource *ds, const QoreListNode *params, ExceptionSink *xsink) {
   return get_bool_node(ds->isInTransaction());
}

QoreClass *initDatasourceClass() {
   QORE_TRACE("initDatasourceClass()");

   QoreClass *QC_DATASOURCE = new QoreClass("Datasource", QDOM_DATABASE);
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
   QC_DATASOURCE->addMethod("setPort",           (q_method_t)DS_setPort);
   QC_DATASOURCE->addMethod("getAutoCommit",     (q_method_t)DS_getAutoCommit);
   QC_DATASOURCE->addMethod("getUserName",       (q_method_t)DS_getUserName);
   QC_DATASOURCE->addMethod("getPassword",       (q_method_t)DS_getPassword);
   QC_DATASOURCE->addMethod("getDBName",         (q_method_t)DS_getDBName);
   QC_DATASOURCE->addMethod("getDBCharset",      (q_method_t)DS_getDBCharset);
   QC_DATASOURCE->addMethod("getOSCharset",      (q_method_t)DS_getOSCharset);
   QC_DATASOURCE->addMethod("getHostName",       (q_method_t)DS_getHostName);
   QC_DATASOURCE->addMethod("getPort",           (q_method_t)DS_getPort);
   QC_DATASOURCE->addMethod("getDriverName",     (q_method_t)DS_getDriverName);
   QC_DATASOURCE->addMethod("getServerVersion",  (q_method_t)DS_getServerVersion);
   QC_DATASOURCE->addMethod("getClientVersion",  (q_method_t)DS_getClientVersion);
   QC_DATASOURCE->addMethod("inTransaction",     (q_method_t)DS_inTransaction);

   QC_DATASOURCE->addMethod("setTransactionLockTimeout", (q_method_t)DS_setTransactionLockTimeout);
   QC_DATASOURCE->addMethod("getTransactionLockTimeout", (q_method_t)DS_getTransactionLockTimeout);

   return QC_DATASOURCE;
}
