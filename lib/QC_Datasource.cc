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

static inline void *getDS(void *obj)
{
   ((Datasource *)obj)->ref();
   return obj;
}

// usage: Datasource(db name, [username, password, dbname])
static QoreNode *DS_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_STRING, 0);
   if (!p)
   {
      xsink->raiseException("DATASOURCE-PARAMETER-ERROR", "expecting database type as first parameter of Datasource() constructor");
      // "de-type" self
      self->doDeleteNoDestructor(xsink);
      return NULL;
   }
   DBIDriver *db_driver = DBI.find(p->val.String->getBuffer());
   if (!db_driver)
  {
      xsink->raiseException("DATASOURCE-UNSUPPORTED-DATABASE", "no DBI driver can be found for database type \"%s\"", p->val.String->getBuffer());
      // "de-type" self
      self->doDeleteNoDestructor(xsink);
      return NULL;
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
   
   self->setPrivate(CID_DATASOURCE, ds, getDS);

   return NULL;
}

static QoreNode *DS_destructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Datasource *ds = (Datasource *)self->getAndClearPrivateData(CID_DATASOURCE);
   if (ds)
      ds->deref();
   return NULL;
}

static QoreNode *DS_copy(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Datasource *ods = (Datasource *)get_param(params, 0)->val.object->getReferencedPrivateData(CID_DATASOURCE);
   if (ods)
   {
      class Datasource *nds = ods->copy();
      self->setPrivate(CID_DATASOURCE, nds, getDS);

      ods->deref();
   }
   else
      alreadyDeleted(xsink, "Datasource::copy");

   return NULL;
}

static QoreNode *DS_open(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Datasource *ds = (Datasource *)self->getReferencedPrivateData(CID_DATASOURCE);
   QoreNode *rv;
   if (ds)
   {
      rv = new QoreNode(NT_INT, ds->open(xsink));
      ds->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Datasource::open");
   }
   return rv;
}

static QoreNode *DS_close(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Datasource *ds = (Datasource *)self->getReferencedPrivateData(CID_DATASOURCE);
   QoreNode *rv;
   if (ds)
   {
      rv = new QoreNode(NT_INT, ds->close());
      ds->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Datasource::close");
   }
   return rv;
}

static QoreNode *DS_commit(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Datasource *ds = (Datasource *)self->getReferencedPrivateData(CID_DATASOURCE);
   QoreNode *rv;
   if (ds)
   {
      rv = new QoreNode(NT_INT, ds->commit(xsink));
      ds->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Datasource::commit");
   }
   return rv;
}

static QoreNode *DS_rollback(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Datasource *ds = (Datasource *)self->getReferencedPrivateData(CID_DATASOURCE);
   QoreNode *rv;
   if (ds)
   {
      rv = new QoreNode(NT_INT, ds->rollback(xsink));
      ds->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Datasource::rollback");
   }
   return rv;
}

static QoreNode *DS_setAutoCommit(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   bool i;
   QoreNode *p = get_param(params, 0);
   if (!p)
      i = true;
   else
      i = p->getAsBool();

   Datasource *ds = (Datasource *)self->getReferencedPrivateData(CID_DATASOURCE);
   if (ds)
   {
      ds->setAutoCommit(i);
      ds->deref();
   }
   else
      alreadyDeleted(xsink, "Datasource::setAutoCommit");
   return NULL;
}

static QoreNode *DS_getAutoCommit(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Datasource *ds = (Datasource *)self->getReferencedPrivateData(CID_DATASOURCE);
   QoreNode *rv;
   if (ds)
   {
      rv = new QoreNode(ds->getAutoCommit());
      ds->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Datasource::getAutoCommit");
   }
   return rv;
}

static QoreNode *DS_exec(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p0;
   if (!(p0 = test_param(params, NT_STRING, 0)))
      return NULL;

   Datasource *ds = (Datasource *)self->getReferencedPrivateData(CID_DATASOURCE);
   QoreNode *rv;
   if (ds)
   {
      List *args = params->val.list->copyListFrom(1);
      rv = ds->exec(p0->val.String, args, xsink);
      ds->deref();
      args->dereference(xsink);
      delete args;
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Datasource::exec");
   }
   return rv;
}

static QoreNode *DS_vexec(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p0;
   if (!(p0 = test_param(params, NT_STRING, 0)))
      return NULL;

   Datasource *ds = (Datasource *)self->getReferencedPrivateData(CID_DATASOURCE);
   QoreNode *rv;
   if (ds)
   {
      QoreNode *p1 = test_param(params, NT_LIST, 1);
      List *args = p1 ? p1->val.list : NULL;
      rv = ds->exec(p0->val.String, args, xsink);
      ds->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Datasource::exec");
   }
   return rv;
}

static QoreNode *DS_select(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_STRING, 0);
   if (!p)
      return NULL;

   Datasource *ds = (Datasource *)self->getReferencedPrivateData(CID_DATASOURCE);
   QoreNode *rv;
   if (ds)
   {
      rv = ds->select(p->val.String, xsink);
      ds->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Datasource::select");
   }
   return rv;
}

