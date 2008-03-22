/*
 QC_QDateTimeEdit.h
 
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

#ifndef _QORE_QT_QC_QDATETIMEEDIT_H

#define _QORE_QT_QC_QDATETIMEEDIT_H

#include <QDateTimeEdit>
#include "QoreAbstractQDateTimeEdit.h"
#include "qore-qt-events.h"

DLLLOCAL extern qore_classid_t CID_QDATETIMEEDIT;
DLLLOCAL extern class QoreClass *QC_QDateTimeEdit;

DLLLOCAL class QoreClass *initQDateTimeEditClass(QoreClass *);

class myQDateTimeEdit : public QDateTimeEdit, public QoreQWidgetExtension
{
#define QOREQTYPE QDateTimeEdit
#define MYQOREQTYPE myQDateTimeEdit
#include "qore-qt-metacode.h"
#include "qore-qt-widget-events.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQDateTimeEdit(QoreObject *obj, QWidget* parent = 0) : QDateTimeEdit(parent), QoreQWidgetExtension(obj->getClass())
      {
         init(obj);
      }
      DLLLOCAL myQDateTimeEdit(QoreObject *obj, const QDateTime& datetime, QWidget* parent = 0) : QDateTimeEdit(datetime, parent), QoreQWidgetExtension(obj->getClass())
      {
         init(obj);
      }
      DLLLOCAL myQDateTimeEdit(QoreObject *obj, const QDate& date, QWidget* parent = 0) : QDateTimeEdit(date, parent), QoreQWidgetExtension(obj->getClass())
      {
         init(obj);
      }
      DLLLOCAL myQDateTimeEdit(QoreObject *obj, const QTime& time, QWidget* parent = 0) : QDateTimeEdit(time, parent), QoreQWidgetExtension(obj->getClass())
      {
         init(obj);
      }
};

class QoreQDateTimeEdit : public QoreAbstractQDateTimeEdit
{
   public:
      QPointer<myQDateTimeEdit> qobj;

      DLLLOCAL QoreQDateTimeEdit(QoreObject *obj, QWidget* parent = 0) : qobj(new myQDateTimeEdit(obj, parent))
      {
      }
      DLLLOCAL QoreQDateTimeEdit(QoreObject *obj, const QDateTime& datetime, QWidget* parent = 0) : qobj(new myQDateTimeEdit(obj, datetime, parent))
      {
      }
      DLLLOCAL QoreQDateTimeEdit(QoreObject *obj, const QDate& date, QWidget* parent = 0) : qobj(new myQDateTimeEdit(obj, date, parent))
      {
      }
      DLLLOCAL QoreQDateTimeEdit(QoreObject *obj, const QTime& time, QWidget* parent = 0) : qobj(new myQDateTimeEdit(obj, time, parent))
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
      DLLLOCAL virtual class QAbstractSpinBox *getQAbstractSpinBox() const
      {
         return static_cast<QAbstractSpinBox *>(&(*qobj));
      }
      DLLLOCAL virtual class QDateTimeEdit *getQDateTimeEdit() const
      {
         return static_cast<QDateTimeEdit *>(&(*qobj));
      }
      QORE_VIRTUAL_QWIDGET_METHODS
};

#endif // _QORE_QT_QC_QDATETIMEEDIT_H
