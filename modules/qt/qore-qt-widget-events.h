
#define _QLSTR(x) #x
#define QLSTR(x) _QLSTR(x)

#if 0
class T {
#endif

   protected:
      DLLLOCAL virtual void paintEvent(QPaintEvent *event) 
      {
	 //printd(0, QLSTR(QOREQTYPE) "::paintEvent(%08p) this=%08p func=%08p\n", event, this, e_paintEvent);

	 if (!e_paintEvent) {
	    QOREQTYPE::paintEvent(event);
	    return;
	 }
	 dispatch_event(qore_obj, e_paintEvent, QC_QPaintEvent, new QoreQPaintEvent(*event));
      }
      DLLLOCAL virtual void mouseMoveEvent(QMouseEvent *event) 
      {
	 if (!e_mouseMoveEvent) {
	    QOREQTYPE::mouseMoveEvent(event);
	    return;
	 }

	 ExceptionSink xsink;
	 QoreQMouseEvent *e = new QoreQMouseEvent(*event);
	 // reference so we can copy value after method has been run
	 e->ref();
	 // holder for extra reference
	 ReferenceHolder<AbstractPrivateData> eventHolder(e, &xsink);
	 dispatch_event(qore_obj, e_mouseMoveEvent, QC_QMouseEvent, e);

	 *event = *e;
      }
      DLLLOCAL virtual void mousePressEvent(QMouseEvent *event) 
      {
	 if (!e_mousePressEvent) {
	    QOREQTYPE::mousePressEvent(event);
	    return;
	 }

	 ExceptionSink xsink;
	 QoreQMouseEvent *e = new QoreQMouseEvent(*event);
	 // reference so we can copy value after method has been run
	 e->ref();
	 // holder for extra reference
	 ReferenceHolder<AbstractPrivateData> eventHolder(e, &xsink);
	 dispatch_event(qore_obj, e_mousePressEvent, QC_QMouseEvent, e);

	 *event = *e;
      }
      DLLLOCAL virtual void mouseReleaseEvent(QMouseEvent *event) 
      {
	 if (!e_mouseReleaseEvent) {
	    QOREQTYPE::mouseReleaseEvent(event);
	    return;
	 }

	 ExceptionSink xsink;
	 QoreQMouseEvent *e = new QoreQMouseEvent(*event);
	 // reference so we can copy value after method has been run
	 e->ref();
	 // holder for extra reference
	 ReferenceHolder<AbstractPrivateData> eventHolder(e, &xsink);
	 dispatch_event(qore_obj, e_mouseReleaseEvent, QC_QMouseEvent, e);

	 *event = *e;
      }
      DLLLOCAL virtual void mouseDoubleClickEvent(QMouseEvent *event) 
      {
	 if (!e_mouseDoubleClickEvent) {
	    QOREQTYPE::mouseDoubleClickEvent(event);
	    return;
	 }

	 ExceptionSink xsink;
	 QoreQMouseEvent *e = new QoreQMouseEvent(*event);
	 // reference so we can copy value after method has been run
	 e->ref();
	 // holder for extra reference
	 ReferenceHolder<AbstractPrivateData> eventHolder(e, &xsink);
	 dispatch_event(qore_obj, e_mouseDoubleClickEvent, QC_QMouseEvent, e);

	 *event = *e;
      }
      DLLLOCAL virtual void keyPressEvent(QKeyEvent *event) 
      {
	 if (!e_keyPressEvent) {
	    QOREQTYPE::keyPressEvent(event);
	    return;
	 }

	 ExceptionSink xsink;
	 QoreQKeyEvent *e = new QoreQKeyEvent(*event);
	 // reference so we can copy value after method has been run
	 e->ref();
	 // holder for extra reference
	 ReferenceHolder<AbstractPrivateData> eventHolder(e, &xsink);
	 dispatch_event(qore_obj, e_keyPressEvent, QC_QKeyEvent, e);

	 *event = *e;
      }
      DLLLOCAL virtual void keyReleaseEvent(QKeyEvent *event) 
      {
	 if (!e_keyReleaseEvent) {
	    QOREQTYPE::keyReleaseEvent(event);
	    return;
	 }

	 ExceptionSink xsink;
	 QoreQKeyEvent *e = new QoreQKeyEvent(*event);
	 // reference so we can copy value after method has been run
	 e->ref();
	 // holder for extra reference
	 ReferenceHolder<AbstractPrivateData> eventHolder(e, &xsink);
	 dispatch_event(qore_obj, e_keyReleaseEvent, QC_QKeyEvent, e);

	 *event = *e;
      }

