/*
 QoreAbstractQAction.h
 
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

#ifndef _QORE_QT_QOREABSTRACTQACTION_H

#define _QORE_QT_QOREABSTRACTQACTION_H

#include "QoreAbstractQObject.h"

class QoreAbstractQAction : public QoreAbstractQObject
{
   public:
      DLLLOCAL virtual QAction *getQAction() const = 0;
};

template<typename T, typename V>
class QoreQActionBase : public QoreQObjectBase<T, V>
{
   public:
      DLLLOCAL QoreQActionBase(T *qo) : QoreQObjectBase<T, V>(qo)
      {
      }
      DLLLOCAL virtual QAction *getQAction() const
      {
         return &(*this->qobj);
      }
};

template<typename T, typename V>
class QoreQtQActionBase : public QoreQtQObjectBase<T, V>
{
   public:
      DLLLOCAL QoreQtQActionBase(QoreObject *obj, T *qo) : QoreQtQObjectBase<T, V>(obj, qo)
      {
      }

      DLLLOCAL virtual QAction *getQAction() const
      {
         return this->qobj;
      }
};


#endif  // _QORE_QT_QOREABSTRACTQACTION_H
