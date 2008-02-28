/*
 QC_QScrollBar.h
 
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

#ifndef _QORE_QT_QC_QSCROLLBAR_H

#define _QORE_QT_QC_QSCROLLBAR_H

#include <QScrollBar>
#include "QoreAbstractQScrollBar.h"
#include "qore-qt-events.h"

DLLLOCAL extern qore_classid_t CID_QSCROLLBAR;
DLLLOCAL extern class QoreClass *QC_QScrollBar;

DLLLOCAL class QoreClass *initQScrollBarClass(QoreClass *);

class myQScrollBar : public QScrollBar, public QoreQWidgetExtension
{
#define QOREQTYPE QScrollBar
#include "qore-qt-metacode.h"
#include "qore-qt-widget-events.h"
#undef QOREQTYPE

   public:
      DLLLOCAL myQScrollBar(QoreObject *obj, QWidget* parent = 0) : QScrollBar(parent), QoreQWidgetExtension(obj->getClass())
      {
         init(obj);
      }
      DLLLOCAL myQScrollBar(QoreObject *obj, Qt::Orientation orientation, QWidget* parent = 0) : QScrollBar(orientation, parent), QoreQWidgetExtension(obj->getClass())
      {
         init(obj);
      }
};

class QoreQScrollBar : public QoreAbstractQScrollBar
{
   public:
      QPointer<myQScrollBar> qobj;

      DLLLOCAL QoreQScrollBar(QoreObject *obj, QWidget* parent = 0) : qobj(new myQScrollBar(obj, parent))
      {
      }
      DLLLOCAL QoreQScrollBar(QoreObject *obj, Qt::Orientation orientation, QWidget* parent = 0) : qobj(new myQScrollBar(obj, orientation, parent))
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
      DLLLOCAL virtual class QAbstractSlider *getQAbstractSlider() const
      {
         return static_cast<QAbstractSlider *>(&(*qobj));
      }
      DLLLOCAL virtual class QScrollBar *getQScrollBar() const
      {
         return static_cast<QScrollBar *>(&(*qobj));
      }
      QORE_VIRTUAL_QWIDGET_METHODS
};

class QoreQtQScrollBar : public QoreAbstractQScrollBar
{
   public:
      QoreObject *qore_obj;
      QPointer<QScrollBar> qobj;

      DLLLOCAL QoreQtQScrollBar(QoreObject *obj, QScrollBar *qscrollbar) : qore_obj(obj), qobj(qscrollbar)
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
      DLLLOCAL virtual class QAbstractSlider *getQAbstractSlider() const
      {
         return static_cast<QAbstractSlider *>(&(*qobj));
      }
      DLLLOCAL virtual class QScrollBar *getQScrollBar() const
      {
         return static_cast<QScrollBar *>(&(*qobj));
      }
#include "qore-qt-static-qwidget-methods.h"
};

#endif // _QORE_QT_QC_QSCROLLBAR_H
