/*
 QC_QCheckBox.h
 
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

#ifndef _QORE_QT_QC_QCHECKBOX_H

#define _QORE_QT_QC_QCHECKBOX_H

#include <QCheckBox>
#include "QoreAbstractQCheckBox.h"
#include "qore-qt-events.h"

DLLLOCAL extern int CID_QCHECKBOX;
DLLLOCAL extern class QoreClass *QC_QCheckBox;

DLLLOCAL class QoreClass *initQCheckBoxClass(QoreClass *);

class myQCheckBox : public QCheckBox, public QoreQWidgetExtension
{
#define QOREQTYPE QCheckBox
#include "qore-qt-metacode.h"
#include "qore-qt-widget-events.h"
#undef QOREQTYPE

   public:
      DLLLOCAL myQCheckBox(QoreObject *obj, QWidget* parent = 0) : QCheckBox(parent), QoreQWidgetExtension(obj->getClass())
      {
         init(obj);
      }
      DLLLOCAL myQCheckBox(QoreObject *obj, const QString& text, QWidget* parent = 0) : QCheckBox(text, parent), QoreQWidgetExtension(obj->getClass())
      {
         init(obj);
      }
};

class QoreQCheckBox : public QoreAbstractQCheckBox
{
   public:
      QPointer<myQCheckBox> qobj;

      DLLLOCAL QoreQCheckBox(QoreObject *obj, QWidget* parent = 0) : qobj(new myQCheckBox(obj, parent))
      {
      }
      DLLLOCAL QoreQCheckBox(QoreObject *obj, const QString& text, QWidget* parent = 0) : qobj(new myQCheckBox(obj, text, parent))
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
      DLLLOCAL virtual class QCheckBox *getQCheckBox() const
      {
         return static_cast<QCheckBox *>(&(*qobj));
      }
      QORE_VIRTUAL_QWIDGET_METHODS
};

class QoreQtQCheckBox : public QoreAbstractQCheckBox
{
   public:
      QoreObject *qore_obj;
      QPointer<QCheckBox> qobj;

      DLLLOCAL QoreQtQCheckBox(QoreObject *obj, QCheckBox *cb) : qore_obj(obj), qobj(cb)
      {
      }
      DLLLOCAL ~QoreQtQCheckBox()
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
      DLLLOCAL virtual class QCheckBox *getQCheckBox() const
      {
         return static_cast<QCheckBox *>(&(*qobj));
      }

#include "qore-qt-static-qwidget-methods.h"
};

#endif // _QORE_QT_QC_QCHECKBOX_H
