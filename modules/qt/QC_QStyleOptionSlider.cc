/*
 QC_QStyleOptionSlider.cc
 
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

#include "QC_QStyleOptionSlider.h"

qore_classid_t CID_QSTYLEOPTIONSLIDER;
QoreClass *QC_QStyleOptionSlider = 0;

int QStyleOptionSlider_Notification(QoreObject *obj, QStyleOptionSlider *qsob, const char *mem, ExceptionSink *xsink)
{
   AbstractQoreNode *p;

   if (!strcmp(mem, "dialWrapping")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink)
	 return 0;

      bool dialWrapping = p ? p->getAsBool() : 0;
      qsob->dialWrapping = dialWrapping;
      return 0;
   }

   if (!strcmp(mem, "maximum")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink)
	 return 0;

      int maximum = p ? p->getAsInt() : 0;
      qsob->maximum = maximum;
      return 0;
   }

   if (!strcmp(mem, "minimum")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink)
	 return 0;

      int minimum = p ? p->getAsInt() : 0;
      qsob->minimum = minimum;
      return 0;
   }

   if (!strcmp(mem, "notchTarget")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink)
	 return 0;

      qreal notchTarget = p ? p->getAsInt() : 0;
      qsob->notchTarget = notchTarget;
      return 0;
   }

   if (!strcmp(mem, "orientation")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink)
	 return 0;

      Qt::Orientation orientation = (Qt::Orientation)(p ? p->getAsInt() : 0);
      qsob->orientation = orientation;
      return 0;
   }

   if (!strcmp(mem, "pageStep")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink)
	 return 0;

      int pageStep = p ? p->getAsInt() : 0;
      qsob->pageStep = pageStep;
      return 0;
   }

   if (!strcmp(mem, "singleStep")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink)
	 return 0;

      int singleStep = p ? p->getAsInt() : 0;
      qsob->singleStep = singleStep;
      return 0;
   }

   if (!strcmp(mem, "sliderPosition")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink)
	 return 0;

      int sliderPosition = p ? p->getAsInt() : 0;
      qsob->sliderPosition = sliderPosition;
      return 0;
   }

   if (!strcmp(mem, "sliderValue")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink)
	 return 0;

      int sliderValue = p ? p->getAsInt() : 0;
      qsob->sliderValue = sliderValue;
      return 0;
   }

   if (!strcmp(mem, "tickInterval")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink)
	 return 0;

      int tickInterval = p ? p->getAsInt() : 0;
      qsob->tickInterval = tickInterval;
      return 0;
   }

   if (!strcmp(mem, "tickPosition")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink)
	 return 0;

      QSlider::TickPosition tickPosition = (QSlider::TickPosition)(p ? p->getAsInt() : 0);
      qsob->tickPosition = tickPosition;
      return 0;
   }

   if (!strcmp(mem, "upsideDown")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink)
	 return 0;

      bool upsideDown = p ? p->getAsBool() : 0;
      qsob->upsideDown = upsideDown;
      return 0;
   }

   return -1;
}

AbstractQoreNode *QStyleOptionSlider_MemberGate(QStyleOptionSlider *qsob, const char *mem)
{
   if (!strcmp(mem, "dialWrapping"))
      return get_bool_node(qsob->dialWrapping);

   if (!strcmp(mem, "maximum"))
      return new QoreBigIntNode(qsob->maximum);

   if (!strcmp(mem, "minimum"))
      return new QoreBigIntNode(qsob->minimum);

   if (!strcmp(mem, "notchTarget"))
      return new QoreFloatNode(qsob->notchTarget);

   if (!strcmp(mem, "orientation"))
      return new QoreBigIntNode(qsob->orientation);

   if (!strcmp(mem, "pageStep"))
      return new QoreBigIntNode(qsob->pageStep);

   if (!strcmp(mem, "singleStep"))
      return new QoreBigIntNode(qsob->singleStep);

   if (!strcmp(mem, "sliderPosition"))
      return new QoreBigIntNode(qsob->sliderPosition);

   if (!strcmp(mem, "sliderValue"))
      return new QoreBigIntNode(qsob->sliderValue);

   if (!strcmp(mem, "tickInterval"))
      return new QoreBigIntNode(qsob->tickInterval);

   if (!strcmp(mem, "tickPosition"))
      return new QoreBigIntNode(qsob->tickPosition);

   if (!strcmp(mem, "upsideDown"))
      return get_bool_node(qsob->upsideDown);

   return 0;
}

static AbstractQoreNode *QSTYLEOPTIONSLIDER_memberNotification(QoreObject *self, QoreQStyleOptionSlider *qsob, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *str = test_string_param(params, 0);
   if (!str || !str->strlen())
      return 0;

   const char *member = str->getBuffer();
   if (!QStyleOptionSlider_Notification(self, qsob, member, xsink) || *xsink)
      return 0;

   QStyleOption_Notification(self, qsob, member, xsink);
   return 0;
}

static AbstractQoreNode *QSTYLEOPTIONSLIDER_memberGate(QoreObject *self, QoreQStyleOptionSlider *qsob, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *str = test_string_param(params, 0);
   if (!str || !str->strlen())
      return 0;

   const char *member = str->getBuffer();
   AbstractQoreNode *rv = QStyleOptionSlider_MemberGate(qsob, member);
   if (rv)
      return rv;

   return QStyleOption_MemberGate(qsob, member);
}

//QStyleOptionSlider ()
//QStyleOptionSlider ( const QStyleOptionSlider & other )
static void QSTYLEOPTIONSLIDER_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_QSTYLEOPTIONSLIDER, new QoreQStyleOptionSlider());
}

static void QSTYLEOPTIONSLIDER_copy(class QoreObject *self, class QoreObject *old, class QoreQStyleOptionSlider *qsos, ExceptionSink *xsink)
{
   self->setPrivate(CID_QSTYLEOPTIONSLIDER, new QoreQStyleOptionSlider(*qsos));
}

QoreClass *initQStyleOptionSliderClass(QoreClass *qstyleoptioncomplex)
{
   QC_QStyleOptionSlider = new QoreClass("QStyleOptionSlider", QDOM_GUI);
   CID_QSTYLEOPTIONSLIDER = QC_QStyleOptionSlider->getID();

   QC_QStyleOptionSlider->addBuiltinVirtualBaseClass(qstyleoptioncomplex);

   QC_QStyleOptionSlider->setConstructor(QSTYLEOPTIONSLIDER_constructor);
   QC_QStyleOptionSlider->setCopy((q_copy_t)QSTYLEOPTIONSLIDER_copy);

   // add special methods
   QC_QStyleOptionSlider->addMethod("memberNotification",          (q_method_t)QSTYLEOPTIONSLIDER_memberNotification);
   QC_QStyleOptionSlider->addMethod("memberGate",                  (q_method_t)QSTYLEOPTIONSLIDER_memberGate);

   return QC_QStyleOptionSlider;
}
