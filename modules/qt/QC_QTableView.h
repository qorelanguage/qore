/*
 QC_QTableView.h
 
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

#ifndef _QORE_QT_QC_QTABLEVIEW_H

#define _QORE_QT_QC_QTABLEVIEW_H

#include <QTableView>
#include "QoreAbstractQTableView.h"
#include "qore-qt-events.h"

DLLLOCAL extern qore_classid_t CID_QTABLEVIEW;
DLLLOCAL extern class QoreClass *QC_QTableView;

DLLLOCAL class QoreClass *initQTableViewClass(QoreClass *);

class myQTableView : public QTableView, public QoreQWidgetExtension
{
      friend class QoreQTableView;

#define QOREQTYPE QTableView
#define MYQOREQTYPE myQTableView
#include "qore-qt-metacode.h"
#include "qore-qt-widget-events.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQTableView(QoreObject *obj, QWidget* parent = 0) : QTableView(parent), QoreQWidgetExtension(obj->getClass())
      {
         init(obj);
      }
};

class QoreQTableView : public QoreAbstractQTableView
{
   public:
      QPointer<myQTableView> qobj;

      DLLLOCAL QoreQTableView(QoreObject *obj, QWidget* parent = 0) : qobj(new myQTableView(obj, parent))
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
      DLLLOCAL virtual class QTableView *getQTableView() const
      {
         return static_cast<QTableView *>(&(*qobj));
      }
      DLLLOCAL virtual class QFrame *getQFrame() const
      {
         return static_cast<QFrame *>(&(*qobj));
      }
      DLLLOCAL virtual class QAbstractScrollArea *getQAbstractScrollArea() const
      {
         return static_cast<QAbstractScrollArea *>(&(*qobj));
      }
      DLLLOCAL virtual class QAbstractItemView *getQAbstractItemView() const
      {
         return static_cast<QAbstractItemView *>(&(*qobj));
      }
      DLLLOCAL virtual void setupViewport(QWidget *w)
      {
         qobj->setupViewport(w);
      }

      QORE_VIRTUAL_QWIDGET_METHODS
};

#endif // _QORE_QT_QC_QTABLEVIEW_H
