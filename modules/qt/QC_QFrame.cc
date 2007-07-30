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

int CID_QFRAME;

static void QFRAME_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreQFrame *qw;
   QoreNode *p = test_param(params, NT_OBJECT, 0);
   QoreAbstractQWidget *parent = p ? (QoreAbstractQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;

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

static void QFRAME_copy(class Object *self, class Object *old, class QoreQFrame *qw, ExceptionSink *xsink)
{
   xsink->raiseException("QFRAME-COPY-ERROR", "objects of this class cannot be copied");
}

//QRect frameRect () const
//static QoreNode *QFRAME_frameRect(Object *self, QoreQFrame *qf, QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreNode((int64)qf->qobj->frameRect());
//}

//Shadow frameShadow () const
static QoreNode *QFRAME_frameShadow(Object *self, QoreQFrame *qf, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qf->qobj->frameShadow());
}

//Shape frameShape () const
static QoreNode *QFRAME_frameShape(Object *self, QoreQFrame *qf, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qf->qobj->frameShape());
}

//int frameStyle () const
static QoreNode *QFRAME_frameStyle(Object *self, QoreQFrame *qf, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qf->qobj->frameStyle());
}

//int frameWidth () const
static QoreNode *QFRAME_frameWidth(Object *self, QoreQFrame *qf, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qf->qobj->frameWidth());
}

//int lineWidth () const
static QoreNode *QFRAME_lineWidth(Object *self, QoreQFrame *qf, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qf->qobj->lineWidth());
}

//int midLineWidth () const
static QoreNode *QFRAME_midLineWidth(Object *self, QoreQFrame *qf, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qf->qobj->midLineWidth());
}

//void setFrameRect ( const QRect & )
//static QoreNode *QFRAME_setFrameRect(Object *self, QoreQFrame *qf, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//     = ()(p ? p->getAsInt() : 0);
//   qf->qobj->setFrameRect();
//   return 0;
//}

//void setFrameShadow ( Shadow )
static QoreNode *QFRAME_setFrameShadow(Object *self, QoreQFrame *qf, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QFrame::Shadow shadow = (QFrame::Shadow)(p ? p->getAsInt() : 0);
   qf->qobj->setFrameShadow(shadow);
   return 0;
}

//void setFrameShape ( Shape )
static QoreNode *QFRAME_setFrameShape(Object *self, QoreQFrame *qf, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QFrame::Shape shape = (QFrame::Shape)(p ? p->getAsInt() : 0);
   qf->qobj->setFrameShape(shape);
   return 0;
}

//void setFrameStyle ( int style )
static QoreNode *QFRAME_setFrameStyle(Object *self, QoreQFrame *qf, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int style = p ? p->getAsInt() : 0;
   qf->qobj->setFrameStyle(style);
   return 0;
}

//void setLineWidth ( int )
static QoreNode *QFRAME_setLineWidth(Object *self, QoreQFrame *qf, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int x = p ? p->getAsInt() : 0;
   qf->qobj->setLineWidth(x);
   return 0;
}

//void setMidLineWidth ( int )
static QoreNode *QFRAME_setMidLineWidth(Object *self, QoreQFrame *qf, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int x = p ? p->getAsInt() : 0;
   qf->qobj->setMidLineWidth(x);
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

   //QC_QFrame->addMethod("frameRect",                   (q_method_t)QFRAME_frameRect);
   QC_QFrame->addMethod("frameShadow",                 (q_method_t)QFRAME_frameShadow);
   QC_QFrame->addMethod("frameShape",                  (q_method_t)QFRAME_frameShape);
   QC_QFrame->addMethod("frameStyle",                  (q_method_t)QFRAME_frameStyle);
   QC_QFrame->addMethod("frameWidth",                  (q_method_t)QFRAME_frameWidth);
   QC_QFrame->addMethod("lineWidth",                   (q_method_t)QFRAME_lineWidth);
   QC_QFrame->addMethod("midLineWidth",                (q_method_t)QFRAME_midLineWidth);
   //QC_QFrame->addMethod("setFrameRect",                (q_method_t)QFRAME_setFrameRect);
   QC_QFrame->addMethod("setFrameShadow",              (q_method_t)QFRAME_setFrameShadow);
   QC_QFrame->addMethod("setFrameShape",               (q_method_t)QFRAME_setFrameShape);
   QC_QFrame->addMethod("setFrameStyle",               (q_method_t)QFRAME_setFrameStyle);
   QC_QFrame->addMethod("setLineWidth",                (q_method_t)QFRAME_setLineWidth);
   QC_QFrame->addMethod("setMidLineWidth",             (q_method_t)QFRAME_setMidLineWidth);

   traceout("initQFrameClass()");
   return QC_QFrame;
}
