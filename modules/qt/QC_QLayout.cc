/*
 QC_QLayout.cc
 
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

#include "QC_QLayout.h"

int CID_QLAYOUT;

static void QL_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   xsink->raiseException("ABSTRACT-CLASS-ERROR", "QLayout is an abstract builtin class and cannot be directly instantiated or referenced by user code");
}

QoreNode *QL_addWidget(class Object *self, QoreAbstractQLayout *ql, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_OBJECT, 0);
   QoreAbstractQWidget *widget = p ? (QoreAbstractQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!widget)
   {
      if (!xsink->isException())
         xsink->raiseException("QLAYOUT-ADDWIDGET-ERROR", "expecting an object derived from QWidget as the only argument to QLayout::addWidget()");
      return 0;
   }
   ReferenceHolder<QoreAbstractQWidget> holder(widget, xsink);
   ql->getQLayout()->addWidget(widget->getQWidget());
   return 0;
}

class QoreClass *initQLayoutClass(class QoreClass *qobject)
{
   tracein("initQLayoutClass()");
   
   class QoreClass *QC_QLayout = new QoreClass("QLayout", QDOM_GUI);
   CID_QLAYOUT = QC_QLayout->getID();

   QC_QLayout->addBuiltinVirtualBaseClass(qobject);

   QC_QLayout->setConstructor(QL_constructor);
   QC_QLayout->addMethod("addWidget",       (q_method_t)QL_addWidget);

   traceout("initQLayoutClass()");
   return QC_QLayout;
}
