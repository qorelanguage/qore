/*
 QoreAbstractQAbstractSpinBox.h
 
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

#ifndef _QORE_QOREABSTRACTQABSTRACTSPINBOX_H

#define _QORE_QOREABSTRACTQABSTRACTSPINBOX_H

#include "QoreAbstractQWidget.h"

class QoreAbstractQAbstractSpinBox : public QoreAbstractQWidget
{
   public:
      DLLLOCAL virtual class QAbstractSpinBox *getQAbstractSpinBox() const = 0;
};

template<typename T, typename V>
class QoreQAbstractSpinBoxBase : public QoreQWidgetBase<T, V>
{
   public:
      DLLLOCAL QoreQAbstractSpinBoxBase(T *qo) : QoreQWidgetBase<T, V>(qo)
      {
      }
      DLLLOCAL virtual QAbstractSpinBox *getQAbstractSpinBox() const
      {
         return &(*this->qobj);
      }
};

template<typename T, typename V>
class QoreQtQAbstractSpinBoxBase : public QoreQtQWidgetBase<T, V>
{
   public:
      DLLLOCAL QoreQtQAbstractSpinBoxBase(QoreObject *obj, T *qo) : QoreQtQWidgetBase<T, V>(obj, qo)
      {
      }

      DLLLOCAL virtual QAbstractSpinBox *getQAbstractSpinBox() const
      {
         return this->qobj;
      }
};

#endif
