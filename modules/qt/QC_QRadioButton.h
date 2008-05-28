/*
 QC_QRadioButton.h
 
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

#ifndef _QORE_QT_QC_QRADIOBUTTON_H

#define _QORE_QT_QC_QRADIOBUTTON_H

#include <QRadioButton>
#include "QoreAbstractQRadioButton.h"
#include "qore-qt-events.h"

DLLEXPORT extern qore_classid_t CID_QRADIOBUTTON;
DLLEXPORT extern class QoreClass *QC_QRadioButton;

DLLEXPORT class QoreClass *initQRadioButtonClass(QoreClass *);

class myQRadioButton : public QRadioButton, public QoreQWidgetExtension
{
#define QOREQTYPE QRadioButton
#define MYQOREQTYPE myQRadioButton
#include "qore-qt-metacode.h"
#include "qore-qt-widget-events.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQRadioButton(QoreObject *obj, QWidget* parent = 0) : QRadioButton(parent), QoreQWidgetExtension(obj, this)
      {
         
      }
      DLLLOCAL myQRadioButton(QoreObject *obj, const QString& text, QWidget* parent = 0) : QRadioButton(text, parent), QoreQWidgetExtension(obj, this)
      {
         
      }
};

typedef QoreQRadioButtonBase<myQRadioButton, QoreAbstractQRadioButton> QoreQRadioButtonImpl;

class QoreQRadioButton : public QoreQRadioButtonImpl
{
   public:
      DLLLOCAL QoreQRadioButton(QoreObject *obj, QWidget* parent = 0) : QoreQRadioButtonImpl(new myQRadioButton(obj, parent))
      {
      }
      DLLLOCAL QoreQRadioButton(QoreObject *obj, const QString& text, QWidget* parent = 0) : QoreQRadioButtonImpl(new myQRadioButton(obj, text, parent))
      {
      }
};

typedef QoreQtQRadioButtonBase<QRadioButton, QoreAbstractQRadioButton> QoreQtQRadioButtonImpl;

class QoreQtQRadioButton : public QoreQtQRadioButtonImpl
{
   public:
      DLLLOCAL QoreQtQRadioButton(QoreObject *obj, QRadioButton *qb) : QoreQtQRadioButtonImpl(obj, qb)
      {
      }
};

#endif // _QORE_QT_QC_QRADIOBUTTON_H
