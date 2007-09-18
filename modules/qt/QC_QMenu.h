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
#include "QoreAbstractQWidget.h"
#include "qore-qt-events.h"

DLLLOCAL extern int CID_QMENU;
DLLLOCAL extern class QoreClass *QC_QMenu;

DLLLOCAL class QoreClass *initQMenuClass(QoreClass *);

class myQMenu : public QMenu, public QoreQWidgetExtension
{
#define QOREQTYPE QMenu
#include "qore-qt-metacode.h"
#include "qore-qt-widget-events.h"
#undef QOREQTYPE

   public:
   DLLLOCAL myQMenu(Object *obj, QWidget* parent = 0) : QMenu(parent), QoreQWidgetExtension(obj->getClass())
      {
         init(obj);
         //init_widget_events();
      }
   DLLLOCAL myQMenu(Object *obj, const QString& title, QWidget* parent = 0) : QMenu(title, parent), QoreQWidgetExtension(obj->getClass())
      {
         init(obj);
         //init_widget_events();
      }
};

class QoreQMenu : public QoreAbstractQWidget
{
   public:
      QPointer<myQMenu> qobj;

      DLLLOCAL QoreQMenu(Object *obj, QWidget* parent = 0) : qobj(new myQMenu(obj, parent))
      {
      }
      DLLLOCAL QoreQMenu(Object *obj, const QString& title, QWidget* parent = 0) : qobj(new myQMenu(obj, title, parent))
      {
      }
      DLLLOCAL virtual class QObject *getQObject() const
      {
         return static_cast<QObject *>(&(*qobj));
      }
      DLLLOCAL virtual class QWidget *getQWidget() const
      {
         return static_cast<QWidget *>(&(*qobj));
      }
      DLLLOCAL virtual QPaintDevice *getQPaintDevice() const
      {
         return static_cast<QPaintDevice *>(&(*qobj));
      }
      QORE_VIRTUAL_QWIDGET_METHODS
};

#endif // _QORE_QT_QC_QMENU_H
