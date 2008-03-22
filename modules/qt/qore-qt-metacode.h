
#if 0
class T {
#endif

   private:
      QoreObject *qore_obj;                      // containing qore object for dispatching to qore code
      DynamicMethodMap methodMap;            // dynamic method manager
      QHash<QByteArray, int> slotIndices;    // map slot signatures to indices in the methodMap
      QHash<QByteArray, int> signalIndices;  // map signal signatures to signal IDs

      DLLLOCAL void init(QoreObject *obj)
      {
	 qore_obj = 0;

	 // set pointer to object owner as a property
	 setProperty("qobject", reinterpret_cast<qulonglong>(obj));

	 qore_obj = obj;
	 // reference container object for the lifetime of this object
	 qore_obj->tRef();
	 
	 // create dummy slot entry for "destroyed" signal
	 methodMap.addMethod(new QoreQtDynamicSlot(qore_obj, 0, 0));

	 // catch destroyed signal
	 QByteArray theSignal = QMetaObject::normalizedSignature("destroyed()");
	 int signalId = metaObject()->indexOfSignal(theSignal);
	 //printd(5, "sigid=%d %s\n", signalId, theSignal.data());
	 assert(signalId >= 0);
	 QMetaObject::connect(this, signalId, this, metaObject()->methodCount());
     }

   protected:
#ifndef QORE_NO_TIMER_EVENT
      DLLLOCAL virtual void timerEvent(QTimerEvent * event)
      {
	 if (!e_timerEvent) {
	    QOREQTYPE::timerEvent(event);
	    return;
	 }

	 dispatch_event(qore_obj, e_timerEvent, QC_QTimerEvent, new QoreQTimerEvent(*event));
      }
#endif

      DLLLOCAL virtual void childEvent(QChildEvent * event)
      {
	 if (!e_childEvent) {
	    QOREQTYPE::childEvent(event);
	    return;
	 }

	 dispatch_event(qore_obj, e_childEvent, QC_QChildEvent, new QoreQChildEvent(*event));
      }

   public:

      DLLLOCAL MYQOREQTYPE::~MYQOREQTYPE()
      {
	 qore_obj->tDeref();
      }

      DLLLOCAL virtual void parent_timerEvent(QTimerEvent * event)
      {
	 QOREQTYPE::timerEvent(event);
      }

      DLLLOCAL virtual void parent_childEvent(QChildEvent * event)
      {
	 QOREQTYPE::childEvent(event);
      }
      
      DLLLOCAL virtual int qt_metacall(QMetaObject::Call call, int id, void **arguments)
      {
	 int nid = QOREQTYPE::qt_metacall(call, id, arguments);

	 printd(5, "%s::qt_metacall(call=%d, id=%d, arguments=%08p) this=%08p new id=%d (dynamic methods: %d, static methods: %d)\n", metaObject()->className(), call, id, arguments, this, nid, methodMap.size(), metaObject()->methodCount());
	 id = nid;
	 if (id < 0 || call != QMetaObject::InvokeMetaMethod)
            return id;

	 // process "destroyed" signal
	 if (!id) {
	    assert(false);
	    //ExceptionSink xsink;
	    //qore_obj->deref(&xsink);
	    qore_obj->tDeref();
	    return -1;
	 }

#ifdef QORE_QT_METACALL
	 QORE_QT_METACALL
#undef QORE_QT_METACALL
#endif

	 //printd(5, "%s::qt_metacall() id=%d (%s)\n", metaObject()->className(), id, i == methodMap.end() ? "not found" : (i->second->getSlot() ? "slot" : "signal"));
	 assert(id < (int)methodMap.size());

	 QoreQtDynamicSlot *slot = dynamic_cast<QoreQtDynamicSlot *>(methodMap[id]);
	 if (slot)
	 {
	    slot->call(arguments);
	 }
	 else
	 {
	    id += metaObject()->methodCount();
	    // warning: this function is listed as "internal"
	    QMetaObject::activate(this, id, id, arguments);
	 }

	 return -1;
      }

      DLLLOCAL bool connectDynamic(QoreAbstractQObject *sender, const char *signal, const char *slot, class ExceptionSink *xsink)
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

