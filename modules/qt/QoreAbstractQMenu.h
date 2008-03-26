/*
 QoreAbstractQMenu.h
 
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

#ifndef _QORE_QOREABSTRACTQMENU_H

#define _QORE_QOREABSTRACTQMENU_H

#include "QoreAbstractQWidget.h"

class QoreAbstractQMenu : public QoreAbstractQWidget
{
   public:
      DLLLOCAL virtual QMenu *getQMenu() const = 0;

      DLLLOCAL virtual int columnCount () const = 0;
      DLLLOCAL virtual void initStyleOption ( QStyleOptionMenuItem * option, const QAction * action ) const = 0;
};

template<typename T, typename V>
class QoreQMenuBase : public QoreQWidgetBase<T, V>
{
   public:
      DLLLOCAL QoreQMenuBase(T *qo) : QoreQWidgetBase<T, V>(qo)
      {
      }
      DLLLOCAL virtual QMenu *getQMenu() const
      {
         return &(*this->qobj);
      }
      DLLLOCAL virtual int columnCount () const
      {
         return this->qobj->parent_columnCount();
      }
      DLLLOCAL virtual void initStyleOption ( QStyleOptionMenuItem * option, const QAction * action ) const
      {
         this->qobj->parent_initStyleOption(option, action);
      }
};

template<typename T, typename V>
class QoreQtQMenuBase : public QoreQtQWidgetBase<T, V>
{
   public:
      DLLLOCAL QoreQtQMenuBase(QoreObject *obj, T *qo) : QoreQtQWidgetBase<T, V>(obj, qo)
      {
      }
      DLLLOCAL virtual QMenu *getQMenu() const
      {
         return this->qobj;
      }

      DLLLOCAL virtual int columnCount () const { return 0; }
      DLLLOCAL virtual void initStyleOption ( QStyleOptionMenuItem * option, const QAction * action ) const
      {
      }

};

#endif
