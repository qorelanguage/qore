/*
 QC_QScrollBar.h
 
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

#ifndef _QORE_QT_QC_QSCROLLBAR_H

#define _QORE_QT_QC_QSCROLLBAR_H

#include <QScrollBar>
#include "QoreAbstractQScrollBar.h"
#include "qore-qt-events.h"

DLLEXPORT extern qore_classid_t CID_QSCROLLBAR;
DLLEXPORT extern class QoreClass *QC_QScrollBar;

DLLEXPORT class QoreClass *initQScrollBarClass(QoreClass *);

class myQScrollBar : public QScrollBar, public QoreQWidgetExtension
{
#define QOREQTYPE QScrollBar
#define MYQOREQTYPE myQScrollBar
#include "qore-qt-metacode.h"
#include "qore-qt-widget-events.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQScrollBar(QoreObject *obj, QWidget* parent = 0) : QScrollBar(parent), QoreQWidgetExtension(obj, this)
      {
         
      }
      DLLLOCAL myQScrollBar(QoreObject *obj, Qt::Orientation orientation, QWidget* parent = 0) : QScrollBar(orientation, parent), QoreQWidgetExtension(obj, this)
      {
         
      }
};

typedef QoreQScrollBarBase<myQScrollBar, QoreAbstractQScrollBar> QoreQScrollBarImpl;

class QoreQScrollBar : public QoreQScrollBarImpl
{
   public:
      DLLLOCAL QoreQScrollBar(QoreObject *obj, QWidget* parent = 0) : QoreQScrollBarImpl(new myQScrollBar(obj, parent))
      {
      }
      DLLLOCAL QoreQScrollBar(QoreObject *obj, Qt::Orientation orientation, QWidget* parent = 0) : QoreQScrollBarImpl(new myQScrollBar(obj, orientation, parent))
      {
      }
};

typedef QoreQtQScrollBarBase<QScrollBar, QoreAbstractQScrollBar> QoreQtQScrollBarImpl;

class QoreQtQScrollBar : public QoreQtQScrollBarImpl
{
   public:
      DLLLOCAL QoreQtQScrollBar(QoreObject *obj, QScrollBar *qscrollbar) : QoreQtQScrollBarImpl(obj, qscrollbar)
      {
      }
};

#endif // _QORE_QT_QC_QSCROLLBAR_H
