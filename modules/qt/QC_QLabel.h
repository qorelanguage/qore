/*
 QC_QLabel.h
 
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

#ifndef _QORE_QC_QLABEL_H

#define _QORE_QC_QLABEL_H

#include "QoreAbstractQFrame.h"

#include <QLabel>

#include "qore-qt-events.h"

DLLEXPORT extern int CID_QLABEL;

DLLLOCAL class QoreClass *initQLabelClass(class QoreClass *qframe);

class myQLabel : public QLabel, public QoreQWidgetExtension
{
#define QOREQTYPE QLabel
#include "qore-qt-metacode.h"
#include "qore-qt-widget-events.h"
#undef QOREQTYPE

   public:
   DLLLOCAL myQLabel(Object *obj, QWidget *parent = 0, Qt::WindowFlags f = 0) : QLabel(parent, f), QoreQWidgetExtension(obj->getClass())
      {
	 init(obj);
	 //init_widget_events();
      }
   DLLLOCAL myQLabel(Object *obj, const char *text, QWidget *parent = 0, Qt::WindowFlags f = 0) : QLabel(text, parent, f), QoreQWidgetExtension(obj->getClass())
      {
	 init(obj);
	 //init_widget_events();
      }
};

class QoreQLabel : public QoreAbstractQFrame
{
   public:
      QPointer<myQLabel>qobj;

      DLLLOCAL QoreQLabel(Object *obj, QWidget *parent = 0, Qt::WindowFlags f = 0) : qobj(new myQLabel(obj, parent, f))
      {
      }
      DLLLOCAL QoreQLabel(Object *obj, const char *text, QWidget *parent = 0, Qt::WindowFlags f = 0) : qobj(new myQLabel(obj, text, parent, f))
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
