/*
 QC_QSlider.cc
 
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

#include "QC_QSlider.h"

int CID_QSLIDER;

static void QSLIDER_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreQSlider *qs;
   QoreNode *p0 = get_param(params, 0);
   QoreNode *p1 = get_param(params, 1);
   QoreAbstractQWidget *parent;

   if (p0 && p0->type == NT_OBJECT)
   {
      parent = p0 ? (QoreAbstractQWidget *)p0->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
      if (!parent) {
	 xsink->raiseException("QSLIDER-CONSTRUCTOR-ERROR", "class '%s' passed as first parameter of QSlider::constructor() is not a subclass of QWidget", p0->val.object->getClass()->getName());
	 return;
      }
      ReferenceHolder<QoreAbstractQWidget> holder(parent, xsink);
      qs = new QoreQSlider(self, parent->getQWidget());
   }
   else if (p1 && p1->type == NT_OBJECT) {
      parent = p1 ? (QoreAbstractQWidget *)p1->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
      if (!parent) {
	 xsink->raiseException("QSLIDER-CONSTRUCTOR-ERROR", "class '%s' passed as second parameter of QSlider::constructor() is not a subclass of QWidget", p1->val.object->getClass()->getName());
	 return;
      }
      ReferenceHolder<QoreAbstractQWidget> holder(parent, xsink);
      int orientation = p0 ? p0->getAsInt() : 0;
      qs = new QoreQSlider(self, (Qt::Orientation)orientation, parent->getQWidget());
   }
   else
      qs = new QoreQSlider(self, (Qt::Orientation)(p0 ? p0->getAsInt() : 0));

   self->setPrivate(CID_QSLIDER, qs);
}

static void QSLIDER_copy(class Object *self, class Object *old, class QoreQSlider *qs, ExceptionSink *xsink)
{
   xsink->raiseException("QSLIDER-COPY-ERROR", "objects of this class cannot be copied");
}

static class QoreNode *QSLIDER_setRange(class Object *self, class QoreQSlider *qs, QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p = get_param(params, 0);
   int min = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int max = p ? p->getAsInt() : 0;
   qs->qobj->setRange(min, max);
   return 0;
}

static class QoreNode *QSLIDER_setValue(class Object *self, class QoreQSlider *qs, QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p = get_param(params, 0);
   if (!is_nothing(p))
      qs->qobj->setValue(p->getAsInt());
   return 0;
}

static class QoreNode *QSLIDER_setOrientation(class Object *self, class QoreQSlider *qs, QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p = get_param(params, 0);
   if (!is_nothing(p))
      qs->qobj->setOrientation((Qt::Orientation)p->getAsInt());
   return 0;
}

static class QoreNode *QSLIDER_orientation(class Object *self, class QoreQSlider *qs, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qs->qobj->orientation());
}

static class QoreNode *QSLIDER_setMinimum(class Object *self, class QoreQSlider *qs, QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p = get_param(params, 0);
   if (!is_nothing(p))
      qs->qobj->setMinimum(p->getAsInt());
   return 0;
}

static class QoreNode *QSLIDER_setMaximum(class Object *self, class QoreQSlider *qs, QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p = get_param(params, 0);
   if (!is_nothing(p))
      qs->qobj->setMaximum(p->getAsInt());
   return 0;
}

static class QoreNode *QSLIDER_minimum(class Object *self, class QoreQSlider *qs, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qs->qobj->minimum());
}

static class QoreNode *QSLIDER_maximum(class Object *self, class QoreQSlider *qs, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qs->qobj->maximum());
}

static class QoreNode *QSLIDER_setPageStep(class Object *self, class QoreQSlider *qs, QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p = get_param(params, 0);
   if (!is_nothing(p))
      qs->qobj->setPageStep(p->getAsInt());
   return 0;
}

static class QoreNode *QSLIDER_pageStep(class Object *self, class QoreQSlider *qs, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qs->qobj->pageStep());
}

static class QoreNode *QSLIDER_setSingleStep(class Object *self, class QoreQSlider *qs, QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p = get_param(params, 0);
   if (!is_nothing(p))
      qs->qobj->setSingleStep(p->getAsInt());
   return 0;
}

static class QoreNode *QSLIDER_singleStep(class Object *self, class QoreQSlider *qs, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qs->qobj->singleStep());
}

static class QoreNode *QSLIDER_setTracking(class Object *self, class QoreQSlider *qs, QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p = get_param(params, 0);
   qs->qobj->setTracking(is_nothing(p) ? true : p->getAsBool());
   return 0;
}

static class QoreNode *QSLIDER_hasTracking(class Object *self, class QoreQSlider *qs, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qs->qobj->hasTracking());
}

static class QoreNode *QSLIDER_setInvertedAppearance(class Object *self, class QoreQSlider *qs, QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p = get_param(params, 0);
   qs->qobj->setInvertedAppearance(is_nothing(p) ? true : p->getAsBool());
   return 0;
}

static class QoreNode *QSLIDER_invertedAppearance(class Object *self, class QoreQSlider *qs, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qs->qobj->invertedAppearance());
}

static class QoreNode *QSLIDER_setInvertedControls(class Object *self, class QoreQSlider *qs, QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p = get_param(params, 0);
   qs->qobj->setInvertedControls(is_nothing(p) ? true : p->getAsBool());
   return 0;
}

static class QoreNode *QSLIDER_invertedControls(class Object *self, class QoreQSlider *qs, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qs->qobj->invertedControls());
}

static class QoreNode *QSLIDER_setSliderDown(class Object *self, class QoreQSlider *qs, QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p = get_param(params, 0);
   qs->qobj->setSliderDown(is_nothing(p) ? true : p->getAsBool());
   return 0;
}

static class QoreNode *QSLIDER_isSliderDown(class Object *self, class QoreQSlider *qs, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qs->qobj->isSliderDown());
}

static class QoreNode *QSLIDER_setSliderPosition(class Object *self, class QoreQSlider *qs, QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p = get_param(params, 0);
   qs->qobj->setSliderPosition(p ? p->getAsInt() : 0);
   return 0;
}

static class QoreNode *QSLIDER_sliderPosition(class Object *self, class QoreQSlider *qs, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qs->qobj->sliderPosition());
}

static class QoreNode *QSLIDER_value(class Object *self, class QoreQSlider *qs, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qs->qobj->value());
}

class QoreClass *initQSliderClass(class QoreClass *qframe)
{
   tracein("initQSliderClass()");
   
   class QoreClass *QC_QSlider = new QoreClass("QSlider", QDOM_GUI);

   CID_QSLIDER = QC_QSlider->getID();

   QC_QSlider->addBuiltinVirtualBaseClass(qframe);

   QC_QSlider->setConstructor(QSLIDER_constructor);
   QC_QSlider->setCopy((q_copy_t)QSLIDER_copy);

   QC_QSlider->addMethod("setRange",               (q_method_t)QSLIDER_setRange);
   QC_QSlider->addMethod("setValue",               (q_method_t)QSLIDER_setValue);
   QC_QSlider->addMethod("setOrientation",         (q_method_t)QSLIDER_setOrientation);
   QC_QSlider->addMethod("orientation",            (q_method_t)QSLIDER_orientation);
   QC_QSlider->addMethod("setMinimum",             (q_method_t)QSLIDER_setMinimum);
   QC_QSlider->addMethod("setMaximum",             (q_method_t)QSLIDER_setMaximum);
   QC_QSlider->addMethod("minimum",                (q_method_t)QSLIDER_minimum);
   QC_QSlider->addMethod("maximum",                (q_method_t)QSLIDER_maximum);
   QC_QSlider->addMethod("setPageStep",            (q_method_t)QSLIDER_setPageStep);
   QC_QSlider->addMethod("pageStep",               (q_method_t)QSLIDER_pageStep);
   QC_QSlider->addMethod("setSingleStep",          (q_method_t)QSLIDER_setSingleStep);
   QC_QSlider->addMethod("singleStep",             (q_method_t)QSLIDER_singleStep);
   QC_QSlider->addMethod("setTracking",            (q_method_t)QSLIDER_setTracking);
   QC_QSlider->addMethod("hasTracking",            (q_method_t)QSLIDER_hasTracking);
   QC_QSlider->addMethod("setInvertedAppearance",  (q_method_t)QSLIDER_setInvertedAppearance);
   QC_QSlider->addMethod("invertedAppearance",     (q_method_t)QSLIDER_invertedAppearance);
   QC_QSlider->addMethod("setInvertedControls",    (q_method_t)QSLIDER_setInvertedControls);
   QC_QSlider->addMethod("invertedControls",       (q_method_t)QSLIDER_invertedControls);
   QC_QSlider->addMethod("setSliderDown",          (q_method_t)QSLIDER_setSliderDown);
   QC_QSlider->addMethod("isSliderDown",           (q_method_t)QSLIDER_isSliderDown);
   QC_QSlider->addMethod("setSLiderPosition",      (q_method_t)QSLIDER_setSliderPosition);
   QC_QSlider->addMethod("sliderPosition",         (q_method_t)QSLIDER_sliderPosition);
   QC_QSlider->addMethod("value",                  (q_method_t)QSLIDER_value);

   traceout("initQSliderClass()");
   return QC_QSlider;
}
