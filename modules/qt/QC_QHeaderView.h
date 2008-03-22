/*
 QC_QHeaderView.h
 
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

#ifndef _QORE_QT_QC_QHEADERVIEW_H

#define _QORE_QT_QC_QHEADERVIEW_H

#include <QHeaderView>
#include "QoreAbstractQHeaderView.h"
#include "qore-qt-events.h"

DLLLOCAL extern qore_classid_t CID_QHEADERVIEW;
DLLLOCAL extern class QoreClass *QC_QHeaderView;

DLLLOCAL class QoreClass *initQHeaderViewClass(QoreClass *);

class myQHeaderView : public QHeaderView, public QoreQWidgetExtension
{
      friend class QoreQHeaderView;

#define QOREQTYPE QHeaderView
#define MYQOREQTYPE myQHeaderView
#include "qore-qt-metacode.h"
#include "qore-qt-widget-events.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQHeaderView(QoreObject *obj, Qt::Orientation orientation, QWidget* parent = 0) : QHeaderView(orientation, parent), QoreQWidgetExtension(obj->getClass())
      {
         init(obj);
      }
};

class QoreQHeaderView : public QoreAbstractQHeaderView
{
   public:
      QPointer<myQHeaderView> qobj;

      DLLLOCAL QoreQHeaderView(QoreObject *obj, Qt::Orientation orientation, QWidget* parent = 0) : qobj(new myQHeaderView(obj, orientation, parent))
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
      DLLLOCAL virtual class QAbstractItemView *getQAbstractItemView() const
      {
         return static_cast<QAbstractItemView *>(&(*qobj));
      } 
      DLLLOCAL virtual class QHeaderView *getQHeaderView() const
      {
         return static_cast<QHeaderView *>(&(*qobj));
      }
      DLLLOCAL virtual class QFrame *getQFrame() const
      {
         return static_cast<QFrame *>(&(*qobj));
      }
      DLLLOCAL virtual class QAbstractScrollArea *getQAbstractScrollArea() const
      {
         return static_cast<QAbstractScrollArea *>(&(*qobj));
      }
      DLLLOCAL virtual void setupViewport(QWidget *w)
      {
         qobj->setupViewport(w);
      }
      QORE_VIRTUAL_QWIDGET_METHODS
};

class QoreQtQHeaderView : public QoreAbstractQHeaderView
{
   public:
      QoreObject *qore_obj;
      QPointer<QHeaderView> qobj;

      DLLLOCAL QoreQtQHeaderView(QoreObject *obj, QHeaderView *hv) : qore_obj(obj), qobj(hv)
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
      DLLLOCAL virtual class QAbstractItemView *getQAbstractItemView() const
      {
         return static_cast<QAbstractItemView *>(&(*qobj));
      }
      DLLLOCAL virtual class QFrame *getQFrame() const
      {
         return static_cast<QFrame *>(&(*qobj));
      }
      DLLLOCAL virtual class QAbstractScrollArea *getQAbstractScrollArea() const
      {
         return static_cast<QAbstractScrollArea *>(&(*qobj));
      }
      DLLLOCAL virtual class QHeaderView *getQHeaderView() const
      {
         return static_cast<QHeaderView *>(&(*qobj));
      }
      // will never be called
      DLLLOCAL virtual void setupViewport(QWidget *w)
      {
      }

#include "qore-qt-static-qwidget-methods.h"

};

#endif // _QORE_QT_QC_QHEADERVIEW_H
