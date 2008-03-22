/*
 QC_QTabBar.h
 
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

#ifndef _QORE_QT_QC_QTABBAR_H

#define _QORE_QT_QC_QTABBAR_H

#include <QTabBar>
#include "QoreAbstractQTabBar.h"
#include "qore-qt-events.h"

DLLLOCAL extern qore_classid_t CID_QTABBAR;
DLLLOCAL extern class QoreClass *QC_QTabBar;

DLLLOCAL class QoreClass *initQTabBarClass(QoreClass *);

class myQTabBar : public QTabBar, public QoreQWidgetExtension
{
#define QOREQTYPE QTabBar
#define MYQOREQTYPE myQTabBar
#include "qore-qt-metacode.h"
#include "qore-qt-widget-events.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQTabBar(QoreObject *obj, QWidget* parent = 0) : QTabBar(parent), QoreQWidgetExtension(obj->getClass())
      {
         init(obj);
      }

      void parent_initStyleOption ( QStyleOptionTab * option, int tabIndex ) const
      {
         QTabBar::initStyleOption(option, tabIndex);
      }

      void parent_tabInserted ( int index )
      {
         QTabBar::tabInserted(index);
      }

      void parent_tabLayoutChange ()
      {
         QTabBar::tabLayoutChange();
      }

      void parent_tabRemoved ( int index )
      {
         QTabBar::tabRemoved(index);
      }

      QSize parent_tabSizeHint ( int index ) const
      {
         return QTabBar::tabSizeHint(index);
      }
};

class QoreQTabBar : public QoreAbstractQTabBar
{
   public:
      QPointer<myQTabBar> qobj;

      DLLLOCAL QoreQTabBar(QoreObject *obj, QWidget* parent = 0) : qobj(new myQTabBar(obj, parent))
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
      DLLLOCAL virtual class QTabBar *getQTabBar() const
      {
         return static_cast<QTabBar *>(&(*qobj));
      }

      DLLLOCAL virtual void initStyleOption ( QStyleOptionTab * option, int tabIndex ) const
      {
         qobj->parent_initStyleOption(option, tabIndex);
      }

      DLLLOCAL virtual void tabInserted ( int index )
      {
         qobj->parent_tabInserted(index);
      }

      DLLLOCAL virtual void tabLayoutChange ()
      {
         qobj->parent_tabLayoutChange();
      }

      DLLLOCAL virtual void tabRemoved ( int index )
      {
         qobj->parent_tabRemoved(index);
      }

      DLLLOCAL QSize virtual tabSizeHint ( int index ) const
      {
         return qobj->parent_tabSizeHint(index);
      }

      QORE_VIRTUAL_QWIDGET_METHODS
};

class QoreQtQTabBar : public QoreAbstractQTabBar
{
   public:
      QoreObject *qore_obj;
      QPointer<QTabBar> qobj;

      DLLLOCAL QoreQtQTabBar(QoreObject *obj, QTabBar *qtabbar) : qore_obj(obj), qobj(qtabbar)
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
      DLLLOCAL virtual class QTabBar *getQTabBar() const
      {
         return static_cast<QTabBar *>(&(*qobj));
      }
#include "qore-qt-static-qwidget-methods.h"

      // these functions will never be called
      DLLLOCAL virtual void initStyleOption ( QStyleOptionTab * option, int tabIndex ) const
      {
      }
      DLLLOCAL virtual void tabInserted ( int index )
      {
      }
      DLLLOCAL virtual void tabLayoutChange ()
      {
      }
      DLLLOCAL virtual void tabRemoved ( int index )
      {
      }
      DLLLOCAL virtual QSize tabSizeHint ( int index ) const
      {
	 return QSize();
      }
};

#endif // _QORE_QT_QC_QTABBAR_H
