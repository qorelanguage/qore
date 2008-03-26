/*
 QC_QListView.h
 
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

#ifndef _QORE_QT_QC_QLISTVIEW_H

#define _QORE_QT_QC_QLISTVIEW_H

#include <QListView>
#include "QoreAbstractQListView.h"
#include "qore-qt-events.h"

DLLLOCAL extern qore_classid_t CID_QLISTVIEW;
DLLLOCAL extern QoreClass *QC_QListView;
DLLLOCAL QoreNamespace *initQListViewNS(QoreClass *);

class myQListView : public QListView, public QoreQWidgetExtension
{
#define QOREQTYPE QListView
#define MYQOREQTYPE myQListView
#include "qore-qt-metacode.h"
#include "qore-qt-widget-events.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQListView(QoreObject *obj, QWidget* parent = 0) : QListView(parent), QoreQWidgetExtension(obj, this)
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

typedef QoreQListViewBase<myQListView, QoreAbstractQListView> QoreQListViewImpl;

class QoreQListView : public QoreQListViewImpl
{
   public:
      DLLLOCAL QoreQListView(QoreObject *obj, QWidget* parent = 0) : QoreQListViewImpl(new myQListView(obj, parent))
      {
      }
};

#endif // _QORE_QT_QC_QLISTVIEW_H