      DLLLOCAL virtual void changeEvent(QEvent *event)
      {
	 if (!e_changeEvent) {
	    QOREQTYPE::changeEvent(event);
	    return;
	 }

	 ExceptionSink xsink;
	 QoreQEvent *e = new QoreQEvent(*event);
	 // reference so we can copy value after method has been run
	 e->ref();
	 // holder for extra reference
	 ReferenceHolder<AbstractPrivateData> eventHolder(e, &xsink);
	 dispatch_event(qore_obj, e_changeEvent, QC_QEvent, e);

	 *event = *e;
      }

      DLLLOCAL virtual void enterEvent(QEvent *event)
      {
	 if (!e_enterEvent) {
	    QOREQTYPE::enterEvent(event);
	    return;
	 }

	 ExceptionSink xsink;
	 QoreQEvent *e = new QoreQEvent(*event);
	 // reference so we can copy value after method has been run
	 e->ref();
	 // holder for extra reference
	 ReferenceHolder<AbstractPrivateData> eventHolder(e, &xsink);
	 dispatch_event(qore_obj, e_enterEvent, QC_QEvent, e);

	 *event = *e;
      }
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

      DLLLOCAL virtual void leaveEvent(QEvent *event)
      {
	 if (!e_leaveEvent) {
	    QOREQTYPE::leaveEvent(event);
	    return;
	 }

	 ExceptionSink xsink;
	 QoreQEvent *e = new QoreQEvent(*event);
	 // reference so we can copy value after method has been run
	 e->ref();
	 // holder for extra reference
	 ReferenceHolder<AbstractPrivateData> eventHolder(e, &xsink);
	 dispatch_event(qore_obj, e_leaveEvent, QC_QEvent, e);

	 *event = *e;
      }

      DLLLOCAL virtual void resizeEvent(QResizeEvent *event)
      {
	 if (!e_resizeEvent) {
	    QOREQTYPE::resizeEvent(event);
	    return;
	 }

	 ExceptionSink xsink;
	 QoreQResizeEvent *e = new QoreQResizeEvent(*event);
	 // reference so we can copy value after method has been run
	 e->ref();
	 // holder for extra reference
	 ReferenceHolder<AbstractPrivateData> eventHolder(e, &xsink);
	 dispatch_event(qore_obj, e_resizeEvent, QC_QResizeEvent, e);

	 *event = *e;
      }

      DLLLOCAL virtual void moveEvent(QMoveEvent *event)
      {
	 if (!e_moveEvent) {
	    QOREQTYPE::moveEvent(event);
	    return;
	 }

	 ExceptionSink xsink;
	 QoreQMoveEvent *e = new QoreQMoveEvent(*event);
	 // reference so we can copy value after method has been run
	 e->ref();
	 // holder for extra reference
	 ReferenceHolder<AbstractPrivateData> eventHolder(e, &xsink);
	 dispatch_event(qore_obj, e_moveEvent, QC_QMoveEvent, e);

	 *event = *e;
      }

      DLLLOCAL virtual void actionEvent(QActionEvent *event)
      {
	 if (!e_actionEvent) {
	    QOREQTYPE::actionEvent(event);
	    return;
	 }

	 ExceptionSink xsink;
	 QoreQActionEvent *e = new QoreQActionEvent(*event);
	 // reference so we can copy value after method has been run
	 e->ref();
	 // holder for extra reference
	 ReferenceHolder<AbstractPrivateData> eventHolder(e, &xsink);
	 dispatch_event(qore_obj, e_actionEvent, QC_QActionEvent, e);

	 *event = *e;
      }

      DLLLOCAL virtual void closeEvent(QCloseEvent *event)
      {
	 if (!e_closeEvent) {
	    QOREQTYPE::closeEvent(event);
	    return;
	 }

	 ExceptionSink xsink;

	 QoreQCloseEvent *e = new QoreQCloseEvent(*event);
	 // reference so we can copy value after method has been run
	 e->ref();
	 // holder for extra reference
	 ReferenceHolder<AbstractPrivateData> eventHolder(e, &xsink);

	 dispatch_event(qore_obj, e_closeEvent, QC_QCloseEvent, e);

	 *event = *e;
      }

