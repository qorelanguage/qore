/*
 QC_QBoxLayout.cc
 
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

#include "QC_QBoxLayout.h"

int CID_QBOXLAYOUT;

static void QBOXLAYOUT_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int direction = p ? p->getAsInt() : 0;

   QoreQBoxLayout *qw;
   p = test_param(params, NT_OBJECT, 0);
   QoreAbstractQWidget *parent = p ? (QoreAbstractQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;

   if (!parent)
      qw = new QoreQBoxLayout((QBoxLayout::Direction)direction);
   else 
   {
      ReferenceHolder<QoreAbstractQWidget> holder(parent, xsink);
      qw = new QoreQBoxLayout((QBoxLayout::Direction)direction, parent->getQWidget());
   }

   self->setPrivate(CID_QBOXLAYOUT, qw);
}

static void QBOXLAYOUT_destructor(class Object *self, class QoreQBoxLayout *ql, ExceptionSink *xsink)
{
   ql->destructor(xsink);
   ql->deref(xsink);
}

static void QBOXLAYOUT_copy(class Object *self, class Object *old, class QoreQBoxLayout *ql, ExceptionSink *xsink)
{
   xsink->raiseException("QBOXLAYOUT-COPY-ERROR", "objects of this class cannot be copied");
}

static QoreNode *QBOXLAYOUT_addLayout(class Object *self, QoreAbstractQBoxLayout *qbl, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p = test_param(params, NT_OBJECT, 0);
   QoreAbstractQLayout *qal = p ? (QoreAbstractQLayout *)p->val.object->getReferencedPrivateData(CID_QLAYOUT, xsink) : NULL;
   if (!p || !qal)
   {
      if (!xsink->isException())
         xsink->raiseException("QBOXLAYOUT-ADDLAYOUT-PARAM-ERROR", "expecting a QLayout object as argument to QBoxLayout::addLayout()");
      return NULL;
   }
   ReferenceHolder<QoreAbstractQLayout> holder(qal, xsink);

   p = get_param(params, 1);
   int stretch = p ? p->getAsInt() : 0;

   qbl->getQBoxLayout()->addLayout(qal->getQLayout(), stretch);
   return 0;
}

QoreNode *QBOXLAYOUT_addWidget(class Object *self, QoreAbstractQBoxLayout *qbl, class QoreNode *params, ExceptionSink *xsink)
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

   p = get_param(params, 1);
   int stretch = p ? p->getAsInt() : 0;

   p = get_param(params, 2);
   int alignment = p ? p->getAsInt() : 0;

   qbl->getQBoxLayout()->addWidget(widget->getQWidget(), stretch, (Qt::Alignment)alignment);
   return 0;
}

class QoreClass *initQBoxLayoutClass(class QoreClass *qlayout)
{
   tracein("initQBoxLayoutClass()");
   
   class QoreClass *QC_QBoxLayout = new QoreClass("QBoxLayout", QDOM_GUI);
   CID_QBOXLAYOUT = QC_QBoxLayout->getID();

   QC_QBoxLayout->addBuiltinVirtualBaseClass(qlayout);

   QC_QBoxLayout->setConstructor(QBOXLAYOUT_constructor);
   QC_QBoxLayout->setDestructor((q_destructor_t)QBOXLAYOUT_destructor);
   QC_QBoxLayout->setCopy((q_copy_t)QBOXLAYOUT_copy);

   QC_QBoxLayout->addMethod("addLayout",     (q_method_t)QBOXLAYOUT_addLayout);
   QC_QBoxLayout->addMethod("addWidget",     (q_method_t)QBOXLAYOUT_addWidget);

   traceout("initQBoxLayoutClass()");
   return QC_QBoxLayout;
}
