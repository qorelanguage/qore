/*
 QC_QToolBar.h
 
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

#ifndef _QORE_QT_QC_QTOOLBAR_H

#define _QORE_QT_QC_QTOOLBAR_H

#include <QToolBar>
#include "QoreAbstractQWidget.h"
#include "qore-qt-events.h"

DLLLOCAL extern qore_classid_t CID_QTOOLBAR;
DLLLOCAL extern QoreClass *QC_QToolBar;
DLLLOCAL QoreNamespace *initQToolBarNS(QoreClass *);

class myQToolBar : public QToolBar, public QoreQWidgetExtension
{
#define QOREQTYPE QToolBar
#include "qore-qt-metacode.h"
#include "qore-qt-widget-events.h"
#undef QOREQTYPE

   public:
      DLLLOCAL myQToolBar(QoreObject *obj, const QString& title, QWidget* parent = 0) : QToolBar(title, parent), QoreQWidgetExtension(obj->getClass())
      {
         init(obj);
      }
      DLLLOCAL myQToolBar(QoreObject *obj, QWidget* parent = 0) : QToolBar(parent), QoreQWidgetExtension(obj->getClass())
      {
         init(obj);
      }
};

class QoreQToolBar : public QoreAbstractQWidget
{
   public:
      QPointer<myQToolBar> qobj;

      DLLLOCAL QoreQToolBar(QoreObject *obj, const QString& title, QWidget* parent = 0) : qobj(new myQToolBar(obj, title, parent))
      {
      }
      DLLLOCAL QoreQToolBar(QoreObject *obj, QWidget* parent = 0) : qobj(new myQToolBar(obj, parent))
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

#endif // _QORE_QT_QC_QTOOLBAR_H
