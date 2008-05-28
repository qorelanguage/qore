/*
 QC_QGroupBox.h
 
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

#ifndef _QORE_QT_QC_QGROUPBOX_H

#define _QORE_QT_QC_QGROUPBOX_H

#include <QGroupBox>
#include "QoreAbstractQGroupBox.h"
#include "qore-qt-events.h"

DLLEXPORT extern qore_classid_t CID_QGROUPBOX;
DLLEXPORT extern class QoreClass *QC_QGroupBox;

DLLEXPORT class QoreClass *initQGroupBoxClass(QoreClass *);

class myQGroupBox : public QGroupBox, public QoreQWidgetExtension
{
#define QOREQTYPE QGroupBox
#define MYQOREQTYPE myQGroupBox
#include "qore-qt-metacode.h"
#include "qore-qt-widget-events.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQGroupBox(QoreObject *obj, QWidget* parent = 0) : QGroupBox(parent), QoreQWidgetExtension(obj, this)
      {
         
      }
      DLLLOCAL myQGroupBox(QoreObject *obj, const QString& title, QWidget* parent = 0) : QGroupBox(title, parent), QoreQWidgetExtension(obj, this)
      {
         
      }
};

typedef QoreQGroupBoxBase<myQGroupBox, QoreAbstractQGroupBox> QoreQGroupBoxImpl;

class QoreQGroupBox : public QoreQGroupBoxImpl
{
   public:
      DLLLOCAL QoreQGroupBox(QoreObject *obj, QWidget* parent = 0) : QoreQGroupBoxImpl(new myQGroupBox(obj, parent))
      {
      }
      DLLLOCAL QoreQGroupBox(QoreObject *obj, const QString& title, QWidget* parent = 0) : QoreQGroupBoxImpl(new myQGroupBox(obj, title, parent))
      {
      }
};

#endif // _QORE_QT_QC_QGROUPBOX_H
