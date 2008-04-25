/*
 QC_QDesktopWidget.h
 
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

#ifndef _QORE_QT_QC_QDESKTOPWIDGET_H

#define _QORE_QT_QC_QDESKTOPWIDGET_H

#include <QDesktopWidget>
#include "QoreAbstractQWidget.h"
#include "qore-qt-events.h"

DLLLOCAL extern qore_classid_t CID_QDESKTOPWIDGET;
DLLLOCAL extern class QoreClass *QC_QDesktopWidget;

DLLLOCAL class QoreClass *initQDesktopWidgetClass(QoreClass *);

class myQDesktopWidget : public QDesktopWidget, public QoreQWidgetExtension
{
#define QOREQTYPE QDesktopWidget
#define MYQOREQTYPE myQDesktopWidget
#include "qore-qt-metacode.h"
#include "qore-qt-widget-events.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQDesktopWidget(QoreObject *obj) : QDesktopWidget(), QoreQWidgetExtension(obj, this)
      {         
      }
};

typedef QoreQWidgetBase<myQDesktopWidget, QoreAbstractQWidget> QoreQDesktopWidgetImpl;

class QoreQDesktopWidget : public QoreQDesktopWidgetImpl
{
   public:
      DLLLOCAL QoreQDesktopWidget(QoreObject *obj) : QoreQDesktopWidgetImpl(new myQDesktopWidget(obj))
      {
      }
};

typedef QoreQtQWidgetBase<QDesktopWidget, QoreAbstractQWidget> QoreQtQDesktopWidgetImpl;

class QoreQtQDesktopWidget : public QoreQtQDesktopWidgetImpl
{
   public:
   DLLLOCAL QoreQtQDesktopWidget(QoreObject *obj, QDesktopWidget *qdw, bool managed = true) : QoreQtQDesktopWidgetImpl(obj, qdw, managed)
      {
      }
};

#endif // _QORE_QT_QC_QDESKTOPWIDGET_H
