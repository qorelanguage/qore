/*
 QoreAbstractQCleanlooksStyle.h
 
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

#ifndef _QORE_QT_QOREABSTRACTQCLEANLOOKSSTYLE_H

#define _QORE_QT_QOREABSTRACTQCLEANLOOKSSTYLE_H

#include "QoreAbstractQWindowsStyle.h"

class QoreAbstractQCleanlooksStyle : public QoreAbstractQWindowsStyle
{
   public:
      DLLLOCAL virtual QCleanlooksStyle *getQCleanlooksStyle() const = 0;
};

template<typename T, typename V>
class QoreQCleanlooksStyleBase : public QoreQWindowsStyleBase<T, V>
{
   public:
      DLLLOCAL QoreQCleanlooksStyleBase(T *qo) : QoreQWindowsStyleBase<T, V>(qo)
      {
      }
      DLLLOCAL virtual QCleanlooksStyle *getQCleanlooksStyle() const
      {
         return &(*this->qobj);
      }
};

template<typename T, typename V>
class QoreQtQCleanlooksStyleBase : public QoreQtQWindowsStyleBase<T, V>
{
   public:
      DLLLOCAL QoreQtQCleanlooksStyleBase(QoreObject *obj, T *qo) : QoreQtQWindowsStyleBase<T, V>(obj, qo)
      {
      }

      DLLLOCAL virtual QCleanlooksStyle *getQCleanlooksStyle() const
      {
         return this->qobj;
      }
};

#endif  // _QORE_QT_QOREABSTRACTQCLEANLOOKSSTYLE_H
