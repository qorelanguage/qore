/*
 QC_QListWidget.cc
 
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

#include "QC_QListWidget.h"
#include "QC_QWidget.h"
#include "QC_QListWidgetItem.h"
#include "QC_QRect.h"
#include "QC_QPoint.h"

#include "qore-qt.h"

int CID_QLISTWIDGET;
class QoreClass *QC_QListWidget = 0;

//QListWidget ( QWidget * parent = 0 )
static void QLISTWIDGET_constructor(QoreObject *self, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQWidget *parent = (p && p->type == NT_OBJECT) ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return;
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   self->setPrivate(CID_QLISTWIDGET, new QoreQListWidget(self, parent ? static_cast<QWidget *>(parent->getQWidget()) : 0));
   return;
}

static void QLISTWIDGET_copy(class QoreObject *self, class QoreObject *old, class QoreQListWidget *qlw, ExceptionSink *xsink)
{
   xsink->raiseException("QLISTWIDGET-COPY-ERROR", "objects of this class cannot be copied");
}

//void addItem ( const QString & label )
//void addItem ( QListWidgetItem * item )
static QoreNode *QLISTWIDGET_addItem(QoreObject *self, QoreAbstractQListWidget *qlw, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQListWidgetItem *item = (QoreQListWidgetItem *)p->val.object->getReferencedPrivateData(CID_QLISTWIDGETITEM, xsink);
      if (!item) {
         if (!xsink->isException())
            xsink->raiseException("QLISTWIDGET-ADDITEM-PARAM-ERROR", "QListWidget::addItem() does not know how to handle arguments of class '%s' as passed as the first argument", p
				  ->val.object->getClass()->getName());
         return 0;
      }
      ReferenceHolder<AbstractPrivateData> itemHolder(static_cast<AbstractPrivateData *>(item), xsink);
      qlw->getQListWidget()->addItem(item->getQListWidgetItem());
      return 0;
   }
   QString label;
   if (get_qstring(p, label, xsink))
      return 0;
   qlw->getQListWidget()->addItem(label);
   return 0;
}

//void addItems ( const QStringList & labels )
static QoreNode *QLISTWIDGET_addItems(QoreObject *self, QoreAbstractQListWidget *qlw, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (!p || p->type != NT_LIST) {
      xsink->raiseException("QLISTWIDGET-ADDITEMS-PARAM-ERROR", "expecting a list as first argument to QListWidget::addItems()");
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
   qlw->getQListWidget()->addItems(labels);
   return 0;
}

//void closePersistentEditor ( QListWidgetItem * item )
static QoreNode *QLISTWIDGET_closePersistentEditor(QoreObject *self, QoreAbstractQListWidget *qlw, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQListWidgetItem *item = (p && p->type == NT_OBJECT) ? (QoreQListWidgetItem *)p->val.object->getReferencedPrivateData(CID_QLISTWIDGETITEM, xsink) : 0;
   if (!item) {
      if (!xsink->isException())
         xsink->raiseException("QLISTWIDGET-CLOSEPERSISTENTEDITOR-PARAM-ERROR", "expecting a QListWidgetItem object as first argument to QListWidget::closePersistentEditor()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> itemHolder(static_cast<AbstractPrivateData *>(item), xsink);
   qlw->getQListWidget()->closePersistentEditor(item->getQListWidgetItem());
   return 0;
}

//int count () const
static QoreNode *QLISTWIDGET_count(QoreObject *self, QoreAbstractQListWidget *qlw, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qlw->getQListWidget()->count());
}

//QListWidgetItem * currentItem () const
static QoreNode *QLISTWIDGET_currentItem(QoreObject *self, QoreAbstractQListWidget *qlw, const QoreNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qlwi = new QoreObject(QC_QListWidgetItem, getProgram());
   QoreQListWidgetItem *q_qlwi = new QoreQListWidgetItem(qlw->getQListWidget()->currentItem());
   o_qlwi->setPrivate(CID_QLISTWIDGETITEM, q_qlwi);
   return new QoreNode(o_qlwi);
}

//int currentRow () const
static QoreNode *QLISTWIDGET_currentRow(QoreObject *self, QoreAbstractQListWidget *qlw, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qlw->getQListWidget()->currentRow());
}

//void editItem ( QListWidgetItem * item )
static QoreNode *QLISTWIDGET_editItem(QoreObject *self, QoreAbstractQListWidget *qlw, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQListWidgetItem *item = (p && p->type == NT_OBJECT) ? (QoreQListWidgetItem *)p->val.object->getReferencedPrivateData(CID_QLISTWIDGETITEM, xsink) : 0;
   if (!item) {
      if (!xsink->isException())
         xsink->raiseException("QLISTWIDGET-EDITITEM-PARAM-ERROR", "expecting a QListWidgetItem object as first argument to QListWidget::editItem()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> itemHolder(static_cast<AbstractPrivateData *>(item), xsink);
   qlw->getQListWidget()->editItem(item->getQListWidgetItem());
   return 0;
}

/*
//QList<QListWidgetItem *> findItems ( const QString & text, Qt::MatchFlags flags ) const
static QoreNode *QLISTWIDGET_findItems(QoreObject *self, QoreAbstractQListWidget *qlw, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString text;
   if (get_qstring(p, text, xsink))
      return 0;
   p = get_param(params, 1);
   Qt::MatchFlags flags = (Qt::MatchFlags)(p ? p->getAsInt() : 0);
   ??? return new QoreNode((int64)qlw->getQListWidget()->findItems(text, flags));
}
*/

 //void insertItem ( int row, QListWidgetItem * item )
 //void insertItem ( int row, const QString & label )
