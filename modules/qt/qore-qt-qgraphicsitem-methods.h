
#if 0
class T {
#endif
  
public:

      DLLLOCAL virtual QRectF boundingRect() const
      {
	 if (!m_boundingRect)
	    return parent_boundingRect();

	 ExceptionSink xsink;
	 ReferenceHolder<AbstractQoreNode> rv(dispatch_event_intern(qore_obj, m_boundingRect, 0, &xsink), &xsink);
	 QoreObject *o = rv && rv->getType() == NT_OBJECT ? reinterpret_cast<QoreObject *>(*rv) : 0;
	 QoreQRect *qr = o ? (QoreQRect *)o->getReferencedPrivateData(CID_QRECT, &xsink) : 0;
	 if (xsink)
	    return QRect();

	 if (!qr) {
	    xsink.raiseException("QGRAPHICSITEM-BOUNDINGRECT-ERROR", "%s::boundingRect() did not return a QRect object (type returned: '%s')", qore_obj->getClassName(), rv ? rv->getTypeName() : "NOTHING");
	    return QRect();
	 }
	 ReferenceHolder<AbstractPrivateData> apd(qr, &xsink);
	 return *qr;
      }

      DLLLOCAL virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
      {
	 if (!m_paint) {
	    parent_paint(painter, option, widget);
	    return;
	 }

	 ExceptionSink xsink;
         ReferenceHolder<QoreListNode> args(new QoreListNode(), &xsink);
         args->push(return_object(QC_QPainter, new QoreQPainter(painter)));
	 args->push(return_object(QC_QStyleOptionGraphicsItem, new QoreQStyleOptionGraphicsItem(*option)));
	 args->push(return_qwidget(const_cast<QWidget *>(widget), false));

         discard(dispatch_event_intern(qore_obj, m_paint, *args, &xsink), &xsink);
      }

#ifdef QORE_IS_QGRAPHICSITEM
      DLLLOCAL virtual QRectF parent_boundingRect() const
      {
	 return QRectF();
      }
      
      DLLLOCAL virtual void parent_paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
      {
      }
#else
      DLLLOCAL virtual QRectF parent_boundingRect() const
      {
	 return QOREQTYPE::boundingRect();
      }
      
      DLLLOCAL virtual void parent_paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
      {
	 QOREQTYPE::paint(painter, option, widget);
      }
#endif
  
      DLLLOCAL void parent_contextMenuEvent ( QGraphicsSceneContextMenuEvent * event ) { QOREQTYPE::contextMenuEvent(event); }
      DLLLOCAL void parent_dragEnterEvent ( QGraphicsSceneDragDropEvent * event ) { QOREQTYPE::dragEnterEvent(event); }
      DLLLOCAL void parent_dragLeaveEvent ( QGraphicsSceneDragDropEvent * event ) { QOREQTYPE::dragLeaveEvent(event); }
      DLLLOCAL void parent_dragMoveEvent ( QGraphicsSceneDragDropEvent * event ) { QOREQTYPE::dragMoveEvent(event); }
      DLLLOCAL void parent_dropEvent ( QGraphicsSceneDragDropEvent * event ) { QOREQTYPE::dropEvent(event); }
      DLLLOCAL void parent_focusInEvent ( QFocusEvent * event ) { QOREQTYPE::focusInEvent(event); }
      DLLLOCAL void parent_focusOutEvent ( QFocusEvent * event ) { QOREQTYPE::focusOutEvent(event); }
      DLLLOCAL void parent_hoverEnterEvent ( QGraphicsSceneHoverEvent * event ) { QOREQTYPE::hoverEnterEvent(event); }
      DLLLOCAL void parent_hoverLeaveEvent ( QGraphicsSceneHoverEvent * event ) { QOREQTYPE::hoverLeaveEvent(event); }
      DLLLOCAL void parent_hoverMoveEvent ( QGraphicsSceneHoverEvent * event ) { QOREQTYPE::hoverMoveEvent(event); }
      DLLLOCAL void parent_inputMethodEvent ( QInputMethodEvent * event ) { QOREQTYPE::inputMethodEvent(event); }
      DLLLOCAL QVariant parent_inputMethodQuery ( Qt::InputMethodQuery query ) const { return QOREQTYPE::inputMethodQuery(query); }
      DLLLOCAL QVariant parent_itemChange ( QGraphicsItem::GraphicsItemChange change, const QVariant & value ) { return QOREQTYPE::itemChange(change, value); }
      DLLLOCAL void parent_keyPressEvent ( QKeyEvent * event ) { QOREQTYPE::keyPressEvent(event); }
      DLLLOCAL void parent_keyReleaseEvent ( QKeyEvent * event ) { QOREQTYPE::keyReleaseEvent(event); }
      DLLLOCAL void parent_mouseDoubleClickEvent ( QGraphicsSceneMouseEvent * event ) { QOREQTYPE::mouseDoubleClickEvent(event); }
      DLLLOCAL void parent_mouseMoveEvent ( QGraphicsSceneMouseEvent * event ) { QOREQTYPE::mouseMoveEvent(event); }
      DLLLOCAL void parent_mousePressEvent ( QGraphicsSceneMouseEvent * event ) { QOREQTYPE::mousePressEvent(event); }
      DLLLOCAL void parent_mouseReleaseEvent ( QGraphicsSceneMouseEvent * event ) { QOREQTYPE::mouseReleaseEvent(event); }
      DLLLOCAL void parent_prepareGeometryChange () { QOREQTYPE::prepareGeometryChange(); }
      DLLLOCAL bool parent_sceneEvent ( QEvent * event ) { return QOREQTYPE::sceneEvent(event); }
      DLLLOCAL bool parent_sceneEventFilter ( QGraphicsItem * watched, QEvent * event ) { return QOREQTYPE::sceneEventFilter(watched, event); }
      DLLLOCAL void parent_wheelEvent ( QGraphicsSceneWheelEvent * event ) { QOREQTYPE::wheelEvent(event); }
  
#if 0
}
#endif
