/*
 QC_QStyleOptionTabWidgetFrame.cc
 
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

#include "QC_QStyleOptionTabWidgetFrame.h"

qore_classid_t CID_QSTYLEOPTIONTABWIDGETFRAME;
QoreClass *QC_QStyleOptionTabWidgetFrame = 0;

int QStyleOptionTabWidgetFrame_Notification(QoreObject *obj, QStyleOptionTabWidgetFrame *qsotwf, const char *mem, ExceptionSink *xsink)
{
   AbstractQoreNode *p;

   if (!strcmp(mem, "leftCornerWidgetSize")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink || !p || p->getType() != NT_OBJECT)
	 return 0;

      QoreObject *o = reinterpret_cast<QoreObject *>(p);
      QoreQSize *leftCornerWidgetSize = (QoreQSize *)o->getReferencedPrivateData(CID_QSIZE, xsink);
      if (!leftCornerWidgetSize)
	 return 0;
      ReferenceHolder<AbstractPrivateData> sizeHolder(static_cast<AbstractPrivateData *>(leftCornerWidgetSize), xsink);
      qsotwf->leftCornerWidgetSize = *(static_cast<QSize *>(leftCornerWidgetSize));
      return 0;
   }

   if (!strcmp(mem, "lineWidth")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink)
	 return 0;

      int lineWidth = p ? p->getAsInt() : 0;
      qsotwf->lineWidth = lineWidth;
      return 0;
   }

   if (!strcmp(mem, "midLineWidth")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink)
	 return 0;

      int midLineWidth = p ? p->getAsInt() : 0;
      qsotwf->midLineWidth = midLineWidth;
      return 0;
   }

   if (!strcmp(mem, "rightCornerWidgetSize")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink || !p || p->getType() != NT_OBJECT)
	 return 0;

      QoreObject *o = reinterpret_cast<QoreObject *>(p);
      QoreQSize *rightCornerWidgetSize = (QoreQSize *)o->getReferencedPrivateData(CID_QSIZE, xsink);
      if (!rightCornerWidgetSize)
	 return 0;
      ReferenceHolder<AbstractPrivateData> sizeHolder(static_cast<AbstractPrivateData *>(rightCornerWidgetSize), xsink);
      qsotwf->rightCornerWidgetSize = *(static_cast<QSize *>(rightCornerWidgetSize));
      return 0;
   }

   if (!strcmp(mem, "shape")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink)
	 return 0;

      QTabBar::Shape shape = (QTabBar::Shape)(p ? p->getAsInt() : 0);
      qsotwf->shape = shape;
      return 0;
   }

   if (!strcmp(mem, "tabBarSize")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink || !p || p->getType() != NT_OBJECT)
	 return 0;

      QoreObject *o = reinterpret_cast<QoreObject *>(p);
      QoreQSize *tabBarSize = (QoreQSize *)o->getReferencedPrivateData(CID_QSIZE, xsink);
      if (!tabBarSize)
	 return 0;
      ReferenceHolder<AbstractPrivateData> sizeHolder(static_cast<AbstractPrivateData *>(tabBarSize), xsink);
      qsotwf->tabBarSize = *(static_cast<QSize *>(tabBarSize));
      return 0;
   }

   return -1;
}

AbstractQoreNode *QStyleOptionTabWidgetFrame_MemberGate(QStyleOptionTabWidgetFrame *qsotwf, const char *mem)
{
   if (!strcmp(mem, "leftCornerWidgetSize"))
      return return_object(QC_QSize, new QoreQSize(qsotwf->leftCornerWidgetSize));

   if (!strcmp(mem, "lineWidth"))
      return new QoreBigIntNode(qsotwf->lineWidth);

   if (!strcmp(mem, "midLineWidth"))
      return new QoreBigIntNode(qsotwf->midLineWidth);

   if (!strcmp(mem, "rightCornerWidgetSize"))
      return return_object(QC_QSize, new QoreQSize(qsotwf->rightCornerWidgetSize));

   if (!strcmp(mem, "shape"))
      return new QoreBigIntNode(qsotwf->shape);

   if (!strcmp(mem, "tabBarSize"))
      return return_object(QC_QSize, new QoreQSize(qsotwf->tabBarSize));

   return 0;
}

static AbstractQoreNode *QSTYLEOPTIONTABWIDGETFRAME_memberNotification(QoreObject *self, QoreQStyleOptionTabWidgetFrame *qsotwf, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *str = test_string_param(params, 0);
   if (!str || !str->strlen())
      return 0;

   const char *member = str->getBuffer();
   if (!QStyleOptionTabWidgetFrame_Notification(self, qsotwf, member, xsink) || *xsink)
      return 0;

   QStyleOption_Notification(self, qsotwf, member, xsink);
   return 0;
}

static AbstractQoreNode *QSTYLEOPTIONTABWIDGETFRAME_memberGate(QoreObject *self, QoreQStyleOptionTabWidgetFrame *qsotwf, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *str = test_string_param(params, 0);
   if (!str || !str->strlen())
      return 0;

   const char *member = str->getBuffer();
   AbstractQoreNode *rv = QStyleOptionTabWidgetFrame_MemberGate(qsotwf, member);
   if (rv)
      return rv;

   return QStyleOption_MemberGate(qsotwf, member);
}

//QStyleOptionTabWidgetFrame ()
//QStyleOptionTabWidgetFrame ( const QStyleOptionTabWidgetFrame & other )
static void QSTYLEOPTIONTABWIDGETFRAME_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_QSTYLEOPTIONTABWIDGETFRAME, new QoreQStyleOptionTabWidgetFrame());
}

static void QSTYLEOPTIONTABWIDGETFRAME_copy(class QoreObject *self, class QoreObject *old, class QoreQStyleOptionTabWidgetFrame *qsotwf, ExceptionSink *xsink)
{
   self->setPrivate(CID_QSTYLEOPTIONTABWIDGETFRAME, new QoreQStyleOptionTabWidgetFrame(*qsotwf));
}

QoreClass *initQStyleOptionTabWidgetFrameClass(QoreClass *qstyleoption)
{
   QC_QStyleOptionTabWidgetFrame = new QoreClass("QStyleOptionTabWidgetFrame", QDOM_GUI);
   CID_QSTYLEOPTIONTABWIDGETFRAME = QC_QStyleOptionTabWidgetFrame->getID();

   QC_QStyleOptionTabWidgetFrame->addBuiltinVirtualBaseClass(qstyleoption);

   QC_QStyleOptionTabWidgetFrame->setConstructor(QSTYLEOPTIONTABWIDGETFRAME_constructor);
   QC_QStyleOptionTabWidgetFrame->setCopy((q_copy_t)QSTYLEOPTIONTABWIDGETFRAME_copy);

   // add special methods
   QC_QStyleOptionTabWidgetFrame->addMethod("memberNotification",    (q_method_t)QSTYLEOPTIONTABWIDGETFRAME_memberNotification);
   QC_QStyleOptionTabWidgetFrame->addMethod("memberGate",            (q_method_t)QSTYLEOPTIONTABWIDGETFRAME_memberGate);

   return QC_QStyleOptionTabWidgetFrame;
}
