/*
 QC_QPushButton.h
 
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

#ifndef _QORE_QT_QC_QPUSHBUTTON_H

#define _QORE_QT_QC_QPUSHBUTTON_H

#include <QPushButton>
#include "QoreAbstractQPushButton.h"
#include "qore-qt-events.h"

DLLEXPORT extern qore_classid_t CID_QPUSHBUTTON;
DLLEXPORT extern class QoreClass *QC_QPushButton;

DLLEXPORT class QoreClass *initQPushButtonClass(QoreClass *);

class myQPushButton : public QPushButton, public QoreQWidgetExtension
{
      friend class QoreQPushbutton;

#define QOREQTYPE QPushButton
#define MYQOREQTYPE myQPushButton
#include "qore-qt-metacode.h"
#include "qore-qt-widget-events.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQPushButton(QoreObject *obj, QWidget* parent = 0) : QPushButton(parent), QoreQWidgetExtension(obj, this)
      {
         
      }
      DLLLOCAL myQPushButton(QoreObject *obj, const QString& text, QWidget* parent = 0) : QPushButton(text, parent), QoreQWidgetExtension(obj, this)
      {
         
      }
      DLLLOCAL myQPushButton(QoreObject *obj, const QIcon& icon, const QString& text, QWidget* parent = 0) : QPushButton(icon, text, parent), QoreQWidgetExtension(obj, this)
      {
         
      }
};

typedef QoreQPushButtonBase<myQPushButton, QoreAbstractQPushButton> QoreQPushButtonImpl;

class QoreQPushButton : public QoreQPushButtonImpl
{
   public:
      DLLLOCAL QoreQPushButton(QoreObject *obj, QWidget* parent = 0) : QoreQPushButtonImpl(new myQPushButton(obj, parent))
      {
      }
      DLLLOCAL QoreQPushButton(QoreObject *obj, const QString& text, QWidget* parent = 0) : QoreQPushButtonImpl(new myQPushButton(obj, text, parent))
      {
      }
      DLLLOCAL QoreQPushButton(QoreObject *obj, const QIcon& icon, const QString& text, QWidget* parent = 0) : QoreQPushButtonImpl(new myQPushButton(obj, icon, text, parent))
      {
      }
};

typedef QoreQtQPushButtonBase<QPushButton, QoreAbstractQPushButton> QoreQtQPushButtonImpl;

class QoreQtQPushButton : public QoreQtQPushButtonImpl
{
   public:
      DLLLOCAL QoreQtQPushButton(QoreObject *obj, QPushButton *qpushbutton) : QoreQtQPushButtonImpl(obj, qpushbutton)
      {
      }
};

#endif // _QORE_QT_QC_QPUSHBUTTON_H
