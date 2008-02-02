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
#include "QC_QLayout.h"

int CID_QGRIDLAYOUT;

static void QGRIDLAYOUT_constructor(class QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreQGridLayout *qw;
   AbstractQoreNode *p = test_param(params, NT_OBJECT, 0);
   QoreAbstractQWidget *parent = p ? (QoreAbstractQWidget *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;

   if (!parent)
      qw = new QoreQGridLayout(self);
   else 
   {
      ReferenceHolder<QoreAbstractQWidget> holder(parent, xsink);
      qw = new QoreQGridLayout(self, parent->getQWidget());
   }

   self->setPrivate(CID_QGRIDLAYOUT, qw);
}

static void QGRIDLAYOUT_copy(class QoreObject *self, class QoreObject *old, class QoreQGridLayout *ql, ExceptionSink *xsink)
{
   xsink->raiseException("QGRIDLAYOUT-COPY-ERROR", "objects of this class cannot be copied");
}

//void addItem ( QLayoutItem * item, int row, int column, int rowSpan = 1, int columnSpan = 1, Qt::Alignment alignment = 0 )
//static AbstractQoreNode *QGRIDLAYOUT_addItem(QoreObject *self, QoreQGridLayout *qgl, const QoreListNode *params, ExceptionSink *xsink)
//{
//   AbstractQoreNode *p = get_param(params, 0);
//   ??? QLayoutItem* item = p;
//   p = get_param(params, 1);
//   int row = p ? p->getAsInt() : 0;
//   p = get_param(params, 2);
//   int column = p ? p->getAsInt() : 0;
//   p = get_param(params, 3);
//   int rowSpan = !is_nothing(p) ? p->getAsInt() : 1;
//   p = get_param(params, 4);
//   int columnSpan = !is_nothing(p) ? p->getAsInt() : 1;
//   p = get_param(params, 5);
//   Qt::Alignment alignment = (Qt::Alignment)(p ? p->getAsInt() : 0);
//   qgl->qobj->addItem(item, row, column, rowSpan, columnSpan, alignment);
//   return 0;
//}

//void addLayout ( QLayout * layout, int row, int column, int rowSpan, int columnSpan, Qt::Alignment alignment = 0 )
//void addLayout ( QLayout * layout, int row, int column, Qt::Alignment alignment = 0 )
AbstractQoreNode *QGRIDLAYOUT_addLayout(class QoreObject *self, QoreQGridLayout *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = test_param(params, NT_OBJECT, 0);
   QoreAbstractQLayout *layout = p ? (QoreAbstractQLayout *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QLAYOUT, xsink) : 0;
   if (!layout)
   {
      if (!xsink->isException())
         xsink->raiseException("QLAYOUT-ADDLAYOUT-ERROR", "expecting an object derived from QLayout as the only argument to QLayout::addLayout()");
      return 0;
   }
   ReferenceHolder<QoreAbstractQLayout> holder(layout, xsink);

   p = get_param(params, 1);
   int row = p? p->getAsInt() : 0;

   p = get_param(params, 2);
   int col = p ? p->getAsInt() : 0;

   p = get_param(params, 3);
   AbstractQoreNode *p1 = get_param(params, 4);
   if (is_nothing(p1))
   {
      //printd(5, "addLayout(%08x, %d, %d, %d)\n", layout->getQLayout(), row, col, p ? p->getAsInt() : 0);
      ql->qobj->addLayout(layout->getQLayout(), row, col, (Qt::Alignment)(p ? p->getAsInt() : 0));
      return 0;
   }
   int row_span = p ? p->getAsInt() : 0;
   int col_span = p1->getAsInt();
   p = get_param(params, 5);

   ql->qobj->addLayout(layout->getQLayout(), row, col, row_span, col_span, (Qt::Alignment)(p ? p->getAsInt() : 0));
   return 0;
}

//void addWidget ( QWidget * widget, int row, int column, Qt::Alignment alignment = 0 )
//void addWidget ( QWidget * widget, int fromRow, int fromColumn, int rowSpan, int columnSpan, Qt::Alignment alignment = 0 )
AbstractQoreNode *QGRIDLAYOUT_addWidget(class QoreObject *self, QoreQGridLayout *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = test_param(params, NT_OBJECT, 0);
   QoreAbstractQWidget *widget = p ? (QoreAbstractQWidget *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
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
   AbstractQoreNode *p1 = get_param(params, 4);
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

//QRect cellRect ( int row, int column ) const
//static AbstractQoreNode *QGRIDLAYOUT_cellRect(QoreObject *self, QoreQGridLayout *qgl, const QoreListNode *params, ExceptionSink *xsink)
//{
//   AbstractQoreNode *p = get_param(params, 0);
//   int row = p ? p->getAsInt() : 0;
//   p = get_param(params, 1);
//   int column = p ? p->getAsInt() : 0;
//   ??? return new QoreBigIntNode(qgl->qobj->cellRect(row, column));
//}

//int columnCount () const
static AbstractQoreNode *QGRIDLAYOUT_columnCount(QoreObject *self, QoreQGridLayout *qgl, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qgl->qobj->columnCount());
}

//int columnMinimumWidth ( int column ) const
static AbstractQoreNode *QGRIDLAYOUT_columnMinimumWidth(QoreObject *self, QoreQGridLayout *qgl, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   int column = p ? p->getAsInt() : 0;
   return new QoreBigIntNode(qgl->qobj->columnMinimumWidth(column));
}

//int columnStretch ( int column ) const
static AbstractQoreNode *QGRIDLAYOUT_columnStretch(QoreObject *self, QoreQGridLayout *qgl, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   int column = p ? p->getAsInt() : 0;
   return new QoreBigIntNode(qgl->qobj->columnStretch(column));
}

//void getItemPosition ( int index, int * row, int * column, int * rowSpan, int * columnSpan )
//static AbstractQoreNode *QGRIDLAYOUT_getItemPosition(QoreObject *self, QoreQGridLayout *qgl, const QoreListNode *params, ExceptionSink *xsink)
//{
//   AbstractQoreNode *p = get_param(params, 0);
//   int index = p ? p->getAsInt() : 0;
//   p = get_param(params, 1);
//   ??? int* row = p;
//   p = get_param(params, 2);
//   ??? int* column = p;
//   p = get_param(params, 3);
//   ??? int* rowSpan = p;
//   p = get_param(params, 4);
//   ??? int* columnSpan = p;
//   qgl->qobj->getItemPosition(index, row, column, rowSpan, columnSpan);
//   return 0;
//}

//int horizontalSpacing () const
static AbstractQoreNode *QGRIDLAYOUT_horizontalSpacing(QoreObject *self, QoreQGridLayout *qgl, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qgl->qobj->horizontalSpacing());
}

//Qt::Corner originCorner () const
//static AbstractQoreNode *QGRIDLAYOUT_originCorner(QoreObject *self, QoreQGridLayout *qgl, const QoreListNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreBigIntNode(qgl->qobj->originCorner());
//}

//int rowCount () const
static AbstractQoreNode *QGRIDLAYOUT_rowCount(QoreObject *self, QoreQGridLayout *qgl, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qgl->qobj->rowCount());
}

