/*
 QC_QGraphicsItem.cc
 
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

#include <qore/Qore.h>

#include "qore-qt-gui.h"

#include "QC_QGraphicsItem.h"
#include "QC_QAbstractGraphicsShapeItem.h"
#include "QC_QGraphicsEllipseItem.h"
#include "QC_QGraphicsPathItem.h"
#include "QC_QGraphicsPolygonItem.h"
#include "QC_QGraphicsRectItem.h"
#include "QC_QGraphicsSimpleTextItem.h"
#include "QC_QGraphicsItemGroup.h"
#include "QC_QGraphicsLineItem.h"
#include "QC_QGraphicsPixmapItem.h"
#include "QC_QRectF.h"
#include "QC_QPainterPath.h"
#include "QC_QPainter.h"
#include "QC_QPointF.h"
#include "QC_QCursor.h"
#include "QC_QTransform.h"
#include "QC_QPolygonF.h"
#include "QC_QWidget.h"
#include "QC_QStyleOptionGraphicsItem.h"
#include "QC_QGraphicsSceneContextMenuEvent.h"
#include "QC_QGraphicsSceneDragDropEvent.h"
#include "QC_QGraphicsSceneHoverEvent.h"
#include "QC_QGraphicsSceneMouseEvent.h"
#include "QC_QGraphicsSceneWheelEvent.h"
#include "QC_QRect.h"

qore_classid_t CID_QGRAPHICSITEM;
QoreClass *QC_QGraphicsItem = 0;

//QGraphicsItem ( QGraphicsItem * parent = 0 )
static void QGRAPHICSITEM_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQGraphicsItem *parent = (p && p->getType() == NT_OBJECT) ? (QoreQGraphicsItem *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QGRAPHICSITEM, xsink) : 0;
   if (*xsink)
      return;
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   self->setPrivate(CID_QGRAPHICSITEM, new QoreQGraphicsItem(self, parent ? parent->getQGraphicsItem() : 0));
   return;
}

static void QGRAPHICSITEM_copy(QoreObject *self, QoreObject *old, QoreQGraphicsItem *qgi, ExceptionSink *xsink)
{
   xsink->raiseException("QGRAPHICSITEM-COPY-ERROR", "objects of this class cannot be copied");
}

//bool acceptDrops () const
static AbstractQoreNode *QGRAPHICSITEM_acceptDrops(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qgi->getQGraphicsItem()->acceptDrops());
}

//Qt::MouseButtons acceptedMouseButtons () const
static AbstractQoreNode *QGRAPHICSITEM_acceptedMouseButtons(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qgi->getQGraphicsItem()->acceptedMouseButtons());
}

//bool acceptsHoverEvents () const
static AbstractQoreNode *QGRAPHICSITEM_acceptsHoverEvents(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qgi->getQGraphicsItem()->acceptsHoverEvents());
}

//virtual void advance ( int phase )
static AbstractQoreNode *QGRAPHICSITEM_advance(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int phase = p ? p->getAsInt() : 0;
   qgi->getQGraphicsItem()->advance(phase);
   return 0;
}

//virtual QRectF boundingRect () const = 0
static AbstractQoreNode *QGRAPHICSITEM_boundingRect(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QRectF, new QoreQRectF(qgi->getQGraphicsItem()->boundingRect()));
}

/*
//QList<QGraphicsItem *> children () const
static AbstractQoreNode *QGRAPHICSITEM_children(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   ??? return new QoreBigIntNode(qgi->getQGraphicsItem()->children());
}
*/

//QRectF childrenBoundingRect () const
static AbstractQoreNode *QGRAPHICSITEM_childrenBoundingRect(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QRectF, new QoreQRectF(qgi->getQGraphicsItem()->childrenBoundingRect()));
}

//void clearFocus ()
static AbstractQoreNode *QGRAPHICSITEM_clearFocus(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   qgi->getQGraphicsItem()->clearFocus();
   return 0;
}

//virtual bool collidesWithItem ( const QGraphicsItem * other, Qt::ItemSelectionMode mode = Qt::IntersectsItemShape ) const
static AbstractQoreNode *QGRAPHICSITEM_collidesWithItem(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQGraphicsItem *other = (p && p->getType() == NT_OBJECT) ? (QoreQGraphicsItem *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QGRAPHICSITEM, xsink) : 0;
   if (!other) {
      if (!xsink->isException())
         xsink->raiseException("QGRAPHICSITEM-COLLIDESWITHITEM-PARAM-ERROR", "expecting a QGraphicsItem object as first argument to QGraphicsItem::collidesWithItem()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> otherHolder(static_cast<AbstractPrivateData *>(other), xsink);
   p = get_param(params, 1);
   Qt::ItemSelectionMode mode = !is_nothing(p) ? (Qt::ItemSelectionMode)p->getAsInt() : Qt::IntersectsItemShape;
   return get_bool_node(qgi->getQGraphicsItem()->collidesWithItem(other->getQGraphicsItem(), mode));
}

//virtual bool collidesWithPath ( const QPainterPath & path, Qt::ItemSelectionMode mode = Qt::IntersectsItemShape ) const
static AbstractQoreNode *QGRAPHICSITEM_collidesWithPath(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQPainterPath *path = (p && p->getType() == NT_OBJECT) ? (QoreQPainterPath *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QPAINTERPATH, xsink) : 0;
   if (!path) {
      if (!xsink->isException())
         xsink->raiseException("QGRAPHICSITEM-COLLIDESWITHPATH-PARAM-ERROR", "expecting a QPainterPath object as first argument to QGraphicsItem::collidesWithPath()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> pathHolder(static_cast<AbstractPrivateData *>(path), xsink);
   p = get_param(params, 1);
   Qt::ItemSelectionMode mode = !is_nothing(p) ? (Qt::ItemSelectionMode)p->getAsInt() : Qt::IntersectsItemShape;
   return get_bool_node(qgi->getQGraphicsItem()->collidesWithPath(*(static_cast<QPainterPath *>(path)), mode));
}

/*
//QList<QGraphicsItem *> collidingItems ( Qt::ItemSelectionMode mode = Qt::IntersectsItemShape ) const
static AbstractQoreNode *QGRAPHICSITEM_collidingItems(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   Qt::ItemSelectionMode mode = !is_nothing(p) ? (Qt::ItemSelectionMode)p->getAsInt() : Qt::IntersectsItemShape;
   ??? return new QoreBigIntNode(qgi->getQGraphicsItem()->collidingItems(mode));
}
*/

//virtual bool contains ( const QPointF & point ) const
static AbstractQoreNode *QGRAPHICSITEM_contains(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQPointF *point = (p && p->getType() == NT_OBJECT) ? (QoreQPointF *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QPOINTF, xsink) : 0;
   if (!point) {
      if (!xsink->isException())
         xsink->raiseException("QGRAPHICSITEM-CONTAINS-PARAM-ERROR", "expecting a QPointF object as first argument to QGraphicsItem::contains()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> pointHolder(static_cast<AbstractPrivateData *>(point), xsink);
   return get_bool_node(qgi->getQGraphicsItem()->contains(*(static_cast<QPointF *>(point))));
}

//QCursor cursor () const
static AbstractQoreNode *QGRAPHICSITEM_cursor(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QCursor, new QoreQCursor(qgi->getQGraphicsItem()->cursor()));
}

//QVariant data ( int key ) const
static AbstractQoreNode *QGRAPHICSITEM_data(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int key = p ? p->getAsInt() : 0;
   return return_qvariant(qgi->getQGraphicsItem()->data(key));
}

//QTransform deviceTransform ( const QTransform & viewportTransform ) const
static AbstractQoreNode *QGRAPHICSITEM_deviceTransform(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQTransform *viewportTransform = (p && p->getType() == NT_OBJECT) ? (QoreQTransform *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QTRANSFORM, xsink) : 0;
   if (!viewportTransform) {
      if (!xsink->isException())
         xsink->raiseException("QGRAPHICSITEM-DEVICETRANSFORM-PARAM-ERROR", "expecting a QTransform object as first argument to QGraphicsItem::deviceTransform()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> viewportTransformHolder(static_cast<AbstractPrivateData *>(viewportTransform), xsink);
   return return_object(QC_QTransform, new QoreQTransform(qgi->getQGraphicsItem()->deviceTransform(*(static_cast<QTransform *>(viewportTransform)))));
}

//void ensureVisible ( const QRectF & rect = QRectF(), int xmargin = 50, int ymargin = 50 )
//void ensureVisible ( qreal x, qreal y, qreal w, qreal h, int xmargin = 50, int ymargin = 50 )
static AbstractQoreNode *QGRAPHICSITEM_ensureVisible(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      qgi->getQGraphicsItem()->ensureVisible();
      return 0;
   }
   if (p && p->getType() == NT_OBJECT) {
      QoreQRectF *rect = (QoreQRectF *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QRECTF, xsink);
      if (*xsink)
         return 0;
      ReferenceHolder<AbstractPrivateData> rectHolder(static_cast<AbstractPrivateData *>(rect), xsink);
      p = get_param(params, 1);
      int xmargin = !is_nothing(p) ? p->getAsInt() : 50;
      p = get_param(params, 2);
      int ymargin = !is_nothing(p) ? p->getAsInt() : 50;
      qgi->getQGraphicsItem()->ensureVisible(*(static_cast<QRectF *>(rect)), xmargin, ymargin);
      return 0;
   }
   qreal x = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 1);
   qreal y = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 2);
   qreal w = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 3);
   qreal h = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 4);
   int xmargin = !is_nothing(p) ? p->getAsInt() : 50;
   p = get_param(params, 5);
   int ymargin = !is_nothing(p) ? p->getAsInt() : 50;
   qgi->getQGraphicsItem()->ensureVisible(x, y, w, h, xmargin, ymargin);
   return 0;
}

//GraphicsItemFlags flags () const
static AbstractQoreNode *QGRAPHICSITEM_flags(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qgi->getQGraphicsItem()->flags());
}

/*
//QGraphicsItemGroup * group () const
static AbstractQoreNode *QGRAPHICSITEM_group(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   ??? return new QoreBigIntNode(qgi->getQGraphicsItem()->group());
}
*/

//bool handlesChildEvents () const
static AbstractQoreNode *QGRAPHICSITEM_handlesChildEvents(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qgi->getQGraphicsItem()->handlesChildEvents());
}

//bool hasCursor () const
static AbstractQoreNode *QGRAPHICSITEM_hasCursor(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qgi->getQGraphicsItem()->hasCursor());
}

//bool hasFocus () const
static AbstractQoreNode *QGRAPHICSITEM_hasFocus(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qgi->getQGraphicsItem()->hasFocus());
}

//void hide ()
static AbstractQoreNode *QGRAPHICSITEM_hide(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   qgi->getQGraphicsItem()->hide();
   return 0;
}