static QoreNode *QLISTWIDGET_insertItem(QoreObject *self, QoreAbstractQListWidget *qlw, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int row = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   QoreQListWidgetItem *item = (p && p->type == NT_OBJECT) ? (QoreQListWidgetItem *)p->val.object->getReferencedPrivateData(CID_QLISTWIDGETITEM, xsink) : 0;
   if (!item) {
      if (*xsink)
	 return 0;
      QString label;
      if (get_qstring(p, label, xsink))
	 return 0;
      qlw->getQListWidget()->insertItem(row, label);
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> itemHolder(static_cast<AbstractPrivateData *>(item), xsink);
   qlw->getQListWidget()->insertItem(row, item->getQListWidgetItem());
   return 0;
}

//void insertItems ( int row, const QStringList & labels )
static QoreNode *QLISTWIDGET_insertItems(QoreObject *self, QoreAbstractQListWidget *qlw, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int row = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   if (!p || p->type != NT_LIST) {
      xsink->raiseException("QLISTWIDGET-INSERTITEMS-PARAM-ERROR", "expecting a list as second argument to QListWidget::insertItems()");
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
   qlw->getQListWidget()->insertItems(row, labels);
   return 0;
}

//bool isSortingEnabled () const
static QoreNode *QLISTWIDGET_isSortingEnabled(QoreObject *self, QoreAbstractQListWidget *qlw, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qlw->getQListWidget()->isSortingEnabled());
}

//QListWidgetItem * item ( int row ) const
static QoreNode *QLISTWIDGET_item(QoreObject *self, QoreAbstractQListWidget *qlw, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int row = p ? p->getAsInt() : 0;
   QoreObject *o_qlwi = new QoreObject(QC_QListWidgetItem, getProgram());
   QoreQListWidgetItem *q_qlwi = new QoreQListWidgetItem(qlw->getQListWidget()->item(row));
   o_qlwi->setPrivate(CID_QLISTWIDGETITEM, q_qlwi);
   return new QoreNode(o_qlwi);
}

//QListWidgetItem * itemAt ( const QPoint & p ) const
//QListWidgetItem * itemAt ( int x, int y ) const
static QoreNode *QLISTWIDGET_itemAt(QoreObject *self, QoreAbstractQListWidget *qlw, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQPoint *point = (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink);
      if (!point) {
         if (!xsink->isException())
            xsink->raiseException("QLISTWIDGET-ITEMAT-PARAM-ERROR", "QListWidget::itemAt() does not know how to handle arguments of class '%s' as passed as the first argument", p->val.object->getClass()->getName());
         return 0;
      }
      ReferenceHolder<AbstractPrivateData> pHolder(static_cast<AbstractPrivateData *>(point), xsink);
      QoreObject *o_qlwi = new QoreObject(QC_QListWidgetItem, getProgram());
      QoreQListWidgetItem *q_qlwi = new QoreQListWidgetItem(qlw->getQListWidget()->itemAt(*(static_cast<QPoint *>(point))));
      o_qlwi->setPrivate(CID_QLISTWIDGETITEM, q_qlwi);
      return new QoreNode(o_qlwi);
   }
   int x = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int y = p ? p->getAsInt() : 0;
   QoreObject *o_qlwi = new QoreObject(QC_QListWidgetItem, getProgram());
   QoreQListWidgetItem *q_qlwi = new QoreQListWidgetItem(qlw->getQListWidget()->itemAt(x, y));
   o_qlwi->setPrivate(CID_QLISTWIDGETITEM, q_qlwi);
   return new QoreNode(o_qlwi);
}

