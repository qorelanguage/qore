/*
 QC_QTableWidget.h
 
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

#ifndef _QORE_QT_QC_QTABLEWIDGET_H

#define _QORE_QT_QC_QTABLEWIDGET_H

#include <QTableWidget>
#include "QoreAbstractQTableView.h"
#include "qore-qt-events.h"

DLLLOCAL extern qore_classid_t CID_QTABLEWIDGET;
DLLLOCAL extern class QoreClass *QC_QTableWidget;

DLLLOCAL class QoreClass *initQTableWidgetClass(QoreClass *);

class myQTableWidget : public QTableWidget, public QoreQWidgetExtension
{
      friend class QoreQTableWidget;

#define QOREQTYPE QTableWidget
#define MYQOREQTYPE myQTableWidget
#include "qore-qt-metacode.h"
#include "qore-qt-widget-events.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQTableWidget(QoreObject *obj, QWidget* parent = 0) : QTableWidget(parent), QoreQWidgetExtension(obj->getClass())
      {
         init(obj);
      }
      DLLLOCAL myQTableWidget(QoreObject *obj, int rows, int columns, QWidget* parent = 0) : QTableWidget(rows, columns, parent), QoreQWidgetExtension(obj->getClass())
      {
         init(obj);
      }
};

class QoreQTableWidget : public QoreAbstractQTableView
{
   public:
      QPointer<myQTableWidget> qobj;

      DLLLOCAL QoreQTableWidget(QoreObject *obj, QWidget* parent = 0) : qobj(new myQTableWidget(obj, parent))
      {
      }
      DLLLOCAL QoreQTableWidget(QoreObject *obj, int rows, int columns, QWidget* parent = 0) : qobj(new myQTableWidget(obj, rows, columns, parent))
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

#endif // _QORE_QT_QC_QTABLEWIDGET_H
