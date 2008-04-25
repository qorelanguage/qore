/*
 QC_QGraphicsPolygonItem.cc
 
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

#include "QC_QGraphicsPolygonItem.h"

qore_classid_t CID_QGRAPHICSPOLYGONITEM;
QoreClass *QC_QGraphicsPolygonItem = 0;

//QGraphicsPolygonItem ( QGraphicsItem * parent = 0 )
//QGraphicsPolygonItem ( const QPolygonF & polygon, QGraphicsItem * parent = 0 )
static void QGRAPHICSPOLYGONITEM_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QGRAPHICSPOLYGONITEM, new QoreQGraphicsPolygonItem(self));
      return;
   }
   if (p->getType() != NT_OBJECT) {
      xsink->raiseException("QGRAPHICSPOLYGONITEM-ERROR", "expecting either NOTHING or an object derived from QGraphicsItem or QPolygonF as the first argument of QGraphicsPolygonItem::constructor()");
      return;
   }
   QoreQPolygonF *polygon = (QoreQPolygonF *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QPOLYGONF, xsink);
   if (!polygon) {
      QoreQGraphicsItem *parent = (QoreQGraphicsItem *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QGRAPHICSITEM, xsink);
      if (*xsink)
	 return;
      ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
      self->setPrivate(CID_QGRAPHICSPOLYGONITEM, new QoreQGraphicsPolygonItem(self, parent ? static_cast<QGraphicsItem *>(parent->getQGraphicsItem()) : 0));
      return;
   }
   ReferenceHolder<AbstractPrivateData> polygonHolder(static_cast<AbstractPrivateData *>(polygon), xsink);
   p = get_param(params, 1);
   QoreQGraphicsItem *parent = (p && p->getType() == NT_OBJECT) ? (QoreQGraphicsItem *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QGRAPHICSITEM, xsink) : 0;
   if (*xsink)
      return;
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   self->setPrivate(CID_QGRAPHICSPOLYGONITEM, new QoreQGraphicsPolygonItem(self, *(static_cast<QPolygonF *>(polygon)), parent ? static_cast<QGraphicsItem *>(parent->getQGraphicsItem()) : 0));
   return;
}

static void QGRAPHICSPOLYGONITEM_copy(QoreObject *self, QoreObject *old, QoreQGraphicsPolygonItem *qgpi, ExceptionSink *xsink)
{
   xsink->raiseException("QGRAPHICSPOLYGONITEM-COPY-ERROR", "objects of this class cannot be copied");
}

//Qt::FillRule fillRule () const
static AbstractQoreNode *QGRAPHICSPOLYGONITEM_fillRule(QoreObject *self, QoreAbstractQGraphicsPolygonItem *qgpi, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qgpi->getQGraphicsPolygonItem()->fillRule());
}

//QPolygonF polygon () const
static AbstractQoreNode *QGRAPHICSPOLYGONITEM_polygon(QoreObject *self, QoreAbstractQGraphicsPolygonItem *qgpi, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QPolygonF, new QoreQPolygonF(qgpi->getQGraphicsPolygonItem()->polygon()));
}

//void setFillRule ( Qt::FillRule rule )
static AbstractQoreNode *QGRAPHICSPOLYGONITEM_setFillRule(QoreObject *self, QoreAbstractQGraphicsPolygonItem *qgpi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   Qt::FillRule rule = (Qt::FillRule)(p ? p->getAsInt() : 0);
   qgpi->getQGraphicsPolygonItem()->setFillRule(rule);
   return 0;
}

//void setPolygon ( const QPolygonF & polygon )
static AbstractQoreNode *QGRAPHICSPOLYGONITEM_setPolygon(QoreObject *self, QoreAbstractQGraphicsPolygonItem *qgpi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQPolygonF *polygon = (p && p->getType() == NT_OBJECT) ? (QoreQPolygonF *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QPOLYGONF, xsink) : 0;
   if (!polygon) {
      if (!xsink->isException())
         xsink->raiseException("QGRAPHICSPOLYGONITEM-SETPOLYGON-PARAM-ERROR", "expecting a QPolygonF object as first argument to QGraphicsPolygonItem::setPolygon()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> polygonHolder(static_cast<AbstractPrivateData *>(polygon), xsink);
   qgpi->getQGraphicsPolygonItem()->setPolygon(*(static_cast<QPolygonF *>(polygon)));
   return 0;
}

QoreClass *initQGraphicsPolygonItemClass(QoreClass *qabstractgraphicsshapeitem)
{
   QC_QGraphicsPolygonItem = new QoreClass("QGraphicsPolygonItem", QDOM_GUI);
   CID_QGRAPHICSPOLYGONITEM = QC_QGraphicsPolygonItem->getID();

   QC_QGraphicsPolygonItem->addBuiltinVirtualBaseClass(qabstractgraphicsshapeitem);

   QC_QGraphicsPolygonItem->setConstructor(QGRAPHICSPOLYGONITEM_constructor);
   QC_QGraphicsPolygonItem->setCopy((q_copy_t)QGRAPHICSPOLYGONITEM_copy);

   QC_QGraphicsPolygonItem->addMethod("fillRule",                    (q_method_t)QGRAPHICSPOLYGONITEM_fillRule);
   QC_QGraphicsPolygonItem->addMethod("polygon",                     (q_method_t)QGRAPHICSPOLYGONITEM_polygon);
   QC_QGraphicsPolygonItem->addMethod("setFillRule",                 (q_method_t)QGRAPHICSPOLYGONITEM_setFillRule);
   QC_QGraphicsPolygonItem->addMethod("setPolygon",                  (q_method_t)QGRAPHICSPOLYGONITEM_setPolygon);

   return QC_QGraphicsPolygonItem;
}
