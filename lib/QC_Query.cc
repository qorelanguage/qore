/*
  Objects/Query.cc

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
#include <qore/Object.h>
#include <qore/Namespace.h>

#include <qore/SQL/Objects/Query.h>
#include <qore/SQL/Objects/Datasource.h>

#include <strings.h>

int CID_QUERY;

// global thread-safe query list for inter-query references
class QueryList queryList;

void Query::validate(int &stc, QList *&ql, ExceptionSink *xsink)
{
   // if there is a static function before and we are dynamic, then
   // return an error
   if (stc && isDynamic())
   {
      xsink->raiseException("QUERY-DYNAMIC-DEPENDENCY-EXCEPTION", "static query depends on dynamic query");
      return;
   }

   // if there is a circular dependency, then return an error
   if (ql && ql->inlist(this))
   {
      xsink->raiseException("QUERY-CIRCULAR-DEPENDENCY-EXCEPTION", "there is a circular dependency in the query dependency list");
      return;
   }

   if (isStatic())
      stc = 1;

   for (int i = 0; i < qpl->len; i++)
   {
      Object *qo = qpl->pl[i]->getQuery();
      if (qo)
      {
	 Query *q = (Query *)qo->getPrivateData(CID_QUERY);
	 if (q)
	 {
	    if (!ql)
	    {
	       ql = new QList();
	       ql->add(this);
	    }
	    q->validate(stc, ql, xsink);
	 }
	 qo->dereference(xsink);
      }
      if (xsink->isEvent())
	 return;
   }
}
class QoreNode *f_dbg_node_info(class Object *self, class QoreNode *params, ExceptionSink *xsink);

static inline void getQ(void *obj)
{
   ((Query *)obj)->ROreference();
}

static inline void releaseQ(void *obj)
{
   ((Query *)obj)->deref();
}

static void Q_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   tracein("Q_constructor()");

   int i = 2;

   QoreNode *p0 = test_param(params, NT_STRING, 0);
   if (!p0)
   {
      xsink->raiseException("QUERY-PARAMETER-ERROR", "expecting query name (string) as first parameter of Query() constructor");
      // "de-type" self
      self->doDeleteNoDestructor(xsink);
      traceout("Q_constructor()");
      return;
   }
   QoreNode *p2 = test_param(params, NT_STRING, 2);
   if (!p2)
   {
      xsink->raiseException("QUERY-PARAMETER-ERROR", "expecting query string as third parameter of Query() constructor");
      // "de-type" self
      self->doDeleteNoDestructor(xsink);
      traceout("Q_constructor()");
      return;
   }

   Gate *g = NULL;
   QoreNode *p1 = test_object_param(params, CID_DATASOURCE, 1, &g);
   if (!p1)
   {
#ifdef DEBUG
      QoreNode *n = params->val.list->retrieve_entry(1);
      printd(0, "Q_constructor() params[0]=%08p type=%s class=%s id=%d (CID_DATASOURCE=%d)\n",
	     n, n->type->getName(), n->val.object->getClass(), n->val.object->getClass()->getID(), CID_DATASOURCE);
#endif
      xsink->raiseException("QUERY-PARAMETER-ERROR", "expecting datasource as first parameter of Query() constructor");
      // "de-type" self
      self->doDeleteNoDestructor(xsink);
      traceout("Q_constructor()");
      return;
   }
   Datasource *ds = (Datasource *)p0->val.object->getReferencedPrivateData(CID_DATASOURCE);
   g->exit();
   if (!ds)
   {
      xsink->raiseException("QUERY-PARAMETER-ERROR", "expecting datasource as first parameter of Query() constructor");
      // "de-type" self
      self->doDeleteNoDestructor(xsink);
      traceout("Q_constructor()");
      return;
   }

   QoreNode *p2 = get_param(params, 2);
   int stc = 1;
   if (p2)
   {
      stc = p2->getAsInt();
      i++;
   }

   class Query *q = new Query(p0->val.String->getBuffer(), ds, p2->val.String->getBuffer(), stc, xsink);
   if (xsink->isEvent())
   {
      // delete query object and "de-type" self
      delete q;
      self->doDeleteNoDestructor(xsink);
      traceout("Q_constructor()");
      return;
   }

   QoreNode *p;
   // allow user to set options
   if ((p = test_param(params, NT_HASH, i)))
      self->merge(p->val.hash, xsink);

   if (self->setPrivate(CID_QUERY, q, getQ, releaseQ))
      delete q;

   traceout("Q_constructor()");
}

static void Q_copy(class Object *self, class Object *old, class Query *q, class ExceptionSink *xsink)
{
   xsink->raiseException("COPY-ERROR", "cannot copy queue objects");
}

static QoreNode *Q_refresh(class Object *self, class Query *q, class QoreNode *params, ExceptionSink *xsink)
{
   Query *q = (Query *)self->getReferencedPrivateData(CID_QUERY);
   if (q)
   {
      Hash *h = q->exec(xsink);
      if (h)
	 self->replaceData(h, xsink);
      q->deref();
   }
   else
      alreadyDeleted(xsink, "Query::refresh");
   return NULL;
}

static QoreNode *Q_check(class Object *self, class Query *q, class QoreNode *params, ExceptionSink *xsink)
{
   Query *q = (Query *)self->getReferencedPrivateData(CID_QUERY);

   if (q)
   {
      class Hash *h = q->check(xsink);

      if (h) // set new data
	 self->replaceData(h, xsink);
      q->deref();
   }
   else
      alreadyDeleted(xsink, "Query::check");
   return NULL;
}

static QoreNode *Q_key(class Object *self, class Query *q, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0, *p1, *p2;
   if (!(p0 = test_param(params, NT_STRING, 0)) ||
       !(p1 = test_param(params, NT_STRING, 1)) ||
       !(p2 = test_param(params, NT_STRING, 2)))
   {
      xsink->raiseException("KEY-PARAMETER-EXCEPTION", "expecting membername (string), objectname (string), and membername (string) as parameters");
      return NULL;
   }

   Query *q = (Query *)self->getReferencedPrivateData(CID_QUERY);

   if (q)
   {
      q->key(p0->val.String->getBuffer(),
	     p1->val.String->getBuffer(),
	     p2->val.String->getBuffer(), xsink);
      q->deref();
   }
   else
      alreadyDeleted(xsink, "Query::key");
   return NULL;
}

static QoreNode *Q_setSQL(class Object *self, class Query *q, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = test_param(params, NT_STRING, 0);
   if (!p0)
   {
      xsink->raiseException("QUERY-SET-QUERY-PARAMETER-ERROR", 
		     "expecting string as sole argument for Query::setQuery()");
      return NULL;
   }

   Query *q = (Query *)self->getReferencedPrivateData(CID_QUERY);

   if (q)
   {
      q->setSQL(p0->val.String->getBuffer(), xsink);
      q->deref();
   }
   else
      alreadyDeleted(xsink, "Query::setSQL");

   return NULL;
}

class QoreClass *initQueryQoreClass()
{
   tracein("initQueryQoreClass()");

   class QoreClass *QC_QUERY = new QoreClass(strdup("Query"));
   CID_QUERY = QC_QUERY->getID();
   QC_QUERY->setConstructor(Q_constructor);
   QC_QUERY->setCopy((q_copy_t)Q_copy);
   QC_QUERY->addMethod("refresh",       (q_method_t)Q_refresh);

   traceout("initQueryQoreClass()");
   return QC_QUERY;
}
