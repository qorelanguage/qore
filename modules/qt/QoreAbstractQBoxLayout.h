/*
 QoreAbstractQBoxLayout.h
 
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

#ifndef _QORE_QOREABSTRACTQBOXLAYOUT_H

#define _QORE_QOREABSTRACTQBOXLAYOUT_H

#include "QoreAbstractQLayout.h"

extern qore_classid_t CID_QWIDGET;

class QoreAbstractQBoxLayout : public QoreAbstractQLayout
{
   public:
      DLLLOCAL virtual class QBoxLayout *getQBoxLayout() const = 0;
};

template<typename T, typename V>
class QoreQBoxLayoutBase : public QoreQLayoutBase<T, V>
{
   public:
      DLLLOCAL QoreQBoxLayoutBase(T *qo) : QoreQLayoutBase<T, V>(qo)
      {
      }
      DLLLOCAL virtual QBoxLayout *getQBoxLayout() const
      {
         return &(*this->qobj);
      }
};

template<typename T, typename V>
class QoreQtQBoxLayoutBase : public QoreQtQLayoutBase<T, V>
{
   public:
      DLLLOCAL QoreQtQBoxLayoutBase(QoreObject *obj, T *qo) : QoreQtQLayoutBase<T, V>(obj, qo)
      {
      }
      DLLLOCAL virtual QBoxLayout *getQBoxLayout() const
      {
         return this->qobj;
      }
};

#endif
