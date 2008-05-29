/*
 QoreAbstractQItemDelegate.h
 
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

#ifndef _QORE_QOREABSTRACTQITEMDELEGATE_H

#define _QORE_QOREABSTRACTQITEMDELEGATE_H

#include "QoreAbstractQAbstractItemDelegate.h"

extern qore_classid_t CID_QWIDGET;

class QoreAbstractQItemDelegate : public QoreAbstractQAbstractItemDelegate
{
   public:
      DLLLOCAL virtual QAbstractItemDelegate *getQAbstractItemDelegate() const = 0;
      DLLLOCAL virtual QItemDelegate *getQItemDelegate() const = 0;
};

template<typename T, typename V>
class QoreQItemDelegateBase : public QoreQAbstractItemDelegateBase<T, V>
{
   public:
      DLLLOCAL QoreQItemDelegateBase(T *qo) : QoreQAbstractItemDelegateBase<T, V>(qo)
      {
      }
      DLLLOCAL virtual QAbstractItemDelegate *getQAbstractItemDelegate() const
      {
         return static_cast<QAbstractItemDelegate *>(&(*this->qobj));
      }
      DLLLOCAL virtual QItemDelegate *getQItemDelegate() const
      {
         return &(*this->qobj);
      }
};

template<typename T, typename V>
class QoreQtQItemDelegateBase : public QoreQtQAbstractItemDelegateBase<T, V>
{
   public:
      DLLLOCAL QoreQtQItemDelegateBase(QoreObject *obj, T *qo) : QoreQtQAbstractItemDelegateBase<T, V>(obj, qo)
      {
      }

      DLLLOCAL virtual QAbstractItemDelegate *getQAbstractItemDelegate() const
      {
         return static_cast<QAbstractItemDelegate *>(this->qobj);
      }

      DLLLOCAL virtual QItemDelegate *getQItemDelegate() const
      {
         return this->qobj;
      }
};

#endif