      DLLLOCAL virtual void contextMenuEvent(QContextMenuEvent *event)
      {
	 if (!e_contextMenuEvent) {
	    QOREQTYPE::contextMenuEvent(event);
	    return;
	 }

	 ExceptionSink xsink;
	 QoreQContextMenuEvent *e = new QoreQContextMenuEvent(*event);
	 // reference so we can copy value after method has been run
	 e->ref();
	 // holder for extra reference
	 ReferenceHolder<AbstractPrivateData> eventHolder(e, &xsink);
	 dispatch_event(qore_obj, e_contextMenuEvent, QC_QContextMenuEvent, e);

	 *event = *e;
      }

      DLLLOCAL virtual void dragEnterEvent(QDragEnterEvent *event)
      {
	 if (!e_dragEnterEvent) {
	    QOREQTYPE::dragEnterEvent(event);
	    return;
	 }

	 ExceptionSink xsink;
	 QoreQDragEnterEvent *e = new QoreQDragEnterEvent(*event);
	 // reference so we can copy value after method has been run
	 e->ref();
	 // holder for extra reference
	 ReferenceHolder<AbstractPrivateData> eventHolder(e, &xsink);
	 dispatch_event(qore_obj, e_dragEnterEvent, QC_QDragEnterEvent, e);

	 *event = *e;
      }

      DLLLOCAL virtual void dragMoveEvent(QDragMoveEvent *event)
      {
	 if (!e_dragMoveEvent) {
	    QOREQTYPE::dragMoveEvent(event);
	    return;
	 }

	 ExceptionSink xsink;
	 QoreQDragMoveEvent *e = new QoreQDragMoveEvent(*event);
	 // reference so we can copy value after method has been run
	 e->ref();
	 // holder for extra reference
	 ReferenceHolder<AbstractPrivateData> eventHolder(e, &xsink);
	 dispatch_event(qore_obj, e_dragMoveEvent, QC_QDragMoveEvent, e);

	 *event = *e;
      }

      DLLLOCAL virtual void dragLeaveEvent(QDragLeaveEvent *event)
      {
	 if (!e_dragLeaveEvent) {
	    QOREQTYPE::dragLeaveEvent(event);
	    return;
	 }

	 ExceptionSink xsink;
	 QoreQDragLeaveEvent *e = new QoreQDragLeaveEvent(*event);
	 // reference so we can copy value after method has been run
	 e->ref();
	 // holder for extra reference
	 ReferenceHolder<AbstractPrivateData> eventHolder(e, &xsink);
	 dispatch_event(qore_obj, e_dragLeaveEvent, QC_QDragLeaveEvent, e);

	 *event = *e;
      }

      DLLLOCAL virtual void dropEvent(QDropEvent *event)
      {
	 if (!e_dropEvent) {
	    QOREQTYPE::dropEvent(event);
	    return;
	 }

	 ExceptionSink xsink;
	 QoreQDropEvent *e = new QoreQDropEvent(*event);
	 // reference so we can copy value after method has been run
	 e->ref();
	 // holder for extra reference
	 ReferenceHolder<AbstractPrivateData> eventHolder(e, &xsink);
	 dispatch_event(qore_obj, e_dropEvent, QC_QDropEvent, e);

	 *event = *e;
      }

      DLLLOCAL virtual void focusInEvent(QFocusEvent *event)
      {
	 if (!e_focusInEvent) {
	    QOREQTYPE::focusInEvent(event);
	    return;
	 }

	 ExceptionSink xsink;
	 QoreQFocusEvent *e = new QoreQFocusEvent(*event);
	 // reference so we can copy value after method has been run
	 e->ref();
	 // holder for extra reference
	 ReferenceHolder<AbstractPrivateData> eventHolder(e, &xsink);
	 dispatch_event(qore_obj, e_focusInEvent, QC_QFocusEvent, e);

	 *event = *e;
      }

      DLLLOCAL virtual void focusOutEvent(QFocusEvent *event)
      {
	 if (!e_focusOutEvent) {
	    QOREQTYPE::focusOutEvent(event);
	    return;
	 }

	 ExceptionSink xsink;
	 QoreQFocusEvent *e = new QoreQFocusEvent(*event);
	 // reference so we can copy value after method has been run
	 e->ref();
	 // holder for extra reference
	 ReferenceHolder<AbstractPrivateData> eventHolder(e, &xsink);
	 dispatch_event(qore_obj, e_focusOutEvent, QC_QFocusEvent, e);

	 *event = *e;
      }

