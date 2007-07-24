/*
 QC_QVBoxLayout.cc
 
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

#include "QC_QVBoxLayout.h"
#include "QC_QFont.h"
#include "QC_QWidget.h"

int CID_QVBOXLAYOUT;

static void QVBOXL_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreQVBoxLayout *qw;
   QoreNode *p = test_param(params, NT_OBJECT, 0);
   QoreAbstractQWidget *parent = p ? (QoreAbstractQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;

   if (!parent)
      qw = new QoreQVBoxLayout();
   else 
   {
      ReferenceHolder<QoreAbstractQWidget> holder(parent, xsink);
      qw = new QoreQVBoxLayout(parent->getQWidget());
   }

   self->setPrivate(CID_QVBOXLAYOUT, qw);
}

static void QVBOXL_destructor(class Object *self, class QoreQVBoxLayout *ql, ExceptionSink *xsink)
{
   ql->destructor(xsink);
   ql->deref(xsink);
}

static void QVBOXL_copy(class Object *self, class Object *old, class QoreQVBoxLayout *ql, ExceptionSink *xsink)
{
   xsink->raiseException("QVBOXLAYOUT-COPY-ERROR", "objects of this class cannot be copied");
}

class QoreClass *initQVBoxLayoutClass(class QoreClass *qlayout)
{
   tracein("initQVBoxLayoutClass()");
   
   class QoreClass *QC_QVBoxLayout = new QoreClass("QVBoxLayout", QDOM_GUI);
   CID_QVBOXLAYOUT = QC_QVBoxLayout->getID();

   QC_QVBoxLayout->addBuiltinVirtualBaseClass(qlayout);

   QC_QVBoxLayout->setConstructor(QVBOXL_constructor);
   QC_QVBoxLayout->setDestructor((q_destructor_t)QVBOXL_destructor);
   QC_QVBoxLayout->setCopy((q_copy_t)QVBOXL_copy);

   traceout("initQVBoxLayoutClass()");
   return QC_QVBoxLayout;
}
