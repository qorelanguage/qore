/*
 QC_QAbstractSlider.cc
 
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

#include "QC_QAbstractSlider.h"

int CID_QABSTRACTSLIDER;

static void QABSTRACTSLIDER_constructor(class QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   xsink->raiseException("QABSTRACTSLIDER-CONSTRUCTOR-ERROR", "QAbstractSlider is an abstract class");
}

static void QABSTRACTSLIDER_copy(class QoreObject *self, class QoreObject *old, class QoreQAbstractSlider *qs, ExceptionSink *xsink)
{
   xsink->raiseException("QABSTRACTSLIDER-COPY-ERROR", "objects of this class cannot be copied");
}

//bool hasTracking () const
static AbstractQoreNode *QABSTRACTSLIDER_hasTracking(QoreObject *self, QoreAbstractQAbstractSlider *qas, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qas->getQAbstractSlider()->hasTracking());
}

//bool invertedAppearance () const
static AbstractQoreNode *QABSTRACTSLIDER_invertedAppearance(QoreObject *self, QoreAbstractQAbstractSlider *qas, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qas->getQAbstractSlider()->invertedAppearance());
}

//bool invertedControls () const
static AbstractQoreNode *QABSTRACTSLIDER_invertedControls(QoreObject *self, QoreAbstractQAbstractSlider *qas, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qas->getQAbstractSlider()->invertedControls());
}

//bool isSliderDown () const
static AbstractQoreNode *QABSTRACTSLIDER_isSliderDown(QoreObject *self, QoreAbstractQAbstractSlider *qas, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qas->getQAbstractSlider()->isSliderDown());
}

//int maximum () const
static AbstractQoreNode *QABSTRACTSLIDER_maximum(QoreObject *self, QoreAbstractQAbstractSlider *qas, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qas->getQAbstractSlider()->maximum());
}

//int minimum () const
static AbstractQoreNode *QABSTRACTSLIDER_minimum(QoreObject *self, QoreAbstractQAbstractSlider *qas, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qas->getQAbstractSlider()->minimum());
}
//Qt::Orientation orientation () const
static AbstractQoreNode *QABSTRACTSLIDER_orientation(QoreObject *self, QoreAbstractQAbstractSlider *qas, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qas->getQAbstractSlider()->orientation());
}

//int pageStep () const
static AbstractQoreNode *QABSTRACTSLIDER_pageStep(QoreObject *self, QoreAbstractQAbstractSlider *qas, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qas->getQAbstractSlider()->pageStep());
}

//void setInvertedAppearance ( bool )
static AbstractQoreNode *QABSTRACTSLIDER_setInvertedAppearance(QoreObject *self, QoreAbstractQAbstractSlider *qas, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   bool b = p ? p->getAsBool() : false;
   qas->getQAbstractSlider()->setInvertedAppearance(b);
   return 0;
}

//void setInvertedControls ( bool )
static AbstractQoreNode *QABSTRACTSLIDER_setInvertedControls(QoreObject *self, QoreAbstractQAbstractSlider *qas, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   bool b = p ? p->getAsBool() : false;
   qas->getQAbstractSlider()->setInvertedControls(b);
   return 0;
}

//void setMaximum ( int )
static AbstractQoreNode *QABSTRACTSLIDER_setMaximum(QoreObject *self, QoreAbstractQAbstractSlider *qas, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   int x = p ? p->getAsInt() : 0;
   qas->getQAbstractSlider()->setMaximum(x);
   return 0;
}

//void setMinimum ( int )
static AbstractQoreNode *QABSTRACTSLIDER_setMinimum(QoreObject *self, QoreAbstractQAbstractSlider *qas, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   int x = p ? p->getAsInt() : 0;
   qas->getQAbstractSlider()->setMinimum(x);
   return 0;
}

//void setPageStep ( int )
static AbstractQoreNode *QABSTRACTSLIDER_setPageStep(QoreObject *self, QoreAbstractQAbstractSlider *qas, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   int x = p ? p->getAsInt() : 0;
   qas->getQAbstractSlider()->setPageStep(x);
   return 0;
}

//void setRange ( int min, int max )
static AbstractQoreNode *QABSTRACTSLIDER_setRange(QoreObject *self, QoreAbstractQAbstractSlider *qas, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   int min = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int max = p ? p->getAsInt() : 0;
   qas->getQAbstractSlider()->setRange(min, max);
   return 0;
}

//void setSingleStep ( int )
static AbstractQoreNode *QABSTRACTSLIDER_setSingleStep(QoreObject *self, QoreAbstractQAbstractSlider *qas, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   int x = p ? p->getAsInt() : 0;
   qas->getQAbstractSlider()->setSingleStep(x);
   return 0;
}

//void setSliderDown ( bool )
static AbstractQoreNode *QABSTRACTSLIDER_setSliderDown(QoreObject *self, QoreAbstractQAbstractSlider *qas, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   bool b = p ? p->getAsBool() : false;
   qas->getQAbstractSlider()->setSliderDown(b);
   return 0;
}

//void setSliderPosition ( int )
static AbstractQoreNode *QABSTRACTSLIDER_setSliderPosition(QoreObject *self, QoreAbstractQAbstractSlider *qas, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   int x = p ? p->getAsInt() : 0;
   qas->getQAbstractSlider()->setSliderPosition(x);
   return 0;
}

//void setTracking ( bool enable )
static AbstractQoreNode *QABSTRACTSLIDER_setTracking(QoreObject *self, QoreAbstractQAbstractSlider *qas, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   bool enable = p ? p->getAsBool() : false;
   qas->getQAbstractSlider()->setTracking(enable);
   return 0;
}

//int singleStep () const
static AbstractQoreNode *QABSTRACTSLIDER_singleStep(QoreObject *self, QoreAbstractQAbstractSlider *qas, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qas->getQAbstractSlider()->singleStep());
}

//int sliderPosition () const
static AbstractQoreNode *QABSTRACTSLIDER_sliderPosition(QoreObject *self, QoreAbstractQAbstractSlider *qas, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qas->getQAbstractSlider()->sliderPosition());
}

//void triggerAction ( SliderAction action )
static AbstractQoreNode *QABSTRACTSLIDER_triggerAction(QoreObject *self, QoreAbstractQAbstractSlider *qas, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   QAbstractSlider::SliderAction action = (QAbstractSlider::SliderAction)(p ? p->getAsInt() : 0);
   qas->getQAbstractSlider()->triggerAction(action);
   return 0;
}


//int value () const
static AbstractQoreNode *QABSTRACTSLIDER_value(QoreObject *self, QoreAbstractQAbstractSlider *qas, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qas->getQAbstractSlider()->value());
}

//void setOrientation ( Qt::Orientation )
static AbstractQoreNode *QABSTRACTSLIDER_setOrientation(QoreObject *self, QoreAbstractQAbstractSlider *qas, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   Qt::Orientation orientation = (Qt::Orientation)(p ? p->getAsInt() : 0);
   qas->getQAbstractSlider()->setOrientation(orientation);
   return 0;
}

//void setValue ( int )
static AbstractQoreNode *QABSTRACTSLIDER_setValue(QoreObject *self, QoreAbstractQAbstractSlider *qas, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   int x = p ? p->getAsInt() : 0;
   qas->getQAbstractSlider()->setValue(x);
   return 0;
}

class QoreClass *initQAbstractSliderClass(class QoreClass *qwidget)
{
   tracein("initQAbstractSliderClass()");
   
   class QoreClass *QC_QAbstractSlider = new QoreClass("QAbstractSlider", QDOM_GUI);

   CID_QABSTRACTSLIDER = QC_QAbstractSlider->getID();

   QC_QAbstractSlider->addBuiltinVirtualBaseClass(qwidget);

   QC_QAbstractSlider->setConstructor(QABSTRACTSLIDER_constructor);
   QC_QAbstractSlider->setCopy((q_copy_t)QABSTRACTSLIDER_copy);

   QC_QAbstractSlider->addMethod("hasTracking",                 (q_method_t)QABSTRACTSLIDER_hasTracking);
   QC_QAbstractSlider->addMethod("invertedAppearance",          (q_method_t)QABSTRACTSLIDER_invertedAppearance);
   QC_QAbstractSlider->addMethod("invertedControls",            (q_method_t)QABSTRACTSLIDER_invertedControls);
   QC_QAbstractSlider->addMethod("isSliderDown",                (q_method_t)QABSTRACTSLIDER_isSliderDown);
   QC_QAbstractSlider->addMethod("maximum",                     (q_method_t)QABSTRACTSLIDER_maximum);
   QC_QAbstractSlider->addMethod("minimum",                     (q_method_t)QABSTRACTSLIDER_minimum);
   QC_QAbstractSlider->addMethod("orientation",                 (q_method_t)QABSTRACTSLIDER_orientation);
   QC_QAbstractSlider->addMethod("pageStep",                    (q_method_t)QABSTRACTSLIDER_pageStep);
   QC_QAbstractSlider->addMethod("setInvertedAppearance",       (q_method_t)QABSTRACTSLIDER_setInvertedAppearance);
   QC_QAbstractSlider->addMethod("setInvertedControls",         (q_method_t)QABSTRACTSLIDER_setInvertedControls);
   QC_QAbstractSlider->addMethod("setMaximum",                  (q_method_t)QABSTRACTSLIDER_setMaximum);
   QC_QAbstractSlider->addMethod("setMinimum",                  (q_method_t)QABSTRACTSLIDER_setMinimum);
   QC_QAbstractSlider->addMethod("setPageStep",                 (q_method_t)QABSTRACTSLIDER_setPageStep);
   QC_QAbstractSlider->addMethod("setRange",                    (q_method_t)QABSTRACTSLIDER_setRange);
   QC_QAbstractSlider->addMethod("setSingleStep",               (q_method_t)QABSTRACTSLIDER_setSingleStep);
   QC_QAbstractSlider->addMethod("setSliderDown",               (q_method_t)QABSTRACTSLIDER_setSliderDown);
   QC_QAbstractSlider->addMethod("setSliderPosition",           (q_method_t)QABSTRACTSLIDER_setSliderPosition);
   QC_QAbstractSlider->addMethod("setTracking",                 (q_method_t)QABSTRACTSLIDER_setTracking);
   QC_QAbstractSlider->addMethod("singleStep",                  (q_method_t)QABSTRACTSLIDER_singleStep);
   QC_QAbstractSlider->addMethod("sliderPosition",              (q_method_t)QABSTRACTSLIDER_sliderPosition);
   QC_QAbstractSlider->addMethod("triggerAction",               (q_method_t)QABSTRACTSLIDER_triggerAction);
   QC_QAbstractSlider->addMethod("value",                       (q_method_t)QABSTRACTSLIDER_value);

   // slots
   QC_QAbstractSlider->addMethod("setOrientation",              (q_method_t)QABSTRACTSLIDER_setOrientation);
   QC_QAbstractSlider->addMethod("setValue",                    (q_method_t)QABSTRACTSLIDER_setValue);

   traceout("initQAbstractSliderClass()");
   return QC_QAbstractSlider;
}
