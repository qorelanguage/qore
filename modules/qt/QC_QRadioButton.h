/*
 QC_QRadioButton.h
 
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

#ifndef _QORE_QT_QC_QRADIOBUTTON_H

#define _QORE_QT_QC_QRADIOBUTTON_H

#include <QRadioButton>
#include "QoreAbstractQRadioButton.h"
#include "qore-qt-events.h"

DLLLOCAL extern int CID_QRADIOBUTTON;
DLLLOCAL extern class QoreClass *QC_QRadioButton;

DLLLOCAL class QoreClass *initQRadioButtonClass(QoreClass *);

class myQRadioButton : public QRadioButton, public QoreQWidgetExtension
{
#define QOREQTYPE QRadioButton
#include "qore-qt-metacode.h"
#include "qore-qt-widget-events.h"
#undef QOREQTYPE

   public:
      DLLLOCAL myQRadioButton(Object *obj, QWidget* parent = 0) : QRadioButton(parent), QoreQWidgetExtension(obj->getClass())
      {
         init(obj);
      }
      DLLLOCAL myQRadioButton(Object *obj, const QString& text, QWidget* parent = 0) : QRadioButton(text, parent), QoreQWidgetExtension(obj->getClass())
      {
         init(obj);
      }
};

class QoreQRadioButton : public QoreAbstractQRadioButton
{
   public:
      QPointer<myQRadioButton> qobj;

      DLLLOCAL QoreQRadioButton(Object *obj, QWidget* parent = 0) : qobj(new myQRadioButton(obj, parent))
      {
      }
      DLLLOCAL QoreQRadioButton(Object *obj, const QString& text, QWidget* parent = 0) : qobj(new myQRadioButton(obj, text, parent))
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
      DLLLOCAL virtual class QRadioButton *getQRadioButton() const
      {
         return static_cast<QRadioButton *>(&(*qobj));
      }
      QORE_VIRTUAL_QWIDGET_METHODS
};

class QoreQtQRadioButton : public QoreAbstractQAbstractButton
{
   public:
      Object *qore_obj;
      QPointer<QRadioButton> qobj;

      DLLLOCAL QoreQtQRadioButton(Object *obj, QRadioButton *qb) : qore_obj(obj), qobj(qb)
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
      DLLLOCAL virtual class QRadioButton *getQRadioButton() const
      {
         return static_cast<QRadioButton *>(&(*qobj));
      }

#include "qore-qt-static-qwidget-methods.h"

};

#endif // _QORE_QT_QC_QRADIOBUTTON_H
