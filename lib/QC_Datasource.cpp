/*
  QC_Datasource.cpp

  Qore Programming Language

  Copyright 2003 - 2010 David Nichols

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

static const char *DSC_ERR = "DATASOURCE-CONSTRUCTOR-ERROR";

// usage: Datasource(db name, [username, password, dbname, encoding, hostname, port])
static void DS_constructor_str(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p, const QoreStringNode, params, 0);
   DBIDriver *db_driver = DBI.find(p->getBuffer(), xsink);
   if (!db_driver) {
      if (!*xsink)
	 xsink->raiseException("DATASOURCE-UNSUPPORTED-DATABASE", "DBI driver '%s' cannot be loaded", p->getBuffer());
      return;
   }

   ManagedDatasource *ds = new ManagedDatasource(db_driver);

   p = HARD_QORE_STRING(params, 1);
   if (p->strlen())
      ds->setPendingUsername(p->getBuffer());

   p = HARD_QORE_STRING(params, 2);
   if (p->strlen())
      ds->setPendingPassword(p->getBuffer());

   p = HARD_QORE_STRING(params, 3);
   if (p->strlen())
      ds->setPendingDBName(p->getBuffer());
   
   p = HARD_QORE_STRING(params, 4);
   if (p->strlen())
      ds->setPendingDBEncoding(p->getBuffer());
   
   p = HARD_QORE_STRING(params, 5);
   if (p->strlen())
      ds->setPendingHostName(p->getBuffer());
   
   int port = (int)HARD_QORE_INT(params, 6);
   if (port) {
      if (port < 0) {
	 xsink->raiseException(DSC_ERR, "port value must be zero (meaning use the default port) or positive (value given: %d)", port);
	 return;
      }
      ds->setPendingPort(port);
   }

   self->setPrivate(CID_DATASOURCE, ds);
}

static void DS_constructor_hash(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreHashNode *h = HARD_QORE_HASH(params, 0);
   
   const char *str = check_hash_key(h, "type", DSC_ERR, xsink);
   if (*xsink) return;
   
   if (!str) {
      xsink->raiseException(DSC_ERR, "expecting a string value with the 'type' key giving the driver name");
      return;
   }
   
   DBIDriver *db_driver = DBI.find(str, xsink);
   if (!db_driver) {
      if (!*xsink)
         xsink->raiseException("DATASOURCE-UNSUPPORTED-DATABASE", "DBI driver '%s' cannot be loaded", str);
      return;
   }
   
   ReferenceHolder<ManagedDatasource> ds(new ManagedDatasource(db_driver), xsink);

   str = check_hash_key(h, "user", DSC_ERR, xsink);
   if (*xsink) return;
   if (str) ds->setPendingUsername(str);

   str = check_hash_key(h, "pass", DSC_ERR, xsink);
   if (*xsink) return;
   if (str) ds->setPendingPassword(str);

   str = check_hash_key(h, "db", DSC_ERR, xsink);
   if (*xsink) return;
   if (str) ds->setPendingDBName(str);

   str = check_hash_key(h, "charset", DSC_ERR, xsink);
   if (*xsink) return;
   if (str) ds->setPendingDBEncoding(str);

   str = check_hash_key(h, "host", DSC_ERR, xsink);
   if (*xsink) return;
   if (str) ds->setPendingHostName(str);

   bool found;
   int port = (int)h->getKeyAsBigInt("port", found);
   if (port) {
      if (port < 0) {
	 xsink->raiseException(DSC_ERR, "port value must be zero (meaning use the default port) or positive (value given: %d)", port);
	 return;
      }

      ds->setPendingPort(port);
   }

   self->setPrivate(CID_DATASOURCE, ds.release());
}

static void DS_destructor(QoreObject *self, ManagedDatasource *ods, ExceptionSink *xsink) {
   ods->destructor(xsink);
   ods->deref(xsink);
}

static void DS_copy(QoreObject *self, QoreObject *old, ManagedDatasource *ods, ExceptionSink *xsink) {
   self->setPrivate(CID_DATASOURCE, ods->copy());
}

static AbstractQoreNode *DS_open(QoreObject *self, ManagedDatasource *ds, const QoreListNode *params, ExceptionSink *xsink) {
   ds->open(xsink);
   return 0;
}

static AbstractQoreNode *DS_close(QoreObject *self, ManagedDatasource *ds, const QoreListNode *params, ExceptionSink *xsink) {
   ds->close(xsink);
   return 0;
}

static AbstractQoreNode *DS_commit(QoreObject *self, ManagedDatasource *ds, const QoreListNode *params, ExceptionSink *xsink) {
   ds->commit(xsink);
   return 0;
}

static AbstractQoreNode *DS_rollback(QoreObject *self, ManagedDatasource *ds, const QoreListNode *params, ExceptionSink *xsink) {
   ds->rollback(xsink);
   return 0;
}

static AbstractQoreNode *DS_setAutoCommit_bool(QoreObject *self, ManagedDatasource *ds, const QoreListNode *params, ExceptionSink *xsink) {
   ds->setAutoCommit(HARD_QORE_BOOL(params, 0), xsink);
   return 0;
}

static AbstractQoreNode *DS_getAutoCommit(QoreObject *self, ManagedDatasource *ds, const QoreListNode *params, ExceptionSink *xsink) {
   return get_bool_node(ds->getAutoCommit());
}

static AbstractQoreNode *DS_exec(QoreObject *self, ManagedDatasource *ds, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(params, 0);
   ReferenceHolder<QoreListNode> args((params->size() > 1 ? params->copyListFrom(1) : 0), xsink);
   return ds->exec(p0, *args, xsink);
}

static AbstractQoreNode *DS_vexec(QoreObject *self, ManagedDatasource *ds, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(params, 0);
   const QoreListNode *args = test_list_param(params, 1);
   return ds->exec(p0, args, xsink);
}

static AbstractQoreNode *DS_execRaw(QoreObject *self, ManagedDatasource *ds, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(params, 0);
   return ds->execRaw(p0, xsink);
}

static AbstractQoreNode *DS_select(QoreObject *self, ManagedDatasource *ds, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p = HARD_QORE_STRING(params, 0);
   ReferenceHolder<QoreListNode> args((params->size() > 1 ? params->copyListFrom(1) : 0), xsink);
   return ds->select(p, *args, xsink);
}

static AbstractQoreNode *DS_selectRow(QoreObject *self, ManagedDatasource *ds, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p = HARD_QORE_STRING(params, 0);
   ReferenceHolder<QoreListNode> args((params->size() > 1 ? params->copyListFrom(1) : 0), xsink);
   return ds->selectRow(p, *args, xsink);
}

static AbstractQoreNode *DS_selectRows(QoreObject *self, ManagedDatasource *ds, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p = HARD_QORE_STRING(params, 0);
   ReferenceHolder<QoreListNode> args((params->size() > 1 ? params->copyListFrom(1) : 0), xsink);
   return ds->selectRows(p, *args, xsink);
}

static AbstractQoreNode *DS_vselect(QoreObject *self, ManagedDatasource *ds, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(params, 0);
   const QoreListNode *args = test_list_param(params, 1);
   return ds->select(p0, args, xsink);
}

static AbstractQoreNode *DS_vselectRow(QoreObject *self, ManagedDatasource *ds, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(params, 0);
   const QoreListNode *args = test_list_param(params, 1);
   return ds->selectRow(p0, args, xsink);
}

static AbstractQoreNode *DS_vselectRows(QoreObject *self, ManagedDatasource *ds, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(params, 0);
   const QoreListNode *args = test_list_param(params, 1);
   return ds->selectRows(p0, args, xsink);
}

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
   const QoreStringNode *p = HARD_QORE_STRING(params, 0);
   ds->setPendingUsername(p->getBuffer());
   return 0;
}

static AbstractQoreNode *DS_setPassword(QoreObject *self, ManagedDatasource *ds, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p = HARD_QORE_STRING(params, 0);
   ds->setPendingPassword(p->getBuffer());
   return 0;
}

static AbstractQoreNode *DS_setDBName(QoreObject *self, ManagedDatasource *ds, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p = HARD_QORE_STRING(params, 0);
   ds->setPendingDBName(p->getBuffer());
   return 0;
}

static AbstractQoreNode *DS_setDBCharset(QoreObject *self, ManagedDatasource *ds, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p = HARD_QORE_STRING(params, 0);
   ds->setPendingDBEncoding(p->getBuffer());
   return 0;
}

static AbstractQoreNode *DS_setHostName(QoreObject *self, ManagedDatasource *ds, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p = HARD_QORE_STRING(params, 0);
   ds->setPendingHostName(p->getBuffer());
   return 0;
}

static AbstractQoreNode *DS_setPort(QoreObject *self, ManagedDatasource *ds, const QoreListNode *params, ExceptionSink *xsink) {
   ds->setPendingPort((int)HARD_QORE_INT(params, 0));
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

// Datasource::getPort() returns *int
static AbstractQoreNode *DS_getPort(QoreObject *self, ManagedDatasource *ds, const QoreListNode *params, ExceptionSink *xsink) {
   int port = ds->getPort();
   return port ? new QoreBigIntNode(port) : 0;
}

// Datasource::setTransactionLockTimeout(timeout $timeout_ms = 0) returns nothing
static AbstractQoreNode *DS_setTransactionLockTimeout(QoreObject *self, ManagedDatasource *ds, const QoreListNode *params, ExceptionSink *xsink) {
   ds->setTransactionLockTimeout((int)HARD_QORE_INT(params, 0));
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

   QC_DATASOURCE->setConstructorExtended(DS_constructor_str, false, QC_NO_FLAGS, QDOM_DEFAULT, 7, stringTypeInfo, QORE_PARAM_NO_ARG, stringTypeInfo, null_string(), stringTypeInfo, null_string(), stringTypeInfo, null_string(), stringTypeInfo, null_string(), stringTypeInfo, null_string(), softBigIntTypeInfo, zero());

   QC_DATASOURCE->setConstructorExtended(DS_constructor_hash, false, QC_NO_FLAGS, QDOM_DEFAULT, 1, hashTypeInfo, QORE_PARAM_NO_ARG);

   QC_DATASOURCE->setDestructor((q_destructor_t)DS_destructor);
   QC_DATASOURCE->setCopy((q_copy_t)DS_copy);
   QC_DATASOURCE->addMethodExtended("open",              (q_method_t)DS_open, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo);
   QC_DATASOURCE->addMethodExtended("close",             (q_method_t)DS_close, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo);
   QC_DATASOURCE->addMethodExtended("commit",            (q_method_t)DS_commit, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo);
   QC_DATASOURCE->addMethodExtended("rollback",          (q_method_t)DS_rollback, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo);

   QC_DATASOURCE->addMethodExtended("setAutoCommit",     (q_method_t)DS_setAutoCommit_bool, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 1, softBoolTypeInfo, &True);

   QC_DATASOURCE->addMethodExtended("getAutoCommit",     (q_method_t)DS_getAutoCommit, false, QC_NO_FLAGS, QDOM_DEFAULT, boolTypeInfo);

   QC_DATASOURCE->addMethodExtended("exec",              (q_method_t)class_noop, false, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   QC_DATASOURCE->addMethodExtended("exec",              (q_method_t)DS_exec, false, QC_USES_EXTRA_ARGS, QDOM_DEFAULT, anyTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   QC_DATASOURCE->addMethodExtended("vexec",             (q_method_t)class_noop, false, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   QC_DATASOURCE->addMethodExtended("vexec",             (q_method_t)DS_vexec, false, QC_NO_FLAGS, QDOM_DEFAULT, anyTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   QC_DATASOURCE->addMethodExtended("vexec",             (q_method_t)DS_vexec, false, QC_NO_FLAGS, QDOM_DEFAULT, anyTypeInfo, 2, stringTypeInfo, QORE_PARAM_NO_ARG, listTypeInfo, QORE_PARAM_NO_ARG);

   QC_DATASOURCE->addMethodExtended("execRaw",           (q_method_t)DS_execRaw, false, QC_NO_FLAGS, QDOM_DEFAULT, anyTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   // should normally return a hash, but unfortunately the internal API does not enforce this
   QC_DATASOURCE->addMethodExtended("select",            (q_method_t)class_noop, false, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   QC_DATASOURCE->addMethodExtended("select",            (q_method_t)DS_select, false, QC_USES_EXTRA_ARGS, QDOM_DEFAULT, anyTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   // should normally return a hash, but unfortunately the internal API does not enforce this
   QC_DATASOURCE->addMethodExtended("selectRow",         (q_method_t)class_noop, false, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   QC_DATASOURCE->addMethodExtended("selectRow",         (q_method_t)DS_selectRow, false, QC_USES_EXTRA_ARGS, QDOM_DEFAULT, anyTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   // should normally return a list, but unfortunately the internal API does not enforce this
   QC_DATASOURCE->addMethodExtended("selectRows",        (q_method_t)class_noop, false, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   QC_DATASOURCE->addMethodExtended("selectRows",        (q_method_t)DS_selectRows, false, QC_USES_EXTRA_ARGS, QDOM_DEFAULT, anyTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   
   // should normally return a hash, but unfortunately the internal API does not enforce this
   QC_DATASOURCE->addMethodExtended("vselect",           (q_method_t)class_noop, false, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   QC_DATASOURCE->addMethodExtended("vselect",           (q_method_t)DS_vselect, false, QC_NO_FLAGS, QDOM_DEFAULT, anyTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   QC_DATASOURCE->addMethodExtended("vselect",           (q_method_t)DS_vselect, false, QC_NO_FLAGS, QDOM_DEFAULT, anyTypeInfo, 2, stringTypeInfo, QORE_PARAM_NO_ARG, listTypeInfo, QORE_PARAM_NO_ARG);

   // should normally return a hash, but unfortunately the internal API does not enforce this
   QC_DATASOURCE->addMethodExtended("vselectRow",        (q_method_t)class_noop, false, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   QC_DATASOURCE->addMethodExtended("vselectRow",        (q_method_t)DS_vselectRow, false, QC_NO_FLAGS, QDOM_DEFAULT, anyTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   QC_DATASOURCE->addMethodExtended("vselectRow",        (q_method_t)DS_vselectRow, false, QC_NO_FLAGS, QDOM_DEFAULT, anyTypeInfo, 2, stringTypeInfo, QORE_PARAM_NO_ARG, listTypeInfo, QORE_PARAM_NO_ARG);

   // should normally return a list, but unfortunately the internal API does not enforce this
   QC_DATASOURCE->addMethodExtended("vselectRows",       (q_method_t)class_noop, false, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   QC_DATASOURCE->addMethodExtended("vselectRows",       (q_method_t)DS_vselectRows, false, QC_NO_FLAGS, QDOM_DEFAULT, anyTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   QC_DATASOURCE->addMethodExtended("vselectRows",       (q_method_t)DS_vselectRows, false, QC_NO_FLAGS, QDOM_DEFAULT, anyTypeInfo, 2, stringTypeInfo, QORE_PARAM_NO_ARG, listTypeInfo, QORE_PARAM_NO_ARG);

   QC_DATASOURCE->addMethodExtended("beginTransaction",  (q_method_t)DS_beginTransaction, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo);
   QC_DATASOURCE->addMethodExtended("reset",             (q_method_t)DS_reset, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo);
   QC_DATASOURCE->addMethodExtended("getCapabilities",   (q_method_t)DS_getCapabilities, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo);
   QC_DATASOURCE->addMethodExtended("getCapabilityList", (q_method_t)DS_getCapabilityList, false, QC_NO_FLAGS, QDOM_DEFAULT, listTypeInfo);

   QC_DATASOURCE->addMethodExtended("setUserName",       (q_method_t)class_noop, false, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   QC_DATASOURCE->addMethodExtended("setUserName",       (q_method_t)DS_setUserName, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   
   QC_DATASOURCE->addMethodExtended("setPassword",       (q_method_t)class_noop, false, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   QC_DATASOURCE->addMethodExtended("setPassword",       (q_method_t)DS_setPassword, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   
   QC_DATASOURCE->addMethodExtended("setDBName",         (q_method_t)class_noop, false, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   QC_DATASOURCE->addMethodExtended("setDBName",         (q_method_t)DS_setDBName, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   
   QC_DATASOURCE->addMethodExtended("setDBCharset",      (q_method_t)class_noop, false, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   QC_DATASOURCE->addMethodExtended("setDBCharset",      (q_method_t)DS_setDBCharset, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   
   QC_DATASOURCE->addMethodExtended("setHostName",       (q_method_t)class_noop, false, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   QC_DATASOURCE->addMethodExtended("setHostName",       (q_method_t)DS_setHostName, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   
   QC_DATASOURCE->addMethodExtended("setPort",           (q_method_t)DS_setPort, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 1, softBigIntTypeInfo, zero());

   // returns *string
   QC_DATASOURCE->addMethodExtended("getUserName",       (q_method_t)DS_getUserName, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, stringOrNothingTypeInfo);
   // returns *string
   QC_DATASOURCE->addMethodExtended("getPassword",       (q_method_t)DS_getPassword, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, stringOrNothingTypeInfo);
   // returns *string
   QC_DATASOURCE->addMethodExtended("getDBName",         (q_method_t)DS_getDBName, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, stringOrNothingTypeInfo);
   // returns *string
   QC_DATASOURCE->addMethodExtended("getDBCharset",      (q_method_t)DS_getDBCharset, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, stringOrNothingTypeInfo);
   // returns *string
   QC_DATASOURCE->addMethodExtended("getOSCharset",      (q_method_t)DS_getOSCharset, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, stringOrNothingTypeInfo);
   // returns *string
   QC_DATASOURCE->addMethodExtended("getHostName",       (q_method_t)DS_getHostName, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, stringOrNothingTypeInfo);

   // Datasource::getPort() returns *int
   QC_DATASOURCE->addMethodExtended("getPort",           (q_method_t)DS_getPort, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, bigIntOrNothingTypeInfo);

   // Datasource::setTransactionLockTimeout(timeout $timeout_ms = 0) returns nothing
   QC_DATASOURCE->addMethodExtended("setTransactionLockTimeout", (q_method_t)DS_setTransactionLockTimeout, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 1, timeoutTypeInfo, zero());

   QC_DATASOURCE->addMethodExtended("getTransactionLockTimeout", (q_method_t)DS_getTransactionLockTimeout, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, bigIntTypeInfo);

   QC_DATASOURCE->addMethodExtended("getDriverName",     (q_method_t)DS_getDriverName, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, stringTypeInfo);

   QC_DATASOURCE->addMethodExtended("getServerVersion",  (q_method_t)DS_getServerVersion, false, QC_NO_FLAGS, QDOM_DEFAULT);
   QC_DATASOURCE->addMethodExtended("getClientVersion",  (q_method_t)DS_getClientVersion, false, QC_NO_FLAGS, QDOM_DEFAULT);
   QC_DATASOURCE->addMethodExtended("inTransaction",     (q_method_t)DS_inTransaction, false, QC_NO_FLAGS, QDOM_DEFAULT, boolTypeInfo);

   return QC_DATASOURCE;
}
