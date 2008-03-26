/*
 QC_QToolButton.h
 
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

#ifndef _QORE_QT_QC_QTOOLBUTTON_H

#define _QORE_QT_QC_QTOOLBUTTON_H

#include <QToolButton>
#include "QoreAbstractQToolButton.h"
#include "qore-qt-events.h"

DLLLOCAL extern qore_classid_t CID_QTOOLBUTTON;
DLLLOCAL extern class QoreClass *QC_QToolButton;

DLLLOCAL class QoreClass *initQToolButtonClass(QoreClass *);

class myQToolButton : public QToolButton, public QoreQWidgetExtension
{
#define QOREQTYPE QToolButton
#define MYQOREQTYPE myQToolButton
#include "qore-qt-metacode.h"
#include "qore-qt-widget-events.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQToolButton(QoreObject *obj, QWidget* parent = 0) : QToolButton(parent), QoreQWidgetExtension(obj, this)
      {
         
      }
};

typedef QoreQToolButtonBase<myQToolButton, QoreAbstractQToolButton> QoreQToolButtonImpl;

class QoreQToolButton : public QoreQToolButtonImpl
{
   public:
      DLLLOCAL QoreQToolButton(QoreObject *obj, QWidget* parent = 0) : QoreQToolButtonImpl(new myQToolButton(obj, parent))
      {
      }
};

typedef QoreQtQToolButtonBase<QToolButton, QoreAbstractQToolButton> QoreQtQToolButtonImpl;

class QoreQtQToolButton : public QoreQtQToolButtonImpl
{
   public:
      DLLLOCAL QoreQtQToolButton(QoreObject *obj, QToolButton *qtoolbutton) : QoreQtQToolButtonImpl(obj, qtoolbutton)
      {
      }
};

#endif // _QORE_QT_QC_QTOOLBUTTON_H
