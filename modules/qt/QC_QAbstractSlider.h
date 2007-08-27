/*
 QC_QAbstractSlider.h
 
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

#ifndef _QORE_QC_QABSTRACTSLIDER_H

#define _QORE_QC_QABSTRACTSLIDER_H

#include "QoreAbstractQAbstractSlider.h"

#include <QAbstractSlider>

#include "qore-qt-events.h"

DLLEXPORT extern int CID_QABSTRACTSLIDER;

DLLLOCAL class QoreClass *initQAbstractSliderClass(class QoreClass *qframe);

class myQAbstractSlider : public QAbstractSlider
{
#define QOREQTYPE QAbstractSlider
#include "qore-qt-metacode.h"
#include "qore-qt-widget-events.h"
#undef QOREQTYPE

   public:
      DLLLOCAL myQAbstractSlider(Object *obj, QWidget *parent = 0) : QAbstractSlider(parent)
      {
	 init(obj);
      }
};


class QoreQAbstractSlider : public QoreAbstractQAbstractSlider
{
   public:
      QPointer<myQAbstractSlider>qobj;

      DLLLOCAL QoreQAbstractSlider(Object *obj, QWidget *parent = 0) : qobj(new myQAbstractSlider(obj, parent))
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

      QORE_VIRTUAL_QOBJECT_METHODS

};

#endif
