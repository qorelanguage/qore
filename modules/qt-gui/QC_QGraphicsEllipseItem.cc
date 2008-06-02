/*
 QC_QGraphicsEllipseItem.cc
 
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

#include "QC_QGraphicsEllipseItem.h"
#include "QC_QGraphicsItem.h"
#include "QC_QRectF.h"

qore_classid_t CID_QGRAPHICSELLIPSEITEM;
QoreClass *QC_QGraphicsEllipseItem = 0;

//QGraphicsEllipseItem ( QGraphicsItem * parent = 0 )
//QGraphicsEllipseItem ( const QRectF & rect, QGraphicsItem * parent = 0 )
//QGraphicsEllipseItem ( qreal x, qreal y, qreal width, qreal height, QGraphicsItem * parent = 0 )
static void QGRAPHICSELLIPSEITEM_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QGRAPHICSELLIPSEITEM, new QoreQGraphicsEllipseItem(self));
      return;
   }
   if (p && p->getType() == NT_OBJECT) {
      QoreQRectF *rect = (QoreQRectF *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QRECTF, xsink);
      if (!rect) {
         QoreQGraphicsItem *parent = (QoreQGraphicsItem *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QGRAPHICSITEM, xsink);
         if (*xsink)
            return;
         ReferenceHolder<AbstractPrivateData> parentHolder(parent, xsink);
         self->setPrivate(CID_QGRAPHICSELLIPSEITEM, new QoreQGraphicsEllipseItem(self, parent ? parent->getQGraphicsItem() : 0));
         return;
      }
      ReferenceHolder<AbstractPrivateData> rectHolder(static_cast<AbstractPrivateData *>(rect), xsink);
      p = get_param(params, 1);
      QoreQGraphicsItem *parent = (p && p->getType() == NT_OBJECT) ? (QoreQGraphicsItem *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QGRAPHICSITEM, xsink) : 0;
      if (*xsink)
         return;
      ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
      self->setPrivate(CID_QGRAPHICSELLIPSEITEM, new QoreQGraphicsEllipseItem(self, *(static_cast<QRectF *>(rect)), parent ? parent->getQGraphicsItem() : 0));
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
   self->setPrivate(CID_QGRAPHICSELLIPSEITEM, new QoreQGraphicsEllipseItem(self, x, y, width, height, parent ? parent->getQGraphicsItem() : 0));
   return;
}

static void QGRAPHICSELLIPSEITEM_copy(QoreObject *self, QoreObject *old, QoreQGraphicsEllipseItem *qgei, ExceptionSink *xsink)
{
   xsink->raiseException("QGRAPHICSELLIPSEITEM-COPY-ERROR", "objects of this class cannot be copied");
}

//QRectF rect () const
static AbstractQoreNode *QGRAPHICSELLIPSEITEM_rect(QoreObject *self, QoreAbstractQGraphicsEllipseItem *qgei, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QRectF, new QoreQRectF(qgei->getQGraphicsEllipseItem()->rect()));
}

//void setRect ( const QRectF & rect )
//void setRect ( qreal x, qreal y, qreal width, qreal height )
static AbstractQoreNode *QGRAPHICSELLIPSEITEM_setRect(QoreObject *self, QoreAbstractQGraphicsEllipseItem *qgei, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (p && p->getType() == NT_OBJECT) {
      QoreQRectF *rect = (QoreQRectF *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QRECTF, xsink);
      if (!rect) {
         if (!xsink->isException())
            xsink->raiseException("QGRAPHICSELLIPSEITEM-SETRECT-PARAM-ERROR", "QGraphicsEllipseItem::setRect() does not know how to handle arguments of class '%s' as passed as the first argument", reinterpret_cast<const QoreObject *>(p)->getClassName());
         return 0;
      }
      ReferenceHolder<AbstractPrivateData> rectHolder(static_cast<AbstractPrivateData *>(rect), xsink);
      qgei->getQGraphicsEllipseItem()->setRect(*(static_cast<QRectF *>(rect)));
      return 0;
   }
   qreal x = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 1);
   qreal y = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 2);
   qreal width = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 3);
   qreal height = p ? p->getAsFloat() : 0.0;
   qgei->getQGraphicsEllipseItem()->setRect(x, y, width, height);
   return 0;
}

//void setSpanAngle ( int angle )
static AbstractQoreNode *QGRAPHICSELLIPSEITEM_setSpanAngle(QoreObject *self, QoreAbstractQGraphicsEllipseItem *qgei, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int angle = p ? p->getAsInt() : 0;
   qgei->getQGraphicsEllipseItem()->setSpanAngle(angle);
   return 0;
}

//void setStartAngle ( int angle )
static AbstractQoreNode *QGRAPHICSELLIPSEITEM_setStartAngle(QoreObject *self, QoreAbstractQGraphicsEllipseItem *qgei, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int angle = p ? p->getAsInt() : 0;
   qgei->getQGraphicsEllipseItem()->setStartAngle(angle);
   return 0;
}

//int spanAngle () const
static AbstractQoreNode *QGRAPHICSELLIPSEITEM_spanAngle(QoreObject *self, QoreAbstractQGraphicsEllipseItem *qgei, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qgei->getQGraphicsEllipseItem()->spanAngle());
}

//int startAngle () const
static AbstractQoreNode *QGRAPHICSELLIPSEITEM_startAngle(QoreObject *self, QoreAbstractQGraphicsEllipseItem *qgei, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qgei->getQGraphicsEllipseItem()->startAngle());
}

QoreClass *initQGraphicsEllipseItemClass(QoreClass *qabstractgraphicsshapeitem)
{
   QC_QGraphicsEllipseItem = new QoreClass("QGraphicsEllipseItem", QDOM_GUI);
   CID_QGRAPHICSELLIPSEITEM = QC_QGraphicsEllipseItem->getID();

   QC_QGraphicsEllipseItem->addBuiltinVirtualBaseClass(qabstractgraphicsshapeitem);

   QC_QGraphicsEllipseItem->setConstructor(QGRAPHICSELLIPSEITEM_constructor);
   QC_QGraphicsEllipseItem->setCopy((q_copy_t)QGRAPHICSELLIPSEITEM_copy);

   QC_QGraphicsEllipseItem->addMethod("rect",                        (q_method_t)QGRAPHICSELLIPSEITEM_rect);
   QC_QGraphicsEllipseItem->addMethod("setRect",                     (q_method_t)QGRAPHICSELLIPSEITEM_setRect);
   QC_QGraphicsEllipseItem->addMethod("setSpanAngle",                (q_method_t)QGRAPHICSELLIPSEITEM_setSpanAngle);
   QC_QGraphicsEllipseItem->addMethod("setStartAngle",               (q_method_t)QGRAPHICSELLIPSEITEM_setStartAngle);
   QC_QGraphicsEllipseItem->addMethod("spanAngle",                   (q_method_t)QGRAPHICSELLIPSEITEM_spanAngle);
   QC_QGraphicsEllipseItem->addMethod("startAngle",                  (q_method_t)QGRAPHICSELLIPSEITEM_startAngle);

   return QC_QGraphicsEllipseItem;
}
