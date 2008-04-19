/*
 QC_QGraphicsPixmapItem.h
 
  Qore Programming Language

 Copyright (C) 2003 - 2008 David Nichols

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

#ifndef _QORE_QT_QC_QGRAPHICSPIXMAPITEM_H

#define _QORE_QT_QC_QGRAPHICSPIXMAPITEM_H

#include <QGraphicsPixmapItem>
#include "QoreAbstractQGraphicsPixmapItem.h"
#include "qore-qt-events.h"

DLLLOCAL extern qore_classid_t CID_QGRAPHICSPIXMAPITEM;
DLLLOCAL extern QoreClass *QC_QGraphicsPixmapItem;
DLLLOCAL QoreNamespace *initQGraphicsPixmapItemNS(QoreClass *);

class myQGraphicsPixmapItem : public QGraphicsPixmapItem, public QoreQGraphicsItemExtension
{
#define QOREQTYPE QGraphicsPixmapItem
#define MYQOREQTYPE myQGraphicsPixmapItem
#include "qore-qt-qgraphicsitem-methods.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQGraphicsPixmapItem(QoreObject *obj, QGraphicsItem* parent = 0) : QGraphicsPixmapItem(parent), QoreQGraphicsItemExtension(obj)
      {
      }
      DLLLOCAL myQGraphicsPixmapItem(QoreObject *obj, const QPixmap& pixmap, QGraphicsItem* parent = 0) : QGraphicsPixmapItem(pixmap, parent), QoreQGraphicsItemExtension(obj)
      {
      }
};

typedef QoreQGraphicsPixmapItemBase<myQGraphicsPixmapItem, QoreAbstractQGraphicsPixmapItem> QoreQGraphicsPixmapItemImpl;

class QoreQGraphicsPixmapItem : public QoreQGraphicsPixmapItemImpl
{
   public:
      DLLLOCAL QoreQGraphicsPixmapItem(QoreObject *obj, QGraphicsItem* parent = 0) : QoreQGraphicsPixmapItemImpl(new myQGraphicsPixmapItem(obj, parent))
      {
      }
      DLLLOCAL QoreQGraphicsPixmapItem(QoreObject *obj, const QPixmap& pixmap, QGraphicsItem* parent = 0) : QoreQGraphicsPixmapItemImpl(new myQGraphicsPixmapItem(obj, pixmap, parent))
      {
      }
};

typedef QoreQtQGraphicsPixmapItemBase<QGraphicsPixmapItem, QoreAbstractQGraphicsPixmapItem> QoreQtQGraphicsPixmapItemImpl;

class QoreQtQGraphicsPixmapItem : public QoreQtQGraphicsPixmapItemImpl
{
   public:
      DLLLOCAL QoreQtQGraphicsPixmapItem(QGraphicsPixmapItem *qgraphicspixmapitem, bool managed = false) : QoreQtQGraphicsPixmapItemImpl(qgraphicspixmapitem, managed)
      {
      }
};

#endif // _QORE_QT_QC_QGRAPHICSPIXMAPITEM_H
