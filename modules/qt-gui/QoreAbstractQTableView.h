/*
 QoreAbstractQTableView.h
 
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

#ifndef _QORE_QT_QOREABSTRACTQTABLEVIEW_H

#define _QORE_QT_QOREABSTRACTQTABLEVIEW_H

#include "QoreAbstractQAbstractItemView.h"

class QoreAbstractQTableView : public QoreAbstractQAbstractItemView
{
   public:
      DLLLOCAL virtual QTableView *getQTableView() const = 0;
};

template<typename T, typename V>
class QoreQTableViewBase : public QoreQAbstractItemViewBase<T, V>
{
   public:
      DLLLOCAL QoreQTableViewBase(T *qo) : QoreQAbstractItemViewBase<T, V>(qo)
      {
      }
      DLLLOCAL virtual QTableView *getQTableView() const
      {
         return &(*this->qobj);
      }
};

template<typename T, typename V>
class QoreQtQTableViewBase : public QoreQtQAbstractItemViewBase<T, V>
{
   public:
      DLLLOCAL QoreQtQTableViewBase(QoreObject *obj, T *qo) : QoreQtQAbstractItemViewBase<T, V>(obj, qo)
      {
      }

      DLLLOCAL virtual QTableView *getQTableView() const
      {
         return this->qobj;
      }
};

#endif  // _QORE_QT_QOREABSTRACTQTABLEVIEW_H
