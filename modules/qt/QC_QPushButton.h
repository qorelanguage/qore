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

#ifndef _QORE_QC_QPUSHBUTTON_H

#define _QORE_QC_QPUSHBUTTON_H

#include "QoreAbstractQAbstractButton.h"

#include <QPushButton>

#include "qore-qt-events.h"

DLLEXPORT extern int CID_QPUSHBUTTON;

DLLLOCAL class QoreClass *initQPushButtonClass(class QoreClass *parent);

class myQPushButton : public QPushButton, public QoreQWidgetExtension
{
#define QOREQTYPE QPushButton
#include "qore-qt-metacode.h"
#include "qore-qt-widget-events.h"
#undef QOREQTYPE
      myQPushButton(Object *obj, const char *str, QWidget *parent = 0) : QPushButton(str, parent), QoreQWidgetExtension(obj->getClass())
      {
	 init(obj);
      }
      myQPushButton(Object *obj, QWidget *parent = 0) : QPushButton(parent), QoreQWidgetExtension(obj->getClass())
      {
	 init(obj);
      }
};

class QoreQPushButton : public QoreAbstractQAbstractButton
{
   public:
      QPointer<myQPushButton> qobj;
   
      DLLLOCAL QoreQPushButton(Object *obj, const char *str, QWidget *parent = 0) : qobj(new myQPushButton(obj, str, parent))
      {
      }
      DLLLOCAL QoreQPushButton(Object *obj, QWidget *parent = 0) : qobj(new myQPushButton(obj, parent))
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
      DLLLOCAL virtual QAbstractButton *getQAbstractButton() const
      {
         return static_cast<QAbstractButton *>(&(*qobj));
      }

      QORE_VIRTUAL_QWIDGET_METHODS
};


#endif