//void installSceneEventFilter ( QGraphicsItem * filterItem )
static AbstractQoreNode *QGRAPHICSITEM_installSceneEventFilter(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQGraphicsItem *filterItem = (p && p->getType() == NT_OBJECT) ? (QoreQGraphicsItem *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QGRAPHICSITEM, xsink) : 0;
   if (!filterItem) {
      if (!xsink->isException())
         xsink->raiseException("QGRAPHICSITEM-INSTALLSCENEEVENTFILTER-PARAM-ERROR", "expecting a QGraphicsItem object as first argument to QGraphicsItem::installSceneEventFilter()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> filterItemHolder(static_cast<AbstractPrivateData *>(filterItem), xsink);
   qgi->getQGraphicsItem()->installSceneEventFilter(filterItem->getQGraphicsItem());
   return 0;
}

//bool isAncestorOf ( const QGraphicsItem * child ) const
static AbstractQoreNode *QGRAPHICSITEM_isAncestorOf(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQGraphicsItem *child = (p && p->getType() == NT_OBJECT) ? (QoreQGraphicsItem *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QGRAPHICSITEM, xsink) : 0;
   if (!child) {
      if (!xsink->isException())
         xsink->raiseException("QGRAPHICSITEM-ISANCESTOROF-PARAM-ERROR", "expecting a QGraphicsItem object as first argument to QGraphicsItem::isAncestorOf()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> childHolder(static_cast<AbstractPrivateData *>(child), xsink);
   return get_bool_node(qgi->getQGraphicsItem()->isAncestorOf(child->getQGraphicsItem()));
}

//bool isEnabled () const
static AbstractQoreNode *QGRAPHICSITEM_isEnabled(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qgi->getQGraphicsItem()->isEnabled());
}

//bool isObscured () const
//bool isObscured ( qreal x, qreal y, qreal w, qreal h ) const
//bool isObscured ( const QRectF & rect ) const
static AbstractQoreNode *QGRAPHICSITEM_isObscured(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      return get_bool_node(qgi->getQGraphicsItem()->isObscured());
   }
   if (p && p->getType() == NT_OBJECT) {
      QoreQRectF *rect = (QoreQRectF *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QRECTF, xsink);
      if (!rect) {
         if (!xsink->isException())
            xsink->raiseException("QGRAPHICSITEM-ISOBSCURED-PARAM-ERROR", "QGraphicsItem::isObscured() does not know how to handle arguments of class '%s' as passed as the first argument", reinterpret_cast<const QoreObject *>(p)->getClassName());
         return 0;
      }
      ReferenceHolder<AbstractPrivateData> rectHolder(static_cast<AbstractPrivateData *>(rect), xsink);
      return get_bool_node(qgi->getQGraphicsItem()->isObscured(*(static_cast<QRectF *>(rect))));
   }
   qreal x = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 1);
   qreal y = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 2);
   qreal w = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 3);
   qreal h = p ? p->getAsFloat() : 0.0;
   return get_bool_node(qgi->getQGraphicsItem()->isObscured(x, y, w, h));
}

//virtual bool isObscuredBy ( const QGraphicsItem * item ) const
static AbstractQoreNode *QGRAPHICSITEM_isObscuredBy(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQGraphicsItem *item = (p && p->getType() == NT_OBJECT) ? (QoreQGraphicsItem *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QGRAPHICSITEM, xsink) : 0;
   if (!item) {
      if (!xsink->isException())
         xsink->raiseException("QGRAPHICSITEM-ISOBSCUREDBY-PARAM-ERROR", "expecting a QGraphicsItem object as first argument to QGraphicsItem::isObscuredBy()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> itemHolder(static_cast<AbstractPrivateData *>(item), xsink);
   return get_bool_node(qgi->getQGraphicsItem()->isObscuredBy(item->getQGraphicsItem()));
}

//bool isSelected () const
static AbstractQoreNode *QGRAPHICSITEM_isSelected(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qgi->getQGraphicsItem()->isSelected());
}

//bool isVisible () const
static AbstractQoreNode *QGRAPHICSITEM_isVisible(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qgi->getQGraphicsItem()->isVisible());
}

//QPointF mapFromItem ( const QGraphicsItem * item, const QPointF & point ) const
//QPolygonF mapFromItem ( const QGraphicsItem * item, const QRectF & rect ) const
//QPolygonF mapFromItem ( const QGraphicsItem * item, const QPolygonF & polygon ) const
//QPainterPath mapFromItem ( const QGraphicsItem * item, const QPainterPath & path ) const
//QPolygonF mapFromItem ( const QGraphicsItem * item, qreal x, qreal y, qreal w, qreal h ) const
//QPointF mapFromItem ( const QGraphicsItem * item, qreal x, qreal y ) const
static AbstractQoreNode *QGRAPHICSITEM_mapFromItem(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQGraphicsItem *item = (p && p->getType() == NT_OBJECT) ? (QoreQGraphicsItem *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QGRAPHICSITEM, xsink) : 0;
   if (!item) {
      if (!xsink->isException())
	 xsink->raiseException("QGRAPHICSITEM-MAPFROMITEM-PARAM-ERROR", "QGraphicsItem::mapFromItem() expects an arguments derived from class QGraphicsItem as the first argument");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> itemHolder(static_cast<AbstractPrivateData *>(item), xsink);

   p = get_param(params, 1);
   if (p && p->getType() == NT_OBJECT) {
      const QoreObject *o = reinterpret_cast<const QoreObject *>(p);
      QoreQPointF *point = (QoreQPointF *)o->getReferencedPrivateData(CID_QPOINTF, xsink);
      if (*xsink)
	 return 0;
      if (!point) {
	 QoreQRectF *rect = (QoreQRectF *)o->getReferencedPrivateData(CID_QRECTF, xsink);
	 if (*xsink)
	    return 0;
	 if (!rect) {
	    QoreQPolygonF *polygon = (QoreQPolygonF *)o->getReferencedPrivateData(CID_QPOLYGONF, xsink);
	    if (*xsink)
	       return 0;
	    if (!polygon) {
	       QoreQPainterPath *path = (QoreQPainterPath *)o->getReferencedPrivateData(CID_QPAINTERPATH, xsink);
	       if (*xsink)
		  return 0;
	       if (!path) {
		  xsink->raiseException("QGRAPHICSITEM-MAPFROMITEM-PARAM-ERROR", "this version of QGraphicsItem::mapFromItem() expects an object derived from QPainterPath as the second argument", o->getClassName());
		  return 0;
	       }
	       ReferenceHolder<AbstractPrivateData> pathHolder(static_cast<AbstractPrivateData *>(path), xsink);
	       return return_object(QC_QPainterPath, new QoreQPainterPath(qgi->getQGraphicsItem()->mapFromItem(item->getQGraphicsItem(), *(static_cast<QPainterPath *>(path)))));
	    }
	    ReferenceHolder<AbstractPrivateData> polygonHolder(static_cast<AbstractPrivateData *>(polygon), xsink);
	    return return_object(QC_QPolygonF, new QoreQPolygonF(qgi->getQGraphicsItem()->mapFromItem(item->getQGraphicsItem(), *(static_cast<QPolygonF *>(polygon)))));
	 }
	 ReferenceHolder<AbstractPrivateData> rectHolder(static_cast<AbstractPrivateData *>(rect), xsink);
	 return return_object(QC_QPolygonF, new QoreQPolygonF(qgi->getQGraphicsItem()->mapFromItem(item->getQGraphicsItem(), *(static_cast<QRectF *>(rect)))));
      }
      ReferenceHolder<AbstractPrivateData> pointHolder(static_cast<AbstractPrivateData *>(point), xsink);
      return return_object(QC_QPointF, new QoreQPointF(qgi->getQGraphicsItem()->mapFromItem(item->getQGraphicsItem(), *(static_cast<QPointF *>(point)))));
   }

   qreal x = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 2);
   qreal y = p ? p->getAsFloat() : 0.0;
   if (num_params(params) == 2) 
      return return_object(QC_QPointF, new QoreQPointF(qgi->getQGraphicsItem()->mapFromItem(item->getQGraphicsItem(), x, y)));
   
   p = get_param(params, 3);
   qreal w = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 4);
   qreal h = p ? p->getAsFloat() : 0.0;
   return return_object(QC_QPolygonF, new QoreQPolygonF(qgi->getQGraphicsItem()->mapFromItem(item->getQGraphicsItem(), x, y, w, h)));
}

//QPointF mapFromParent ( const QPointF & point ) const
//QPolygonF mapFromParent ( const QRectF & rect ) const
//QPolygonF mapFromParent ( const QPolygonF & polygon ) const
//QPainterPath mapFromParent ( const QPainterPath & path ) const
//QPolygonF mapFromParent ( qreal x, qreal y, qreal w, qreal h ) const
//QPointF mapFromParent ( qreal x, qreal y ) const
static AbstractQoreNode *QGRAPHICSITEM_mapFromParent(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (p && p->getType() == NT_OBJECT) {
      QoreQRectF *rect = (QoreQRectF *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QRECTF, xsink);
      if (!rect) {
         QoreQPolygonF *polygon = (QoreQPolygonF *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QPOLYGONF, xsink);
         if (!polygon) {
            QoreQPainterPath *path = (QoreQPainterPath *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QPAINTERPATH, xsink);
            if (!path) {
               QoreQPointF *point = (QoreQPointF *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QPOINTF, xsink);
               if (!point) {
                  if (!xsink->isException())
                     xsink->raiseException("QGRAPHICSITEM-MAPFROMPARENT-PARAM-ERROR", "QGraphicsItem::mapFromParent() does not know how to handle arguments of class '%s' as passed as the first argument", reinterpret_cast<const QoreObject *>(p)->getClassName());
                  return 0;
               }
               ReferenceHolder<AbstractPrivateData> pointHolder(static_cast<AbstractPrivateData *>(point), xsink);
               return return_object(QC_QPointF, new QoreQPointF(qgi->getQGraphicsItem()->mapFromParent(*(static_cast<QPointF *>(point)))));
            }
            ReferenceHolder<AbstractPrivateData> pathHolder(static_cast<AbstractPrivateData *>(path), xsink);
            return return_object(QC_QPainterPath, new QoreQPainterPath(qgi->getQGraphicsItem()->mapFromParent(*(static_cast<QPainterPath *>(path)))));
         }
         ReferenceHolder<AbstractPrivateData> polygonHolder(static_cast<AbstractPrivateData *>(polygon), xsink);
         return return_object(QC_QPolygonF, new QoreQPolygonF(qgi->getQGraphicsItem()->mapFromParent(*(static_cast<QPolygonF *>(polygon)))));
      }
      ReferenceHolder<AbstractPrivateData> rectHolder(static_cast<AbstractPrivateData *>(rect), xsink);
      return return_object(QC_QPolygonF, new QoreQPolygonF(qgi->getQGraphicsItem()->mapFromParent(*(static_cast<QRectF *>(rect)))));
   }
   qreal x = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 1);
   qreal y = p ? p->getAsFloat() : 0.0;
   if (num_params(params) == 2)
      return return_object(QC_QPointF, new QoreQPointF(qgi->getQGraphicsItem()->mapFromParent(x, y)));

   p = get_param(params, 2);
   qreal w = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 3);
   qreal h = p ? p->getAsFloat() : 0.0;
   return return_object(QC_QPolygonF, new QoreQPolygonF(qgi->getQGraphicsItem()->mapFromParent(x, y, w, h)));
}

