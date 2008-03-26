/*
 QoreAbstractQAbstractSlider.h
 
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

#ifndef _QORE_QOREABSTRACTQABSTRACTSLIDER_H

#define _QORE_QOREABSTRACTQABSTRACTSLIDER_H

#include "QoreAbstractQWidget.h"

class QoreAbstractQAbstractSlider : public QoreAbstractQWidget
{
   public:
      DLLLOCAL virtual class QAbstractSlider *getQAbstractSlider() const = 0;
};

template<typename T, typename V>
class QoreQAbstractSliderBase : public QoreQWidgetBase<T, V>
{
   public:
      DLLLOCAL QoreQAbstractSliderBase(T *qo) : QoreQWidgetBase<T, V>(qo)
      {
      }

      DLLLOCAL virtual QAbstractSlider *getQAbstractSlider() const
      {
	 return &(*this->qobj);
      }
};

template<typename T, typename V>
class QoreQtQAbstractSliderBase : public QoreQtQWidgetBase<T, V>
{
   public:
      DLLLOCAL QoreQtQAbstractSliderBase(QoreObject *obj, T *qo) : QoreQtQWidgetBase<T, V>(obj, qo)
      {
      }

      DLLLOCAL virtual QAbstractSlider *getQAbstractSlider() const
      {
	 return this->qobj;
      }
};

#endif
