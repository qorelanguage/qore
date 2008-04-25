/*
 QC_QTableWidget.h
 
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

#ifndef _QORE_QT_QC_QTABLEWIDGET_H

#define _QORE_QT_QC_QTABLEWIDGET_H

#include <QTableWidget>
#include "QoreAbstractQTableView.h"
#include "qore-qt-events.h"

DLLLOCAL extern qore_classid_t CID_QTABLEWIDGET;
DLLLOCAL extern QoreClass *QC_QTableWidget;

DLLLOCAL class QoreClass *initQTableWidgetClass(QoreClass *);

class myQTableWidget : public QTableWidget, public QoreQWidgetExtension
{
#define QOREQTYPE QTableWidget
#define MYQOREQTYPE myQTableWidget
#include "qore-qt-metacode.h"
#include "qore-qt-widget-events.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQTableWidget(QoreObject *obj, QWidget* parent = 0) : QTableWidget(parent), QoreQWidgetExtension(obj, this)
      {
      }

      DLLLOCAL myQTableWidget(QoreObject *obj, int rows, int columns, QWidget* parent = 0) : QTableWidget(rows, columns, parent), QoreQWidgetExtension(obj, this)
      {
      }

      DLLLOCAL void pub_setupViewport(QWidget *w)
      {
         setupViewport(w);
      }
};

typedef QoreQTableViewBase<myQTableWidget, QoreAbstractQTableView> QoreQTableWidgetImpl;

class QoreQTableWidget : public QoreQTableWidgetImpl
{
   public:
      DLLLOCAL QoreQTableWidget(QoreObject *obj, QWidget* parent = 0) : QoreQTableWidgetImpl(new myQTableWidget(obj, parent))
      {
      }
      DLLLOCAL QoreQTableWidget(QoreObject *obj, int rows, int columns, QWidget* parent = 0) : QoreQTableWidgetImpl(new myQTableWidget(obj, rows, columns, parent))
      {
      }
};

#endif // _QORE_QT_QC_QTABLEWIDGET_H
