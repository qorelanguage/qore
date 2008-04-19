/*
 QC_QAbstractGraphicsShapeItem.h
 
 Qore Programming Language
 
 Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols
 
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

#ifndef _QORE_QT_QC_QABSTRACTGRAPHICSSHAPEITEM_H

#define _QORE_QT_QC_QABSTRACTGRAPHICSSHAPEITEM_H

#include <QAbstractGraphicsShapeItem>
#include "QoreAbstractQAbstractGraphicsShapeItem.h"
#include "qore-qt-events.h"

DLLLOCAL extern qore_classid_t CID_QABSTRACTGRAPHICSSHAPEITEM;
DLLLOCAL extern QoreClass *QC_QAbstractGraphicsShapeItem;
DLLLOCAL QoreClass *initQAbstractGraphicsShapeItemClass(QoreClass *);

class myQAbstractGraphicsShapeItem : public QAbstractGraphicsShapeItem, public QoreQGraphicsItemExtension
{
#define QORE_IS_QGRAPHICSITEM
#define QOREQTYPE QAbstractGraphicsShapeItem
#define MYQOREQTYPE myQAbstractGraphicsShapeItem
#include "qore-qt-qgraphicsitem-methods.h"
#undef MYQOREQTYPE
#undef QOREQTYPE
#undef QORE_IS_QGRAPHICSITEM

   public:
      DLLLOCAL myQAbstractGraphicsShapeItem(QoreObject *obj, QGraphicsItem* parent = 0) : QAbstractGraphicsShapeItem(parent), QoreQGraphicsItemExtension(obj)
      {
      }
};

typedef QoreQAbstractGraphicsShapeItemBase<myQAbstractGraphicsShapeItem, QoreAbstractQAbstractGraphicsShapeItem> QoreQAbstractGraphicsShapeItemImpl;

class QoreQAbstractGraphicsShapeItem : public QoreQAbstractGraphicsShapeItemImpl
{
   public:
      DLLLOCAL QoreQAbstractGraphicsShapeItem(QoreObject *obj, QGraphicsItem* parent = 0) : QoreQAbstractGraphicsShapeItemImpl(new myQAbstractGraphicsShapeItem(obj, parent))
      {
      }
};

typedef QoreQtQAbstractGraphicsShapeItemBase<QAbstractGraphicsShapeItem, QoreAbstractQAbstractGraphicsShapeItem> QoreQtQAbstractGraphicsShapeItemImpl;

class QoreQtQAbstractGraphicsShapeItem : public QoreQtQAbstractGraphicsShapeItemImpl
{
   public:
   DLLLOCAL QoreQtQAbstractGraphicsShapeItem(QAbstractGraphicsShapeItem *qabstractgraphicsshapeitem, bool managed = false) : QoreQtQAbstractGraphicsShapeItemImpl(qabstractgraphicsshapeitem, managed)
      {
      }
};

#endif // _QORE_QT_QC_QABSTRACTGRAPHICSSHAPEITEM_H
