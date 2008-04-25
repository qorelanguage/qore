/*
 QC_QAbstractButton.h
 
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

#ifndef _QORE_QT_QC_QABSTRACTBUTTON_H

#define _QORE_QT_QC_QABSTRACTBUTTON_H

#include <QAbstractButton>
#include "QoreAbstractQAbstractButton.h"
#include "qore-qt-events.h"

DLLLOCAL extern qore_classid_t CID_QABSTRACTBUTTON;
DLLLOCAL extern class QoreClass *QC_QAbstractButton;

DLLLOCAL class QoreClass *initQAbstractButtonClass(QoreClass *);

class myQAbstractButton : public QAbstractButton, public QoreQWidgetExtension
{
#define QOREQTYPE QAbstractButton
#define MYQOREQTYPE myQAbstractButton
#include "qore-qt-metacode.h"
#include "qore-qt-widget-events.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
   DLLLOCAL myQAbstractButton(QoreObject *obj, QWidget* parent = 0) : QAbstractButton(parent), QoreQWidgetExtension(obj, this)
      {
         
         //init_widget_events();
      }
};

typedef QoreQAbstractButtonBase<myQAbstractButton, QoreAbstractQAbstractButton> QoreQAbstractButtonImpl;

class QoreQAbstractButton : public QoreQAbstractButtonImpl
{
   public:
      DLLLOCAL QoreQAbstractButton(QoreObject *obj, QWidget* parent = 0) : QoreQAbstractButtonImpl(new myQAbstractButton(obj, parent))
      {
      }
};

typedef QoreQtQAbstractButtonBase<QAbstractButton, QoreAbstractQAbstractButton> QoreQtQAbstractButtonImpl;

class QoreQtQAbstractButton : public QoreQtQAbstractButtonImpl
{
   public:
      DLLLOCAL QoreQtQAbstractButton(QoreObject *obj, QAbstractButton *b) : QoreQtQAbstractButtonImpl(obj, b)
      {
      }
};

#endif // _QORE_QT_QC_QABSTRACTBUTTON_H
