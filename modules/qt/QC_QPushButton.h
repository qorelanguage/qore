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

#include "QoreAbstractQWidget.h"

#include <QPushButton>

DLLEXPORT extern int CID_QPUSHBUTTON;

DLLLOCAL class QoreClass *initQPushButtonClass(class QoreClass *parent);

class myQPushButton : public QPushButton
{
#define QOREQTYPE QPushButton
#include "qore-qt-metacode.h"
#undef QOREQTYPE
      myQPushButton(Object *obj, const char *str, QWidget *parent = 0) : QPushButton(str, parent)
      {
	 init(obj);
      }
      myQPushButton(Object *obj, QWidget *parent = 0) : QPushButton(parent)
      {
	 init(obj);
      }
};

class QoreQPushButton : public QoreAbstractQWidget
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

      QORE_VIRTUAL_QOBJECT_METHODS
};


#endif
