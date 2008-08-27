/*
 QC_QPaintEvent.cc
 
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

#include "QC_QPaintEvent.h"
#include "QC_QRegion.h"
#include "QC_QRect.h"

qore_classid_t CID_QPAINTEVENT;

class QoreClass *QC_QPaintEvent = 0;

static void QPAINTEVENT_constructor(class QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreQPaintEvent *qr;

   const QoreObject *p = test_object_param(params, 0);
      
   QoreQRect *rectangle = p ? (QoreQRect *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QRECT, xsink) : 0;
   if (*xsink)
      return;
   if (!rectangle)
   {
      QoreQRegion *region = p ? (QoreQRegion *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QREGION, xsink) : 0;
      if (!region) {
	 if (!xsink->isException())
	    xsink->raiseException("QPAINTEVENT-CONSTRUCTOR-ERROR", "Expecting a QRect or QRegion object as argument to QPaintEvent::constructor()");
	 return ;
      }
      ReferenceHolder<QoreQRegion> holder(region, xsink);
      qr = new QoreQPaintEvent(*region);
   }
   else {
      ReferenceHolder<QoreQRect> holder(rectangle, xsink);
      qr = new QoreQPaintEvent(*rectangle);
   }

   self->setPrivate(CID_QPAINTEVENT, qr);
}

static void QPAINTEVENT_copy(class QoreObject *self, class QoreObject *old, class QoreQPaintEvent *qr, ExceptionSink *xsink)
{
   xsink->raiseException("QPAINTEVENT-COPY-ERROR", "objects of this class cannot be copied");
}

//const QRect & rect () const
static AbstractQoreNode *QPAINTEVENT_rect(QoreObject *self, QoreQPaintEvent *qpe, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreQRect *q_qr = new QoreQRect(qpe->rect());
   QoreObject *o_qr = new QoreObject(QC_QRect, getProgram());
   o_qr->setPrivate(CID_QRECT, q_qr);
   return o_qr;
}

//const QRegion & region () const
static AbstractQoreNode *QPAINTEVENT_region(QoreObject *self, QoreQPaintEvent *qpe, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreQRegion *q_qr = new QoreQRegion(qpe->region());
   QoreObject *o_qr = new QoreObject(QC_QRegion, getProgram());
   o_qr->setPrivate(CID_QREGION, q_qr);
   return o_qr;
}

class QoreClass *initQPaintEventClass(class QoreClass *qevent)
{
   QORE_TRACE("initQPaintEventClass()");
   
   QC_QPaintEvent = new QoreClass("QPaintEvent", QDOM_GUI);
   CID_QPAINTEVENT = QC_QPaintEvent->getID();

   QC_QPaintEvent->addBuiltinVirtualBaseClass(qevent);

   QC_QPaintEvent->setConstructor(QPAINTEVENT_constructor);
   QC_QPaintEvent->setCopy((q_copy_t)QPAINTEVENT_copy);

   QC_QPaintEvent->addMethod("rect",                        (q_method_t)QPAINTEVENT_rect);
   QC_QPaintEvent->addMethod("region",                      (q_method_t)QPAINTEVENT_region);


   return QC_QPaintEvent;
}