//QPointF mapFromScene ( const QPointF & point ) const
//QPolygonF mapFromScene ( const QRectF & rect ) const
//QPolygonF mapFromScene ( const QPolygonF & polygon ) const
//QPainterPath mapFromScene ( const QPainterPath & path ) const
//QPolygonF mapFromScene ( qreal x, qreal y, qreal w, qreal h ) const
//QPointF mapFromScene ( qreal x, qreal y ) const
static AbstractQoreNode *QGRAPHICSITEM_mapFromScene(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (p && p->getType() == NT_OBJECT) {
      QoreQRectF *rect = (QoreQRectF *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QRECTF, xsink);
      if (!rect) {
         QoreQPolygonF *polygon = (QoreQPolygonF *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QPOLYGONF, xsink);
         if (!polygon) {
            QoreQPainterPath *path = (QoreQPainterPath *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QPAINTERPATH, xsink);
            if (!path) {
               QoreQPointF *point = (QoreQPointF *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QPOINTF, xsink);
               if (!point) {
                  if (!xsink->isException())
                     xsink->raiseException("QGRAPHICSITEM-MAPFROMSCENE-PARAM-ERROR", "QGraphicsItem::mapFromScene() does not know how to handle arguments of class '%s' as passed as the first argument", reinterpret_cast<const QoreObject *>(p)->getClassName());
                  return 0;
               }
               ReferenceHolder<AbstractPrivateData> pointHolder(static_cast<AbstractPrivateData *>(point), xsink);
               return return_object(QC_QPointF, new QoreQPointF(qgi->getQGraphicsItem()->mapFromScene(*(static_cast<QPointF *>(point)))));
            }
            ReferenceHolder<AbstractPrivateData> pathHolder(static_cast<AbstractPrivateData *>(path), xsink);
            return return_object(QC_QPainterPath, new QoreQPainterPath(qgi->getQGraphicsItem()->mapFromScene(*(static_cast<QPainterPath *>(path)))));
         }
         ReferenceHolder<AbstractPrivateData> polygonHolder(static_cast<AbstractPrivateData *>(polygon), xsink);
         return return_object(QC_QPolygonF, new QoreQPolygonF(qgi->getQGraphicsItem()->mapFromScene(*(static_cast<QPolygonF *>(polygon)))));
      }
      ReferenceHolder<AbstractPrivateData> rectHolder(static_cast<AbstractPrivateData *>(rect), xsink);
      return return_object(QC_QPolygonF, new QoreQPolygonF(qgi->getQGraphicsItem()->mapFromScene(*(static_cast<QRectF *>(rect)))));
   }
   qreal x = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 1);
   qreal y = p ? p->getAsFloat() : 0.0;
   if (num_params(params) == 2)
      return return_object(QC_QPointF, new QoreQPointF(qgi->getQGraphicsItem()->mapFromScene(x, y)));

   p = get_param(params, 2);
   qreal w = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 3);
   qreal h = p ? p->getAsFloat() : 0.0;
   return return_object(QC_QPolygonF, new QoreQPolygonF(qgi->getQGraphicsItem()->mapFromScene(x, y, w, h)));
}

//QPointF mapToItem ( const QGraphicsItem * item, const QPointF & point ) const
//QPolygonF mapToItem ( const QGraphicsItem * item, const QRectF & rect ) const
//QPolygonF mapToItem ( const QGraphicsItem * item, const QPolygonF & polygon ) const
//QPainterPath mapToItem ( const QGraphicsItem * item, const QPainterPath & path ) const
//QPolygonF mapToItem ( const QGraphicsItem * item, qreal x, qreal y, qreal w, qreal h ) const
//QPointF mapToItem ( const QGraphicsItem * item, qreal x, qreal y ) const
static AbstractQoreNode *QGRAPHICSITEM_mapToItem(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQGraphicsItem *item = (p && p->getType() == NT_OBJECT) ? (QoreQGraphicsItem *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QGRAPHICSITEM, xsink) : 0;
   if (!item) {
      if (!xsink->isException())
	 xsink->raiseException("QGRAPHICSITEM-MAPTOITEM-PARAM-ERROR", "QGraphicsItem::mapToItem() expects an arguments derived from class QGraphicsItem as the first argument");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> itemHolder(static_cast<AbstractPrivateData *>(item), xsink);

   p = get_param(params, 1);
   if (p && p->getType() == NT_OBJECT) {
      const QoreObject *o = reinterpret_cast<const QoreObject *>(p);
      QoreQPointF *point = (QoreQPointF *)o->getReferencedPrivateData(CID_QPOINTF, xsink);
      if (*xsink)
	 return 0;
      if (!point) {
	 QoreQRectF *rect = (QoreQRectF *)o->getReferencedPrivateData(CID_QRECTF, xsink);
	 if (*xsink)
	    return 0;
	 if (!rect) {
	    QoreQPolygonF *polygon = (QoreQPolygonF *)o->getReferencedPrivateData(CID_QPOLYGONF, xsink);
	    if (*xsink)
	       return 0;
	    if (!polygon) {
	       QoreQPainterPath *path = (QoreQPainterPath *)o->getReferencedPrivateData(CID_QPAINTERPATH, xsink);
	       if (*xsink)
		  return 0;
	       if (!path) {
		  xsink->raiseException("QGRAPHICSITEM-MAPTOITEM-PARAM-ERROR", "this version of QGraphicsItem::mapToItem() expects an object derived from QPainterPath as the second argument", o->getClassName());
		  return 0;
	       }
	       ReferenceHolder<AbstractPrivateData> pathHolder(static_cast<AbstractPrivateData *>(path), xsink);
	       return return_object(QC_QPainterPath, new QoreQPainterPath(qgi->getQGraphicsItem()->mapToItem(item->getQGraphicsItem(), *(static_cast<QPainterPath *>(path)))));
	    }
	    ReferenceHolder<AbstractPrivateData> polygonHolder(static_cast<AbstractPrivateData *>(polygon), xsink);
	    return return_object(QC_QPolygonF, new QoreQPolygonF(qgi->getQGraphicsItem()->mapToItem(item->getQGraphicsItem(), *(static_cast<QPolygonF *>(polygon)))));
	 }
	 ReferenceHolder<AbstractPrivateData> rectHolder(static_cast<AbstractPrivateData *>(rect), xsink);
	 return return_object(QC_QPolygonF, new QoreQPolygonF(qgi->getQGraphicsItem()->mapToItem(item->getQGraphicsItem(), *(static_cast<QRectF *>(rect)))));
      }
      ReferenceHolder<AbstractPrivateData> pointHolder(static_cast<AbstractPrivateData *>(point), xsink);
      return return_object(QC_QPointF, new QoreQPointF(qgi->getQGraphicsItem()->mapToItem(item->getQGraphicsItem(), *(static_cast<QPointF *>(point)))));
   }

   qreal x = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 2);
   qreal y = p ? p->getAsFloat() : 0.0;
   if (num_params(params) == 2) 
      return return_object(QC_QPointF, new QoreQPointF(qgi->getQGraphicsItem()->mapToItem(item->getQGraphicsItem(), x, y)));
   
   p = get_param(params, 3);
   qreal w = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 4);
   qreal h = p ? p->getAsFloat() : 0.0;
   return return_object(QC_QPolygonF, new QoreQPolygonF(qgi->getQGraphicsItem()->mapToItem(item->getQGraphicsItem(), x, y, w, h)));
}

//QPointF mapToParent ( const QPointF & point ) const
//QPolygonF mapToParent ( const QRectF & rect ) const
//QPolygonF mapToParent ( const QPolygonF & polygon ) const
//QPainterPath mapToParent ( const QPainterPath & path ) const
//QPolygonF mapToParent ( qreal x, qreal y, qreal w, qreal h ) const
//QPointF mapToParent ( qreal x, qreal y ) const
static AbstractQoreNode *QGRAPHICSITEM_mapToParent(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (p && p->getType() == NT_OBJECT) {
      QoreQRectF *rect = (QoreQRectF *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QRECTF, xsink);
      if (!rect) {
         QoreQPolygonF *polygon = (QoreQPolygonF *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QPOLYGONF, xsink);
         if (!polygon) {
            QoreQPainterPath *path = (QoreQPainterPath *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QPAINTERPATH, xsink);
            if (!path) {
               QoreQPointF *point = (QoreQPointF *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QPOINTF, xsink);
               if (!point) {
                  if (!xsink->isException())
                     xsink->raiseException("QGRAPHICSITEM-MAPTOPARENT-PARAM-ERROR", "QGraphicsItem::mapToParent() does not know how to handle arguments of class '%s' as passed as the first argument", reinterpret_cast<const QoreObject *>(p)->getClassName());
                  return 0;
               }
               ReferenceHolder<AbstractPrivateData> pointHolder(static_cast<AbstractPrivateData *>(point), xsink);
               return return_object(QC_QPointF, new QoreQPointF(qgi->getQGraphicsItem()->mapToParent(*(static_cast<QPointF *>(point)))));
            }
            ReferenceHolder<AbstractPrivateData> pathHolder(static_cast<AbstractPrivateData *>(path), xsink);
            return return_object(QC_QPainterPath, new QoreQPainterPath(qgi->getQGraphicsItem()->mapToParent(*(static_cast<QPainterPath *>(path)))));
         }
         ReferenceHolder<AbstractPrivateData> polygonHolder(static_cast<AbstractPrivateData *>(polygon), xsink);
         return return_object(QC_QPolygonF, new QoreQPolygonF(qgi->getQGraphicsItem()->mapToParent(*(static_cast<QPolygonF *>(polygon)))));
      }
      ReferenceHolder<AbstractPrivateData> rectHolder(static_cast<AbstractPrivateData *>(rect), xsink);
      return return_object(QC_QPolygonF, new QoreQPolygonF(qgi->getQGraphicsItem()->mapToParent(*(static_cast<QRectF *>(rect)))));
   }
   qreal x = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 1);
   qreal y = p ? p->getAsFloat() : 0.0;
   if (num_params(params) == 2)
      return return_object(QC_QPointF, new QoreQPointF(qgi->getQGraphicsItem()->mapToParent(x, y)));

   p = get_param(params, 2);
   qreal w = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 3);
   qreal h = p ? p->getAsFloat() : 0.0;
   return return_object(QC_QPolygonF, new QoreQPolygonF(qgi->getQGraphicsItem()->mapToParent(x, y, w, h)));
}

