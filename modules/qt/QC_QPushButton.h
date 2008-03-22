/*
 QC_QPushButton.h
 
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

#ifndef _QORE_QT_QC_QPUSHBUTTON_H

#define _QORE_QT_QC_QPUSHBUTTON_H

#include <QPushButton>
#include "QoreAbstractQPushButton.h"
#include "qore-qt-events.h"

DLLLOCAL extern qore_classid_t CID_QPUSHBUTTON;
DLLLOCAL extern class QoreClass *QC_QPushButton;

DLLLOCAL class QoreClass *initQPushButtonClass(QoreClass *);

class myQPushButton : public QPushButton, public QoreQWidgetExtension
{
      friend class QoreQPushbutton;

#define QOREQTYPE QPushButton
#define MYQOREQTYPE myQPushButton
#include "qore-qt-metacode.h"
#include "qore-qt-widget-events.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQPushButton(QoreObject *obj, QWidget* parent = 0) : QPushButton(parent), QoreQWidgetExtension(obj->getClass())
      {
         init(obj);
      }
      DLLLOCAL myQPushButton(QoreObject *obj, const QString& text, QWidget* parent = 0) : QPushButton(text, parent), QoreQWidgetExtension(obj->getClass())
      {
         init(obj);
      }
      DLLLOCAL myQPushButton(QoreObject *obj, const QIcon& icon, const QString& text, QWidget* parent = 0) : QPushButton(icon, text, parent), QoreQWidgetExtension(obj->getClass())
      {
         init(obj);
      }
};

class QoreQPushButton : public QoreAbstractQPushButton
{
   public:
      QPointer<myQPushButton> qobj;

      DLLLOCAL QoreQPushButton(QoreObject *obj, QWidget* parent = 0) : qobj(new myQPushButton(obj, parent))
      {
      }
      DLLLOCAL QoreQPushButton(QoreObject *obj, const QString& text, QWidget* parent = 0) : qobj(new myQPushButton(obj, text, parent))
      {
      }
      DLLLOCAL QoreQPushButton(QoreObject *obj, const QIcon& icon, const QString& text, QWidget* parent = 0) : qobj(new myQPushButton(obj, icon, text, parent))
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
      DLLLOCAL virtual class QPushButton *getQPushButton() const
      {
         return static_cast<QPushButton *>(&(*qobj));
      }
      DLLLOCAL virtual class QAbstractButton *getQAbstractButton() const
      {
         return static_cast<QAbstractButton *>(&(*qobj));
      }
/*
      DLLLOCAL virtual void initStyleOption(QStyleOptionButton *style) const 
      {
	 qobj->initStyleOption(style);
      }
*/
      QORE_VIRTUAL_QWIDGET_METHODS
};

class QoreQtQPushButton : public QoreAbstractQPushButton
{
   public:
      QoreObject *qore_obj;
      QPointer<QPushButton> qobj;

      DLLLOCAL QoreQtQPushButton(QoreObject *obj, QPushButton *qpushbutton) : qore_obj(obj), qobj(qpushbutton)
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
      DLLLOCAL virtual class QPushButton *getQPushButton() const
      {
         return static_cast<QPushButton *>(&(*qobj));
      }
      DLLLOCAL virtual class QAbstractButton *getQAbstractButton() const
      {
         return static_cast<QAbstractButton *>(&(*qobj));
      }
/*
      DLLLOCAL virtual void initStyleOption(QStyleOptionButton *style) const 
      {
	 //qobj->initStyleOption(style);
      }
*/

#include "qore-qt-static-qwidget-methods.h"

};

#endif // _QORE_QT_QC_QPUSHBUTTON_H
