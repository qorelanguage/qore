/*
 QC_QGraphicsItemGroup.h
 
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

#ifndef _QORE_QT_QC_QGRAPHICSITEMGROUP_H

#define _QORE_QT_QC_QGRAPHICSITEMGROUP_H

#include <QGraphicsItemGroup>
#include "QoreAbstractQGraphicsItemGroup.h"
#include "qore-qt-events.h"

DLLEXPORT extern qore_classid_t CID_QGRAPHICSITEMGROUP;
DLLEXPORT extern QoreClass *QC_QGraphicsItemGroup;
DLLEXPORT QoreClass *initQGraphicsItemGroupClass(QoreClass *);

class myQGraphicsItemGroup : public QGraphicsItemGroup, public QoreQGraphicsItemExtension
{
#define QOREQTYPE QGraphicsItemGroup
#define MYQOREQTYPE myQGraphicsItemGroup
#include "qore-qt-qgraphicsitem-methods.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQGraphicsItemGroup(QoreObject *obj, QGraphicsItem* parent = 0) : QGraphicsItemGroup(parent), QoreQGraphicsItemExtension(obj)
      {
      }
};

typedef QoreQGraphicsItemGroupBase<myQGraphicsItemGroup, QoreAbstractQGraphicsItemGroup> QoreQGraphicsItemGroupImpl;

class QoreQGraphicsItemGroup : public QoreQGraphicsItemGroupImpl
{
   public:
      DLLLOCAL QoreQGraphicsItemGroup(QoreObject *obj, QGraphicsItem* parent = 0) : QoreQGraphicsItemGroupImpl(new myQGraphicsItemGroup(obj, parent))
      {
      }
};

typedef QoreQtQGraphicsItemGroupBase<QGraphicsItemGroup, QoreAbstractQGraphicsItemGroup> QoreQtQGraphicsItemGroupImpl;

class QoreQtQGraphicsItemGroup : public QoreQtQGraphicsItemGroupImpl
{
   public:
      DLLLOCAL QoreQtQGraphicsItemGroup(QGraphicsItemGroup *qgraphicsitemgroup, bool managed = false) : QoreQtQGraphicsItemGroupImpl(qgraphicsitemgroup, managed)
      {
      }
};

#endif // _QORE_QT_QC_QGRAPHICSITEMGROUP_H
