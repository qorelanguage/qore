/*
 QC_QGraphicsRectItem.cc
 
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

#include "QC_QGraphicsRectItem.h"

qore_classid_t CID_QGRAPHICSRECTITEM;
QoreClass *QC_QGraphicsRectItem = 0;

//QGraphicsRectItem ( QGraphicsItem * parent = 0 )
//QGraphicsRectItem ( const QRectF & rect, QGraphicsItem * parent = 0 )
//QGraphicsRectItem ( qreal x, qreal y, qreal width, qreal height, QGraphicsItem * parent = 0 )
static void QGRAPHICSRECTITEM_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QGRAPHICSRECTITEM, new QoreQGraphicsRectItem(self));
      return;
   }
   if (p && p->getType() == NT_OBJECT) {
      QoreQRectF *rect = (QoreQRectF *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QRECTF, xsink);
      if (!rect) {
         QoreQGraphicsItem *parent = (QoreQGraphicsItem *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QGRAPHICSITEM, xsink);
         if (*xsink)
            return;
         ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
         self->setPrivate(CID_QGRAPHICSRECTITEM, new QoreQGraphicsRectItem(self, parent ? static_cast<QGraphicsItem *>(parent->getQGraphicsItem()) : 0));
         return;
      }
      ReferenceHolder<AbstractPrivateData> rectHolder(static_cast<AbstractPrivateData *>(rect), xsink);
      p = get_param(params, 1);
      QoreQGraphicsItem *parent = (p && p->getType() == NT_OBJECT) ? (QoreQGraphicsItem *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QGRAPHICSITEM, xsink) : 0;
      if (*xsink)
         return;
      ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
      self->setPrivate(CID_QGRAPHICSRECTITEM, new QoreQGraphicsRectItem(self, *(static_cast<QRectF *>(rect)), parent ? static_cast<QGraphicsItem *>(parent->getQGraphicsItem()) : 0));
      return;
   }
   qreal x = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 1);
   qreal y = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 2);
   qreal width = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 3);
   qreal height = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 4);
   QoreQGraphicsItem *parent = (p && p->getType() == NT_OBJECT) ? (QoreQGraphicsItem *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QGRAPHICSITEM, xsink) : 0;
   if (*xsink)
      return;
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   self->setPrivate(CID_QGRAPHICSRECTITEM, new QoreQGraphicsRectItem(self, x, y, width, height, parent ? static_cast<QGraphicsItem *>(parent->getQGraphicsItem()) : 0));
   return;
}

static void QGRAPHICSRECTITEM_copy(QoreObject *self, QoreObject *old, QoreQGraphicsRectItem *qgri, ExceptionSink *xsink)
{
   xsink->raiseException("QGRAPHICSRECTITEM-COPY-ERROR", "objects of this class cannot be copied");
}

//QRectF rect () const
static AbstractQoreNode *QGRAPHICSRECTITEM_rect(QoreObject *self, QoreAbstractQGraphicsRectItem *qgri, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QRectF, new QoreQRectF(qgri->getQGraphicsRectItem()->rect()));
}

//void setRect ( const QRectF & rectangle )
//void setRect ( qreal x, qreal y, qreal width, qreal height )
static AbstractQoreNode *QGRAPHICSRECTITEM_setRect(QoreObject *self, QoreAbstractQGraphicsRectItem *qgri, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (p && p->getType() == NT_OBJECT) {
      QoreQRectF *rectangle = (QoreQRectF *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QRECTF, xsink);
      if (!rectangle) {
         if (!xsink->isException())
            xsink->raiseException("QGRAPHICSRECTITEM-SETRECT-PARAM-ERROR", "QGraphicsRectItem::setRect() does not know how to handle arguments of class '%s' as passed as the first argument", reinterpret_cast<const QoreObject *>(p)->getClassName());
         return 0;
      }
      ReferenceHolder<AbstractPrivateData> rectangleHolder(static_cast<AbstractPrivateData *>(rectangle), xsink);
      qgri->getQGraphicsRectItem()->setRect(*(static_cast<QRectF *>(rectangle)));
      return 0;
   }
   qreal x = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 1);
   qreal y = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 2);
   qreal width = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 3);
   qreal height = p ? p->getAsFloat() : 0.0;
   qgri->getQGraphicsRectItem()->setRect(x, y, width, height);
   return 0;
}

QoreClass *initQGraphicsRectItemClass(QoreClass *qabstractgraphicsshapeitem)
{
   QC_QGraphicsRectItem = new QoreClass("QGraphicsRectItem", QDOM_GUI);
   CID_QGRAPHICSRECTITEM = QC_QGraphicsRectItem->getID();

   QC_QGraphicsRectItem->addBuiltinVirtualBaseClass(qabstractgraphicsshapeitem);

   QC_QGraphicsRectItem->setConstructor(QGRAPHICSRECTITEM_constructor);
   QC_QGraphicsRectItem->setCopy((q_copy_t)QGRAPHICSRECTITEM_copy);

   QC_QGraphicsRectItem->addMethod("rect",                        (q_method_t)QGRAPHICSRECTITEM_rect);
   QC_QGraphicsRectItem->addMethod("setRect",                     (q_method_t)QGRAPHICSRECTITEM_setRect);

   return QC_QGraphicsRectItem;
}
