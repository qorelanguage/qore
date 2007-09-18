/*
 QC_QClipboard.h
 
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

#ifndef _QORE_QT_QC_QCLIPBOARD_H

#define _QORE_QT_QC_QCLIPBOARD_H

#include <QClipboard>
#include "QoreAbstractQObject.h"
#include "qore-qt-events.h"

DLLLOCAL extern int CID_QCLIPBOARD;
DLLLOCAL extern class QoreClass *QC_QClipboard;

DLLLOCAL class QoreClass *initQClipboardClass(QoreClass *);

class QoreQClipboard : public QoreAbstractQObject
{
   private:
      Object *qore_obj;     // containing qore object

   protected:
      DLLLOCAL ~QoreQClipboard()
      {
	 
      }

   public:
      QPointer<QClipboard> qobj;

      DLLLOCAL QoreQClipboard(Object *obj, QClipboard *cb)
      {
	 qobj = cb;

	 // set pointer to object owner as a property
         qobj->setProperty("qobject", reinterpret_cast<qulonglong>(obj));
      }
      DLLLOCAL virtual class QObject *getQObject() const
      {
         return static_cast<QObject *>(&(*qobj));
      }

      virtual Object* getQoreObject() const
      {
	 return qore_obj;
      }

      virtual int getSlotIndex(const QByteArray &theSlot, ExceptionSink *xsink)
      {
	 return qobj->metaObject()->indexOfSlot(theSlot);
      }

      virtual int getSignalIndex(const QByteArray &theSignal) const
      {
	 return qobj->metaObject()->indexOfSignal(theSignal);
      }

      virtual int createSignal(const char *sig, ExceptionSink *xsink)
      {
	 xsink->raiseException("DYNAMIC-SIGNAL-ERROR", "this class does not support dynamic signals");
	 return -1;
      }

      virtual int connectDynamic(QoreAbstractQObject *sender, const char *signal, const char *slot, ExceptionSink *xsink)
      {
         if (!signal || signal[0] != '2' || !slot || (slot[0] != '1' && slot[0] != '2'))
            return false;
         bool is_signal = slot[0] == '2';

         QByteArray theSignal = QMetaObject::normalizedSignature(signal + 1);
         QByteArray theSlot = QMetaObject::normalizedSignature(slot + 1);
         if (!QMetaObject::checkConnectArgs(theSignal, theSlot)) {
            xsink->raiseException("QT-CONNECT-ERROR", "cannot connect signal '%s' with '%s' due to incompatible arguments", signal + 1, slot + 1);
            //printd(5, "%s::connectDynamic(sender=%08p, signal=%s, slot=%s) this=%08p failed\n", metaObject()->className(), sender, signal, slot, this);
            return -1;
         }

         int targetId = is_signal ? getSignalIndex(theSlot) : getSlotIndex(theSlot, xsink);
         if (targetId < 0) {
            if (!*xsink)
               if (is_signal)
                  xsink->raiseException("QT-CONNECT-ERROR", "target signal '%s' does not exist", slot + 1);
               else
                  xsink->raiseException("QT-CONNECT-ERROR", "target slot '%s' does not exist", slot + 1);
            return -1;
         }

         int signalId = sender->getSignalIndex(theSignal);
         if (signalId < 0) {
            xsink->raiseException("QT-CONNECT-ERROR", "signal '%s' does not exist", signal + 1);
            return -1;
         }

         if (!QMetaObject::connect(sender->getQObject(), signalId, qobj, targetId)) {
            xsink->raiseException("QT-CONNECT-ERROR", "connection from signal '%s' to '%s' failed", signal + 1, slot + 1);
            return -1;
         }
         //printd(5, "%s::connectDynamic(sender=%08p, signal=%d:%s, receiver=%d:%s) this=%08p success\n", metaObject()->className(), sender, signalId, signal, targetId, slot, this);
         return 0;
      }

      virtual void emit_signal(const char *sig, List *args)
      {
         QByteArray theSignal = QMetaObject::normalizedSignature(sig);   
         int id = qobj->metaObject()->indexOfSignal(theSignal);

         if (id < 0)
	    return;
	 QMetaMethod qmm = qobj->metaObject()->method(id);
	 printd(5, "%s::emit_signal(%s, %08p) static signal %d\n", qobj->metaObject()->className(), sig, args, id);
      }

      virtual QoreQtDynamicSlot* getSlot(const char *slot, ExceptionSink *xsink)
      {
	 xsink->raiseException("DYNAMIC-SLOT-ERROR", "this class does not support dynamic slots");
	 return 0;
      }

      virtual QObject *sender() const
      {
	 return 0;
      }

};

#endif // _QORE_QT_QC_QCLIPBOARD_H