      DLLLOCAL virtual void hideEvent(QHideEvent *event)
      {
	 if (!e_hideEvent) {
	    QOREQTYPE::hideEvent(event);
	    return;
	 }

	 ExceptionSink xsink;
	 QoreQHideEvent *e = new QoreQHideEvent(*event);
	 // reference so we can copy value after method has been run
	 e->ref();
	 // holder for extra reference
	 ReferenceHolder<AbstractPrivateData> eventHolder(e, &xsink);
	 dispatch_event(qore_obj, e_hideEvent, QC_QHideEvent, e);

	 *event = *e;
      }

      DLLLOCAL virtual void inputMethodEvent(QInputMethodEvent *event)
      {
	 if (!e_inputMethodEvent) {
	    QOREQTYPE::inputMethodEvent(event);
	    return;
	 }

	 ExceptionSink xsink;
	 QoreQInputMethodEvent *e = new QoreQInputMethodEvent(*event);
	 // reference so we can copy value after method has been run
	 e->ref();
	 // holder for extra reference
	 ReferenceHolder<AbstractPrivateData> eventHolder(e, &xsink);
	 dispatch_event(qore_obj, e_inputMethodEvent, QC_QInputMethodEvent, e);

	 *event = *e;
      }

      DLLLOCAL virtual void showEvent(QShowEvent *event)
      {
	 if (!e_showEvent) {
	    QOREQTYPE::showEvent(event);
	    return;
	 }

	 ExceptionSink xsink;
	 QoreQShowEvent *e = new QoreQShowEvent(*event);
	 // reference so we can copy value after method has been run
	 e->ref();
	 // holder for extra reference
	 ReferenceHolder<AbstractPrivateData> eventHolder(e, &xsink);
	 dispatch_event(qore_obj, e_showEvent, QC_QShowEvent, e);

	 *event = *e;
      }

      DLLLOCAL virtual void tabletEvent(QTabletEvent *event)
      {
	 if (!e_tabletEvent) {
	    QOREQTYPE::tabletEvent(event);
	    return;
	 }

	 ExceptionSink xsink;
	 QoreQTabletEvent *e = new QoreQTabletEvent(*event);
	 // reference so we can copy value after method has been run
	 e->ref();
	 // holder for extra reference
	 ReferenceHolder<AbstractPrivateData> eventHolder(e, &xsink);
	 dispatch_event(qore_obj, e_tabletEvent, QC_QTabletEvent, e);

	 *event = *e;
      }
      
      DLLLOCAL virtual void wheelEvent(QWheelEvent *event)
      {
	 if (!e_wheelEvent) {
	    QOREQTYPE::wheelEvent(event);
	    return;
	 }

	 ExceptionSink xsink;
	 QoreQWheelEvent *e = new QoreQWheelEvent(*event);
	 // reference so we can copy value after method has been run
	 e->ref();
	 // holder for extra reference
	 ReferenceHolder<AbstractPrivateData> eventHolder(e, &xsink);
	 dispatch_event(qore_obj, e_wheelEvent, QC_QWheelEvent, e);

	 *event = *e;
      }

      /*
      DLLLOCAL virtual HDC getDC () const
      {
         if (!p_getDC)
	    return QOREQTYPE::getDC();
      }
      */
      
      DLLLOCAL virtual int heightForWidth ( int w ) const
      {
	 if (!p_heightForWidth)
	    return QOREQTYPE::heightForWidth(w);

	 ExceptionSink xsink;
	 ReferenceHolder<QoreListNode> args(new QoreListNode(), &xsink);
	 args->push(new QoreBigIntNode(w));
	 
	 return dispatch_event_int(qore_obj, p_heightForWidth, *args, &xsink);
      }

      DLLLOCAL virtual QVariant inputMethodQuery ( Qt::InputMethodQuery query ) const
      {
	 if (!p_inputMethodQuery)
	    return QOREQTYPE::inputMethodQuery(query);

	 ExceptionSink xsink;
	 ReferenceHolder<QoreListNode> args(new QoreListNode(), &xsink);
	 args->push(new QoreBigIntNode(query));

	 ReferenceHolder<AbstractQoreNode> rv(dispatch_event_intern(qore_obj, p_inputMethodQuery, *args, &xsink), &xsink);
	 if (xsink)
	    return QOREQTYPE::inputMethodQuery(query);

	 QVariant qrv;
	 if (get_qvariant(*rv, qrv, &xsink))
	    return QOREQTYPE::inputMethodQuery(query);

	 return qrv;
      }

