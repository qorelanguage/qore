/*
 QC_QGraphicsSimpleTextItem.h
 
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

#ifndef _QORE_QT_QC_QGRAPHICSSIMPLETEXTITEM_H

#define _QORE_QT_QC_QGRAPHICSSIMPLETEXTITEM_H

#include <QGraphicsSimpleTextItem>
#include "QoreAbstractQGraphicsSimpleTextItem.h"
#include "qore-qt-events.h"

DLLLOCAL extern qore_classid_t CID_QGRAPHICSSIMPLETEXTITEM;
DLLLOCAL extern QoreClass *QC_QGraphicsSimpleTextItem;
DLLLOCAL QoreClass *initQGraphicsSimpleTextItemClass(QoreClass *);

class myQGraphicsSimpleTextItem : public QGraphicsSimpleTextItem, public QoreQGraphicsItemExtension
{
#define QOREQTYPE QGraphicsSimpleTextItem
#define MYQOREQTYPE myQGraphicsSimpleTextItem
#include "qore-qt-qgraphicsitem-methods.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQGraphicsSimpleTextItem(QoreObject *obj, QGraphicsItem* parent = 0) : QGraphicsSimpleTextItem(parent), QoreQGraphicsItemExtension(obj)
      {
      }
      DLLLOCAL myQGraphicsSimpleTextItem(QoreObject *obj, const QString& text, QGraphicsItem* parent = 0) : QGraphicsSimpleTextItem(text, parent), QoreQGraphicsItemExtension(obj)
      {
      }
};

typedef QoreQGraphicsSimpleTextItemBase<myQGraphicsSimpleTextItem, QoreAbstractQGraphicsSimpleTextItem> QoreQGraphicsSimpleTextItemImpl;

class QoreQGraphicsSimpleTextItem : public QoreQGraphicsSimpleTextItemImpl
{
   public:
      DLLLOCAL QoreQGraphicsSimpleTextItem(QoreObject *obj, QGraphicsItem* parent = 0) : QoreQGraphicsSimpleTextItemImpl(new myQGraphicsSimpleTextItem(obj, parent))
      {
      }
      DLLLOCAL QoreQGraphicsSimpleTextItem(QoreObject *obj, const QString& text, QGraphicsItem* parent = 0) : QoreQGraphicsSimpleTextItemImpl(new myQGraphicsSimpleTextItem(obj, text, parent))
      {
      }
};

typedef QoreQtQGraphicsSimpleTextItemBase<QGraphicsSimpleTextItem, QoreAbstractQGraphicsSimpleTextItem> QoreQtQGraphicsSimpleTextItemImpl;

class QoreQtQGraphicsSimpleTextItem : public QoreQtQGraphicsSimpleTextItemImpl
{
   public:
      DLLLOCAL QoreQtQGraphicsSimpleTextItem(QGraphicsSimpleTextItem *qgraphicssimpletextitem, bool managed = false) : QoreQtQGraphicsSimpleTextItemImpl(qgraphicssimpletextitem, managed)
      {
      }
};

#endif // _QORE_QT_QC_QGRAPHICSSIMPLETEXTITEM_H
