/*
 QC_QTableView.cc
 
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

#include "QC_QTableView.h"
#include "QC_QHeaderView.h"
#include "QC_QWidget.h"
#include "QC_QPoint.h"
#include "QC_QModelIndex.h"

#include "qore-qt.h"

qore_classid_t CID_QTABLEVIEW;
class QoreClass *QC_QTableView = 0;

//QTableView ( QWidget * parent = 0 )
static void QTABLEVIEW_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreQWidget *parent = p ? (QoreQWidget *)p->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return;
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   self->setPrivate(CID_QTABLEVIEW, new QoreQTableView(self, parent ? static_cast<QWidget *>(parent->getQWidget()) : 0));
   return;
}

static void QTABLEVIEW_copy(class QoreObject *self, class QoreObject *old, class QoreQTableView *qtv, ExceptionSink *xsink)
{
   xsink->raiseException("QTABLEVIEW-COPY-ERROR", "objects of this class cannot be copied");
}

//int columnAt ( int x ) const
static AbstractQoreNode *QTABLEVIEW_columnAt(QoreObject *self, QoreAbstractQTableView *qtv, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int x = p ? p->getAsInt() : 0;
   return new QoreBigIntNode(qtv->getQTableView()->columnAt(x));
}

//int columnSpan ( int row, int column ) const
static AbstractQoreNode *QTABLEVIEW_columnSpan(QoreObject *self, QoreAbstractQTableView *qtv, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int row = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int column = p ? p->getAsInt() : 0;
   return new QoreBigIntNode(qtv->getQTableView()->columnSpan(row, column));
}

//int columnViewportPosition ( int column ) const
static AbstractQoreNode *QTABLEVIEW_columnViewportPosition(QoreObject *self, QoreAbstractQTableView *qtv, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int column = p ? p->getAsInt() : 0;
   return new QoreBigIntNode(qtv->getQTableView()->columnViewportPosition(column));
}

//int columnWidth ( int column ) const
static AbstractQoreNode *QTABLEVIEW_columnWidth(QoreObject *self, QoreAbstractQTableView *qtv, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int column = p ? p->getAsInt() : 0;
   return new QoreBigIntNode(qtv->getQTableView()->columnWidth(column));
}

//Qt::PenStyle gridStyle () const
static AbstractQoreNode *QTABLEVIEW_gridStyle(QoreObject *self, QoreAbstractQTableView *qtv, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qtv->getQTableView()->gridStyle());
}

//QHeaderView * horizontalHeader () const
static AbstractQoreNode *QTABLEVIEW_horizontalHeader(QoreObject *self, QoreAbstractQTableView *qtv, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qhv = new QoreObject(QC_QHeaderView, getProgram());
   QHeaderView *hv = qtv->getQTableView()->horizontalHeader();
   QoreQtQHeaderView *q_qhv = new QoreQtQHeaderView(o_qhv, hv);
   o_qhv->setPrivate(CID_QHEADERVIEW, q_qhv);
   //printd(5, "QTABLEVIEW_horizontalHeader() obj=%08p, QoreQtQHeaderView=%08p, QHeaderView=%08p\n", o_qhv, q_qhv, hv);
   return o_qhv;
}

//virtual QModelIndex indexAt ( const QPoint & pos ) const
static AbstractQoreNode *QTABLEVIEW_indexAt(QoreObject *self, QoreAbstractQTableView *qtv, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreQPoint *pos = p ? (QoreQPoint *)p->getReferencedPrivateData(CID_QPOINT, xsink) : 0;
   if (!pos) {
      if (!xsink->isException())
         xsink->raiseException("QTABLEVIEW-INDEXAT-PARAM-ERROR", "expecting a QPoint object as first argument to QTableView::indexAt()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> posHolder(static_cast<AbstractPrivateData *>(pos), xsink);
   QoreObject *o_qmi = new QoreObject(QC_QModelIndex, getProgram());
   QoreQModelIndex *q_qmi = new QoreQModelIndex(qtv->getQTableView()->indexAt(*(static_cast<QPoint *>(pos))));
   o_qmi->setPrivate(CID_QMODELINDEX, q_qmi);
   return o_qmi;
}

//bool isColumnHidden ( int column ) const
static AbstractQoreNode *QTABLEVIEW_isColumnHidden(QoreObject *self, QoreAbstractQTableView *qtv, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int column = p ? p->getAsInt() : 0;
   return get_bool_node(qtv->getQTableView()->isColumnHidden(column));
}

//bool isCornerButtonEnabled () const
static AbstractQoreNode *QTABLEVIEW_isCornerButtonEnabled(QoreObject *self, QoreAbstractQTableView *qtv, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qtv->getQTableView()->isCornerButtonEnabled());
}

//bool isRowHidden ( int row ) const
static AbstractQoreNode *QTABLEVIEW_isRowHidden(QoreObject *self, QoreAbstractQTableView *qtv, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int row = p ? p->getAsInt() : 0;
   return get_bool_node(qtv->getQTableView()->isRowHidden(row));
}

//bool isSortingEnabled () const
static AbstractQoreNode *QTABLEVIEW_isSortingEnabled(QoreObject *self, QoreAbstractQTableView *qtv, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qtv->getQTableView()->isSortingEnabled());
}

//int rowAt ( int y ) const
static AbstractQoreNode *QTABLEVIEW_rowAt(QoreObject *self, QoreAbstractQTableView *qtv, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int y = p ? p->getAsInt() : 0;
   return new QoreBigIntNode(qtv->getQTableView()->rowAt(y));
}

//int rowHeight ( int row ) const
static AbstractQoreNode *QTABLEVIEW_rowHeight(QoreObject *self, QoreAbstractQTableView *qtv, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int row = p ? p->getAsInt() : 0;
   return new QoreBigIntNode(qtv->getQTableView()->rowHeight(row));
}

//int rowSpan ( int row, int column ) const
static AbstractQoreNode *QTABLEVIEW_rowSpan(QoreObject *self, QoreAbstractQTableView *qtv, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int row = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int column = p ? p->getAsInt() : 0;
   return new QoreBigIntNode(qtv->getQTableView()->rowSpan(row, column));
}

//int rowViewportPosition ( int row ) const
static AbstractQoreNode *QTABLEVIEW_rowViewportPosition(QoreObject *self, QoreAbstractQTableView *qtv, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int row = p ? p->getAsInt() : 0;
   return new QoreBigIntNode(qtv->getQTableView()->rowViewportPosition(row));
}

//void setColumnHidden ( int column, bool hide )
static AbstractQoreNode *QTABLEVIEW_setColumnHidden(QoreObject *self, QoreAbstractQTableView *qtv, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int column = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   bool hide = p ? p->getAsBool() : false;
   qtv->getQTableView()->setColumnHidden(column, hide);
   return 0;
}

//void setColumnWidth ( int column, int width )
static AbstractQoreNode *QTABLEVIEW_setColumnWidth(QoreObject *self, QoreAbstractQTableView *qtv, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int column = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int width = p ? p->getAsInt() : 0;
   qtv->getQTableView()->setColumnWidth(column, width);
   return 0;
}

//void setCornerButtonEnabled ( bool enable )
static AbstractQoreNode *QTABLEVIEW_setCornerButtonEnabled(QoreObject *self, QoreAbstractQTableView *qtv, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   bool enable = p ? p->getAsBool() : false;
   qtv->getQTableView()->setCornerButtonEnabled(enable);
   return 0;
}

//void setGridStyle ( Qt::PenStyle style )
static AbstractQoreNode *QTABLEVIEW_setGridStyle(QoreObject *self, QoreAbstractQTableView *qtv, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   Qt::PenStyle style = (Qt::PenStyle)(p ? p->getAsInt() : 0);
   qtv->getQTableView()->setGridStyle(style);
   return 0;
}

//void setHorizontalHeader ( QHeaderView * header )
static AbstractQoreNode *QTABLEVIEW_setHorizontalHeader(QoreObject *self, QoreAbstractQTableView *qtv, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreAbstractQHeaderView *header = p ? (QoreAbstractQHeaderView *)p->getReferencedPrivateData(CID_QHEADERVIEW, xsink) : 0;
   if (!header) {
      if (!xsink->isException())
         xsink->raiseException("QTABLEVIEW-SETHORIZONTALHEADER-PARAM-ERROR", "expecting a QHeaderView object as first argument to QTableView::setHorizontalHeader()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> headerHolder(static_cast<AbstractPrivateData *>(header), xsink);
   qtv->getQTableView()->setHorizontalHeader(header->getQHeaderView());
   return 0;
}

//void setRowHeight ( int row, int height )
static AbstractQoreNode *QTABLEVIEW_setRowHeight(QoreObject *self, QoreAbstractQTableView *qtv, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int row = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int height = p ? p->getAsInt() : 0;
   qtv->getQTableView()->setRowHeight(row, height);
   return 0;
}

//void setRowHidden ( int row, bool hide )
static AbstractQoreNode *QTABLEVIEW_setRowHidden(QoreObject *self, QoreAbstractQTableView *qtv, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int row = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   bool hide = p ? p->getAsBool() : false;
   qtv->getQTableView()->setRowHidden(row, hide);
   return 0;
}

//void setSortingEnabled ( bool enable )
static AbstractQoreNode *QTABLEVIEW_setSortingEnabled(QoreObject *self, QoreAbstractQTableView *qtv, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   bool enable = p ? p->getAsBool() : false;
   qtv->getQTableView()->setSortingEnabled(enable);
   return 0;
}

//void setSpan ( int row, int column, int rowSpan, int columnSpan )
static AbstractQoreNode *QTABLEVIEW_setSpan(QoreObject *self, QoreAbstractQTableView *qtv, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int row = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int column = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   int rowSpan = p ? p->getAsInt() : 0;
   p = get_param(params, 3);
   int columnSpan = p ? p->getAsInt() : 0;
   qtv->getQTableView()->setSpan(row, column, rowSpan, columnSpan);
   return 0;
}

//void setVerticalHeader ( QHeaderView * header )
static AbstractQoreNode *QTABLEVIEW_setVerticalHeader(QoreObject *self, QoreAbstractQTableView *qtv, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreAbstractQHeaderView *header = p ? (QoreAbstractQHeaderView *)p->getReferencedPrivateData(CID_QHEADERVIEW, xsink) : 0;
   if (!header) {
      if (!xsink->isException())
         xsink->raiseException("QTABLEVIEW-SETVERTICALHEADER-PARAM-ERROR", "expecting a QHeaderView object as first argument to QTableView::setVerticalHeader()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> headerHolder(static_cast<AbstractPrivateData *>(header), xsink);
   qtv->getQTableView()->setVerticalHeader(header->getQHeaderView());
   return 0;
}

//void setWordWrap ( bool on )
static AbstractQoreNode *QTABLEVIEW_setWordWrap(QoreObject *self, QoreAbstractQTableView *qtv, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   bool on = p ? p->getAsBool() : false;
   qtv->getQTableView()->setWordWrap(on);
   return 0;
}

//bool showGrid () const
static AbstractQoreNode *QTABLEVIEW_showGrid(QoreObject *self, QoreAbstractQTableView *qtv, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qtv->getQTableView()->showGrid());
}

//void sortByColumn ( int column, Qt::SortOrder order )
static AbstractQoreNode *QTABLEVIEW_sortByColumn(QoreObject *self, QoreAbstractQTableView *qtv, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int column = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   Qt::SortOrder order = (Qt::SortOrder)(p ? p->getAsInt() : 0);
   qtv->getQTableView()->sortByColumn(column, order);
   return 0;
}

//QHeaderView * verticalHeader () const
static AbstractQoreNode *QTABLEVIEW_verticalHeader(QoreObject *self, QoreAbstractQTableView *qtv, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qhv = new QoreObject(QC_QHeaderView, getProgram());
   QoreQtQHeaderView *q_qhv = new QoreQtQHeaderView(o_qhv, qtv->getQTableView()->verticalHeader());
   o_qhv->setPrivate(CID_QHEADERVIEW, q_qhv);
   return o_qhv;
}

//bool wordWrap () const
static AbstractQoreNode *QTABLEVIEW_wordWrap(QoreObject *self, QoreAbstractQTableView *qtv, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qtv->getQTableView()->wordWrap());
}

//void hideColumn ( int column )
static AbstractQoreNode *QTABLEVIEW_hideColumn(QoreObject *self, QoreAbstractQTableView *qtv, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int column = p ? p->getAsInt() : 0;
   qtv->getQTableView()->hideColumn(column);
   return 0;
}

//void hideRow ( int row )
static AbstractQoreNode *QTABLEVIEW_hideRow(QoreObject *self, QoreAbstractQTableView *qtv, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int row = p ? p->getAsInt() : 0;
   qtv->getQTableView()->hideRow(row);
   return 0;
}

//void resizeColumnToContents ( int column )
static AbstractQoreNode *QTABLEVIEW_resizeColumnToContents(QoreObject *self, QoreAbstractQTableView *qtv, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int column = p ? p->getAsInt() : 0;
   qtv->getQTableView()->resizeColumnToContents(column);
   return 0;
}

//void resizeColumnsToContents ()
static AbstractQoreNode *QTABLEVIEW_resizeColumnsToContents(QoreObject *self, QoreAbstractQTableView *qtv, const QoreListNode *params, ExceptionSink *xsink)
{
   qtv->getQTableView()->resizeColumnsToContents();
   return 0;
}

//void resizeRowToContents ( int row )
static AbstractQoreNode *QTABLEVIEW_resizeRowToContents(QoreObject *self, QoreAbstractQTableView *qtv, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int row = p ? p->getAsInt() : 0;
   qtv->getQTableView()->resizeRowToContents(row);
   return 0;
}

//void resizeRowsToContents ()
static AbstractQoreNode *QTABLEVIEW_resizeRowsToContents(QoreObject *self, QoreAbstractQTableView *qtv, const QoreListNode *params, ExceptionSink *xsink)
{
   qtv->getQTableView()->resizeRowsToContents();
   return 0;
}

//void selectColumn ( int column )
static AbstractQoreNode *QTABLEVIEW_selectColumn(QoreObject *self, QoreAbstractQTableView *qtv, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int column = p ? p->getAsInt() : 0;
   qtv->getQTableView()->selectColumn(column);
   return 0;
}

//void selectRow ( int row )
static AbstractQoreNode *QTABLEVIEW_selectRow(QoreObject *self, QoreAbstractQTableView *qtv, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int row = p ? p->getAsInt() : 0;
   qtv->getQTableView()->selectRow(row);
   return 0;
}

//void setShowGrid ( bool show )
static AbstractQoreNode *QTABLEVIEW_setShowGrid(QoreObject *self, QoreAbstractQTableView *qtv, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   bool show = p ? p->getAsBool() : false;
   qtv->getQTableView()->setShowGrid(show);
   return 0;
}

//void showColumn ( int column )
static AbstractQoreNode *QTABLEVIEW_showColumn(QoreObject *self, QoreAbstractQTableView *qtv, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int column = p ? p->getAsInt() : 0;
   qtv->getQTableView()->showColumn(column);
   return 0;
}

//void showRow ( int row )
static AbstractQoreNode *QTABLEVIEW_showRow(QoreObject *self, QoreAbstractQTableView *qtv, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int row = p ? p->getAsInt() : 0;
   qtv->getQTableView()->showRow(row);
   return 0;
}

QoreClass *initQTableViewClass(QoreClass *qabstractitemview)
{
   QC_QTableView = new QoreClass("QTableView", QDOM_GUI);
   CID_QTABLEVIEW = QC_QTableView->getID();

   QC_QTableView->addBuiltinVirtualBaseClass(qabstractitemview);

   QC_QTableView->setConstructor(QTABLEVIEW_constructor);
   QC_QTableView->setCopy((q_copy_t)QTABLEVIEW_copy);

   QC_QTableView->addMethod("columnAt",                    (q_method_t)QTABLEVIEW_columnAt);
   QC_QTableView->addMethod("columnSpan",                  (q_method_t)QTABLEVIEW_columnSpan);
   QC_QTableView->addMethod("columnViewportPosition",      (q_method_t)QTABLEVIEW_columnViewportPosition);
   QC_QTableView->addMethod("columnWidth",                 (q_method_t)QTABLEVIEW_columnWidth);
   QC_QTableView->addMethod("gridStyle",                   (q_method_t)QTABLEVIEW_gridStyle);
   QC_QTableView->addMethod("horizontalHeader",            (q_method_t)QTABLEVIEW_horizontalHeader);
   QC_QTableView->addMethod("indexAt",                     (q_method_t)QTABLEVIEW_indexAt);
   QC_QTableView->addMethod("isColumnHidden",              (q_method_t)QTABLEVIEW_isColumnHidden);
   QC_QTableView->addMethod("isCornerButtonEnabled",       (q_method_t)QTABLEVIEW_isCornerButtonEnabled);
   QC_QTableView->addMethod("isRowHidden",                 (q_method_t)QTABLEVIEW_isRowHidden);
   QC_QTableView->addMethod("isSortingEnabled",            (q_method_t)QTABLEVIEW_isSortingEnabled);
   QC_QTableView->addMethod("rowAt",                       (q_method_t)QTABLEVIEW_rowAt);
   QC_QTableView->addMethod("rowHeight",                   (q_method_t)QTABLEVIEW_rowHeight);
   QC_QTableView->addMethod("rowSpan",                     (q_method_t)QTABLEVIEW_rowSpan);
   QC_QTableView->addMethod("rowViewportPosition",         (q_method_t)QTABLEVIEW_rowViewportPosition);
   QC_QTableView->addMethod("setColumnHidden",             (q_method_t)QTABLEVIEW_setColumnHidden);
   QC_QTableView->addMethod("setColumnWidth",              (q_method_t)QTABLEVIEW_setColumnWidth);
   QC_QTableView->addMethod("setCornerButtonEnabled",      (q_method_t)QTABLEVIEW_setCornerButtonEnabled);
   QC_QTableView->addMethod("setGridStyle",                (q_method_t)QTABLEVIEW_setGridStyle);
   QC_QTableView->addMethod("setHorizontalHeader",         (q_method_t)QTABLEVIEW_setHorizontalHeader);
   QC_QTableView->addMethod("setRowHeight",                (q_method_t)QTABLEVIEW_setRowHeight);
   QC_QTableView->addMethod("setRowHidden",                (q_method_t)QTABLEVIEW_setRowHidden);
   QC_QTableView->addMethod("setSortingEnabled",           (q_method_t)QTABLEVIEW_setSortingEnabled);
   QC_QTableView->addMethod("setSpan",                     (q_method_t)QTABLEVIEW_setSpan);
   QC_QTableView->addMethod("setVerticalHeader",           (q_method_t)QTABLEVIEW_setVerticalHeader);
   QC_QTableView->addMethod("setWordWrap",                 (q_method_t)QTABLEVIEW_setWordWrap);
   QC_QTableView->addMethod("showGrid",                    (q_method_t)QTABLEVIEW_showGrid);
   QC_QTableView->addMethod("sortByColumn",                (q_method_t)QTABLEVIEW_sortByColumn);
   QC_QTableView->addMethod("verticalHeader",              (q_method_t)QTABLEVIEW_verticalHeader);
   QC_QTableView->addMethod("wordWrap",                    (q_method_t)QTABLEVIEW_wordWrap);
   QC_QTableView->addMethod("hideColumn",                  (q_method_t)QTABLEVIEW_hideColumn);
   QC_QTableView->addMethod("hideRow",                     (q_method_t)QTABLEVIEW_hideRow);
   QC_QTableView->addMethod("resizeColumnToContents",      (q_method_t)QTABLEVIEW_resizeColumnToContents);
   QC_QTableView->addMethod("resizeColumnsToContents",     (q_method_t)QTABLEVIEW_resizeColumnsToContents);
   QC_QTableView->addMethod("resizeRowToContents",         (q_method_t)QTABLEVIEW_resizeRowToContents);
   QC_QTableView->addMethod("resizeRowsToContents",        (q_method_t)QTABLEVIEW_resizeRowsToContents);
   QC_QTableView->addMethod("selectColumn",                (q_method_t)QTABLEVIEW_selectColumn);
   QC_QTableView->addMethod("selectRow",                   (q_method_t)QTABLEVIEW_selectRow);
   QC_QTableView->addMethod("setShowGrid",                 (q_method_t)QTABLEVIEW_setShowGrid);
   QC_QTableView->addMethod("showColumn",                  (q_method_t)QTABLEVIEW_showColumn);
   QC_QTableView->addMethod("showRow",                     (q_method_t)QTABLEVIEW_showRow);

   return QC_QTableView;
}
