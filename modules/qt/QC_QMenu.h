/*
 QC_QMenu.h
 
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

#ifndef _QORE_QT_QC_QMENU_H

#define _QORE_QT_QC_QMENU_H

#include <QMenu>
#include "QoreAbstractQMenu.h"
#include "qore-qt-events.h"

DLLLOCAL extern qore_classid_t CID_QMENU;
DLLLOCAL extern QoreClass *QC_QMenu;

DLLLOCAL QoreClass *initQMenuClass(QoreClass *);

class myQMenu : public QMenu, public QoreQWidgetExtension
{
#define QOREQTYPE QMenu
#define MYQOREQTYPE myQMenu
#include "qore-qt-metacode.h"
#include "qore-qt-widget-events.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQMenu(QoreObject *obj, QWidget* parent = 0) : QMenu(parent), QoreQWidgetExtension(obj, this)
      {
      }
      DLLLOCAL myQMenu(QoreObject *obj, const QString& title, QWidget* parent = 0) : QMenu(title, parent), QoreQWidgetExtension(obj, this)
      {
      }

      DLLLOCAL int parent_columnCount () const
      {
	 return columnCount();
      }
      DLLLOCAL void parent_initStyleOption ( QStyleOptionMenuItem * option, const QAction * action ) const
      {
	 initStyleOption(option, action);
      }

};

typedef QoreQMenuBase<myQMenu, QoreAbstractQMenu> QoreQMenuImpl;

class QoreQMenu : public QoreQMenuImpl
{
   public:
      DLLLOCAL QoreQMenu(QoreObject *obj, QWidget* parent = 0) : QoreQMenuImpl(new myQMenu(obj, parent))
      {
      }
      DLLLOCAL QoreQMenu(QoreObject *obj, const QString& title, QWidget* parent = 0) : QoreQMenuImpl(new myQMenu(obj, title, parent))
      {
      }
};

typedef QoreQtQMenuBase<QMenu, QoreAbstractQMenu> QoreQtQMenuImpl;

class QoreQtQMenu : public QoreQtQMenuImpl
{
   public:
      DLLLOCAL QoreQtQMenu(QoreObject *obj, QMenu *qm) : QoreQtQMenuImpl(obj, qm)
      {
      }
};

#endif // _QORE_QT_QC_QMENU_H