static QoreNode *DS_selectRows(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_STRING, 0);
   if (!p)
      return NULL;

   Datasource *ds = (Datasource *)self->getReferencedPrivateData(CID_DATASOURCE);
   QoreNode *rv;
   if (ds)
   {
      rv = ds->selectRows(p->val.String, xsink);
      ds->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Datasource::selectRows");
   }
   return rv;
}

/*
static QoreNode *DS_describe(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_STRING, 0);
   if (!p)
      return NULL;

   Datasource *ds = (Datasource *)self->getReferencedPrivateData(CID_DATASOURCE);
   QoreNode *rv;
   if (ds)
   {
      class Hash *h = ds->describe(p->val.String->getBuffer(), xsink);
      if (h)
	 rv = new QoreNode(h);
      else
	 rv = NULL;
      ds->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Datasource::describe");
   }
   return rv;
}
*/

static QoreNode *DS_beginTransaction(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Datasource *ds = (Datasource *)self->getReferencedPrivateData(CID_DATASOURCE);
   if (ds)
   {
      ds->beginTransaction(xsink);
      ds->deref();
   }
   else
      alreadyDeleted(xsink, "Datasource::beginTransaction");

   return NULL;
}

static QoreNode *DS_reset(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Datasource *ds = (Datasource *)self->getReferencedPrivateData(CID_DATASOURCE);
   if (ds)
   {
      ds->reset(xsink);
      ds->deref();
   }
   else
      alreadyDeleted(xsink, "Datasource::reset");

   return NULL;
}

static QoreNode *DS_getCapabilities(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Datasource *ds = (Datasource *)self->getReferencedPrivateData(CID_DATASOURCE);
   QoreNode *rv;
   if (ds)
   {
      rv = new QoreNode(NT_INT, ds->getCapabilities());
      ds->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Datasource::getCapabilities");
   }

   return rv;
}

static QoreNode *DS_getCapabilityList(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Datasource *ds = (Datasource *)self->getReferencedPrivateData(CID_DATASOURCE);
   QoreNode *rv;
   if (ds)
   {
      rv = new QoreNode(ds->getCapabilityList());
      ds->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Datasource::getCapabilityList");
   }

   return rv;
}

static QoreNode *DS_setUserName(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_STRING, 0);
   if (!p)
      return NULL;

   Datasource *ds = (Datasource *)self->getReferencedPrivateData(CID_DATASOURCE);
   if (ds)
   {
      ds->setUsername(p->val.String->getBuffer());
      ds->deref();
   }
   else
      alreadyDeleted(xsink, "Datasource::setUserName");
   return NULL;
}

static QoreNode *DS_setPassword(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_STRING, 0);
   if (!p)
      return NULL;

   Datasource *ds = (Datasource *)self->getReferencedPrivateData(CID_DATASOURCE);
   if (ds)
   {
      ds->setPassword(p->val.String->getBuffer());
      ds->deref();
   }
   else
      alreadyDeleted(xsink, "Datasource::setPassword");
   return NULL;
}

static QoreNode *DS_setDBName(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_STRING, 0);
   if (!p)
      return NULL;

   Datasource *ds = (Datasource *)self->getReferencedPrivateData(CID_DATASOURCE);
   if (ds)
   {
      ds->setDBName(p->val.String->getBuffer());
      ds->deref();
   }
   else
      alreadyDeleted(xsink, "Datasource::setDBName");
   return NULL;
}

static QoreNode *DS_setDBCharset(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_STRING, 0);
   if (!p)
      return NULL;

   Datasource *ds = (Datasource *)self->getReferencedPrivateData(CID_DATASOURCE);
   if (ds)
   {
      ds->setCharset(p->val.String->getBuffer());
      ds->deref();
   }
   else
      alreadyDeleted(xsink, "Datasource::setDBCharset");
   return NULL;
}

static QoreNode *DS_setHostName(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_STRING, 0);
   if (!p)
      return NULL;

   Datasource *ds = (Datasource *)self->getReferencedPrivateData(CID_DATASOURCE);
   if (ds)
   {
      ds->setHostName(p->val.String->getBuffer());
      ds->deref();
   }
   else
      alreadyDeleted(xsink, "Datasource::setHostName");
   return NULL;
}

static QoreNode *DS_getUserName(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Datasource *ds = (Datasource *)self->getReferencedPrivateData(CID_DATASOURCE);
   QoreNode *rv;

   if (ds)
   {
      rv = ds->getUsername();
      ds->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Datasource::getUserName");
   }
   return rv;
}

static QoreNode *DS_getPassword(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Datasource *ds = (Datasource *)self->getReferencedPrivateData(CID_DATASOURCE);
   QoreNode *rv;

   if (ds)
   {
      rv = ds->getPassword();
      ds->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Datasource::getPassword");
   }
   return rv;
}

