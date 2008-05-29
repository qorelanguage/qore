/*
 QC_QGraphicsLineItem.cc
 
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

#include "QC_QGraphicsLineItem.h"
#include "QC_QGraphicsItem.h"
#include "QC_QLineF.h"
#include "QC_QPen.h"

qore_classid_t CID_QGRAPHICSLINEITEM;
QoreClass *QC_QGraphicsLineItem = 0;

//QGraphicsLineItem ( QGraphicsItem * parent = 0 )
//QGraphicsLineItem ( const QLineF & line, QGraphicsItem * parent = 0 )
//QGraphicsLineItem ( qreal x1, qreal y1, qreal x2, qreal y2, QGraphicsItem * parent = 0 )
static void QGRAPHICSLINEITEM_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QGRAPHICSLINEITEM, new QoreQGraphicsLineItem(self));
      return;
   }
   if (p && p->getType() == NT_OBJECT) {
      QoreQLineF *line = (QoreQLineF *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QLINEF, xsink);
      if (!line) {
         QoreQGraphicsItem *parent = (QoreQGraphicsItem *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QGRAPHICSITEM, xsink);
         if (*xsink)
            return;
         ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
         self->setPrivate(CID_QGRAPHICSLINEITEM, new QoreQGraphicsLineItem(self, parent ? static_cast<QGraphicsItem *>(parent->getQGraphicsItem()) : 0));
         return;
      }
      ReferenceHolder<AbstractPrivateData> lineHolder(static_cast<AbstractPrivateData *>(line), xsink);
      p = get_param(params, 1);
      QoreQGraphicsItem *parent = (p && p->getType() == NT_OBJECT) ? (QoreQGraphicsItem *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QGRAPHICSITEM, xsink) : 0;
      if (*xsink)
         return;
      ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
      self->setPrivate(CID_QGRAPHICSLINEITEM, new QoreQGraphicsLineItem(self, *(static_cast<QLineF *>(line)), parent ? static_cast<QGraphicsItem *>(parent->getQGraphicsItem()) : 0));
      return;
   }
   qreal x1 = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 1);
   qreal y1 = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 2);
   qreal x2 = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 3);
   qreal y2 = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 4);
   QoreQGraphicsItem *parent = (p && p->getType() == NT_OBJECT) ? (QoreQGraphicsItem *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QGRAPHICSITEM, xsink) : 0;
   if (*xsink)
      return;
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   self->setPrivate(CID_QGRAPHICSLINEITEM, new QoreQGraphicsLineItem(self, x1, y1, x2, y2, parent ? static_cast<QGraphicsItem *>(parent->getQGraphicsItem()) : 0));
   return;
}

static void QGRAPHICSLINEITEM_copy(QoreObject *self, QoreObject *old, QoreQGraphicsLineItem *qgli, ExceptionSink *xsink)
{
   xsink->raiseException("QGRAPHICSLINEITEM-COPY-ERROR", "objects of this class cannot be copied");
}

//QLineF line () const
static AbstractQoreNode *QGRAPHICSLINEITEM_line(QoreObject *self, QoreAbstractQGraphicsLineItem *qgli, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QLineF, new QoreQLineF(qgli->getQGraphicsLineItem()->line()));
}

//QPen pen () const
static AbstractQoreNode *QGRAPHICSLINEITEM_pen(QoreObject *self, QoreAbstractQGraphicsLineItem *qgli, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QPen, new QoreQPen(qgli->getQGraphicsLineItem()->pen()));
}

//void setLine ( const QLineF & line )
//void setLine ( qreal x1, qreal y1, qreal x2, qreal y2 )
static AbstractQoreNode *QGRAPHICSLINEITEM_setLine(QoreObject *self, QoreAbstractQGraphicsLineItem *qgli, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (p && p->getType() == NT_OBJECT) {
      QoreQLineF *line = (QoreQLineF *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QLINEF, xsink);
      if (!line) {
         if (!xsink->isException())
            xsink->raiseException("QGRAPHICSLINEITEM-SETLINE-PARAM-ERROR", "QGraphicsLineItem::setLine() does not know how to handle arguments of class '%s' as passed as the first argument", reinterpret_cast<const QoreObject *>(p)->getClassName());
         return 0;
      }
      ReferenceHolder<AbstractPrivateData> lineHolder(static_cast<AbstractPrivateData *>(line), xsink);
      qgli->getQGraphicsLineItem()->setLine(*(static_cast<QLineF *>(line)));
      return 0;
   }
   qreal x1 = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 1);
   qreal y1 = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 2);
   qreal x2 = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 3);
   qreal y2 = p ? p->getAsFloat() : 0.0;
   qgli->getQGraphicsLineItem()->setLine(x1, y1, x2, y2);
   return 0;
}

//void setPen ( const QPen & pen )
static AbstractQoreNode *QGRAPHICSLINEITEM_setPen(QoreObject *self, QoreAbstractQGraphicsLineItem *qgli, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQPen *pen = (p && p->getType() == NT_OBJECT) ? (QoreQPen *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QPEN, xsink) : 0;
   if (!pen) {
      if (!xsink->isException())
         xsink->raiseException("QGRAPHICSLINEITEM-SETPEN-PARAM-ERROR", "expecting a QPen object as first argument to QGraphicsLineItem::setPen()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> penHolder(static_cast<AbstractPrivateData *>(pen), xsink);
   qgli->getQGraphicsLineItem()->setPen(*(static_cast<QPen *>(pen)));
   return 0;
}

QoreClass *initQGraphicsLineItemClass(QoreClass *qgraphicsitem)
{
   QC_QGraphicsLineItem = new QoreClass("QGraphicsLineItem", QDOM_GUI);
   CID_QGRAPHICSLINEITEM = QC_QGraphicsLineItem->getID();

   QC_QGraphicsLineItem->addBuiltinVirtualBaseClass(qgraphicsitem);

   QC_QGraphicsLineItem->setConstructor(QGRAPHICSLINEITEM_constructor);
   QC_QGraphicsLineItem->setCopy((q_copy_t)QGRAPHICSLINEITEM_copy);

   QC_QGraphicsLineItem->addMethod("line",                        (q_method_t)QGRAPHICSLINEITEM_line);
   QC_QGraphicsLineItem->addMethod("pen",                         (q_method_t)QGRAPHICSLINEITEM_pen);
   QC_QGraphicsLineItem->addMethod("setLine",                     (q_method_t)QGRAPHICSLINEITEM_setLine);
   QC_QGraphicsLineItem->addMethod("setPen",                      (q_method_t)QGRAPHICSLINEITEM_setPen);

   return QC_QGraphicsLineItem;
}
