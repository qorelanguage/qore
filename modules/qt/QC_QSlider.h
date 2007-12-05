/*
 QC_QSlider.h
 
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

#ifndef _QORE_QC_QSLIDER_H

#define _QORE_QC_QSLIDER_H

#include "QoreAbstractQAbstractSlider.h"

#include <QSlider>

#include "qore-qt-events.h"

DLLEXPORT extern int CID_QSLIDER;

DLLLOCAL class QoreClass *initQSliderClass(class QoreClass *qframe);

class myQSlider : public QSlider, public QoreQWidgetExtension
{
#define QOREQTYPE QSlider
#include "qore-qt-metacode.h"
#include "qore-qt-widget-events.h"
#undef QOREQTYPE

   public:
      DLLLOCAL myQSlider(QoreObject *obj, QWidget *parent = 0) : QSlider(parent), QoreQWidgetExtension(obj->getClass())
      {
	 init(obj);
	 //init_widget_events();
      }
      DLLLOCAL myQSlider(QoreObject *obj, Qt::Orientation orientation, QWidget *parent = 0) : QSlider(orientation, parent), QoreQWidgetExtension(obj->getClass())
      {
	 init(obj);
	 //init_widget_events();
      }
};


class QoreQSlider : public QoreAbstractQAbstractSlider
{
   public:
      QPointer<myQSlider>qobj;

      DLLLOCAL QoreQSlider(QoreObject *obj, QWidget *parent = 0) : qobj(new myQSlider(obj, parent))
      {
      }
      DLLLOCAL QoreQSlider(QoreObject *obj, Qt::Orientation orientation, QWidget *parent = 0) : qobj(new myQSlider(obj, orientation, parent))
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
      DLLLOCAL virtual QPaintDevice *getQPaintDevice() const
      {
         return static_cast<QPaintDevice *>(&(*qobj));
      }
      DLLLOCAL virtual QAbstractSlider *getQAbstractSlider() const
      {
         return static_cast<QAbstractSlider *>(&(*qobj));
      }

      QORE_VIRTUAL_QWIDGET_METHODS

};

#endif
