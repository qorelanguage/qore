/*
 QC_QAbstractSlider.h
 
 Qore Programming Language
 
 Copyright 2003 - 2008 David Nichols
 
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

DLLEXPORT extern qore_classid_t CID_QABSTRACTSLIDER;

DLLLOCAL class QoreClass *initQAbstractSliderClass(class QoreClass *qframe);

class myQAbstractSlider : public QAbstractSlider, public QoreQWidgetExtension
{
#define QOREQTYPE QAbstractSlider
#define MYQOREQTYPE myQAbstractSlider
#include "qore-qt-metacode.h"
#include "qore-qt-widget-events.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQAbstractSlider(QoreObject *obj, QWidget *parent = 0) : QAbstractSlider(parent), QoreQWidgetExtension(obj, this)
      {
	 
      }
};

typedef QoreQAbstractSliderBase<myQAbstractSlider, QoreAbstractQAbstractSlider> QoreQAbstractSliderImpl;

class QoreQAbstractSlider : public QoreQAbstractSliderImpl
{
   public:
      DLLLOCAL QoreQAbstractSlider(QoreObject *obj, QWidget *parent = 0) : QoreQAbstractSliderImpl(new myQAbstractSlider(obj, parent))
      {
      }
};

#endif
