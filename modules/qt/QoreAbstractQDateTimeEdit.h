/*
 QoreAbstractQDateTimeEdit.h
 
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

#ifndef _QORE_QOREABSTRACTQDATETIMEEDIT_H

#define _QORE_QOREABSTRACTQDATETIMEEDIT_H

#include "QoreAbstractQAbstractSpinBox.h"

class QoreAbstractQDateTimeEdit : public QoreAbstractQAbstractSpinBox
{
   public:
      DLLLOCAL virtual class QDateTimeEdit *getQDateTimeEdit() const = 0;
};

template<typename T, typename V>
class QoreQDateTimeEditBase : public QoreQAbstractSpinBoxBase<T, V>
{
   public:
      DLLLOCAL QoreQDateTimeEditBase(T *qo) : QoreQAbstractSpinBoxBase<T, V>(qo)
      {
      }
      DLLLOCAL virtual QDateTimeEdit *getQDateTimeEdit() const
      {
         return &(*this->qobj);
      }
};

template<typename T, typename V>
class QoreQtQDateTimeEditBase : public QoreQtQAbstractSpinBoxBase<T, V>
{
   public:
      DLLLOCAL QoreQtQDateTimeEditBase(QoreObject *obj, T *qo) : QoreQtQAbstractSpinBoxBase<T, V>(obj, qo)
      {
      }

      DLLLOCAL virtual QDateTimeEdit *getQDateTimeEdit() const
      {
         return this->qobj;
      }
};

#endif
