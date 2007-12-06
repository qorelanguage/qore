/*
 QoreAbstractQObject.h
 
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

#ifndef _QORE_QOREABSTRACTQOBJECT_H

#define _QORE_QOREABSTRACTQOBJECT_H

#include <qore/Qore.h>

#include <QPointer>
#include <QObject>
#include <QHash>
#include <QList>
#include <QMetaMethod>

#include "QoreQtEventDispatcher.h"
#include "QoreQtDynamicMethod.h"

#include "qore-qt-events.h"

class QoreQObjectExtension : public QoreQtEventDispatcher
{
   protected:
      QoreMethod *e_timerEvent, *e_childEvent;

   public:
      DLLLOCAL QoreQObjectExtension(QoreClass *qc)
      {
         e_timerEvent = findMethod(qc, "timerEvent");
         e_childEvent = findMethod(qc, "childEvent");
      }
};

class QoreAbstractQObject : public AbstractPrivateData
{
   public:

      DLLLOCAL virtual class QObject *getQObject() const = 0;

      DLLLOCAL virtual QoreObject *getQoreObject() const = 0;

      // event methods
      DLLLOCAL virtual void timerEvent(QTimerEvent * event) = 0;
      DLLLOCAL virtual void childEvent(QChildEvent * event) = 0;

      // for dynamic signals and slots
      DLLLOCAL virtual int getSlotIndex(const QByteArray &theSlot, class ExceptionSink *xsink) = 0;
      DLLLOCAL virtual int getSignalIndex(const QByteArray &theSignal) const = 0;
      DLLLOCAL virtual int createSignal(const char *signal, class ExceptionSink *xsink) = 0;
      DLLLOCAL virtual int connectDynamic(QoreAbstractQObject *sender, const char *signal, const char *slot, class ExceptionSink *xsink) = 0;

      // emits a signal; args are offset from 1
      DLLLOCAL virtual void emit_signal(const char *sig, QoreList *args) = 0;
      DLLLOCAL virtual QoreQtDynamicSlot *getSlot(const char *sig, class ExceptionSink *xsink) = 0;

      // protected QObject methods
      DLLLOCAL virtual QObject *sender() const = 0;
};

#endif
