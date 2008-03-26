/*
 QC_QMainWindow.h
 
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

#ifndef _QORE_QT_QC_QMAINWINDOW_H

#define _QORE_QT_QC_QMAINWINDOW_H

#include <QMainWindow>
#include "QoreAbstractQWidget.h"
#include "qore-qt-events.h"

DLLLOCAL extern qore_classid_t CID_QMAINWINDOW;
DLLLOCAL extern class QoreClass *QC_QMainWindow;

DLLLOCAL class QoreClass *initQMainWindowClass(QoreClass *);

class myQMainWindow : public QMainWindow, public QoreQWidgetExtension
{
#define QOREQTYPE QMainWindow
#define MYQOREQTYPE myQMainWindow
#include "qore-qt-metacode.h"
#include "qore-qt-widget-events.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQMainWindow(QoreObject *obj, QWidget* parent = 0, Qt::WindowFlags flags = 0) : QMainWindow(parent, flags), QoreQWidgetExtension(obj, this)
      {
         
      }
};

typedef QoreQWidgetBase<myQMainWindow, QoreAbstractQWidget> QoreQMainWindowImpl;

class QoreQMainWindow : public QoreQMainWindowImpl
{
   public:
      DLLLOCAL QoreQMainWindow(QoreObject *obj, QWidget* parent = 0, Qt::WindowFlags flags = 0) : QoreQMainWindowImpl(new myQMainWindow(obj, parent, flags))
      {
      }
};

#endif // _QORE_QT_QC_QMAINWINDOW_H
