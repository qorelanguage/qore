/*
 QC_QLabel.h
 
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

#ifndef _QORE_QC_QLABEL_H

#define _QORE_QC_QLABEL_H

#include "QoreAbstractQFrame.h"

#include <QLabel>

#include "qore-qt-events.h"

DLLEXPORT extern qore_classid_t CID_QLABEL;

DLLLOCAL class QoreClass *initQLabelClass(class QoreClass *qframe);

class myQLabel : public QLabel, public QoreQWidgetExtension
{
#define QOREQTYPE QLabel
#define MYQOREQTYPE myQLabel
#include "qore-qt-metacode.h"
#include "qore-qt-widget-events.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQLabel(QoreObject *obj, QWidget *parent = 0, Qt::WindowFlags f = 0) : QLabel(parent, f), QoreQWidgetExtension(obj, this)
      {
	 
	 //init_widget_events();
      }
      DLLLOCAL myQLabel(QoreObject *obj, const char *text, QWidget *parent = 0, Qt::WindowFlags f = 0) : QLabel(text, parent, f), QoreQWidgetExtension(obj, this)
      {
	 
	 //init_widget_events();
      }
};

typedef QoreQFrameBase<myQLabel, QoreAbstractQFrame> QoreQLabelImpl;

class QoreQLabel : public QoreQLabelImpl
{
   public:
      DLLLOCAL QoreQLabel(QoreObject *obj, QWidget *parent = 0, Qt::WindowFlags f = 0) : QoreQLabelImpl(new myQLabel(obj, parent, f))
      {
      }
      DLLLOCAL QoreQLabel(QoreObject *obj, const char *text, QWidget *parent = 0, Qt::WindowFlags f = 0) : QoreQLabelImpl(new myQLabel(obj, text, parent, f))
      {
      }
};

#endif
