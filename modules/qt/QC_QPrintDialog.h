/*
 QC_QPrintDialog.h
 
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

#ifndef _QORE_QT_QC_QPRINTDIALOG_H

#define _QORE_QT_QC_QPRINTDIALOG_H

#include <QPrintDialog>
#include "QoreAbstractQDialog.h"
#include "qore-qt-events.h"

DLLLOCAL extern qore_classid_t CID_QPRINTDIALOG;
DLLLOCAL extern class QoreClass *QC_QPrintDialog;

DLLLOCAL class QoreClass *initQPrintDialogClass(QoreClass *);

class myQPrintDialog : public QPrintDialog, public QoreQDialogExtension
{
#define QOREQTYPE QPrintDialog
#define MYQOREQTYPE myQPrintDialog
#include "qore-qt-metacode.h"
#include "qore-qt-qdialog-methods.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQPrintDialog(QoreObject *obj, QPrinter* printer, QWidget* parent = 0) : QPrintDialog(printer, parent), QoreQDialogExtension(obj, this)
      {
         
      }
};

typedef QoreQDialogBase<myQPrintDialog, QoreAbstractQDialog> QoreQPrintDialogImpl;

class QoreQPrintDialog : public QoreQPrintDialogImpl
{
   public:
      DLLLOCAL QoreQPrintDialog(QoreObject *obj, QPrinter* printer, QWidget* parent = 0) : QoreQPrintDialogImpl(new myQPrintDialog(obj, printer, parent))
      {
      }
};

#endif // _QORE_QT_QC_QPRINTDIALOG_H
