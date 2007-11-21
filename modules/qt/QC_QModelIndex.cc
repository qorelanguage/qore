/*
 QC_QModelIndex.cc
 
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

#include "QC_QModelIndex.h"

int CID_QMODELINDEX;
class QoreClass *QC_QModelIndex = 0;

//QModelIndex ()
//QModelIndex ( const QModelIndex & other )
static void QMODELINDEX_constructor(Object *self, QoreNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_QMODELINDEX, new QoreQModelIndex());
   return;
}

static void QMODELINDEX_copy(class Object *self, class Object *old, class QoreQModelIndex *qmi, ExceptionSink *xsink)
{
   self->setPrivate(CID_QMODELINDEX, new QoreQModelIndex(*qmi));
}

//QModelIndex child ( int row, int column ) const
static QoreNode *QMODELINDEX_child(Object *self, QoreQModelIndex *qmi, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int row = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int column = p ? p->getAsInt() : 0;
   Object *o_qmi = new Object(self->getClass(CID_QMODELINDEX), getProgram());
   QoreQModelIndex *q_qmi = new QoreQModelIndex(qmi->child(row, column));
   o_qmi->setPrivate(CID_QMODELINDEX, q_qmi);
   return new QoreNode(o_qmi);
}

//int column () const
static QoreNode *QMODELINDEX_column(Object *self, QoreQModelIndex *qmi, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qmi->column());
}

//QVariant data ( int role = Qt::DisplayRole ) const
static QoreNode *QMODELINDEX_data(Object *self, QoreQModelIndex *qmi, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int role = !is_nothing(p) ? p->getAsInt() : Qt::DisplayRole;
   return return_qvariant(qmi->data(role));
}

//Qt::ItemFlags flags () const
static QoreNode *QMODELINDEX_flags(Object *self, QoreQModelIndex *qmi, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qmi->flags());
}

//qint64 internalId () const
static QoreNode *QMODELINDEX_internalId(Object *self, QoreQModelIndex *qmi, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qmi->internalId());
}

//void * internalPointer () const
//static QoreNode *QMODELINDEX_internalPointer(Object *self, QoreQModelIndex *qmi, QoreNode *params, ExceptionSink *xsink)
//{
//   qmi->internalPointer();
//   return 0;
//}

//bool isValid () const
static QoreNode *QMODELINDEX_isValid(Object *self, QoreQModelIndex *qmi, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qmi->isValid());
}

//const QAbstractItemModel * model () const
static QoreNode *QMODELINDEX_model(Object *self, QoreQModelIndex *qmi, QoreNode *params, ExceptionSink *xsink)
{
   const QAbstractItemModel *qt_qobj = qmi->model();
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   Object *rv_obj = reinterpret_cast<Object *>(qv_ptr.toULongLong());
   if (rv_obj)
      rv_obj->ref();
   else {
      rv_obj = new Object(QC_QAbstractItemModel, getProgram());
      QoreQtQAbstractItemModel *aim = new QoreQtQAbstractItemModel(rv_obj, const_cast<QAbstractItemModel *>(qt_qobj));
      rv_obj->setPrivate(CID_QABSTRACTITEMMODEL, aim);
   }
   return new QoreNode(rv_obj);
}

//QModelIndex parent () const
static QoreNode *QMODELINDEX_parent(Object *self, QoreQModelIndex *qmi, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qmi = new Object(self->getClass(CID_QMODELINDEX), getProgram());
   QoreQModelIndex *q_qmi = new QoreQModelIndex(qmi->parent());
   o_qmi->setPrivate(CID_QMODELINDEX, q_qmi);
   return new QoreNode(o_qmi);
}

//int row () const
static QoreNode *QMODELINDEX_row(Object *self, QoreQModelIndex *qmi, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qmi->row());
}

//QModelIndex sibling ( int row, int column ) const
static QoreNode *QMODELINDEX_sibling(Object *self, QoreQModelIndex *qmi, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int row = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int column = p ? p->getAsInt() : 0;
   Object *o_qmi = new Object(self->getClass(CID_QMODELINDEX), getProgram());
   QoreQModelIndex *q_qmi = new QoreQModelIndex(qmi->sibling(row, column));
   o_qmi->setPrivate(CID_QMODELINDEX, q_qmi);
   return new QoreNode(o_qmi);
}

QoreClass *initQModelIndexClass()
{
   QC_QModelIndex = new QoreClass("QModelIndex", QDOM_GUI);
   CID_QMODELINDEX = QC_QModelIndex->getID();

   QC_QModelIndex->setConstructor(QMODELINDEX_constructor);
   QC_QModelIndex->setCopy((q_copy_t)QMODELINDEX_copy);

   QC_QModelIndex->addMethod("child",                       (q_method_t)QMODELINDEX_child);
   QC_QModelIndex->addMethod("column",                      (q_method_t)QMODELINDEX_column);
   QC_QModelIndex->addMethod("data",                        (q_method_t)QMODELINDEX_data);
   QC_QModelIndex->addMethod("flags",                       (q_method_t)QMODELINDEX_flags);
   QC_QModelIndex->addMethod("internalId",                  (q_method_t)QMODELINDEX_internalId);
   //QC_QModelIndex->addMethod("internalPointer",             (q_method_t)QMODELINDEX_internalPointer);
   QC_QModelIndex->addMethod("isValid",                     (q_method_t)QMODELINDEX_isValid);
   QC_QModelIndex->addMethod("model",                       (q_method_t)QMODELINDEX_model);
   QC_QModelIndex->addMethod("parent",                      (q_method_t)QMODELINDEX_parent);
   QC_QModelIndex->addMethod("row",                         (q_method_t)QMODELINDEX_row);
   QC_QModelIndex->addMethod("sibling",                     (q_method_t)QMODELINDEX_sibling);

   return QC_QModelIndex;
}