      /*
      DLLLOCAL virtual QPaintEngine * paintEngine () const
      {
	 if (!p_paintEngine)
	    return QOREQTYPE::paintEngine();
      }

      DLLLOCAL virtual void releaseDC ( HDC hdc ) const
      {
         if (!p_releaseDC) {
	    QOREQTYPE::releaseDC(hdc);
	    return;
	 }
      }
      */

      DLLLOCAL virtual void setVisible ( bool visible )
      {
	 if (!p_setVisible) {
	    QOREQTYPE::setVisible(visible);
	    return;
	 }
	 
	 ExceptionSink xsink;
	 ReferenceHolder<QoreListNode> args(new QoreListNode(), &xsink);
	 args->push(get_bool_node(visible));

	 discard(dispatch_event_intern(qore_obj, p_setVisible, *args, &xsink), &xsink);
      }

      DLLLOCAL virtual QSize sizeHint() const
      {
	 if (!p_sizeHint)
	    return QOREQTYPE::sizeHint();

	 ExceptionSink xsink;

	 // call sizeHint method
	 AbstractQoreNode *rv = p_sizeHint->eval(qore_obj, 0, &xsink);
	 if (xsink) {
	    discard(rv, &xsink);
	    return QOREQTYPE::sizeHint();
	 }
         QoreObject *o = dynamic_cast<QoreObject *>(rv);
	 QoreQSize *qs = o ? (QoreQSize *)o->getReferencedPrivateData(CID_QSIZE, &xsink) : 0;
	 discard(rv, &xsink);

	 if (!qs) {
	    xsink.raiseException("SIZEHINT-ERROR", "the sizeHint() method did not return a QSize object");
	    return QOREQTYPE::sizeHint();
	 }
	 ReferenceHolder<QoreQSize> sizeHolder(qs, &xsink);
	 QSize rv_qs = *(static_cast<QSize *>(qs));
	 return rv_qs;
      }

   public:
      DLLLOCAL virtual QSize minimumSizeHint () const
      {
	 if (!p_minimumSizeHint)
	    return QOREQTYPE::minimumSizeHint();

	 ExceptionSink xsink;

	 // call minimumSizeHint method
	 AbstractQoreNode *rv = p_minimumSizeHint->eval(qore_obj, 0, &xsink);
	 if (xsink) {
	    discard(rv, &xsink);
	    return QOREQTYPE::minimumSizeHint();
	 }
         QoreObject *o = dynamic_cast<QoreObject *>(rv);
	 QoreQSize *qs = o ? (QoreQSize *)o->getReferencedPrivateData(CID_QSIZE, &xsink) : 0;
	 discard(rv, &xsink);

	 if (!qs) {
	    xsink.raiseException("MINIMUMSIZEHINT-ERROR", "the minimumSizeHint() method did not return a QSize object");
	    return QOREQTYPE::minimumSizeHint();
	 }
	 ReferenceHolder<QoreQSize> sizeHolder(qs, &xsink);
	 QSize rv_qs = *(static_cast<QSize *>(qs));
	 return rv_qs;
      }

      DLLLOCAL virtual void parent_paintEvent(QPaintEvent *event) 
      {
	 QOREQTYPE::paintEvent(event);
      }

      DLLLOCAL virtual void parent_mouseMoveEvent(QMouseEvent *event) 
      {
	 QOREQTYPE::mouseMoveEvent(event);
      }

      DLLLOCAL virtual void parent_mousePressEvent(QMouseEvent *event) 
      {
	 QOREQTYPE::mousePressEvent(event);
      }

      DLLLOCAL virtual void parent_mouseReleaseEvent(QMouseEvent *event) 
      {
	 QOREQTYPE::mouseReleaseEvent(event);
      }

      DLLLOCAL virtual void parent_mouseDoubleClickEvent(QMouseEvent *event) 
      {
	 QOREQTYPE::mouseDoubleClickEvent(event);
      }

      DLLLOCAL virtual void parent_keyPressEvent(QKeyEvent *event) 
      {
	 QOREQTYPE::keyPressEvent(event);
      }

