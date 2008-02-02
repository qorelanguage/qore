/*
 QC_QItemDelegate.cc
 
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

#include "QC_QItemDelegate.h"
#include "QC_QObject.h"
#include "QC_QWidget.h"
#include "QC_QStyleOptionViewItem.h"
#include "QC_QModelIndex.h"
#include "QC_QAbstractItemModel.h"
#include "QC_QSize.h"

#include "qore-qt.h"

int CID_QITEMDELEGATE;
class QoreClass *QC_QItemDelegate = 0;

//QItemDelegate ( QObject * parent = 0 )
static void QITEMDELEGATE_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   QoreAbstractQObject *parent = (p && p->type == NT_OBJECT) ? (QoreAbstractQObject *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QOBJECT, xsink) : 0;
   if (!is_nothing(p) && !parent) {
      if (!xsink->isException())
         xsink->raiseException("QITEMDELEGATE-QITEMDELEGATE-PARAM-ERROR", "expecting either NOTHING or a QObject as first argument to QItemDelegate::constructor()");
      return;
   }
   ReferenceHolder<QoreAbstractQObject> parentHolder(parent, xsink);
   self->setPrivate(CID_QITEMDELEGATE, new QoreQItemDelegate(self, parent->getQObject()));
   return;
}

static void QITEMDELEGATE_copy(class QoreObject *self, class QoreObject *old, class QoreQItemDelegate *qid, ExceptionSink *xsink)
{
   xsink->raiseException("QITEMDELEGATE-COPY-ERROR", "objects of this class cannot be copied");
}

//virtual QWidget * createEditor ( QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index ) const
static AbstractQoreNode *QITEMDELEGATE_createEditor(QoreObject *self, QoreAbstractQItemDelegate *qid, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   QoreAbstractQWidget *parent = (p && p->type == NT_OBJECT) ? (QoreAbstractQWidget *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!parent) {
      if (!xsink->isException())
         xsink->raiseException("QITEMDELEGATE-CREATEEDITOR-PARAM-ERROR", "expecting a QWidget object as first argument to QItemDelegate::createEditor()");
      return 0;
   }
   ReferenceHolder<QoreAbstractQWidget> parentHolder(parent, xsink);
   p = get_param(params, 1);
   QoreQStyleOptionViewItem *option = (p && p->type == NT_OBJECT) ? (QoreQStyleOptionViewItem *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QSTYLEOPTIONVIEWITEM, xsink) : 0;
   if (!option) {
      if (!xsink->isException())
         xsink->raiseException("QITEMDELEGATE-CREATEEDITOR-PARAM-ERROR", "expecting a QStyleOptionViewItem object as second argument to QItemDelegate::createEditor()");
      return 0;
   }
   ReferenceHolder<QoreQStyleOptionViewItem> optionHolder(option, xsink);
   p = get_param(params, 2);
   QoreQModelIndex *index = (p && p->type == NT_OBJECT) ? (QoreQModelIndex *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QMODELINDEX, xsink) : 0;
   if (!index) {
      if (!xsink->isException())
         xsink->raiseException("QITEMDELEGATE-CREATEEDITOR-PARAM-ERROR", "expecting a QModelIndex object as third argument to QItemDelegate::createEditor()");
      return 0;
   }
   ReferenceHolder<QoreQModelIndex> indexHolder(index, xsink);
   QObject *qt_qobj = qid->getQItemDelegate()->createEditor(parent->getQWidget(), *(static_cast<QStyleOptionViewItem *>(option)), *(static_cast<QModelIndex *>(index)));
   QVariant qv_ptr = qt_qobj->property("qobject");
   QoreObject *rv_obj = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   assert(rv_obj);
   rv_obj->ref();
   return rv_obj;
}

//bool hasClipping () const
static AbstractQoreNode *QITEMDELEGATE_hasClipping(QoreObject *self, QoreAbstractQItemDelegate *qid, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qid->getQItemDelegate()->hasClipping());
}

////QItemEditorFactory * itemEditorFactory () const
//static AbstractQoreNode *QITEMDELEGATE_itemEditorFactory(QoreObject *self, QoreAbstractQItemDelegate *qid, const QoreListNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreBigIntNode(qid->getQItemDelegate()->itemEditorFactory());
//}

//virtual void paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
static AbstractQoreNode *QITEMDELEGATE_paint(QoreObject *self, QoreAbstractQItemDelegate *qid, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   QoreQPainter *painter = (p && p->type == NT_OBJECT) ? (QoreQPainter *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QPAINTER, xsink) : 0;
   if (!painter) {
      if (!xsink->isException())
         xsink->raiseException("QITEMDELEGATE-PAINT-PARAM-ERROR", "expecting a QPainter object as first argument to QItemDelegate::paint()");
      return 0;
   }
   ReferenceHolder<QoreQPainter> painterHolder(painter, xsink);
   p = get_param(params, 1);
   QoreQStyleOptionViewItem *option = (p && p->type == NT_OBJECT) ? (QoreQStyleOptionViewItem *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QSTYLEOPTIONVIEWITEM, xsink) : 0;
   if (!option) {
      if (!xsink->isException())
         xsink->raiseException("QITEMDELEGATE-PAINT-PARAM-ERROR", "expecting a QStyleOptionViewItem object as second argument to QItemDelegate::paint()");
      return 0;
   }
   ReferenceHolder<QoreQStyleOptionViewItem> optionHolder(option, xsink);
   p = get_param(params, 2);
   QoreQModelIndex *index = (p && p->type == NT_OBJECT) ? (QoreQModelIndex *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QMODELINDEX, xsink) : 0;
   if (!index) {
      if (!xsink->isException())
         xsink->raiseException("QITEMDELEGATE-PAINT-PARAM-ERROR", "expecting a QModelIndex object as third argument to QItemDelegate::paint()");
      return 0;
   }
   ReferenceHolder<QoreQModelIndex> indexHolder(index, xsink);
   qid->getQItemDelegate()->paint(painter->getQPainter(), *(static_cast<QStyleOptionViewItem *>(option)), *(static_cast<QModelIndex *>(index)));
   return 0;
}

//void setClipping ( bool clip )
static AbstractQoreNode *QITEMDELEGATE_setClipping(QoreObject *self, QoreAbstractQItemDelegate *qid, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   bool clip = p ? p->getAsBool() : false;
   qid->getQItemDelegate()->setClipping(clip);
   return 0;
}

//virtual void setEditorData ( QWidget * editor, const QModelIndex & index ) const
static AbstractQoreNode *QITEMDELEGATE_setEditorData(QoreObject *self, QoreAbstractQItemDelegate *qid, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   QoreAbstractQWidget *editor = (p && p->type == NT_OBJECT) ? (QoreAbstractQWidget *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!editor) {
      if (!xsink->isException())
         xsink->raiseException("QITEMDELEGATE-SETEDITORDATA-PARAM-ERROR", "expecting a QWidget object as first argument to QItemDelegate::setEditorData()");
      return 0;
   }
   ReferenceHolder<QoreAbstractQWidget> editorHolder(editor, xsink);
   p = get_param(params, 1);
   QoreQModelIndex *index = (p && p->type == NT_OBJECT) ? (QoreQModelIndex *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QMODELINDEX, xsink) : 0;
   if (!index) {
      if (!xsink->isException())
         xsink->raiseException("QITEMDELEGATE-SETEDITORDATA-PARAM-ERROR", "expecting a QModelIndex object as second argument to QItemDelegate::setEditorData()");
      return 0;
   }
   ReferenceHolder<QoreQModelIndex> indexHolder(index, xsink);
   qid->getQItemDelegate()->setEditorData(editor->getQWidget(), *(static_cast<QModelIndex *>(index)));
   return 0;
}

////void setItemEditorFactory ( QItemEditorFactory * factory )
//static AbstractQoreNode *QITEMDELEGATE_setItemEditorFactory(QoreObject *self, QoreAbstractQItemDelegate *qid, const QoreListNode *params, ExceptionSink *xsink)
//{
//   AbstractQoreNode *p = get_param(params, 0);
//   ??? QItemEditorFactory* factory = p;
//   qid->getQItemDelegate()->setItemEditorFactory(factory);
//   return 0;
//}

//virtual void setModelData ( QWidget * editor, QAbstractItemModel * model, const QModelIndex & index ) const
static AbstractQoreNode *QITEMDELEGATE_setModelData(QoreObject *self, QoreAbstractQItemDelegate *qid, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   QoreQWidget *editor = (p && p->type == NT_OBJECT) ? (QoreQWidget *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!editor) {
      if (!xsink->isException())
         xsink->raiseException("QITEMDELEGATE-SETMODELDATA-PARAM-ERROR", "expecting a QWidget object as first argument to QItemDelegate::setModelData()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> editorHolder(static_cast<AbstractPrivateData *>(editor), xsink);
   p = get_param(params, 1);
   QoreAbstractQAbstractItemModel *model = (p && p->type == NT_OBJECT) ? (QoreAbstractQAbstractItemModel *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QABSTRACTITEMMODEL, xsink) : 0;
   if (!model) {
      if (!xsink->isException())
         xsink->raiseException("QITEMDELEGATE-SETMODELDATA-PARAM-ERROR", "expecting a QAbstractItemModel object as second argument to QItemDelegate::setModelData()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> modelHolder(static_cast<AbstractPrivateData *>(model), xsink);
   p = get_param(params, 2);
   QoreQModelIndex *index = (p && p->type == NT_OBJECT) ? (QoreQModelIndex *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QMODELINDEX, xsink) : 0;
   if (!index) {
      if (!xsink->isException())
         xsink->raiseException("QITEMDELEGATE-SETMODELDATA-PARAM-ERROR", "expecting a QModelIndex object as third argument to QItemDelegate::setModelData()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> indexHolder(static_cast<AbstractPrivateData *>(index), xsink);
   qid->getQItemDelegate()->setModelData(static_cast<QWidget *>(editor->getQWidget()), model->getQAbstractItemModel(), *(static_cast<QModelIndex *>(index)));
   return 0;
}

//virtual QSize sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const
static AbstractQoreNode *QITEMDELEGATE_sizeHint(QoreObject *self, QoreAbstractQItemDelegate *qid, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   QoreQStyleOptionViewItem *option = (p && p->type == NT_OBJECT) ? (QoreQStyleOptionViewItem *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QSTYLEOPTIONVIEWITEM, xsink) : 0;
   if (!option) {
      if (!xsink->isException())
         xsink->raiseException("QITEMDELEGATE-SIZEHINT-PARAM-ERROR", "expecting a QStyleOptionViewItem object as first argument to QItemDelegate::sizeHint()");
      return 0;
   }
   ReferenceHolder<QoreQStyleOptionViewItem> optionHolder(option, xsink);
   p = get_param(params, 1);
   QoreQModelIndex *index = (p && p->type == NT_OBJECT) ? (QoreQModelIndex *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QMODELINDEX, xsink) : 0;
   if (!index) {
      if (!xsink->isException())
         xsink->raiseException("QITEMDELEGATE-SIZEHINT-PARAM-ERROR", "expecting a QModelIndex object as second argument to QItemDelegate::sizeHint()");
      return 0;
   }
   ReferenceHolder<QoreQModelIndex> indexHolder(index, xsink);
   QoreObject *o_qs = new QoreObject(QC_QSize, getProgram());
   QoreQSize *q_qs = new QoreQSize(qid->getQItemDelegate()->sizeHint(*(static_cast<QStyleOptionViewItem *>(option)), *(static_cast<QModelIndex *>(index))));
   o_qs->setPrivate(CID_QSIZE, q_qs);
   return o_qs;
}

//virtual void updateEditorGeometry ( QWidget * editor, const QStyleOptionViewItem & option, const QModelIndex & index ) const
static AbstractQoreNode *QITEMDELEGATE_updateEditorGeometry(QoreObject *self, QoreAbstractQItemDelegate *qid, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   QoreAbstractQWidget *editor = (p && p->type == NT_OBJECT) ? (QoreAbstractQWidget *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!editor) {
      if (!xsink->isException())
         xsink->raiseException("QITEMDELEGATE-UPDATEEDITORGEOMETRY-PARAM-ERROR", "expecting a QWidget object as first argument to QItemDelegate::updateEditorGeometry()");
      return 0;
   }
   ReferenceHolder<QoreAbstractQWidget> editorHolder(editor, xsink);
   p = get_param(params, 1);
   QoreQStyleOptionViewItem *option = (p && p->type == NT_OBJECT) ? (QoreQStyleOptionViewItem *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QSTYLEOPTIONVIEWITEM, xsink) : 0;
   if (!option) {
      if (!xsink->isException())
         xsink->raiseException("QITEMDELEGATE-UPDATEEDITORGEOMETRY-PARAM-ERROR", "expecting a QStyleOptionViewItem object as second argument to QItemDelegate::updateEditorGeometry()");
      return 0;
   }
   ReferenceHolder<QoreQStyleOptionViewItem> optionHolder(option, xsink);
   p = get_param(params, 2);
   QoreQModelIndex *index = (p && p->type == NT_OBJECT) ? (QoreQModelIndex *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QMODELINDEX, xsink) : 0;
   if (!index) {
      if (!xsink->isException())
         xsink->raiseException("QITEMDELEGATE-UPDATEEDITORGEOMETRY-PARAM-ERROR", "expecting a QModelIndex object as third argument to QItemDelegate::updateEditorGeometry()");
      return 0;
   }
   ReferenceHolder<QoreQModelIndex> indexHolder(index, xsink);
   qid->getQItemDelegate()->updateEditorGeometry(editor->getQWidget(), *(static_cast<QStyleOptionViewItem *>(option)), *(static_cast<QModelIndex *>(index)));
   return 0;
}

QoreClass *initQItemDelegateClass(QoreClass *qabstractitemdelegate)
{
   QC_QItemDelegate = new QoreClass("QItemDelegate", QDOM_GUI);
   CID_QITEMDELEGATE = QC_QItemDelegate->getID();

   QC_QItemDelegate->addBuiltinVirtualBaseClass(qabstractitemdelegate);

   QC_QItemDelegate->setConstructor(QITEMDELEGATE_constructor);
   QC_QItemDelegate->setCopy((q_copy_t)QITEMDELEGATE_copy);

   QC_QItemDelegate->addMethod("createEditor",                (q_method_t)QITEMDELEGATE_createEditor);
   QC_QItemDelegate->addMethod("hasClipping",                 (q_method_t)QITEMDELEGATE_hasClipping);
   //QC_QItemDelegate->addMethod("itemEditorFactory",           (q_method_t)QITEMDELEGATE_itemEditorFactory);
   QC_QItemDelegate->addMethod("paint",                       (q_method_t)QITEMDELEGATE_paint);
   QC_QItemDelegate->addMethod("setClipping",                 (q_method_t)QITEMDELEGATE_setClipping);
   QC_QItemDelegate->addMethod("setEditorData",               (q_method_t)QITEMDELEGATE_setEditorData);
   //QC_QItemDelegate->addMethod("setItemEditorFactory",        (q_method_t)QITEMDELEGATE_setItemEditorFactory);
   QC_QItemDelegate->addMethod("setModelData",                (q_method_t)QITEMDELEGATE_setModelData);
   QC_QItemDelegate->addMethod("sizeHint",                    (q_method_t)QITEMDELEGATE_sizeHint);
   QC_QItemDelegate->addMethod("updateEditorGeometry",        (q_method_t)QITEMDELEGATE_updateEditorGeometry);

   return QC_QItemDelegate;
}