	 if (!QMetaObject::connect(sender->getQObject(), signalId, (QObject *)this, targetId)) {
	    xsink->raiseException("QT-CONNECT-ERROR", "connection from signal '%s' to '%s' failed", signal + 1, slot + 1);
	    return -1;
	 }
	 //printd(5, "%s::connectDynamic(sender=%08p, signal=%d:%s, receiver=%d:%s) this=%08p success\n", metaObject()->className(), sender, signalId, signal, targetId, slot, this);
	 return 0;
      }

      // returns 0: OK, -1: error
      DLLLOCAL int createDynamicSignal(const char *signal, class ExceptionSink *xsink)
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
	 //printd(5, "%s::createDynamicSignal() this=%08p id=%d, method_id=%d: '%s'\n", metaObject()->className(), this, signalId, signalId + metaObject()->methodCount(), signal);
	 return 0;
      }

      DLLLOCAL virtual QoreQtDynamicSlot *getSlot(const char *sig, class ExceptionSink *xsink)
      {
	 QByteArray theSlot = QMetaObject::normalizedSignature(sig);
	 int index = getSlotIndex(theSlot, xsink);
	 if (*xsink)
	    return 0;
	 return dynamic_cast<QoreQtDynamicSlot *>(methodMap[index]);	 
      }

      DLLLOCAL int getSlotIndex(const QByteArray &theSlot, class ExceptionSink *xsink)
      {
	 int slotId = metaObject()->indexOfSlot(theSlot);
	 // see if it's a static slot
	 if (slotId >= 0) {
	    //printd(5, "%s:getSlotIndex('%s') is static (%d) this=%08p\n", metaObject()->className(), theSlot.data(), slotId, this);
	    return slotId;
	 }

	 // see if it's an existing dynamic slot
	 slotId = slotIndices.value(theSlot, -1);
	 if (slotId >= 0) {
	    //printd(5, "%s:getSlotIndex('%s') is dynamic (%d methodid %d) this=%08p\n", metaObject()->className(), theSlot.data(), slotId, slotId + metaObject()->methodCount(), this);
	    return slotId + metaObject()->methodCount();
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
	 //printd(5, "%s::getSlotIndex() this=%08p created new dynamic slot, id=%d method_id=%d: '%s'\n", metaObject()->className(), this, slotId, slotId + metaObject()->methodCount(), theSlot.data());

	 return slotId + metaObject()->methodCount();
      }

      DLLLOCAL int getSignalIndex(const QByteArray &theSignal) const
      {
	 int signalId = metaObject()->indexOfSignal(theSignal);
	 
	 if (signalId >= 0) {
	    //printd(5, "%s:getSignalIndex('%s') this=%08p is static (%d)\n", metaObject()->className(), theSignal.data(), this, signalId);
	    return signalId;
	 }

	 signalId = signalIndices.value(theSignal, -1);
	 if (signalId < 0)
	    return -1;
	 
	 //printd(5, "%s:getSignalIndex('%s') this=%08p is dynamic (%d methodid %d)\n", metaObject()->className(), theSignal.data(), this, signalId, signalId + metaObject()->methodCount());
	 return signalId + metaObject()->methodCount();//QORE_SIGNAL_START;
      }

      // emits a signal; args are offset from 1
      DLLLOCAL void emit_signal(const char *sig, const QoreListNode *args)
      {
	 QByteArray theSignal = QMetaObject::normalizedSignature(sig);	 
	 int id = metaObject()->indexOfSignal(theSignal);

	 if (id >= 0) {
	    QMetaMethod qmm = metaObject()->method(id);
	    //printd(5, "%s::emit_signal(%s, %08p) static signal %d\n", metaObject()->className(), sig, args, id);
	    
	    emit_static_signal(this, id, qmm, args);
	 }
	 else { // emit dynamic signal
	    int signalId = signalIndices.value(theSignal, -1);
	    if (signalId < 0)
	       return;
	    QoreQtDynamicSignal *sp = dynamic_cast<QoreQtDynamicSignal *>(methodMap[signalId]);
	    sp->emit_signal(this, signalId + metaObject()->methodCount(), args);
	 }
      }

      DLLLOCAL virtual QoreObject *getQoreObject() const
      {
	 return qore_obj;
      }

      DLLLOCAL QObject *getSender() const
      {
	 return sender();
      }

#undef MYQOREQTYPE
      
#if 0
}
#endif
