/*
 QoreAbstractQCoreApplication.h
 
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

#ifndef _QORE_QT_QOREABSTRACTQCOREAPPLICATION_H

#define _QORE_QT_QOREABSTRACTQCOREAPPLICATION_H

#include "QoreAbstractQObject.h"

class QoreAbstractQCoreApplication : public QoreAbstractQObject
{
   public:
      DLLLOCAL virtual class QCoreApplication *getQCoreApplication() const = 0;
};

template<typename T, typename V>
class QoreQCoreApplicationBase : public QoreQObjectBase<T, V>
{
   public:
      DLLLOCAL QoreQCoreApplicationBase(T *qo) : QoreQObjectBase<T, V>(qo)
      {
      }

      DLLLOCAL virtual QCoreApplication *getQCoreApplication() const
      {
	 return static_cast<QCoreApplication *>(&(*this->qobj));
      }
};

template<typename T, typename V>
class QoreQtQCoreApplicationBase : public QoreQtQObjectBase<T, V>
{
   public:
      DLLLOCAL QoreQtQCoreApplicationBase(QoreObject *obj, T *qo) : QoreQtQObjectBase<T, V>(obj, qo)
      {
      }

      DLLLOCAL virtual QCoreApplication *getQCoreApplication() const
      {
	 return static_cast<QCoreApplication *>(&(*this->qobj));
      }
};

#endif  // _QORE_QT_QOREABSTRACTQCOREAPPLICATION_H
