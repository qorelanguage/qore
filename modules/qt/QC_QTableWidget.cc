/*
 QC_QTableWidget.cc
 
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

#include "QC_QTableWidget.h"

int CID_QTABLEWIDGET;
class QoreClass *QC_QTableWidget = 0;

//QTableWidget ( QWidget * parent = 0 )
//QTableWidget ( int rows, int columns, QWidget * parent = 0 )
static void QTABLEWIDGET_constructor(Object *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQWidget *parent = (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink);
      if (*xsink)
         return;
      if (!parent) {
         xsink->raiseException("QTABLEWIDGET-CONSTRUCTOR-ERROR", "QTableWidget::constructor() cannot handle arguments of class '%s'", p->val.object->getClass()->getName());
         return;
      }
      ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
      self->setPrivate(CID_QTABLEWIDGET, new QoreQTableWidget(self, parent->getQWidget()));
      return;
   }
   int rows = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int columns = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   QoreQWidget *parent = (p && p->type == NT_OBJECT) ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return;
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   self->setPrivate(CID_QTABLEWIDGET, new QoreQTableWidget(self, rows, columns, parent ? static_cast<QWidget *>(parent->getQWidget()) : 0));
   return;
}

static void QTABLEWIDGET_copy(class Object *self, class Object *old, class QoreQTableWidget *qtw, ExceptionSink *xsink)
{
   xsink->raiseException("QTABLEWIDGET-COPY-ERROR", "objects of this class cannot be copied");
}

//QWidget * cellWidget ( int row, int column ) const
static QoreNode *QTABLEWIDGET_cellWidget(Object *self, QoreQTableWidget *qtw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int row = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int column = p ? p->getAsInt() : 0;
   QWidget *qt_qobj = qtw->qobj->cellWidget(row, column);
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   Object *rv_obj = reinterpret_cast<Object *>(qv_ptr.toULongLong());
   assert(rv_obj);
   rv_obj->ref();
   return new QoreNode(rv_obj);
}

//void closePersistentEditor ( QTableWidgetItem * item )
static QoreNode *QTABLEWIDGET_closePersistentEditor(Object *self, QoreQTableWidget *qtw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQTableWidgetItem *item = (p && p->type == NT_OBJECT) ? (QoreQTableWidgetItem *)p->val.object->getReferencedPrivateData(CID_QTABLEWIDGETITEM, xsink) : 0;
   if (!item) {
      if (!xsink->isException())
         xsink->raiseException("QTABLEWIDGET-CLOSEPERSISTENTEDITOR-PARAM-ERROR", "expecting a QTableWidgetItem object as first argument to QTableWidget::closePersistentEditor()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> itemHolder(static_cast<AbstractPrivateData *>(item), xsink);
   qtw->qobj->closePersistentEditor(static_cast<QTableWidgetItem *>(item));
   return 0;
}

//int column ( const QTableWidgetItem * item ) const
static QoreNode *QTABLEWIDGET_column(Object *self, QoreQTableWidget *qtw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQTableWidgetItem *item = (p && p->type == NT_OBJECT) ? (QoreQTableWidgetItem *)p->val.object->getReferencedPrivateData(CID_QTABLEWIDGETITEM, xsink) : 0;
   if (!item) {
      if (!xsink->isException())
         xsink->raiseException("QTABLEWIDGET-COLUMN-PARAM-ERROR", "expecting a QTableWidgetItem object as first argument to QTableWidget::column()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> itemHolder(static_cast<AbstractPrivateData *>(item), xsink);
   return new QoreNode((int64)qtw->qobj->column(static_cast<QTableWidgetItem *>(item)));
}

//int columnCount () const
static QoreNode *QTABLEWIDGET_columnCount(Object *self, QoreQTableWidget *qtw, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qtw->qobj->columnCount());
}

//int currentColumn () const
static QoreNode *QTABLEWIDGET_currentColumn(Object *self, QoreQTableWidget *qtw, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qtw->qobj->currentColumn());
}

//QTableWidgetItem * currentItem () const
static QoreNode *QTABLEWIDGET_currentItem(Object *self, QoreQTableWidget *qtw, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qtwi = new Object(QC_QTableWidgetItem, getProgram());
   QoreQTableWidgetItem *q_qtwi = new QoreQTableWidgetItem(*(qtw->qobj->currentItem()));
   o_qtwi->setPrivate(CID_QTABLEWIDGETITEM, q_qtwi);
   return new QoreNode(o_qtwi);
}

//int currentRow () const
static QoreNode *QTABLEWIDGET_currentRow(Object *self, QoreQTableWidget *qtw, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qtw->qobj->currentRow());
}

//void editItem ( QTableWidgetItem * item )
static QoreNode *QTABLEWIDGET_editItem(Object *self, QoreQTableWidget *qtw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQTableWidgetItem *item = (p && p->type == NT_OBJECT) ? (QoreQTableWidgetItem *)p->val.object->getReferencedPrivateData(CID_QTABLEWIDGETITEM, xsink) : 0;
   if (!item) {
      if (!xsink->isException())
         xsink->raiseException("QTABLEWIDGET-EDITITEM-PARAM-ERROR", "expecting a QTableWidgetItem object as first argument to QTableWidget::editItem()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> itemHolder(static_cast<AbstractPrivateData *>(item), xsink);
   qtw->qobj->editItem(static_cast<QTableWidgetItem *>(item));
   return 0;
}

////QList<QTableWidgetItem *> findItems ( const QString & text, Qt::MatchFlags flags ) const
//static QoreNode *QTABLEWIDGET_findItems(Object *self, QoreQTableWidget *qtw, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   QString text;
//   if (get_qstring(p, text, xsink))
//      return 0;
//   p = get_param(params, 1);
//   Qt::MatchFlags flags = (Qt::MatchFlags)(p ? p->getAsInt() : 0);
//   ??? return new QoreNode((int64)qtw->qobj->findItems(text, flags));
//}

//QTableWidgetItem * horizontalHeaderItem ( int column ) const
static QoreNode *QTABLEWIDGET_horizontalHeaderItem(Object *self, QoreQTableWidget *qtw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int column = p ? p->getAsInt() : 0;
   Object *o_qtwi = new Object(QC_QTableWidgetItem, getProgram());
   QoreQTableWidgetItem *q_qtwi = new QoreQTableWidgetItem(*(qtw->qobj->horizontalHeaderItem(column)));
   o_qtwi->setPrivate(CID_QTABLEWIDGETITEM, q_qtwi);
   return new QoreNode(o_qtwi);
}

//QTableWidgetItem * item ( int row, int column ) const
static QoreNode *QTABLEWIDGET_item(Object *self, QoreQTableWidget *qtw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int row = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int column = p ? p->getAsInt() : 0;
   Object *o_qtwi = new Object(QC_QTableWidgetItem, getProgram());
   QoreQTableWidgetItem *q_qtwi = new QoreQTableWidgetItem(*(qtw->qobj->item(row, column)));
   o_qtwi->setPrivate(CID_QTABLEWIDGETITEM, q_qtwi);
   return new QoreNode(o_qtwi);
}

//QTableWidgetItem * itemAt ( const QPoint & point ) const
//QTableWidgetItem * itemAt ( int ax, int ay ) const
static QoreNode *QTABLEWIDGET_itemAt(Object *self, QoreQTableWidget *qtw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQPoint *point = (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink);
      if (!point) {
         if (!xsink->isException())
            xsink->raiseException("QTABLEWIDGET-ITEMAT-PARAM-ERROR", "QTableWidget::itemAt() does not know how to handle arguments of class '%s' as passed as the first argument", p->val.object->getClass()->getName());
         return 0;
      }
      ReferenceHolder<AbstractPrivateData> pointHolder(static_cast<AbstractPrivateData *>(point), xsink);
      Object *o_qtwi = new Object(QC_QTableWidgetItem, getProgram());
      QoreQTableWidgetItem *q_qtwi = new QoreQTableWidgetItem(*(qtw->qobj->itemAt(*(static_cast<QPoint *>(point)))));
      o_qtwi->setPrivate(CID_QTABLEWIDGETITEM, q_qtwi);
      return new QoreNode(o_qtwi);
   }
   int ax = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int ay = p ? p->getAsInt() : 0;
   Object *o_qtwi = new Object(QC_QTableWidgetItem, getProgram());
   QoreQTableWidgetItem *q_qtwi = new QoreQTableWidgetItem(*(qtw->qobj->itemAt(ax, ay)));
   o_qtwi->setPrivate(CID_QTABLEWIDGETITEM, q_qtwi);
   return new QoreNode(o_qtwi);
}

//const QTableWidgetItem * itemPrototype () const
static QoreNode *QTABLEWIDGET_itemPrototype(Object *self, QoreQTableWidget *qtw, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qtwi = new Object(QC_QTableWidgetItem, getProgram());
   QoreQTableWidgetItem *q_qtwi = new QoreQTableWidgetItem(*(qtw->qobj->itemPrototype()));
   o_qtwi->setPrivate(CID_QTABLEWIDGETITEM, q_qtwi);
   return new QoreNode(o_qtwi);
}

//void openPersistentEditor ( QTableWidgetItem * item )
static QoreNode *QTABLEWIDGET_openPersistentEditor(Object *self, QoreQTableWidget *qtw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQTableWidgetItem *item = (p && p->type == NT_OBJECT) ? (QoreQTableWidgetItem *)p->val.object->getReferencedPrivateData(CID_QTABLEWIDGETITEM, xsink) : 0;
   if (!item) {
      if (!xsink->isException())
         xsink->raiseException("QTABLEWIDGET-OPENPERSISTENTEDITOR-PARAM-ERROR", "expecting a QTableWidgetItem object as first argument to QTableWidget::openPersistentEditor()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> itemHolder(static_cast<AbstractPrivateData *>(item), xsink);
   qtw->qobj->openPersistentEditor(static_cast<QTableWidgetItem *>(item));
   return 0;
}

//void removeCellWidget ( int row, int column )
static QoreNode *QTABLEWIDGET_removeCellWidget(Object *self, QoreQTableWidget *qtw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int row = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int column = p ? p->getAsInt() : 0;
   qtw->qobj->removeCellWidget(row, column);
   return 0;
}

//int row ( const QTableWidgetItem * item ) const
static QoreNode *QTABLEWIDGET_row(Object *self, QoreQTableWidget *qtw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQTableWidgetItem *item = (p && p->type == NT_OBJECT) ? (QoreQTableWidgetItem *)p->val.object->getReferencedPrivateData(CID_QTABLEWIDGETITEM, xsink) : 0;
   if (!item) {
      if (!xsink->isException())
         xsink->raiseException("QTABLEWIDGET-ROW-PARAM-ERROR", "expecting a QTableWidgetItem object as first argument to QTableWidget::row()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> itemHolder(static_cast<AbstractPrivateData *>(item), xsink);
   return new QoreNode((int64)qtw->qobj->row(static_cast<QTableWidgetItem *>(item)));
}

//int rowCount () const
static QoreNode *QTABLEWIDGET_rowCount(Object *self, QoreQTableWidget *qtw, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qtw->qobj->rowCount());
}

////QList<QTableWidgetItem *> selectedItems ()
//static QoreNode *QTABLEWIDGET_selectedItems(Object *self, QoreQTableWidget *qtw, QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreNode((int64)qtw->qobj->selectedItems());
//}

////QList<QTableWidgetSelectionRange> selectedRanges () const
//static QoreNode *QTABLEWIDGET_selectedRanges(Object *self, QoreQTableWidget *qtw, QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreNode((int64)qtw->qobj->selectedRanges());
//}

//void setCellWidget ( int row, int column, QWidget * widget )
static QoreNode *QTABLEWIDGET_setCellWidget(Object *self, QoreQTableWidget *qtw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int row = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int column = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   QoreQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
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
static QoreNode *QTABLEWIDGET_setColumnCount(Object *self, QoreQTableWidget *qtw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int columns = p ? p->getAsInt() : 0;
   qtw->qobj->setColumnCount(columns);
   return 0;
}

//void setCurrentCell ( int row, int column )
static QoreNode *QTABLEWIDGET_setCurrentCell(Object *self, QoreQTableWidget *qtw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int row = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int column = p ? p->getAsInt() : 0;
   qtw->qobj->setCurrentCell(row, column);
   return 0;
}

//void setCurrentItem ( QTableWidgetItem * item )
static QoreNode *QTABLEWIDGET_setCurrentItem(Object *self, QoreQTableWidget *qtw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQTableWidgetItem *item = (p && p->type == NT_OBJECT) ? (QoreQTableWidgetItem *)p->val.object->getReferencedPrivateData(CID_QTABLEWIDGETITEM, xsink) : 0;
   if (!item) {
      if (!xsink->isException())
         xsink->raiseException("QTABLEWIDGET-SETCURRENTITEM-PARAM-ERROR", "expecting a QTableWidgetItem object as first argument to QTableWidget::setCurrentItem()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> itemHolder(static_cast<AbstractPrivateData *>(item), xsink);
   qtw->qobj->setCurrentItem(static_cast<QTableWidgetItem *>(item));
   return 0;
}

//void setHorizontalHeaderItem ( int column, QTableWidgetItem * item )
static QoreNode *QTABLEWIDGET_setHorizontalHeaderItem(Object *self, QoreQTableWidget *qtw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int column = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   QoreQTableWidgetItem *item = (p && p->type == NT_OBJECT) ? (QoreQTableWidgetItem *)p->val.object->getReferencedPrivateData(CID_QTABLEWIDGETITEM, xsink) : 0;
   if (!item) {
      if (!xsink->isException())
         xsink->raiseException("QTABLEWIDGET-SETHORIZONTALHEADERITEM-PARAM-ERROR", "expecting a QTableWidgetItem object as second argument to QTableWidget::setHorizontalHeaderItem()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> itemHolder(static_cast<AbstractPrivateData *>(item), xsink);
   qtw->qobj->setHorizontalHeaderItem(column, static_cast<QTableWidgetItem *>(item));
   return 0;
}

//void setHorizontalHeaderLabels ( const QStringList & labels )
static QoreNode *QTABLEWIDGET_setHorizontalHeaderLabels(Object *self, QoreQTableWidget *qtw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (!p || p->type != NT_LIST) {
      xsink->raiseException("QTABLEWIDGET-SETHORIZONTALHEADERLABELS-PARAM-ERROR", "expecting a list as first argument to QTableWidget::setHorizontalHeaderLabels()");
      return 0;
   }
   QStringList labels;
   ListIterator li_labels(p->val.list);
   while (li_labels.next())
   {
      QoreNodeTypeHelper str(li_labels.getValue(), NT_STRING, xsink);
      if (*xsink)
         return 0;
      QString tmp;
      if (get_qstring(*str, tmp, xsink))
         return 0;
      labels.push_back(tmp);
   }
   qtw->qobj->setHorizontalHeaderLabels(labels);
   return 0;
}

//void setItem ( int row, int column, QTableWidgetItem * item )
static QoreNode *QTABLEWIDGET_setItem(Object *self, QoreQTableWidget *qtw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int row = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int column = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   QoreQTableWidgetItem *item = (p && p->type == NT_OBJECT) ? (QoreQTableWidgetItem *)p->val.object->getReferencedPrivateData(CID_QTABLEWIDGETITEM, xsink) : 0;
   if (!item) {
      if (!xsink->isException())
         xsink->raiseException("QTABLEWIDGET-SETITEM-PARAM-ERROR", "expecting a QTableWidgetItem object as third argument to QTableWidget::setItem()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> itemHolder(static_cast<AbstractPrivateData *>(item), xsink);
   qtw->qobj->setItem(row, column, static_cast<QTableWidgetItem *>(item));
   return 0;
}

//void setItemPrototype ( const QTableWidgetItem * item )
static QoreNode *QTABLEWIDGET_setItemPrototype(Object *self, QoreQTableWidget *qtw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQTableWidgetItem *item = (p && p->type == NT_OBJECT) ? (QoreQTableWidgetItem *)p->val.object->getReferencedPrivateData(CID_QTABLEWIDGETITEM, xsink) : 0;
   if (!item) {
      if (!xsink->isException())
         xsink->raiseException("QTABLEWIDGET-SETITEMPROTOTYPE-PARAM-ERROR", "expecting a QTableWidgetItem object as first argument to QTableWidget::setItemPrototype()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> itemHolder(static_cast<AbstractPrivateData *>(item), xsink);
   qtw->qobj->setItemPrototype(static_cast<QTableWidgetItem *>(item));
   return 0;
}

////void setRangeSelected ( const QTableWidgetSelectionRange & range, bool select )
//static QoreNode *QTABLEWIDGET_setRangeSelected(Object *self, QoreQTableWidget *qtw, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QTableWidgetSelectionRange range = p;
//   p = get_param(params, 1);
//   bool select = p ? p->getAsBool() : false;
//   qtw->qobj->setRangeSelected(range, select);
//   return 0;
//}

//void setRowCount ( int rows )
static QoreNode *QTABLEWIDGET_setRowCount(Object *self, QoreQTableWidget *qtw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int rows = p ? p->getAsInt() : 0;
   qtw->qobj->setRowCount(rows);
   return 0;
}

//void setVerticalHeaderItem ( int row, QTableWidgetItem * item )
static QoreNode *QTABLEWIDGET_setVerticalHeaderItem(Object *self, QoreQTableWidget *qtw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int row = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   QoreQTableWidgetItem *item = (p && p->type == NT_OBJECT) ? (QoreQTableWidgetItem *)p->val.object->getReferencedPrivateData(CID_QTABLEWIDGETITEM, xsink) : 0;
   if (!item) {
      if (!xsink->isException())
         xsink->raiseException("QTABLEWIDGET-SETVERTICALHEADERITEM-PARAM-ERROR", "expecting a QTableWidgetItem object as second argument to QTableWidget::setVerticalHeaderItem()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> itemHolder(static_cast<AbstractPrivateData *>(item), xsink);
   qtw->qobj->setVerticalHeaderItem(row, static_cast<QTableWidgetItem *>(item));
   return 0;
}

//void setVerticalHeaderLabels ( const QStringList & labels )
static QoreNode *QTABLEWIDGET_setVerticalHeaderLabels(Object *self, QoreQTableWidget *qtw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (!p || p->type != NT_LIST) {
      xsink->raiseException("QTABLEWIDGET-SETVERTICALHEADERLABELS-PARAM-ERROR", "expecting a list as first argument to QTableWidget::setVerticalHeaderLabels()");
      return 0;
   }
   QStringList labels;
   ListIterator li_labels(p->val.list);
   while (li_labels.next())
   {
      QoreNodeTypeHelper str(li_labels.getValue(), NT_STRING, xsink);
      if (*xsink)
         return 0;
      QString tmp;
      if (get_qstring(*str, tmp, xsink))
         return 0;
      labels.push_back(tmp);
   }
   qtw->qobj->setVerticalHeaderLabels(labels);
   return 0;
}

//void sortItems ( int column, Qt::SortOrder order = Qt::AscendingOrder )
static QoreNode *QTABLEWIDGET_sortItems(Object *self, QoreQTableWidget *qtw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int column = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   Qt::SortOrder order = !is_nothing(p) ? (Qt::SortOrder)p->getAsInt() : Qt::AscendingOrder;
   qtw->qobj->sortItems(column, order);
   return 0;
}

//QTableWidgetItem * takeHorizontalHeaderItem ( int column )
static QoreNode *QTABLEWIDGET_takeHorizontalHeaderItem(Object *self, QoreQTableWidget *qtw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int column = p ? p->getAsInt() : 0;
   Object *o_qtwi = new Object(QC_QTableWidgetItem, getProgram());
   QoreQTableWidgetItem *q_qtwi = new QoreQTableWidgetItem(*(qtw->qobj->takeHorizontalHeaderItem(column)));
   o_qtwi->setPrivate(CID_QTABLEWIDGETITEM, q_qtwi);
   return new QoreNode(o_qtwi);
}

//QTableWidgetItem * takeItem ( int row, int column )
static QoreNode *QTABLEWIDGET_takeItem(Object *self, QoreQTableWidget *qtw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int row = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int column = p ? p->getAsInt() : 0;
   Object *o_qtwi = new Object(QC_QTableWidgetItem, getProgram());
   QoreQTableWidgetItem *q_qtwi = new QoreQTableWidgetItem(*(qtw->qobj->takeItem(row, column)));
   o_qtwi->setPrivate(CID_QTABLEWIDGETITEM, q_qtwi);
   return new QoreNode(o_qtwi);
}

//QTableWidgetItem * takeVerticalHeaderItem ( int row )
static QoreNode *QTABLEWIDGET_takeVerticalHeaderItem(Object *self, QoreQTableWidget *qtw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int row = p ? p->getAsInt() : 0;
   Object *o_qtwi = new Object(QC_QTableWidgetItem, getProgram());
   QoreQTableWidgetItem *q_qtwi = new QoreQTableWidgetItem(*(qtw->qobj->takeVerticalHeaderItem(row)));
   o_qtwi->setPrivate(CID_QTABLEWIDGETITEM, q_qtwi);
   return new QoreNode(o_qtwi);
}

//QTableWidgetItem * verticalHeaderItem ( int row ) const
static QoreNode *QTABLEWIDGET_verticalHeaderItem(Object *self, QoreQTableWidget *qtw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int row = p ? p->getAsInt() : 0;
   Object *o_qtwi = new Object(QC_QTableWidgetItem, getProgram());
   QoreQTableWidgetItem *q_qtwi = new QoreQTableWidgetItem(*(qtw->qobj->verticalHeaderItem(row)));
   o_qtwi->setPrivate(CID_QTABLEWIDGETITEM, q_qtwi);
   return new QoreNode(o_qtwi);
}

//int visualColumn ( int logicalColumn ) const
static QoreNode *QTABLEWIDGET_visualColumn(Object *self, QoreQTableWidget *qtw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int logicalColumn = p ? p->getAsInt() : 0;
   return new QoreNode((int64)qtw->qobj->visualColumn(logicalColumn));
}

//QRect visualItemRect ( const QTableWidgetItem * item ) const
static QoreNode *QTABLEWIDGET_visualItemRect(Object *self, QoreQTableWidget *qtw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQTableWidgetItem *item = (p && p->type == NT_OBJECT) ? (QoreQTableWidgetItem *)p->val.object->getReferencedPrivateData(CID_QTABLEWIDGETITEM, xsink) : 0;
   if (!item) {
      if (!xsink->isException())
         xsink->raiseException("QTABLEWIDGET-VISUALITEMRECT-PARAM-ERROR", "expecting a QTableWidgetItem object as first argument to QTableWidget::visualItemRect()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> itemHolder(static_cast<AbstractPrivateData *>(item), xsink);
   Object *o_qr = new Object(QC_QRect, getProgram());
   QoreQRect *q_qr = new QoreQRect(qtw->qobj->visualItemRect(static_cast<QTableWidgetItem *>(item)));
   o_qr->setPrivate(CID_QRECT, q_qr);
   return new QoreNode(o_qr);
}

//int visualRow ( int logicalRow ) const
static QoreNode *QTABLEWIDGET_visualRow(Object *self, QoreQTableWidget *qtw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int logicalRow = p ? p->getAsInt() : 0;
   return new QoreNode((int64)qtw->qobj->visualRow(logicalRow));
}

//void clear ()
static QoreNode *QTABLEWIDGET_clear(Object *self, QoreQTableWidget *qtw, QoreNode *params, ExceptionSink *xsink)
{
   qtw->qobj->clear();
   return 0;
}

//void clearContents ()
static QoreNode *QTABLEWIDGET_clearContents(Object *self, QoreQTableWidget *qtw, QoreNode *params, ExceptionSink *xsink)
{
   qtw->qobj->clearContents();
   return 0;
}

//void insertColumn ( int column )
static QoreNode *QTABLEWIDGET_insertColumn(Object *self, QoreQTableWidget *qtw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int column = p ? p->getAsInt() : 0;
   qtw->qobj->insertColumn(column);
   return 0;
}

//void insertRow ( int row )
static QoreNode *QTABLEWIDGET_insertRow(Object *self, QoreQTableWidget *qtw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int row = p ? p->getAsInt() : 0;
   qtw->qobj->insertRow(row);
   return 0;
}

//void removeColumn ( int column )
static QoreNode *QTABLEWIDGET_removeColumn(Object *self, QoreQTableWidget *qtw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int column = p ? p->getAsInt() : 0;
   qtw->qobj->removeColumn(column);
   return 0;
}

//void removeRow ( int row )
static QoreNode *QTABLEWIDGET_removeRow(Object *self, QoreQTableWidget *qtw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int row = p ? p->getAsInt() : 0;
   qtw->qobj->removeRow(row);
   return 0;
}

//void scrollToItem ( const QTableWidgetItem * item, QAbstractItemView::ScrollHint hint = EnsureVisible )
static QoreNode *QTABLEWIDGET_scrollToItem(Object *self, QoreQTableWidget *qtw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQTableWidgetItem *item = (p && p->type == NT_OBJECT) ? (QoreQTableWidgetItem *)p->val.object->getReferencedPrivateData(CID_QTABLEWIDGETITEM, xsink) : 0;
   if (!item) {
      if (!xsink->isException())
         xsink->raiseException("QTABLEWIDGET-SCROLLTOITEM-PARAM-ERROR", "expecting a QTableWidgetItem object as first argument to QTableWidget::scrollToItem()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> itemHolder(static_cast<AbstractPrivateData *>(item), xsink);
   p = get_param(params, 1);
   QAbstractItemView::ScrollHint hint = !is_nothing(p) ? (QAbstractItemView::ScrollHint)p->getAsInt() : QAbstractItemView::EnsureVisible;
   qtw->qobj->scrollToItem(static_cast<QTableWidgetItem *>(item), hint);
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
