/*
 QC_QGraphicsPathItem.cc
 
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

#include "QC_QGraphicsPathItem.h"
#include "QC_QGraphicsItem.h"
#include "QC_QPainterPath.h"

qore_classid_t CID_QGRAPHICSPATHITEM;
QoreClass *QC_QGraphicsPathItem = 0;

//QGraphicsPathItem ( QGraphicsItem * parent = 0 )
//QGraphicsPathItem ( const QPainterPath & path, QGraphicsItem * parent = 0 )
static void QGRAPHICSPATHITEM_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QGRAPHICSPATHITEM, new QoreQGraphicsPathItem(self));
      return;
   }
   if (p->getType() != NT_OBJECT) {
      xsink->raiseException("QGRAPHICSPATHITEM-ERROR", "expecting either NOTHING or an object derived from QGraphicsItem or QPainterPath as the first argument of QGraphicsPathItem::constructor()");
      return;
   }
   QoreQPainterPath *path = (QoreQPainterPath *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QPAINTERPATH, xsink);
   if (!path) {
      QoreQGraphicsItem *parent = (QoreQGraphicsItem *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QGRAPHICSITEM, xsink);
      if (*xsink)
	 return;
      ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
      self->setPrivate(CID_QGRAPHICSPATHITEM, new QoreQGraphicsPathItem(self, parent ? static_cast<QGraphicsItem *>(parent->getQGraphicsItem()) : 0));
      return;
   }
   ReferenceHolder<AbstractPrivateData> pathHolder(static_cast<AbstractPrivateData *>(path), xsink);
   p = get_param(params, 1);
   QoreQGraphicsItem *parent = (p && p->getType() == NT_OBJECT) ? (QoreQGraphicsItem *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QGRAPHICSITEM, xsink) : 0;
   if (*xsink)
      return;
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   self->setPrivate(CID_QGRAPHICSPATHITEM, new QoreQGraphicsPathItem(self, *(static_cast<QPainterPath *>(path)), parent ? static_cast<QGraphicsItem *>(parent->getQGraphicsItem()) : 0));
   return;
}

static void QGRAPHICSPATHITEM_copy(QoreObject *self, QoreObject *old, QoreQGraphicsPathItem *qgpi, ExceptionSink *xsink)
{
   xsink->raiseException("QGRAPHICSPATHITEM-COPY-ERROR", "objects of this class cannot be copied");
}

//QPainterPath path () const
static AbstractQoreNode *QGRAPHICSPATHITEM_path(QoreObject *self, QoreAbstractQGraphicsPathItem *qgpi, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QPainterPath, new QoreQPainterPath(qgpi->getQGraphicsPathItem()->path()));
}

//void setPath ( const QPainterPath & path )
static AbstractQoreNode *QGRAPHICSPATHITEM_setPath(QoreObject *self, QoreAbstractQGraphicsPathItem *qgpi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQPainterPath *path = (p && p->getType() == NT_OBJECT) ? (QoreQPainterPath *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QPAINTERPATH, xsink) : 0;
   if (!path) {
      if (!xsink->isException())
         xsink->raiseException("QGRAPHICSPATHITEM-SETPATH-PARAM-ERROR", "expecting a QPainterPath object as first argument to QGraphicsPathItem::setPath()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> pathHolder(static_cast<AbstractPrivateData *>(path), xsink);
   qgpi->getQGraphicsPathItem()->setPath(*(static_cast<QPainterPath *>(path)));
   return 0;
}

QoreClass *initQGraphicsPathItemClass(QoreClass *qabstractgraphicsshapeitem)
{
   QC_QGraphicsPathItem = new QoreClass("QGraphicsPathItem", QDOM_GUI);
   CID_QGRAPHICSPATHITEM = QC_QGraphicsPathItem->getID();

   QC_QGraphicsPathItem->addBuiltinVirtualBaseClass(qabstractgraphicsshapeitem);

   QC_QGraphicsPathItem->setConstructor(QGRAPHICSPATHITEM_constructor);
   QC_QGraphicsPathItem->setCopy((q_copy_t)QGRAPHICSPATHITEM_copy);

   QC_QGraphicsPathItem->addMethod("path",                        (q_method_t)QGRAPHICSPATHITEM_path);
   QC_QGraphicsPathItem->addMethod("setPath",                     (q_method_t)QGRAPHICSPATHITEM_setPath);

   return QC_QGraphicsPathItem;
}