static QoreNode *DS_getDBName(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Datasource *ds = (Datasource *)self->getReferencedPrivateData(CID_DATASOURCE);
   QoreNode *rv;

   if (ds)
   {
      rv = ds->getDBName();
      ds->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Datasource::getDBName");
   }
   return rv;
}

static QoreNode *DS_getDBCharset(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Datasource *ds = (Datasource *)self->getReferencedPrivateData(CID_DATASOURCE);
   QoreNode *rv;

   if (ds)
   {
      rv = ds->getCharset();
      ds->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Datasource::getDBCharset");
   }
   return rv;
}

static QoreNode *DS_getOSCharset(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Datasource *ds = (Datasource *)self->getReferencedPrivateData(CID_DATASOURCE);
   QoreNode *rv;

   if (ds)
   {
      rv = new QoreNode(ds->qorecharset ? ds->qorecharset->code : "(unknown)");
      ds->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Datasource::getOSCharset");
   }
   return rv;
}

static QoreNode *DS_getHostName(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Datasource *ds = (Datasource *)self->getReferencedPrivateData(CID_DATASOURCE);
   QoreNode *rv;

   if (ds)
   {
      rv = ds->getHostName();
      ds->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Datasource::getHostName");
   }
   return rv;
}

static QoreNode *DS_setTransactionLockTimeout(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Datasource *ds = (Datasource *)self->getReferencedPrivateData(CID_DATASOURCE);

   int t;
   QoreNode *p0 = get_param(params, 0);
   if (p0)
      t = p0->getAsInt();
   else
      t = 0;

   if (ds)
      ds->setTransactionLockTimeout(t);
   else
      alreadyDeleted(xsink, "Datasource::setTransactionLockTimeout");

   return NULL;
}

static QoreNode *DS_getTransactionLockTimeout(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Datasource *ds = (Datasource *)self->getReferencedPrivateData(CID_DATASOURCE);
   QoreNode *rv;

   if (ds)
      rv = new QoreNode(NT_INT, ds->getTransactionLockTimeout());
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Datasource::getTransactionLockTimeout");
   }

   return rv;
}

class QoreClass *initDatasourceClass()
{
   tracein("initDatasourceClass()");

   class QoreClass *QC_DATASOURCE = new QoreClass(strdup("Datasource"));
   CID_DATASOURCE = QC_DATASOURCE->getID();
   QC_DATASOURCE->addMethod("constructor",       DS_constructor);
   QC_DATASOURCE->addMethod("destructor",        DS_destructor);
   QC_DATASOURCE->addMethod("copy",              DS_copy);
   QC_DATASOURCE->addMethod("open",              DS_open);
   QC_DATASOURCE->addMethod("close",             DS_close);
   QC_DATASOURCE->addMethod("commit",            DS_commit);
   QC_DATASOURCE->addMethod("rollback",          DS_rollback);
   QC_DATASOURCE->addMethod("exec",              DS_exec);
   QC_DATASOURCE->addMethod("vexec",             DS_vexec);
   QC_DATASOURCE->addMethod("select",            DS_select);
   QC_DATASOURCE->addMethod("selectRows",        DS_selectRows);
   //QC_DATASOURCE->addMethod("describe",          DS_describe);
   QC_DATASOURCE->addMethod("beginTransaction",  DS_beginTransaction);
   QC_DATASOURCE->addMethod("reset",             DS_reset);
   QC_DATASOURCE->addMethod("getCapabilities",   DS_getCapabilities);
   QC_DATASOURCE->addMethod("getCapabilityList", DS_getCapabilityList);
   QC_DATASOURCE->addMethod("setAutoCommit",     DS_setAutoCommit);
   QC_DATASOURCE->addMethod("setUserName",       DS_setUserName);
   QC_DATASOURCE->addMethod("setPassword",       DS_setPassword);
   QC_DATASOURCE->addMethod("setDBName",         DS_setDBName);
   QC_DATASOURCE->addMethod("setDBCharset",      DS_setDBCharset);
   QC_DATASOURCE->addMethod("setHostName",       DS_setHostName);
   QC_DATASOURCE->addMethod("getAutoCommit",     DS_getAutoCommit);
   QC_DATASOURCE->addMethod("getUserName",       DS_getUserName);
   QC_DATASOURCE->addMethod("getPassword",       DS_getPassword);
   QC_DATASOURCE->addMethod("getDBName",         DS_getDBName);
   QC_DATASOURCE->addMethod("getDBCharset",      DS_getDBCharset);
   QC_DATASOURCE->addMethod("getOSCharset",      DS_getOSCharset);
   QC_DATASOURCE->addMethod("getHostName",       DS_getHostName);

   QC_DATASOURCE->addMethod("setTransactionLockTimeout", DS_setTransactionLockTimeout);
   QC_DATASOURCE->addMethod("getTransactionLockTimeout", DS_getTransactionLockTimeout);

   traceout("initDatasourceClass()");
   return QC_DATASOURCE;
}
