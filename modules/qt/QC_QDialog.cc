/*
 QC_QDialog.cc
 
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

#include "QC_QDialog.h"
#include "QC_QWidget.h"

#include "qore-qt.h"

int CID_QDIALOG;
class QoreClass *QC_QDialog = 0;

//QDialog ( QWidget * parent = 0, Qt::WindowFlags f = 0 )
static void QDIALOG_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   QoreQWidget *parent = (p && p->type == NT_OBJECT) ? (QoreQWidget *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return;
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   p = get_param(params, 1);
   Qt::WindowFlags f = (Qt::WindowFlags)(!is_nothing(p) ? p->getAsInt() : 0);
   self->setPrivate(CID_QDIALOG, new QoreQDialog(self, parent ? static_cast<QWidget *>(parent->getQWidget()) : 0, f));
   return;
}

static void QDIALOG_copy(class QoreObject *self, class QoreObject *old, class QoreQDialog *qd, ExceptionSink *xsink)
{
   xsink->raiseException("QDIALOG-COPY-ERROR", "objects of this class cannot be copied");
}

//bool isSizeGripEnabled () const
static AbstractQoreNode *QDIALOG_isSizeGripEnabled(QoreObject *self, QoreAbstractQDialog *qd, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qd->getQDialog()->isSizeGripEnabled());
}

//int result () const
static AbstractQoreNode *QDIALOG_result(QoreObject *self, QoreAbstractQDialog *qd, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qd->getQDialog()->result());
}

//void setModal ( bool modal )
static AbstractQoreNode *QDIALOG_setModal(QoreObject *self, QoreAbstractQDialog *qd, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   bool modal = p ? p->getAsBool() : false;
   qd->getQDialog()->setModal(modal);
   return 0;
}

//void setResult ( int i )
static AbstractQoreNode *QDIALOG_setResult(QoreObject *self, QoreAbstractQDialog *qd, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   int i = p ? p->getAsInt() : 0;
   qd->getQDialog()->setResult(i);
   return 0;
}

//void setSizeGripEnabled ( bool )
static AbstractQoreNode *QDIALOG_setSizeGripEnabled(QoreObject *self, QoreAbstractQDialog *qd, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   bool b = p ? p->getAsBool() : false;
   qd->getQDialog()->setSizeGripEnabled(b);
   return 0;
}

//virtual void accept ()
static AbstractQoreNode *QDIALOG_accept(QoreObject *self, QoreAbstractQDialog *qd, const QoreListNode *params, ExceptionSink *xsink)
{
   qd->accept();
   return 0;
}

//virtual void done ( int r )
static AbstractQoreNode *QDIALOG_done(QoreObject *self, QoreAbstractQDialog *qd, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   int r = p ? p->getAsInt() : 0;
   qd->done(r);
   return 0;
}

//int exec ()
static AbstractQoreNode *QDIALOG_exec(QoreObject *self, QoreAbstractQDialog *qd, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qd->getQDialog()->exec());
}

//virtual void reject ()
static AbstractQoreNode *QDIALOG_reject(QoreObject *self, QoreAbstractQDialog *qd, const QoreListNode *params, ExceptionSink *xsink)
{
   qd->reject();
   return 0;
}

QoreClass *initQDialogClass(QoreClass *qwidget)
{
   QC_QDialog = new QoreClass("QDialog", QDOM_GUI);
   CID_QDIALOG = QC_QDialog->getID();

   QC_QDialog->addBuiltinVirtualBaseClass(qwidget);

   QC_QDialog->setConstructor(QDIALOG_constructor);
   QC_QDialog->setCopy((q_copy_t)QDIALOG_copy);

   QC_QDialog->addMethod("isSizeGripEnabled",           (q_method_t)QDIALOG_isSizeGripEnabled);
   QC_QDialog->addMethod("result",                      (q_method_t)QDIALOG_result);
   QC_QDialog->addMethod("setModal",                    (q_method_t)QDIALOG_setModal);
   QC_QDialog->addMethod("setResult",                   (q_method_t)QDIALOG_setResult);
   QC_QDialog->addMethod("setSizeGripEnabled",          (q_method_t)QDIALOG_setSizeGripEnabled);
   QC_QDialog->addMethod("accept",                      (q_method_t)QDIALOG_accept);
   QC_QDialog->addMethod("done",                        (q_method_t)QDIALOG_done);
   QC_QDialog->addMethod("exec",                        (q_method_t)QDIALOG_exec);
   QC_QDialog->addMethod("reject",                      (q_method_t)QDIALOG_reject);

   return QC_QDialog;
}