//QWidget * itemWidget ( QListWidgetItem * item ) const
static QoreNode *QLISTWIDGET_itemWidget(QoreObject *self, QoreAbstractQListWidget *qlw, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQListWidgetItem *item = (p && p->type == NT_OBJECT) ? (QoreQListWidgetItem *)p->val.object->getReferencedPrivateData(CID_QLISTWIDGETITEM, xsink) : 0;
   if (!item) {
      if (!xsink->isException())
         xsink->raiseException("QLISTWIDGET-ITEMWIDGET-PARAM-ERROR", "expecting a QListWidgetItem object as first argument to QListWidget::itemWidget()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> itemHolder(static_cast<AbstractPrivateData *>(item), xsink);
   QWidget *qt_qobj = qlw->getQListWidget()->itemWidget(item->getQListWidgetItem());
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   QoreObject *rv_obj = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   if (rv_obj)
      rv_obj->ref();
   else {
      rv_obj = new QoreObject(QC_QWidget, getProgram());
      QoreQtQWidget *t_qobj = new QoreQtQWidget(rv_obj, qt_qobj);
      rv_obj->setPrivate(CID_QWIDGET, t_qobj);
   }
   return new QoreNode(rv_obj);
}

//void openPersistentEditor ( QListWidgetItem * item )
static QoreNode *QLISTWIDGET_openPersistentEditor(QoreObject *self, QoreAbstractQListWidget *qlw, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQListWidgetItem *item = (p && p->type == NT_OBJECT) ? (QoreQListWidgetItem *)p->val.object->getReferencedPrivateData(CID_QLISTWIDGETITEM, xsink) : 0;
   if (!item) {
      if (!xsink->isException())
         xsink->raiseException("QLISTWIDGET-OPENPERSISTENTEDITOR-PARAM-ERROR", "expecting a QListWidgetItem object as first argument to QListWidget::openPersistentEditor()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> itemHolder(static_cast<AbstractPrivateData *>(item), xsink);
   qlw->getQListWidget()->openPersistentEditor(item->getQListWidgetItem());
   return 0;
}

//void removeItemWidget ( QListWidgetItem * item )
static QoreNode *QLISTWIDGET_removeItemWidget(QoreObject *self, QoreAbstractQListWidget *qlw, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQListWidgetItem *item = (p && p->type == NT_OBJECT) ? (QoreQListWidgetItem *)p->val.object->getReferencedPrivateData(CID_QLISTWIDGETITEM, xsink) : 0;
   if (!item) {
      if (!xsink->isException())
         xsink->raiseException("QLISTWIDGET-REMOVEITEMWIDGET-PARAM-ERROR", "expecting a QListWidgetItem object as first argument to QListWidget::removeItemWidget()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> itemHolder(static_cast<AbstractPrivateData *>(item), xsink);
   qlw->getQListWidget()->removeItemWidget(item->getQListWidgetItem());
   return 0;
}

//int row ( const QListWidgetItem * item ) const
static QoreNode *QLISTWIDGET_row(QoreObject *self, QoreAbstractQListWidget *qlw, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQListWidgetItem *item = (p && p->type == NT_OBJECT) ? (QoreQListWidgetItem *)p->val.object->getReferencedPrivateData(CID_QLISTWIDGETITEM, xsink) : 0;
   if (!item) {
      if (!xsink->isException())
         xsink->raiseException("QLISTWIDGET-ROW-PARAM-ERROR", "expecting a QListWidgetItem object as first argument to QListWidget::row()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> itemHolder(static_cast<AbstractPrivateData *>(item), xsink);
   return new QoreNode((int64)qlw->getQListWidget()->row(item->getQListWidgetItem()));
}

/*
//QList<QListWidgetItem *> selectedItems () const
static QoreNode *QLISTWIDGET_selectedItems(QoreObject *self, QoreAbstractQListWidget *qlw, const QoreNode *params, ExceptionSink *xsink)
{
   ??? return new QoreNode((int64)qlw->getQListWidget()->selectedItems());
}
*/

 //void setCurrentItem ( QListWidgetItem * item )
static QoreNode *QLISTWIDGET_setCurrentItem(QoreObject *self, QoreAbstractQListWidget *qlw, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQListWidgetItem *item = (p && p->type == NT_OBJECT) ? (QoreQListWidgetItem *)p->val.object->getReferencedPrivateData(CID_QLISTWIDGETITEM, xsink) : 0;
   if (!item) {
      if (!xsink->isException())
         xsink->raiseException("QLISTWIDGET-SETCURRENTITEM-PARAM-ERROR", "expecting a QListWidgetItem object as first argument to QListWidget::setCurrentItem()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> itemHolder(static_cast<AbstractPrivateData *>(item), xsink);
   qlw->getQListWidget()->setCurrentItem(item->getQListWidgetItem());
   return 0;
}

//void setCurrentRow ( int row )
static QoreNode *QLISTWIDGET_setCurrentRow(QoreObject *self, QoreAbstractQListWidget *qlw, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int row = p ? p->getAsInt() : 0;
   qlw->getQListWidget()->setCurrentRow(row);
   return 0;
}

//void setItemWidget ( QListWidgetItem * item, QWidget * widget )
static QoreNode *QLISTWIDGET_setItemWidget(QoreObject *self, QoreAbstractQListWidget *qlw, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQListWidgetItem *item = (p && p->type == NT_OBJECT) ? (QoreQListWidgetItem *)p->val.object->getReferencedPrivateData(CID_QLISTWIDGETITEM, xsink) : 0;
   if (!item) {
      if (!xsink->isException())
         xsink->raiseException("QLISTWIDGET-SETITEMWIDGET-PARAM-ERROR", "expecting a QListWidgetItem object as first argument to QListWidget::setItemWidget()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> itemHolder(static_cast<AbstractPrivateData *>(item), xsink);
   p = get_param(params, 1);
   QoreQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!widget) {
      if (!xsink->isException())
         xsink->raiseException("QLISTWIDGET-SETITEMWIDGET-PARAM-ERROR", "expecting a QWidget object as second argument to QListWidget::setItemWidget()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   qlw->getQListWidget()->setItemWidget(item->getQListWidgetItem(), static_cast<QWidget *>(widget->getQWidget()));
   return 0;
}

//void setSortingEnabled ( bool enable )
static QoreNode *QLISTWIDGET_setSortingEnabled(QoreObject *self, QoreAbstractQListWidget *qlw, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool enable = p ? p->getAsBool() : false;
   qlw->getQListWidget()->setSortingEnabled(enable);
   return 0;
}

//void sortItems ( Qt::SortOrder order = Qt::AscendingOrder )
static QoreNode *QLISTWIDGET_sortItems(QoreObject *self, QoreAbstractQListWidget *qlw, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   Qt::SortOrder order = !is_nothing(p) ? (Qt::SortOrder)p->getAsInt() : Qt::AscendingOrder;
   qlw->getQListWidget()->sortItems(order);
   return 0;
}

//QListWidgetItem * takeItem ( int row )
static QoreNode *QLISTWIDGET_takeItem(QoreObject *self, QoreAbstractQListWidget *qlw, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int row = p ? p->getAsInt() : 0;
   QoreObject *o_qlwi = new QoreObject(QC_QListWidgetItem, getProgram());
   QoreQListWidgetItem *q_qlwi = new QoreQListWidgetItem(qlw->getQListWidget()->takeItem(row));
   o_qlwi->setPrivate(CID_QLISTWIDGETITEM, q_qlwi);
   return new QoreNode(o_qlwi);
}

//QRect visualItemRect ( const QListWidgetItem * item ) const
static QoreNode *QLISTWIDGET_visualItemRect(QoreObject *self, QoreAbstractQListWidget *qlw, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQListWidgetItem *item = (p && p->type == NT_OBJECT) ? (QoreQListWidgetItem *)p->val.object->getReferencedPrivateData(CID_QLISTWIDGETITEM, xsink) : 0;
   if (!item) {
      if (!xsink->isException())
         xsink->raiseException("QLISTWIDGET-VISUALITEMRECT-PARAM-ERROR", "expecting a QListWidgetItem object as first argument to QListWidget::visualItemRect()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> itemHolder(static_cast<AbstractPrivateData *>(item), xsink);
   QoreObject *o_qr = new QoreObject(QC_QRect, getProgram());
   QoreQRect *q_qr = new QoreQRect(qlw->getQListWidget()->visualItemRect(item->getQListWidgetItem()));
   o_qr->setPrivate(CID_QRECT, q_qr);
   return new QoreNode(o_qr);
}

//void clear ()
static QoreNode *QLISTWIDGET_clear(QoreObject *self, QoreAbstractQListWidget *qlw, const QoreNode *params, ExceptionSink *xsink)
{
   qlw->getQListWidget()->clear();
   return 0;
}

//void scrollToItem ( const QListWidgetItem * item, QAbstractItemView::ScrollHint hint = EnsureVisible )
static QoreNode *QLISTWIDGET_scrollToItem(QoreObject *self, QoreAbstractQListWidget *qlw, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQListWidgetItem *item = (p && p->type == NT_OBJECT) ? (QoreQListWidgetItem *)p->val.object->getReferencedPrivateData(CID_QLISTWIDGETITEM, xsink) : 0;
   if (!item) {
      if (!xsink->isException())
         xsink->raiseException("QLISTWIDGET-SCROLLTOITEM-PARAM-ERROR", "expecting a QListWidgetItem object as first argument to QListWidget::scrollToItem()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> itemHolder(static_cast<AbstractPrivateData *>(item), xsink);
   p = get_param(params, 1);
   QAbstractItemView::ScrollHint hint = !is_nothing(p) ? (QAbstractItemView::ScrollHint)p->getAsInt() : QAbstractItemView::EnsureVisible;
   qlw->getQListWidget()->scrollToItem(item->getQListWidgetItem(), hint);
   return 0;
}

QoreClass *initQListWidgetClass(QoreClass *qlistview)
{
   QC_QListWidget = new QoreClass("QListWidget", QDOM_GUI);
   CID_QLISTWIDGET = QC_QListWidget->getID();

   QC_QListWidget->addBuiltinVirtualBaseClass(qlistview);

   QC_QListWidget->setConstructor(QLISTWIDGET_constructor);
   QC_QListWidget->setCopy((q_copy_t)QLISTWIDGET_copy);

   QC_QListWidget->addMethod("addItem",                     (q_method_t)QLISTWIDGET_addItem);
   QC_QListWidget->addMethod("addItems",                    (q_method_t)QLISTWIDGET_addItems);
   QC_QListWidget->addMethod("closePersistentEditor",       (q_method_t)QLISTWIDGET_closePersistentEditor);
   QC_QListWidget->addMethod("count",                       (q_method_t)QLISTWIDGET_count);
   QC_QListWidget->addMethod("currentItem",                 (q_method_t)QLISTWIDGET_currentItem);
   QC_QListWidget->addMethod("currentRow",                  (q_method_t)QLISTWIDGET_currentRow);
   QC_QListWidget->addMethod("editItem",                    (q_method_t)QLISTWIDGET_editItem);
   //QC_QListWidget->addMethod("findItems",                   (q_method_t)QLISTWIDGET_findItems);
   QC_QListWidget->addMethod("insertItem",                  (q_method_t)QLISTWIDGET_insertItem);
   QC_QListWidget->addMethod("insertItems",                 (q_method_t)QLISTWIDGET_insertItems);
   QC_QListWidget->addMethod("isSortingEnabled",            (q_method_t)QLISTWIDGET_isSortingEnabled);
   QC_QListWidget->addMethod("item",                        (q_method_t)QLISTWIDGET_item);
   QC_QListWidget->addMethod("itemAt",                      (q_method_t)QLISTWIDGET_itemAt);
   QC_QListWidget->addMethod("itemWidget",                  (q_method_t)QLISTWIDGET_itemWidget);
   QC_QListWidget->addMethod("openPersistentEditor",        (q_method_t)QLISTWIDGET_openPersistentEditor);
   QC_QListWidget->addMethod("removeItemWidget",            (q_method_t)QLISTWIDGET_removeItemWidget);
   QC_QListWidget->addMethod("row",                         (q_method_t)QLISTWIDGET_row);
   //QC_QListWidget->addMethod("selectedItems",               (q_method_t)QLISTWIDGET_selectedItems);
   QC_QListWidget->addMethod("setCurrentItem",              (q_method_t)QLISTWIDGET_setCurrentItem);
   QC_QListWidget->addMethod("setCurrentRow",               (q_method_t)QLISTWIDGET_setCurrentRow);
   QC_QListWidget->addMethod("setItemWidget",               (q_method_t)QLISTWIDGET_setItemWidget);
   QC_QListWidget->addMethod("setSortingEnabled",           (q_method_t)QLISTWIDGET_setSortingEnabled);
   QC_QListWidget->addMethod("sortItems",                   (q_method_t)QLISTWIDGET_sortItems);
   QC_QListWidget->addMethod("takeItem",                    (q_method_t)QLISTWIDGET_takeItem);
   QC_QListWidget->addMethod("visualItemRect",              (q_method_t)QLISTWIDGET_visualItemRect);
   QC_QListWidget->addMethod("clear",                       (q_method_t)QLISTWIDGET_clear);
   QC_QListWidget->addMethod("scrollToItem",                (q_method_t)QLISTWIDGET_scrollToItem);

   return QC_QListWidget;
}
