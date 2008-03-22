/*
 QC_QAbstractButton.h
 
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

#ifndef _QORE_QT_QC_QABSTRACTBUTTON_H

#define _QORE_QT_QC_QABSTRACTBUTTON_H

#include <QAbstractButton>
#include "QoreAbstractQAbstractButton.h"
#include "qore-qt-events.h"

DLLLOCAL extern qore_classid_t CID_QABSTRACTBUTTON;
DLLLOCAL extern class QoreClass *QC_QAbstractButton;

DLLLOCAL class QoreClass *initQAbstractButtonClass(QoreClass *);

class myQAbstractButton : public QAbstractButton, public QoreQWidgetExtension
{
#define QOREQTYPE QAbstractButton
#define MYQOREQTYPE myQAbstractButton
#include "qore-qt-metacode.h"
#include "qore-qt-widget-events.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
   DLLLOCAL myQAbstractButton(QoreObject *obj, QWidget* parent = 0) : QAbstractButton(parent), QoreQWidgetExtension(obj->getClass())
      {
         init(obj);
         //init_widget_events();
      }
};

class QoreQAbstractButton : public QoreAbstractQAbstractButton
{
   public:
      QPointer<myQAbstractButton> qobj;

      DLLLOCAL QoreQAbstractButton(QoreObject *obj, QWidget* parent = 0) : qobj(new myQAbstractButton(obj, parent))
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
      DLLLOCAL virtual class QAbstractButton *getQAbstractButton() const
      {
         return static_cast<QAbstractButton *>(&(*qobj));
      }

      QORE_VIRTUAL_QWIDGET_METHODS
};

class QoreQtQAbstractButton : public QoreAbstractQAbstractButton
{
   public:
      QoreObject *qore_obj;
      QPointer<QAbstractButton> qobj;

      DLLLOCAL QoreQtQAbstractButton(QoreObject *obj, QAbstractButton *b) : qore_obj(obj), qobj(b)
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
      DLLLOCAL virtual class QAbstractButton *getQAbstractButton() const
      {
         return static_cast<QAbstractButton *>(&(*qobj));
      }

#include "qore-qt-static-qwidget-methods.h"

};

#endif // _QORE_QT_QC_QABSTRACTBUTTON_H
