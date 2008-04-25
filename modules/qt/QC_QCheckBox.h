/*
 QC_QCheckBox.h
 
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

#ifndef _QORE_QT_QC_QCHECKBOX_H

#define _QORE_QT_QC_QCHECKBOX_H

#include <QCheckBox>
#include "QoreAbstractQCheckBox.h"
#include "qore-qt-events.h"

DLLLOCAL extern qore_classid_t CID_QCHECKBOX;
DLLLOCAL extern class QoreClass *QC_QCheckBox;

DLLLOCAL class QoreClass *initQCheckBoxClass(QoreClass *);

class myQCheckBox : public QCheckBox, public QoreQWidgetExtension
{
#define QOREQTYPE QCheckBox
#define MYQOREQTYPE myQCheckBox
#include "qore-qt-metacode.h"
#include "qore-qt-widget-events.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQCheckBox(QoreObject *obj, QWidget* parent = 0) : QCheckBox(parent), QoreQWidgetExtension(obj, this)
      {
         
      }
      DLLLOCAL myQCheckBox(QoreObject *obj, const QString& text, QWidget* parent = 0) : QCheckBox(text, parent), QoreQWidgetExtension(obj, this)
      {
         
      }
};

typedef QoreQCheckBoxBase<myQCheckBox, QoreAbstractQCheckBox> QoreQCheckBoxImpl;

class QoreQCheckBox : public QoreQCheckBoxImpl
{
   public:
      DLLLOCAL QoreQCheckBox(QoreObject *obj, QWidget* parent = 0) : QoreQCheckBoxImpl(new myQCheckBox(obj, parent))
      {
      }
      DLLLOCAL QoreQCheckBox(QoreObject *obj, const QString& text, QWidget* parent = 0) : QoreQCheckBoxImpl(new myQCheckBox(obj, text, parent))
      {
      }
};

typedef QoreQtQCheckBoxBase<QCheckBox, QoreAbstractQCheckBox> QoreQtQCheckBoxImpl;

class QoreQtQCheckBox : public QoreQtQCheckBoxImpl
{
   public:
      DLLLOCAL QoreQtQCheckBox(QoreObject *obj, QCheckBox *cb) : QoreQtQCheckBoxImpl(obj, cb)
      {
      }
      DLLLOCAL ~QoreQtQCheckBox()
      {
      }
};

#endif // _QORE_QT_QC_QCHECKBOX_H
