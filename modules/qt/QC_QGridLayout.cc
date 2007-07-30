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

static void QGRIDLAYOUT_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreQGridLayout *qw;
   QoreNode *p = test_param(params, NT_OBJECT, 0);
   QoreAbstractQWidget *parent = p ? (QoreAbstractQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;

   if (!parent)
      qw = new QoreQGridLayout(self);
   else 
   {
      ReferenceHolder<QoreAbstractQWidget> holder(parent, xsink);
      qw = new QoreQGridLayout(self, parent->getQWidget());
   }

   self->setPrivate(CID_QGRIDLAYOUT, qw);
}

static void QGRIDLAYOUT_copy(class Object *self, class Object *old, class QoreQGridLayout *ql, ExceptionSink *xsink)
{
   xsink->raiseException("QGRIDLAYOUT-COPY-ERROR", "objects of this class cannot be copied");
}

//void addItem ( QLayoutItem * item, int row, int column, int rowSpan = 1, int columnSpan = 1, Qt::Alignment alignment = 0 )
//static QoreNode *QGRIDLAYOUT_addItem(Object *self, QoreQGridLayout *qgl, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
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
QoreNode *QGRIDLAYOUT_addLayout(class Object *self, QoreQGridLayout *ql, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_OBJECT, 0);
   QoreAbstractQLayout *layout = p ? (QoreAbstractQLayout *)p->val.object->getReferencedPrivateData(CID_QLAYOUT, xsink) : 0;
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
   QoreNode *p1 = get_param(params, 4);
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
QoreNode *QGRIDLAYOUT_addWidget(class Object *self, QoreQGridLayout *ql, class QoreNode *params, ExceptionSink *xsink)
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

//QRect cellRect ( int row, int column ) const
//static QoreNode *QGRIDLAYOUT_cellRect(Object *self, QoreQGridLayout *qgl, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   int row = p ? p->getAsInt() : 0;
//   p = get_param(params, 1);
//   int column = p ? p->getAsInt() : 0;
//   ??? return new QoreNode((int64)qgl->qobj->cellRect(row, column));
//}

//int columnCount () const
static QoreNode *QGRIDLAYOUT_columnCount(Object *self, QoreQGridLayout *qgl, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qgl->qobj->columnCount());
}

//int columnMinimumWidth ( int column ) const
static QoreNode *QGRIDLAYOUT_columnMinimumWidth(Object *self, QoreQGridLayout *qgl, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int column = p ? p->getAsInt() : 0;
   return new QoreNode((int64)qgl->qobj->columnMinimumWidth(column));
}

//int columnStretch ( int column ) const
static QoreNode *QGRIDLAYOUT_columnStretch(Object *self, QoreQGridLayout *qgl, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int column = p ? p->getAsInt() : 0;
   return new QoreNode((int64)qgl->qobj->columnStretch(column));
}

//void getItemPosition ( int index, int * row, int * column, int * rowSpan, int * columnSpan )
//static QoreNode *QGRIDLAYOUT_getItemPosition(Object *self, QoreQGridLayout *qgl, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
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
static QoreNode *QGRIDLAYOUT_horizontalSpacing(Object *self, QoreQGridLayout *qgl, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qgl->qobj->horizontalSpacing());
}

//Qt::Corner originCorner () const
//static QoreNode *QGRIDLAYOUT_originCorner(Object *self, QoreQGridLayout *qgl, QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreNode((int64)qgl->qobj->originCorner());
//}

//int rowCount () const
static QoreNode *QGRIDLAYOUT_rowCount(Object *self, QoreQGridLayout *qgl, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qgl->qobj->rowCount());
}

//int rowMinimumHeight ( int row ) const
static QoreNode *QGRIDLAYOUT_rowMinimumHeight(Object *self, QoreQGridLayout *qgl, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int row = p ? p->getAsInt() : 0;
   return new QoreNode((int64)qgl->qobj->rowMinimumHeight(row));
}

//int rowStretch ( int row ) const
static QoreNode *QGRIDLAYOUT_rowStretch(Object *self, QoreQGridLayout *qgl, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int row = p ? p->getAsInt() : 0;
   return new QoreNode((int64)qgl->qobj->rowStretch(row));
}

//void setColumnMinimumWidth ( int column, int minSize )
static QoreNode *QGRIDLAYOUT_setColumnMinimumWidth(Object *self, QoreQGridLayout *qgl, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int column = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int minSize = p ? p->getAsInt() : 0;
   qgl->qobj->setColumnMinimumWidth(column, minSize);
   return 0;
}

//void setColumnStretch ( int column, int stretch )
static QoreNode *QGRIDLAYOUT_setColumnStretch(Object *self, QoreQGridLayout *qgl, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int column = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int stretch = p ? p->getAsInt() : 0;
   qgl->qobj->setColumnStretch(column, stretch);
   return 0;
}

//void setHorizontalSpacing ( int spacing )
static QoreNode *QGRIDLAYOUT_setHorizontalSpacing(Object *self, QoreQGridLayout *qgl, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int spacing = p ? p->getAsInt() : 0;
   qgl->qobj->setHorizontalSpacing(spacing);
   return 0;
}

//void setOriginCorner ( Qt::Corner corner )
static QoreNode *QGRIDLAYOUT_setOriginCorner(Object *self, QoreQGridLayout *qgl, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   Qt::Corner corner = (Qt::Corner)(p ? p->getAsInt() : 0);
   qgl->qobj->setOriginCorner(corner);
   return 0;
}

//void setRowMinimumHeight ( int row, int minSize )
static QoreNode *QGRIDLAYOUT_setRowMinimumHeight(Object *self, QoreQGridLayout *qgl, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int row = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int minSize = p ? p->getAsInt() : 0;
   qgl->qobj->setRowMinimumHeight(row, minSize);
   return 0;
}

//void setRowStretch ( int row, int stretch )
static QoreNode *QGRIDLAYOUT_setRowStretch(Object *self, QoreQGridLayout *qgl, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int row = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int stretch = p ? p->getAsInt() : 0;
   qgl->qobj->setRowStretch(row, stretch);
   return 0;
}

//void setSpacing ( int spacing )
static QoreNode *QGRIDLAYOUT_setSpacing(Object *self, QoreQGridLayout *qgl, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int spacing = p ? p->getAsInt() : 0;
   qgl->qobj->setSpacing(spacing);
   return 0;
}

//void setVerticalSpacing ( int spacing )
static QoreNode *QGRIDLAYOUT_setVerticalSpacing(Object *self, QoreQGridLayout *qgl, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int spacing = p ? p->getAsInt() : 0;
   qgl->qobj->setVerticalSpacing(spacing);
   return 0;
}

//int spacing () const
static QoreNode *QGRIDLAYOUT_spacing(Object *self, QoreQGridLayout *qgl, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qgl->qobj->spacing());
}

//int verticalSpacing () const
static QoreNode *QGRIDLAYOUT_verticalSpacing(Object *self, QoreQGridLayout *qgl, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qgl->qobj->verticalSpacing());
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
