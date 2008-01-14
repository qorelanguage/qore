/*
 QC_QSpinBox.cc
 
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

#include "QC_QSpinBox.h"
#include "QC_QWidget.h"

#include "qore-qt.h"

int CID_QSPINBOX;
class QoreClass *QC_QSpinBox = 0;

//QSpinBox ( QWidget * parent = 0 )
static void QSPINBOX_constructor(QoreObject *self, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQWidget *parent = (p && p->type == NT_OBJECT) ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return;
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   self->setPrivate(CID_QSPINBOX, new QoreQSpinBox(self, parent ? static_cast<QWidget *>(parent->getQWidget()) : 0));
   return;
}

static void QSPINBOX_copy(class QoreObject *self, class QoreObject *old, class QoreQSpinBox *qsb, ExceptionSink *xsink)
{
   xsink->raiseException("QSPINBOX-COPY-ERROR", "objects of this class cannot be copied");
}

//QString cleanText () const
static QoreNode *QSPINBOX_cleanText(QoreObject *self, QoreQSpinBox *qsb, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qsb->qobj->cleanText().toUtf8().data(), QCS_UTF8);
}

//int maximum () const
static QoreNode *QSPINBOX_maximum(QoreObject *self, QoreQSpinBox *qsb, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qsb->qobj->maximum());
}

//int minimum () const
static QoreNode *QSPINBOX_minimum(QoreObject *self, QoreQSpinBox *qsb, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qsb->qobj->minimum());
}

//QString prefix () const
static QoreNode *QSPINBOX_prefix(QoreObject *self, QoreQSpinBox *qsb, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qsb->qobj->prefix().toUtf8().data(), QCS_UTF8);
}

//void setMaximum ( int max )
static QoreNode *QSPINBOX_setMaximum(QoreObject *self, QoreQSpinBox *qsb, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int max = p ? p->getAsInt() : 0;
   qsb->qobj->setMaximum(max);
   return 0;
}

//void setMinimum ( int min )
static QoreNode *QSPINBOX_setMinimum(QoreObject *self, QoreQSpinBox *qsb, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int min = p ? p->getAsInt() : 0;
   qsb->qobj->setMinimum(min);
   return 0;
}

//void setPrefix ( const QString & prefix )
static QoreNode *QSPINBOX_setPrefix(QoreObject *self, QoreQSpinBox *qsb, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString prefix;
   if (get_qstring(p, prefix, xsink))
      return 0;
   qsb->qobj->setPrefix(prefix);
   return 0;
}

//void setRange ( int minimum, int maximum )
static QoreNode *QSPINBOX_setRange(QoreObject *self, QoreQSpinBox *qsb, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int minimum = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int maximum = p ? p->getAsInt() : 0;
   qsb->qobj->setRange(minimum, maximum);
   return 0;
}

//void setSingleStep ( int val )
static QoreNode *QSPINBOX_setSingleStep(QoreObject *self, QoreQSpinBox *qsb, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int val = p ? p->getAsInt() : 0;
   qsb->qobj->setSingleStep(val);
   return 0;
}

//void setSuffix ( const QString & suffix )
static QoreNode *QSPINBOX_setSuffix(QoreObject *self, QoreQSpinBox *qsb, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString suffix;
   if (get_qstring(p, suffix, xsink))
      return 0;
   qsb->qobj->setSuffix(suffix);
   return 0;
}

//int singleStep () const
static QoreNode *QSPINBOX_singleStep(QoreObject *self, QoreQSpinBox *qsb, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qsb->qobj->singleStep());
}

//QString suffix () const
static QoreNode *QSPINBOX_suffix(QoreObject *self, QoreQSpinBox *qsb, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qsb->qobj->suffix().toUtf8().data(), QCS_UTF8);
}

//int value () const
static QoreNode *QSPINBOX_value(QoreObject *self, QoreQSpinBox *qsb, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qsb->qobj->value());
}

//void setValue ( int val )
static QoreNode *QSPINBOX_setValue(QoreObject *self, QoreQSpinBox *qsb, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int val = p ? p->getAsInt() : 0;
   qsb->qobj->setValue(val);
   return 0;
}

//virtual QString textFromValue ( int value ) const
static QoreNode *QSPINBOX_textFromValue(QoreObject *self, QoreQSpinBox *qsb, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int value = p ? p->getAsInt() : 0;
   return new QoreStringNode(qsb->qobj->parent_textFromValue(value).toUtf8().data(), QCS_UTF8);
}

//virtual int valueFromText ( const QString & text ) const
static QoreNode *QSPINBOX_valueFromText(QoreObject *self, QoreQSpinBox *qsb, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString text;
   if (get_qstring(p, text, xsink))
      return 0;
   return new QoreNode((int64)qsb->qobj->parent_valueFromText(text));
}

QoreClass *initQSpinBoxClass(QoreClass *qabstractspinbox)
{
   QC_QSpinBox = new QoreClass("QSpinBox", QDOM_GUI);
   CID_QSPINBOX = QC_QSpinBox->getID();

   QC_QSpinBox->addBuiltinVirtualBaseClass(qabstractspinbox);

   QC_QSpinBox->setConstructor(QSPINBOX_constructor);
   QC_QSpinBox->setCopy((q_copy_t)QSPINBOX_copy);

   QC_QSpinBox->addMethod("cleanText",                   (q_method_t)QSPINBOX_cleanText);
   QC_QSpinBox->addMethod("maximum",                     (q_method_t)QSPINBOX_maximum);
   QC_QSpinBox->addMethod("minimum",                     (q_method_t)QSPINBOX_minimum);
   QC_QSpinBox->addMethod("prefix",                      (q_method_t)QSPINBOX_prefix);
   QC_QSpinBox->addMethod("setMaximum",                  (q_method_t)QSPINBOX_setMaximum);
   QC_QSpinBox->addMethod("setMinimum",                  (q_method_t)QSPINBOX_setMinimum);
   QC_QSpinBox->addMethod("setPrefix",                   (q_method_t)QSPINBOX_setPrefix);
   QC_QSpinBox->addMethod("setRange",                    (q_method_t)QSPINBOX_setRange);
   QC_QSpinBox->addMethod("setSingleStep",               (q_method_t)QSPINBOX_setSingleStep);
   QC_QSpinBox->addMethod("setSuffix",                   (q_method_t)QSPINBOX_setSuffix);
   QC_QSpinBox->addMethod("singleStep",                  (q_method_t)QSPINBOX_singleStep);
   QC_QSpinBox->addMethod("suffix",                      (q_method_t)QSPINBOX_suffix);
   QC_QSpinBox->addMethod("value",                       (q_method_t)QSPINBOX_value);
   QC_QSpinBox->addMethod("setValue",                    (q_method_t)QSPINBOX_setValue);

   // private methods
   QC_QSpinBox->addMethod("textFromValue",               (q_method_t)QSPINBOX_textFromValue);
   QC_QSpinBox->addMethod("valueFromText",               (q_method_t)QSPINBOX_valueFromText);

   return QC_QSpinBox;
}