//int rowMinimumHeight ( int row ) const
static AbstractQoreNode *QGRIDLAYOUT_rowMinimumHeight(QoreObject *self, QoreQGridLayout *qgl, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   int row = p ? p->getAsInt() : 0;
   return new QoreBigIntNode(qgl->qobj->rowMinimumHeight(row));
}

//int rowStretch ( int row ) const
static AbstractQoreNode *QGRIDLAYOUT_rowStretch(QoreObject *self, QoreQGridLayout *qgl, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   int row = p ? p->getAsInt() : 0;
   return new QoreBigIntNode(qgl->qobj->rowStretch(row));
}

//void setColumnMinimumWidth ( int column, int minSize )
static AbstractQoreNode *QGRIDLAYOUT_setColumnMinimumWidth(QoreObject *self, QoreQGridLayout *qgl, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   int column = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int minSize = p ? p->getAsInt() : 0;
   qgl->qobj->setColumnMinimumWidth(column, minSize);
   return 0;
}

//void setColumnStretch ( int column, int stretch )
static AbstractQoreNode *QGRIDLAYOUT_setColumnStretch(QoreObject *self, QoreQGridLayout *qgl, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   int column = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int stretch = p ? p->getAsInt() : 0;
   qgl->qobj->setColumnStretch(column, stretch);
   return 0;
}

//void setHorizontalSpacing ( int spacing )
static AbstractQoreNode *QGRIDLAYOUT_setHorizontalSpacing(QoreObject *self, QoreQGridLayout *qgl, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   int spacing = p ? p->getAsInt() : 0;
   qgl->qobj->setHorizontalSpacing(spacing);
   return 0;
}

//void setOriginCorner ( Qt::Corner corner )
static AbstractQoreNode *QGRIDLAYOUT_setOriginCorner(QoreObject *self, QoreQGridLayout *qgl, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   Qt::Corner corner = (Qt::Corner)(p ? p->getAsInt() : 0);
   qgl->qobj->setOriginCorner(corner);
   return 0;
}

//void setRowMinimumHeight ( int row, int minSize )
static AbstractQoreNode *QGRIDLAYOUT_setRowMinimumHeight(QoreObject *self, QoreQGridLayout *qgl, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   int row = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int minSize = p ? p->getAsInt() : 0;
   qgl->qobj->setRowMinimumHeight(row, minSize);
   return 0;
}

