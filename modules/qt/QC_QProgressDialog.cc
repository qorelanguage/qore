/*
 QC_QProgressDialog.cc
 
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

#include <qore/Qore.h>

#include "QC_QProgressDialog.h"

int CID_QPROGRESSDIALOG;
class QoreClass *QC_QProgressDialog = 0;

//QProgressDialog ( QWidget * parent = 0, Qt::WindowFlags f = 0 )
//QProgressDialog ( const QString & labelText, const QString & cancelButtonText, int minimum, int maximum, QWidget * parent = 0, Qt::WindowFlags f = 0 )
static void QPROGRESSDIALOG_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QPROGRESSDIALOG, new QoreQProgressDialog(self));
      return;
   }
   if (p && p->getType() == NT_OBJECT) {
      QoreQWidget *parent = (QoreQWidget *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QWIDGET, xsink);
      if (*xsink)
         return;
      ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);

      p = get_param(params, 1);
      Qt::WindowFlags f = (Qt::WindowFlags)(!is_nothing(p) ? p->getAsInt() : 0);
      self->setPrivate(CID_QPROGRESSDIALOG, new QoreQProgressDialog(self, parent ? static_cast<QWidget *>(parent->getQWidget()) : 0, f));
      return;
   }
   QString labelText;
   if (get_qstring(p, labelText, xsink))
      return;
   p = get_param(params, 1);
   QString cancelButtonText;
   if (get_qstring(p, cancelButtonText, xsink))
      return;
   p = get_param(params, 2);
   int minimum = p ? p->getAsInt() : 0;
   p = get_param(params, 3);
   int maximum = p ? p->getAsInt() : 0;
   p = get_param(params, 4);
   QoreQWidget *parent = (p && p->getType() == NT_OBJECT) ? (QoreQWidget *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return;
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   p = get_param(params, 5);
   Qt::WindowFlags f = (Qt::WindowFlags)(!is_nothing(p) ? p->getAsInt() : 0);
   self->setPrivate(CID_QPROGRESSDIALOG, new QoreQProgressDialog(self, labelText, cancelButtonText, minimum, maximum, parent ? static_cast<QWidget *>(parent->getQWidget()) : 0, f));
}

static void QPROGRESSDIALOG_copy(QoreObject *self, QoreObject *old, QoreQProgressDialog *qpd, ExceptionSink *xsink)
{
   xsink->raiseException("QPROGRESSDIALOG-COPY-ERROR", "objects of this class cannot be copied");
}

//bool autoClose () const
static AbstractQoreNode *QPROGRESSDIALOG_autoClose(QoreObject *self, QoreQProgressDialog *qpd, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qpd->qobj->autoClose());
}

//bool autoReset () const
static AbstractQoreNode *QPROGRESSDIALOG_autoReset(QoreObject *self, QoreQProgressDialog *qpd, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qpd->qobj->autoReset());
}

//QString labelText () const
static AbstractQoreNode *QPROGRESSDIALOG_labelText(QoreObject *self, QoreQProgressDialog *qpd, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qpd->qobj->labelText().toUtf8().data(), QCS_UTF8);
}

//int maximum () const
static AbstractQoreNode *QPROGRESSDIALOG_maximum(QoreObject *self, QoreQProgressDialog *qpd, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qpd->qobj->maximum());
}

//int minimum () const
static AbstractQoreNode *QPROGRESSDIALOG_minimum(QoreObject *self, QoreQProgressDialog *qpd, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qpd->qobj->minimum());
}

//int minimumDuration () const
static AbstractQoreNode *QPROGRESSDIALOG_minimumDuration(QoreObject *self, QoreQProgressDialog *qpd, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qpd->qobj->minimumDuration());
}

//void setAutoClose ( bool b )
static AbstractQoreNode *QPROGRESSDIALOG_setAutoClose(QoreObject *self, QoreQProgressDialog *qpd, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   bool b = p ? p->getAsBool() : false;
   qpd->qobj->setAutoClose(b);
   return 0;
}

//void setAutoReset ( bool b )
static AbstractQoreNode *QPROGRESSDIALOG_setAutoReset(QoreObject *self, QoreQProgressDialog *qpd, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   bool b = p ? p->getAsBool() : false;
   qpd->qobj->setAutoReset(b);
   return 0;
}

//void setBar ( QProgressBar * bar )
static AbstractQoreNode *QPROGRESSDIALOG_setBar(QoreObject *self, QoreQProgressDialog *qpd, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQProgressBar *bar = (p && p->getType() == NT_OBJECT) ? (QoreQProgressBar *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QPROGRESSBAR, xsink) : 0;
   if (!bar) {
      if (!xsink->isException())
         xsink->raiseException("QPROGRESSDIALOG-SETBAR-PARAM-ERROR", "expecting a QProgressBar object as first argument to QProgressDialog::setBar()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> barHolder(static_cast<AbstractPrivateData *>(bar), xsink);
   qpd->qobj->setBar(static_cast<QProgressBar *>(bar->qobj));
   return 0;
}

//void setCancelButton ( QPushButton * cancelButton )
static AbstractQoreNode *QPROGRESSDIALOG_setCancelButton(QoreObject *self, QoreQProgressDialog *qpd, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQPushButton *cancelButton = (p && p->getType() == NT_OBJECT) ? (QoreQPushButton *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QPUSHBUTTON, xsink) : 0;
   if (!cancelButton) {
      if (!xsink->isException())
         xsink->raiseException("QPROGRESSDIALOG-SETCANCELBUTTON-PARAM-ERROR", "expecting a QPushButton object as first argument to QProgressDialog::setCancelButton()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> cancelButtonHolder(static_cast<AbstractPrivateData *>(cancelButton), xsink);
   qpd->qobj->setCancelButton(static_cast<QPushButton *>(cancelButton->qobj));
   return 0;
}

//void setLabel ( QLabel * label )
static AbstractQoreNode *QPROGRESSDIALOG_setLabel(QoreObject *self, QoreQProgressDialog *qpd, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQLabel *label = (p && p->getType() == NT_OBJECT) ? (QoreQLabel *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QLABEL, xsink) : 0;
   if (!label) {
      if (!xsink->isException())
         xsink->raiseException("QPROGRESSDIALOG-SETLABEL-PARAM-ERROR", "expecting a QLabel object as first argument to QProgressDialog::setLabel()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> labelHolder(static_cast<AbstractPrivateData *>(label), xsink);
   qpd->qobj->setLabel(static_cast<QLabel *>(label->qobj));
   return 0;
}

//virtual QSize sizeHint () const
static AbstractQoreNode *QPROGRESSDIALOG_sizeHint(QoreObject *self, QoreQProgressDialog *qpd, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QSize, new QoreQSize(qpd->qobj->parent_sizeHint()));
}

//int value () const
static AbstractQoreNode *QPROGRESSDIALOG_value(QoreObject *self, QoreQProgressDialog *qpd, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qpd->qobj->value());
}

//bool wasCanceled () const
static AbstractQoreNode *QPROGRESSDIALOG_wasCanceled(QoreObject *self, QoreQProgressDialog *qpd, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qpd->qobj->wasCanceled());
}

//void cancel ()
static AbstractQoreNode *QPROGRESSDIALOG_cancel(QoreObject *self, QoreQProgressDialog *qpd, const QoreListNode *params, ExceptionSink *xsink)
{
   qpd->qobj->cancel();
   return 0;
}

//void reset ()
static AbstractQoreNode *QPROGRESSDIALOG_reset(QoreObject *self, QoreQProgressDialog *qpd, const QoreListNode *params, ExceptionSink *xsink)
{
   qpd->qobj->reset();
   return 0;
}

//void setCancelButtonText ( const QString & cancelButtonText )
static AbstractQoreNode *QPROGRESSDIALOG_setCancelButtonText(QoreObject *self, QoreQProgressDialog *qpd, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QString cancelButtonText;
   if (get_qstring(p, cancelButtonText, xsink))
      return 0;
   qpd->qobj->setCancelButtonText(cancelButtonText);
   return 0;
}

//void setLabelText ( const QString & )
static AbstractQoreNode *QPROGRESSDIALOG_setLabelText(QoreObject *self, QoreQProgressDialog *qpd, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QString qstring;
   if (get_qstring(p, qstring, xsink))
      return 0;
   qpd->qobj->setLabelText(qstring);
   return 0;
}

//void setMaximum ( int maximum )
static AbstractQoreNode *QPROGRESSDIALOG_setMaximum(QoreObject *self, QoreQProgressDialog *qpd, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int maximum = p ? p->getAsInt() : 0;
   qpd->qobj->setMaximum(maximum);
   return 0;
}

//void setMinimum ( int minimum )
static AbstractQoreNode *QPROGRESSDIALOG_setMinimum(QoreObject *self, QoreQProgressDialog *qpd, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int minimum = p ? p->getAsInt() : 0;
   qpd->qobj->setMinimum(minimum);
   return 0;
}

//void setMinimumDuration ( int ms )
static AbstractQoreNode *QPROGRESSDIALOG_setMinimumDuration(QoreObject *self, QoreQProgressDialog *qpd, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int ms = p ? p->getAsInt() : 0;
   qpd->qobj->setMinimumDuration(ms);
   return 0;
}

//void setRange ( int minimum, int maximum )
static AbstractQoreNode *QPROGRESSDIALOG_setRange(QoreObject *self, QoreQProgressDialog *qpd, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int minimum = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int maximum = p ? p->getAsInt() : 0;
   qpd->qobj->setRange(minimum, maximum);
   return 0;
}

//void setValue ( int progress )
static AbstractQoreNode *QPROGRESSDIALOG_setValue(QoreObject *self, QoreQProgressDialog *qpd, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int progress = p ? p->getAsInt() : 0;
   qpd->qobj->setValue(progress);
   return 0;
}

//void forceShow ()
static AbstractQoreNode *QPROGRESSDIALOG_forceShow(QoreObject *self, QoreQProgressDialog *qpd, const QoreListNode *params, ExceptionSink *xsink)
{
   qpd->qobj->parent_forceShow();
   return 0;
}

QoreClass *initQProgressDialogClass(QoreClass *qdialog)
{
   QC_QProgressDialog = new QoreClass("QProgressDialog", QDOM_GUI);
   CID_QPROGRESSDIALOG = QC_QProgressDialog->getID();

   QC_QProgressDialog->addBuiltinVirtualBaseClass(qdialog);

   QC_QProgressDialog->setConstructor(QPROGRESSDIALOG_constructor);
   QC_QProgressDialog->setCopy((q_copy_t)QPROGRESSDIALOG_copy);

   QC_QProgressDialog->addMethod("autoClose",                   (q_method_t)QPROGRESSDIALOG_autoClose);
   QC_QProgressDialog->addMethod("autoReset",                   (q_method_t)QPROGRESSDIALOG_autoReset);
   QC_QProgressDialog->addMethod("labelText",                   (q_method_t)QPROGRESSDIALOG_labelText);
   QC_QProgressDialog->addMethod("maximum",                     (q_method_t)QPROGRESSDIALOG_maximum);
   QC_QProgressDialog->addMethod("minimum",                     (q_method_t)QPROGRESSDIALOG_minimum);
   QC_QProgressDialog->addMethod("minimumDuration",             (q_method_t)QPROGRESSDIALOG_minimumDuration);
   QC_QProgressDialog->addMethod("setAutoClose",                (q_method_t)QPROGRESSDIALOG_setAutoClose);
   QC_QProgressDialog->addMethod("setAutoReset",                (q_method_t)QPROGRESSDIALOG_setAutoReset);
   QC_QProgressDialog->addMethod("setBar",                      (q_method_t)QPROGRESSDIALOG_setBar);
   QC_QProgressDialog->addMethod("setCancelButton",             (q_method_t)QPROGRESSDIALOG_setCancelButton);
   QC_QProgressDialog->addMethod("setLabel",                    (q_method_t)QPROGRESSDIALOG_setLabel);
   QC_QProgressDialog->addMethod("sizeHint",                    (q_method_t)QPROGRESSDIALOG_sizeHint);
   QC_QProgressDialog->addMethod("value",                       (q_method_t)QPROGRESSDIALOG_value);
   QC_QProgressDialog->addMethod("wasCanceled",                 (q_method_t)QPROGRESSDIALOG_wasCanceled);
   QC_QProgressDialog->addMethod("cancel",                      (q_method_t)QPROGRESSDIALOG_cancel);
   QC_QProgressDialog->addMethod("reset",                       (q_method_t)QPROGRESSDIALOG_reset);
   QC_QProgressDialog->addMethod("setCancelButtonText",         (q_method_t)QPROGRESSDIALOG_setCancelButtonText);
   QC_QProgressDialog->addMethod("setLabelText",                (q_method_t)QPROGRESSDIALOG_setLabelText);
   QC_QProgressDialog->addMethod("setMaximum",                  (q_method_t)QPROGRESSDIALOG_setMaximum);
   QC_QProgressDialog->addMethod("setMinimum",                  (q_method_t)QPROGRESSDIALOG_setMinimum);
   QC_QProgressDialog->addMethod("setMinimumDuration",          (q_method_t)QPROGRESSDIALOG_setMinimumDuration);
   QC_QProgressDialog->addMethod("setRange",                    (q_method_t)QPROGRESSDIALOG_setRange);
   QC_QProgressDialog->addMethod("setValue",                    (q_method_t)QPROGRESSDIALOG_setValue);
   QC_QProgressDialog->addMethod("forceShow",                   (q_method_t)QPROGRESSDIALOG_forceShow, true);

   return QC_QProgressDialog;
}
