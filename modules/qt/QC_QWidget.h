/*
 QC_QWidget.h
 
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

#ifndef _QORE_QC_QWIDGET_H

#define _QORE_QC_QWIDGET_H

#include "QoreAbstractQWidget.h"
#include "QoreAbstractQLayout.h"

#include <QWidget>

#include <string>

#include "qore-qt-events.h"

DLLLOCAL extern int CID_QWIDGET;
DLLLOCAL extern QoreClass *QC_QWidget;

DLLLOCAL class QoreClass *initQWidgetClass(class QoreClass *qobject, class QoreClass *qpaintdevice);

class myQWidget : public QWidget, public QoreQWidgetExtension
{
#define QOREQTYPE QWidget
#include "qore-qt-metacode.h"
#include "qore-qt-widget-events.h"
#undef QOREQTYPE

   public:
      DLLLOCAL myQWidget(Object *obj, QWidget *parent = 0, Qt::WindowFlags window_flags = 0) : QWidget(parent, window_flags), QoreQWidgetExtension(obj->getClass())
      {
	 init(obj);
	 //init_widget_events();
      }
};

class QoreQWidget : public QoreAbstractQWidget
{
   public:
      QPointer<myQWidget>qobj;

      DLLLOCAL QoreQWidget(Object *obj, QWidget *parent = 0, Qt::WindowFlags window_flags = 0) : qobj(new myQWidget(obj, parent, window_flags))
      {
      }
      DLLLOCAL virtual QObject *getQObject() const
      {
	 return static_cast<QObject *>(&(*qobj));
      }
      DLLLOCAL virtual QWidget *getQWidget() const
      {
	 //return static_cast<QWidget *>(&(*qobj));
	 return &(*qobj);
      }
      DLLLOCAL virtual QPaintDevice *getQPaintDevice() const
      {
	 return static_cast<QPaintDevice *>(&(*qobj));
      }

      QORE_VIRTUAL_QWIDGET_METHODS
};

class QoreQtQWidget : public QoreAbstractQWidget
{
   public:
      Object *qore_obj;
      QPointer<QWidget>qobj;

      DLLLOCAL QoreQtQWidget(Object *obj, QWidget *qw) : qore_obj(obj), qobj(qw)
      {
      }
      DLLLOCAL virtual QObject *getQObject() const
      {
	 return static_cast<QObject *>(&(*qobj));
      }
      DLLLOCAL virtual QWidget *getQWidget() const
      {
	 //return static_cast<QWidget *>(&(*qobj));
	 return &(*qobj);
      }
      DLLLOCAL virtual QPaintDevice *getQPaintDevice() const
      {
	 return static_cast<QPaintDevice *>(&(*qobj));
      }

#include "qore-qt-static-qwidget-methods.h"
};

#endif
