/*
 QC_QListWidget.h
 
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

#ifndef _QORE_QT_QC_QLISTWIDGET_H

#define _QORE_QT_QC_QLISTWIDGET_H

#include <QListWidget>
#include "QoreAbstractQListWidget.h"
#include "qore-qt-events.h"

DLLLOCAL extern qore_classid_t CID_QLISTWIDGET;
DLLLOCAL extern QoreClass *QC_QListWidget;
DLLLOCAL QoreClass *initQListWidgetClass(QoreClass *);

class myQListWidget : public QListWidget, public QoreQWidgetExtension
{
#define QOREQTYPE QListWidget
#define MYQOREQTYPE myQListWidget
#include "qore-qt-metacode.h"
#include "qore-qt-widget-events.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQListWidget(QoreObject *obj, QWidget* parent = 0) : QListWidget(parent), QoreQWidgetExtension(obj, this)
      {
         
      }
      DLLLOCAL void pub_setupViewport(QWidget *w)
      {
	 setupViewport(w);
      }
      DLLLOCAL QRect pub_rectForIndex(const QModelIndex & index) const
      {
	 return rectForIndex(index);
      }
      DLLLOCAL void pub_setPositionForIndex(const QPoint & position, const QModelIndex & index)
      {
	 setPositionForIndex(position, index);
      }
};

typedef QoreQListWidgetBase<myQListWidget, QoreAbstractQListWidget> QoreQListWidgetImpl;

class QoreQListWidget : public QoreQListWidgetImpl
{
   public:
      DLLLOCAL QoreQListWidget(QoreObject *obj, QWidget* parent = 0) : QoreQListWidgetImpl(new myQListWidget(obj, parent))
      {
      }
};

typedef QoreQtQListWidgetBase<QListWidget, QoreAbstractQListWidget> QoreQtQListWidgetImpl;

class QoreQtQListWidget : public QoreQtQListWidgetImpl
{
   public:
      DLLLOCAL QoreQtQListWidget(QoreObject *obj, QListWidget *qlw) : QoreQtQListWidgetImpl(obj, qlw)
      {
      }
};


#endif // _QORE_QT_QC_QLISTWIDGET_H