//void setRowStretch ( int row, int stretch )
static AbstractQoreNode *QGRIDLAYOUT_setRowStretch(QoreObject *self, QoreQGridLayout *qgl, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   int row = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int stretch = p ? p->getAsInt() : 0;
   qgl->qobj->setRowStretch(row, stretch);
   return 0;
}

//void setSpacing ( int spacing )
static AbstractQoreNode *QGRIDLAYOUT_setSpacing(QoreObject *self, QoreQGridLayout *qgl, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   int spacing = p ? p->getAsInt() : 0;
   qgl->qobj->setSpacing(spacing);
   return 0;
}

//void setVerticalSpacing ( int spacing )
static AbstractQoreNode *QGRIDLAYOUT_setVerticalSpacing(QoreObject *self, QoreQGridLayout *qgl, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   int spacing = p ? p->getAsInt() : 0;
   qgl->qobj->setVerticalSpacing(spacing);
   return 0;
}

//int spacing () const
static AbstractQoreNode *QGRIDLAYOUT_spacing(QoreObject *self, QoreQGridLayout *qgl, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qgl->qobj->spacing());
}

//int verticalSpacing () const
static AbstractQoreNode *QGRIDLAYOUT_verticalSpacing(QoreObject *self, QoreQGridLayout *qgl, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qgl->qobj->verticalSpacing());
}


class QoreClass *initQGridLayoutClass(class QoreClass *qlayout)
{
   tracein("initQGridLayoutClass()");
   
   class QoreClass *QC_QGridLayout = new QoreClass("QGridLayout", QDOM_GUI);
   CID_QGRIDLAYOUT = QC_QGridLayout->getID();

   QC_QGridLayout->addBuiltinVirtualBaseClass(qlayout);

   QC_QGridLayout->setConstructor(QGRIDLAYOUT_constructor);
   QC_QGridLayout->setCopy((q_copy_t)QGRIDLAYOUT_copy);

   //QC_QGridLayout->addMethod("addItem",                     (q_method_t)QGRIDLAYOUT_addItem);
   QC_QGridLayout->addMethod("addLayout",                   (q_method_t)QGRIDLAYOUT_addLayout);
   QC_QGridLayout->addMethod("addWidget",                   (q_method_t)QGRIDLAYOUT_addWidget);
   //QC_QGridLayout->addMethod("cellRect",                    (q_method_t)QGRIDLAYOUT_cellRect);
   QC_QGridLayout->addMethod("columnCount",                 (q_method_t)QGRIDLAYOUT_columnCount);
   QC_QGridLayout->addMethod("columnMinimumWidth",          (q_method_t)QGRIDLAYOUT_columnMinimumWidth);
   QC_QGridLayout->addMethod("columnStretch",               (q_method_t)QGRIDLAYOUT_columnStretch);
   //QC_QGridLayout->addMethod("getItemPosition",             (q_method_t)QGRIDLAYOUT_getItemPosition);
   QC_QGridLayout->addMethod("horizontalSpacing",           (q_method_t)QGRIDLAYOUT_horizontalSpacing);
   //QC_QGridLayout->addMethod("originCorner",                (q_method_t)QGRIDLAYOUT_originCorner);
   QC_QGridLayout->addMethod("rowCount",                    (q_method_t)QGRIDLAYOUT_rowCount);
   QC_QGridLayout->addMethod("rowMinimumHeight",            (q_method_t)QGRIDLAYOUT_rowMinimumHeight);
   QC_QGridLayout->addMethod("rowStretch",                  (q_method_t)QGRIDLAYOUT_rowStretch);
   QC_QGridLayout->addMethod("setColumnMinimumWidth",       (q_method_t)QGRIDLAYOUT_setColumnMinimumWidth);
   QC_QGridLayout->addMethod("setColumnStretch",            (q_method_t)QGRIDLAYOUT_setColumnStretch);
   QC_QGridLayout->addMethod("setHorizontalSpacing",        (q_method_t)QGRIDLAYOUT_setHorizontalSpacing);
   QC_QGridLayout->addMethod("setOriginCorner",             (q_method_t)QGRIDLAYOUT_setOriginCorner);
   QC_QGridLayout->addMethod("setRowMinimumHeight",         (q_method_t)QGRIDLAYOUT_setRowMinimumHeight);
   QC_QGridLayout->addMethod("setRowStretch",               (q_method_t)QGRIDLAYOUT_setRowStretch);
   QC_QGridLayout->addMethod("setSpacing",                  (q_method_t)QGRIDLAYOUT_setSpacing);
   QC_QGridLayout->addMethod("setVerticalSpacing",          (q_method_t)QGRIDLAYOUT_setVerticalSpacing);
   QC_QGridLayout->addMethod("spacing",                     (q_method_t)QGRIDLAYOUT_spacing);
   QC_QGridLayout->addMethod("verticalSpacing",             (q_method_t)QGRIDLAYOUT_verticalSpacing);

   traceout("initQGridLayoutClass()");
   return QC_QGridLayout;
}
