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

class QoreAbstractQObject : public AbstractPrivateData
{
   public:
      DLLLOCAL virtual class QObject *getQObject() const = 0;

      DLLLOCAL virtual QoreObject *getQoreObject() const = 0;

      // event methods
      DLLLOCAL virtual bool event(QEvent * event) = 0;
      DLLLOCAL virtual void timerEvent(QTimerEvent * event) = 0;
      DLLLOCAL virtual void childEvent(QChildEvent * event) = 0;

      // for dynamic signals and slots
      DLLLOCAL virtual int getSlotIndex(const QByteArray &theSlot, class ExceptionSink *xsink) = 0;
      DLLLOCAL virtual int getSignalIndex(const QByteArray &theSignal) const = 0;
      DLLLOCAL virtual int createSignal(const char *signal, class ExceptionSink *xsink) = 0;
      DLLLOCAL virtual int connectDynamic(QoreAbstractQObject *sender, const char *signal, const char *slot, class ExceptionSink *xsink) = 0;

      DLLLOCAL virtual bool deleteBlocker() = 0;

      // emits a signal; args are offset from 1
      DLLLOCAL virtual void emit_signal(const char *sig, const QoreListNode *args) = 0;

      // to mark an object as being externally owned
      DLLLOCAL virtual void setExternallyOwned() = 0;

      // protected QObject methods
      DLLLOCAL virtual QObject *sender() const = 0;
};

class QoreQObjectExtension : public QoreQtEventDispatcher
{
   protected:
      QoreObject *qore_obj;                  // containing qore object for dispatching to qore code
      DynamicMethodMap methodMap;            // dynamic method manager
      QHash<QByteArray, int> slotIndices;    // map slot signatures to indices in the methodMap
      QHash<QByteArray, int> signalIndices;  // map signal signatures to signal IDs
      bool obj_ref, externally_owned;

      // event method pointers
      const QoreMethod *e_timerEvent, *e_childEvent, *e_event;

   public:
      DLLLOCAL QoreQObjectExtension(QoreObject *obj, QObject *qo) : qore_obj(0), obj_ref(false), externally_owned(false)
      {
	 const QoreClass *qc = obj->getClass();
         e_timerEvent = findMethod(qc, "timerEvent");
         e_childEvent = findMethod(qc, "childEvent");
         e_event      = findMethod(qc, "event");

	 // set pointer to object owner as a property
	 qo->setProperty("qobject", reinterpret_cast<qulonglong>(obj));
	 qore_obj = obj;
	 
	 //printd(5, "QoreQObjectExtension::QoreQObjectExtension() %s %s qore_obj=%08p qo=%08p parent=%08p executing qore_obj->tRef()\n", qore_obj->getClassName(), qo->metaObject()->className(), qore_obj, qo, qo->parent());
	 if (qo->parent()) {
	    obj_ref = true;
	    qore_obj->ref();
	 }
	 qore_obj->tRef();

	 // create dummy slot entry for "destroyed" signal
	 methodMap.addMethod(new QoreQtDynamicSlot(qore_obj, 0, 0));

	 // catch destroyed signal
	 QByteArray theSignal = QMetaObject::normalizedSignature("destroyed()");
	 int signalId = qo->metaObject()->indexOfSignal(theSignal);
	 //printd(5, "sigid=%d %s\n", signalId, theSignal.data());
	 assert(signalId >= 0);
	 QMetaObject::connect(qo, signalId, qo, qo->metaObject()->methodCount());
      }

      DLLLOCAL ~QoreQObjectExtension()
      {
	 //printd(5, "QoreQObjectExtension::~QoreQObjectExtension() this=%08p qore_obj=%08p\n", this, qore_obj);
      }

      DLLLOCAL void setExternallyOwned()
      {
	 externally_owned = true;
	 if (!obj_ref) {
	    obj_ref = true;
	    qore_obj->ref();
	 }
      }

      DLLLOCAL QoreObject *getQoreObject() const
      {
	 return qore_obj;
      }

      // returns 0: OK, -1: error
      DLLLOCAL int createDynamicSignal(const char *signal, ExceptionSink *xsink)
      {
	 QByteArray theSignal = QMetaObject::normalizedSignature(signal);
	 int signalId = signalIndices.value(theSignal, -1);
	 if (signalId >= 0)
	    return -1;

	 class QoreQtDynamicSignal *ds = new QoreQtDynamicSignal(theSignal.data(), xsink);
	 if (*xsink) {
	    delete ds;
	    return -1;
	 }

	 signalId = methodMap.addMethod(ds);
	 signalIndices[theSignal] = signalId;
	 //printd(5, "%s::createDynamicSignal() this=%08p id=%d '%s'\n", qore_obj->getClassName(), this, signalId, signal);
	 return 0;
      }

