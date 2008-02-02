/*
 QC_QFrame.cc
 
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

#include "QC_QFrame.h"
#include "QC_QRect.h"
#include "QC_QWidget.h"

int CID_QFRAME;

static void QFRAME_constructor(class QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreQFrame *qw;
   AbstractQoreNode *p = test_param(params, NT_OBJECT, 0);
   QoreAbstractQWidget *parent = p ? (QoreAbstractQWidget *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;

   if (!parent)
      qw = new QoreQFrame(self);
   else 
   {
      ReferenceHolder<QoreAbstractQWidget> holder(parent, xsink);
      p = get_param(params, 1);
      int window_flags = p ? p->getAsInt() : 0;
      qw = new QoreQFrame(self, parent->getQWidget(), (Qt::WindowFlags)window_flags);
   }

   self->setPrivate(CID_QFRAME, qw);
}

static void QFRAME_copy(class QoreObject *self, class QoreObject *old, class QoreQFrame *qw, ExceptionSink *xsink)
{
   xsink->raiseException("QFRAME-COPY-ERROR", "objects of this class cannot be copied");
}

//QRect frameRect () const
static AbstractQoreNode *QFRAME_frameRect(QoreObject *self, QoreAbstractQFrame *qf, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreQRect *q_qr = new QoreQRect(qf->getQFrame()->frameRect());
   QoreObject *o_qr = new QoreObject(QC_QRect, getProgram());
   o_qr->setPrivate(CID_QRECT, q_qr);
   return o_qr;
}

//Shadow frameShadow () const
static AbstractQoreNode *QFRAME_frameShadow(QoreObject *self, QoreAbstractQFrame *qf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qf->getQFrame()->frameShadow());
}

//Shape frameShape () const
static AbstractQoreNode *QFRAME_frameShape(QoreObject *self, QoreAbstractQFrame *qf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qf->getQFrame()->frameShape());
}

//int frameStyle () const
static AbstractQoreNode *QFRAME_frameStyle(QoreObject *self, QoreAbstractQFrame *qf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qf->getQFrame()->frameStyle());
}

//int frameWidth () const
static AbstractQoreNode *QFRAME_frameWidth(QoreObject *self, QoreAbstractQFrame *qf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qf->getQFrame()->frameWidth());
}

//int lineWidth () const
static AbstractQoreNode *QFRAME_lineWidth(QoreObject *self, QoreAbstractQFrame *qf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qf->getQFrame()->lineWidth());
}

//int midLineWidth () const
static AbstractQoreNode *QFRAME_midLineWidth(QoreObject *self, QoreAbstractQFrame *qf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qf->getQFrame()->midLineWidth());
}

//void setFrameRect ( const QRect & )
static AbstractQoreNode *QFRAME_setFrameRect(QoreObject *self, QoreAbstractQFrame *qf, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   QoreQRect *qrect = (p && p->type == NT_OBJECT) ? (QoreQRect *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QRECT, xsink) : 0;
   if (!qrect) {
      if (!xsink->isException())
         xsink->raiseException("QFRAME-SETFRAMERECT-PARAM-ERROR", "expecting a QRect object as first argument to QFrame::setFrameRect()");
      return 0;
   }
   ReferenceHolder<QoreQRect> holder(qrect, xsink);
   qf->getQFrame()->setFrameRect(*((QRect *)qrect));
   return 0;
}

//void setFrameShadow ( Shadow )
static AbstractQoreNode *QFRAME_setFrameShadow(QoreObject *self, QoreAbstractQFrame *qf, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   QFrame::Shadow shadow = (QFrame::Shadow)(p ? p->getAsInt() : 0);
   qf->getQFrame()->setFrameShadow(shadow);
   return 0;
}

//void setFrameShape ( Shape )
static AbstractQoreNode *QFRAME_setFrameShape(QoreObject *self, QoreAbstractQFrame *qf, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   QFrame::Shape shape = (QFrame::Shape)(p ? p->getAsInt() : 0);
   qf->getQFrame()->setFrameShape(shape);
   return 0;
}

//void setFrameStyle ( int style )
static AbstractQoreNode *QFRAME_setFrameStyle(QoreObject *self, QoreAbstractQFrame *qf, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   int style = p ? p->getAsInt() : 0;
   qf->getQFrame()->setFrameStyle(style);
   return 0;
}

//void setLineWidth ( int )
static AbstractQoreNode *QFRAME_setLineWidth(QoreObject *self, QoreAbstractQFrame *qf, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   int x = p ? p->getAsInt() : 0;
   qf->getQFrame()->setLineWidth(x);
   return 0;
}

//void setMidLineWidth ( int )
static AbstractQoreNode *QFRAME_setMidLineWidth(QoreObject *self, QoreAbstractQFrame *qf, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   int x = p ? p->getAsInt() : 0;
   qf->getQFrame()->setMidLineWidth(x);
   return 0;
}

class QoreClass *initQFrameClass(class QoreClass *qwidget)
{
   tracein("initQFrameClass()");
   
   class QoreClass *QC_QFrame = new QoreClass("QFrame", QDOM_GUI);
   CID_QFRAME = QC_QFrame->getID();

   QC_QFrame->addBuiltinVirtualBaseClass(qwidget);

   QC_QFrame->setConstructor(QFRAME_constructor);
   QC_QFrame->setCopy((q_copy_t)QFRAME_copy);

   QC_QFrame->addMethod("frameRect",                   (q_method_t)QFRAME_frameRect);
   QC_QFrame->addMethod("frameShadow",                 (q_method_t)QFRAME_frameShadow);
   QC_QFrame->addMethod("frameShape",                  (q_method_t)QFRAME_frameShape);
   QC_QFrame->addMethod("frameStyle",                  (q_method_t)QFRAME_frameStyle);
   QC_QFrame->addMethod("frameWidth",                  (q_method_t)QFRAME_frameWidth);
   QC_QFrame->addMethod("lineWidth",                   (q_method_t)QFRAME_lineWidth);
   QC_QFrame->addMethod("midLineWidth",                (q_method_t)QFRAME_midLineWidth);
   QC_QFrame->addMethod("setFrameRect",                (q_method_t)QFRAME_setFrameRect);
   QC_QFrame->addMethod("setFrameShadow",              (q_method_t)QFRAME_setFrameShadow);
   QC_QFrame->addMethod("setFrameShape",               (q_method_t)QFRAME_setFrameShape);
   QC_QFrame->addMethod("setFrameStyle",               (q_method_t)QFRAME_setFrameStyle);
   QC_QFrame->addMethod("setLineWidth",                (q_method_t)QFRAME_setLineWidth);
   QC_QFrame->addMethod("setMidLineWidth",             (q_method_t)QFRAME_setMidLineWidth);

   traceout("initQFrameClass()");
   return QC_QFrame;
}
