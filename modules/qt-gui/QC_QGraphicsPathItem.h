/*
 QC_QGraphicsPathItem.h
 
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

#ifndef _QORE_QT_QC_QGRAPHICSPATHITEM_H

#define _QORE_QT_QC_QGRAPHICSPATHITEM_H

#include <QGraphicsPathItem>
#include "QoreAbstractQGraphicsPathItem.h"
#include "qore-qt-events.h"

DLLEXPORT extern qore_classid_t CID_QGRAPHICSPATHITEM;
DLLEXPORT extern QoreClass *QC_QGraphicsPathItem;
DLLEXPORT QoreClass *initQGraphicsPathItemClass(QoreClass *);

class myQGraphicsPathItem : public QGraphicsPathItem, public QoreQGraphicsItemExtension
{
#define QOREQTYPE QGraphicsPathItem
#define MYQOREQTYPE myQGraphicsPathItem
#include "qore-qt-qgraphicsitem-methods.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQGraphicsPathItem(QoreObject *obj, QGraphicsItem* parent = 0) : QGraphicsPathItem(parent), QoreQGraphicsItemExtension(obj)
      {
      }
      DLLLOCAL myQGraphicsPathItem(QoreObject *obj, const QPainterPath& path, QGraphicsItem* parent = 0) : QGraphicsPathItem(path, parent), QoreQGraphicsItemExtension(obj)
      {
      }
};

typedef QoreQGraphicsPathItemBase<myQGraphicsPathItem, QoreAbstractQGraphicsPathItem> QoreQGraphicsPathItemImpl;

class QoreQGraphicsPathItem : public QoreQGraphicsPathItemImpl
{
   public:
      DLLLOCAL QoreQGraphicsPathItem(QoreObject *obj, QGraphicsItem* parent = 0) : QoreQGraphicsPathItemImpl(new myQGraphicsPathItem(obj, parent))
      {
      }
      DLLLOCAL QoreQGraphicsPathItem(QoreObject *obj, const QPainterPath& path, QGraphicsItem* parent = 0) : QoreQGraphicsPathItemImpl(new myQGraphicsPathItem(obj, path, parent))
      {
      }
};

typedef QoreQtQGraphicsPathItemBase<QGraphicsPathItem, QoreAbstractQGraphicsPathItem> QoreQtQGraphicsPathItemImpl;

class QoreQtQGraphicsPathItem : public QoreQtQGraphicsPathItemImpl
{
   public:
      DLLLOCAL QoreQtQGraphicsPathItem(QGraphicsPathItem *qgraphicspathitem, bool managed = false) : QoreQtQGraphicsPathItemImpl(qgraphicspathitem, managed)
      {
      }
};

#endif // _QORE_QT_QC_QGRAPHICSPATHITEM_H
