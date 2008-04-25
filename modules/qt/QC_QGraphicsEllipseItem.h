/*
 QC_QGraphicsEllipseItem.h
 
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

#ifndef _QORE_QT_QC_QGRAPHICSELLIPSEITEM_H

#define _QORE_QT_QC_QGRAPHICSELLIPSEITEM_H

#include <QGraphicsEllipseItem>
#include "QoreAbstractQGraphicsEllipseItem.h"
#include "qore-qt-events.h"

DLLLOCAL extern qore_classid_t CID_QGRAPHICSELLIPSEITEM;
DLLLOCAL extern QoreClass *QC_QGraphicsEllipseItem;
DLLLOCAL QoreClass *initQGraphicsEllipseItemClass(QoreClass *);

class myQGraphicsEllipseItem : public QGraphicsEllipseItem, public QoreQGraphicsItemExtension
{
#define QOREQTYPE QGraphicsEllipseItem
#define MYQOREQTYPE myQGraphicsEllipseItem
#include "qore-qt-qgraphicsitem-methods.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQGraphicsEllipseItem(QoreObject *obj, QGraphicsItem* parent = 0) : QGraphicsEllipseItem(parent), QoreQGraphicsItemExtension(obj)
      {
      }
      DLLLOCAL myQGraphicsEllipseItem(QoreObject *obj, const QRectF& rect, QGraphicsItem* parent = 0) : QGraphicsEllipseItem(rect, parent), QoreQGraphicsItemExtension(obj)
      {
      }
      DLLLOCAL myQGraphicsEllipseItem(QoreObject *obj, qreal x, qreal y, qreal width, qreal height, QGraphicsItem* parent = 0) : QGraphicsEllipseItem(x, y, width, height, parent), QoreQGraphicsItemExtension(obj)
      {
      }
};

typedef QoreQGraphicsEllipseItemBase<myQGraphicsEllipseItem, QoreAbstractQGraphicsEllipseItem> QoreQGraphicsEllipseItemImpl;

class QoreQGraphicsEllipseItem : public QoreQGraphicsEllipseItemImpl
{
   public:
      DLLLOCAL QoreQGraphicsEllipseItem(QoreObject *obj, QGraphicsItem* parent = 0) : QoreQGraphicsEllipseItemImpl(new myQGraphicsEllipseItem(obj, parent))
      {
      }
      DLLLOCAL QoreQGraphicsEllipseItem(QoreObject *obj, const QRectF& rect, QGraphicsItem* parent = 0) : QoreQGraphicsEllipseItemImpl(new myQGraphicsEllipseItem(obj, rect, parent))
      {
      }
      DLLLOCAL QoreQGraphicsEllipseItem(QoreObject *obj, qreal x, qreal y, qreal width, qreal height, QGraphicsItem* parent = 0) : QoreQGraphicsEllipseItemImpl(new myQGraphicsEllipseItem(obj, x, y, width, height, parent))
      {
      }
};

typedef QoreQtQGraphicsEllipseItemBase<QGraphicsEllipseItem, QoreAbstractQGraphicsEllipseItem> QoreQtQGraphicsEllipseItemImpl;

class QoreQtQGraphicsEllipseItem : public QoreQtQGraphicsEllipseItemImpl
{
   public:
      DLLLOCAL QoreQtQGraphicsEllipseItem(QGraphicsEllipseItem *qgraphicsellipseitem, bool managed = false) : QoreQtQGraphicsEllipseItemImpl(qgraphicsellipseitem, managed)
      {
      }
};

#endif // _QORE_QT_QC_QGRAPHICSELLIPSEITEM_H
