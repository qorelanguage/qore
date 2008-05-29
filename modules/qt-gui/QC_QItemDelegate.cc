/*
 QC_QItemDelegate.cc
 
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

#include "QC_QItemDelegate.h"
#include "QC_QObject.h"
#include "QC_QWidget.h"
#include "QC_QStyleOptionViewItem.h"
#include "QC_QModelIndex.h"
#include "QC_QAbstractItemModel.h"
#include "QC_QSize.h"

#include "qore-qt.h"

qore_classid_t CID_QITEMDELEGATE;
class QoreClass *QC_QItemDelegate = 0;

//QItemDelegate ( QObject * parent = 0 )
static void QITEMDELEGATE_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   const QoreObject *o = dynamic_cast<const QoreObject *>(p);
   QoreAbstractQObject *parent = o ? (QoreAbstractQObject *)o->getReferencedPrivateData(CID_QOBJECT, xsink) : 0;
   if (*xsink)
      return;
   if (!is_nothing(p) && !parent) {
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
   const QoreObject *o = test_object_param(params, 0);
   QoreAbstractQWidget *parent = o ? (QoreAbstractQWidget *)o->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!parent) {
      if (!xsink->isException())
         xsink->raiseException("QITEMDELEGATE-CREATEEDITOR-PARAM-ERROR", "expecting a QWidget object as first argument to QItemDelegate::createEditor()");
      return 0;
   }
   ReferenceHolder<QoreAbstractQWidget> parentHolder(parent, xsink);

   o = test_object_param(params, 1);
   QoreQStyleOptionViewItem *option = o ? (QoreQStyleOptionViewItem *)o->getReferencedPrivateData(CID_QSTYLEOPTIONVIEWITEM, xsink) : 0;
   if (!option) {
      if (!xsink->isException())
         xsink->raiseException("QITEMDELEGATE-CREATEEDITOR-PARAM-ERROR", "expecting a QStyleOptionViewItem object as second argument to QItemDelegate::createEditor()");
      return 0;
   }
   ReferenceHolder<QoreQStyleOptionViewItem> optionHolder(option, xsink);
   o = test_object_param(params, 2);
   QoreQModelIndex *index = o ? (QoreQModelIndex *)o->getReferencedPrivateData(CID_QMODELINDEX, xsink) : 0;
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
   return get_bool_node(qid->getQItemDelegate()->hasClipping());
}

////QItemEditorFactory * itemEditorFactory () const
//static AbstractQoreNode *QITEMDELEGATE_itemEditorFactory(QoreObject *self, QoreAbstractQItemDelegate *qid, const QoreListNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreBigIntNode(qid->getQItemDelegate()->itemEditorFactory());
//}

//virtual void paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
static AbstractQoreNode *QITEMDELEGATE_paint(QoreObject *self, QoreAbstractQItemDelegate *qid, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreObject *o = test_object_param(params, 0);
   QoreQPainter *painter = o ? (QoreQPainter *)o->getReferencedPrivateData(CID_QPAINTER, xsink) : 0;
   if (!painter) {
      if (!xsink->isException())
         xsink->raiseException("QITEMDELEGATE-PAINT-PARAM-ERROR", "expecting a QPainter object as first argument to QItemDelegate::paint()");
      return 0;
   }
   ReferenceHolder<QoreQPainter> painterHolder(painter, xsink);
   o = test_object_param(params, 1);
   QoreQStyleOptionViewItem *option = o ? (QoreQStyleOptionViewItem *)o->getReferencedPrivateData(CID_QSTYLEOPTIONVIEWITEM, xsink) : 0;
   if (!option) {
      if (!xsink->isException())
         xsink->raiseException("QITEMDELEGATE-PAINT-PARAM-ERROR", "expecting a QStyleOptionViewItem object as second argument to QItemDelegate::paint()");
      return 0;
   }
   ReferenceHolder<QoreQStyleOptionViewItem> optionHolder(option, xsink);
   o = test_object_param(params, 2);
   QoreQModelIndex *index = o ? (QoreQModelIndex *)o->getReferencedPrivateData(CID_QMODELINDEX, xsink) : 0;
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
   const AbstractQoreNode *p = get_param(params, 0);
   bool clip = p ? p->getAsBool() : false;
   qid->getQItemDelegate()->setClipping(clip);
   return 0;
}

//virtual void setEditorData ( QWidget * editor, const QModelIndex & index ) const
static AbstractQoreNode *QITEMDELEGATE_setEditorData(QoreObject *self, QoreAbstractQItemDelegate *qid, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreObject *o = test_object_param(params, 0);
   QoreAbstractQWidget *editor = o ? (QoreAbstractQWidget *)o->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!editor) {
      if (!xsink->isException())
         xsink->raiseException("QITEMDELEGATE-SETEDITORDATA-PARAM-ERROR", "expecting a QWidget object as first argument to QItemDelegate::setEditorData()");
      return 0;
   }
   ReferenceHolder<QoreAbstractQWidget> editorHolder(editor, xsink);
   o = test_object_param(params, 1);
   QoreQModelIndex *index = o ? (QoreQModelIndex *)o->getReferencedPrivateData(CID_QMODELINDEX, xsink) : 0;
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
//   const AbstractQoreNode *o = test_object_param(params, 0);
//   ??? QItemEditorFactory* factory = p;
//   qid->getQItemDelegate()->setItemEditorFactory(factory);
//   return 0;
//}

//virtual void setModelData ( QWidget * editor, QAbstractItemModel * model, const QModelIndex & index ) const
static AbstractQoreNode *QITEMDELEGATE_setModelData(QoreObject *self, QoreAbstractQItemDelegate *qid, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreObject *o = test_object_param(params, 0);
   QoreQWidget *editor = o ? (QoreQWidget *)o->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!editor) {
      if (!xsink->isException())
         xsink->raiseException("QITEMDELEGATE-SETMODELDATA-PARAM-ERROR", "expecting a QWidget object as first argument to QItemDelegate::setModelData()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> editorHolder(static_cast<AbstractPrivateData *>(editor), xsink);
   o = test_object_param(params, 1);
   QoreAbstractQAbstractItemModel *model = o ? (QoreAbstractQAbstractItemModel *)o->getReferencedPrivateData(CID_QABSTRACTITEMMODEL, xsink) : 0;
   if (!model) {
      if (!xsink->isException())
         xsink->raiseException("QITEMDELEGATE-SETMODELDATA-PARAM-ERROR", "expecting a QAbstractItemModel object as second argument to QItemDelegate::setModelData()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> modelHolder(static_cast<AbstractPrivateData *>(model), xsink);
   o = test_object_param(params, 2);
   QoreQModelIndex *index = o ? (QoreQModelIndex *)o->getReferencedPrivateData(CID_QMODELINDEX, xsink) : 0;
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
   const QoreObject *o = test_object_param(params, 0);
   QoreQStyleOptionViewItem *option = o ? (QoreQStyleOptionViewItem *)o->getReferencedPrivateData(CID_QSTYLEOPTIONVIEWITEM, xsink) : 0;
   if (!option) {
      if (!xsink->isException())
         xsink->raiseException("QITEMDELEGATE-SIZEHINT-PARAM-ERROR", "expecting a QStyleOptionViewItem object as first argument to QItemDelegate::sizeHint()");
      return 0;
   }
   ReferenceHolder<QoreQStyleOptionViewItem> optionHolder(option, xsink);
   o = test_object_param(params, 1);
   QoreQModelIndex *index = o ? (QoreQModelIndex *)o->getReferencedPrivateData(CID_QMODELINDEX, xsink) : 0;
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
   const QoreObject *o = test_object_param(params, 0);
   QoreAbstractQWidget *editor = o ? (QoreAbstractQWidget *)o->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!editor) {
      if (!xsink->isException())
         xsink->raiseException("QITEMDELEGATE-UPDATEEDITORGEOMETRY-PARAM-ERROR", "expecting a QWidget object as first argument to QItemDelegate::updateEditorGeometry()");
      return 0;
   }
   ReferenceHolder<QoreAbstractQWidget> editorHolder(editor, xsink);
   o = test_object_param(params, 1);
   QoreQStyleOptionViewItem *option = o ? (QoreQStyleOptionViewItem *)o->getReferencedPrivateData(CID_QSTYLEOPTIONVIEWITEM, xsink) : 0;
   if (!option) {
      if (!xsink->isException())
         xsink->raiseException("QITEMDELEGATE-UPDATEEDITORGEOMETRY-PARAM-ERROR", "expecting a QStyleOptionViewItem object as second argument to QItemDelegate::updateEditorGeometry()");
      return 0;
   }
   ReferenceHolder<QoreQStyleOptionViewItem> optionHolder(option, xsink);
   o = test_object_param(params, 2);
   QoreQModelIndex *index = o ? (QoreQModelIndex *)o->getReferencedPrivateData(CID_QMODELINDEX, xsink) : 0;
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
