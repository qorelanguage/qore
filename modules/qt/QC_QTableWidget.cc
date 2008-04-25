/*
 QC_QTableWidget.cc
 
 Qore Programming Language
 
 Copyright 2003 - 2008 David Nichols
 
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

#include "QC_QTableWidget.h"
#include "QC_QTableWidgetItem.h"
#include "QC_QWidget.h"
#include "QC_QRect.h"
#include "QC_QPoint.h"

#include "qore-qt.h"

qore_classid_t CID_QTABLEWIDGET;
class QoreClass *QC_QTableWidget = 0;

//QTableWidget ( QWidget * parent = 0 )
//QTableWidget ( int rows, int columns, QWidget * parent = 0 )
static void QTABLEWIDGET_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (p && p->getType() == NT_OBJECT) {
      QoreQWidget *parent = (QoreQWidget *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink);
      if (*xsink)
         return;
      if (!parent) {
         xsink->raiseException("QTABLEWIDGET-CONSTRUCTOR-ERROR", "QTableWidget::constructor() cannot handle arguments of class '%s'", (reinterpret_cast<const QoreObject *>(p))->getClassName());
         return;
      }
      ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
      self->setPrivate(CID_QTABLEWIDGET, new QoreQTableWidget(self, parent->getQWidget()));
      return;
   }
   int rows = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int columns = p ? p->getAsInt() : 0;

   const QoreObject *o = test_object_param(params, 2);
   QoreQWidget *parent = o ? (QoreQWidget *)o->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return;
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   self->setPrivate(CID_QTABLEWIDGET, new QoreQTableWidget(self, rows, columns, parent ? static_cast<QWidget *>(parent->getQWidget()) : 0));
   return;
}

static void QTABLEWIDGET_copy(class QoreObject *self, class QoreObject *old, class QoreQTableWidget *qtw, ExceptionSink *xsink)
{
   xsink->raiseException("QTABLEWIDGET-COPY-ERROR", "objects of this class cannot be copied");
}

//QWidget * cellWidget ( int row, int column ) const
static AbstractQoreNode *QTABLEWIDGET_cellWidget(QoreObject *self, QoreQTableWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int row = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int column = p ? p->getAsInt() : 0;
   QWidget *qt_qobj = qtw->qobj->cellWidget(row, column);
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   QoreObject *rv_obj = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   assert(rv_obj);
   rv_obj->ref();
   return rv_obj;
}

//void closePersistentEditor ( QTableWidgetItem * item )
static AbstractQoreNode *QTABLEWIDGET_closePersistentEditor(QoreObject *self, QoreQTableWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreQTableWidgetItem *item = p ? (QoreQTableWidgetItem *)p->getReferencedPrivateData(CID_QTABLEWIDGETITEM, xsink) : 0;
   if (!item) {
      if (!xsink->isException())
         xsink->raiseException("QTABLEWIDGET-CLOSEPERSISTENTEDITOR-PARAM-ERROR", "expecting a QTableWidgetItem object as first argument to QTableWidget::closePersistentEditor()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> itemHolder(static_cast<AbstractPrivateData *>(item), xsink);
   qtw->qobj->closePersistentEditor(item->qore_obj);
   return 0;
}

//int column ( const QTableWidgetItem * item ) const
static AbstractQoreNode *QTABLEWIDGET_column(QoreObject *self, QoreQTableWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreQTableWidgetItem *item = p ? (QoreQTableWidgetItem *)p->getReferencedPrivateData(CID_QTABLEWIDGETITEM, xsink) : 0;
   if (!item) {
      if (!xsink->isException())
         xsink->raiseException("QTABLEWIDGET-COLUMN-PARAM-ERROR", "expecting a QTableWidgetItem object as first argument to QTableWidget::column()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> itemHolder(static_cast<AbstractPrivateData *>(item), xsink);
   return new QoreBigIntNode(qtw->qobj->column(item->qore_obj));
}

//int columnCount () const
static AbstractQoreNode *QTABLEWIDGET_columnCount(QoreObject *self, QoreQTableWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qtw->qobj->columnCount());
}

//int currentColumn () const
static AbstractQoreNode *QTABLEWIDGET_currentColumn(QoreObject *self, QoreQTableWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qtw->qobj->currentColumn());
}

//QTableWidgetItem * currentItem () const
static AbstractQoreNode *QTABLEWIDGET_currentItem(QoreObject *self, QoreQTableWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qtwi = new QoreObject(QC_QTableWidgetItem, getProgram());
   QoreQTableWidgetItem *q_qtwi = new QoreQTableWidgetItem(qtw->qobj->currentItem());
   o_qtwi->setPrivate(CID_QTABLEWIDGETITEM, q_qtwi);
   return o_qtwi;
}

//int currentRow () const
static AbstractQoreNode *QTABLEWIDGET_currentRow(QoreObject *self, QoreQTableWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qtw->qobj->currentRow());
}

//void editItem ( QTableWidgetItem * item )
static AbstractQoreNode *QTABLEWIDGET_editItem(QoreObject *self, QoreQTableWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreQTableWidgetItem *item = p ? (QoreQTableWidgetItem *)p->getReferencedPrivateData(CID_QTABLEWIDGETITEM, xsink) : 0;
   if (!item) {
      if (!xsink->isException())
         xsink->raiseException("QTABLEWIDGET-EDITITEM-PARAM-ERROR", "expecting a QTableWidgetItem object as first argument to QTableWidget::editItem()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> itemHolder(static_cast<AbstractPrivateData *>(item), xsink);
   qtw->qobj->editItem(item->qore_obj);
   return 0;
}

////QList<QTableWidgetItem *> findItems ( const QString & text, Qt::MatchFlags flags ) const
//static AbstractQoreNode *QTABLEWIDGET_findItems(QoreObject *self, QoreQTableWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
//{
//   const AbstractQoreNode *p = get_param(params, 0);
//   QString text;
//   if (get_qstring(p, text, xsink))
//      return 0;
//   p = get_param(params, 1);
//   Qt::MatchFlags flags = (Qt::MatchFlags)(p ? p->getAsInt() : 0);
//   ??? return new QoreBigIntNode(qtw->qobj->findItems(text, flags));
//}

//QTableWidgetItem * horizontalHeaderItem ( int column ) const
static AbstractQoreNode *QTABLEWIDGET_horizontalHeaderItem(QoreObject *self, QoreQTableWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int column = p ? p->getAsInt() : 0;

   QTableWidgetItem *twi = qtw->qobj->horizontalHeaderItem(column);
   if (!twi)
      return 0;

   QoreObject *o_qtwi = new QoreObject(QC_QTableWidgetItem, getProgram());
   QoreQTableWidgetItem *q_qtwi = new QoreQTableWidgetItem(twi);
   o_qtwi->setPrivate(CID_QTABLEWIDGETITEM, q_qtwi);
   return o_qtwi;
}

//QTableWidgetItem * item ( int row, int column ) const
static AbstractQoreNode *QTABLEWIDGET_item(QoreObject *self, QoreQTableWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int row = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int column = p ? p->getAsInt() : 0;
   
   QTableWidgetItem *twi = qtw->qobj->item(row, column);
   if (!twi)
      return 0;

   QoreObject *o_qtwi = new QoreObject(QC_QTableWidgetItem, getProgram());
   QoreQTableWidgetItem *q_qtwi = new QoreQTableWidgetItem(twi);
   o_qtwi->setPrivate(CID_QTABLEWIDGETITEM, q_qtwi);
   return o_qtwi;
}

//QTableWidgetItem * itemAt ( const QPoint & point ) const
//QTableWidgetItem * itemAt ( int ax, int ay ) const
static AbstractQoreNode *QTABLEWIDGET_itemAt(QoreObject *self, QoreQTableWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (p && p->getType() == NT_OBJECT) {
      QoreQPoint *point = (QoreQPoint *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QPOINT, xsink);
      if (!point) {
         if (!xsink->isException())
            xsink->raiseException("QTABLEWIDGET-ITEMAT-PARAM-ERROR", "QTableWidget::itemAt() does not know how to handle arguments of class '%s' as passed as the first argument", (reinterpret_cast<const QoreObject *>(p))->getClassName());
         return 0;
      }
      ReferenceHolder<AbstractPrivateData> pointHolder(static_cast<AbstractPrivateData *>(point), xsink);

      QTableWidgetItem *twi = qtw->qobj->itemAt(*(static_cast<QPoint *>(point)));
      if (!twi)
	 return 0;

      QoreObject *o_qtwi = new QoreObject(QC_QTableWidgetItem, getProgram());
      QoreQTableWidgetItem *q_qtwi = new QoreQTableWidgetItem(twi);
      o_qtwi->setPrivate(CID_QTABLEWIDGETITEM, q_qtwi);
      return o_qtwi;
   }
   int ax = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int ay = p ? p->getAsInt() : 0;

   QTableWidgetItem *twi = qtw->qobj->itemAt(ax, ay);
   if (!twi)
      return 0;

   QoreObject *o_qtwi = new QoreObject(QC_QTableWidgetItem, getProgram());
   QoreQTableWidgetItem *q_qtwi = new QoreQTableWidgetItem(twi);
   o_qtwi->setPrivate(CID_QTABLEWIDGETITEM, q_qtwi);
   return o_qtwi;
}

//const QTableWidgetItem * itemPrototype () const
static AbstractQoreNode *QTABLEWIDGET_itemPrototype(QoreObject *self, QoreQTableWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   const QTableWidgetItem *twi = qtw->qobj->itemPrototype();
   if (!twi)
      return 0;

   QoreObject *o_qtwi = new QoreObject(QC_QTableWidgetItem, getProgram());
   QoreQTableWidgetItem *q_qtwi = new QoreQTableWidgetItem(const_cast<QTableWidgetItem *>(twi));
   o_qtwi->setPrivate(CID_QTABLEWIDGETITEM, q_qtwi);
   return o_qtwi;
}

//void openPersistentEditor ( QTableWidgetItem * item )
static AbstractQoreNode *QTABLEWIDGET_openPersistentEditor(QoreObject *self, QoreQTableWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreQTableWidgetItem *item = p ? (QoreQTableWidgetItem *)p->getReferencedPrivateData(CID_QTABLEWIDGETITEM, xsink) : 0;
   if (!item) {
      if (!xsink->isException())
         xsink->raiseException("QTABLEWIDGET-OPENPERSISTENTEDITOR-PARAM-ERROR", "expecting a QTableWidgetItem object as first argument to QTableWidget::openPersistentEditor()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> itemHolder(static_cast<AbstractPrivateData *>(item), xsink);
   qtw->qobj->openPersistentEditor(item->qore_obj);
   return 0;
}

//void removeCellWidget ( int row, int column )
static AbstractQoreNode *QTABLEWIDGET_removeCellWidget(QoreObject *self, QoreQTableWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int row = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int column = p ? p->getAsInt() : 0;
   qtw->qobj->removeCellWidget(row, column);
   return 0;
}

//int row ( const QTableWidgetItem * item ) const
static AbstractQoreNode *QTABLEWIDGET_row(QoreObject *self, QoreQTableWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreQTableWidgetItem *item = p ? (QoreQTableWidgetItem *)p->getReferencedPrivateData(CID_QTABLEWIDGETITEM, xsink) : 0;
   if (!item) {
      if (!xsink->isException())
         xsink->raiseException("QTABLEWIDGET-ROW-PARAM-ERROR", "expecting a QTableWidgetItem object as first argument to QTableWidget::row()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> itemHolder(static_cast<AbstractPrivateData *>(item), xsink);
   return new QoreBigIntNode(qtw->qobj->row(item->qore_obj));
}

//int rowCount () const
static AbstractQoreNode *QTABLEWIDGET_rowCount(QoreObject *self, QoreQTableWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qtw->qobj->rowCount());
}

////QList<QTableWidgetItem *> selectedItems ()
//static AbstractQoreNode *QTABLEWIDGET_selectedItems(QoreObject *self, QoreQTableWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreBigIntNode(qtw->qobj->selectedItems());
//}

////QList<QTableWidgetSelectionRange> selectedRanges () const
//static AbstractQoreNode *QTABLEWIDGET_selectedRanges(QoreObject *self, QoreQTableWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreBigIntNode(qtw->qobj->selectedRanges());
//}

//void setCellWidget ( int row, int column, QWidget * widget )
static AbstractQoreNode *QTABLEWIDGET_setCellWidget(QoreObject *self, QoreQTableWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int row = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int column = p ? p->getAsInt() : 0;

   const QoreObject *o = test_object_param(params, 2);
   QoreQWidget *widget = o ? (QoreQWidget *)o->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!widget) {
      if (!xsink->isException())
         xsink->raiseException("QTABLEWIDGET-SETCELLWIDGET-PARAM-ERROR", "expecting a QWidget object as third argument to QTableWidget::setCellWidget()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   qtw->qobj->setCellWidget(row, column, static_cast<QWidget *>(widget->getQWidget()));
   return 0;
}

//void setColumnCount ( int columns )
static AbstractQoreNode *QTABLEWIDGET_setColumnCount(QoreObject *self, QoreQTableWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int columns = p ? p->getAsInt() : 0;
   qtw->qobj->setColumnCount(columns);
   return 0;
}

//void setCurrentCell ( int row, int column )
static AbstractQoreNode *QTABLEWIDGET_setCurrentCell(QoreObject *self, QoreQTableWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int row = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int column = p ? p->getAsInt() : 0;
   qtw->qobj->setCurrentCell(row, column);
   return 0;
}

//void setCurrentItem ( QTableWidgetItem * item )
static AbstractQoreNode *QTABLEWIDGET_setCurrentItem(QoreObject *self, QoreQTableWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreQTableWidgetItem *item = p ? (QoreQTableWidgetItem *)p->getReferencedPrivateData(CID_QTABLEWIDGETITEM, xsink) : 0;
   if (!item) {
      if (!xsink->isException())
         xsink->raiseException("QTABLEWIDGET-SETCURRENTITEM-PARAM-ERROR", "expecting a QTableWidgetItem object as first argument to QTableWidget::setCurrentItem()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> itemHolder(static_cast<AbstractPrivateData *>(item), xsink);
   qtw->qobj->setCurrentItem(item->qore_obj);
   return 0;
}

//void setHorizontalHeaderItem ( int column, QTableWidgetItem * item )
static AbstractQoreNode *QTABLEWIDGET_setHorizontalHeaderItem(QoreObject *self, QoreQTableWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int column = p ? p->getAsInt() : 0;

   const QoreObject *o = test_object_param(params, 1);
   QoreQTableWidgetItem *item = o ? (QoreQTableWidgetItem *)o->getReferencedPrivateData(CID_QTABLEWIDGETITEM, xsink) : 0;
   if (!item) {
      if (!xsink->isException())
         xsink->raiseException("QTABLEWIDGET-SETHORIZONTALHEADERITEM-PARAM-ERROR", "expecting a QTableWidgetItem object as second argument to QTableWidget::setHorizontalHeaderItem()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> itemHolder(static_cast<AbstractPrivateData *>(item), xsink);
   qtw->qobj->setHorizontalHeaderItem(column, item->qore_obj);
   return 0;
}

//void setHorizontalHeaderLabels ( const QStringList & labels )
static AbstractQoreNode *QTABLEWIDGET_setHorizontalHeaderLabels(QoreObject *self, QoreQTableWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreListNode *p = test_list_param(params, 0);
   if (!p) {
      xsink->raiseException("QTABLEWIDGET-SETHORIZONTALHEADERLABELS-PARAM-ERROR", "expecting a list as first argument to QTableWidget::setHorizontalHeaderLabels()");
      return 0;
   }
   QStringList labels;
   ConstListIterator li_labels(p);
   while (li_labels.next())
   {
      QoreStringNodeValueHelper str(li_labels.getValue());
      QString tmp;
      if (get_qstring(*str, tmp, xsink))
         return 0;
      labels.push_back(tmp);
   }
   qtw->qobj->setHorizontalHeaderLabels(labels);
   return 0;
}

//void setItem ( int row, int column, QTableWidgetItem * item )
static AbstractQoreNode *QTABLEWIDGET_setItem(QoreObject *self, QoreQTableWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int row = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int column = p ? p->getAsInt() : 0;

   const QoreObject *o = test_object_param(params, 2);
   QoreQTableWidgetItem *item = o ? (QoreQTableWidgetItem *)o->getReferencedPrivateData(CID_QTABLEWIDGETITEM, xsink) : 0;
   if (!item) {
      if (!xsink->isException())
         xsink->raiseException("QTABLEWIDGET-SETITEM-PARAM-ERROR", "expecting a QTableWidgetItem object as third argument to QTableWidget::setItem()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> itemHolder(static_cast<AbstractPrivateData *>(item), xsink);
   qtw->qobj->setItem(row, column, item->qore_obj);
   //printd(5, "QTableWidget::setItem() %08p flags=%d\n", item, (int)item->qore_obj->flags());
   return 0;
}

//void setItemPrototype ( const QTableWidgetItem * item )
static AbstractQoreNode *QTABLEWIDGET_setItemPrototype(QoreObject *self, QoreQTableWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreObject *p = test_object_param(params, 0);
   QoreQTableWidgetItem *item = p ? (QoreQTableWidgetItem *)p->getReferencedPrivateData(CID_QTABLEWIDGETITEM, xsink) : 0;
   if (!item) {
      if (!xsink->isException())
         xsink->raiseException("QTABLEWIDGET-SETITEMPROTOTYPE-PARAM-ERROR", "expecting a QTableWidgetItem object as first argument to QTableWidget::setItemPrototype()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> itemHolder(static_cast<AbstractPrivateData *>(item), xsink);
   qtw->qobj->setItemPrototype(item->qore_obj);
   return 0;
}

////void setRangeSelected ( const QTableWidgetSelectionRange & range, bool select )
//static AbstractQoreNode *QTABLEWIDGET_setRangeSelected(QoreObject *self, QoreQTableWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
//{
//   const AbstractQoreNode *p = get_param(params, 0);
//   ??? QTableWidgetSelectionRange range = p;
//   p = get_param(params, 1);
//   bool select = p ? p->getAsBool() : false;
//   qtw->qobj->setRangeSelected(range, select);
//   return 0;
//}

//void setRowCount ( int rows )
static AbstractQoreNode *QTABLEWIDGET_setRowCount(QoreObject *self, QoreQTableWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int rows = p ? p->getAsInt() : 0;
   qtw->qobj->setRowCount(rows);
   return 0;
}

//void setVerticalHeaderItem ( int row, QTableWidgetItem * item )
static AbstractQoreNode *QTABLEWIDGET_setVerticalHeaderItem(QoreObject *self, QoreQTableWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int row = p ? p->getAsInt() : 0;

   const QoreObject *o = test_object_param(params, 1);
   QoreQTableWidgetItem *item = o ? (QoreQTableWidgetItem *)o->getReferencedPrivateData(CID_QTABLEWIDGETITEM, xsink) : 0;
   if (!item) {
      if (!xsink->isException())
         xsink->raiseException("QTABLEWIDGET-SETVERTICALHEADERITEM-PARAM-ERROR", "expecting a QTableWidgetItem object as second argument to QTableWidget::setVerticalHeaderItem()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> itemHolder(static_cast<AbstractPrivateData *>(item), xsink);
   qtw->qobj->setVerticalHeaderItem(row, item->qore_obj);
   return 0;
}

//void setVerticalHeaderLabels ( const QStringList & labels )
static AbstractQoreNode *QTABLEWIDGET_setVerticalHeaderLabels(QoreObject *self, QoreQTableWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreListNode *p = test_list_param(params, 0);
   if (!p) {
      xsink->raiseException("QTABLEWIDGET-SETVERTICALHEADERLABELS-PARAM-ERROR", "expecting a list as first argument to QTableWidget::setVerticalHeaderLabels()");
      return 0;
   }
   QStringList labels;
   ConstListIterator li_labels(p);
   while (li_labels.next())
   {
      QoreStringNodeValueHelper str(li_labels.getValue());
      QString tmp;
      if (get_qstring(*str, tmp, xsink))
         return 0;
      labels.push_back(tmp);
   }
   qtw->qobj->setVerticalHeaderLabels(labels);
   return 0;
}

//void sortItems ( int column, Qt::SortOrder order = Qt::AscendingOrder )
static AbstractQoreNode *QTABLEWIDGET_sortItems(QoreObject *self, QoreQTableWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int column = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   Qt::SortOrder order = !is_nothing(p) ? (Qt::SortOrder)p->getAsInt() : Qt::AscendingOrder;
   qtw->qobj->sortItems(column, order);
   return 0;
}

//QTableWidgetItem * takeHorizontalHeaderItem ( int column )
static AbstractQoreNode *QTABLEWIDGET_takeHorizontalHeaderItem(QoreObject *self, QoreQTableWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int column = p ? p->getAsInt() : 0;

   QTableWidgetItem *twi = qtw->qobj->takeHorizontalHeaderItem(column);
   if (!twi)
      return 0;

   QoreObject *o_qtwi = new QoreObject(QC_QTableWidgetItem, getProgram());
   QoreQTableWidgetItem *q_qtwi = new QoreQTableWidgetItem(twi);
   o_qtwi->setPrivate(CID_QTABLEWIDGETITEM, q_qtwi);
   return o_qtwi;
}

//QTableWidgetItem * takeItem ( int row, int column )
static AbstractQoreNode *QTABLEWIDGET_takeItem(QoreObject *self, QoreQTableWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int row = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int column = p ? p->getAsInt() : 0;

   QTableWidgetItem *twi = qtw->qobj->takeItem(row, column);
   if (!twi)
      return 0;

   QoreObject *o_qtwi = new QoreObject(QC_QTableWidgetItem, getProgram());
   QoreQTableWidgetItem *q_qtwi = new QoreQTableWidgetItem(twi);
   o_qtwi->setPrivate(CID_QTABLEWIDGETITEM, q_qtwi);
   return o_qtwi;
}

//QTableWidgetItem * takeVerticalHeaderItem ( int row )
static AbstractQoreNode *QTABLEWIDGET_takeVerticalHeaderItem(QoreObject *self, QoreQTableWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int row = p ? p->getAsInt() : 0;

   QTableWidgetItem *twi = qtw->qobj->takeVerticalHeaderItem(row);
   if (!twi)
      return 0;

   QoreObject *o_qtwi = new QoreObject(QC_QTableWidgetItem, getProgram());
   QoreQTableWidgetItem *q_qtwi = new QoreQTableWidgetItem(twi);
   o_qtwi->setPrivate(CID_QTABLEWIDGETITEM, q_qtwi);
   return o_qtwi;
}

//QTableWidgetItem * verticalHeaderItem ( int row ) const
static AbstractQoreNode *QTABLEWIDGET_verticalHeaderItem(QoreObject *self, QoreQTableWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int row = p ? p->getAsInt() : 0;

   QTableWidgetItem *twi = qtw->qobj->verticalHeaderItem(row);
   if (!twi)
      return 0;

   QoreObject *o_qtwi = new QoreObject(QC_QTableWidgetItem, getProgram());
   QoreQTableWidgetItem *q_qtwi = new QoreQTableWidgetItem(twi);
   o_qtwi->setPrivate(CID_QTABLEWIDGETITEM, q_qtwi);
   return o_qtwi;
}

//int visualColumn ( int logicalColumn ) const
static AbstractQoreNode *QTABLEWIDGET_visualColumn(QoreObject *self, QoreQTableWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int logicalColumn = p ? p->getAsInt() : 0;
   return new QoreBigIntNode(qtw->qobj->visualColumn(logicalColumn));
}

//QRect visualItemRect ( const QTableWidgetItem * item ) const
static AbstractQoreNode *QTABLEWIDGET_visualItemRect(QoreObject *self, QoreQTableWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreObject *p = test_object_param(params, 0);
   QoreQTableWidgetItem *item = p ? (QoreQTableWidgetItem *)p->getReferencedPrivateData(CID_QTABLEWIDGETITEM, xsink) : 0;
   if (!item) {
      if (!xsink->isException())
         xsink->raiseException("QTABLEWIDGET-VISUALITEMRECT-PARAM-ERROR", "expecting a QTableWidgetItem object as first argument to QTableWidget::visualItemRect()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> itemHolder(static_cast<AbstractPrivateData *>(item), xsink);
   QoreObject *o_qr = new QoreObject(QC_QRect, getProgram());
   QoreQRect *q_qr = new QoreQRect(qtw->qobj->visualItemRect(item->qore_obj));
   o_qr->setPrivate(CID_QRECT, q_qr);
   return o_qr;
}

//int visualRow ( int logicalRow ) const
static AbstractQoreNode *QTABLEWIDGET_visualRow(QoreObject *self, QoreQTableWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int logicalRow = p ? p->getAsInt() : 0;
   return new QoreBigIntNode(qtw->qobj->visualRow(logicalRow));
}

//void clear ()
static AbstractQoreNode *QTABLEWIDGET_clear(QoreObject *self, QoreQTableWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   qtw->qobj->clear();
   return 0;
}

//void clearContents ()
static AbstractQoreNode *QTABLEWIDGET_clearContents(QoreObject *self, QoreQTableWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   qtw->qobj->clearContents();
   return 0;
}

//void insertColumn ( int column )
static AbstractQoreNode *QTABLEWIDGET_insertColumn(QoreObject *self, QoreQTableWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int column = p ? p->getAsInt() : 0;
   qtw->qobj->insertColumn(column);
   return 0;
}

//void insertRow ( int row )
static AbstractQoreNode *QTABLEWIDGET_insertRow(QoreObject *self, QoreQTableWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int row = p ? p->getAsInt() : 0;
   qtw->qobj->insertRow(row);
   return 0;
}

//void removeColumn ( int column )
static AbstractQoreNode *QTABLEWIDGET_removeColumn(QoreObject *self, QoreQTableWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int column = p ? p->getAsInt() : 0;
   qtw->qobj->removeColumn(column);
   return 0;
}

//void removeRow ( int row )
static AbstractQoreNode *QTABLEWIDGET_removeRow(QoreObject *self, QoreQTableWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int row = p ? p->getAsInt() : 0;
   qtw->qobj->removeRow(row);
   return 0;
}

//void scrollToItem ( const QTableWidgetItem * item, QAbstractItemView::ScrollHint hint = EnsureVisible )
static AbstractQoreNode *QTABLEWIDGET_scrollToItem(QoreObject *self, QoreQTableWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreObject *o = test_object_param(params, 0);
   QoreQTableWidgetItem *item = o ? (QoreQTableWidgetItem *)o->getReferencedPrivateData(CID_QTABLEWIDGETITEM, xsink) : 0;
   if (!item) {
      if (!xsink->isException())
         xsink->raiseException("QTABLEWIDGET-SCROLLTOITEM-PARAM-ERROR", "expecting a QTableWidgetItem object as first argument to QTableWidget::scrollToItem()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> itemHolder(static_cast<AbstractPrivateData *>(item), xsink);

   const AbstractQoreNode *p = get_param(params, 1);
   QAbstractItemView::ScrollHint hint = !is_nothing(p) ? (QAbstractItemView::ScrollHint)p->getAsInt() : QAbstractItemView::EnsureVisible;
   qtw->qobj->scrollToItem(item->qore_obj, hint);
   return 0;
}

QoreClass *initQTableWidgetClass(QoreClass *qwidget)
{
   QC_QTableWidget = new QoreClass("QTableWidget", QDOM_GUI);
   CID_QTABLEWIDGET = QC_QTableWidget->getID();

   QC_QTableWidget->addBuiltinVirtualBaseClass(qwidget);

   QC_QTableWidget->setConstructor(QTABLEWIDGET_constructor);
   QC_QTableWidget->setCopy((q_copy_t)QTABLEWIDGET_copy);

   QC_QTableWidget->addMethod("cellWidget",                  (q_method_t)QTABLEWIDGET_cellWidget);
   QC_QTableWidget->addMethod("closePersistentEditor",       (q_method_t)QTABLEWIDGET_closePersistentEditor);
   QC_QTableWidget->addMethod("column",                      (q_method_t)QTABLEWIDGET_column);
   QC_QTableWidget->addMethod("columnCount",                 (q_method_t)QTABLEWIDGET_columnCount);
   QC_QTableWidget->addMethod("currentColumn",               (q_method_t)QTABLEWIDGET_currentColumn);
   QC_QTableWidget->addMethod("currentItem",                 (q_method_t)QTABLEWIDGET_currentItem);
   QC_QTableWidget->addMethod("currentRow",                  (q_method_t)QTABLEWIDGET_currentRow);
   QC_QTableWidget->addMethod("editItem",                    (q_method_t)QTABLEWIDGET_editItem);
   //QC_QTableWidget->addMethod("findItems",                   (q_method_t)QTABLEWIDGET_findItems);
   QC_QTableWidget->addMethod("horizontalHeaderItem",        (q_method_t)QTABLEWIDGET_horizontalHeaderItem);
   QC_QTableWidget->addMethod("item",                        (q_method_t)QTABLEWIDGET_item);
   QC_QTableWidget->addMethod("itemAt",                      (q_method_t)QTABLEWIDGET_itemAt);
   QC_QTableWidget->addMethod("itemPrototype",               (q_method_t)QTABLEWIDGET_itemPrototype);
   QC_QTableWidget->addMethod("openPersistentEditor",        (q_method_t)QTABLEWIDGET_openPersistentEditor);
   QC_QTableWidget->addMethod("removeCellWidget",            (q_method_t)QTABLEWIDGET_removeCellWidget);
   QC_QTableWidget->addMethod("row",                         (q_method_t)QTABLEWIDGET_row);
   QC_QTableWidget->addMethod("rowCount",                    (q_method_t)QTABLEWIDGET_rowCount);
   //QC_QTableWidget->addMethod("selectedItems",               (q_method_t)QTABLEWIDGET_selectedItems);
   //QC_QTableWidget->addMethod("selectedRanges",              (q_method_t)QTABLEWIDGET_selectedRanges);
   QC_QTableWidget->addMethod("setCellWidget",               (q_method_t)QTABLEWIDGET_setCellWidget);
   QC_QTableWidget->addMethod("setColumnCount",              (q_method_t)QTABLEWIDGET_setColumnCount);
   QC_QTableWidget->addMethod("setCurrentCell",              (q_method_t)QTABLEWIDGET_setCurrentCell);
   QC_QTableWidget->addMethod("setCurrentItem",              (q_method_t)QTABLEWIDGET_setCurrentItem);
   QC_QTableWidget->addMethod("setHorizontalHeaderItem",     (q_method_t)QTABLEWIDGET_setHorizontalHeaderItem);
   QC_QTableWidget->addMethod("setHorizontalHeaderLabels",   (q_method_t)QTABLEWIDGET_setHorizontalHeaderLabels);
   QC_QTableWidget->addMethod("setItem",                     (q_method_t)QTABLEWIDGET_setItem);
   QC_QTableWidget->addMethod("setItemPrototype",            (q_method_t)QTABLEWIDGET_setItemPrototype);
   //QC_QTableWidget->addMethod("setRangeSelected",            (q_method_t)QTABLEWIDGET_setRangeSelected);
   QC_QTableWidget->addMethod("setRowCount",                 (q_method_t)QTABLEWIDGET_setRowCount);
   QC_QTableWidget->addMethod("setVerticalHeaderItem",       (q_method_t)QTABLEWIDGET_setVerticalHeaderItem);
   QC_QTableWidget->addMethod("setVerticalHeaderLabels",     (q_method_t)QTABLEWIDGET_setVerticalHeaderLabels);
   QC_QTableWidget->addMethod("sortItems",                   (q_method_t)QTABLEWIDGET_sortItems);
   QC_QTableWidget->addMethod("takeHorizontalHeaderItem",    (q_method_t)QTABLEWIDGET_takeHorizontalHeaderItem);
   QC_QTableWidget->addMethod("takeItem",                    (q_method_t)QTABLEWIDGET_takeItem);
   QC_QTableWidget->addMethod("takeVerticalHeaderItem",      (q_method_t)QTABLEWIDGET_takeVerticalHeaderItem);
   QC_QTableWidget->addMethod("verticalHeaderItem",          (q_method_t)QTABLEWIDGET_verticalHeaderItem);
   QC_QTableWidget->addMethod("visualColumn",                (q_method_t)QTABLEWIDGET_visualColumn);
   QC_QTableWidget->addMethod("visualItemRect",              (q_method_t)QTABLEWIDGET_visualItemRect);
   QC_QTableWidget->addMethod("visualRow",                   (q_method_t)QTABLEWIDGET_visualRow);
   QC_QTableWidget->addMethod("clear",                       (q_method_t)QTABLEWIDGET_clear);
   QC_QTableWidget->addMethod("clearContents",               (q_method_t)QTABLEWIDGET_clearContents);
   QC_QTableWidget->addMethod("insertColumn",                (q_method_t)QTABLEWIDGET_insertColumn);
   QC_QTableWidget->addMethod("insertRow",                   (q_method_t)QTABLEWIDGET_insertRow);
   QC_QTableWidget->addMethod("removeColumn",                (q_method_t)QTABLEWIDGET_removeColumn);
   QC_QTableWidget->addMethod("removeRow",                   (q_method_t)QTABLEWIDGET_removeRow);
   QC_QTableWidget->addMethod("scrollToItem",                (q_method_t)QTABLEWIDGET_scrollToItem);

   return QC_QTableWidget;
}
