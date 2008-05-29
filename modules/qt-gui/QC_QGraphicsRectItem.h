/*
 QC_QGraphicsRectItem.h
 
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

#ifndef _QORE_QT_QC_QGRAPHICSRECTITEM_H

#define _QORE_QT_QC_QGRAPHICSRECTITEM_H

#include <QGraphicsRectItem>
#include "QoreAbstractQGraphicsRectItem.h"
#include "qore-qt-events.h"

DLLEXPORT extern qore_classid_t CID_QGRAPHICSRECTITEM;
DLLEXPORT extern QoreClass *QC_QGraphicsRectItem;
DLLEXPORT QoreClass *initQGraphicsRectItemClass(QoreClass *);

class myQGraphicsRectItem : public QGraphicsRectItem, public QoreQGraphicsItemExtension
{
#define QOREQTYPE QGraphicsRectItem
#define MYQOREQTYPE myQGraphicsRectItem
#include "qore-qt-qgraphicsitem-methods.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQGraphicsRectItem(QoreObject *obj, QGraphicsItem* parent = 0) : QGraphicsRectItem(parent), QoreQGraphicsItemExtension(obj)
      {
      }
      DLLLOCAL myQGraphicsRectItem(QoreObject *obj, const QRectF& rect, QGraphicsItem* parent = 0) : QGraphicsRectItem(rect, parent), QoreQGraphicsItemExtension(obj)
      {
      }
      DLLLOCAL myQGraphicsRectItem(QoreObject *obj, qreal x, qreal y, qreal width, qreal height, QGraphicsItem* parent = 0) : QGraphicsRectItem(x, y, width, height, parent), QoreQGraphicsItemExtension(obj)
      {
      }
};

typedef QoreQGraphicsRectItemBase<myQGraphicsRectItem, QoreAbstractQGraphicsRectItem> QoreQGraphicsRectItemImpl;

class QoreQGraphicsRectItem : public QoreQGraphicsRectItemImpl
{
   public:
      DLLLOCAL QoreQGraphicsRectItem(QoreObject *obj, QGraphicsItem* parent = 0) : QoreQGraphicsRectItemImpl(new myQGraphicsRectItem(obj, parent))
      {
      }
      DLLLOCAL QoreQGraphicsRectItem(QoreObject *obj, const QRectF& rect, QGraphicsItem* parent = 0) : QoreQGraphicsRectItemImpl(new myQGraphicsRectItem(obj, rect, parent))
      {
      }
      DLLLOCAL QoreQGraphicsRectItem(QoreObject *obj, qreal x, qreal y, qreal width, qreal height, QGraphicsItem* parent = 0) : QoreQGraphicsRectItemImpl(new myQGraphicsRectItem(obj, x, y, width, height, parent))
      {
      }
};

typedef QoreQtQGraphicsRectItemBase<QGraphicsRectItem, QoreAbstractQGraphicsRectItem> QoreQtQGraphicsRectItemImpl;

class QoreQtQGraphicsRectItem : public QoreQtQGraphicsRectItemImpl
{
   public:
      DLLLOCAL QoreQtQGraphicsRectItem(QGraphicsRectItem *qgraphicsrectitem, bool managed = false) : QoreQtQGraphicsRectItemImpl(qgraphicsrectitem, managed)
      {
      }
};

#endif // _QORE_QT_QC_QGRAPHICSRECTITEM_H
