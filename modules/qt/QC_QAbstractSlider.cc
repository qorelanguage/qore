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

static void QABSTRACTSLIDER_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   xsink->raiseException("QABSTRACTSLIDER-CONSTRUCTOR-ERROR", "QAbstractSlider is an abstract class");
}

static void QABSTRACTSLIDER_copy(class Object *self, class Object *old, class QoreQAbstractSlider *qs, ExceptionSink *xsink)
{
   xsink->raiseException("QABSTRACTSLIDER-COPY-ERROR", "objects of this class cannot be copied");
}

//bool hasTracking () const
static QoreNode *QABSTRACTSLIDER_hasTracking(Object *self, QoreAbstractQAbstractSlider *qas, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qas->getQAbstractSlider()->hasTracking());
}

//bool invertedAppearance () const
static QoreNode *QABSTRACTSLIDER_invertedAppearance(Object *self, QoreAbstractQAbstractSlider *qas, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qas->getQAbstractSlider()->invertedAppearance());
}

//bool invertedControls () const
static QoreNode *QABSTRACTSLIDER_invertedControls(Object *self, QoreAbstractQAbstractSlider *qas, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qas->getQAbstractSlider()->invertedControls());
}

//bool isSliderDown () const
static QoreNode *QABSTRACTSLIDER_isSliderDown(Object *self, QoreAbstractQAbstractSlider *qas, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qas->getQAbstractSlider()->isSliderDown());
}

//int maximum () const
static QoreNode *QABSTRACTSLIDER_maximum(Object *self, QoreAbstractQAbstractSlider *qas, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qas->getQAbstractSlider()->maximum());
}

//int minimum () const
static QoreNode *QABSTRACTSLIDER_minimum(Object *self, QoreAbstractQAbstractSlider *qas, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qas->getQAbstractSlider()->minimum());
}
//Qt::Orientation orientation () const
static QoreNode *QABSTRACTSLIDER_orientation(Object *self, QoreAbstractQAbstractSlider *qas, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qas->getQAbstractSlider()->orientation());
}

//int pageStep () const
static QoreNode *QABSTRACTSLIDER_pageStep(Object *self, QoreAbstractQAbstractSlider *qas, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qas->getQAbstractSlider()->pageStep());
}

//void setInvertedAppearance ( bool )
static QoreNode *QABSTRACTSLIDER_setInvertedAppearance(Object *self, QoreAbstractQAbstractSlider *qas, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool b = p ? p->getAsBool() : false;
   qas->getQAbstractSlider()->setInvertedAppearance(b);
   return 0;
}

//void setInvertedControls ( bool )
static QoreNode *QABSTRACTSLIDER_setInvertedControls(Object *self, QoreAbstractQAbstractSlider *qas, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool b = p ? p->getAsBool() : false;
   qas->getQAbstractSlider()->setInvertedControls(b);
   return 0;
}

//void setMaximum ( int )
static QoreNode *QABSTRACTSLIDER_setMaximum(Object *self, QoreAbstractQAbstractSlider *qas, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int x = p ? p->getAsInt() : 0;
   qas->getQAbstractSlider()->setMaximum(x);
   return 0;
}

//void setMinimum ( int )
static QoreNode *QABSTRACTSLIDER_setMinimum(Object *self, QoreAbstractQAbstractSlider *qas, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int x = p ? p->getAsInt() : 0;
   qas->getQAbstractSlider()->setMinimum(x);
   return 0;
}

//void setPageStep ( int )
static QoreNode *QABSTRACTSLIDER_setPageStep(Object *self, QoreAbstractQAbstractSlider *qas, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int x = p ? p->getAsInt() : 0;
   qas->getQAbstractSlider()->setPageStep(x);
   return 0;
}

//void setRange ( int min, int max )
static QoreNode *QABSTRACTSLIDER_setRange(Object *self, QoreAbstractQAbstractSlider *qas, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int min = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int max = p ? p->getAsInt() : 0;
   qas->getQAbstractSlider()->setRange(min, max);
   return 0;
}

//void setSingleStep ( int )
static QoreNode *QABSTRACTSLIDER_setSingleStep(Object *self, QoreAbstractQAbstractSlider *qas, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int x = p ? p->getAsInt() : 0;
   qas->getQAbstractSlider()->setSingleStep(x);
   return 0;
}

//void setSliderDown ( bool )
static QoreNode *QABSTRACTSLIDER_setSliderDown(Object *self, QoreAbstractQAbstractSlider *qas, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool b = p ? p->getAsBool() : false;
   qas->getQAbstractSlider()->setSliderDown(b);
   return 0;
}

//void setSliderPosition ( int )
static QoreNode *QABSTRACTSLIDER_setSliderPosition(Object *self, QoreAbstractQAbstractSlider *qas, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int x = p ? p->getAsInt() : 0;
   qas->getQAbstractSlider()->setSliderPosition(x);
   return 0;
}

//void setTracking ( bool enable )
static QoreNode *QABSTRACTSLIDER_setTracking(Object *self, QoreAbstractQAbstractSlider *qas, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool enable = p ? p->getAsBool() : false;
   qas->getQAbstractSlider()->setTracking(enable);
   return 0;
}

//int singleStep () const
static QoreNode *QABSTRACTSLIDER_singleStep(Object *self, QoreAbstractQAbstractSlider *qas, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qas->getQAbstractSlider()->singleStep());
}

//int sliderPosition () const
static QoreNode *QABSTRACTSLIDER_sliderPosition(Object *self, QoreAbstractQAbstractSlider *qas, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qas->getQAbstractSlider()->sliderPosition());
}

//void triggerAction ( SliderAction action )
static QoreNode *QABSTRACTSLIDER_triggerAction(Object *self, QoreAbstractQAbstractSlider *qas, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QAbstractSlider::SliderAction action = (QAbstractSlider::SliderAction)(p ? p->getAsInt() : 0);
   qas->getQAbstractSlider()->triggerAction(action);
   return 0;
}


//int value () const
static QoreNode *QABSTRACTSLIDER_value(Object *self, QoreAbstractQAbstractSlider *qas, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qas->getQAbstractSlider()->value());
}

//void setOrientation ( Qt::Orientation )
static QoreNode *QABSTRACTSLIDER_setOrientation(Object *self, QoreAbstractQAbstractSlider *qas, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   Qt::Orientation orientation = (Qt::Orientation)(p ? p->getAsInt() : 0);
   qas->getQAbstractSlider()->setOrientation(orientation);
   return 0;
}

//void setValue ( int )
static QoreNode *QABSTRACTSLIDER_setValue(Object *self, QoreAbstractQAbstractSlider *qas, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
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
