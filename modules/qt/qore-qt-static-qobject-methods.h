
#if 0
class T {
#endif
      
   public:
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

      DLLLOCAL virtual void emit_signal(const char *sig, const QoreList *args)
      {
         QByteArray theSignal = QMetaObject::normalizedSignature(sig);   
         int id = qobj->metaObject()->indexOfSignal(theSignal);

         if (id < 0)
            return;
         QMetaMethod qmm = qobj->metaObject()->method(id);
         printd(5, "%s::emit_signal(%s, %08p) static signal %d\n", qobj->metaObject()->className(), sig, args, id);
      }

      DLLLOCAL virtual QoreQtDynamicSlot* getSlot(const char *slot, ExceptionSink *xsink)
      {
         xsink->raiseException("DYNAMIC-SLOT-ERROR", "this class does not support dynamic slots");
         return 0;
      }

      DLLLOCAL virtual QObject *sender() const
      {
         return 0;
      }

      // these are protected functions that therefore will never be called
      DLLLOCAL virtual void timerEvent(QTimerEvent * event) {}
      DLLLOCAL virtual void childEvent(QChildEvent * event) {}

#if 0
};
#endif
