
#if 0
class T {
#endif

   protected:
      DLLLOCAL virtual bool event(QEvent *event)
      {
	 //printd(5, QLSTR(QOREQTYPE) "::event(%08p) this=%08p func=%08p type=%d qore_obj=%08p\n", event, this, e_paintEvent, (int)event->getType()(), qore_obj);
	 if (!e_event || !qore_obj)
	    return QOREQTYPE::event(event);

	 ExceptionSink xsink;
         ReferenceHolder<QoreListNode> args(new QoreListNode(), &xsink);
	 args->push(return_qevent(event));

	 return dispatch_event_bool(qore_obj, e_event, *args, &xsink);
      }

#ifndef QORE_NO_TIMER_EVENT
      DLLLOCAL virtual void timerEvent(QTimerEvent * event)
      {
	 if (!e_timerEvent || !qore_obj) {
	    QOREQTYPE::timerEvent(event);
	    return;
	 }

	 dispatch_event(qore_obj, e_timerEvent, QC_QTimerEvent, new QoreQTimerEvent(*event));
      }
#endif

      DLLLOCAL virtual void childEvent(QChildEvent * event)
      {
	 if (!e_childEvent || !qore_obj) {
	    QOREQTYPE::childEvent(event);
	    return;
	 }

	 dispatch_event(qore_obj, e_childEvent, QC_QChildEvent, new QoreQChildEvent(*event));
      }

   public:
      DLLLOCAL virtual ~MYQOREQTYPE()
      {
	 //printd(5, "destructor() this=%08p (%s) qore_obj=%08p this=%08p parent=%08p obj_ref=%d\n", this, metaObject()->className(), qore_obj, this, QOREQTYPE::parent(), obj_ref);
	 
	 if (obj_ref) {
	    ExceptionSink xsink;
	    //printd(5, "QoreQObjectExtension::~QoreQObjectExtension() deleting object of class %s\n", qore_obj->getClassName());
	    obj_ref = false;
	    // delete the object if necessary (if not already in the destructor)
	    if (qore_obj->isValid()) 
	       qore_obj->doDelete(&xsink);
	    qore_obj->deref(&xsink);
	 }
	 qore_obj->tDeref();
      }

      DLLLOCAL virtual bool deleteBlocker()
      {
	 //printd(5, "deleteBlocker() %s returning %s\n", metaObject()->className(), QOREQTYPE::parent() ? "true" : "false");
	 if (QOREQTYPE::parent() || externally_owned) {
	    if (!obj_ref) {
	       obj_ref = true;
	       qore_obj->ref();
	    }
	    return true;
	 }

	 return false;
      }

      // the associated QoreObject is being deleted
      DLLLOCAL virtual void detach()
      {
	 //printd(5, "detach() this=%08p (%s) qore_obj=%08p obj_ref=%d parent=%08p\n", this, metaObject()->className(), qore_obj, obj_ref, QOREQTYPE::parent());
	 if (obj_ref) {
	    obj_ref = false;
	    ExceptionSink xsink;
	    qore_obj->deref(&xsink);
	 }
	 if (!QOREQTYPE::parent()) {	    
	    if (!externally_owned)
	       delete this;
	 }
	 //printd(5, "detach() exiting\n");
      }

      DLLLOCAL virtual bool parent_event(QEvent *event)
      {
	 return QOREQTYPE::event(event);
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
	 if (!qore_obj || id < 0 || call != QMetaObject::InvokeMetaMethod)
            return id;

	 // process "destroyed" signal
	 if (!id) {
	    assert(false);
	    return -1;
	 }

#ifdef QORE_QT_METACALL
	 QORE_QT_METACALL
#undef QORE_QT_METACALL
#endif

	 //printd(5, "%s::qt_metacall() id=%d (%s)\n", metaObject()->className(), id, i == methodMap.end() ? "not found" : "found"));
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

      DLLLOCAL QObject *getSender() const
      {
	 return sender();
      }
      
#if 0
}
#endif
