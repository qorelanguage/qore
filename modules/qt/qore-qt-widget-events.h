
#if 0
class T {
#endif

  private:
/*
      // event methods
      Method *e_changeEvent, *e_enterEvent, *e_event, *e_leaveEvent,
	 *e_paintEvent, 
	 *e_mouseMoveEvent, *e_mousePressEvent, 
	 *e_mouseReleaseEvent, *e_mouseDoubleClickEvent,
	 *e_keyPressEvent, *e_keyReleaseEvent,
	 *e_resizeEvent,
	 *e_moveEvent;

      DLLLOCAL void init_widget_events()
      {
	 QoreClass *qc = qore_obj->getClass();
	 e_paintEvent             = qc->findMethod("paintEvent");
	 e_mouseMoveEvent         = qc->findMethod("mouseMoveEvent");
	 e_mousePressEvent        = qc->findMethod("mousePressEvent");
	 e_mouseReleaseEvent      = qc->findMethod("mouseReleaseEvent");
	 e_mouseDoubleClickEvent  = qc->findMethod("mouseDoubleClickEvent");
	 e_keyPressEvent          = qc->findMethod("keyPressEvent");
	 e_keyReleaseEvent        = qc->findMethod("keyReleaseEvent");
	 e_changeEvent            = qc->findMethod("changeEvent");
	 e_enterEvent             = qc->findMethod("enterEvent");
	 e_event                  = qc->findMethod("event");
	 e_leaveEvent             = qc->findMethod("leaveEvent");
	 e_resizeEvent            = qc->findMethod("resizeEvent");
	 e_moveEvent              = qc->findMethod("moveEvent");
      }
*/
      DLLLOCAL class QoreNode *dispatch_event_intern(class Method *m, QoreClass *qclass, class AbstractPrivateData *data, class ExceptionSink *xsink)
      {
	 // create argument list
	 Object *peo = new Object(qclass, getProgram());
	 peo->setPrivate(qclass->getID(), data);
	 QoreNode *a = new QoreNode(peo);
	 List *args = new List();
	 args->push(a);
	 QoreNode *na = new QoreNode(args);
	 
	 // call event method
	 QoreNode *rv = m->eval(qore_obj, na, xsink);
	 
	 // delete arguments
	 na->deref(xsink);

	 return rv;
      }

      DLLLOCAL void dispatch_event(class Method *m, QoreClass *qclass, class AbstractPrivateData *data)
      {
	 class ExceptionSink xsink;

	 discard(dispatch_event_intern(m, qclass, data, &xsink), &xsink);
      }

      DLLLOCAL bool dispatch_event_bool(class Method *m, QoreClass *qclass, class AbstractPrivateData *data)
      {
	 class ExceptionSink xsink;

	 QoreNode *rv = dispatch_event_intern(m, qclass, data, &xsink);
	 return rv ? rv->getAsBool() : false;
      }

   public:
      DLLLOCAL virtual void paintEvent(QPaintEvent *event) 
      {
	 if (!e_paintEvent) {
	    QOREQTYPE::paintEvent(event);
	    return;
	 }

	 dispatch_event(e_paintEvent, QC_QPaintEvent, new QoreQPaintEvent(*event));
      }
      DLLLOCAL virtual void mouseMoveEvent(QMouseEvent *event) 
      {
	 if (!e_mouseMoveEvent) {
	    QOREQTYPE::mouseMoveEvent(event);
	    return;
	 }

	 dispatch_event(e_mouseMoveEvent, QC_QMouseEvent, new QoreQMouseEvent(*event));
      }
      DLLLOCAL virtual void mousePressEvent(QMouseEvent *event) 
      {
	 if (!e_mousePressEvent) {
	    QOREQTYPE::mousePressEvent(event);
	    return;
	 }

	 dispatch_event(e_mousePressEvent, QC_QMouseEvent, new QoreQMouseEvent(*event));
      }
      DLLLOCAL virtual void mouseReleaseEvent(QMouseEvent *event) 
      {
	 if (!e_mouseReleaseEvent) {
	    QOREQTYPE::mouseReleaseEvent(event);
	    return;
	 }

	 dispatch_event(e_mouseReleaseEvent, QC_QMouseEvent, new QoreQMouseEvent(*event));
      }
      DLLLOCAL virtual void mouseDoubleClickEvent(QMouseEvent *event) 
      {
	 if (!e_mouseDoubleClickEvent) {
	    QOREQTYPE::mouseDoubleClickEvent(event);
	    return;
	 }

	 dispatch_event(e_mouseDoubleClickEvent, QC_QMouseEvent, new QoreQMouseEvent(*event));
      }
      DLLLOCAL virtual void keyPressEvent(QKeyEvent *event) 
      {
	 if (!e_keyPressEvent) {
	    QOREQTYPE::keyPressEvent(event);
	    return;
	 }

	 dispatch_event(e_keyPressEvent, QC_QKeyEvent, new QoreQKeyEvent(*event));
      }
      DLLLOCAL virtual void keyReleaseEvent(QKeyEvent *event) 
      {
	 if (!e_keyReleaseEvent) {
	    QOREQTYPE::keyReleaseEvent(event);
	    return;
	 }

	 dispatch_event(e_keyReleaseEvent, QC_QKeyEvent, new QoreQKeyEvent(*event));
      }

      DLLLOCAL virtual void changeEvent(QEvent *event)
      {
	 if (!e_changeEvent) {
	    QOREQTYPE::changeEvent(event);
	    return;
	 }

	 dispatch_event(e_changeEvent, QC_QEvent, new QoreQEvent(*event));
      }

      DLLLOCAL virtual void enterEvent(QEvent *event)
      {
	 if (!e_enterEvent) {
	    QOREQTYPE::enterEvent(event);
	    return;
	 }

	 dispatch_event(e_enterEvent, QC_QEvent, new QoreQEvent(*event));
      }
      DLLLOCAL virtual bool event(QEvent *event)
      {
	 if (!e_event)
	    return QOREQTYPE::event(event);

	 return dispatch_event_bool(e_event, QC_QEvent, new QoreQEvent(*event));
      }
      DLLLOCAL virtual void leaveEvent(QEvent *event)
      {
	 if (!e_leaveEvent) {
	    QOREQTYPE::leaveEvent(event);
	    return;
	 }

	 dispatch_event(e_leaveEvent, QC_QEvent, new QoreQEvent(*event));
      }

      DLLLOCAL virtual void resizeEvent(QResizeEvent *event)
      {
	 if (!e_resizeEvent) {
	    QOREQTYPE::resizeEvent(event);
	    return;
	 }

	 dispatch_event(e_resizeEvent, QC_QResizeEvent, new QoreQResizeEvent(*event));
      }

      DLLLOCAL virtual void moveEvent(QMoveEvent *event)
      {
	 if (!e_moveEvent) {
	    QOREQTYPE::moveEvent(event);
	    return;
	 }

	 dispatch_event(e_moveEvent, QC_QMoveEvent, new QoreQMoveEvent(*event));
      }

#if 0
}
#endif

