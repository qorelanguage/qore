/*
 QC_QGraphicsLineItem.h
 
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

#ifndef _QORE_QT_QC_QGRAPHICSLINEITEM_H

#define _QORE_QT_QC_QGRAPHICSLINEITEM_H

#include <QGraphicsLineItem>
#include "QoreAbstractQGraphicsLineItem.h"
#include "qore-qt-events.h"

DLLEXPORT extern qore_classid_t CID_QGRAPHICSLINEITEM;
DLLEXPORT extern QoreClass *QC_QGraphicsLineItem;
DLLEXPORT QoreClass *initQGraphicsLineItemClass(QoreClass *);

class myQGraphicsLineItem : public QGraphicsLineItem, public QoreQGraphicsItemExtension
{
#define QOREQTYPE QGraphicsLineItem
#define MYQOREQTYPE myQGraphicsLineItem
#include "qore-qt-qgraphicsitem-methods.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQGraphicsLineItem(QoreObject *obj, QGraphicsItem* parent = 0) : QGraphicsLineItem(parent), QoreQGraphicsItemExtension(obj)
      {
      }
      DLLLOCAL myQGraphicsLineItem(QoreObject *obj, const QLineF& line, QGraphicsItem* parent = 0) : QGraphicsLineItem(line, parent), QoreQGraphicsItemExtension(obj)
      {
      }
      DLLLOCAL myQGraphicsLineItem(QoreObject *obj, qreal x1, qreal y1, qreal x2, qreal y2, QGraphicsItem* parent = 0) : QGraphicsLineItem(x1, y1, x2, y2, parent), QoreQGraphicsItemExtension(obj)
      {
      }
};

typedef QoreQGraphicsLineItemBase<myQGraphicsLineItem, QoreAbstractQGraphicsLineItem> QoreQGraphicsLineItemImpl;

class QoreQGraphicsLineItem : public QoreQGraphicsLineItemImpl
{
   public:
      DLLLOCAL QoreQGraphicsLineItem(QoreObject *obj, QGraphicsItem* parent = 0) : QoreQGraphicsLineItemImpl(new myQGraphicsLineItem(obj, parent))
      {
      }
      DLLLOCAL QoreQGraphicsLineItem(QoreObject *obj, const QLineF& line, QGraphicsItem* parent = 0) : QoreQGraphicsLineItemImpl(new myQGraphicsLineItem(obj, line, parent))
      {
      }
      DLLLOCAL QoreQGraphicsLineItem(QoreObject *obj, qreal x1, qreal y1, qreal x2, qreal y2, QGraphicsItem* parent = 0) : QoreQGraphicsLineItemImpl(new myQGraphicsLineItem(obj, x1, y1, x2, y2, parent))
      {
      }
};

typedef QoreQtQGraphicsLineItemBase<QGraphicsLineItem, QoreAbstractQGraphicsLineItem> QoreQtQGraphicsLineItemImpl;

class QoreQtQGraphicsLineItem : public QoreQtQGraphicsLineItemImpl
{
   public:
      DLLLOCAL QoreQtQGraphicsLineItem(QGraphicsLineItem *qgraphicslineitem, bool managed = false) : QoreQtQGraphicsLineItemImpl(qgraphicslineitem, managed)
      {
      }
};

#endif // _QORE_QT_QC_QGRAPHICSLINEITEM_H
