/*
 QC_QMessageBox.h
 
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

#ifndef _QORE_QT_QC_QMESSAGEBOX_H

#define _QORE_QT_QC_QMESSAGEBOX_H

#include <QMessageBox>
#include "QoreAbstractQDialog.h"
#include "qore-qt-events.h"

DLLLOCAL extern qore_classid_t CID_QMESSAGEBOX;
DLLLOCAL extern class QoreClass *QC_QMessageBox;

DLLLOCAL class QoreClass *initQMessageBoxClass(QoreClass *);
DLLLOCAL void initQMessageBoxStaticFunctions();

class myQMessageBox : public QMessageBox, public QoreQDialogExtension
{
#define QOREQTYPE QMessageBox
#define MYQOREQTYPE myQMessageBox
#include "qore-qt-metacode.h"
#include "qore-qt-qdialog-methods.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQMessageBox(QoreObject *obj, QWidget* parent = 0) : QMessageBox(parent), QoreQDialogExtension(obj, this)
      {
         
      }
      DLLLOCAL myQMessageBox(QoreObject *obj, Icon icon, const QString& title, const QString& text, StandardButtons buttons = NoButton, QWidget* parent = 0, Qt::WindowFlags f = Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint) : QMessageBox(icon, title, text, buttons, parent, f), QoreQDialogExtension(obj, this)
      {
         
      }
};

typedef QoreQDialogBase<myQMessageBox, QoreAbstractQDialog> QoreQMessageBoxImpl;

class QoreQMessageBox : public QoreQMessageBoxImpl
{
   public:
      DLLLOCAL QoreQMessageBox(QoreObject *obj, QWidget* parent = 0) : QoreQMessageBoxImpl(new myQMessageBox(obj, parent))
      {
      }
      DLLLOCAL QoreQMessageBox(QoreObject *obj, QMessageBox::Icon icon, const QString& title, const QString& text, QMessageBox::StandardButtons buttons = QMessageBox::NoButton, QWidget* parent = 0, Qt::WindowFlags f = Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint) : QoreQMessageBoxImpl(new myQMessageBox(obj, icon, title, text, buttons, parent, f))
      {
      }
};

#endif // _QORE_QT_QC_QMESSAGEBOX_H
