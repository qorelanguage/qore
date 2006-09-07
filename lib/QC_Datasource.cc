/*
  QC_Datasource.cc

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006 David Nichols

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

#include <qore/config.h>
#include <qore/common.h>
#include <qore/params.h>
#include <qore/Exception.h>
#include <qore/support.h>
#include <qore/QoreClass.h>
#include <qore/Namespace.h>
#include <qore/DBI.h>
#include <qore/QC_Datasource.h>

int CID_DATASOURCE;

static void getDS(void *obj)
{
   ((Datasource *)obj)->ref();
}

static void releaseDS(void *obj)
{
   ((Datasource *)obj)->deref();
}

// usage: Datasource(db name, [username, password, dbname])
static void DS_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_STRING, 0);
   if (!p)
   {
      xsink->raiseException("DATASOURCE-PARAMETER-ERROR", "expecting database type as first parameter of Datasource() constructor");
      // "de-type" self
      self->doDeleteNoDestructor(xsink);
      return;
   }
   DBIDriver *db_driver = DBI.find(p->val.String->getBuffer());
   if (!db_driver)
   {
      xsink->raiseException("DATASOURCE-UNSUPPORTED-DATABASE", "no DBI driver can be found for database type \"%s\"", p->val.String->getBuffer());
      // "de-type" self
      self->doDeleteNoDestructor(xsink);
      return;
   }
   class Datasource *ds = new Datasource(db_driver);

   if ((p = test_param(params, NT_STRING, 1)))
      ds->setUsername(p->val.String->getBuffer());

   if ((p = test_param(params, NT_STRING, 2)))
      ds->setPassword(p->val.String->getBuffer());

   if ((p = test_param(params, NT_STRING, 3)))
      ds->setDBName(p->val.String->getBuffer());
   
   if ((p = test_param(params, NT_STRING, 4)))
      ds->setCharset(p->val.String->getBuffer());
   
   self->setPrivate(CID_DATASOURCE, ds, getDS, releaseDS);
}

static void DS_destructor(class Object *self, class Datasource *ds, ExceptionSink *xsink)
{
   ds->deref();
}

static void DS_copy(class Object *self, class Object *old, class Datasource *ods, class ExceptionSink *xsink)
{
   self->setPrivate(CID_DATASOURCE, ods->copy(), getDS, releaseDS);
}

static QoreNode *DS_open(class Object *self, class Datasource *ds, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)ds->open(xsink));
}

static QoreNode *DS_close(class Object *self, class Datasource *ds, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)ds->close());
}

static QoreNode *DS_commit(class Object *self, class Datasource *ds, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)ds->commit(xsink));
}

static QoreNode *DS_rollback(class Object *self, class Datasource *ds, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)ds->rollback(xsink));
}

static QoreNode *DS_setAutoCommit(class Object *self, class Datasource *ds, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   ds->setAutoCommit(p ? p->getAsBool() : true);
   return NULL;
}

static QoreNode *DS_getAutoCommit(class Object *self, class Datasource *ds, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(ds->getAutoCommit());
}

static QoreNode *DS_exec(class Object *self, class Datasource *ds, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p0;
   if (!(p0 = test_param(params, NT_STRING, 0)))
      return NULL;

   List *args = params->val.list->size() > 1 ? params->val.list->copyListFrom(1) : NULL;
   class QoreNode *rv = ds->exec(p0->val.String, args, xsink);
   if (args)
   {
      args->dereference(xsink);
      delete args;
   }
   return rv;
}

static QoreNode *DS_vexec(class Object *self, class Datasource *ds, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p0;
   if (!(p0 = test_param(params, NT_STRING, 0)))
      return NULL;

   QoreNode *p1 = test_param(params, NT_LIST, 1);
   List *args = p1 ? p1->val.list : NULL;
   return ds->exec(p0->val.String, args, xsink);
}

static QoreNode *DS_select(class Object *self, class Datasource *ds, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_STRING, 0);
   if (!p)
      return NULL;

   List *args = params->val.list->size() > 1 ? params->val.list->copyListFrom(1) : NULL;
   class QoreNode *rv = ds->select(p->val.String, args, xsink);
   if (args)
   {
      args->dereference(xsink);
      delete args;
   }
   return rv;
}

static QoreNode *DS_selectRows(class Object *self, class Datasource *ds, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_STRING, 0);
   if (!p)
      return NULL;

   List *args = params->val.list->size() > 1 ? params->val.list->copyListFrom(1) : NULL;
   class QoreNode *rv = ds->selectRows(p->val.String, args, xsink);
   if (args)
   {
      args->dereference(xsink);
      delete args;
   }
   return rv;
}

/*
static QoreNode *DS_describe(class Object *self, class Datasource *ds, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_STRING, 0);
   if (!p)
      return NULL;

   class Hash *h = ds->describe(p->val.String->getBuffer(), xsink);
   if (h)
	return new QoreNode(h);

   return NULL;
}
*/

