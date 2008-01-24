/*
 QC_QAbstractItemView.cc
 
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

#include "QC_QAbstractItemView.h"
#include "QC_QModelIndex.h"
#include "QC_QPoint.h"
#include "QC_QWidget.h"
#include "QC_QAbstractItemDelegate.h"
#include "QC_QRect.h"

#include "qore-qt.h"

int CID_QABSTRACTITEMVIEW;
class QoreClass *QC_QAbstractItemView = 0;

//QAbstractItemView ( QWidget * parent = 0 )
static void QABSTRACTITEMVIEW_constructor(QoreObject *self, const QoreList *params, ExceptionSink *xsink)
{
   xsink->raiseException("QABSTRACTITEMVIEW-CONSTRUCTOR-ERROR", "QAbstractItemView is an abstract class");
   return;
}

static void QABSTRACTITEMVIEW_copy(class QoreObject *self, class QoreObject *old, class QoreAbstractQAbstractItemView *qaiv, ExceptionSink *xsink)
{
   xsink->raiseException("QABSTRACTITEMVIEW-COPY-ERROR", "objects of this class cannot be copied");
}

//bool alternatingRowColors () const
static QoreNode *QABSTRACTITEMVIEW_alternatingRowColors(QoreObject *self, QoreAbstractQAbstractItemView *qaiv, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode(qaiv->getQAbstractItemView()->alternatingRowColors());
}

//void closePersistentEditor ( const QModelIndex & index )
static QoreNode *QABSTRACTITEMVIEW_closePersistentEditor(QoreObject *self, QoreAbstractQAbstractItemView *qaiv, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQModelIndex *index = (p && p->type == NT_OBJECT) ? (QoreQModelIndex *)p->val.object->getReferencedPrivateData(CID_QMODELINDEX, xsink) : 0;
   if (!index) {
      if (!xsink->isException())
         xsink->raiseException("QABSTRACTITEMVIEW-CLOSEPERSISTENTEDITOR-PARAM-ERROR", "expecting a QModelIndex object as first argument to QAbstractItemView::closePersistentEditor()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> indexHolder(static_cast<AbstractPrivateData *>(index), xsink);
   qaiv->getQAbstractItemView()->closePersistentEditor(*(static_cast<QModelIndex *>(index)));
   return 0;
}

//QModelIndex currentIndex () const
static QoreNode *QABSTRACTITEMVIEW_currentIndex(QoreObject *self, QoreAbstractQAbstractItemView *qaiv, const QoreList *params, ExceptionSink *xsink)
{
   QoreObject *o_qmi = new QoreObject(QC_QModelIndex, getProgram());
   QoreQModelIndex *q_qmi = new QoreQModelIndex(qaiv->getQAbstractItemView()->currentIndex());
   o_qmi->setPrivate(CID_QMODELINDEX, q_qmi);
   return new QoreNode(o_qmi);
}

//DragDropMode dragDropMode () const
static QoreNode *QABSTRACTITEMVIEW_dragDropMode(QoreObject *self, QoreAbstractQAbstractItemView *qaiv, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qaiv->getQAbstractItemView()->dragDropMode());
}

//bool dragDropOverwriteMode () const
static QoreNode *QABSTRACTITEMVIEW_dragDropOverwriteMode(QoreObject *self, QoreAbstractQAbstractItemView *qaiv, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode(qaiv->getQAbstractItemView()->dragDropOverwriteMode());
}

//bool dragEnabled () const
static QoreNode *QABSTRACTITEMVIEW_dragEnabled(QoreObject *self, QoreAbstractQAbstractItemView *qaiv, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode(qaiv->getQAbstractItemView()->dragEnabled());
}

//EditTriggers editTriggers () const
static QoreNode *QABSTRACTITEMVIEW_editTriggers(QoreObject *self, QoreAbstractQAbstractItemView *qaiv, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qaiv->getQAbstractItemView()->editTriggers());
}

//bool hasAutoScroll () const
static QoreNode *QABSTRACTITEMVIEW_hasAutoScroll(QoreObject *self, QoreAbstractQAbstractItemView *qaiv, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode(qaiv->getQAbstractItemView()->hasAutoScroll());
}

//ScrollMode horizontalScrollMode () const
static QoreNode *QABSTRACTITEMVIEW_horizontalScrollMode(QoreObject *self, QoreAbstractQAbstractItemView *qaiv, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qaiv->getQAbstractItemView()->horizontalScrollMode());
}

//QSize iconSize () const
static QoreNode *QABSTRACTITEMVIEW_iconSize(QoreObject *self, QoreAbstractQAbstractItemView *qaiv, const QoreList *params, ExceptionSink *xsink)
{
   QoreObject *o_qs = new QoreObject(QC_QSize, getProgram());
   QoreQSize *q_qs = new QoreQSize(qaiv->getQAbstractItemView()->iconSize());
   o_qs->setPrivate(CID_QSIZE, q_qs);
   return new QoreNode(o_qs);
}

//virtual QModelIndex indexAt ( const QPoint & point ) const = 0
static QoreNode *QABSTRACTITEMVIEW_indexAt(QoreObject *self, QoreAbstractQAbstractItemView *qaiv, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQPoint *point = (p && p->type == NT_OBJECT) ? (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink) : 0;
   if (!point) {
      if (!xsink->isException())
         xsink->raiseException("QABSTRACTITEMVIEW-INDEXAT-PARAM-ERROR", "expecting a QPoint object as first argument to QAbstractItemView::indexAt()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> pointHolder(static_cast<AbstractPrivateData *>(point), xsink);
   QoreObject *o_qmi = new QoreObject(QC_QModelIndex, getProgram());
   QoreQModelIndex *q_qmi = new QoreQModelIndex(qaiv->getQAbstractItemView()->indexAt(*(static_cast<QPoint *>(point))));
   o_qmi->setPrivate(CID_QMODELINDEX, q_qmi);
   return new QoreNode(o_qmi);
}

//QWidget * indexWidget ( const QModelIndex & index ) const
static QoreNode *QABSTRACTITEMVIEW_indexWidget(QoreObject *self, QoreAbstractQAbstractItemView *qaiv, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQModelIndex *index = (p && p->type == NT_OBJECT) ? (QoreQModelIndex *)p->val.object->getReferencedPrivateData(CID_QMODELINDEX, xsink) : 0;
   if (!index) {
      if (!xsink->isException())
         xsink->raiseException("QABSTRACTITEMVIEW-INDEXWIDGET-PARAM-ERROR", "expecting a QModelIndex object as first argument to QAbstractItemView::indexWidget()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> indexHolder(static_cast<AbstractPrivateData *>(index), xsink);
   QWidget *qt_qobj = qaiv->getQAbstractItemView()->indexWidget(*(static_cast<QModelIndex *>(index)));
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   QoreObject *rv_obj = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   assert(rv_obj);
   rv_obj->ref();
   return new QoreNode(rv_obj);
}

//QAbstractItemDelegate * itemDelegate () const
//QAbstractItemDelegate * itemDelegate ( const QModelIndex & index ) const
static QoreNode *QABSTRACTITEMVIEW_itemDelegate(QoreObject *self, QoreAbstractQAbstractItemView *qaiv, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      QAbstractItemDelegate *qt_qobj = qaiv->getQAbstractItemView()->itemDelegate();
      if (!qt_qobj)
         return 0;
      QVariant qv_ptr = qt_qobj->property("qobject");
      QoreObject *rv_obj = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
      assert(rv_obj);
      rv_obj->ref();
      return new QoreNode(rv_obj);
   }
   QoreQModelIndex *index = (p && p->type == NT_OBJECT) ? (QoreQModelIndex *)p->val.object->getReferencedPrivateData(CID_QMODELINDEX, xsink) : 0;
   if (!index) {
      if (!xsink->isException())
         xsink->raiseException("QABSTRACTITEMVIEW-ITEMDELEGATE-PARAM-ERROR", "this version of QAbstractItemView::itemDelegate() expects an object derived from QModelIndex as the first argument", p->val.object->getClass()->getName());
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> indexHolder(static_cast<AbstractPrivateData *>(index), xsink);
   QAbstractItemDelegate *qt_qobj = qaiv->getQAbstractItemView()->itemDelegate(*(static_cast<QModelIndex *>(index)));
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   QoreObject *rv_obj = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   assert(rv_obj);
   rv_obj->ref();
   return new QoreNode(rv_obj);
}

//QAbstractItemDelegate * itemDelegateForColumn ( int column ) const
static QoreNode *QABSTRACTITEMVIEW_itemDelegateForColumn(QoreObject *self, QoreAbstractQAbstractItemView *qaiv, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int column = p ? p->getAsInt() : 0;
   QAbstractItemDelegate *qt_qobj = qaiv->getQAbstractItemView()->itemDelegateForColumn(column);
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   QoreObject *rv_obj = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   assert(rv_obj);
   rv_obj->ref();
   return new QoreNode(rv_obj);
}

//QAbstractItemDelegate * itemDelegateForRow ( int row ) const
static QoreNode *QABSTRACTITEMVIEW_itemDelegateForRow(QoreObject *self, QoreAbstractQAbstractItemView *qaiv, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int row = p ? p->getAsInt() : 0;
   QAbstractItemDelegate *qt_qobj = qaiv->getQAbstractItemView()->itemDelegateForRow(row);
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   QoreObject *rv_obj = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   assert(rv_obj);
   rv_obj->ref();
   return new QoreNode(rv_obj);
}

//virtual void keyboardSearch ( const QString & search )
static QoreNode *QABSTRACTITEMVIEW_keyboardSearch(QoreObject *self, QoreAbstractQAbstractItemView *qaiv, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString search;
   if (get_qstring(p, search, xsink))
      return 0;
   qaiv->getQAbstractItemView()->keyboardSearch(search);
   return 0;
}

////QAbstractItemModel * model () const
//static QoreNode *QABSTRACTITEMVIEW_model(QoreObject *self, QoreAbstractQAbstractItemView *qaiv, const QoreList *params, ExceptionSink *xsink)
//{
//   ??? return new QoreNode((int64)qaiv->getQAbstractItemView()->model());
//}

//void openPersistentEditor ( const QModelIndex & index )
static QoreNode *QABSTRACTITEMVIEW_openPersistentEditor(QoreObject *self, QoreAbstractQAbstractItemView *qaiv, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQModelIndex *index = (p && p->type == NT_OBJECT) ? (QoreQModelIndex *)p->val.object->getReferencedPrivateData(CID_QMODELINDEX, xsink) : 0;
   if (!index) {
      if (!xsink->isException())
         xsink->raiseException("QABSTRACTITEMVIEW-OPENPERSISTENTEDITOR-PARAM-ERROR", "expecting a QModelIndex object as first argument to QAbstractItemView::openPersistentEditor()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> indexHolder(static_cast<AbstractPrivateData *>(index), xsink);
   qaiv->getQAbstractItemView()->openPersistentEditor(*(static_cast<QModelIndex *>(index)));
   return 0;
}

//QModelIndex rootIndex () const
static QoreNode *QABSTRACTITEMVIEW_rootIndex(QoreObject *self, QoreAbstractQAbstractItemView *qaiv, const QoreList *params, ExceptionSink *xsink)
{
   QoreObject *o_qmi = new QoreObject(QC_QModelIndex, getProgram());
   QoreQModelIndex *q_qmi = new QoreQModelIndex(qaiv->getQAbstractItemView()->rootIndex());
   o_qmi->setPrivate(CID_QMODELINDEX, q_qmi);
   return new QoreNode(o_qmi);
}

//virtual void scrollTo ( const QModelIndex & index, ScrollHint hint = EnsureVisible ) = 0
static QoreNode *QABSTRACTITEMVIEW_scrollTo(QoreObject *self, QoreAbstractQAbstractItemView *qaiv, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQModelIndex *index = (p && p->type == NT_OBJECT) ? (QoreQModelIndex *)p->val.object->getReferencedPrivateData(CID_QMODELINDEX, xsink) : 0;
   if (!index) {
      if (!xsink->isException())
         xsink->raiseException("QABSTRACTITEMVIEW-SCROLLTO-PARAM-ERROR", "expecting a QModelIndex object as first argument to QAbstractItemView::scrollTo()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> indexHolder(static_cast<AbstractPrivateData *>(index), xsink);
   p = get_param(params, 1);
   QAbstractItemView::ScrollHint hint = !is_nothing(p) ? (QAbstractItemView::ScrollHint)p->getAsInt() : QAbstractItemView::EnsureVisible;
   qaiv->getQAbstractItemView()->scrollTo(*(static_cast<QModelIndex *>(index)), hint);
   return 0;
}

//QAbstractItemView::SelectionBehavior selectionBehavior () const
static QoreNode *QABSTRACTITEMVIEW_selectionBehavior(QoreObject *self, QoreAbstractQAbstractItemView *qaiv, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qaiv->getQAbstractItemView()->selectionBehavior());
}

//QAbstractItemView::SelectionMode selectionMode () const
static QoreNode *QABSTRACTITEMVIEW_selectionMode(QoreObject *self, QoreAbstractQAbstractItemView *qaiv, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qaiv->getQAbstractItemView()->selectionMode());
}

////QItemSelectionModel * selectionModel () const
//static QoreNode *QABSTRACTITEMVIEW_selectionModel(QoreObject *self, QoreAbstractQAbstractItemView *qaiv, const QoreList *params, ExceptionSink *xsink)
//{
//   ??? return new QoreNode((int64)qaiv->getQAbstractItemView()->selectionModel());
//}

//void setAlternatingRowColors ( bool enable )
static QoreNode *QABSTRACTITEMVIEW_setAlternatingRowColors(QoreObject *self, QoreAbstractQAbstractItemView *qaiv, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool enable = p ? p->getAsBool() : false;
   qaiv->getQAbstractItemView()->setAlternatingRowColors(enable);
   return 0;
}

//void setAutoScroll ( bool enable )
static QoreNode *QABSTRACTITEMVIEW_setAutoScroll(QoreObject *self, QoreAbstractQAbstractItemView *qaiv, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool enable = p ? p->getAsBool() : false;
   qaiv->getQAbstractItemView()->setAutoScroll(enable);
   return 0;
}

//void setDragDropMode ( DragDropMode behavior )
static QoreNode *QABSTRACTITEMVIEW_setDragDropMode(QoreObject *self, QoreAbstractQAbstractItemView *qaiv, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QAbstractItemView::DragDropMode behavior = (QAbstractItemView::DragDropMode)(p ? p->getAsInt() : 0);
   qaiv->getQAbstractItemView()->setDragDropMode(behavior);
   return 0;
}

//void setDragDropOverwriteMode ( bool overwrite )
static QoreNode *QABSTRACTITEMVIEW_setDragDropOverwriteMode(QoreObject *self, QoreAbstractQAbstractItemView *qaiv, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool overwrite = p ? p->getAsBool() : false;
   qaiv->getQAbstractItemView()->setDragDropOverwriteMode(overwrite);
   return 0;
}

//void setDragEnabled ( bool enable )
static QoreNode *QABSTRACTITEMVIEW_setDragEnabled(QoreObject *self, QoreAbstractQAbstractItemView *qaiv, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool enable = p ? p->getAsBool() : false;
   qaiv->getQAbstractItemView()->setDragEnabled(enable);
   return 0;
}

//void setDropIndicatorShown ( bool enable )
static QoreNode *QABSTRACTITEMVIEW_setDropIndicatorShown(QoreObject *self, QoreAbstractQAbstractItemView *qaiv, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool enable = p ? p->getAsBool() : false;
   qaiv->getQAbstractItemView()->setDropIndicatorShown(enable);
   return 0;
}

//void setEditTriggers ( EditTriggers triggers )
static QoreNode *QABSTRACTITEMVIEW_setEditTriggers(QoreObject *self, QoreAbstractQAbstractItemView *qaiv, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QAbstractItemView::EditTriggers triggers = (QAbstractItemView::EditTriggers)(p ? p->getAsInt() : 0);
   qaiv->getQAbstractItemView()->setEditTriggers(triggers);
   return 0;
}

//void setHorizontalScrollMode ( ScrollMode mode )
static QoreNode *QABSTRACTITEMVIEW_setHorizontalScrollMode(QoreObject *self, QoreAbstractQAbstractItemView *qaiv, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QAbstractItemView::ScrollMode mode = (QAbstractItemView::ScrollMode)(p ? p->getAsInt() : 0);
   qaiv->getQAbstractItemView()->setHorizontalScrollMode(mode);
   return 0;
}

//void setIconSize ( const QSize & size )
static QoreNode *QABSTRACTITEMVIEW_setIconSize(QoreObject *self, QoreAbstractQAbstractItemView *qaiv, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQSize *size = (p && p->type == NT_OBJECT) ? (QoreQSize *)p->val.object->getReferencedPrivateData(CID_QSIZE, xsink) : 0;
   if (!size) {
      if (!xsink->isException())
         xsink->raiseException("QABSTRACTITEMVIEW-SETICONSIZE-PARAM-ERROR", "expecting a QSize object as first argument to QAbstractItemView::setIconSize()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> sizeHolder(static_cast<AbstractPrivateData *>(size), xsink);
   qaiv->getQAbstractItemView()->setIconSize(*(static_cast<QSize *>(size)));
   return 0;
}

//void setIndexWidget ( const QModelIndex & index, QWidget * widget )
static QoreNode *QABSTRACTITEMVIEW_setIndexWidget(QoreObject *self, QoreAbstractQAbstractItemView *qaiv, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQModelIndex *index = (p && p->type == NT_OBJECT) ? (QoreQModelIndex *)p->val.object->getReferencedPrivateData(CID_QMODELINDEX, xsink) : 0;
   if (!index) {
      if (!xsink->isException())
         xsink->raiseException("QABSTRACTITEMVIEW-SETINDEXWIDGET-PARAM-ERROR", "expecting a QModelIndex object as first argument to QAbstractItemView::setIndexWidget()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> indexHolder(static_cast<AbstractPrivateData *>(index), xsink);
   p = get_param(params, 1);
   QoreQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!widget) {
      if (!xsink->isException())
         xsink->raiseException("QABSTRACTITEMVIEW-SETINDEXWIDGET-PARAM-ERROR", "expecting a QWidget object as second argument to QAbstractItemView::setIndexWidget()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   qaiv->getQAbstractItemView()->setIndexWidget(*(static_cast<QModelIndex *>(index)), static_cast<QWidget *>(widget->getQWidget()));
   return 0;
}

//void setItemDelegate ( QAbstractItemDelegate * delegate )
static QoreNode *QABSTRACTITEMVIEW_setItemDelegate(QoreObject *self, QoreAbstractQAbstractItemView *qaiv, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreAbstractQAbstractItemDelegate *delegate = (p && p->type == NT_OBJECT) ? (QoreAbstractQAbstractItemDelegate *)p->val.object->getReferencedPrivateData(CID_QABSTRACTITEMDELEGATE, xsink) : 0;
   if (!delegate) {
      if (!xsink->isException())
         xsink->raiseException("QABSTRACTITEMVIEW-SETITEMDELEGATE-PARAM-ERROR", "expecting a QAbstractItemDelegate object as first argument to QAbstractItemView::setItemDelegate()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> delegateHolder(static_cast<AbstractPrivateData *>(delegate), xsink);
   qaiv->getQAbstractItemView()->setItemDelegate(static_cast<QAbstractItemDelegate *>(delegate->getQAbstractItemDelegate()));
   return 0;
}

//void setItemDelegateForColumn ( int column, QAbstractItemDelegate * delegate )
static QoreNode *QABSTRACTITEMVIEW_setItemDelegateForColumn(QoreObject *self, QoreAbstractQAbstractItemView *qaiv, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int column = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   QoreAbstractQAbstractItemDelegate *delegate = (p && p->type == NT_OBJECT) ? (QoreAbstractQAbstractItemDelegate *)p->val.object->getReferencedPrivateData(CID_QABSTRACTITEMDELEGATE, xsink) : 0;
   if (!delegate) {
      if (!xsink->isException())
         xsink->raiseException("QABSTRACTITEMVIEW-SETITEMDELEGATEFORCOLUMN-PARAM-ERROR", "expecting a QAbstractItemDelegate object as second argument to QAbstractItemView::setItemDelegateForColumn()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> delegateHolder(static_cast<AbstractPrivateData *>(delegate), xsink);
   qaiv->getQAbstractItemView()->setItemDelegateForColumn(column, static_cast<QAbstractItemDelegate *>(delegate->getQAbstractItemDelegate()));
   return 0;
}

//void setItemDelegateForRow ( int row, QAbstractItemDelegate * delegate )
static QoreNode *QABSTRACTITEMVIEW_setItemDelegateForRow(QoreObject *self, QoreAbstractQAbstractItemView *qaiv, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int row = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   QoreAbstractQAbstractItemDelegate *delegate = (p && p->type == NT_OBJECT) ? (QoreAbstractQAbstractItemDelegate *)p->val.object->getReferencedPrivateData(CID_QABSTRACTITEMDELEGATE, xsink) : 0;
   if (!delegate) {
      if (!xsink->isException())
         xsink->raiseException("QABSTRACTITEMVIEW-SETITEMDELEGATEFORROW-PARAM-ERROR", "expecting a QAbstractItemDelegate object as second argument to QAbstractItemView::setItemDelegateForRow()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> delegateHolder(static_cast<AbstractPrivateData *>(delegate), xsink);
   qaiv->getQAbstractItemView()->setItemDelegateForRow(row, static_cast<QAbstractItemDelegate *>(delegate->getQAbstractItemDelegate()));
   return 0;
}

////virtual void setModel ( QAbstractItemModel * model )
//static QoreNode *QABSTRACTITEMVIEW_setModel(QoreObject *self, QoreAbstractQAbstractItemView *qaiv, const QoreList *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QAbstractItemModel* model = p;
//   qaiv->getQAbstractItemView()->setModel(model);
//   return 0;
//}

//void setSelectionBehavior ( QAbstractItemView::SelectionBehavior behavior )
static QoreNode *QABSTRACTITEMVIEW_setSelectionBehavior(QoreObject *self, QoreAbstractQAbstractItemView *qaiv, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QAbstractItemView::SelectionBehavior behavior = (QAbstractItemView::SelectionBehavior)(p ? p->getAsInt() : 0);
   qaiv->getQAbstractItemView()->setSelectionBehavior(behavior);
   return 0;
}

//void setSelectionMode ( QAbstractItemView::SelectionMode mode )
static QoreNode *QABSTRACTITEMVIEW_setSelectionMode(QoreObject *self, QoreAbstractQAbstractItemView *qaiv, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QAbstractItemView::SelectionMode mode = (QAbstractItemView::SelectionMode)(p ? p->getAsInt() : 0);
   qaiv->getQAbstractItemView()->setSelectionMode(mode);
   return 0;
}

////virtual void setSelectionModel ( QItemSelectionModel * selectionModel )
//static QoreNode *QABSTRACTITEMVIEW_setSelectionModel(QoreObject *self, QoreAbstractQAbstractItemView *qaiv, const QoreList *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QItemSelectionModel* selectionModel = p;
//   qaiv->getQAbstractItemView()->setSelectionModel(selectionModel);
//   return 0;
//}

//void setTabKeyNavigation ( bool enable )
static QoreNode *QABSTRACTITEMVIEW_setTabKeyNavigation(QoreObject *self, QoreAbstractQAbstractItemView *qaiv, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool enable = p ? p->getAsBool() : false;
   qaiv->getQAbstractItemView()->setTabKeyNavigation(enable);
   return 0;
}

//void setTextElideMode ( Qt::TextElideMode mode )
static QoreNode *QABSTRACTITEMVIEW_setTextElideMode(QoreObject *self, QoreAbstractQAbstractItemView *qaiv, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   Qt::TextElideMode mode = (Qt::TextElideMode)(p ? p->getAsInt() : 0);
   qaiv->getQAbstractItemView()->setTextElideMode(mode);
   return 0;
}

//void setVerticalScrollMode ( ScrollMode mode )
static QoreNode *QABSTRACTITEMVIEW_setVerticalScrollMode(QoreObject *self, QoreAbstractQAbstractItemView *qaiv, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QAbstractItemView::ScrollMode mode = (QAbstractItemView::ScrollMode)(p ? p->getAsInt() : 0);
   qaiv->getQAbstractItemView()->setVerticalScrollMode(mode);
   return 0;
}

//bool showDropIndicator () const
static QoreNode *QABSTRACTITEMVIEW_showDropIndicator(QoreObject *self, QoreAbstractQAbstractItemView *qaiv, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode(qaiv->getQAbstractItemView()->showDropIndicator());
}

//virtual int sizeHintForColumn ( int column ) const
static QoreNode *QABSTRACTITEMVIEW_sizeHintForColumn(QoreObject *self, QoreAbstractQAbstractItemView *qaiv, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int column = p ? p->getAsInt() : 0;
   return new QoreNode((int64)qaiv->getQAbstractItemView()->sizeHintForColumn(column));
}

//QSize sizeHintForIndex ( const QModelIndex & index ) const
static QoreNode *QABSTRACTITEMVIEW_sizeHintForIndex(QoreObject *self, QoreAbstractQAbstractItemView *qaiv, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQModelIndex *index = (p && p->type == NT_OBJECT) ? (QoreQModelIndex *)p->val.object->getReferencedPrivateData(CID_QMODELINDEX, xsink) : 0;
   if (!index) {
      if (!xsink->isException())
         xsink->raiseException("QABSTRACTITEMVIEW-SIZEHINTFORINDEX-PARAM-ERROR", "expecting a QModelIndex object as first argument to QAbstractItemView::sizeHintForIndex()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> indexHolder(static_cast<AbstractPrivateData *>(index), xsink);
   QoreObject *o_qs = new QoreObject(QC_QSize, getProgram());
   QoreQSize *q_qs = new QoreQSize(qaiv->getQAbstractItemView()->sizeHintForIndex(*(static_cast<QModelIndex *>(index))));
   o_qs->setPrivate(CID_QSIZE, q_qs);
   return new QoreNode(o_qs);
}

//virtual int sizeHintForRow ( int row ) const
static QoreNode *QABSTRACTITEMVIEW_sizeHintForRow(QoreObject *self, QoreAbstractQAbstractItemView *qaiv, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int row = p ? p->getAsInt() : 0;
   return new QoreNode((int64)qaiv->getQAbstractItemView()->sizeHintForRow(row));
}

//bool tabKeyNavigation () const
static QoreNode *QABSTRACTITEMVIEW_tabKeyNavigation(QoreObject *self, QoreAbstractQAbstractItemView *qaiv, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode(qaiv->getQAbstractItemView()->tabKeyNavigation());
}

//Qt::TextElideMode textElideMode () const
static QoreNode *QABSTRACTITEMVIEW_textElideMode(QoreObject *self, QoreAbstractQAbstractItemView *qaiv, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qaiv->getQAbstractItemView()->textElideMode());
}

//ScrollMode verticalScrollMode () const
static QoreNode *QABSTRACTITEMVIEW_verticalScrollMode(QoreObject *self, QoreAbstractQAbstractItemView *qaiv, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qaiv->getQAbstractItemView()->verticalScrollMode());
}

//virtual QRect visualRect ( const QModelIndex & index ) const = 0
static QoreNode *QABSTRACTITEMVIEW_visualRect(QoreObject *self, QoreAbstractQAbstractItemView *qaiv, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQModelIndex *index = (p && p->type == NT_OBJECT) ? (QoreQModelIndex *)p->val.object->getReferencedPrivateData(CID_QMODELINDEX, xsink) : 0;
   if (!index) {
      if (!xsink->isException())
         xsink->raiseException("QABSTRACTITEMVIEW-VISUALRECT-PARAM-ERROR", "expecting a QModelIndex object as first argument to QAbstractItemView::visualRect()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> indexHolder(static_cast<AbstractPrivateData *>(index), xsink);
   QoreObject *o_qr = new QoreObject(QC_QRect, getProgram());
   QoreQRect *q_qr = new QoreQRect(qaiv->getQAbstractItemView()->visualRect(*(static_cast<QModelIndex *>(index))));
   o_qr->setPrivate(CID_QRECT, q_qr);
   return new QoreNode(o_qr);
}

QoreClass *initQAbstractItemViewClass(QoreClass *qabstractscrollarea)
{
   QC_QAbstractItemView = new QoreClass("QAbstractItemView", QDOM_GUI);
   CID_QABSTRACTITEMVIEW = QC_QAbstractItemView->getID();

   QC_QAbstractItemView->addBuiltinVirtualBaseClass(qabstractscrollarea);

   QC_QAbstractItemView->setConstructor(QABSTRACTITEMVIEW_constructor);
   QC_QAbstractItemView->setCopy((q_copy_t)QABSTRACTITEMVIEW_copy);

   QC_QAbstractItemView->addMethod("alternatingRowColors",        (q_method_t)QABSTRACTITEMVIEW_alternatingRowColors);
   QC_QAbstractItemView->addMethod("closePersistentEditor",       (q_method_t)QABSTRACTITEMVIEW_closePersistentEditor);
   QC_QAbstractItemView->addMethod("currentIndex",                (q_method_t)QABSTRACTITEMVIEW_currentIndex);
   QC_QAbstractItemView->addMethod("dragDropMode",                (q_method_t)QABSTRACTITEMVIEW_dragDropMode);
   QC_QAbstractItemView->addMethod("dragDropOverwriteMode",       (q_method_t)QABSTRACTITEMVIEW_dragDropOverwriteMode);
   QC_QAbstractItemView->addMethod("dragEnabled",                 (q_method_t)QABSTRACTITEMVIEW_dragEnabled);
   QC_QAbstractItemView->addMethod("editTriggers",                (q_method_t)QABSTRACTITEMVIEW_editTriggers);
   QC_QAbstractItemView->addMethod("hasAutoScroll",               (q_method_t)QABSTRACTITEMVIEW_hasAutoScroll);
   QC_QAbstractItemView->addMethod("horizontalScrollMode",        (q_method_t)QABSTRACTITEMVIEW_horizontalScrollMode);
   QC_QAbstractItemView->addMethod("iconSize",                    (q_method_t)QABSTRACTITEMVIEW_iconSize);
   QC_QAbstractItemView->addMethod("indexAt",                     (q_method_t)QABSTRACTITEMVIEW_indexAt);
   QC_QAbstractItemView->addMethod("indexWidget",                 (q_method_t)QABSTRACTITEMVIEW_indexWidget);
   QC_QAbstractItemView->addMethod("itemDelegate",                (q_method_t)QABSTRACTITEMVIEW_itemDelegate);
   QC_QAbstractItemView->addMethod("itemDelegateForColumn",       (q_method_t)QABSTRACTITEMVIEW_itemDelegateForColumn);
   QC_QAbstractItemView->addMethod("itemDelegateForRow",          (q_method_t)QABSTRACTITEMVIEW_itemDelegateForRow);
   QC_QAbstractItemView->addMethod("keyboardSearch",              (q_method_t)QABSTRACTITEMVIEW_keyboardSearch);
   //QC_QAbstractItemView->addMethod("model",                       (q_method_t)QABSTRACTITEMVIEW_model);
   QC_QAbstractItemView->addMethod("openPersistentEditor",        (q_method_t)QABSTRACTITEMVIEW_openPersistentEditor);
   QC_QAbstractItemView->addMethod("rootIndex",                   (q_method_t)QABSTRACTITEMVIEW_rootIndex);
   QC_QAbstractItemView->addMethod("scrollTo",                    (q_method_t)QABSTRACTITEMVIEW_scrollTo);
   QC_QAbstractItemView->addMethod("selectionBehavior",           (q_method_t)QABSTRACTITEMVIEW_selectionBehavior);
   QC_QAbstractItemView->addMethod("selectionMode",               (q_method_t)QABSTRACTITEMVIEW_selectionMode);
   //QC_QAbstractItemView->addMethod("selectionModel",              (q_method_t)QABSTRACTITEMVIEW_selectionModel);
   QC_QAbstractItemView->addMethod("setAlternatingRowColors",     (q_method_t)QABSTRACTITEMVIEW_setAlternatingRowColors);
   QC_QAbstractItemView->addMethod("setAutoScroll",               (q_method_t)QABSTRACTITEMVIEW_setAutoScroll);
   QC_QAbstractItemView->addMethod("setDragDropMode",             (q_method_t)QABSTRACTITEMVIEW_setDragDropMode);
   QC_QAbstractItemView->addMethod("setDragDropOverwriteMode",    (q_method_t)QABSTRACTITEMVIEW_setDragDropOverwriteMode);
   QC_QAbstractItemView->addMethod("setDragEnabled",              (q_method_t)QABSTRACTITEMVIEW_setDragEnabled);
   QC_QAbstractItemView->addMethod("setDropIndicatorShown",       (q_method_t)QABSTRACTITEMVIEW_setDropIndicatorShown);
   QC_QAbstractItemView->addMethod("setEditTriggers",             (q_method_t)QABSTRACTITEMVIEW_setEditTriggers);
   QC_QAbstractItemView->addMethod("setHorizontalScrollMode",     (q_method_t)QABSTRACTITEMVIEW_setHorizontalScrollMode);
   QC_QAbstractItemView->addMethod("setIconSize",                 (q_method_t)QABSTRACTITEMVIEW_setIconSize);
   QC_QAbstractItemView->addMethod("setIndexWidget",              (q_method_t)QABSTRACTITEMVIEW_setIndexWidget);
   QC_QAbstractItemView->addMethod("setItemDelegate",             (q_method_t)QABSTRACTITEMVIEW_setItemDelegate);
   QC_QAbstractItemView->addMethod("setItemDelegateForColumn",    (q_method_t)QABSTRACTITEMVIEW_setItemDelegateForColumn);
   QC_QAbstractItemView->addMethod("setItemDelegateForRow",       (q_method_t)QABSTRACTITEMVIEW_setItemDelegateForRow);
   //QC_QAbstractItemView->addMethod("setModel",                    (q_method_t)QABSTRACTITEMVIEW_setModel);
   QC_QAbstractItemView->addMethod("setSelectionBehavior",        (q_method_t)QABSTRACTITEMVIEW_setSelectionBehavior);
   QC_QAbstractItemView->addMethod("setSelectionMode",            (q_method_t)QABSTRACTITEMVIEW_setSelectionMode);
   //QC_QAbstractItemView->addMethod("setSelectionModel",           (q_method_t)QABSTRACTITEMVIEW_setSelectionModel);
   QC_QAbstractItemView->addMethod("setTabKeyNavigation",         (q_method_t)QABSTRACTITEMVIEW_setTabKeyNavigation);
   QC_QAbstractItemView->addMethod("setTextElideMode",            (q_method_t)QABSTRACTITEMVIEW_setTextElideMode);
   QC_QAbstractItemView->addMethod("setVerticalScrollMode",       (q_method_t)QABSTRACTITEMVIEW_setVerticalScrollMode);
   QC_QAbstractItemView->addMethod("showDropIndicator",           (q_method_t)QABSTRACTITEMVIEW_showDropIndicator);
   QC_QAbstractItemView->addMethod("sizeHintForColumn",           (q_method_t)QABSTRACTITEMVIEW_sizeHintForColumn);
   QC_QAbstractItemView->addMethod("sizeHintForIndex",            (q_method_t)QABSTRACTITEMVIEW_sizeHintForIndex);
   QC_QAbstractItemView->addMethod("sizeHintForRow",              (q_method_t)QABSTRACTITEMVIEW_sizeHintForRow);
   QC_QAbstractItemView->addMethod("tabKeyNavigation",            (q_method_t)QABSTRACTITEMVIEW_tabKeyNavigation);
   QC_QAbstractItemView->addMethod("textElideMode",               (q_method_t)QABSTRACTITEMVIEW_textElideMode);
   QC_QAbstractItemView->addMethod("verticalScrollMode",          (q_method_t)QABSTRACTITEMVIEW_verticalScrollMode);
   QC_QAbstractItemView->addMethod("visualRect",                  (q_method_t)QABSTRACTITEMVIEW_visualRect);

   return QC_QAbstractItemView;
}