      DLLLOCAL virtual void parent_keyReleaseEvent(QKeyEvent *event) 
      {
	 QOREQTYPE::keyReleaseEvent(event);
      }

      DLLLOCAL virtual void parent_changeEvent(QEvent *event)
      {
	 QOREQTYPE::changeEvent(event);
      }

      DLLLOCAL virtual void parent_enterEvent(QEvent *event)
      {
	 QOREQTYPE::enterEvent(event);
      }

      DLLLOCAL virtual bool parent_event(QEvent *event)
      {
	 return QOREQTYPE::event(event);
      }

      DLLLOCAL virtual void parent_leaveEvent(QEvent *event)
      {
	 QOREQTYPE::leaveEvent(event);
      }

      DLLLOCAL virtual void parent_resizeEvent(QResizeEvent *event)
      {
	 QOREQTYPE::resizeEvent(event);
      }

      DLLLOCAL virtual void parent_moveEvent(QMoveEvent *event)
      {
	 QOREQTYPE::moveEvent(event);
      }

      DLLLOCAL virtual void parent_actionEvent(QActionEvent *event)
      {
	 QOREQTYPE::actionEvent(event);
      }

      DLLLOCAL virtual void parent_closeEvent(QCloseEvent *event)
      {
	 QOREQTYPE::closeEvent(event);
      }

      DLLLOCAL virtual void parent_contextMenuEvent(QContextMenuEvent *event)
      {
	 QOREQTYPE::contextMenuEvent(event);
      }

      DLLLOCAL virtual void parent_dragEnterEvent(QDragEnterEvent *event)
      {
	 QOREQTYPE::dragEnterEvent(event);
      }

      DLLLOCAL virtual void parent_dragMoveEvent(QDragMoveEvent *event)
      {
	 QOREQTYPE::dragMoveEvent(event);
      }

      DLLLOCAL virtual void parent_dragLeaveEvent(QDragLeaveEvent *event)
      {
	 QOREQTYPE::dragLeaveEvent(event);
      }

      DLLLOCAL virtual void parent_dropEvent(QDropEvent *event)
      {
	 QOREQTYPE::dropEvent(event);
      }

      DLLLOCAL virtual void parent_focusInEvent(QFocusEvent *event)
      {
	 QOREQTYPE::focusInEvent(event);
      }

      DLLLOCAL virtual void parent_focusOutEvent(QFocusEvent *event)
      {
	 QOREQTYPE::focusOutEvent(event);
      }

      DLLLOCAL virtual void parent_hideEvent(QHideEvent *event)
      {
	 QOREQTYPE::hideEvent(event);
      }

      DLLLOCAL virtual void parent_inputMethodEvent(QInputMethodEvent *event)
      {
	 QOREQTYPE::inputMethodEvent(event);
      }

      DLLLOCAL virtual void parent_showEvent(QShowEvent *event)
      {
	 QOREQTYPE::showEvent(event);
      }

      DLLLOCAL virtual void parent_tabletEvent(QTabletEvent *event)
      {
	 QOREQTYPE::tabletEvent(event);
      }

      DLLLOCAL virtual void parent_wheelEvent(QWheelEvent *event)
      {
	 QOREQTYPE::wheelEvent(event);
      }
      
      /*
      DLLLOCAL virtual HDC parent_getDC () const
      {
	 return QOREQTYPE::getDC();
      }
      */
      
      DLLLOCAL virtual int parent_heightForWidth ( int w ) const
      {
	 return QOREQTYPE::heightForWidth(w);
      }

      DLLLOCAL virtual QVariant parent_inputMethodQuery ( Qt::InputMethodQuery query ) const
      {
	 return QOREQTYPE::inputMethodQuery(query);
      }

      DLLLOCAL virtual QSize parent_minimumSizeHint () const
      {
	 return QOREQTYPE::minimumSizeHint();
      }

      /*
      DLLLOCAL virtual QPaintEngine * parent_paintEngine () const
      {
	 return QOREQTYPE::paintEngine();
      }

      DLLLOCAL virtual void parent_releaseDC ( HDC hdc ) const
      {
	 QOREQTYPE::releaseDC(hdc);
      }
      */

      DLLLOCAL virtual void parent_setVisible ( bool visible )
      {
	 QOREQTYPE::setVisible(visible);
      }

      DLLLOCAL virtual QSize parent_sizeHint() const
      {
	 return QOREQTYPE::sizeHint();
      }


#if 0
}
#endif

