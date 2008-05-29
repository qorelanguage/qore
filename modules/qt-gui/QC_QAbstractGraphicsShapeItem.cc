/*
 QC_QAbstractGraphicsShapeItem.cc
 
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

#include "qore-qt.h"

#include "QC_QAbstractGraphicsShapeItem.h"
#include "QC_QGraphicsItem.h"
#include "QC_QBrush.h"
#include "QC_QPen.h"

qore_classid_t CID_QABSTRACTGRAPHICSSHAPEITEM;
QoreClass *QC_QAbstractGraphicsShapeItem = 0;

//QAbstractGraphicsShapeItem ( QGraphicsItem * parent = 0 )
static void QABSTRACTGRAPHICSSHAPEITEM_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQGraphicsItem *parent = (p && p->getType() == NT_OBJECT) ? (QoreQGraphicsItem *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QGRAPHICSITEM, xsink) : 0;
   if (*xsink)
      return;
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   self->setPrivate(CID_QABSTRACTGRAPHICSSHAPEITEM, new QoreQAbstractGraphicsShapeItem(self, parent ? parent->getQGraphicsItem() : 0));
   return;
}

static void QABSTRACTGRAPHICSSHAPEITEM_copy(QoreObject *self, QoreObject *old, QoreQAbstractGraphicsShapeItem *qagsi, ExceptionSink *xsink)
{
   xsink->raiseException("QABSTRACTGRAPHICSSHAPEITEM-COPY-ERROR", "objects of this class cannot be copied");
}

//QBrush brush () const
static AbstractQoreNode *QABSTRACTGRAPHICSSHAPEITEM_brush(QoreObject *self, QoreAbstractQAbstractGraphicsShapeItem *qagsi, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QBrush, new QoreQBrush(qagsi->getQAbstractGraphicsShapeItem()->brush()));
}

//QPen pen () const
static AbstractQoreNode *QABSTRACTGRAPHICSSHAPEITEM_pen(QoreObject *self, QoreAbstractQAbstractGraphicsShapeItem *qagsi, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QPen, new QoreQPen(qagsi->getQAbstractGraphicsShapeItem()->pen()));
}

//void setBrush ( const QBrush & brush )
static AbstractQoreNode *QABSTRACTGRAPHICSSHAPEITEM_setBrush(QoreObject *self, QoreAbstractQAbstractGraphicsShapeItem *qagsi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QBrush brush;
   if (get_qbrush(p, brush, xsink))
      return 0;
   qagsi->getQAbstractGraphicsShapeItem()->setBrush(brush);
   return 0;
}

//void setPen ( const QPen & pen )
static AbstractQoreNode *QABSTRACTGRAPHICSSHAPEITEM_setPen(QoreObject *self, QoreAbstractQAbstractGraphicsShapeItem *qagsi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQPen *pen = (p && p->getType() == NT_OBJECT) ? (QoreQPen *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QPEN, xsink) : 0;
   if (!pen) {
      if (!xsink->isException())
         xsink->raiseException("QABSTRACTGRAPHICSSHAPEITEM-SETPEN-PARAM-ERROR", "expecting a QPen object as first argument to QAbstractGraphicsShapeItem::setPen()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> penHolder(static_cast<AbstractPrivateData *>(pen), xsink);
   qagsi->getQAbstractGraphicsShapeItem()->setPen(*(static_cast<QPen *>(pen)));
   return 0;
}

QoreClass *initQAbstractGraphicsShapeItemClass(QoreClass *qgraphicsitem)
{
   QC_QAbstractGraphicsShapeItem = new QoreClass("QAbstractGraphicsShapeItem", QDOM_GUI);
   CID_QABSTRACTGRAPHICSSHAPEITEM = QC_QAbstractGraphicsShapeItem->getID();

   QC_QAbstractGraphicsShapeItem->addBuiltinVirtualBaseClass(qgraphicsitem);

   QC_QAbstractGraphicsShapeItem->setConstructor(QABSTRACTGRAPHICSSHAPEITEM_constructor);
   QC_QAbstractGraphicsShapeItem->setCopy((q_copy_t)QABSTRACTGRAPHICSSHAPEITEM_copy);

   QC_QAbstractGraphicsShapeItem->addMethod("brush",                       (q_method_t)QABSTRACTGRAPHICSSHAPEITEM_brush);
   QC_QAbstractGraphicsShapeItem->addMethod("pen",                         (q_method_t)QABSTRACTGRAPHICSSHAPEITEM_pen);
   QC_QAbstractGraphicsShapeItem->addMethod("setBrush",                    (q_method_t)QABSTRACTGRAPHICSSHAPEITEM_setBrush);
   QC_QAbstractGraphicsShapeItem->addMethod("setPen",                      (q_method_t)QABSTRACTGRAPHICSSHAPEITEM_setPen);

   return QC_QAbstractGraphicsShapeItem;
}
