/*
 QC_QLCDNumber.h
 
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

#ifndef _QORE_QC_QLCDNUMBER_H

#define _QORE_QC_QLCDNUMBER_H

#include "QoreAbstractQFrame.h"

#include <QLCDNumber>

#include "qore-qt-events.h"

DLLEXPORT extern qore_classid_t CID_QLCDNUMBER;

DLLLOCAL class QoreClass *initQLCDNumberClass(class QoreClass *qframe);

class myQLCDNumber : public QLCDNumber, public QoreQWidgetExtension
{
#define QOREQTYPE QLCDNumber
#include "qore-qt-metacode.h"
#include "qore-qt-widget-events.h"
#undef QOREQTYPE

   public:
      DLLLOCAL myQLCDNumber(QoreObject *obj, QWidget *parent = 0) : QLCDNumber(parent), QoreQWidgetExtension(obj->getClass())
      {
	 init(obj);
	 //init_widget_events();
      }
      DLLLOCAL myQLCDNumber(QoreObject *obj, int num_digits, QWidget *parent = 0) : QLCDNumber(num_digits, parent), QoreQWidgetExtension(obj->getClass())
      {
	 init(obj);
	 //init_widget_events();
      }
};

class QoreQLCDNumber : public QoreAbstractQFrame
{
   public:
      QPointer<myQLCDNumber>qobj;

      DLLLOCAL QoreQLCDNumber(QoreObject *obj, int num_digits, QWidget *parent = 0) : qobj(new myQLCDNumber(obj, num_digits, parent))
      {
      }
      DLLLOCAL QoreQLCDNumber(QoreObject *obj, QWidget *parent = 0) : qobj(new myQLCDNumber(obj, parent))
      {
      }
      DLLLOCAL virtual ~QoreQLCDNumber()
      {
      }
      DLLLOCAL virtual class QObject *getQObject() const
      {
	 return static_cast<QObject *>(&(*qobj));
      }
      DLLLOCAL virtual QWidget *getQWidget() const
      {
	 return static_cast<QWidget *>(&(*qobj));
      }
      DLLLOCAL virtual QFrame *getQFrame() const
      {
	 return static_cast<QFrame *>(&(*qobj));
      }
      DLLLOCAL virtual QPaintDevice *getQPaintDevice() const
      {
         return static_cast<QPaintDevice *>(&(*qobj));
      }

      QORE_VIRTUAL_QWIDGET_METHODS

};

#endif
