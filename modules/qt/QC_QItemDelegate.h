/*
 QC_QItemDelegate.h
 
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

#ifndef _QORE_QT_QC_QITEMDELEGATE_H

#define _QORE_QT_QC_QITEMDELEGATE_H

#include <QItemDelegate>
#include "QoreAbstractQItemDelegate.h"
#include "qore-qt-events.h"

DLLLOCAL extern qore_classid_t CID_QITEMDELEGATE;
DLLLOCAL extern class QoreClass *QC_QItemDelegate;

DLLLOCAL class QoreClass *initQItemDelegateClass(QoreClass *);

class myQItemDelegate : public QItemDelegate, public QoreQAbstractItemDelegateExtension
{
#define QOREQTYPE QItemDelegate
#define MYQOREQTYPE myQItemDelegate
#include "qore-qt-metacode.h"
#include "qore-qt-qabstractitemdelegate-methods.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
   DLLLOCAL myQItemDelegate(QoreObject *obj, QObject* parent = 0) : QItemDelegate(parent), QoreQAbstractItemDelegateExtension(obj->getClass())
      {
         init(obj);
      }
};

class QoreQItemDelegate : public QoreAbstractQItemDelegate
{
   public:
      QPointer<myQItemDelegate> qobj;

      DLLLOCAL QoreQItemDelegate(QoreObject *obj, QObject* parent = 0) : qobj(new myQItemDelegate(obj, parent))
      {
      }
      DLLLOCAL virtual class QObject *getQObject() const
      {
         return static_cast<QObject *>(&(*qobj));
      }
      DLLLOCAL virtual class QAbstractItemDelegate *getQAbstractItemDelegate() const
      {
         return static_cast<QAbstractItemDelegate *>(&(*qobj));
      }
      DLLLOCAL virtual class QItemDelegate *getQItemDelegate() const
      {
         return static_cast<QItemDelegate *>(&(*qobj));
      }
      QORE_VIRTUAL_QABSTRACTITEMDELEGATE_METHODS
};

#endif // _QORE_QT_QC_QITEMDELEGATE_H
