/*
 QoreAbstractQGroupBox.h
 
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

#ifndef _QORE_QOREABSTRACTQGROUPBOX_H

#define _QORE_QOREABSTRACTQGROUPBOX_H

#include "QoreAbstractQWidget.h"

class QoreAbstractQGroupBox : public QoreAbstractQWidget
{
   public:
      DLLLOCAL virtual QGroupBox *getQGroupBox() const = 0;
};

template<typename T, typename V>
class QoreQGroupBoxBase : public QoreQWidgetBase<T, V>
{
   public:
      DLLLOCAL QoreQGroupBoxBase(T *qo) : QoreQWidgetBase<T, V>(qo)
      {
      }
      DLLLOCAL virtual QGroupBox *getQGroupBox() const
      {
         return &(*this->qobj);
      }
};

template<typename T, typename V>
class QoreQtQGroupBoxBase : public QoreQtQWidgetBase<T, V>
{
   public:
      DLLLOCAL QoreQtQGroupBoxBase(QoreObject *obj, T *qo) : QoreQtQWidgetBase<T, V>(obj, qo)
      {
      }

      DLLLOCAL virtual QGroupBox *getQGroupBox() const
      {
         return this->qobj;
      }
};

#endif
