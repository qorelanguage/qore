/*
 QC_QLineEdit.h
 
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

#ifndef _QORE_QT_QC_QLINEEDIT_H

#define _QORE_QT_QC_QLINEEDIT_H

#include <QLineEdit>
#include "QoreAbstractQWidget.h"
#include "qore-qt-events.h"

DLLLOCAL extern qore_classid_t CID_QLINEEDIT;
DLLLOCAL extern class QoreClass *QC_QLineEdit;

DLLLOCAL class QoreClass *initQLineEditClass(QoreClass *);

class myQLineEdit : public QLineEdit, public QoreQWidgetExtension
{
#define QOREQTYPE QLineEdit
#define MYQOREQTYPE myQLineEdit
#include "qore-qt-metacode.h"
#include "qore-qt-widget-events.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQLineEdit(QoreObject *obj, QWidget* parent = 0) : QLineEdit(parent), QoreQWidgetExtension(obj, this)
      {
         
         //init_widget_events();
      }
      DLLLOCAL myQLineEdit(QoreObject *obj, const QString& contents, QWidget* parent = 0) : QLineEdit(contents, parent), QoreQWidgetExtension(obj, this)
      {
         
         //init_widget_events();
      }
};

typedef QoreQWidgetBase<myQLineEdit, QoreAbstractQWidget> QoreQLineEditImpl;

class QoreQLineEdit : public QoreQLineEditImpl
{
   public:
      DLLLOCAL QoreQLineEdit(QoreObject *obj, QWidget* parent = 0) : QoreQLineEditImpl(new myQLineEdit(obj, parent))
      {
      }
      DLLLOCAL QoreQLineEdit(QoreObject *obj, const QString& contents, QWidget* parent = 0) : QoreQLineEditImpl(new myQLineEdit(obj, contents, parent))
      {
      }
};

#endif // _QORE_QT_QC_QLINEEDIT_H
