/*
 QC_QMenuBar.h
 
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

#ifndef _QORE_QT_QC_QMENUBAR_H

#define _QORE_QT_QC_QMENUBAR_H

#include <QMenuBar>
#include "QoreAbstractQMenuBar.h"
#include "qore-qt-events.h"

DLLLOCAL extern qore_classid_t CID_QMENUBAR;
DLLLOCAL extern class QoreClass *QC_QMenuBar;

DLLLOCAL class QoreClass *initQMenuBarClass(QoreClass *);

class myQMenuBar : public QMenuBar, public QoreQWidgetExtension
{
#define QOREQTYPE QMenuBar
#define MYQOREQTYPE myQMenuBar
#include "qore-qt-metacode.h"
#include "qore-qt-widget-events.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQMenuBar(QoreObject *obj, QWidget* parent = 0) : QMenuBar(parent), QoreQWidgetExtension(obj->getClass())
      {
         init(obj);
      }
};

class QoreQMenuBar : public QoreAbstractQMenuBar
{
   public:
      QPointer<myQMenuBar> qobj;

      DLLLOCAL QoreQMenuBar(QoreObject *obj, QWidget* parent = 0) : qobj(new myQMenuBar(obj, parent))
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
      DLLLOCAL virtual class QMenuBar *getQMenuBar() const
      {
         return static_cast<QMenuBar *>(&(*qobj));
      }
      QORE_VIRTUAL_QWIDGET_METHODS
};

class QoreQtQMenuBar : public QoreAbstractQMenuBar
{
   public:
      QoreObject *qore_obj;
      QPointer<QMenuBar> qobj;

      DLLLOCAL QoreQtQMenuBar(QoreObject *obj, QMenuBar *qmenubar) : qore_obj(obj), qobj(qmenubar)
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
      DLLLOCAL virtual class QMenuBar *getQMenuBar() const
      {
         return static_cast<QMenuBar *>(&(*qobj));
      }
#include "qore-qt-static-qwidget-methods.h"
};

#endif // _QORE_QT_QC_QMENUBAR_H