      DLLLOCAL bool connectDynamic(QoreAbstractQObject *sender, const char *signal, QObject *receiver, const char *slot, class ExceptionSink *xsink)
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

	 int targetId = is_signal ? getSignalIndex(receiver, theSlot) : getSlotIndex(receiver, theSlot, xsink);
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

	 if (!QMetaObject::connect(sender->getQObject(), signalId, receiver, targetId)) {
	    xsink->raiseException("QT-CONNECT-ERROR", "connection from signal '%s' to '%s' failed", signal + 1, slot + 1);
	    return -1;
	 }
	 //printd(5, "%s::connectDynamic(sender=%08p, signal=%d:%s, receiver=%d:%s) receiver=%08p success\n", receiver->metaObject()->className(), sender, signalId, signal, targetId, slot, receiver);
	 return 0;
      }

      DLLLOCAL int getSignalIndex(QObject *qo, const QByteArray &theSignal) const
      {
	 int signalId = qo->metaObject()->indexOfSignal(theSignal);
	 
	 if (signalId >= 0) {
	    //printd(5, "%s:getSignalIndex('%s') this=%08p is static (%d)\n", qo->metaObject()->className(), theSignal.data(), this, signalId);
	    return signalId;
	 }

	 signalId = signalIndices.value(theSignal, -1);
	 if (signalId < 0) {
	    //printd(5, "%s:getSignalIndex('%s') this=%08p does not exist\n", qo->metaObject()->className(), theSignal.data(), this);
	    return -1;
	 }
	 
	 //printd(5, "%s:getSignalIndex('%s') this=%08p is dynamic (%d methodid %d)\n", qo->metaObject()->className(), theSignal.data(), this, signalId, signalId + qo->metaObject()->methodCount());
	 return signalId + qo->metaObject()->methodCount();
      }

      DLLLOCAL int getSlotIndex(QObject *qo, const QByteArray &theSlot, class ExceptionSink *xsink)
      {
	 int slotId = qo->metaObject()->indexOfSlot(theSlot);
	 // see if it's a static slot
	 if (slotId >= 0) {
	    //printd(5, "%s:getSlotIndex('%s') is static (%d) qo=%08p\n", qo->metaObject()->className(), theSlot.data(), slotId, qo);
	    return slotId;
	 }

	 // see if it's an existing dynamic slot
	 slotId = slotIndices.value(theSlot, -1);
	 if (slotId >= 0) {
	    //printd(5, "%s:getSlotIndex('%s') is dynamic (%d methodid %d) this=%08p\n", qo->metaObject()->className(), theSlot.data(), slotId, slotId + qo->metaObject()->methodCount(), this);
	    return slotId + qo->metaObject()->methodCount();
	 }

	 // create the slot if possible
	 QoreQtDynamicSlot *ds = new QoreQtDynamicSlot(qore_obj, theSlot.data(), xsink);
	 if (*xsink) {
	    delete ds;
	    return -1;
	 }

	 // create slot entry
	 slotId = methodMap.addMethod(ds);
	 slotIndices[theSlot] = slotId;
	 //printd(5, "%s::getSlotIndex() this=%08p created new dynamic slot, id=%d method_id=%d: '%s'\n", qo->metaObject()->className(), this, slotId, slotId + qo->metaObject()->methodCount(), theSlot.data());

	 return slotId + qo->metaObject()->methodCount();
      }

};

// template for private data classes based on QObject corresponding to the QoreAbstractQObject API
template<typename T, typename V>
class QoreQObjectBase : public V
{
   public:
      QPointer<T> qobj;

      DLLLOCAL QoreQObjectBase(T *qo) : qobj(qo)
      {
      }

      DLLLOCAL ~QoreQObjectBase()
      {
	 //printd(5, "QoreQObjectBase::~QoreQObjectBase() qobj=%08p (%s) parent=%08p, qore_obj=%08p\n", &*qobj, qobj ? qobj->metaObject()->className() : "n/a", qobj ? qobj->parent() : 0, qobj ? qobj->getQoreObject() : 0);
	 // inform the Qt object being managed that the associated QoreObject has been deleted
	 if (qobj)
	    qobj->detach();
      }

      DLLLOCAL virtual class QObject *getQObject() const
      {
         return &*qobj;
      }

