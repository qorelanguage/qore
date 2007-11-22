/*
 QoreQtEventDispatcher.h
 
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

#ifndef _QORE_QOREQTEVENTDISPATCHER_H

#define _QORE_QOREQTEVENTDISPATCHER_H

#include "qore-qt.h"

class QoreQtEventDispatcher {
   protected:
      DLLLOCAL static void dispatch_event(Object *qore_obj, Method *m, QoreClass *qclass, class AbstractPrivateData *data)
      {
	 class ExceptionSink xsink;

	 discard(dispatch_event_intern(qore_obj, m, qclass, data, &xsink), &xsink);
      }

      DLLLOCAL static void dispatch_event(Object *qore_obj, Method *m, class QoreList *args)
      {
	 class ExceptionSink xsink;

	 discard(dispatch_event_intern(qore_obj, m, args, &xsink), &xsink);
      }

      DLLLOCAL static bool dispatch_event_bool(Object *qore_obj, Method *m, QoreClass *qclass, class AbstractPrivateData *data)
      {
	 class ExceptionSink xsink;

	 ReferenceHolder<QoreNode> rv(dispatch_event_intern(qore_obj, m, qclass, data, &xsink), &xsink);
	 return *rv ? rv->getAsBool() : false;
      }

      DLLLOCAL static int dispatch_event_int(Object *qore_obj, Method *m, QoreList *args)
      {
	 class ExceptionSink xsink;

	 ReferenceHolder<QoreNode> rv(dispatch_event_intern(qore_obj, m, args, &xsink), &xsink);
	 return *rv ? rv->getAsInt() : false;
      }

      DLLLOCAL static QString dispatch_event_qstring(Object *qore_obj, Method *m, QoreList *args)
      {
	 class ExceptionSink xsink;

	 ReferenceHolder<QoreNode> rv(dispatch_event_intern(qore_obj, m, args, &xsink), &xsink);
	 if (xsink)
	    return QString();

	 QString str;
	 get_qstring(*rv, str, &xsink);
	 return str;
      }

      DLLLOCAL static bool dispatch_event_bool(Object *qore_obj, Method *m, QoreList *args)
      {
	 class ExceptionSink xsink;

	 ReferenceHolder<QoreNode> rv(dispatch_event_intern(qore_obj, m, args, &xsink), &xsink);
	 return *rv ? rv->getAsBool() : false;
      }

      DLLLOCAL static Method *findMethod(QoreClass *qc, const char *n)
      {
	 Method *m = qc->findMethod(n);
	 //printd(5, "findMethod() %s::%s: %s\n", qc->getName(), n, (m && m->getType() == CT_USER) ? "ok" : "x");
	 return (m && m->getType() == CT_USER) ? m : 0;
      }

      DLLLOCAL static QoreNode *dispatch_event_intern(Object *qore_obj, Method *m, QoreClass *qclass, class AbstractPrivateData *data, class ExceptionSink *xsink)
      {
	 // create argument list
	 Object *peo = new Object(qclass, getProgram());
	 peo->setPrivate(qclass->getID(), data);
	 QoreNode *a = new QoreNode(peo);
	 QoreList *args = new QoreList();
	 args->push(a);
	 QoreNode *na = new QoreNode(args);
	 
	 // call event method
	 QoreNode *rv = m->eval(qore_obj, na, xsink);
	 
	 // delete arguments
	 na->deref(xsink);

	 return rv;
      }
      DLLLOCAL static QoreNode *dispatch_event_intern(Object *qore_obj, Method *m, class QoreList *args, class ExceptionSink *xsink)
      {
	 QoreNode *na = args ? new QoreNode(args) : 0;
	 
	 // call event method
	 QoreNode *rv = m->eval(qore_obj, na, xsink);
	 
	 // delete arguments
	 if (args)
	    na->deref(xsink);

	 return rv;
      }
};

#endif
