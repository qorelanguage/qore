/*
 QoreAbstractQGraphicsItem.h
 
 Qore Programming Language
 
 Copyright 2003 - 2008 David Nichols
 
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

#ifndef _QORE_QT_QOREABSTRACTQGRAPHICSITEM_H

#define _QORE_QT_QOREABSTRACTQGRAPHICSITEM_H

class QoreAbstractQGraphicsItem
{
   public:
      DLLLOCAL virtual ~QoreAbstractQGraphicsItem()
      {
      }

      DLLLOCAL virtual QGraphicsItem *getQGraphicsItem() const = 0;

      DLLLOCAL virtual QRectF boundingRect() const = 0;
      DLLLOCAL virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) = 0;

      DLLLOCAL virtual void contextMenuEvent ( QGraphicsSceneContextMenuEvent * event ) = 0;
      DLLLOCAL virtual void dragEnterEvent ( QGraphicsSceneDragDropEvent * event ) = 0;
      DLLLOCAL virtual void dragLeaveEvent ( QGraphicsSceneDragDropEvent * event ) = 0;
      DLLLOCAL virtual void dragMoveEvent ( QGraphicsSceneDragDropEvent * event ) = 0;
      DLLLOCAL virtual void dropEvent ( QGraphicsSceneDragDropEvent * event ) = 0;
      DLLLOCAL virtual void focusInEvent ( QFocusEvent * event ) = 0;
      DLLLOCAL virtual void focusOutEvent ( QFocusEvent * event ) = 0;
      DLLLOCAL virtual void hoverEnterEvent ( QGraphicsSceneHoverEvent * event ) = 0;
      DLLLOCAL virtual void hoverLeaveEvent ( QGraphicsSceneHoverEvent * event ) = 0;
      DLLLOCAL virtual void hoverMoveEvent ( QGraphicsSceneHoverEvent * event ) = 0;
      DLLLOCAL virtual void inputMethodEvent ( QInputMethodEvent * event ) = 0;
      DLLLOCAL virtual QVariant inputMethodQuery ( Qt::InputMethodQuery query ) const = 0;
      DLLLOCAL virtual QVariant itemChange ( QGraphicsItem::GraphicsItemChange change, const QVariant & value ) = 0;
      DLLLOCAL virtual void keyPressEvent ( QKeyEvent * event ) = 0;
      DLLLOCAL virtual void keyReleaseEvent ( QKeyEvent * event ) = 0;
      DLLLOCAL virtual void mouseDoubleClickEvent ( QGraphicsSceneMouseEvent * event ) = 0;
      DLLLOCAL virtual void mouseMoveEvent ( QGraphicsSceneMouseEvent * event ) = 0;
      DLLLOCAL virtual void mousePressEvent ( QGraphicsSceneMouseEvent * event ) = 0;
      DLLLOCAL virtual void mouseReleaseEvent ( QGraphicsSceneMouseEvent * event ) = 0;
      DLLLOCAL virtual void prepareGeometryChange () = 0;
      DLLLOCAL virtual bool sceneEvent ( QEvent * event ) = 0;
      DLLLOCAL virtual bool sceneEventFilter ( QGraphicsItem * watched, QEvent * event ) = 0;
      DLLLOCAL virtual void wheelEvent ( QGraphicsSceneWheelEvent * event ) = 0;
};

class QoreQGraphicsItemExtension : public QoreQtEventDispatcher
{
   protected:
      QoreObject *qore_obj;     // containing qore object for dispatching to qore code

      // event method pointers
      const QoreMethod *m_boundingRect, *m_paint, 
	 *m_contextMenuEvent, *m_dragEnterEvent, *m_dragLeaveEvent, *m_dragMoveEvent,
	 *m_dropEvent, *m_focusInEvent, *m_focusOutEvent, *m_hoverEnterEvent, *m_hoverLeaveEvent,
	 *m_hoverMoveEvent, *m_inputMethodEvent, *m_inputMethodQuery, *m_itemChange, *m_keyPressEvent,
	 *m_keyReleaseEvent, *m_mouseDoubleClickEvent, *m_mouseMoveEvent, *m_mousePressEvent,
	 *m_mouseReleaseEvent, *m_sceneEvent, *m_sceneEventFilter, *m_wheelEvent;

   public:
      DLLLOCAL QoreQGraphicsItemExtension(QoreObject *n_qore_obj) : qore_obj(n_qore_obj)
      {
         const QoreClass *oc = qore_obj->getClass();

	 m_boundingRect            = oc->findMethod("boundingRect");
	 m_paint                   = oc->findMethod("paint");
	 m_contextMenuEvent        = oc->findMethod("contextMenuEvent");
	 m_dragEnterEvent          = oc->findMethod("dragEnterEvent");
	 m_dragLeaveEvent          = oc->findMethod("dragLeaveEvent");
	 m_dragMoveEvent           = oc->findMethod("dragMoveEvent");
	 m_dropEvent               = oc->findMethod("dropEvent");
	 m_focusInEvent            = oc->findMethod("focusInEvent");
	 m_focusOutEvent           = oc->findMethod("focusOutEvent");
	 m_hoverEnterEvent         = oc->findMethod("hoverEnterEvent");
	 m_hoverLeaveEvent         = oc->findMethod("hoverLeaveEvent");
	 m_hoverMoveEvent          = oc->findMethod("hoverMoveEvent");
	 m_inputMethodEvent        = oc->findMethod("inputMethodEvent");
	 m_inputMethodQuery        = oc->findMethod("inputMethodQuery");
	 m_itemChange              = oc->findMethod("itemChange");
	 m_keyPressEvent           = oc->findMethod("keyPressEvent");
	 m_keyReleaseEvent         = oc->findMethod("keyReleaseEvent");
	 m_mouseDoubleClickEvent   = oc->findMethod("mouseDoubleClickEvent");
	 m_mouseMoveEvent          = oc->findMethod("mouseMoveEvent");
	 m_mousePressEvent         = oc->findMethod("mousePressEvent");
	 m_mouseReleaseEvent       = oc->findMethod("mouseReleaseEvent");
	 m_sceneEvent              = oc->findMethod("sceneEvent");
	 m_sceneEventFilter        = oc->findMethod("sceneEventFilter");
	 m_wheelEvent              = oc->findMethod("wheelEvent");

	 qore_obj->tRef();
      }

      DLLLOCAL ~QoreQGraphicsItemExtension()
      {
	 qore_obj->tDeref();
      }
};

class QoreAbstractQGraphicsItemData : public AbstractPrivateData, public QoreAbstractQGraphicsItem
{
};

template<typename T, typename V = QoreAbstractQGraphicsItemData>
class QoreQGraphicsItemBase : public V
{
   protected:
      T *qobj;

   public:
      DLLLOCAL QoreQGraphicsItemBase(T *n_qobj) : qobj(n_qobj)
      {
      }

      DLLLOCAL virtual QGraphicsItem *getQGraphicsItem() const
      {
	 return const_cast<T *>(qobj);
      }

      DLLLOCAL virtual QRectF boundingRect() const
      {
	 return qobj->parent_boundingRect();
      }

      DLLLOCAL virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
      {
	 qobj->parent_paint(painter, option, widget);
      }

      DLLLOCAL virtual void contextMenuEvent ( QGraphicsSceneContextMenuEvent * event )
      {
	 qobj->parent_contextMenuEvent(event);
      }

      DLLLOCAL virtual void dragEnterEvent ( QGraphicsSceneDragDropEvent * event )
      {
	 qobj->parent_dragEnterEvent(event);
      }

      DLLLOCAL virtual void dragLeaveEvent ( QGraphicsSceneDragDropEvent * event )
      {
	 qobj->parent_dragLeaveEvent(event);
      }

      DLLLOCAL virtual void dragMoveEvent ( QGraphicsSceneDragDropEvent * event )
      {
	 qobj->parent_dragMoveEvent(event);
      }

      DLLLOCAL virtual void dropEvent ( QGraphicsSceneDragDropEvent * event )
      {
	 qobj->parent_dropEvent(event);
      }

      DLLLOCAL virtual void focusInEvent ( QFocusEvent * event )
      {
	 qobj->parent_focusInEvent(event);
      }

      DLLLOCAL virtual void focusOutEvent ( QFocusEvent * event )
      {
	 qobj->parent_focusOutEvent(event);
      }

      DLLLOCAL virtual void hoverEnterEvent ( QGraphicsSceneHoverEvent * event )
      {
	 qobj->parent_hoverEnterEvent(event);
      }

      DLLLOCAL virtual void hoverLeaveEvent ( QGraphicsSceneHoverEvent * event )
      {
	 qobj->parent_hoverLeaveEvent(event);
      }

      DLLLOCAL virtual void hoverMoveEvent ( QGraphicsSceneHoverEvent * event )
      {
	 qobj->parent_hoverMoveEvent(event);
      }

      DLLLOCAL virtual void inputMethodEvent ( QInputMethodEvent * event )
      {
	 qobj->parent_inputMethodEvent(event);
      }

      DLLLOCAL virtual QVariant inputMethodQuery ( Qt::InputMethodQuery query ) const
      {
	 return qobj->parent_inputMethodQuery(query);
      }

      DLLLOCAL virtual QVariant itemChange ( QGraphicsItem::GraphicsItemChange change, const QVariant & value )
      {
	 return qobj->parent_itemChange(change, value);
      }

      DLLLOCAL virtual void keyPressEvent ( QKeyEvent * event )
      {
	 qobj->parent_keyPressEvent(event);
      }

      DLLLOCAL virtual void keyReleaseEvent ( QKeyEvent * event )
      {
	 qobj->parent_keyReleaseEvent(event);
      }

      DLLLOCAL virtual void mouseDoubleClickEvent ( QGraphicsSceneMouseEvent * event )
      {
	 qobj->parent_mouseDoubleClickEvent(event);
      }

      DLLLOCAL virtual void mouseMoveEvent ( QGraphicsSceneMouseEvent * event )
      {
	 qobj->parent_mouseMoveEvent(event);
      }

      DLLLOCAL virtual void mousePressEvent ( QGraphicsSceneMouseEvent * event )
      {
	 qobj->parent_mousePressEvent(event);
      }

      DLLLOCAL virtual void mouseReleaseEvent ( QGraphicsSceneMouseEvent * event )
      {
	 qobj->parent_mouseReleaseEvent(event);
      }

      DLLLOCAL virtual void prepareGeometryChange ()
      {
	 qobj->parent_prepareGeometryChange();
      }

      DLLLOCAL virtual bool sceneEvent ( QEvent * event )
      {
	 return qobj->parent_sceneEvent(event);
      }

      DLLLOCAL virtual bool sceneEventFilter ( QGraphicsItem * watched, QEvent * event )
      {
	 return qobj->parent_sceneEventFilter(watched, event);
      }

      DLLLOCAL virtual void wheelEvent ( QGraphicsSceneWheelEvent * event )
      {
	 qobj->parent_wheelEvent(event);
      }
};

template<typename T, typename V = QoreAbstractQGraphicsItemData>
class QoreQtQGraphicsItemBase : public V
{
   protected:
      T *qobj;
      bool managed;

   public:
      DLLLOCAL QoreQtQGraphicsItemBase(T *n_qobj, bool n_managed = true) : qobj(n_qobj), managed(n_managed)
      {
      }

      DLLLOCAL ~QoreQtQGraphicsItemBase()
      {
	 if (managed)
	    delete qobj;
      }

      DLLLOCAL virtual QGraphicsItem *getQGraphicsItem() const
      {
	 return const_cast<T *>(qobj);
      }

      DLLLOCAL virtual QRectF boundingRect() const
      {
	 return qobj->boundingRect();
      }

      DLLLOCAL virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
      {
	 qobj->paint(painter, option, widget);
      }

      // these functions can never be called
      DLLLOCAL virtual void contextMenuEvent ( QGraphicsSceneContextMenuEvent * event ) { }
      DLLLOCAL virtual void dragEnterEvent ( QGraphicsSceneDragDropEvent * event ) { }
      DLLLOCAL virtual void dragLeaveEvent ( QGraphicsSceneDragDropEvent * event ) { }
      DLLLOCAL virtual void dragMoveEvent ( QGraphicsSceneDragDropEvent * event ) { }
      DLLLOCAL virtual void dropEvent ( QGraphicsSceneDragDropEvent * event ) { }
      DLLLOCAL virtual void focusInEvent ( QFocusEvent * event ) { }
      DLLLOCAL virtual void focusOutEvent ( QFocusEvent * event ) { }
      DLLLOCAL virtual void hoverEnterEvent ( QGraphicsSceneHoverEvent * event ) { }
      DLLLOCAL virtual void hoverLeaveEvent ( QGraphicsSceneHoverEvent * event ) { }
      DLLLOCAL virtual void hoverMoveEvent ( QGraphicsSceneHoverEvent * event ) { }
      DLLLOCAL virtual void inputMethodEvent ( QInputMethodEvent * event ) { }
      DLLLOCAL virtual QVariant inputMethodQuery ( Qt::InputMethodQuery query ) const { return QVariant(); }
      DLLLOCAL virtual QVariant itemChange ( QGraphicsItem::GraphicsItemChange change, const QVariant & value ) { return QVariant(); }
      DLLLOCAL virtual void keyPressEvent ( QKeyEvent * event ) { }
      DLLLOCAL virtual void keyReleaseEvent ( QKeyEvent * event ) { }
      DLLLOCAL virtual void mouseDoubleClickEvent ( QGraphicsSceneMouseEvent * event ) { }
      DLLLOCAL virtual void mouseMoveEvent ( QGraphicsSceneMouseEvent * event ) { }
      DLLLOCAL virtual void mousePressEvent ( QGraphicsSceneMouseEvent * event ) { }
      DLLLOCAL virtual void mouseReleaseEvent ( QGraphicsSceneMouseEvent * event ) { }
      DLLLOCAL virtual void prepareGeometryChange () { }
      DLLLOCAL virtual bool sceneEvent ( QEvent * event ) { return false; }
      DLLLOCAL virtual bool sceneEventFilter ( QGraphicsItem * watched, QEvent * event ) { return false; }
      DLLLOCAL virtual void wheelEvent ( QGraphicsSceneWheelEvent * event ) { }
};


#endif  // _QORE_QT_QOREABSTRACT%s_H