//QPointF mapToScene ( const QPointF & point ) const
//QPolygonF mapToScene ( const QRectF & rect ) const
//QPolygonF mapToScene ( const QPolygonF & polygon ) const
//QPainterPath mapToScene ( const QPainterPath & path ) const
//QPolygonF mapToScene ( qreal x, qreal y, qreal w, qreal h ) const
//QPointF mapToScene ( qreal x, qreal y ) const
static AbstractQoreNode *QGRAPHICSITEM_mapToScene(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (p && p->getType() == NT_OBJECT) {
      QoreQRectF *rect = (QoreQRectF *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QRECTF, xsink);
      if (!rect) {
         QoreQPolygonF *polygon = (QoreQPolygonF *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QPOLYGONF, xsink);
         if (!polygon) {
            QoreQPainterPath *path = (QoreQPainterPath *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QPAINTERPATH, xsink);
            if (!path) {
               QoreQPointF *point = (QoreQPointF *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QPOINTF, xsink);
               if (!point) {
                  if (!xsink->isException())
                     xsink->raiseException("QGRAPHICSITEM-MAPTOSCENE-PARAM-ERROR", "QGraphicsItem::mapToScene() does not know how to handle arguments of class '%s' as passed as the first argument", reinterpret_cast<const QoreObject *>(p)->getClassName());
                  return 0;
               }
               ReferenceHolder<AbstractPrivateData> pointHolder(static_cast<AbstractPrivateData *>(point), xsink);
               return return_object(QC_QPointF, new QoreQPointF(qgi->getQGraphicsItem()->mapToScene(*(static_cast<QPointF *>(point)))));
            }
            ReferenceHolder<AbstractPrivateData> pathHolder(static_cast<AbstractPrivateData *>(path), xsink);
            return return_object(QC_QPainterPath, new QoreQPainterPath(qgi->getQGraphicsItem()->mapToScene(*(static_cast<QPainterPath *>(path)))));
         }
         ReferenceHolder<AbstractPrivateData> polygonHolder(static_cast<AbstractPrivateData *>(polygon), xsink);
         return return_object(QC_QPolygonF, new QoreQPolygonF(qgi->getQGraphicsItem()->mapToScene(*(static_cast<QPolygonF *>(polygon)))));
      }
      ReferenceHolder<AbstractPrivateData> rectHolder(static_cast<AbstractPrivateData *>(rect), xsink);
      return return_object(QC_QPolygonF, new QoreQPolygonF(qgi->getQGraphicsItem()->mapToScene(*(static_cast<QRectF *>(rect)))));
   }
   qreal x = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 1);
   qreal y = p ? p->getAsFloat() : 0.0;
   if (num_params(params) == 2)
      return return_object(QC_QPointF, new QoreQPointF(qgi->getQGraphicsItem()->mapToScene(x, y)));

   p = get_param(params, 2);
   qreal w = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 3);
   qreal h = p ? p->getAsFloat() : 0.0;
   return return_object(QC_QPolygonF, new QoreQPolygonF(qgi->getQGraphicsItem()->mapToScene(x, y, w, h)));
}

//void moveBy ( qreal dx, qreal dy )
static AbstractQoreNode *QGRAPHICSITEM_moveBy(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   qreal dx = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 1);
   qreal dy = p ? p->getAsFloat() : 0.0;
   qgi->getQGraphicsItem()->moveBy(dx, dy);
   return 0;
}

//virtual QPainterPath opaqueArea () const
static AbstractQoreNode *QGRAPHICSITEM_opaqueArea(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QPainterPath, new QoreQPainterPath(qgi->getQGraphicsItem()->opaqueArea()));
}

