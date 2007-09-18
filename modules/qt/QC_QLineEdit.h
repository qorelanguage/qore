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

DLLLOCAL extern int CID_QLINEEDIT;
DLLLOCAL extern class QoreClass *QC_QLineEdit;

DLLLOCAL class QoreClass *initQLineEditClass(QoreClass *);

class myQLineEdit : public QLineEdit, public QoreQWidgetExtension
{
#define QOREQTYPE QLineEdit
#include "qore-qt-metacode.h"
#include "qore-qt-widget-events.h"
#undef QOREQTYPE

   public:
      DLLLOCAL myQLineEdit(Object *obj, QWidget* parent = 0) : QLineEdit(parent), QoreQWidgetExtension(obj->getClass())
      {
         init(obj);
         //init_widget_events();
      }
      DLLLOCAL myQLineEdit(Object *obj, const QString& contents, QWidget* parent = 0) : QLineEdit(contents, parent), QoreQWidgetExtension(obj->getClass())
      {
         init(obj);
         //init_widget_events();
      }
};

class QoreQLineEdit : public QoreAbstractQWidget
{
   public:
      QPointer<myQLineEdit> qobj;

      DLLLOCAL QoreQLineEdit(Object *obj, QWidget* parent = 0) : qobj(new myQLineEdit(obj, parent))
      {
      }
      DLLLOCAL QoreQLineEdit(Object *obj, const QString& contents, QWidget* parent = 0) : qobj(new myQLineEdit(obj, contents, parent))
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

#endif // _QORE_QT_QC_QLINEEDIT_H
