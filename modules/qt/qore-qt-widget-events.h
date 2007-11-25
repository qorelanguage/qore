
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

	 dispatch_event(qore_obj, e_mouseMoveEvent, QC_QMouseEvent, new QoreQMouseEvent(*event));
      }
      DLLLOCAL virtual void mousePressEvent(QMouseEvent *event) 
      {
	 if (!e_mousePressEvent) {
	    QOREQTYPE::mousePressEvent(event);
	    return;
	 }

	 dispatch_event(qore_obj, e_mousePressEvent, QC_QMouseEvent, new QoreQMouseEvent(*event));
      }
      DLLLOCAL virtual void mouseReleaseEvent(QMouseEvent *event) 
      {
	 if (!e_mouseReleaseEvent) {
	    QOREQTYPE::mouseReleaseEvent(event);
	    return;
	 }

	 dispatch_event(qore_obj, e_mouseReleaseEvent, QC_QMouseEvent, new QoreQMouseEvent(*event));
      }
      DLLLOCAL virtual void mouseDoubleClickEvent(QMouseEvent *event) 
      {
	 if (!e_mouseDoubleClickEvent) {
	    QOREQTYPE::mouseDoubleClickEvent(event);
	    return;
	 }

	 dispatch_event(qore_obj, e_mouseDoubleClickEvent, QC_QMouseEvent, new QoreQMouseEvent(*event));
      }
      DLLLOCAL virtual void keyPressEvent(QKeyEvent *event) 
      {
	 if (!e_keyPressEvent) {
	    QOREQTYPE::keyPressEvent(event);
	    return;
	 }

	 dispatch_event(qore_obj, e_keyPressEvent, QC_QKeyEvent, new QoreQKeyEvent(*event));
      }
      DLLLOCAL virtual void keyReleaseEvent(QKeyEvent *event) 
      {
	 if (!e_keyReleaseEvent) {
	    QOREQTYPE::keyReleaseEvent(event);
	    return;
	 }

	 dispatch_event(qore_obj, e_keyReleaseEvent, QC_QKeyEvent, new QoreQKeyEvent(*event));
      }

      DLLLOCAL virtual void changeEvent(QEvent *event)
      {
	 if (!e_changeEvent) {
	    QOREQTYPE::changeEvent(event);
	    return;
	 }

	 dispatch_event(qore_obj, e_changeEvent, QC_QEvent, new QoreQEvent(*event));
      }

      DLLLOCAL virtual void enterEvent(QEvent *event)
      {
	 if (!e_enterEvent) {
	    QOREQTYPE::enterEvent(event);
	    return;
	 }

	 dispatch_event(qore_obj, e_enterEvent, QC_QEvent, new QoreQEvent(*event));
      }
      DLLLOCAL virtual bool event(QEvent *event)
      {
	 //printd(5, QLSTR(QOREQTYPE) "::event(%08p) this=%08p func=%08p type=%d qore_obj=%08p\n", event, this, e_paintEvent, (int)event->type(), qore_obj);
	 if (!e_event || !qore_obj)
	    return QOREQTYPE::event(event);

	 QoreList *args = new QoreList();
	 args->push(return_qevent(event));

	 return dispatch_event_bool(qore_obj, e_event, args);
      }

      DLLLOCAL virtual void leaveEvent(QEvent *event)
      {
	 if (!e_leaveEvent) {
	    QOREQTYPE::leaveEvent(event);
	    return;
	 }

	 dispatch_event(qore_obj, e_leaveEvent, QC_QEvent, new QoreQEvent(*event));
      }

      DLLLOCAL virtual void resizeEvent(QResizeEvent *event)
      {
	 if (!e_resizeEvent) {
	    QOREQTYPE::resizeEvent(event);
	    return;
	 }

	 dispatch_event(qore_obj, e_resizeEvent, QC_QResizeEvent, new QoreQResizeEvent(*event));
      }

      DLLLOCAL virtual void moveEvent(QMoveEvent *event)
      {
	 if (!e_moveEvent) {
	    QOREQTYPE::moveEvent(event);
	    return;
	 }

	 dispatch_event(qore_obj, e_moveEvent, QC_QMoveEvent, new QoreQMoveEvent(*event));
      }

      DLLLOCAL virtual void actionEvent(QActionEvent *event)
      {
	 if (!e_actionEvent) {
	    QOREQTYPE::actionEvent(event);
	    return;
	 }

	 dispatch_event(qore_obj, e_actionEvent, QC_QActionEvent, new QoreQActionEvent(*event));
      }

      DLLLOCAL virtual void closeEvent(QCloseEvent *event)
      {
	 if (!e_closeEvent) {
	    QOREQTYPE::closeEvent(event);
	    return;
	 }

	 dispatch_event(qore_obj, e_closeEvent, QC_QCloseEvent, new QoreQCloseEvent(*event));
      }

      DLLLOCAL virtual void contextMenuEvent(QContextMenuEvent *event)
      {
	 if (!e_contextMenuEvent) {
	    QOREQTYPE::contextMenuEvent(event);
	    return;
	 }

	 dispatch_event(qore_obj, e_contextMenuEvent, QC_QContextMenuEvent, new QoreQContextMenuEvent(*event));
      }

      DLLLOCAL virtual void dragEnterEvent(QDragEnterEvent *event)
      {
	 if (!e_dragEnterEvent) {
	    QOREQTYPE::dragEnterEvent(event);
	    return;
	 }

	 dispatch_event(qore_obj, e_dragEnterEvent, QC_QDragEnterEvent, new QoreQDragEnterEvent(*event));
      }

      DLLLOCAL virtual void dragMoveEvent(QDragMoveEvent *event)
      {
	 if (!e_dragMoveEvent) {
	    QOREQTYPE::dragMoveEvent(event);
	    return;
	 }

	 dispatch_event(qore_obj, e_dragMoveEvent, QC_QDragMoveEvent, new QoreQDragMoveEvent(*event));
      }

      DLLLOCAL virtual void dragLeaveEvent(QDragLeaveEvent *event)
      {
	 if (!e_dragLeaveEvent) {
	    QOREQTYPE::dragLeaveEvent(event);
	    return;
	 }

	 dispatch_event(qore_obj, e_dragLeaveEvent, QC_QDragLeaveEvent, new QoreQDragLeaveEvent(*event));
      }

      DLLLOCAL virtual void dropEvent(QDropEvent *event)
      {
	 if (!e_dropEvent) {
	    QOREQTYPE::dropEvent(event);
	    return;
	 }

	 dispatch_event(qore_obj, e_dropEvent, QC_QDropEvent, new QoreQDropEvent(*event));
      }

      DLLLOCAL virtual void focusInEvent(QFocusEvent *event)
      {
	 if (!e_focusInEvent) {
	    QOREQTYPE::focusInEvent(event);
	    return;
	 }

	 dispatch_event(qore_obj, e_focusInEvent, QC_QFocusEvent, new QoreQFocusEvent(*event));
      }

      DLLLOCAL virtual void focusOutEvent(QFocusEvent *event)
      {
	 if (!e_focusOutEvent) {
	    QOREQTYPE::focusOutEvent(event);
	    return;
	 }

	 dispatch_event(qore_obj, e_focusOutEvent, QC_QFocusEvent, new QoreQFocusEvent(*event));
      }

      DLLLOCAL virtual void hideEvent(QHideEvent *event)
      {
	 if (!e_hideEvent) {
	    QOREQTYPE::hideEvent(event);
	    return;
	 }

	 dispatch_event(qore_obj, e_hideEvent, QC_QHideEvent, new QoreQHideEvent(*event));
      }

      DLLLOCAL virtual void inputMethodEvent(QInputMethodEvent *event)
      {
	 if (!e_inputMethodEvent) {
	    QOREQTYPE::inputMethodEvent(event);
	    return;
	 }

	 dispatch_event(qore_obj, e_inputMethodEvent, QC_QInputMethodEvent, new QoreQInputMethodEvent(*event));
      }

      DLLLOCAL virtual void showEvent(QShowEvent *event)
      {
	 if (!e_showEvent) {
	    QOREQTYPE::showEvent(event);
	    return;
	 }

	 dispatch_event(qore_obj, e_showEvent, QC_QShowEvent, new QoreQShowEvent(*event));
      }

      DLLLOCAL virtual void tabletEvent(QTabletEvent *event)
      {
	 if (!e_tabletEvent) {
	    QOREQTYPE::tabletEvent(event);
	    return;
	 }

	 dispatch_event(qore_obj, e_tabletEvent, QC_QTabletEvent, new QoreQTabletEvent(*event));
      }
      
      DLLLOCAL virtual void wheelEvent(QWheelEvent *event)
      {
	 if (!e_wheelEvent) {
	    QOREQTYPE::wheelEvent(event);
	    return;
	 }

	 dispatch_event(qore_obj, e_wheelEvent, QC_QWheelEvent, new QoreQWheelEvent(*event));
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

	 QoreList *args; 
	 args = new QoreList();
	 args->push(new QoreNode((int64)w));
	 
	 return dispatch_event_int(qore_obj, p_heightForWidth, args);
      }

      DLLLOCAL virtual QVariant inputMethodQuery ( Qt::InputMethodQuery query ) const
      {
	 if (!p_inputMethodQuery)
	    return QOREQTYPE::inputMethodQuery(query);

	 QoreList *args = new QoreList();
	 args->push(new QoreNode((int64)query));

	 ExceptionSink xsink;
	 ReferenceHolder<QoreNode> rv(dispatch_event_intern(qore_obj, p_inputMethodQuery, args, &xsink), &xsink);
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
	 
	 class QoreList *args = new QoreList();
	 args->push(new QoreNode(visible));

	 dispatch_event(qore_obj, p_setVisible, args);
      }

      DLLLOCAL virtual QSize sizeHint() const
      {
	 if (!p_sizeHint)
	    return QOREQTYPE::sizeHint();

	 ExceptionSink xsink;

	 // call sizeHint method
	 QoreNode *rv = p_sizeHint->eval(qore_obj, 0, &xsink);
	 if (xsink) {
	    discard(rv, &xsink);
	    return QOREQTYPE::sizeHint();
	 }
	 QoreQSize *qs = (rv && rv->type == NT_OBJECT) ? (QoreQSize *)rv->val.object->getReferencedPrivateData(CID_QSIZE, &xsink) : 0;
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
	 QoreNode *rv = p_minimumSizeHint->eval(qore_obj, 0, &xsink);
	 if (xsink) {
	    discard(rv, &xsink);
	    return QOREQTYPE::minimumSizeHint();
	 }
	 QoreQSize *qs = (rv && rv->type == NT_OBJECT) ? (QoreQSize *)rv->val.object->getReferencedPrivateData(CID_QSIZE, &xsink) : 0;
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

