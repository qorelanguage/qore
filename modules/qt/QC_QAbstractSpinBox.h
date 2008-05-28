/*
 QC_QAbstractSpinBox.h
 
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

#ifndef _QORE_QT_QC_QABSTRACTSPINBOX_H

#define _QORE_QT_QC_QABSTRACTSPINBOX_H

#include <QAbstractSpinBox>
#include "QoreAbstractQAbstractSpinBox.h"
#include "qore-qt-events.h"

DLLEXPORT extern qore_classid_t CID_QABSTRACTSPINBOX;
DLLEXPORT extern QoreClass *QC_QAbstractSpinBox;

DLLEXPORT class QoreClass *initQAbstractSpinBoxClass(QoreClass *);

class myQAbstractSpinBox : public QAbstractSpinBox, public QoreQWidgetExtension
{
#define QOREQTYPE QAbstractSpinBox
#define MYQOREQTYPE myQAbstractSpinBox
#include "qore-qt-metacode.h"
#include "qore-qt-widget-events.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQAbstractSpinBox(QoreObject *obj, QWidget* parent = 0) : QAbstractSpinBox(parent), QoreQWidgetExtension(obj, this)
      {
      }
};

typedef QoreQAbstractSpinBoxBase<myQAbstractSpinBox, QoreAbstractQAbstractSpinBox> QoreQAbstractSpinBoxImpl;

class QoreQAbstractSpinBox : public QoreQAbstractSpinBoxImpl
{
   public:
      DLLLOCAL QoreQAbstractSpinBox(QoreObject *obj, QWidget* parent = 0) : QoreQAbstractSpinBoxImpl(new myQAbstractSpinBox(obj, parent))
      {
      }
};

#endif // _QORE_QT_QC_QABSTRACTSPINBOX_H