static QoreNode *DS_beginTransaction(class Object *self, class Datasource *ds, class QoreNode *params, ExceptionSink *xsink)
{
   ds->beginTransaction(xsink);
   return NULL;
}

static QoreNode *DS_reset(class Object *self, class Datasource *ds, class QoreNode *params, ExceptionSink *xsink)
{
   ds->reset(xsink);
   return NULL;
}

static QoreNode *DS_getCapabilities(class Object *self, class Datasource *ds, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)ds->getCapabilities());
}

static QoreNode *DS_getCapabilityList(class Object *self, class Datasource *ds, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(ds->getCapabilityList());
}

static QoreNode *DS_setUserName(class Object *self, class Datasource *ds, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_STRING, 0);
   if (!p)
      return NULL;

   ds->setUsername(p->val.String->getBuffer());
   return NULL;
}

static QoreNode *DS_setPassword(class Object *self, class Datasource *ds, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_STRING, 0);
   if (!p)
      return NULL;

   ds->setPassword(p->val.String->getBuffer());
   return NULL;
}

static QoreNode *DS_setDBName(class Object *self, class Datasource *ds, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_STRING, 0);
   if (!p)
      return NULL;

   ds->setDBName(p->val.String->getBuffer());
   return NULL;
}

static QoreNode *DS_setDBCharset(class Object *self, class Datasource *ds, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_STRING, 0);
   if (!p)
      return NULL;

   ds->setCharset(p->val.String->getBuffer());
   return NULL;
}

static QoreNode *DS_setHostName(class Object *self, class Datasource *ds, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_STRING, 0);
   if (!p)
      return NULL;

   ds->setHostName(p->val.String->getBuffer());
   return NULL;
}

static QoreNode *DS_getUserName(class Object *self, class Datasource *ds, class QoreNode *params, ExceptionSink *xsink)
{
   return ds->getUsername();
}

static QoreNode *DS_getPassword(class Object *self, class Datasource *ds, class QoreNode *params, ExceptionSink *xsink)
{
   return ds->getPassword();
}

static QoreNode *DS_getDBName(class Object *self, class Datasource *ds, class QoreNode *params, ExceptionSink *xsink)
{
   return ds->getDBName();
}

static QoreNode *DS_getDBCharset(class Object *self, class Datasource *ds, class QoreNode *params, ExceptionSink *xsink)
{
   return ds->getCharset();
}

static QoreNode *DS_getOSCharset(class Object *self, class Datasource *ds, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(ds->qorecharset ? ds->qorecharset->code : "(unknown)");
}

static QoreNode *DS_getHostName(class Object *self, class Datasource *ds, class QoreNode *params, ExceptionSink *xsink)
{
   return ds->getHostName();
}

static QoreNode *DS_setTransactionLockTimeout(class Object *self, class Datasource *ds, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = get_param(params, 0);
   ds->setTransactionLockTimeout(p0 ? p0->getAsInt() : 0);
   return NULL;
}

static QoreNode *DS_getTransactionLockTimeout(class Object *self, class Datasource *ds, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)ds->getTransactionLockTimeout());
}

class QoreClass *initDatasourceClass()
{
   tracein("initDatasourceClass()");

   class QoreClass *QC_DATASOURCE = new QoreClass(strdup("Datasource"));
   CID_DATASOURCE = QC_DATASOURCE->getID();
   QC_DATASOURCE->setConstructor(DS_constructor);
   QC_DATASOURCE->setDestructor((q_destructor_t)DS_destructor);
   QC_DATASOURCE->setCopy((q_copy_t)DS_copy);
   QC_DATASOURCE->addMethod("open",              (q_method_t)DS_open);
   QC_DATASOURCE->addMethod("close",             (q_method_t)DS_close);
   QC_DATASOURCE->addMethod("commit",            (q_method_t)DS_commit);
   QC_DATASOURCE->addMethod("rollback",          (q_method_t)DS_rollback);
   QC_DATASOURCE->addMethod("exec",              (q_method_t)DS_exec);
   QC_DATASOURCE->addMethod("vexec",             (q_method_t)DS_vexec);
   QC_DATASOURCE->addMethod("select",            (q_method_t)DS_select);
   QC_DATASOURCE->addMethod("selectRows",        (q_method_t)DS_selectRows);
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

   QC_DATASOURCE->addMethod("setTransactionLockTimeout", (q_method_t)DS_setTransactionLockTimeout);
   QC_DATASOURCE->addMethod("getTransactionLockTimeout", (q_method_t)DS_getTransactionLockTimeout);

   traceout("initDatasourceClass()");
   return QC_DATASOURCE;
}
