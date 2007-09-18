
#define _QLSTR(x) #x
#define QLSTR(x) _QLSTR(x)

#if 0
class T {
#endif

  private:
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
	 //printd(5, "paintEvent this=%08p class=" QLSTR(QOREQTYPE) " func=%08p\n", this, e_paintEvent);
	 //printd(0, QLSTR(QOREQTYPE), this, e_paintEvent);

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

      DLLLOCAL virtual void actionEvent(QActionEvent *event)
      {
	 if (!e_actionEvent) {
	    QOREQTYPE::actionEvent(event);
	    return;
	 }

	 dispatch_event(e_actionEvent, QC_QActionEvent, new QoreQActionEvent(*event));
      }

      DLLLOCAL virtual void closeEvent(QCloseEvent *event)
      {
	 if (!e_closeEvent) {
	    QOREQTYPE::closeEvent(event);
	    return;
	 }

	 dispatch_event(e_closeEvent, QC_QCloseEvent, new QoreQCloseEvent(*event));
      }

      DLLLOCAL virtual void contextMenuEvent(QContextMenuEvent *event)
      {
	 if (!e_contextMenuEvent) {
	    QOREQTYPE::contextMenuEvent(event);
	    return;
	 }

	 dispatch_event(e_contextMenuEvent, QC_QContextMenuEvent, new QoreQContextMenuEvent(*event));
      }

      DLLLOCAL virtual void dragEnterEvent(QDragEnterEvent *event)
      {
	 if (!e_dragEnterEvent) {
	    QOREQTYPE::dragEnterEvent(event);
	    return;
	 }

	 dispatch_event(e_dragEnterEvent, QC_QDragEnterEvent, new QoreQDragEnterEvent(*event));
      }

      DLLLOCAL virtual void dragMoveEvent(QDragMoveEvent *event)
      {
	 if (!e_dragMoveEvent) {
	    QOREQTYPE::dragMoveEvent(event);
	    return;
	 }

	 dispatch_event(e_dragMoveEvent, QC_QDragMoveEvent, new QoreQDragMoveEvent(*event));
      }

      DLLLOCAL virtual void dragLeaveEvent(QDragLeaveEvent *event)
      {
	 if (!e_dragLeaveEvent) {
	    QOREQTYPE::dragLeaveEvent(event);
	    return;
	 }

	 dispatch_event(e_dragLeaveEvent, QC_QDragLeaveEvent, new QoreQDragLeaveEvent(*event));
      }

      DLLLOCAL virtual void dropEvent(QDropEvent *event)
      {
	 if (!e_dropEvent) {
	    QOREQTYPE::dropEvent(event);
	    return;
	 }

	 dispatch_event(e_dropEvent, QC_QDropEvent, new QoreQDropEvent(*event));
      }

      DLLLOCAL virtual void focusInEvent(QFocusEvent *event)
      {
	 if (!e_focusInEvent) {
	    QOREQTYPE::focusInEvent(event);
	    return;
	 }

	 dispatch_event(e_focusInEvent, QC_QFocusEvent, new QoreQFocusEvent(*event));
      }

      DLLLOCAL virtual void focusOutEvent(QFocusEvent *event)
      {
	 if (!e_focusOutEvent) {
	    QOREQTYPE::focusOutEvent(event);
	    return;
	 }

	 dispatch_event(e_focusOutEvent, QC_QFocusEvent, new QoreQFocusEvent(*event));
      }

      DLLLOCAL virtual void hideEvent(QHideEvent *event)
      {
	 if (!e_hideEvent) {
	    QOREQTYPE::hideEvent(event);
	    return;
	 }

	 dispatch_event(e_hideEvent, QC_QHideEvent, new QoreQHideEvent(*event));
      }

      DLLLOCAL virtual void inputMethodEvent(QInputMethodEvent *event)
      {
	 if (!e_inputMethodEvent) {
	    QOREQTYPE::inputMethodEvent(event);
	    return;
	 }

	 dispatch_event(e_inputMethodEvent, QC_QInputMethodEvent, new QoreQInputMethodEvent(*event));
      }

      DLLLOCAL virtual void showEvent(QShowEvent *event)
      {
	 if (!e_showEvent) {
	    QOREQTYPE::showEvent(event);
	    return;
	 }

	 dispatch_event(e_showEvent, QC_QShowEvent, new QoreQShowEvent(*event));
      }

      DLLLOCAL virtual void tabletEvent(QTabletEvent *event)
      {
	 if (!e_tabletEvent) {
	    QOREQTYPE::tabletEvent(event);
	    return;
	 }

	 dispatch_event(e_tabletEvent, QC_QTabletEvent, new QoreQTabletEvent(*event));
      }

      DLLLOCAL virtual void wheelEvent(QWheelEvent *event)
      {
	 if (!e_wheelEvent) {
	    QOREQTYPE::wheelEvent(event);
	    return;
	 }

	 dispatch_event(e_wheelEvent, QC_QWheelEvent, new QoreQWheelEvent(*event));
      }

      DLLLOCAL virtual QSize sizeHint() const
      {
	 if (!p_sizeHint)
	    return QOREQTYPE::sizeHint();

	 ExceptionSink xsink;

	 // call sizeHint method
	 QoreNode *rv = p_sizeHint->eval(qore_obj, 0, &xsink);
	 if (xsink)
	    return QOREQTYPE::sizeHint();

	 QoreQSize *qs = (rv && rv->type == NT_OBJECT) ? (QoreQSize *)rv->val.object->getReferencedPrivateData(CID_QSIZE, &xsink) : 0;
	 if (!qs) {
	    xsink.raiseException("SIZEHINT-ERROR", "the sizeHint() method did not return a QSize object");
	    return QOREQTYPE::sizeHint();
	 }
	 ReferenceHolder<QoreQSize> sizeHolder(qs, &xsink);
	 QSize rv_qs = *(static_cast<QSize *>(qs));
	 return rv_qs;
      }


#if 0
}
#endif

