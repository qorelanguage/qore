/*
 QC_QGridLayout.cc
 
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

#include "QC_QGridLayout.h"
#include "QC_QFont.h"
#include "QC_QWidget.h"

int CID_QGRIDLAYOUT;

static void QGRIDL_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreQGridLayout *qw;
   QoreNode *p = test_param(params, NT_OBJECT, 0);
   QoreAbstractQWidget *parent = p ? (QoreAbstractQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;

   if (!parent)
      qw = new QoreQGridLayout();
   else 
   {
      ReferenceHolder<QoreAbstractQWidget> holder(parent, xsink);
      qw = new QoreQGridLayout(parent->getQWidget());
   }

   self->setPrivate(CID_QGRIDLAYOUT, qw);
}

static void QGRIDL_copy(class Object *self, class Object *old, class QoreQGridLayout *ql, ExceptionSink *xsink)
{
   xsink->raiseException("QGRIDLAYOUT-COPY-ERROR", "objects of this class cannot be copied");
}

QoreNode *QGRIDL_addWidget(class Object *self, QoreQGridLayout *ql, class QoreNode *params, ExceptionSink *xsink)
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
   if (is_nothing(p))
   {
      ql->getQLayout()->addWidget(widget->getQWidget());
      return 0;
   }
   int row = p->getAsInt();

   p = get_param(params, 2);
   int col = p ? p->getAsInt() : 0;

   p = get_param(params, 3);
   QoreNode *p1 = get_param(params, 4);
   if (is_nothing(p1))
   {
      //printd(5, "addWidget(%08x, %d, %d, %d)\n", widget->getQWidget(), row, col, p ? p->getAsInt() : 0);
      ql->qobj->addWidget(widget->getQWidget(), row, col, (Qt::Alignment)(p ? p->getAsInt() : 0));
      return 0;
   }
   int row_span = p ? p->getAsInt() : 0;
   int col_span = p1->getAsInt();
   p = get_param(params, 5);

   ql->qobj->addWidget(widget->getQWidget(), row, col, row_span, col_span, (Qt::Alignment)(p ? p->getAsInt() : 0));
   return 0;
}

class QoreClass *initQGridLayoutClass(class QoreClass *qlayout)
{
   tracein("initQGridLayoutClass()");
   
   class QoreClass *QC_QGridLayout = new QoreClass("QGridLayout", QDOM_GUI);
   CID_QGRIDLAYOUT = QC_QGridLayout->getID();

   QC_QGridLayout->addBuiltinVirtualBaseClass(qlayout);

   QC_QGridLayout->setConstructor(QGRIDL_constructor);
   QC_QGridLayout->setCopy((q_copy_t)QGRIDL_copy);

   QC_QGridLayout->addMethod("addWidget",   (q_method_t)QGRIDL_addWidget);

   traceout("initQGridLayoutClass()");
   return QC_QGridLayout;
}
