/*
  QC_DatasourcePool.cpp

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
#include <qore/DBI.h>

#include <qore/intern/QC_DatasourcePool.h>

qore_classid_t CID_DATASOURCEPOOL;

#define DP_MIN 5
#define DP_MAX 20

static const char *DSPC_ERR = "DATASOURCEPOOL-CONSTRUCTOR-ERROR";

// usage: DatasourcePool(db name, [username, password, dbname, charset, hostname, min, max])
static void DSP_constructor_str(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p, const QoreStringNode, params, 0);
   DBIDriver *db_driver = DBI.find(p->getBuffer());
   if (!db_driver) {
      xsink->raiseException("DATASOURCEPOOL-UNSUPPORTED-DATABASE", "no DBI driver can be found for database type '%s'", p->getBuffer());
      return;
   }

   const char *user = 0, *pass = 0, *db = 0, *charset = 0, *host = 0;
   p = HARD_QORE_STRING(params, 1);
   if (p->strlen())
      user = p->getBuffer();

   p = HARD_QORE_STRING(params, 2);
   if (p->strlen())
      pass = p->getBuffer();

   p = HARD_QORE_STRING(params, 3);
   if (p->strlen())
      db = p->getBuffer();
   
   p = HARD_QORE_STRING(params, 4);
   if (p->strlen())
      charset = p->getBuffer();

   p = HARD_QORE_STRING(params, 5);
   if (p->strlen())
      host = p->getBuffer();

   int min = (int)HARD_QORE_INT(params, 6);
   if (min <= 0) {
      xsink->raiseException(DSPC_ERR, "minimum connections must be > 0 (value given: %d)", min);
      return;
   }

   int max = (int)HARD_QORE_INT(params, 7);
   if (max < min) {
      xsink->raiseException(DSPC_ERR, "maximum connections must be >= min(%d) (value given: %d)", min, max);
      return;
   }
   
   int port = (int)HARD_QORE_INT(params, 8);
   if (port < 0) {
      xsink->raiseException(DSPC_ERR, "port value must be zero (meaning use the default port) or positive (value given: %d)", port);
      return;
   }

   SimpleRefHolder<DatasourcePool> ds(new DatasourcePool(db_driver, user, pass, db, charset, host, min, max, port, xsink));
   if (!*xsink)
      self->setPrivate(CID_DATASOURCEPOOL, ds.release());
}

// usage: DatasourcePool(type=<string>, [name=<string>], [pass=<string>], [db=<string>], [charset=<string>], [host=<string>], [min=<int>], [max=<int>], [port=<int>])
static void DSP_constructor_hash(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreHashNode *h = HARD_QORE_HASH(params, 0);
   
   const char *str = check_hash_key(h, "type", DSPC_ERR, xsink);
   if (*xsink) return;
   
   if (!str) {
      xsink->raiseException(DSPC_ERR, "expecting a string value with the 'type' key giving the driver name");
      return;
   }

   DBIDriver *db_driver = DBI.find(str);
   if (!db_driver) {
      if (!*xsink)
	 xsink->raiseException("DATASOURCEPOOL-UNSUPPORTED-DATABASE", "no DBI driver can be found for database type '%s'", str);
      return;
   }

   const char *user = check_hash_key(h, "user", DSPC_ERR, xsink);
   if (*xsink) return;

   const char *pass = check_hash_key(h, "pass", DSPC_ERR, xsink);
   if (*xsink) return;

   const char *db = check_hash_key(h, "db", DSPC_ERR, xsink);
   if (*xsink) return;

   const char *charset = check_hash_key(h, "charset", DSPC_ERR, xsink);
   if (*xsink) return;

   const char *host = check_hash_key(h, "host", DSPC_ERR, xsink);
   if (*xsink) return;

   bool found;
   int port = (int)h->getKeyAsBigInt("port", found);
   if (port < 0) {
      xsink->raiseException(DSPC_ERR, "port value must be zero (meaning use the default port) or positive (value given: %d)", port);
      return;
   }

   // get options
   int min = 0, max = 0;

   const AbstractQoreNode *p = h->getKeyValue("options");
   if (!is_nothing(p)) {
      if (p->getType() != NT_HASH) {
	 xsink->raiseException(DSPC_ERR, "'options' key is not hash, instead got type '%s'", p->getTypeName());
	 return;
      }
     
      h = reinterpret_cast<const QoreHashNode *>(p);

      min = (int)h->getKeyAsBigInt("min", found);
      if (found) {
	 if (min < 0) {
	    xsink->raiseException(DSPC_ERR, "minimum connections must be > 0 (value given: %d)", min);
	    return;
	 }
      }

      max = (int)h->getKeyAsBigInt("max", found);
      if (found ) {
	 if (max < min) {
	    xsink->raiseException(DSPC_ERR, "maximum connections must be >= min(%d) (value given: %d)", min, max);
	    return;
	 }
      }
   }

   if (!min)
      min = DP_MIN;
   
   if (!max)
      max = DP_MAX;
   
   SimpleRefHolder<DatasourcePool> ds(new DatasourcePool(db_driver, user, pass, db, charset, host, min, max, port, xsink));
   if (!*xsink)
      self->setPrivate(CID_DATASOURCEPOOL, ds.release());
}

static void DSP_destructor(QoreObject *self, DatasourcePool *ds, ExceptionSink *xsink) {
   ds->destructor(xsink);
   ds->deref();
}

static void DSP_copy(QoreObject *self, QoreObject *old, DatasourcePool *ods, ExceptionSink *xsink) {
   xsink->raiseException("COPY-ERROR", "DatasourcePool objects may not be copied");
}

static AbstractQoreNode *DSP_commit(QoreObject *self, DatasourcePool *ds, const QoreListNode *params, ExceptionSink *xsink) {
   ds->commit(xsink);
   return 0;
}

static AbstractQoreNode *DSP_rollback(QoreObject *self, DatasourcePool *ds, const QoreListNode *params, ExceptionSink *xsink) {
   ds->rollback(xsink);
   return 0;
}

static AbstractQoreNode *DSP_exec(QoreObject *self, DatasourcePool *ds, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(params, 0);
   ReferenceHolder<QoreListNode> args(params->size() > 1 ? params->copyListFrom(1) : 0, xsink);
   return ds->exec(p0, *args, xsink);
}

static AbstractQoreNode *DSP_vexec(QoreObject *self, DatasourcePool *ds, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(params, 0);
   const QoreListNode *args = test_list_param(params, 1);
   return ds->exec(p0, args, xsink);
}

static AbstractQoreNode *DSP_execRaw(QoreObject *self, DatasourcePool *ds, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(params, 0);
   return ds->execRaw(p0, xsink);
}

static AbstractQoreNode *DSP_select(QoreObject *self, DatasourcePool *ds, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p = HARD_QORE_STRING(params, 0);
   ReferenceHolder<QoreListNode> args(params->size() > 1 ? params->copyListFrom(1) : 0, xsink);
   return ds->select(p, *args, xsink);
}

static AbstractQoreNode *DSP_selectRow(QoreObject *self, DatasourcePool *ds, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p = HARD_QORE_STRING(params, 0);
   ReferenceHolder<QoreListNode> args(params->size() > 1 ? params->copyListFrom(1) : 0, xsink);
   return ds->selectRow(p, *args, xsink);
}

static AbstractQoreNode *DSP_selectRows(QoreObject *self, DatasourcePool *ds, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p = HARD_QORE_STRING(params, 0);
   ReferenceHolder<QoreListNode> args(params->size() > 1 ? params->copyListFrom(1) : 0, xsink);
   return ds->selectRows(p, *args, xsink);
}

static AbstractQoreNode *DSP_vselect(QoreObject *self, DatasourcePool *ds, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(params, 0);
   const QoreListNode *args = test_list_param(params, 1);
   return ds->select(p0, args, xsink);
}

static AbstractQoreNode *DSP_vselectRow(QoreObject *self, DatasourcePool *ds, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(params, 0);
   const QoreListNode *args = test_list_param(params, 1);
   return ds->selectRow(p0, args, xsink);
}

static AbstractQoreNode *DSP_vselectRows(QoreObject *self, DatasourcePool *ds, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(params, 0);
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

   QC_DATASOURCEPOOL->setConstructorExtended(DSP_constructor_str, false, QC_NO_FLAGS, QDOM_DEFAULT, 9, stringTypeInfo, QORE_PARAM_NO_ARG, stringTypeInfo, null_string(), stringTypeInfo, null_string(), stringTypeInfo, null_string(), stringTypeInfo, null_string(), stringTypeInfo, null_string(), softBigIntTypeInfo, new QoreBigIntNode(DP_MIN), softBigIntTypeInfo, new QoreBigIntNode(DP_MAX), softBigIntTypeInfo, zero());

   QC_DATASOURCEPOOL->setConstructorExtended(DSP_constructor_hash, false, QC_NO_FLAGS, QDOM_DEFAULT, 1, hashTypeInfo, QORE_PARAM_NO_ARG);

   QC_DATASOURCEPOOL->setDestructor((q_destructor_t)DSP_destructor);
   QC_DATASOURCEPOOL->setCopy((q_copy_t)DSP_copy);

   QC_DATASOURCEPOOL->addMethodExtended("commit",            (q_method_t)DSP_commit, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo);
   QC_DATASOURCEPOOL->addMethodExtended("rollback",          (q_method_t)DSP_rollback, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo);

   QC_DATASOURCEPOOL->addMethodExtended("exec",              (q_method_t)class_noop, false, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   QC_DATASOURCEPOOL->addMethodExtended("exec",              (q_method_t)DSP_exec, false, QC_USES_EXTRA_ARGS, QDOM_DEFAULT, anyTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   QC_DATASOURCEPOOL->addMethodExtended("vexec",             (q_method_t)class_noop, false, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   QC_DATASOURCEPOOL->addMethodExtended("vexec",             (q_method_t)DSP_vexec, false, QC_NO_FLAGS, QDOM_DEFAULT, anyTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   QC_DATASOURCEPOOL->addMethodExtended("vexec",             (q_method_t)DSP_vexec, false, QC_NO_FLAGS, QDOM_DEFAULT, anyTypeInfo, 2, stringTypeInfo, QORE_PARAM_NO_ARG, listTypeInfo, QORE_PARAM_NO_ARG);

   QC_DATASOURCEPOOL->addMethodExtended("execRaw",           (q_method_t)DSP_execRaw, false, QC_NO_FLAGS, QDOM_DEFAULT, anyTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   // should normally return a hash, but unfortunately the internal API does not enforce this
   QC_DATASOURCEPOOL->addMethodExtended("select",            (q_method_t)class_noop, false, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   QC_DATASOURCEPOOL->addMethodExtended("select",            (q_method_t)DSP_select, false, QC_USES_EXTRA_ARGS, QDOM_DEFAULT, anyTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   // should normally return a hash, but unfortunately the internal API does not enforce this
   QC_DATASOURCEPOOL->addMethodExtended("selectRow",         (q_method_t)class_noop, false, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   QC_DATASOURCEPOOL->addMethodExtended("selectRow",         (q_method_t)DSP_selectRow, false, QC_USES_EXTRA_ARGS, QDOM_DEFAULT, anyTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   // should normally return a list, but unfortunately the internal API does not enforce this
   QC_DATASOURCEPOOL->addMethodExtended("selectRows",        (q_method_t)class_noop, false, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   QC_DATASOURCEPOOL->addMethodExtended("selectRows",        (q_method_t)DSP_selectRows, false, QC_USES_EXTRA_ARGS, QDOM_DEFAULT, anyTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   // should normally return a hash, but unfortunately the internal API does not enforce this
   QC_DATASOURCEPOOL->addMethodExtended("vselect",           (q_method_t)class_noop, false, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   QC_DATASOURCEPOOL->addMethodExtended("vselect",           (q_method_t)DSP_vselect, false, QC_NO_FLAGS, QDOM_DEFAULT, anyTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   QC_DATASOURCEPOOL->addMethodExtended("vselect",           (q_method_t)DSP_vselect, false, QC_NO_FLAGS, QDOM_DEFAULT, anyTypeInfo, 2, stringTypeInfo, QORE_PARAM_NO_ARG, listTypeInfo, QORE_PARAM_NO_ARG);

   // should normally return a hash, but unfortunately the internal API does not enforce this
   QC_DATASOURCEPOOL->addMethodExtended("vselectRow",        (q_method_t)class_noop, false, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   QC_DATASOURCEPOOL->addMethodExtended("vselectRow",        (q_method_t)DSP_vselectRow, false, QC_NO_FLAGS, QDOM_DEFAULT, anyTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   QC_DATASOURCEPOOL->addMethodExtended("vselectRow",        (q_method_t)DSP_vselectRow, false, QC_NO_FLAGS, QDOM_DEFAULT, anyTypeInfo, 2, stringTypeInfo, QORE_PARAM_NO_ARG, listTypeInfo, QORE_PARAM_NO_ARG);

   // should normally return a list, but unfortunately the internal API does not enforce this
   QC_DATASOURCEPOOL->addMethodExtended("vselectRows",       (q_method_t)class_noop, false, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   QC_DATASOURCEPOOL->addMethodExtended("vselectRows",       (q_method_t)DSP_vselectRows, false, QC_NO_FLAGS, QDOM_DEFAULT, anyTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   QC_DATASOURCEPOOL->addMethodExtended("vselectRows",       (q_method_t)DSP_vselectRows, false, QC_NO_FLAGS, QDOM_DEFAULT, anyTypeInfo, 2, stringTypeInfo, QORE_PARAM_NO_ARG, listTypeInfo, QORE_PARAM_NO_ARG);

   QC_DATASOURCEPOOL->addMethodExtended("beginTransaction",  (q_method_t)DSP_beginTransaction, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo);

   // DatasourcePool::getUserName() returns *string
   QC_DATASOURCEPOOL->addMethodExtended("getUserName",       (q_method_t)DSP_getUserName, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, stringOrNothingTypeInfo);

   // DatasourcePool::getPassword() returns *string
   QC_DATASOURCEPOOL->addMethodExtended("getPassword",       (q_method_t)DSP_getPassword, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, stringOrNothingTypeInfo);

   // DatasourcePool::getDBName() returns *string
   QC_DATASOURCEPOOL->addMethodExtended("getDBName",         (q_method_t)DSP_getDBName, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, stringOrNothingTypeInfo);

   // DatasourcePool::getDBCharset() returns *string
   QC_DATASOURCEPOOL->addMethodExtended("getDBCharset",      (q_method_t)DSP_getDBCharset, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, stringOrNothingTypeInfo);

   // DatasourcePool::getDBCharset() returns *string
   QC_DATASOURCEPOOL->addMethodExtended("getOSCharset",      (q_method_t)DSP_getOSCharset, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, stringOrNothingTypeInfo);

   // DatasourcePool::getHostName() returns *string
   QC_DATASOURCEPOOL->addMethodExtended("getHostName",       (q_method_t)DSP_getHostName, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, stringOrNothingTypeInfo);

   // DatasourcePool::getPort() returns *int
   QC_DATASOURCEPOOL->addMethodExtended("getPort",           (q_method_t)DSP_getPort, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, bigIntOrNothingTypeInfo);

   QC_DATASOURCEPOOL->addMethodExtended("getDriverName",     (q_method_t)DSP_getDriverName, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, stringTypeInfo);

   QC_DATASOURCEPOOL->addMethodExtended("getMinimum",        (q_method_t)DSP_getMinimum, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, bigIntTypeInfo);
   QC_DATASOURCEPOOL->addMethodExtended("getMaximum",        (q_method_t)DSP_getMaximum, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, bigIntTypeInfo);
   QC_DATASOURCEPOOL->addMethodExtended("toString",          (q_method_t)DSP_toString, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, stringTypeInfo);

   QC_DATASOURCEPOOL->addMethodExtended("getServerVersion",  (q_method_t)DSP_getServerVersion, false, QC_NO_FLAGS, QDOM_DEFAULT);
   QC_DATASOURCEPOOL->addMethodExtended("getClientVersion",  (q_method_t)DSP_getClientVersion, false, QC_NO_FLAGS, QDOM_DEFAULT);
   QC_DATASOURCEPOOL->addMethodExtended("inTransaction",     (q_method_t)DSP_inTransaction, false, QC_NO_FLAGS, QDOM_DEFAULT, boolTypeInfo);

   return QC_DATASOURCEPOOL;
}
