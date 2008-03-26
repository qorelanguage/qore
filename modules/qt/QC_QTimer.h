/*
 QC_QTimer.h
 
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

#ifndef _QORE_QC_QTIMER_H

#define _QORE_QC_QTIMER_H

#include "QoreAbstractQLayout.h"

#include <QTimer>

DLLEXPORT extern qore_classid_t CID_QTIMER;

DLLLOCAL class QoreClass *initQTimerClass(class QoreClass *qobject);
DLLLOCAL void initQTimerStaticFunctions();

class myQTimer : public QTimer, public QoreQObjectExtension
{
#define QOREQTYPE QTimer
#define MYQOREQTYPE myQTimer
#include "qore-qt-metacode.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

      DLLLOCAL myQTimer(QoreObject *obj, QObject *parent) : QTimer(parent), QoreQObjectExtension(obj, this)
      {
	 
      }      
};

typedef QoreQObjectBase<myQTimer, QoreAbstractQObject> QoreQTimerImpl;

class QoreQTimer : public QoreQTimerImpl
{
   public:
      DLLLOCAL QoreQTimer(QoreObject *obj, QObject *parent = 0) : QoreQTimerImpl(new myQTimer(obj, parent))
      {
      }
};

class QoreSingleShotTimer : public QObject, public QoreQObjectExtension
{
#define QORE_NO_TIMER_EVENT
#define QOREQTYPE QObject
#define MYQOREQTYPE QoreSingleShotTimer
#include "qore-qt-metacode.h"
#undef MYQOREQTYPE
#undef QOREQTYPE
#undef QORE_NO_TIMER_EVENT

   private:
      int timerId;

   public:
      DLLLOCAL QoreSingleShotTimer(QoreObject *obj) : QoreQObjectExtension(obj, this)
      {
	 
      }

      DLLLOCAL void timer_init(QoreAbstractQObject *qsst, int msec, QoreAbstractQObject *receiver, const char *member, class ExceptionSink *xsink)
      {
	 createDynamicSignal("timeout()", xsink);
	 assert(!*xsink);

	 receiver->connectDynamic(qsst, "2timeout()", member, xsink);
	 if (*xsink) {
	    qore_obj->deref(xsink);
	    return;
	 }
	 timerId = startTimer(msec);
      }
      
protected:
      DLLLOCAL virtual void timerEvent(QTimerEvent *)
      {
	 // need to kill the timer _before_ we emit timeout() in case the
	 // slot connected to timeout calls processEvents()
	 if (timerId > 0)
	    killTimer(timerId);
	 timerId = -1;
	 emit_signal("timeout()", 0);

	 ExceptionSink xsink;
	 qore_obj->deref(&xsink);
      }
};

typedef QoreQtQObjectBase<QoreSingleShotTimer, QoreAbstractQObject> QoreQtSingleShotTimerImpl;

class QoreQtSingleShotTimer : public QoreQtSingleShotTimerImpl
{
   public:
      DLLLOCAL QoreQtSingleShotTimer(QoreObject *obj, int msec, QoreAbstractQObject *receiver, const char *member, class ExceptionSink *xsink) : QoreQtSingleShotTimerImpl(obj, new QoreSingleShotTimer(qore_obj))
      {
	 this->qobj->timer_init(this, msec, receiver, member, xsink);
      }
};

#endif
