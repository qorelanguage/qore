/*
 QC_QTimeEdit.h
 
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

#ifndef _QORE_QT_QC_QTIMEEDIT_H

#define _QORE_QT_QC_QTIMEEDIT_H

#include <QTimeEdit>
#include "QoreAbstractQDateTimeEdit.h"
#include "qore-qt-events.h"

DLLLOCAL extern qore_classid_t CID_QTIMEEDIT;
DLLLOCAL extern class QoreClass *QC_QTimeEdit;

DLLLOCAL class QoreClass *initQTimeEditClass(QoreClass *);

class myQTimeEdit : public QTimeEdit, public QoreQWidgetExtension
{
#define QOREQTYPE QTimeEdit
#define MYQOREQTYPE myQTimeEdit
#include "qore-qt-metacode.h"
#include "qore-qt-widget-events.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQTimeEdit(QoreObject *obj, QWidget* parent = 0) : QTimeEdit(parent), QoreQWidgetExtension(obj, this)
      {
         
      }
      DLLLOCAL myQTimeEdit(QoreObject *obj, const QTime& time, QWidget* parent = 0) : QTimeEdit(time, parent), QoreQWidgetExtension(obj, this)
      {
         
      }
};

typedef QoreQDateTimeEditBase<myQTimeEdit, QoreAbstractQDateTimeEdit> QoreQTimeEditImpl;

class QoreQTimeEdit : public QoreQTimeEditImpl
{
   public:
      DLLLOCAL QoreQTimeEdit(QoreObject *obj, QWidget* parent = 0) : QoreQTimeEditImpl(new myQTimeEdit(obj, parent))
      {
      }
      DLLLOCAL QoreQTimeEdit(QoreObject *obj, const QTime& time, QWidget* parent = 0) : QoreQTimeEditImpl(new myQTimeEdit(obj, time, parent))
      {
      }
};

#endif // _QORE_QT_QC_QTIMEEDIT_H
