/*
 QC_QDateEdit.h
 
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

#ifndef _QORE_QT_QC_QDATEEDIT_H

#define _QORE_QT_QC_QDATEEDIT_H

#include <QDateEdit>
#include "QoreAbstractQDateTimeEdit.h"
#include "qore-qt-events.h"

DLLLOCAL extern qore_classid_t CID_QDATEEDIT;
DLLLOCAL extern class QoreClass *QC_QDateEdit;

DLLLOCAL class QoreClass *initQDateEditClass(QoreClass *);

class myQDateEdit : public QDateEdit, public QoreQWidgetExtension
{
#define QOREQTYPE QDateEdit
#define MYQOREQTYPE myQDateEdit
#include "qore-qt-metacode.h"
#include "qore-qt-widget-events.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQDateEdit(QoreObject *obj, QWidget* parent = 0) : QDateEdit(parent), QoreQWidgetExtension(obj->getClass())
      {
         init(obj);
      }
      DLLLOCAL myQDateEdit(QoreObject *obj, const QDate& date, QWidget* parent = 0) : QDateEdit(date, parent), QoreQWidgetExtension(obj->getClass())
      {
         init(obj);
      }
};

class QoreQDateEdit : public QoreAbstractQDateTimeEdit
{
   public:
      QPointer<myQDateEdit> qobj;

      DLLLOCAL QoreQDateEdit(QoreObject *obj, QWidget* parent = 0) : qobj(new myQDateEdit(obj, parent))
      {
      }
      DLLLOCAL QoreQDateEdit(QoreObject *obj, const QDate& date, QWidget* parent = 0) : qobj(new myQDateEdit(obj, date, parent))
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

#endif // _QORE_QT_QC_QDATEEDIT_H
