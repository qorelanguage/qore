/*
 QC_QComboBox.h
 
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

#ifndef _QORE_QT_QC_QCOMBOBOX_H

#define _QORE_QT_QC_QCOMBOBOX_H

#include <QComboBox>
#include "QoreAbstractQComboBox.h"
#include "qore-qt-events.h"

DLLLOCAL extern qore_classid_t CID_QCOMBOBOX;
DLLLOCAL extern class QoreClass *QC_QComboBox;

DLLLOCAL class QoreClass *initQComboBoxClass(QoreClass *);

class myQComboBox : public QComboBox, public QoreQWidgetExtension
{
#define QOREQTYPE QComboBox
#define MYQOREQTYPE myQComboBox
#include "qore-qt-metacode.h"
#include "qore-qt-widget-events.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQComboBox(QoreObject *obj, QWidget* parent = 0) : QComboBox(parent), QoreQWidgetExtension(obj, this)
      {
         
      }
};

typedef QoreQComboBoxBase<myQComboBox, QoreAbstractQComboBox> QoreQComboBoxImpl;

class QoreQComboBox : public QoreQComboBoxImpl
{
   public:
      DLLLOCAL QoreQComboBox(QoreObject *obj, QWidget* parent = 0) : QoreQComboBoxImpl(new myQComboBox(obj, parent))
      {
      }
};

#endif // _QORE_QT_QC_QCOMBOBOX_H