//virtual void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 ) = 0
static AbstractQoreNode *QGRAPHICSITEM_paint(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQPainter *painter = (p && p->getType() == NT_OBJECT) ? (QoreQPainter *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QPAINTER, xsink) : 0;
   if (!painter) {
      if (!xsink->isException())
         xsink->raiseException("QGRAPHICSITEM-PAINT-PARAM-ERROR", "expecting a QPainter object as first argument to QGraphicsItem::paint()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> painterHolder(static_cast<AbstractPrivateData *>(painter), xsink);
   p = get_param(params, 1);
   QoreQStyleOptionGraphicsItem *option = (p && p->getType() == NT_OBJECT) ? (QoreQStyleOptionGraphicsItem *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QSTYLEOPTIONGRAPHICSITEM, xsink) : 0;
   if (!option) {
      if (!xsink->isException())
         xsink->raiseException("QGRAPHICSITEM-PAINT-PARAM-ERROR", "expecting a QStyleOptionGraphicsItem object as second argument to QGraphicsItem::paint()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);
   p = get_param(params, 2);
   QoreQWidget *widget = (p && p->getType() == NT_OBJECT) ? (QoreQWidget *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   qgi->getQGraphicsItem()->paint(painter->getQPainter(), static_cast<QStyleOptionGraphicsItem *>(option), widget ? static_cast<QWidget *>(widget->getQWidget()) : 0);
   return 0;
}

//QGraphicsItem * parentItem () const
static AbstractQoreNode *QGRAPHICSITEM_parentItem(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(self->getClass(CID_QGRAPHICSITEM), new QoreQtQGraphicsItem(qgi->getQGraphicsItem()->parentItem()));
}

//QPointF pos () const
static AbstractQoreNode *QGRAPHICSITEM_pos(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QPointF, new QoreQPointF(qgi->getQGraphicsItem()->pos()));
}

//void removeSceneEventFilter ( QGraphicsItem * filterItem )
static AbstractQoreNode *QGRAPHICSITEM_removeSceneEventFilter(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQGraphicsItem *filterItem = (p && p->getType() == NT_OBJECT) ? (QoreQGraphicsItem *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QGRAPHICSITEM, xsink) : 0;
   if (!filterItem) {
      if (!xsink->isException())
         xsink->raiseException("QGRAPHICSITEM-REMOVESCENEEVENTFILTER-PARAM-ERROR", "expecting a QGraphicsItem object as first argument to QGraphicsItem::removeSceneEventFilter()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> filterItemHolder(static_cast<AbstractPrivateData *>(filterItem), xsink);
   qgi->getQGraphicsItem()->removeSceneEventFilter(filterItem->getQGraphicsItem());
   return 0;
}

//void resetTransform ()
static AbstractQoreNode *QGRAPHICSITEM_resetTransform(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   qgi->getQGraphicsItem()->resetTransform();
   return 0;
}

//void rotate ( qreal angle )
static AbstractQoreNode *QGRAPHICSITEM_rotate(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   qreal angle = p ? p->getAsFloat() : 0.0;
   qgi->getQGraphicsItem()->rotate(angle);
   return 0;
}

//void scale ( qreal sx, qreal sy )
static AbstractQoreNode *QGRAPHICSITEM_scale(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   qreal sx = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 1);
   qreal sy = p ? p->getAsFloat() : 0.0;
   qgi->getQGraphicsItem()->scale(sx, sy);
   return 0;
}

/*
//QGraphicsScene * scene () const
static AbstractQoreNode *QGRAPHICSITEM_scene(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   ??? return new QoreBigIntNode(qgi->getQGraphicsItem()->scene());
}
*/

//QRectF sceneBoundingRect () const
static AbstractQoreNode *QGRAPHICSITEM_sceneBoundingRect(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QRectF, new QoreQRectF(qgi->getQGraphicsItem()->sceneBoundingRect()));
}

//QPointF scenePos () const
static AbstractQoreNode *QGRAPHICSITEM_scenePos(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QPointF, new QoreQPointF(qgi->getQGraphicsItem()->scenePos()));
}

//QTransform sceneTransform () const
static AbstractQoreNode *QGRAPHICSITEM_sceneTransform(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QTransform, new QoreQTransform(qgi->getQGraphicsItem()->sceneTransform()));
}

//void setAcceptDrops ( bool on )
static AbstractQoreNode *QGRAPHICSITEM_setAcceptDrops(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   bool on = p ? p->getAsBool() : false;
   qgi->getQGraphicsItem()->setAcceptDrops(on);
   return 0;
}

//void setAcceptedMouseButtons ( Qt::MouseButtons buttons )
static AbstractQoreNode *QGRAPHICSITEM_setAcceptedMouseButtons(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   Qt::MouseButtons buttons = (Qt::MouseButtons)(p ? p->getAsInt() : 0);
   qgi->getQGraphicsItem()->setAcceptedMouseButtons(buttons);
   return 0;
}

//void setAcceptsHoverEvents ( bool enabled )
static AbstractQoreNode *QGRAPHICSITEM_setAcceptsHoverEvents(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   bool enabled = p ? p->getAsBool() : false;
   qgi->getQGraphicsItem()->setAcceptsHoverEvents(enabled);
   return 0;
}

//void setCursor ( const QCursor & cursor )
static AbstractQoreNode *QGRAPHICSITEM_setCursor(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQCursor *cursor = (p && p->getType() == NT_OBJECT) ? (QoreQCursor *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QCURSOR, xsink) : 0;
   if (!cursor) {
      if (!xsink->isException())
         xsink->raiseException("QGRAPHICSITEM-SETCURSOR-PARAM-ERROR", "expecting a QCursor object as first argument to QGraphicsItem::setCursor()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> cursorHolder(static_cast<AbstractPrivateData *>(cursor), xsink);
   qgi->getQGraphicsItem()->setCursor(*(static_cast<QCursor *>(cursor)));
   return 0;
}

//void setData ( int key, const QVariant & value )
static AbstractQoreNode *QGRAPHICSITEM_setData(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int key = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   QVariant value;
   if (get_qvariant(p, value, xsink))
      return 0;
   qgi->getQGraphicsItem()->setData(key, value);
   return 0;
}

//void setEnabled ( bool enabled )
static AbstractQoreNode *QGRAPHICSITEM_setEnabled(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   bool enabled = p ? p->getAsBool() : false;
   qgi->getQGraphicsItem()->setEnabled(enabled);
   return 0;
}

//void setFlag ( GraphicsItemFlag flag, bool enabled = true )
static AbstractQoreNode *QGRAPHICSITEM_setFlag(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QGraphicsItem::GraphicsItemFlag flag = (QGraphicsItem::GraphicsItemFlag)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   bool enabled = !is_nothing(p) ? p->getAsBool() : true;
   qgi->getQGraphicsItem()->setFlag(flag, enabled);
   return 0;
}

//void setFlags ( GraphicsItemFlags flags )
static AbstractQoreNode *QGRAPHICSITEM_setFlags(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QGraphicsItem::GraphicsItemFlags flags = (QGraphicsItem::GraphicsItemFlags)(p ? p->getAsInt() : 0);
   qgi->getQGraphicsItem()->setFlags(flags);
   return 0;
}

//void setFocus ( Qt::FocusReason focusReason = Qt::OtherFocusReason )
static AbstractQoreNode *QGRAPHICSITEM_setFocus(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   Qt::FocusReason focusReason = !is_nothing(p) ? (Qt::FocusReason)p->getAsInt() : Qt::OtherFocusReason;
   qgi->getQGraphicsItem()->setFocus(focusReason);
   return 0;
}

/*
//void setGroup ( QGraphicsItemGroup * group )
static AbstractQoreNode *QGRAPHICSITEM_setGroup(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? QGraphicsItemGroup* group = p;
   qgi->getQGraphicsItem()->setGroup(group);
   return 0;
}
*/

//void setHandlesChildEvents ( bool enabled )
static AbstractQoreNode *QGRAPHICSITEM_setHandlesChildEvents(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   bool enabled = p ? p->getAsBool() : false;
   qgi->getQGraphicsItem()->setHandlesChildEvents(enabled);
   return 0;
}

//void setParentItem ( QGraphicsItem * parent )
static AbstractQoreNode *QGRAPHICSITEM_setParentItem(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQGraphicsItem *parent = (p && p->getType() == NT_OBJECT) ? (QoreQGraphicsItem *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QGRAPHICSITEM, xsink) : 0;
   if (!parent) {
      if (!xsink->isException())
         xsink->raiseException("QGRAPHICSITEM-SETPARENTITEM-PARAM-ERROR", "expecting a QGraphicsItem object as first argument to QGraphicsItem::setParentItem()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   qgi->getQGraphicsItem()->setParentItem(parent->getQGraphicsItem());
   return 0;
}

//void setPos ( const QPointF & pos )
//void setPos ( qreal x, qreal y )
static AbstractQoreNode *QGRAPHICSITEM_setPos(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (p && p->getType() == NT_OBJECT) {
      QoreQPointF *pos = (QoreQPointF *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QPOINTF, xsink);
      if (!pos) {
         if (!xsink->isException())
            xsink->raiseException("QGRAPHICSITEM-SETPOS-PARAM-ERROR", "QGraphicsItem::setPos() does not know how to handle arguments of class '%s' as passed as the first argument", reinterpret_cast<const QoreObject *>(p)->getClassName());
         return 0;
      }
      ReferenceHolder<AbstractPrivateData> posHolder(static_cast<AbstractPrivateData *>(pos), xsink);
      qgi->getQGraphicsItem()->setPos(*(static_cast<QPointF *>(pos)));
      return 0;
   }
   qreal x = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 1);
   qreal y = p ? p->getAsFloat() : 0.0;
   qgi->getQGraphicsItem()->setPos(x, y);
   return 0;
}

//void setSelected ( bool selected )
static AbstractQoreNode *QGRAPHICSITEM_setSelected(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   bool selected = p ? p->getAsBool() : false;
   qgi->getQGraphicsItem()->setSelected(selected);
   return 0;
}

//void setToolTip ( const QString & toolTip )
static AbstractQoreNode *QGRAPHICSITEM_setToolTip(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QString toolTip;
   if (get_qstring(p, toolTip, xsink))
      return 0;
   qgi->getQGraphicsItem()->setToolTip(toolTip);
   return 0;
}

//void setTransform ( const QTransform & matrix, bool combine = false )
static AbstractQoreNode *QGRAPHICSITEM_setTransform(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQTransform *matrix = (p && p->getType() == NT_OBJECT) ? (QoreQTransform *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QTRANSFORM, xsink) : 0;
   if (!matrix) {
      if (!xsink->isException())
         xsink->raiseException("QGRAPHICSITEM-SETTRANSFORM-PARAM-ERROR", "expecting a QTransform object as first argument to QGraphicsItem::setTransform()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> matrixHolder(static_cast<AbstractPrivateData *>(matrix), xsink);
   p = get_param(params, 1);
   bool combine = p ? p->getAsBool() : false;
   qgi->getQGraphicsItem()->setTransform(*(static_cast<QTransform *>(matrix)), combine);
   return 0;
}

//void setVisible ( bool visible )
static AbstractQoreNode *QGRAPHICSITEM_setVisible(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   bool visible = p ? p->getAsBool() : false;
   qgi->getQGraphicsItem()->setVisible(visible);
   return 0;
}

//void setZValue ( qreal z )
static AbstractQoreNode *QGRAPHICSITEM_setZValue(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   qreal z = p ? p->getAsFloat() : 0.0;
   qgi->getQGraphicsItem()->setZValue(z);
   return 0;
}

//virtual QPainterPath shape () const
static AbstractQoreNode *QGRAPHICSITEM_shape(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QPainterPath, new QoreQPainterPath(qgi->getQGraphicsItem()->shape()));
}

//void shear ( qreal sh, qreal sv )
static AbstractQoreNode *QGRAPHICSITEM_shear(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   qreal sh = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 1);
   qreal sv = p ? p->getAsFloat() : 0.0;
   qgi->getQGraphicsItem()->shear(sh, sv);
   return 0;
}

//void show ()
static AbstractQoreNode *QGRAPHICSITEM_show(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   qgi->getQGraphicsItem()->show();
   return 0;
}

//QString toolTip () const
static AbstractQoreNode *QGRAPHICSITEM_toolTip(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qgi->getQGraphicsItem()->toolTip().toUtf8().data(), QCS_UTF8);
}

//QGraphicsItem * topLevelItem () const
static AbstractQoreNode *QGRAPHICSITEM_topLevelItem(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(self->getClass(CID_QGRAPHICSITEM), new QoreQtQGraphicsItem(qgi->getQGraphicsItem()->topLevelItem()));
}

//QTransform transform () const
static AbstractQoreNode *QGRAPHICSITEM_transform(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QTransform, new QoreQTransform(qgi->getQGraphicsItem()->transform()));
}

//void translate ( qreal dx, qreal dy )
static AbstractQoreNode *QGRAPHICSITEM_translate(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   qreal dx = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 1);
   qreal dy = p ? p->getAsFloat() : 0.0;
   qgi->getQGraphicsItem()->translate(dx, dy);
   return 0;
}

//virtual int type () const
static AbstractQoreNode *QGRAPHICSITEM_type(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qgi->getQGraphicsItem()->type());
}

//void unsetCursor ()
static AbstractQoreNode *QGRAPHICSITEM_unsetCursor(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   qgi->getQGraphicsItem()->unsetCursor();
   return 0;
}

//void update ( const QRectF & rect = QRectF() )
//void update ( qreal x, qreal y, qreal width, qreal height )
static AbstractQoreNode *QGRAPHICSITEM_update(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      qgi->getQGraphicsItem()->update();
      return 0;
   }
   if (p && p->getType() == NT_OBJECT) {
      QoreQRectF *rect = (QoreQRectF *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QRECTF, xsink);
      if (*xsink)
         return 0;
      ReferenceHolder<AbstractPrivateData> rectHolder(static_cast<AbstractPrivateData *>(rect), xsink);
      qgi->getQGraphicsItem()->update(*(static_cast<QRectF *>(rect)));
      return 0;
   }
   qreal x = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 1);
   qreal y = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 2);
   qreal width = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 3);
   qreal height = p ? p->getAsFloat() : 0.0;
   qgi->getQGraphicsItem()->update(x, y, width, height);
   return 0;
}

//qreal x () const
static AbstractQoreNode *QGRAPHICSITEM_x(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreFloatNode(qgi->getQGraphicsItem()->x());
}

//qreal y () const
static AbstractQoreNode *QGRAPHICSITEM_y(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreFloatNode(qgi->getQGraphicsItem()->y());
}

//qreal zValue () const
static AbstractQoreNode *QGRAPHICSITEM_zValue(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreFloatNode(qgi->getQGraphicsItem()->zValue());
}

