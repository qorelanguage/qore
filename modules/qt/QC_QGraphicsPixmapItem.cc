/*
 QC_QGraphicsPixmapItem.cc
 
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

#include "QC_QGraphicsPixmapItem.h"

qore_classid_t CID_QGRAPHICSPIXMAPITEM;
QoreClass *QC_QGraphicsPixmapItem = 0;

//QGraphicsPixmapItem ( QGraphicsItem * parent = 0 )
//QGraphicsPixmapItem ( const QPixmap & pixmap, QGraphicsItem * parent = 0 )
static void QGRAPHICSPIXMAPITEM_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QGRAPHICSPIXMAPITEM, new QoreQGraphicsPixmapItem(self));
      return;
   }
   if (p->getType() != NT_OBJECT) {
      xsink->raiseException("QGRAPHICSPIXMAPITEM-CONSTRUCTOR-ERROR", "expecting either NOTHING or an object derived from QPixmap or QGraphicsItem as the first argument to QGraphicsPixmapItem::constructor()");
      return;
   }
   QoreQPixmap *pixmap = (QoreQPixmap *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QPIXMAP, xsink);
   if (!pixmap) {
      QoreQGraphicsItem *parent = (QoreQGraphicsItem *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QGRAPHICSITEM, xsink);
      if (*xsink)
	 return;
      ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
      self->setPrivate(CID_QGRAPHICSPIXMAPITEM, new QoreQGraphicsPixmapItem(self, parent ? static_cast<QGraphicsItem *>(parent->getQGraphicsItem()) : 0));
      return;
   }
   ReferenceHolder<AbstractPrivateData> pixmapHolder(static_cast<AbstractPrivateData *>(pixmap), xsink);
   p = get_param(params, 1);
   QoreQGraphicsItem *parent = (p && p->getType() == NT_OBJECT) ? (QoreQGraphicsItem *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QGRAPHICSITEM, xsink) : 0;
   if (*xsink)
      return;
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   self->setPrivate(CID_QGRAPHICSPIXMAPITEM, new QoreQGraphicsPixmapItem(self, *(static_cast<QPixmap *>(pixmap)), parent ? static_cast<QGraphicsItem *>(parent->getQGraphicsItem()) : 0));
   return;
}

static void QGRAPHICSPIXMAPITEM_copy(QoreObject *self, QoreObject *old, QoreQGraphicsPixmapItem *qgpi, ExceptionSink *xsink)
{
   xsink->raiseException("QGRAPHICSPIXMAPITEM-COPY-ERROR", "objects of this class cannot be copied");
}

//QPointF offset () const
static AbstractQoreNode *QGRAPHICSPIXMAPITEM_offset(QoreObject *self, QoreAbstractQGraphicsPixmapItem *qgpi, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QPointF, new QoreQPointF(qgpi->getQGraphicsPixmapItem()->offset()));
}

//QPixmap pixmap () const
static AbstractQoreNode *QGRAPHICSPIXMAPITEM_pixmap(QoreObject *self, QoreAbstractQGraphicsPixmapItem *qgpi, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QPixmap, new QoreQPixmap(qgpi->getQGraphicsPixmapItem()->pixmap()));
}

//void setOffset ( const QPointF & offset )
//void setOffset ( qreal x, qreal y )
static AbstractQoreNode *QGRAPHICSPIXMAPITEM_setOffset(QoreObject *self, QoreAbstractQGraphicsPixmapItem *qgpi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (p && p->getType() == NT_OBJECT) {
      QoreQPointF *offset = (QoreQPointF *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QPOINTF, xsink);
      if (!offset) {
         if (!xsink->isException())
            xsink->raiseException("QGRAPHICSPIXMAPITEM-SETOFFSET-PARAM-ERROR", "QGraphicsPixmapItem::setOffset() does not know how to handle arguments of class '%s' as passed as the first argument", reinterpret_cast<const QoreObject *>(p)->getClassName());
         return 0;
      }
      ReferenceHolder<AbstractPrivateData> offsetHolder(static_cast<AbstractPrivateData *>(offset), xsink);
      qgpi->getQGraphicsPixmapItem()->setOffset(*(static_cast<QPointF *>(offset)));
      return 0;
   }
   qreal x = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 1);
   qreal y = p ? p->getAsFloat() : 0.0;
   qgpi->getQGraphicsPixmapItem()->setOffset(x, y);
   return 0;
}

//void setPixmap ( const QPixmap & pixmap )
static AbstractQoreNode *QGRAPHICSPIXMAPITEM_setPixmap(QoreObject *self, QoreAbstractQGraphicsPixmapItem *qgpi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQPixmap *pixmap = (p && p->getType() == NT_OBJECT) ? (QoreQPixmap *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QPIXMAP, xsink) : 0;
   if (!pixmap) {
      if (!xsink->isException())
         xsink->raiseException("QGRAPHICSPIXMAPITEM-SETPIXMAP-PARAM-ERROR", "expecting a QPixmap object as first argument to QGraphicsPixmapItem::setPixmap()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> pixmapHolder(static_cast<AbstractPrivateData *>(pixmap), xsink);
   qgpi->getQGraphicsPixmapItem()->setPixmap(*(static_cast<QPixmap *>(pixmap)));
   return 0;
}

//void setShapeMode ( ShapeMode mode )
static AbstractQoreNode *QGRAPHICSPIXMAPITEM_setShapeMode(QoreObject *self, QoreAbstractQGraphicsPixmapItem *qgpi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QGraphicsPixmapItem::ShapeMode mode = (QGraphicsPixmapItem::ShapeMode)(p ? p->getAsInt() : 0);
   qgpi->getQGraphicsPixmapItem()->setShapeMode(mode);
   return 0;
}

//void setTransformationMode ( Qt::TransformationMode mode )
static AbstractQoreNode *QGRAPHICSPIXMAPITEM_setTransformationMode(QoreObject *self, QoreAbstractQGraphicsPixmapItem *qgpi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   Qt::TransformationMode mode = (Qt::TransformationMode)(p ? p->getAsInt() : 0);
   qgpi->getQGraphicsPixmapItem()->setTransformationMode(mode);
   return 0;
}

//ShapeMode shapeMode () const
static AbstractQoreNode *QGRAPHICSPIXMAPITEM_shapeMode(QoreObject *self, QoreAbstractQGraphicsPixmapItem *qgpi, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qgpi->getQGraphicsPixmapItem()->shapeMode());
}

//Qt::TransformationMode transformationMode () const
static AbstractQoreNode *QGRAPHICSPIXMAPITEM_transformationMode(QoreObject *self, QoreAbstractQGraphicsPixmapItem *qgpi, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qgpi->getQGraphicsPixmapItem()->transformationMode());
}

static QoreClass *initQGraphicsPixmapItemClass(QoreClass *qgraphicsitem)
{
   QC_QGraphicsPixmapItem = new QoreClass("QGraphicsPixmapItem", QDOM_GUI);
   CID_QGRAPHICSPIXMAPITEM = QC_QGraphicsPixmapItem->getID();

   QC_QGraphicsPixmapItem->addBuiltinVirtualBaseClass(qgraphicsitem);

   QC_QGraphicsPixmapItem->setConstructor(QGRAPHICSPIXMAPITEM_constructor);
   QC_QGraphicsPixmapItem->setCopy((q_copy_t)QGRAPHICSPIXMAPITEM_copy);

   QC_QGraphicsPixmapItem->addMethod("offset",                      (q_method_t)QGRAPHICSPIXMAPITEM_offset);
   QC_QGraphicsPixmapItem->addMethod("pixmap",                      (q_method_t)QGRAPHICSPIXMAPITEM_pixmap);
   QC_QGraphicsPixmapItem->addMethod("setOffset",                   (q_method_t)QGRAPHICSPIXMAPITEM_setOffset);
   QC_QGraphicsPixmapItem->addMethod("setPixmap",                   (q_method_t)QGRAPHICSPIXMAPITEM_setPixmap);
   QC_QGraphicsPixmapItem->addMethod("setShapeMode",                (q_method_t)QGRAPHICSPIXMAPITEM_setShapeMode);
   QC_QGraphicsPixmapItem->addMethod("setTransformationMode",       (q_method_t)QGRAPHICSPIXMAPITEM_setTransformationMode);
   QC_QGraphicsPixmapItem->addMethod("shapeMode",                   (q_method_t)QGRAPHICSPIXMAPITEM_shapeMode);
   QC_QGraphicsPixmapItem->addMethod("transformationMode",          (q_method_t)QGRAPHICSPIXMAPITEM_transformationMode);

   return QC_QGraphicsPixmapItem;
}

QoreNamespace *initQGraphicsPixmapItemNS(QoreClass *qgraphicsitem)
{
   QoreNamespace *ns = new QoreNamespace("QGraphicsPixmapItem");
   ns->addSystemClass(initQGraphicsPixmapItemClass(QC_QGraphicsItem));

   // ShapeMode enum
   ns->addConstant("MaskShape",                new QoreBigIntNode(QGraphicsPixmapItem::MaskShape));
   ns->addConstant("BoundingRectShape",        new QoreBigIntNode(QGraphicsPixmapItem::BoundingRectShape));
   ns->addConstant("HeuristicMaskShape",       new QoreBigIntNode(QGraphicsPixmapItem::HeuristicMaskShape));

   return ns;
}
