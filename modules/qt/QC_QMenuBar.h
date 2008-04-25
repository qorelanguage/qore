/*
 QC_QMenuBar.h
 
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

#ifndef _QORE_QT_QC_QMENUBAR_H

#define _QORE_QT_QC_QMENUBAR_H

#include <QMenuBar>
#include "QoreAbstractQMenuBar.h"
#include "qore-qt-events.h"

DLLLOCAL extern qore_classid_t CID_QMENUBAR;
DLLLOCAL extern class QoreClass *QC_QMenuBar;

DLLLOCAL class QoreClass *initQMenuBarClass(QoreClass *);

class myQMenuBar : public QMenuBar, public QoreQWidgetExtension
{
#define QOREQTYPE QMenuBar
#define MYQOREQTYPE myQMenuBar
#include "qore-qt-metacode.h"
#include "qore-qt-widget-events.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQMenuBar(QoreObject *obj, QWidget* parent = 0) : QMenuBar(parent), QoreQWidgetExtension(obj, this)
      {  
      }
};

typedef QoreQMenuBarBase<myQMenuBar, QoreAbstractQMenuBar> QoreQMenuBarImpl;

class QoreQMenuBar : public QoreQMenuBarImpl
{
   public:
      DLLLOCAL QoreQMenuBar(QoreObject *obj, QWidget* parent = 0) : QoreQMenuBarImpl(new myQMenuBar(obj, parent))
      {
      }
};

typedef QoreQtQMenuBarBase<QMenuBar, QoreAbstractQMenuBar> QoreQtQMenuBarImpl;

class QoreQtQMenuBar : public QoreQtQMenuBarImpl
{
   public:
      DLLLOCAL QoreQtQMenuBar(QoreObject *obj, QMenuBar *qmenubar) : QoreQtQMenuBarImpl(obj, qmenubar)
      {
      }
};

#endif // _QORE_QT_QC_QMENUBAR_H
