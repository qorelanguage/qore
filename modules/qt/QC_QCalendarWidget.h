/*
 QC_QCalendarWidget.h
 
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

#ifndef _QORE_QT_QC_QCALENDARWIDGET_H

#define _QORE_QT_QC_QCALENDARWIDGET_H

#include <QCalendarWidget>
#include "QoreAbstractQWidget.h"
#include "qore-qt-events.h"

DLLLOCAL extern int CID_QCALENDARWIDGET;
DLLLOCAL extern class QoreClass *QC_QCalendarWidget;

DLLLOCAL class QoreClass *initQCalendarWidgetClass(QoreClass *);

class myQCalendarWidget : public QCalendarWidget, public QoreQWidgetExtension
{
#define QOREQTYPE QCalendarWidget
#include "qore-qt-metacode.h"
#include "qore-qt-widget-events.h"
#undef QOREQTYPE

   public:
   DLLLOCAL myQCalendarWidget(Object *obj, QWidget* parent = 0) : QCalendarWidget(parent), QoreQWidgetExtension(obj->getClass())
      {
         init(obj);
      }
};

class QoreQCalendarWidget : public QoreAbstractQWidget
{
   public:
      QPointer<myQCalendarWidget> qobj;

      DLLLOCAL QoreQCalendarWidget(Object *obj, QWidget* parent = 0) : qobj(new myQCalendarWidget(obj, parent))
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

#endif // _QORE_QT_QC_QCALENDARWIDGET_H
