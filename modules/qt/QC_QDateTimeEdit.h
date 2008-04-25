/*
 QC_QDateTimeEdit.h
 
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

#ifndef _QORE_QT_QC_QDATETIMEEDIT_H

#define _QORE_QT_QC_QDATETIMEEDIT_H

#include <QDateTimeEdit>
#include "QoreAbstractQDateTimeEdit.h"
#include "qore-qt-events.h"

DLLLOCAL extern qore_classid_t CID_QDATETIMEEDIT;
DLLLOCAL extern QoreClass *QC_QDateTimeEdit;

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
      DLLLOCAL myQDateTimeEdit(QoreObject *obj, QWidget* parent = 0) : QDateTimeEdit(parent), QoreQWidgetExtension(obj, this)
      {
         
      }
      DLLLOCAL myQDateTimeEdit(QoreObject *obj, const QDateTime& datetime, QWidget* parent = 0) : QDateTimeEdit(datetime, parent), QoreQWidgetExtension(obj, this)
      {
         
      }
      DLLLOCAL myQDateTimeEdit(QoreObject *obj, const QDate& date, QWidget* parent = 0) : QDateTimeEdit(date, parent), QoreQWidgetExtension(obj, this)
      {
         
      }
      DLLLOCAL myQDateTimeEdit(QoreObject *obj, const QTime& time, QWidget* parent = 0) : QDateTimeEdit(time, parent), QoreQWidgetExtension(obj, this)
      {
         
      }
};

typedef QoreQDateTimeEditBase<myQDateTimeEdit, QoreAbstractQDateTimeEdit> QoreQDateTimeEditImpl;

class QoreQDateTimeEdit : public QoreQDateTimeEditImpl
{
   public:
      DLLLOCAL QoreQDateTimeEdit(QoreObject *obj, QWidget* parent = 0) : QoreQDateTimeEditImpl(new myQDateTimeEdit(obj, parent))
      {
      }
      DLLLOCAL QoreQDateTimeEdit(QoreObject *obj, const QDateTime& datetime, QWidget* parent = 0) : QoreQDateTimeEditImpl(new myQDateTimeEdit(obj, datetime, parent))
      {
      }
      DLLLOCAL QoreQDateTimeEdit(QoreObject *obj, const QDate& date, QWidget* parent = 0) : QoreQDateTimeEditImpl(new myQDateTimeEdit(obj, date, parent))
      {
      }
      DLLLOCAL QoreQDateTimeEdit(QoreObject *obj, const QTime& time, QWidget* parent = 0) : QoreQDateTimeEditImpl(new myQDateTimeEdit(obj, time, parent))
      {
      }
};

#endif // _QORE_QT_QC_QDATETIMEEDIT_H
