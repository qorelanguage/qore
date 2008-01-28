/*
 QC_QDial.cc
 
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

#include "QC_QDial.h"
#include "QC_QWidget.h"
#include "QC_QStyleOptionSlider.h"

#include "qore-qt.h"

int CID_QDIAL;
class QoreClass *QC_QDial = 0;

//QDial ( QWidget * parent = 0 )
static void QDIAL_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQWidget *parent = (p && p->type == NT_OBJECT) ? (QoreQWidget *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return;
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   self->setPrivate(CID_QDIAL, new QoreQDial(self, parent ? static_cast<QWidget *>(parent->getQWidget()) : 0));
   return;
}

static void QDIAL_copy(class QoreObject *self, class QoreObject *old, class QoreQDial *qd, ExceptionSink *xsink)
{
   xsink->raiseException("QDIAL-COPY-ERROR", "objects of this class cannot be copied");
}

//int notchSize () const
static QoreNode *QDIAL_notchSize(QoreObject *self, QoreQDial *qd, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qd->qobj->notchSize());
}

//qreal notchTarget () const
static QoreNode *QDIAL_notchTarget(QoreObject *self, QoreQDial *qd, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreNode((double)qd->qobj->notchTarget());
}

//bool notchesVisible () const
static QoreNode *QDIAL_notchesVisible(QoreObject *self, QoreQDial *qd, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qd->qobj->notchesVisible());
}

//void setNotchTarget ( double target )
static QoreNode *QDIAL_setNotchTarget(QoreObject *self, QoreQDial *qd, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   double target = p ? p->getAsFloat() : 0.0;
   qd->qobj->setNotchTarget(target);
   return 0;
}

//bool wrapping () const
static QoreNode *QDIAL_wrapping(QoreObject *self, QoreQDial *qd, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qd->qobj->wrapping());
}

//void setNotchesVisible ( bool visible )
static QoreNode *QDIAL_setNotchesVisible(QoreObject *self, QoreQDial *qd, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool visible = p ? p->getAsBool() : false;
   qd->qobj->setNotchesVisible(visible);
   return 0;
}

//void setWrapping ( bool on )
static QoreNode *QDIAL_setWrapping(QoreObject *self, QoreQDial *qd, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool on = p ? p->getAsBool() : false;
   qd->qobj->setWrapping(on);
   return 0;
}

//void initStyleOption ( QStyleOptionSlider * option ) const
static QoreNode *QDIAL_initStyleOption(QoreObject *self, QoreQDial *qd, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQStyleOptionSlider *option = (p && p->type == NT_OBJECT) ? (QoreQStyleOptionSlider *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QSTYLEOPTIONSLIDER, xsink) : 0;
   if (!option) {
      if (!xsink->isException())
         xsink->raiseException("QDIAL-INITSTYLEOPTION-PARAM-ERROR", "expecting a QStyleOptionSlider object as first argument to QDial::initStyleOption()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);
   qd->qobj->pub_initStyleOption(static_cast<QStyleOptionSlider *>(option));
   return 0;
}

QoreClass *initQDialClass(QoreClass *qabstractslider)
{
   QC_QDial = new QoreClass("QDial", QDOM_GUI);
   CID_QDIAL = QC_QDial->getID();

   QC_QDial->addBuiltinVirtualBaseClass(qabstractslider);

   QC_QDial->setConstructor(QDIAL_constructor);
   QC_QDial->setCopy((q_copy_t)QDIAL_copy);

   QC_QDial->addMethod("notchSize",                   (q_method_t)QDIAL_notchSize);
   QC_QDial->addMethod("notchTarget",                 (q_method_t)QDIAL_notchTarget);
   QC_QDial->addMethod("notchesVisible",              (q_method_t)QDIAL_notchesVisible);
   QC_QDial->addMethod("setNotchTarget",              (q_method_t)QDIAL_setNotchTarget);
   QC_QDial->addMethod("wrapping",                    (q_method_t)QDIAL_wrapping);
   QC_QDial->addMethod("setNotchesVisible",           (q_method_t)QDIAL_setNotchesVisible);
   QC_QDial->addMethod("setWrapping",                 (q_method_t)QDIAL_setWrapping);

   // private methods
   QC_QDial->addMethod("initStyleOption",             (q_method_t)QDIAL_initStyleOption, true);

   return QC_QDial;
}
