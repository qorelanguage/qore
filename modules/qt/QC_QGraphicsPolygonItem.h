/*
 QC_QGraphicsPolygonItem.h
 
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

#ifndef _QORE_QT_QC_QGRAPHICSPOLYGONITEM_H

#define _QORE_QT_QC_QGRAPHICSPOLYGONITEM_H

#include <QGraphicsPolygonItem>
#include "QoreAbstractQGraphicsPolygonItem.h"
#include "qore-qt-events.h"

DLLLOCAL extern qore_classid_t CID_QGRAPHICSPOLYGONITEM;
DLLLOCAL extern QoreClass *QC_QGraphicsPolygonItem;
DLLLOCAL QoreClass *initQGraphicsPolygonItemClass(QoreClass *);

class myQGraphicsPolygonItem : public QGraphicsPolygonItem, public QoreQGraphicsItemExtension
{
#define QOREQTYPE QGraphicsPolygonItem
#define MYQOREQTYPE myQGraphicsPolygonItem
#include "qore-qt-qgraphicsitem-methods.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQGraphicsPolygonItem(QoreObject *obj, QGraphicsItem* parent = 0) : QGraphicsPolygonItem(parent), QoreQGraphicsItemExtension(obj)
      {
      }
      DLLLOCAL myQGraphicsPolygonItem(QoreObject *obj, const QPolygonF& polygon, QGraphicsItem* parent = 0) : QGraphicsPolygonItem(polygon, parent), QoreQGraphicsItemExtension(obj)
      {
      }
};

typedef QoreQGraphicsPolygonItemBase<myQGraphicsPolygonItem, QoreAbstractQGraphicsPolygonItem> QoreQGraphicsPolygonItemImpl;

class QoreQGraphicsPolygonItem : public QoreQGraphicsPolygonItemImpl
{
   public:
      DLLLOCAL QoreQGraphicsPolygonItem(QoreObject *obj, QGraphicsItem* parent = 0) : QoreQGraphicsPolygonItemImpl(new myQGraphicsPolygonItem(obj, parent))
      {
      }
      DLLLOCAL QoreQGraphicsPolygonItem(QoreObject *obj, const QPolygonF& polygon, QGraphicsItem* parent = 0) : QoreQGraphicsPolygonItemImpl(new myQGraphicsPolygonItem(obj, polygon, parent))
      {
      }
};

typedef QoreQtQGraphicsPolygonItemBase<QGraphicsPolygonItem, QoreAbstractQGraphicsPolygonItem> QoreQtQGraphicsPolygonItemImpl;

class QoreQtQGraphicsPolygonItem : public QoreQtQGraphicsPolygonItemImpl
{
   public:
      DLLLOCAL QoreQtQGraphicsPolygonItem(QGraphicsPolygonItem *qgraphicspolygonitem, bool managed = false) : QoreQtQGraphicsPolygonItemImpl(qgraphicspolygonitem, managed)
      {
      }
};

#endif // _QORE_QT_QC_QGRAPHICSPOLYGONITEM_H
