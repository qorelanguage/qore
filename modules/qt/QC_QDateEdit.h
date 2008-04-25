/*
 QC_QDateEdit.h
 
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
      DLLLOCAL myQDateEdit(QoreObject *obj, QWidget* parent = 0) : QDateEdit(parent), QoreQWidgetExtension(obj, this)
      {
         
      }
      DLLLOCAL myQDateEdit(QoreObject *obj, const QDate& date, QWidget* parent = 0) : QDateEdit(date, parent), QoreQWidgetExtension(obj, this)
      {
         
      }
};

typedef QoreQDateTimeEditBase<myQDateEdit, QoreAbstractQDateTimeEdit> QoreQDateEditImpl;

class QoreQDateEdit : public QoreQDateEditImpl
{
   public:
      DLLLOCAL QoreQDateEdit(QoreObject *obj, QWidget* parent = 0) : QoreQDateEditImpl(new myQDateEdit(obj, parent))
      {
      }
      DLLLOCAL QoreQDateEdit(QoreObject *obj, const QDate& date, QWidget* parent = 0) : QoreQDateEditImpl(new myQDateEdit(obj, date, parent))
      {
      }
};

#endif // _QORE_QT_QC_QDATEEDIT_H
