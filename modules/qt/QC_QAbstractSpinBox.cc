/*
 QC_QAbstractSpinBox.cc
 
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

#include "QC_QAbstractSpinBox.h"

int CID_QABSTRACTSPINBOX;
class QoreClass *QC_QAbstractSpinBox = 0;

//QAbstractSpinBox ( QWidget * parent = 0 )
static void QABSTRACTSPINBOX_constructor(Object *self, QoreNode *params, ExceptionSink *xsink)
{
   xsink->raiseException("QABSTRACTSPINBOX-CONSTRUCTOR-PARAM-ERROR", "QAbstractSpinBox is an abstract class");
   return;
}

static void QABSTRACTSPINBOX_copy(class Object *self, class Object *old, class QoreQAbstractSpinBox *qasb, ExceptionSink *xsink)
{
   xsink->raiseException("QABSTRACTSPINBOX-COPY-ERROR", "objects of this class cannot be copied");
}

//Qt::Alignment alignment () const
static QoreNode *QABSTRACTSPINBOX_alignment(Object *self, QoreAbstractQAbstractSpinBox *qasb, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qasb->getQAbstractSpinBox()->alignment());
}

//ButtonSymbols buttonSymbols () const
static QoreNode *QABSTRACTSPINBOX_buttonSymbols(Object *self, QoreAbstractQAbstractSpinBox *qasb, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qasb->getQAbstractSpinBox()->buttonSymbols());
}

//CorrectionMode correctionMode () const
static QoreNode *QABSTRACTSPINBOX_correctionMode(Object *self, QoreAbstractQAbstractSpinBox *qasb, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qasb->getQAbstractSpinBox()->correctionMode());
}

//virtual void fixup ( QString & input ) const
//static QoreNode *QABSTRACTSPINBOX_fixup(Object *self, QoreAbstractQAbstractSpinBox *qasb, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   QString input;
//   if (get_qstring(p, input, xsink))
//      return 0;
//
//   qasb->getQAbstractSpinBox()->fixup(input);
//   return 0;
//}

//bool hasAcceptableInput () const
static QoreNode *QABSTRACTSPINBOX_hasAcceptableInput(Object *self, QoreAbstractQAbstractSpinBox *qasb, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qasb->getQAbstractSpinBox()->hasAcceptableInput());
}

//bool hasFrame () const
static QoreNode *QABSTRACTSPINBOX_hasFrame(Object *self, QoreAbstractQAbstractSpinBox *qasb, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qasb->getQAbstractSpinBox()->hasFrame());
}

//void interpretText ()
static QoreNode *QABSTRACTSPINBOX_interpretText(Object *self, QoreAbstractQAbstractSpinBox *qasb, QoreNode *params, ExceptionSink *xsink)
{
   qasb->getQAbstractSpinBox()->interpretText();
   return 0;
}

//bool isAccelerated () const
static QoreNode *QABSTRACTSPINBOX_isAccelerated(Object *self, QoreAbstractQAbstractSpinBox *qasb, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qasb->getQAbstractSpinBox()->isAccelerated());
}

//bool isReadOnly () const
static QoreNode *QABSTRACTSPINBOX_isReadOnly(Object *self, QoreAbstractQAbstractSpinBox *qasb, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qasb->getQAbstractSpinBox()->isReadOnly());
}

//bool keyboardTracking () const
static QoreNode *QABSTRACTSPINBOX_keyboardTracking(Object *self, QoreAbstractQAbstractSpinBox *qasb, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qasb->getQAbstractSpinBox()->keyboardTracking());
}

//void setAccelerated ( bool on )
static QoreNode *QABSTRACTSPINBOX_setAccelerated(Object *self, QoreAbstractQAbstractSpinBox *qasb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool on = p ? p->getAsBool() : false;
   qasb->getQAbstractSpinBox()->setAccelerated(on);
   return 0;
}

//void setAlignment ( Qt::Alignment flag )
static QoreNode *QABSTRACTSPINBOX_setAlignment(Object *self, QoreAbstractQAbstractSpinBox *qasb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   Qt::Alignment flag = (Qt::Alignment)(p ? p->getAsInt() : 0);
   qasb->getQAbstractSpinBox()->setAlignment(flag);
   return 0;
}

//void setButtonSymbols ( ButtonSymbols bs )
static QoreNode *QABSTRACTSPINBOX_setButtonSymbols(Object *self, QoreAbstractQAbstractSpinBox *qasb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QAbstractSpinBox::ButtonSymbols bs = (QAbstractSpinBox::ButtonSymbols)(p ? p->getAsInt() : 0);
   qasb->getQAbstractSpinBox()->setButtonSymbols(bs);
   return 0;
}

//void setCorrectionMode ( CorrectionMode cm )
static QoreNode *QABSTRACTSPINBOX_setCorrectionMode(Object *self, QoreAbstractQAbstractSpinBox *qasb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QAbstractSpinBox::CorrectionMode cm = (QAbstractSpinBox::CorrectionMode)(p ? p->getAsInt() : 0);
   qasb->getQAbstractSpinBox()->setCorrectionMode(cm);
   return 0;
}

//void setFrame ( bool )
static QoreNode *QABSTRACTSPINBOX_setFrame(Object *self, QoreAbstractQAbstractSpinBox *qasb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool b = p ? p->getAsBool() : false;
   qasb->getQAbstractSpinBox()->setFrame(b);
   return 0;
}

//void setKeyboardTracking ( bool kt )
static QoreNode *QABSTRACTSPINBOX_setKeyboardTracking(Object *self, QoreAbstractQAbstractSpinBox *qasb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool kt = p ? p->getAsBool() : false;
   qasb->getQAbstractSpinBox()->setKeyboardTracking(kt);
   return 0;
}

//void setReadOnly ( bool r )
static QoreNode *QABSTRACTSPINBOX_setReadOnly(Object *self, QoreAbstractQAbstractSpinBox *qasb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool r = p ? p->getAsBool() : false;
   qasb->getQAbstractSpinBox()->setReadOnly(r);
   return 0;
}

//void setSpecialValueText ( const QString & txt )
static QoreNode *QABSTRACTSPINBOX_setSpecialValueText(Object *self, QoreAbstractQAbstractSpinBox *qasb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString txt;

   if (get_qstring(p, txt, xsink))
      return 0;

   qasb->getQAbstractSpinBox()->setSpecialValueText(txt);
   return 0;
}

//void setWrapping ( bool w )
static QoreNode *QABSTRACTSPINBOX_setWrapping(Object *self, QoreAbstractQAbstractSpinBox *qasb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool w = p ? p->getAsBool() : false;
   qasb->getQAbstractSpinBox()->setWrapping(w);
   return 0;
}

//QString specialValueText () const
static QoreNode *QABSTRACTSPINBOX_specialValueText(Object *self, QoreAbstractQAbstractSpinBox *qasb, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(new QoreString(qasb->getQAbstractSpinBox()->specialValueText().toUtf8().data(), QCS_UTF8));
}

//virtual void stepBy ( int steps )
static QoreNode *QABSTRACTSPINBOX_stepBy(Object *self, QoreAbstractQAbstractSpinBox *qasb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int steps = p ? p->getAsInt() : 0;
   qasb->getQAbstractSpinBox()->stepBy(steps);
   return 0;
}

//QString text () const
static QoreNode *QABSTRACTSPINBOX_text(Object *self, QoreAbstractQAbstractSpinBox *qasb, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(new QoreString(qasb->getQAbstractSpinBox()->text().toUtf8().data(), QCS_UTF8));
}

//virtual QValidator::State validate ( QString & input, int & pos ) const
//static QoreNode *QABSTRACTSPINBOX_validate(Object *self, QoreAbstractQAbstractSpinBox *qasb, QoreNode *params, ExceptionSink *xsink)
//{
//  QoreNode *p = get_param(params, 0);
//   if (!p || p->type != NT_STRING) {
//      xsink->raiseException("QABSTRACTSPINBOX-VALIDATE-PARAM-ERROR", "expecting a string as first argument to QAbstractSpinBox::validate()");
//      return 0;
//   }
//   const char *input = p->val.String->getBuffer();
//   p = get_param(params, 1);
//   int pos = p ? p->getAsInt() : 0;
//   return new QoreNode((int64)qasb->getQAbstractSpinBox()->validate(input, pos));
//}

//bool wrapping () const
static QoreNode *QABSTRACTSPINBOX_wrapping(Object *self, QoreAbstractQAbstractSpinBox *qasb, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qasb->getQAbstractSpinBox()->wrapping());
}

//virtual void clear ()
static QoreNode *QABSTRACTSPINBOX_clear(Object *self, QoreAbstractQAbstractSpinBox *qasb, QoreNode *params, ExceptionSink *xsink)
{
   qasb->getQAbstractSpinBox()->clear();
   return 0;
}

//void selectAll ()
static QoreNode *QABSTRACTSPINBOX_selectAll(Object *self, QoreAbstractQAbstractSpinBox *qasb, QoreNode *params, ExceptionSink *xsink)
{
   qasb->getQAbstractSpinBox()->selectAll();
   return 0;
}

//void stepDown ()
static QoreNode *QABSTRACTSPINBOX_stepDown(Object *self, QoreAbstractQAbstractSpinBox *qasb, QoreNode *params, ExceptionSink *xsink)
{
   qasb->getQAbstractSpinBox()->stepDown();
   return 0;
}

//void stepUp ()
static QoreNode *QABSTRACTSPINBOX_stepUp(Object *self, QoreAbstractQAbstractSpinBox *qasb, QoreNode *params, ExceptionSink *xsink)
{
   qasb->getQAbstractSpinBox()->stepUp();
   return 0;
}

QoreClass *initQAbstractSpinBoxClass(QoreClass *qwidget)
{
   QC_QAbstractSpinBox = new QoreClass("QAbstractSpinBox", QDOM_GUI);
   CID_QABSTRACTSPINBOX = QC_QAbstractSpinBox->getID();

   QC_QAbstractSpinBox->addBuiltinVirtualBaseClass(qwidget);

   QC_QAbstractSpinBox->setConstructor(QABSTRACTSPINBOX_constructor);
   QC_QAbstractSpinBox->setCopy((q_copy_t)QABSTRACTSPINBOX_copy);

   QC_QAbstractSpinBox->addMethod("alignment",                   (q_method_t)QABSTRACTSPINBOX_alignment);
   QC_QAbstractSpinBox->addMethod("buttonSymbols",               (q_method_t)QABSTRACTSPINBOX_buttonSymbols);
   QC_QAbstractSpinBox->addMethod("correctionMode",              (q_method_t)QABSTRACTSPINBOX_correctionMode);
   //QC_QAbstractSpinBox->addMethod("fixup",                       (q_method_t)QABSTRACTSPINBOX_fixup);
   QC_QAbstractSpinBox->addMethod("hasAcceptableInput",          (q_method_t)QABSTRACTSPINBOX_hasAcceptableInput);
   QC_QAbstractSpinBox->addMethod("hasFrame",                    (q_method_t)QABSTRACTSPINBOX_hasFrame);
   QC_QAbstractSpinBox->addMethod("interpretText",               (q_method_t)QABSTRACTSPINBOX_interpretText);
   QC_QAbstractSpinBox->addMethod("isAccelerated",               (q_method_t)QABSTRACTSPINBOX_isAccelerated);
   QC_QAbstractSpinBox->addMethod("isReadOnly",                  (q_method_t)QABSTRACTSPINBOX_isReadOnly);
   QC_QAbstractSpinBox->addMethod("keyboardTracking",            (q_method_t)QABSTRACTSPINBOX_keyboardTracking);
   QC_QAbstractSpinBox->addMethod("setAccelerated",              (q_method_t)QABSTRACTSPINBOX_setAccelerated);
   QC_QAbstractSpinBox->addMethod("setAlignment",                (q_method_t)QABSTRACTSPINBOX_setAlignment);
   QC_QAbstractSpinBox->addMethod("setButtonSymbols",            (q_method_t)QABSTRACTSPINBOX_setButtonSymbols);
   QC_QAbstractSpinBox->addMethod("setCorrectionMode",           (q_method_t)QABSTRACTSPINBOX_setCorrectionMode);
   QC_QAbstractSpinBox->addMethod("setFrame",                    (q_method_t)QABSTRACTSPINBOX_setFrame);
   QC_QAbstractSpinBox->addMethod("setKeyboardTracking",         (q_method_t)QABSTRACTSPINBOX_setKeyboardTracking);
   QC_QAbstractSpinBox->addMethod("setReadOnly",                 (q_method_t)QABSTRACTSPINBOX_setReadOnly);
   QC_QAbstractSpinBox->addMethod("setSpecialValueText",         (q_method_t)QABSTRACTSPINBOX_setSpecialValueText);
   QC_QAbstractSpinBox->addMethod("setWrapping",                 (q_method_t)QABSTRACTSPINBOX_setWrapping);
   QC_QAbstractSpinBox->addMethod("specialValueText",            (q_method_t)QABSTRACTSPINBOX_specialValueText);
   QC_QAbstractSpinBox->addMethod("stepBy",                      (q_method_t)QABSTRACTSPINBOX_stepBy);
   QC_QAbstractSpinBox->addMethod("text",                        (q_method_t)QABSTRACTSPINBOX_text);
   //QC_QAbstractSpinBox->addMethod("validate",                    (q_method_t)QABSTRACTSPINBOX_validate);
   QC_QAbstractSpinBox->addMethod("wrapping",                    (q_method_t)QABSTRACTSPINBOX_wrapping);
   QC_QAbstractSpinBox->addMethod("clear",                       (q_method_t)QABSTRACTSPINBOX_clear);
   QC_QAbstractSpinBox->addMethod("selectAll",                   (q_method_t)QABSTRACTSPINBOX_selectAll);
   QC_QAbstractSpinBox->addMethod("stepDown",                    (q_method_t)QABSTRACTSPINBOX_stepDown);
   QC_QAbstractSpinBox->addMethod("stepUp",                      (q_method_t)QABSTRACTSPINBOX_stepUp);

   return QC_QAbstractSpinBox;
}