      DLLLOCAL virtual int getSlotIndex(const QByteArray &theSlot, ExceptionSink *xsink) 
      {
	 return qobj->getSlotIndex(qobj, theSlot, xsink);
      }
      DLLLOCAL virtual int getSignalIndex(const QByteArray &theSignal) const
      {
	 return qobj->getSignalIndex(qobj, theSignal);
      }
      DLLLOCAL virtual int createSignal(const char *signal, ExceptionSink *xsink)
      {
	 return qobj->createDynamicSignal(signal, xsink);
      }
      DLLLOCAL virtual int connectDynamic(QoreAbstractQObject *sender, const char *signal, const char *slot, ExceptionSink *xsink) 
      {
	 return qobj->connectDynamic(sender, signal, qobj, slot, xsink);
      }
      DLLLOCAL virtual void emit_signal(const char *sig, const QoreListNode *args)
      {
	 return qobj->emit_signal(sig, args);
      }
      DLLLOCAL virtual QObject *sender() const
      {
	 return qobj->getSender();
      }
      DLLLOCAL virtual QoreObject *getQoreObject() const
      {
	 return qobj->getQoreObject();
      }
      DLLLOCAL virtual bool event(QEvent *event)
      {
	 return qobj->parent_event(event); 
      }
      DLLLOCAL virtual void timerEvent(QTimerEvent * event)
      {
	 qobj->parent_timerEvent(event);
      }
      DLLLOCAL virtual void childEvent(QChildEvent * event)
      {
	 qobj->parent_childEvent(event);
      }
      DLLLOCAL virtual bool deleteBlocker() 
      {
	 //printd(5, "deleteBlocker() this=%08p qobj=%08p parent=%08p\n", this, &(*qobj), qobj ? qobj->parent() : 0);
	 return qobj ? qobj->deleteBlocker() : false;
      }
      DLLLOCAL virtual void setExternallyOwned()
      {
	 qobj->setExternallyOwned();
      }

};

// template for private data classes based on QObject corresponding to the QoreAbstractQObject API
template<typename T, typename V>
class QoreQtQObjectPrivateBase : public V
{
   public:
      QPointer<T> qobj;
      QoreObject *qore_obj;

      DLLLOCAL QoreQtQObjectPrivateBase(QoreObject *obj, T *qo, bool n_managed = false) : qobj(qo), qore_obj(obj)
      {
      }

      DLLLOCAL virtual bool deleteBlocker() { return false; }

      DLLLOCAL virtual class QObject *getQObject() const
      {
	 return &*qobj;
      }

      DLLLOCAL virtual QoreObject* getQoreObject() const
      {
	 return qore_obj;
      }

      DLLLOCAL virtual int getSlotIndex(const QByteArray &theSlot, ExceptionSink *xsink)
      {
         return qobj->metaObject()->indexOfSlot(theSlot);
      }

      DLLLOCAL virtual int getSignalIndex(const QByteArray &theSignal) const
      {
         return qobj->metaObject()->indexOfSignal(theSignal);
      }

      DLLLOCAL virtual int createSignal(const char *sig, ExceptionSink *xsink)
      {
         xsink->raiseException("DYNAMIC-SIGNAL-ERROR", "this class does not support dynamic signals");
         return -1;
      }

      DLLLOCAL virtual int connectDynamic(QoreAbstractQObject *sender, const char *signal, const char *slot, ExceptionSink *xsink)
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

      DLLLOCAL virtual void emit_signal(const char *sig, const QoreListNode *args)
      {
/*
         QByteArray theSignal = QMetaObject::normalizedSignature(sig);   
         int id = qobj->metaObject()->indexOfSignal(theSignal);

         if (id < 0)
            return;
	 QMetaMethod qmm = qobj->metaObject()->method(id);
         printd(5, "%s::emit_signal(%s, %08p) static signal %d\n", qobj->metaObject()->className(), sig, args, id);
*/
      }

      DLLLOCAL virtual QObject *sender() const
      {
         return 0;
      }

      // these are protected functions that therefore will never be called
      DLLLOCAL virtual bool event(QEvent *event) { return false; }	
      DLLLOCAL virtual void timerEvent(QTimerEvent * event) {}
      DLLLOCAL virtual void childEvent(QChildEvent * event) {}

      DLLLOCAL virtual void setExternallyOwned()
      {
	 assert(false);
      }
};

template<typename T, typename V>
class QoreQtQObjectBase : public QoreQtQObjectPrivateBase<T, V>
{
   protected:
      bool managed;

   public:
      DLLLOCAL QoreQtQObjectBase(QoreObject *obj, T *qo, bool n_managed = true) : QoreQtQObjectPrivateBase<T, V>(obj, qo), managed(n_managed)
      {
      }

      DLLLOCAL virtual ~QoreQtQObjectBase()
      {
	 // only delete the QObject if it is still a valid pointer and it is not a child of someone else
	 if (managed && this->qobj && !this->qobj->parent())
	    delete this->qobj;
      }
};

#endif
