/*
 QC_QDoubleSpinBox.cc
 
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

#include "QC_QDoubleSpinBox.h"
#include "QC_QWidget.h"

#include "qore-qt.h"

int CID_QDOUBLESPINBOX;
class QoreClass *QC_QDoubleSpinBox = 0;

//QDoubleSpinBox ( QWidget * parent = 0 )
static void QDOUBLESPINBOX_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQWidget *parent = (p && p->type == NT_OBJECT) ? (QoreQWidget *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return;
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   self->setPrivate(CID_QDOUBLESPINBOX, new QoreQDoubleSpinBox(self, parent ? static_cast<QWidget *>(parent->getQWidget()) : 0));
   return;
}

static void QDOUBLESPINBOX_copy(class QoreObject *self, class QoreObject *old, class QoreQDoubleSpinBox *qdsb, ExceptionSink *xsink)
{
   xsink->raiseException("QDOUBLESPINBOX-COPY-ERROR", "objects of this class cannot be copied");
}

//QString cleanText () const
static QoreNode *QDOUBLESPINBOX_cleanText(QoreObject *self, QoreQDoubleSpinBox *qdsb, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qdsb->qobj->cleanText().toUtf8().data(), QCS_UTF8);
}

//int decimals () const
static QoreNode *QDOUBLESPINBOX_decimals(QoreObject *self, QoreQDoubleSpinBox *qdsb, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qdsb->qobj->decimals());
}

//double maximum () const
static QoreNode *QDOUBLESPINBOX_maximum(QoreObject *self, QoreQDoubleSpinBox *qdsb, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreNode((double)qdsb->qobj->maximum());
}

//double minimum () const
static QoreNode *QDOUBLESPINBOX_minimum(QoreObject *self, QoreQDoubleSpinBox *qdsb, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreNode((double)qdsb->qobj->minimum());
}

//QString prefix () const
static QoreNode *QDOUBLESPINBOX_prefix(QoreObject *self, QoreQDoubleSpinBox *qdsb, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qdsb->qobj->prefix().toUtf8().data(), QCS_UTF8);
}

//void setDecimals ( int prec )
static QoreNode *QDOUBLESPINBOX_setDecimals(QoreObject *self, QoreQDoubleSpinBox *qdsb, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int prec = p ? p->getAsInt() : 0;
   qdsb->qobj->setDecimals(prec);
   return 0;
}

//void setMaximum ( double max )
static QoreNode *QDOUBLESPINBOX_setMaximum(QoreObject *self, QoreQDoubleSpinBox *qdsb, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   double max = p ? p->getAsFloat() : 0.0;
   qdsb->qobj->setMaximum(max);
   return 0;
}

//void setMinimum ( double min )
static QoreNode *QDOUBLESPINBOX_setMinimum(QoreObject *self, QoreQDoubleSpinBox *qdsb, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   double min = p ? p->getAsFloat() : 0.0;
   qdsb->qobj->setMinimum(min);
   return 0;
}

//void setPrefix ( const QString & prefix )
static QoreNode *QDOUBLESPINBOX_setPrefix(QoreObject *self, QoreQDoubleSpinBox *qdsb, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString prefix;
   if (get_qstring(p, prefix, xsink))
      return 0;
   qdsb->qobj->setPrefix(prefix);
   return 0;
}

//void setRange ( double minimum, double maximum )
static QoreNode *QDOUBLESPINBOX_setRange(QoreObject *self, QoreQDoubleSpinBox *qdsb, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   double minimum = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 1);
   double maximum = p ? p->getAsFloat() : 0.0;
   qdsb->qobj->setRange(minimum, maximum);
   return 0;
}

//void setSingleStep ( double val )
static QoreNode *QDOUBLESPINBOX_setSingleStep(QoreObject *self, QoreQDoubleSpinBox *qdsb, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   double val = p ? p->getAsFloat() : 0.0;
   qdsb->qobj->setSingleStep(val);
   return 0;
}

//void setSuffix ( const QString & suffix )
static QoreNode *QDOUBLESPINBOX_setSuffix(QoreObject *self, QoreQDoubleSpinBox *qdsb, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString suffix;
   if (get_qstring(p, suffix, xsink))
      return 0;
   qdsb->qobj->setSuffix(suffix);
   return 0;
}

//double singleStep () const
static QoreNode *QDOUBLESPINBOX_singleStep(QoreObject *self, QoreQDoubleSpinBox *qdsb, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreNode((double)qdsb->qobj->singleStep());
}

//QString suffix () const
static QoreNode *QDOUBLESPINBOX_suffix(QoreObject *self, QoreQDoubleSpinBox *qdsb, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qdsb->qobj->suffix().toUtf8().data(), QCS_UTF8);
}

//virtual QString textFromValue ( double value ) const
static QoreNode *QDOUBLESPINBOX_textFromValue(QoreObject *self, QoreQDoubleSpinBox *qdsb, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   double value = p ? p->getAsFloat() : 0.0;
   return new QoreStringNode(qdsb->qobj->textFromValue(value).toUtf8().data(), QCS_UTF8);
}

//double value () const
static QoreNode *QDOUBLESPINBOX_value(QoreObject *self, QoreQDoubleSpinBox *qdsb, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreNode((double)qdsb->qobj->value());
}

//virtual double valueFromText ( const QString & text ) const
static QoreNode *QDOUBLESPINBOX_valueFromText(QoreObject *self, QoreQDoubleSpinBox *qdsb, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString text;
   if (get_qstring(p, text, xsink))
      return 0;
   return new QoreNode((double)qdsb->qobj->valueFromText(text));
}

//void setValue ( double val )
static QoreNode *QDOUBLESPINBOX_setValue(QoreObject *self, QoreQDoubleSpinBox *qdsb, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   double val = p ? p->getAsFloat() : 0.0;
   qdsb->qobj->setValue(val);
   return 0;
}

QoreClass *initQDoubleSpinBoxClass(QoreClass *qabstractspinbox)
{
   QC_QDoubleSpinBox = new QoreClass("QDoubleSpinBox", QDOM_GUI);
   CID_QDOUBLESPINBOX = QC_QDoubleSpinBox->getID();

   QC_QDoubleSpinBox->addBuiltinVirtualBaseClass(qabstractspinbox);

   QC_QDoubleSpinBox->setConstructor(QDOUBLESPINBOX_constructor);
   QC_QDoubleSpinBox->setCopy((q_copy_t)QDOUBLESPINBOX_copy);

   QC_QDoubleSpinBox->addMethod("cleanText",                   (q_method_t)QDOUBLESPINBOX_cleanText);
   QC_QDoubleSpinBox->addMethod("decimals",                    (q_method_t)QDOUBLESPINBOX_decimals);
   QC_QDoubleSpinBox->addMethod("maximum",                     (q_method_t)QDOUBLESPINBOX_maximum);
   QC_QDoubleSpinBox->addMethod("minimum",                     (q_method_t)QDOUBLESPINBOX_minimum);
   QC_QDoubleSpinBox->addMethod("prefix",                      (q_method_t)QDOUBLESPINBOX_prefix);
   QC_QDoubleSpinBox->addMethod("setDecimals",                 (q_method_t)QDOUBLESPINBOX_setDecimals);
   QC_QDoubleSpinBox->addMethod("setMaximum",                  (q_method_t)QDOUBLESPINBOX_setMaximum);
   QC_QDoubleSpinBox->addMethod("setMinimum",                  (q_method_t)QDOUBLESPINBOX_setMinimum);
   QC_QDoubleSpinBox->addMethod("setPrefix",                   (q_method_t)QDOUBLESPINBOX_setPrefix);
   QC_QDoubleSpinBox->addMethod("setRange",                    (q_method_t)QDOUBLESPINBOX_setRange);
   QC_QDoubleSpinBox->addMethod("setSingleStep",               (q_method_t)QDOUBLESPINBOX_setSingleStep);
   QC_QDoubleSpinBox->addMethod("setSuffix",                   (q_method_t)QDOUBLESPINBOX_setSuffix);
   QC_QDoubleSpinBox->addMethod("singleStep",                  (q_method_t)QDOUBLESPINBOX_singleStep);
   QC_QDoubleSpinBox->addMethod("suffix",                      (q_method_t)QDOUBLESPINBOX_suffix);
   QC_QDoubleSpinBox->addMethod("textFromValue",               (q_method_t)QDOUBLESPINBOX_textFromValue);
   QC_QDoubleSpinBox->addMethod("value",                       (q_method_t)QDOUBLESPINBOX_value);
   QC_QDoubleSpinBox->addMethod("valueFromText",               (q_method_t)QDOUBLESPINBOX_valueFromText);
   QC_QDoubleSpinBox->addMethod("setValue",                    (q_method_t)QDOUBLESPINBOX_setValue);

   return QC_QDoubleSpinBox;
}
