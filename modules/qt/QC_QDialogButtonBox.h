/*
 QC_QDialogButtonBox.h
 
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

#ifndef _QORE_QT_QC_QDIALOGBUTTONBOX_H

#define _QORE_QT_QC_QDIALOGBUTTONBOX_H

#include <QDialogButtonBox>
#include "QoreAbstractQWidget.h"
#include "qore-qt-events.h"

DLLLOCAL extern qore_classid_t CID_QDIALOGBUTTONBOX;
DLLLOCAL extern QoreClass *QC_QDialogButtonBox;
DLLLOCAL QoreNamespace *initQDialogButtonBoxNS(QoreClass *);

class myQDialogButtonBox : public QDialogButtonBox, public QoreQWidgetExtension
{
#define QOREQTYPE QDialogButtonBox
#define MYQOREQTYPE myQDialogButtonBox
#include "qore-qt-metacode.h"
#include "qore-qt-widget-events.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQDialogButtonBox(QoreObject *obj, QWidget* parent = 0) : QDialogButtonBox(parent), QoreQWidgetExtension(obj, this)
      {
         
      }
      DLLLOCAL myQDialogButtonBox(QoreObject *obj, Qt::Orientation orientation, QWidget* parent = 0) : QDialogButtonBox(orientation, parent), QoreQWidgetExtension(obj, this)
      {
         
      }
      DLLLOCAL myQDialogButtonBox(QoreObject *obj, StandardButtons buttons, Qt::Orientation orientation = Qt::Horizontal, QWidget* parent = 0) : QDialogButtonBox(buttons, orientation, parent), QoreQWidgetExtension(obj, this)
      {
         
      }
};

typedef QoreQWidgetBase<myQDialogButtonBox, QoreAbstractQWidget> QoreQDialogButtonBoxImpl;

class QoreQDialogButtonBox : public QoreQDialogButtonBoxImpl
{
   public:
      DLLLOCAL QoreQDialogButtonBox(QoreObject *obj, QWidget* parent = 0) : QoreQDialogButtonBoxImpl(new myQDialogButtonBox(obj, parent))
      {
      }
      DLLLOCAL QoreQDialogButtonBox(QoreObject *obj, Qt::Orientation orientation, QWidget* parent = 0) : QoreQDialogButtonBoxImpl(new myQDialogButtonBox(obj, orientation, parent))
      {
      }
      DLLLOCAL QoreQDialogButtonBox(QoreObject *obj, QDialogButtonBox::StandardButtons buttons, Qt::Orientation orientation = Qt::Horizontal, QWidget* parent = 0) : QoreQDialogButtonBoxImpl(new myQDialogButtonBox(obj, buttons, orientation, parent))
      {
      }
};

#endif // _QORE_QT_QC_QDIALOGBUTTONBOX_H
