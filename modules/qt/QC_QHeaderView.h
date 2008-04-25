/*
 QC_QHeaderView.h
 
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
      DLLLOCAL myQHeaderView(QoreObject *obj, Qt::Orientation orientation, QWidget* parent = 0) : QHeaderView(orientation, parent), QoreQWidgetExtension(obj, this)
      {
         
      }

      DLLLOCAL void pub_setupViewport(QWidget *w)
      {
         setupViewport(w);
      }
};

typedef QoreQHeaderViewBase<myQHeaderView, QoreAbstractQHeaderView> QoreQHeaderViewImpl;

class QoreQHeaderView : public QoreQHeaderViewImpl
{
   public:
      DLLLOCAL QoreQHeaderView(QoreObject *obj, Qt::Orientation orientation, QWidget* parent = 0) : QoreQHeaderViewImpl(new myQHeaderView(obj, orientation, parent))
      {
      }
/*
      DLLLOCAL virtual void setupViewport(QWidget *w)
      {
         qobj->setupViewport(w);
      }
*/
};

typedef QoreQtQHeaderViewBase<QHeaderView, QoreAbstractQHeaderView> QoreQtQHeaderViewImpl;

class QoreQtQHeaderView : public QoreQtQHeaderViewImpl
{
   public:
      DLLLOCAL QoreQtQHeaderView(QoreObject *obj, QHeaderView *hv) : QoreQtQHeaderViewImpl(obj, hv)
      {
      }
/*
      // will never be called
      DLLLOCAL virtual void setupViewport(QWidget *w)
      {
      }
*/
};

#endif // _QORE_QT_QC_QHEADERVIEW_H
