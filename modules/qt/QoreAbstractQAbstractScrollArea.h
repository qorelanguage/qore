/*
 QoreAbstractQScrollArea.h
 
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

#ifndef _QORE_QOREABSTRACTQSCROLLAREA_H

#define _QORE_QOREABSTRACTQSCROLLAREA_H

#include "QoreAbstractQFrame.h"

class QoreAbstractQAbstractScrollArea : public QoreAbstractQFrame
{
   public:
      DLLLOCAL virtual class QAbstractScrollArea *getQAbstractScrollArea() const = 0;
      DLLLOCAL virtual void setupViewport(QWidget *w) = 0;
};

template<typename T, typename V>
class QoreQAbstractScrollAreaBase : public QoreQFrameBase<T, V>
{
   public:
      DLLLOCAL QoreQAbstractScrollAreaBase(T *qo) : QoreQFrameBase<T, V>(qo)
      {
      }
      DLLLOCAL virtual QAbstractScrollArea *getQAbstractScrollArea() const
      {
         return &(*this->qobj);
      }

      DLLLOCAL virtual void setupViewport(QWidget *w)
      {
         this->qobj->pub_setupViewport(w);
      }
};

template<typename T, typename V>
class QoreQtQAbstractScrollAreaBase : public QoreQtQFrameBase<T, V>
{
   public:
      DLLLOCAL QoreQtQAbstractScrollAreaBase(QoreObject *obj, T *qo) : QoreQtQFrameBase<T, V>(obj, qo)
      {
      }

      DLLLOCAL virtual QAbstractScrollArea *getQAbstractScrollArea() const
      {
         return this->qobj;
      }

      DLLLOCAL virtual void setupViewport(QWidget *w)
      {
      }
};

#endif
