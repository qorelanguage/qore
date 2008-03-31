/*
 QC_QDialog.h
 
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

#ifndef _QORE_QT_QC_QDIALOG_H

#define _QORE_QT_QC_QDIALOG_H

#include <QDialog>
#include "QoreAbstractQDialog.h"
#include "qore-qt-events.h"

DLLLOCAL extern qore_classid_t CID_QDIALOG;
DLLLOCAL extern class QoreClass *QC_QDialog;

DLLLOCAL class QoreClass *initQDialogClass(QoreClass *);

class myQDialog : public QDialog, public QoreQDialogExtension
{
#define QOREQTYPE QDialog
#define MYQOREQTYPE myQDialog
#include "qore-qt-metacode.h"
#include "qore-qt-qdialog-methods.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQDialog(QoreObject *obj, QWidget* parent = 0, Qt::WindowFlags f = 0) : QDialog(parent, f), QoreQDialogExtension(obj, this)
      {
         
      }
};

typedef QoreQDialogBase<myQDialog, QoreAbstractQDialog> QoreQDialogImpl;

class QoreQDialog : public QoreQDialogImpl
{
   public:
      DLLLOCAL QoreQDialog(QoreObject *obj, QWidget* parent = 0, Qt::WindowFlags f = 0) : QoreQDialogImpl(new myQDialog(obj, parent, f))
      {
      }
};

#endif // _QORE_QT_QC_QDIALOG_H
