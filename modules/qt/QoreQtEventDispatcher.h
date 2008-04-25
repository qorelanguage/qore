/*
 QoreQtEventDispatcher.h
 
 Qore Programming Language
 
 Copyright 2003 - 2008 David Nichols
 
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

#ifndef _QORE_QOREQTEVENTDISPATCHER_H

#define _QORE_QOREQTEVENTDISPATCHER_H

#include "qore-qt.h"

class QoreQtEventDispatcher {
   protected:
      DLLLOCAL static void dispatch_event(QoreObject *qore_obj, const QoreMethod *m, QoreClass *qclass, class AbstractPrivateData *data)
      {
	 class ExceptionSink xsink;

	 discard(dispatch_event_intern(qore_obj, m, qclass, data, &xsink), &xsink);
      }

      DLLLOCAL static void dispatch_event(QoreObject *qore_obj, const QoreMethod *m, class QoreListNode *args)
      {
	 class ExceptionSink xsink;

	 discard(dispatch_event_intern(qore_obj, m, args, &xsink), &xsink);
      }

      DLLLOCAL static bool dispatch_event_bool(QoreObject *qore_obj, const QoreMethod *m, QoreClass *qclass, class AbstractPrivateData *data)
      {
	 class ExceptionSink xsink;

	 ReferenceHolder<AbstractQoreNode> rv(dispatch_event_intern(qore_obj, m, qclass, data, &xsink), &xsink);
	 return *rv ? rv->getAsBool() : false;
      }

      DLLLOCAL static int dispatch_event_int(QoreObject *qore_obj, const QoreMethod *m, QoreListNode *args)
      {
	 class ExceptionSink xsink;

	 ReferenceHolder<AbstractQoreNode> rv(dispatch_event_intern(qore_obj, m, args, &xsink), &xsink);
	 return *rv ? rv->getAsInt() : false;
      }

      DLLLOCAL static int dispatch_event_int(QoreObject *qore_obj, const QoreMethod *m, const QoreListNode *args, ExceptionSink *xsink)
      {
	 ReferenceHolder<AbstractQoreNode> rv(dispatch_event_intern(qore_obj, m, args, xsink), xsink);
	 return *rv ? rv->getAsInt() : false;
      }

      DLLLOCAL static QString dispatch_event_qstring(QoreObject *qore_obj, const QoreMethod *m, QoreListNode *args)
      {
	 class ExceptionSink xsink;

	 ReferenceHolder<AbstractQoreNode> rv(dispatch_event_intern(qore_obj, m, args, &xsink), &xsink);
	 if (xsink)
	    return QString();

	 QString str;
	 get_qstring(*rv, str, &xsink);
	 return str;
      }

      DLLLOCAL static bool dispatch_event_bool(QoreObject *qore_obj, const QoreMethod *m, QoreListNode *args)
      {
	 class ExceptionSink xsink;

	 ReferenceHolder<AbstractQoreNode> rv(dispatch_event_intern(qore_obj, m, args, &xsink), &xsink);
	 return *rv ? rv->getAsBool() : false;
      }

      DLLLOCAL static bool dispatch_event_bool(QoreObject *qore_obj, const QoreMethod *m, const QoreListNode *args, ExceptionSink *xsink)
      {
	 ReferenceHolder<AbstractQoreNode> rv(dispatch_event_intern(qore_obj, m, args, xsink), xsink);
	 return *rv ? rv->getAsBool() : false;
      }

      DLLLOCAL static const QoreMethod *findMethod(const QoreClass *qc, const char *n)
      {
	 const QoreMethod *m = qc->findMethod(n);
	 //printd(5, "findMethod() %s::%s: %s\n", qc->getName(), n, (m && m->isUser()) ? "ok" : "x");
	 return (m && m->isUser()) ? m : 0;
      }

      DLLLOCAL static AbstractQoreNode *dispatch_event_intern(QoreObject *qore_obj, const QoreMethod *m, QoreClass *qclass, class AbstractPrivateData *data, class ExceptionSink *xsink)
      {
	 // create argument list
	 QoreObject *peo = new QoreObject(qclass, getProgram());
	 peo->setPrivate(qclass->getID(), data);
	 ReferenceHolder<QoreListNode> args(new QoreListNode(), xsink);
	 args->push(peo);
	 
	 // call event method
	 return m->eval(qore_obj, *args, xsink);
      }
      DLLLOCAL static AbstractQoreNode *dispatch_event_intern(QoreObject *qore_obj, const QoreMethod *m, const QoreListNode *args, class ExceptionSink *xsink)
      {
	 // call event method
	 return m->eval(qore_obj, args, xsink);
      }
};

#endif