//virtual void contextMenuEvent ( QGraphicsSceneContextMenuEvent * event )
static AbstractQoreNode *QGRAPHICSITEM_contextMenuEvent(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQGraphicsSceneContextMenuEvent *event = (p && p->getType() == NT_OBJECT) ? (QoreQGraphicsSceneContextMenuEvent *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QGRAPHICSSCENECONTEXTMENUEVENT, xsink) : 0;
   if (!event) {
      if (!xsink->isException())
         xsink->raiseException("QGRAPHICSITEM-CONTEXTMENUEVENT-PARAM-ERROR", "expecting a QGraphicsSceneContextMenuEvent object as first argument to QGraphicsItem::contextMenuEvent()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> eventHolder(static_cast<AbstractPrivateData *>(event), xsink);
   qgi->contextMenuEvent(static_cast<QGraphicsSceneContextMenuEvent *>(event));
   return 0;
}

//virtual void dragEnterEvent ( QGraphicsSceneDragDropEvent * event )
static AbstractQoreNode *QGRAPHICSITEM_dragEnterEvent(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQGraphicsSceneDragDropEvent *event = (p && p->getType() == NT_OBJECT) ? (QoreQGraphicsSceneDragDropEvent *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QGRAPHICSSCENEDRAGDROPEVENT, xsink) : 0;
   if (!event) {
      if (!xsink->isException())
         xsink->raiseException("QGRAPHICSITEM-DRAGENTEREVENT-PARAM-ERROR", "expecting a QGraphicsSceneDragDropEvent object as first argument to QGraphicsItem::dragEnterEvent()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> eventHolder(static_cast<AbstractPrivateData *>(event), xsink);
   qgi->dragEnterEvent(static_cast<QGraphicsSceneDragDropEvent *>(event));
   return 0;
}

//virtual void dragLeaveEvent ( QGraphicsSceneDragDropEvent * event )
static AbstractQoreNode *QGRAPHICSITEM_dragLeaveEvent(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQGraphicsSceneDragDropEvent *event = (p && p->getType() == NT_OBJECT) ? (QoreQGraphicsSceneDragDropEvent *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QGRAPHICSSCENEDRAGDROPEVENT, xsink) : 0;
   if (!event) {
      if (!xsink->isException())
         xsink->raiseException("QGRAPHICSITEM-DRAGLEAVEEVENT-PARAM-ERROR", "expecting a QGraphicsSceneDragDropEvent object as first argument to QGraphicsItem::dragLeaveEvent()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> eventHolder(static_cast<AbstractPrivateData *>(event), xsink);
   qgi->dragLeaveEvent(static_cast<QGraphicsSceneDragDropEvent *>(event));
   return 0;
}

//virtual void dragMoveEvent ( QGraphicsSceneDragDropEvent * event )
static AbstractQoreNode *QGRAPHICSITEM_dragMoveEvent(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQGraphicsSceneDragDropEvent *event = (p && p->getType() == NT_OBJECT) ? (QoreQGraphicsSceneDragDropEvent *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QGRAPHICSSCENEDRAGDROPEVENT, xsink) : 0;
   if (!event) {
      if (!xsink->isException())
         xsink->raiseException("QGRAPHICSITEM-DRAGMOVEEVENT-PARAM-ERROR", "expecting a QGraphicsSceneDragDropEvent object as first argument to QGraphicsItem::dragMoveEvent()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> eventHolder(static_cast<AbstractPrivateData *>(event), xsink);
   qgi->dragMoveEvent(static_cast<QGraphicsSceneDragDropEvent *>(event));
   return 0;
}

//virtual void dropEvent ( QGraphicsSceneDragDropEvent * event )
static AbstractQoreNode *QGRAPHICSITEM_dropEvent(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQGraphicsSceneDragDropEvent *event = (p && p->getType() == NT_OBJECT) ? (QoreQGraphicsSceneDragDropEvent *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QGRAPHICSSCENEDRAGDROPEVENT, xsink) : 0;
   if (!event) {
      if (!xsink->isException())
         xsink->raiseException("QGRAPHICSITEM-DROPEVENT-PARAM-ERROR", "expecting a QGraphicsSceneDragDropEvent object as first argument to QGraphicsItem::dropEvent()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> eventHolder(static_cast<AbstractPrivateData *>(event), xsink);
   qgi->dropEvent(static_cast<QGraphicsSceneDragDropEvent *>(event));
   return 0;
}

//virtual void focusInEvent ( QFocusEvent * event )
static AbstractQoreNode *QGRAPHICSITEM_focusInEvent(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQFocusEvent *event = (p && p->getType() == NT_OBJECT) ? (QoreQFocusEvent *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QFOCUSEVENT, xsink) : 0;
   if (!event) {
      if (!xsink->isException())
         xsink->raiseException("QGRAPHICSITEM-FOCUSINEVENT-PARAM-ERROR", "expecting a QFocusEvent object as first argument to QGraphicsItem::focusInEvent()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> eventHolder(static_cast<AbstractPrivateData *>(event), xsink);
   qgi->focusInEvent(static_cast<QFocusEvent *>(event));
   return 0;
}

//virtual void focusOutEvent ( QFocusEvent * event )
static AbstractQoreNode *QGRAPHICSITEM_focusOutEvent(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQFocusEvent *event = (p && p->getType() == NT_OBJECT) ? (QoreQFocusEvent *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QFOCUSEVENT, xsink) : 0;
   if (!event) {
      if (!xsink->isException())
         xsink->raiseException("QGRAPHICSITEM-FOCUSOUTEVENT-PARAM-ERROR", "expecting a QFocusEvent object as first argument to QGraphicsItem::focusOutEvent()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> eventHolder(static_cast<AbstractPrivateData *>(event), xsink);
   qgi->focusOutEvent(static_cast<QFocusEvent *>(event));
   return 0;
}

//virtual void hoverEnterEvent ( QGraphicsSceneHoverEvent * event )
static AbstractQoreNode *QGRAPHICSITEM_hoverEnterEvent(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQGraphicsSceneHoverEvent *event = (p && p->getType() == NT_OBJECT) ? (QoreQGraphicsSceneHoverEvent *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QGRAPHICSSCENEHOVEREVENT, xsink) : 0;
   if (!event) {
      if (!xsink->isException())
         xsink->raiseException("QGRAPHICSITEM-HOVERENTEREVENT-PARAM-ERROR", "expecting a QGraphicsSceneHoverEvent object as first argument to QGraphicsItem::hoverEnterEvent()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> eventHolder(static_cast<AbstractPrivateData *>(event), xsink);
   qgi->hoverEnterEvent(static_cast<QGraphicsSceneHoverEvent *>(event));
   return 0;
}

//virtual void hoverLeaveEvent ( QGraphicsSceneHoverEvent * event )
static AbstractQoreNode *QGRAPHICSITEM_hoverLeaveEvent(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQGraphicsSceneHoverEvent *event = (p && p->getType() == NT_OBJECT) ? (QoreQGraphicsSceneHoverEvent *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QGRAPHICSSCENEHOVEREVENT, xsink) : 0;
   if (!event) {
      if (!xsink->isException())
         xsink->raiseException("QGRAPHICSITEM-HOVERLEAVEEVENT-PARAM-ERROR", "expecting a QGraphicsSceneHoverEvent object as first argument to QGraphicsItem::hoverLeaveEvent()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> eventHolder(static_cast<AbstractPrivateData *>(event), xsink);
   qgi->hoverLeaveEvent(static_cast<QGraphicsSceneHoverEvent *>(event));
   return 0;
}

//virtual void hoverMoveEvent ( QGraphicsSceneHoverEvent * event )
static AbstractQoreNode *QGRAPHICSITEM_hoverMoveEvent(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQGraphicsSceneHoverEvent *event = (p && p->getType() == NT_OBJECT) ? (QoreQGraphicsSceneHoverEvent *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QGRAPHICSSCENEHOVEREVENT, xsink) : 0;
   if (!event) {
      if (!xsink->isException())
         xsink->raiseException("QGRAPHICSITEM-HOVERMOVEEVENT-PARAM-ERROR", "expecting a QGraphicsSceneHoverEvent object as first argument to QGraphicsItem::hoverMoveEvent()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> eventHolder(static_cast<AbstractPrivateData *>(event), xsink);
   qgi->hoverMoveEvent(static_cast<QGraphicsSceneHoverEvent *>(event));
   return 0;
}

//virtual void inputMethodEvent ( QInputMethodEvent * event )
static AbstractQoreNode *QGRAPHICSITEM_inputMethodEvent(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQInputMethodEvent *event = (p && p->getType() == NT_OBJECT) ? (QoreQInputMethodEvent *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QINPUTMETHODEVENT, xsink) : 0;
   if (!event) {
      if (!xsink->isException())
         xsink->raiseException("QGRAPHICSITEM-INPUTMETHODEVENT-PARAM-ERROR", "expecting a QInputMethodEvent object as first argument to QGraphicsItem::inputMethodEvent()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> eventHolder(static_cast<AbstractPrivateData *>(event), xsink);
   qgi->inputMethodEvent(static_cast<QInputMethodEvent *>(event));
   return 0;
}

//virtual QVariant inputMethodQuery ( Qt::InputMethodQuery query ) const
static AbstractQoreNode *QGRAPHICSITEM_inputMethodQuery(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   Qt::InputMethodQuery query = (Qt::InputMethodQuery)(p ? p->getAsInt() : 0);
   return return_qvariant(qgi->inputMethodQuery(query));
}

//virtual QVariant itemChange ( GraphicsItemChange change, const QVariant & value )
static AbstractQoreNode *QGRAPHICSITEM_itemChange(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QGraphicsItem::GraphicsItemChange change = (QGraphicsItem::GraphicsItemChange)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QVariant value;
   if (get_qvariant(p, value, xsink))
      return 0;
   return return_qvariant(qgi->itemChange(change, value));
}

//virtual void keyPressEvent ( QKeyEvent * event )
static AbstractQoreNode *QGRAPHICSITEM_keyPressEvent(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQKeyEvent *event = (p && p->getType() == NT_OBJECT) ? (QoreQKeyEvent *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QKEYEVENT, xsink) : 0;
   if (!event) {
      if (!xsink->isException())
         xsink->raiseException("QGRAPHICSITEM-KEYPRESSEVENT-PARAM-ERROR", "expecting a QKeyEvent object as first argument to QGraphicsItem::keyPressEvent()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> eventHolder(static_cast<AbstractPrivateData *>(event), xsink);
   qgi->keyPressEvent(static_cast<QKeyEvent *>(event));
   return 0;
}

//virtual void keyReleaseEvent ( QKeyEvent * event )
static AbstractQoreNode *QGRAPHICSITEM_keyReleaseEvent(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQKeyEvent *event = (p && p->getType() == NT_OBJECT) ? (QoreQKeyEvent *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QKEYEVENT, xsink) : 0;
   if (!event) {
      if (!xsink->isException())
         xsink->raiseException("QGRAPHICSITEM-KEYRELEASEEVENT-PARAM-ERROR", "expecting a QKeyEvent object as first argument to QGraphicsItem::keyReleaseEvent()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> eventHolder(static_cast<AbstractPrivateData *>(event), xsink);
   qgi->keyReleaseEvent(static_cast<QKeyEvent *>(event));
   return 0;
}

//virtual void mouseDoubleClickEvent ( QGraphicsSceneMouseEvent * event )
static AbstractQoreNode *QGRAPHICSITEM_mouseDoubleClickEvent(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQGraphicsSceneMouseEvent *event = (p && p->getType() == NT_OBJECT) ? (QoreQGraphicsSceneMouseEvent *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QGRAPHICSSCENEMOUSEEVENT, xsink) : 0;
   if (!event) {
      if (!xsink->isException())
         xsink->raiseException("QGRAPHICSITEM-MOUSEDOUBLECLICKEVENT-PARAM-ERROR", "expecting a QGraphicsSceneMouseEvent object as first argument to QGraphicsItem::mouseDoubleClickEvent()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> eventHolder(static_cast<AbstractPrivateData *>(event), xsink);
   qgi->mouseDoubleClickEvent(static_cast<QGraphicsSceneMouseEvent *>(event));
   return 0;
}

//virtual void mouseMoveEvent ( QGraphicsSceneMouseEvent * event )
static AbstractQoreNode *QGRAPHICSITEM_mouseMoveEvent(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQGraphicsSceneMouseEvent *event = (p && p->getType() == NT_OBJECT) ? (QoreQGraphicsSceneMouseEvent *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QGRAPHICSSCENEMOUSEEVENT, xsink) : 0;
   if (!event) {
      if (!xsink->isException())
         xsink->raiseException("QGRAPHICSITEM-MOUSEMOVEEVENT-PARAM-ERROR", "expecting a QGraphicsSceneMouseEvent object as first argument to QGraphicsItem::mouseMoveEvent()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> eventHolder(static_cast<AbstractPrivateData *>(event), xsink);
   qgi->mouseMoveEvent(static_cast<QGraphicsSceneMouseEvent *>(event));
   return 0;
}

//virtual void mousePressEvent ( QGraphicsSceneMouseEvent * event )
static AbstractQoreNode *QGRAPHICSITEM_mousePressEvent(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQGraphicsSceneMouseEvent *event = (p && p->getType() == NT_OBJECT) ? (QoreQGraphicsSceneMouseEvent *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QGRAPHICSSCENEMOUSEEVENT, xsink) : 0;
   if (!event) {
      if (!xsink->isException())
         xsink->raiseException("QGRAPHICSITEM-MOUSEPRESSEVENT-PARAM-ERROR", "expecting a QGraphicsSceneMouseEvent object as first argument to QGraphicsItem::mousePressEvent()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> eventHolder(static_cast<AbstractPrivateData *>(event), xsink);
   qgi->mousePressEvent(static_cast<QGraphicsSceneMouseEvent *>(event));
   return 0;
}

//virtual void mouseReleaseEvent ( QGraphicsSceneMouseEvent * event )
static AbstractQoreNode *QGRAPHICSITEM_mouseReleaseEvent(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQGraphicsSceneMouseEvent *event = (p && p->getType() == NT_OBJECT) ? (QoreQGraphicsSceneMouseEvent *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QGRAPHICSSCENEMOUSEEVENT, xsink) : 0;
   if (!event) {
      if (!xsink->isException())
         xsink->raiseException("QGRAPHICSITEM-MOUSERELEASEEVENT-PARAM-ERROR", "expecting a QGraphicsSceneMouseEvent object as first argument to QGraphicsItem::mouseReleaseEvent()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> eventHolder(static_cast<AbstractPrivateData *>(event), xsink);
   qgi->mouseReleaseEvent(static_cast<QGraphicsSceneMouseEvent *>(event));
   return 0;
}

//void prepareGeometryChange ()
static AbstractQoreNode *QGRAPHICSITEM_prepareGeometryChange(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   qgi->prepareGeometryChange();
   return 0;
}

//virtual bool sceneEvent ( QEvent * event )
static AbstractQoreNode *QGRAPHICSITEM_sceneEvent(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQEvent *event = (p && p->getType() == NT_OBJECT) ? (QoreQEvent *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QEVENT, xsink) : 0;
   if (!event) {
      if (!xsink->isException())
         xsink->raiseException("QGRAPHICSITEM-SCENEEVENT-PARAM-ERROR", "expecting a QEvent object as first argument to QGraphicsItem::sceneEvent()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> eventHolder(static_cast<AbstractPrivateData *>(event), xsink);
   return get_bool_node(qgi->sceneEvent(static_cast<QEvent *>(event)));
}

//virtual bool sceneEventFilter ( QGraphicsItem * watched, QEvent * event )
static AbstractQoreNode *QGRAPHICSITEM_sceneEventFilter(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQGraphicsItem *watched = (p && p->getType() == NT_OBJECT) ? (QoreQGraphicsItem *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QGRAPHICSITEM, xsink) : 0;
   if (!watched) {
      if (!xsink->isException())
         xsink->raiseException("QGRAPHICSITEM-SCENEEVENTFILTER-PARAM-ERROR", "expecting a QGraphicsItem object as first argument to QGraphicsItem::sceneEventFilter()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> watchedHolder(static_cast<AbstractPrivateData *>(watched), xsink);
   p = get_param(params, 1);
   QoreQEvent *event = (p && p->getType() == NT_OBJECT) ? (QoreQEvent *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QEVENT, xsink) : 0;
   if (!event) {
      if (!xsink->isException())
         xsink->raiseException("QGRAPHICSITEM-SCENEEVENTFILTER-PARAM-ERROR", "expecting a QEvent object as second argument to QGraphicsItem::sceneEventFilter()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> eventHolder(static_cast<AbstractPrivateData *>(event), xsink);
   return get_bool_node(qgi->sceneEventFilter(watched->getQGraphicsItem(), static_cast<QEvent *>(event)));
}

//virtual void wheelEvent ( QGraphicsSceneWheelEvent * event )
static AbstractQoreNode *QGRAPHICSITEM_wheelEvent(QoreObject *self, QoreAbstractQGraphicsItemData *qgi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQGraphicsSceneWheelEvent *event = (p && p->getType() == NT_OBJECT) ? (QoreQGraphicsSceneWheelEvent *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QGRAPHICSSCENEWHEELEVENT, xsink) : 0;
   if (!event) {
      if (!xsink->isException())
         xsink->raiseException("QGRAPHICSITEM-WHEELEVENT-PARAM-ERROR", "expecting a QGraphicsSceneWheelEvent object as first argument to QGraphicsItem::wheelEvent()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> eventHolder(static_cast<AbstractPrivateData *>(event), xsink);
   qgi->wheelEvent(static_cast<QGraphicsSceneWheelEvent *>(event));
   return 0;
}

static QoreClass *initQGraphicsItemClass()
{
   QC_QGraphicsItem = new QoreClass("QGraphicsItem", QDOM_GUI);
   CID_QGRAPHICSITEM = QC_QGraphicsItem->getID();

   QC_QGraphicsItem->setConstructor(QGRAPHICSITEM_constructor);
   QC_QGraphicsItem->setCopy((q_copy_t)QGRAPHICSITEM_copy);

   QC_QGraphicsItem->addMethod("acceptDrops",                 (q_method_t)QGRAPHICSITEM_acceptDrops);
   QC_QGraphicsItem->addMethod("acceptedMouseButtons",        (q_method_t)QGRAPHICSITEM_acceptedMouseButtons);
   QC_QGraphicsItem->addMethod("acceptsHoverEvents",          (q_method_t)QGRAPHICSITEM_acceptsHoverEvents);
   QC_QGraphicsItem->addMethod("advance",                     (q_method_t)QGRAPHICSITEM_advance);
   QC_QGraphicsItem->addMethod("boundingRect",                (q_method_t)QGRAPHICSITEM_boundingRect);
   //QC_QGraphicsItem->addMethod("children",                    (q_method_t)QGRAPHICSITEM_children);
   QC_QGraphicsItem->addMethod("childrenBoundingRect",        (q_method_t)QGRAPHICSITEM_childrenBoundingRect);
   QC_QGraphicsItem->addMethod("clearFocus",                  (q_method_t)QGRAPHICSITEM_clearFocus);
   QC_QGraphicsItem->addMethod("collidesWithItem",            (q_method_t)QGRAPHICSITEM_collidesWithItem);
   QC_QGraphicsItem->addMethod("collidesWithPath",            (q_method_t)QGRAPHICSITEM_collidesWithPath);
   //QC_QGraphicsItem->addMethod("collidingItems",              (q_method_t)QGRAPHICSITEM_collidingItems);
   QC_QGraphicsItem->addMethod("contains",                    (q_method_t)QGRAPHICSITEM_contains);
   QC_QGraphicsItem->addMethod("cursor",                      (q_method_t)QGRAPHICSITEM_cursor);
   QC_QGraphicsItem->addMethod("data",                        (q_method_t)QGRAPHICSITEM_data);
   QC_QGraphicsItem->addMethod("deviceTransform",             (q_method_t)QGRAPHICSITEM_deviceTransform);
   QC_QGraphicsItem->addMethod("ensureVisible",               (q_method_t)QGRAPHICSITEM_ensureVisible);
   QC_QGraphicsItem->addMethod("flags",                       (q_method_t)QGRAPHICSITEM_flags);
   //QC_QGraphicsItem->addMethod("group",                       (q_method_t)QGRAPHICSITEM_group);
   QC_QGraphicsItem->addMethod("handlesChildEvents",          (q_method_t)QGRAPHICSITEM_handlesChildEvents);
   QC_QGraphicsItem->addMethod("hasCursor",                   (q_method_t)QGRAPHICSITEM_hasCursor);
   QC_QGraphicsItem->addMethod("hasFocus",                    (q_method_t)QGRAPHICSITEM_hasFocus);
   QC_QGraphicsItem->addMethod("hide",                        (q_method_t)QGRAPHICSITEM_hide);
   QC_QGraphicsItem->addMethod("installSceneEventFilter",     (q_method_t)QGRAPHICSITEM_installSceneEventFilter);
   QC_QGraphicsItem->addMethod("isAncestorOf",                (q_method_t)QGRAPHICSITEM_isAncestorOf);
   QC_QGraphicsItem->addMethod("isEnabled",                   (q_method_t)QGRAPHICSITEM_isEnabled);
   QC_QGraphicsItem->addMethod("isObscured",                  (q_method_t)QGRAPHICSITEM_isObscured);
   QC_QGraphicsItem->addMethod("isObscuredBy",                (q_method_t)QGRAPHICSITEM_isObscuredBy);
   QC_QGraphicsItem->addMethod("isSelected",                  (q_method_t)QGRAPHICSITEM_isSelected);
   QC_QGraphicsItem->addMethod("isVisible",                   (q_method_t)QGRAPHICSITEM_isVisible);
   QC_QGraphicsItem->addMethod("mapFromItem",                 (q_method_t)QGRAPHICSITEM_mapFromItem);
   QC_QGraphicsItem->addMethod("mapFromParent",               (q_method_t)QGRAPHICSITEM_mapFromParent);
   QC_QGraphicsItem->addMethod("mapFromScene",                (q_method_t)QGRAPHICSITEM_mapFromScene);
   QC_QGraphicsItem->addMethod("mapToItem",                   (q_method_t)QGRAPHICSITEM_mapToItem);
   QC_QGraphicsItem->addMethod("mapToParent",                 (q_method_t)QGRAPHICSITEM_mapToParent);
   QC_QGraphicsItem->addMethod("mapToScene",                  (q_method_t)QGRAPHICSITEM_mapToScene);
   QC_QGraphicsItem->addMethod("moveBy",                      (q_method_t)QGRAPHICSITEM_moveBy);
   QC_QGraphicsItem->addMethod("opaqueArea",                  (q_method_t)QGRAPHICSITEM_opaqueArea);
   QC_QGraphicsItem->addMethod("paint",                       (q_method_t)QGRAPHICSITEM_paint);
   QC_QGraphicsItem->addMethod("parentItem",                  (q_method_t)QGRAPHICSITEM_parentItem);
   QC_QGraphicsItem->addMethod("pos",                         (q_method_t)QGRAPHICSITEM_pos);
   QC_QGraphicsItem->addMethod("removeSceneEventFilter",      (q_method_t)QGRAPHICSITEM_removeSceneEventFilter);
   QC_QGraphicsItem->addMethod("resetTransform",              (q_method_t)QGRAPHICSITEM_resetTransform);
   QC_QGraphicsItem->addMethod("rotate",                      (q_method_t)QGRAPHICSITEM_rotate);
   QC_QGraphicsItem->addMethod("scale",                       (q_method_t)QGRAPHICSITEM_scale);
   //QC_QGraphicsItem->addMethod("scene",                       (q_method_t)QGRAPHICSITEM_scene);
   QC_QGraphicsItem->addMethod("sceneBoundingRect",           (q_method_t)QGRAPHICSITEM_sceneBoundingRect);
   QC_QGraphicsItem->addMethod("scenePos",                    (q_method_t)QGRAPHICSITEM_scenePos);
   QC_QGraphicsItem->addMethod("sceneTransform",              (q_method_t)QGRAPHICSITEM_sceneTransform);
   QC_QGraphicsItem->addMethod("setAcceptDrops",              (q_method_t)QGRAPHICSITEM_setAcceptDrops);
   QC_QGraphicsItem->addMethod("setAcceptedMouseButtons",     (q_method_t)QGRAPHICSITEM_setAcceptedMouseButtons);
   QC_QGraphicsItem->addMethod("setAcceptsHoverEvents",       (q_method_t)QGRAPHICSITEM_setAcceptsHoverEvents);
   QC_QGraphicsItem->addMethod("setCursor",                   (q_method_t)QGRAPHICSITEM_setCursor);
   QC_QGraphicsItem->addMethod("setData",                     (q_method_t)QGRAPHICSITEM_setData);
   QC_QGraphicsItem->addMethod("setEnabled",                  (q_method_t)QGRAPHICSITEM_setEnabled);
   QC_QGraphicsItem->addMethod("setFlag",                     (q_method_t)QGRAPHICSITEM_setFlag);
   QC_QGraphicsItem->addMethod("setFlags",                    (q_method_t)QGRAPHICSITEM_setFlags);
   QC_QGraphicsItem->addMethod("setFocus",                    (q_method_t)QGRAPHICSITEM_setFocus);
   //QC_QGraphicsItem->addMethod("setGroup",                    (q_method_t)QGRAPHICSITEM_setGroup);
   QC_QGraphicsItem->addMethod("setHandlesChildEvents",       (q_method_t)QGRAPHICSITEM_setHandlesChildEvents);
   QC_QGraphicsItem->addMethod("setParentItem",               (q_method_t)QGRAPHICSITEM_setParentItem);
   QC_QGraphicsItem->addMethod("setPos",                      (q_method_t)QGRAPHICSITEM_setPos);
   QC_QGraphicsItem->addMethod("setSelected",                 (q_method_t)QGRAPHICSITEM_setSelected);
   QC_QGraphicsItem->addMethod("setToolTip",                  (q_method_t)QGRAPHICSITEM_setToolTip);
   QC_QGraphicsItem->addMethod("setTransform",                (q_method_t)QGRAPHICSITEM_setTransform);
   QC_QGraphicsItem->addMethod("setVisible",                  (q_method_t)QGRAPHICSITEM_setVisible);
   QC_QGraphicsItem->addMethod("setZValue",                   (q_method_t)QGRAPHICSITEM_setZValue);
   QC_QGraphicsItem->addMethod("shape",                       (q_method_t)QGRAPHICSITEM_shape);
   QC_QGraphicsItem->addMethod("shear",                       (q_method_t)QGRAPHICSITEM_shear);
   QC_QGraphicsItem->addMethod("show",                        (q_method_t)QGRAPHICSITEM_show);
   QC_QGraphicsItem->addMethod("toolTip",                     (q_method_t)QGRAPHICSITEM_toolTip);
   QC_QGraphicsItem->addMethod("topLevelItem",                (q_method_t)QGRAPHICSITEM_topLevelItem);
   QC_QGraphicsItem->addMethod("transform",                   (q_method_t)QGRAPHICSITEM_transform);
   QC_QGraphicsItem->addMethod("translate",                   (q_method_t)QGRAPHICSITEM_translate);
   QC_QGraphicsItem->addMethod("type",                        (q_method_t)QGRAPHICSITEM_type);
   QC_QGraphicsItem->addMethod("unsetCursor",                 (q_method_t)QGRAPHICSITEM_unsetCursor);
   QC_QGraphicsItem->addMethod("update",                      (q_method_t)QGRAPHICSITEM_update);
   QC_QGraphicsItem->addMethod("x",                           (q_method_t)QGRAPHICSITEM_x);
   QC_QGraphicsItem->addMethod("y",                           (q_method_t)QGRAPHICSITEM_y);
   QC_QGraphicsItem->addMethod("zValue",                      (q_method_t)QGRAPHICSITEM_zValue);

   // private methods
   QC_QGraphicsItem->addMethod("contextMenuEvent",            (q_method_t)QGRAPHICSITEM_contextMenuEvent, true);
   QC_QGraphicsItem->addMethod("dragEnterEvent",              (q_method_t)QGRAPHICSITEM_dragEnterEvent, true);
   QC_QGraphicsItem->addMethod("dragLeaveEvent",              (q_method_t)QGRAPHICSITEM_dragLeaveEvent, true);
   QC_QGraphicsItem->addMethod("dragMoveEvent",               (q_method_t)QGRAPHICSITEM_dragMoveEvent, true);
   QC_QGraphicsItem->addMethod("dropEvent",                   (q_method_t)QGRAPHICSITEM_dropEvent, true);
   QC_QGraphicsItem->addMethod("focusInEvent",                (q_method_t)QGRAPHICSITEM_focusInEvent, true);
   QC_QGraphicsItem->addMethod("focusOutEvent",               (q_method_t)QGRAPHICSITEM_focusOutEvent, true);
   QC_QGraphicsItem->addMethod("hoverEnterEvent",             (q_method_t)QGRAPHICSITEM_hoverEnterEvent, true);
   QC_QGraphicsItem->addMethod("hoverLeaveEvent",             (q_method_t)QGRAPHICSITEM_hoverLeaveEvent, true);
   QC_QGraphicsItem->addMethod("hoverMoveEvent",              (q_method_t)QGRAPHICSITEM_hoverMoveEvent, true);
   QC_QGraphicsItem->addMethod("inputMethodEvent",            (q_method_t)QGRAPHICSITEM_inputMethodEvent, true);
   QC_QGraphicsItem->addMethod("inputMethodQuery",            (q_method_t)QGRAPHICSITEM_inputMethodQuery, true);
   QC_QGraphicsItem->addMethod("itemChange",                  (q_method_t)QGRAPHICSITEM_itemChange, true);
   QC_QGraphicsItem->addMethod("keyPressEvent",               (q_method_t)QGRAPHICSITEM_keyPressEvent, true);
   QC_QGraphicsItem->addMethod("keyReleaseEvent",             (q_method_t)QGRAPHICSITEM_keyReleaseEvent, true);
   QC_QGraphicsItem->addMethod("mouseDoubleClickEvent",       (q_method_t)QGRAPHICSITEM_mouseDoubleClickEvent, true);
   QC_QGraphicsItem->addMethod("mouseMoveEvent",              (q_method_t)QGRAPHICSITEM_mouseMoveEvent, true);
   QC_QGraphicsItem->addMethod("mousePressEvent",             (q_method_t)QGRAPHICSITEM_mousePressEvent, true);
   QC_QGraphicsItem->addMethod("mouseReleaseEvent",           (q_method_t)QGRAPHICSITEM_mouseReleaseEvent, true);
   QC_QGraphicsItem->addMethod("prepareGeometryChange",       (q_method_t)QGRAPHICSITEM_prepareGeometryChange, true);
   QC_QGraphicsItem->addMethod("sceneEvent",                  (q_method_t)QGRAPHICSITEM_sceneEvent, true);
   QC_QGraphicsItem->addMethod("sceneEventFilter",            (q_method_t)QGRAPHICSITEM_sceneEventFilter, true);
   QC_QGraphicsItem->addMethod("wheelEvent",                  (q_method_t)QGRAPHICSITEM_wheelEvent, true);

   return QC_QGraphicsItem;
}

QoreNamespace *initQGraphicsItemNS()
{
   QoreNamespace *ns = new QoreNamespace("QGraphicsItem");
   ns->addSystemClass(initQGraphicsItemClass());

   ns->addSystemClass(initQAbstractGraphicsShapeItemClass(QC_QGraphicsItem));
   ns->addSystemClass(initQGraphicsEllipseItemClass(QC_QAbstractGraphicsShapeItem));
   ns->addSystemClass(initQGraphicsPathItemClass(QC_QAbstractGraphicsShapeItem));
   ns->addSystemClass(initQGraphicsPolygonItemClass(QC_QAbstractGraphicsShapeItem));
   ns->addSystemClass(initQGraphicsRectItemClass(QC_QAbstractGraphicsShapeItem));
   ns->addSystemClass(initQGraphicsSimpleTextItemClass(QC_QAbstractGraphicsShapeItem));
   ns->addSystemClass(initQGraphicsItemGroupClass(QC_QGraphicsItem));
   ns->addSystemClass(initQGraphicsLineItemClass(QC_QGraphicsItem));
   ns->addInitialNamespace(initQGraphicsPixmapItemNS(QC_QGraphicsItem));

   // QGraphicsItemFlag enum
   ns->addConstant("ItemIsMovable",            new QoreBigIntNode(QGraphicsItem::ItemIsMovable));
   ns->addConstant("ItemIsSelectable",         new QoreBigIntNode(QGraphicsItem::ItemIsSelectable));
   ns->addConstant("ItemIsFocusable",          new QoreBigIntNode(QGraphicsItem::ItemIsFocusable));
   ns->addConstant("ItemClipsToShape",         new QoreBigIntNode(QGraphicsItem::ItemClipsToShape));
   ns->addConstant("ItemClipsChildrenToShape", new QoreBigIntNode(QGraphicsItem::ItemClipsChildrenToShape));
   ns->addConstant("ItemIgnoresTransformations", new QoreBigIntNode(QGraphicsItem::ItemIgnoresTransformations));

   // GraphicsItemChange enum
   ns->addConstant("ItemPositionChange",       new QoreBigIntNode(QGraphicsItem::ItemPositionChange));
   ns->addConstant("ItemMatrixChange",         new QoreBigIntNode(QGraphicsItem::ItemMatrixChange));
   ns->addConstant("ItemVisibleChange",        new QoreBigIntNode(QGraphicsItem::ItemVisibleChange));
   ns->addConstant("ItemEnabledChange",        new QoreBigIntNode(QGraphicsItem::ItemEnabledChange));
   ns->addConstant("ItemSelectedChange",       new QoreBigIntNode(QGraphicsItem::ItemSelectedChange));
   ns->addConstant("ItemParentChange",         new QoreBigIntNode(QGraphicsItem::ItemParentChange));
   ns->addConstant("ItemChildAddedChange",     new QoreBigIntNode(QGraphicsItem::ItemChildAddedChange));
   ns->addConstant("ItemChildRemovedChange",   new QoreBigIntNode(QGraphicsItem::ItemChildRemovedChange));
   ns->addConstant("ItemTransformChange",      new QoreBigIntNode(QGraphicsItem::ItemTransformChange));
   ns->addConstant("ItemPositionHasChanged",   new QoreBigIntNode(QGraphicsItem::ItemPositionHasChanged));
   ns->addConstant("ItemTransformHasChanged",  new QoreBigIntNode(QGraphicsItem::ItemTransformHasChanged));
   ns->addConstant("ItemSceneChange",          new QoreBigIntNode(QGraphicsItem::ItemSceneChange));

   return ns;
}

