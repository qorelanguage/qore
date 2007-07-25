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
      qw = new QoreQFrame();
   else 
   {
      ReferenceHolder<QoreAbstractQWidget> holder(parent, xsink);
      p = get_param(params, 1);
      int window_flags = p ? p->getAsInt() : 0;
      qw = new QoreQFrame(parent->getQWidget(), (Qt::WindowFlags)window_flags);
   }

   self->setPrivate(CID_QFRAME, qw);
}

static void QFRAME_copy(class Object *self, class Object *old, class QoreQFrame *qw, ExceptionSink *xsink)
{
   xsink->raiseException("QFRAME-COPY-ERROR", "objects of this class cannot be copied");
}

class QoreClass *initQFrameClass(class QoreClass *qwidget)
{
   tracein("initQFrameClass()");
   
   class QoreClass *QC_QFrame = new QoreClass("QFrame", QDOM_GUI);
   CID_QFRAME = QC_QFrame->getID();

   QC_QFrame->addBuiltinVirtualBaseClass(qwidget);

   QC_QFrame->setConstructor(QFRAME_constructor);
   QC_QFrame->setCopy((q_copy_t)QFRAME_copy);

   traceout("initQFrameClass()");
   return QC_QFrame;
}
