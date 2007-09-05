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

int CID_QDIALOG;
class QoreClass *QC_QDialog = 0;

//QDialog ( QWidget * parent = 0, Qt::WindowFlags f = 0 )
static void QDIALOG_constructor(Object *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreAbstractQWidget *parent = (p && p->type == NT_OBJECT) ? (QoreAbstractQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   ReferenceHolder<QoreAbstractQWidget> parentHolder(parent, xsink);
   p = get_param(params, 1);
   Qt::WindowFlags f = (Qt::WindowFlags)(p ? p->getAsInt() : 0);
   self->setPrivate(CID_QDIALOG, new QoreQDialog(self, parent ? parent->getQWidget() : 0, f));
   return;
}

static void QDIALOG_copy(class Object *self, class Object *old, class QoreQDialog *qd, ExceptionSink *xsink)
{
   xsink->raiseException("QDIALOG-COPY-ERROR", "objects of this class cannot be copied");
}

//bool isSizeGripEnabled () const
static QoreNode *QDIALOG_isSizeGripEnabled(Object *self, QoreQDialog *qd, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qd->qobj->isSizeGripEnabled());
}

//int result () const
static QoreNode *QDIALOG_result(Object *self, QoreQDialog *qd, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qd->qobj->result());
}

//void setModal ( bool modal )
static QoreNode *QDIALOG_setModal(Object *self, QoreQDialog *qd, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool modal = p ? p->getAsBool() : false;
   qd->qobj->setModal(modal);
   return 0;
}

//void setResult ( int i )
static QoreNode *QDIALOG_setResult(Object *self, QoreQDialog *qd, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int i = p ? p->getAsInt() : 0;
   qd->qobj->setResult(i);
   return 0;
}

//void setSizeGripEnabled ( bool )
static QoreNode *QDIALOG_setSizeGripEnabled(Object *self, QoreQDialog *qd, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool b = p ? p->getAsBool() : false;
   qd->qobj->setSizeGripEnabled(b);
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

   return QC_QDialog;
}
