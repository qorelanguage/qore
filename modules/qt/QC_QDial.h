/*
 QC_QDial.h
 
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

#ifndef _QORE_QT_QC_QDIAL_H

#define _QORE_QT_QC_QDIAL_H

#include <QDial>
#include "QoreAbstractQAbstractSlider.h"
#include "qore-qt-events.h"

DLLLOCAL extern qore_classid_t CID_QDIAL;
DLLLOCAL extern class QoreClass *QC_QDial;

DLLLOCAL class QoreClass *initQDialClass(QoreClass *);

class myQDial : public QDial, public QoreQWidgetExtension
{
#define QOREQTYPE QDial
#define MYQOREQTYPE myQDial
#include "qore-qt-metacode.h"
#include "qore-qt-widget-events.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQDial(QoreObject *obj, QWidget* parent = 0) : QDial(parent), QoreQWidgetExtension(obj, this)
      {
         
      }
      DLLLOCAL void pub_initStyleOption(QStyleOptionSlider * option) const
      {
	 initStyleOption(option);
      }
};

typedef QoreQAbstractSliderBase<myQDial, QoreAbstractQAbstractSlider> QoreQDialImpl;

class QoreQDial : public QoreQDialImpl
{
   public:
      DLLLOCAL QoreQDial(QoreObject *obj, QWidget* parent = 0) : QoreQDialImpl(new myQDial(obj, parent))
      {
      }
};

#endif // _QORE_QT_QC_QDIAL_H
