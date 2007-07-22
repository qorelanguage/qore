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
#include "QC_QFont.h"

int CID_QFRAME;

static void QFRAME_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreQFrame *qw;
   QoreNode *p = test_param(params, NT_OBJECT, 0);
   QoreAbstractQWidget *parent = p ? (QoreAbstractQWidget *)p->val.object->getReferencedPrivateDataFromMetaClass(CID_QWIDGET, xsink) : 0;

   if (!parent)
      qw = new QoreQFrame();
   else 
   {
      ReferenceHolder<QoreAbstractQWidget> holder(parent, xsink);
      p = get_param(params, 1);
      int window_flags = p ? p->getAsInt() : 0;
      qw = new QoreQFrame(parent->getQWidget(), (Qt::WindowFlags)window_flags);
   }

   int_set_t *mks = new int_set_t;
   mks->insert(CID_QOBJECT);
   mks->insert(CID_QWIDGET);

   self->setPrivate(CID_QFRAME, mks, qw);
}

static void QFRAME_destructor(class Object *self, class QoreQFrame *qw, ExceptionSink *xsink)
{
   qw->destructor(xsink);
   qw->deref(xsink);
}

static void QFRAME_copy(class Object *self, class Object *old, class QoreQFrame *qw, ExceptionSink *xsink)
{
   xsink->raiseException("QFRAME-COPY-ERROR", "objects of this class cannot be copied");
}

typedef QoreNode *(*qf_func_t)(Object *, QoreQFrame *, QoreNode *, ExceptionSink *);

class QoreClass *initQFrameClass()
{
   tracein("initQFrameClass()");
   
   class QoreClass *QC_QFrame = new QoreClass("QFrame", QDOM_GUI);
   CID_QFRAME = QC_QFrame->getID();
   QC_QFrame->setConstructor(QFRAME_constructor);
   QC_QFrame->setDestructor((q_destructor_t)QFRAME_destructor);
   QC_QFrame->setCopy((q_copy_t)QFRAME_copy);

   // inherited functions from templates
   QC_QFrame->addMethod("inherits",          (q_method_t)(qf_func_t)QO_inherits<QoreQFrame>);
   QC_QFrame->addMethod("resize",            (q_method_t)(qf_func_t)QW_resize<QoreQFrame>);
   QC_QFrame->addMethod("setGeometry",       (q_method_t)(qf_func_t)QW_setGeometry<QoreQFrame>);
   QC_QFrame->addMethod("show",              (q_method_t)(qf_func_t)QW_show<QoreQFrame>);
   QC_QFrame->addMethod("setFont",           (q_method_t)(qf_func_t)QW_setFont<QoreQFrame>);
   QC_QFrame->addMethod("setFixedHeight",    (q_method_t)(qf_func_t)QW_setFixedHeight<QoreQFrame>);
   QC_QFrame->addMethod("setFixedWidth",     (q_method_t)(qf_func_t)QW_setFixedWidth<QoreQFrame>);
   QC_QFrame->addMethod("setFixedSize",      (q_method_t)(qf_func_t)QW_setFixedSize<QoreQFrame>);
   QC_QFrame->addMethod("setMinimumHeight",  (q_method_t)(qf_func_t)QW_setMinimumHeight<QoreQFrame>);
   QC_QFrame->addMethod("setMinimumWidth",   (q_method_t)(qf_func_t)QW_setMinimumWidth<QoreQFrame>);
   QC_QFrame->addMethod("setMinimumSize",    (q_method_t)(qf_func_t)QW_setMinimumSize<QoreQFrame>);
   QC_QFrame->addMethod("setMaximumHeight",  (q_method_t)(qf_func_t)QW_setMaximumHeight<QoreQFrame>);
   QC_QFrame->addMethod("setMaximumWidth",   (q_method_t)(qf_func_t)QW_setMaximumWidth<QoreQFrame>);
   QC_QFrame->addMethod("setMaximumSize",    (q_method_t)(qf_func_t)QW_setMaximumSize<QoreQFrame>);
   
   traceout("initQFrameClass()");
   return QC_QFrame;
}
