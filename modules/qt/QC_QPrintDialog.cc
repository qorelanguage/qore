/*
 QC_QPrintDialog.cc
 
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

#include <qore/Qore.h>

#include "QC_QPrintDialog.h"
#include "QC_QPrinter.h"
#include "QC_QWidget.h"

#include "qore-qt.h"

qore_classid_t CID_QPRINTDIALOG;
class QoreClass *QC_QPrintDialog = 0;

//QPrintDialog ( QPrinter * printer, QWidget * parent = 0 )
static void QPRINTDIALOG_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreObject *o = test_object_param(params, 0);
   QoreQPrinter *printer = o ? (QoreQPrinter *)o->getReferencedPrivateData(CID_QPRINTER, xsink) : 0;
   if (!printer) {
      if (!xsink->isException())
         xsink->raiseException("QPRINTDIALOG-CONSTRUCTOR-PARAM-ERROR", "expecting a QPrinter object as first argument to QPrintDialog::constructor()");
      return;
   }
   ReferenceHolder<AbstractPrivateData> printerHolder(static_cast<AbstractPrivateData *>(printer), xsink);

   o = test_object_param(params, 1);
   QoreQWidget *parent = o ? (QoreQWidget *)o->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return;
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   self->setPrivate(CID_QPRINTDIALOG, new QoreQPrintDialog(self, static_cast<QPrinter *>(printer), parent ? static_cast<QWidget *>(parent->getQWidget()) : 0));
   return;
}

static void QPRINTDIALOG_copy(class QoreObject *self, class QoreObject *old, class QoreQPrintDialog *qpd, ExceptionSink *xsink)
{
   xsink->raiseException("QPRINTDIALOG-COPY-ERROR", "objects of this class cannot be copied");
}

QoreClass *initQPrintDialogClass(QoreClass *qdialog)
{
   QC_QPrintDialog = new QoreClass("QPrintDialog", QDOM_GUI);
   CID_QPRINTDIALOG = QC_QPrintDialog->getID();

   QC_QPrintDialog->addBuiltinVirtualBaseClass(qdialog);

   QC_QPrintDialog->setConstructor(QPRINTDIALOG_constructor);
   QC_QPrintDialog->setCopy((q_copy_t)QPRINTDIALOG_copy);


   return QC_QPrintDialog;
}
