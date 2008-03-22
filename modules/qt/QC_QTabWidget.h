/*
 QC_QTabWidget.h
 
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

#ifndef _QORE_QT_QC_QTABWIDGET_H

#define _QORE_QT_QC_QTABWIDGET_H

#include <QTabWidget>
#include "QoreAbstractQWidget.h"
#include "qore-qt-events.h"

DLLLOCAL extern qore_classid_t CID_QTABWIDGET;
DLLLOCAL extern class QoreClass *QC_QTabWidget;

DLLLOCAL class QoreClass *initQTabWidgetClass(QoreClass *);

class myQTabWidget : public QTabWidget, public QoreQWidgetExtension
{
      friend class QoreQTabWidget;

#define QOREQTYPE QTabWidget
#define MYQOREQTYPE myQTabWidget
#include "qore-qt-metacode.h"
#include "qore-qt-widget-events.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQTabWidget(QoreObject *obj, QWidget* parent = 0) : QTabWidget(parent), QoreQWidgetExtension(obj->getClass())
      {
         init(obj);
      }

      DLLLOCAL void parent_tabInserted ( int index )
      {
	 QTabWidget::tabInserted(index);
      }

      DLLLOCAL void parent_tabRemoved ( int index )
      {
	 QTabWidget::tabRemoved(index);
      }

};

class QoreQTabWidget : public QoreAbstractQWidget
{
   public:
      QPointer<myQTabWidget> qobj;

      DLLLOCAL QoreQTabWidget(QoreObject *obj, QWidget* parent = 0) : qobj(new myQTabWidget(obj, parent))
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

      DLLLOCAL void initStyleOption ( QStyleOptionTabWidgetFrame * option ) const
      {
	 qobj->initStyleOption(option);
      }
      DLLLOCAL void setTabBar ( QTabBar * tb )
      {
	 qobj->setTabBar(tb);
      }
      DLLLOCAL QTabBar * tabBar () const
      {
	 return qobj->tabBar();
      }
      DLLLOCAL void tabInserted ( int index )
      {
	 qobj->parent_tabInserted(index);
      }
      DLLLOCAL void tabRemoved ( int index )
      {
	 qobj->parent_tabRemoved(index);
      }

      QORE_VIRTUAL_QWIDGET_METHODS
};

#endif // _QORE_QT_QC_QTABWIDGET_H
