/*
 QC_QDesktopWidget.h
 
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
      DLLLOCAL myQDesktopWidget(QoreObject *obj) : QDesktopWidget(), QoreQWidgetExtension(obj->getClass())
      {
         init(obj);
      }
};

class QoreQDesktopWidget : public QoreAbstractQWidget
{
   public:
      QPointer<myQDesktopWidget> qobj;

      DLLLOCAL QoreQDesktopWidget(QoreObject *obj) : qobj(new myQDesktopWidget(obj))
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

class QoreQtQDesktopWidget : public QoreAbstractQWidget
{
   public:
      QoreObject *qore_obj;
      QPointer<QDesktopWidget> qobj;

      DLLLOCAL QoreQtQDesktopWidget(QoreObject *obj, QDesktopWidget *qdw) : qore_obj(obj), qobj(qdw)
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
#include "qore-qt-static-qwidget-methods.h"
};

#endif // _QORE_QT_QC_QDESKTOPWIDGET_H
