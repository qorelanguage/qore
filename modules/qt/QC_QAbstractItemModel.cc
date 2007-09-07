/*
 QC_QAbstractItemModel.cc
 
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

#include "QC_QAbstractItemModel.h"

int CID_QABSTRACTITEMMODEL;
class QoreClass *QC_QAbstractItemModel = 0;

//QAbstractItemModel ( QObject * parent = 0 )
static void QABSTRACTITEMMODEL_constructor(Object *self, QoreNode *params, ExceptionSink *xsink)
{
   xsink->raiseException("QABSTRACTITEMMODEL-CONSTRUCTOR-ERROR", "QAbstractItemModel is an abstract class");
   return;
}

static void QABSTRACTITEMMODEL_copy(class Object *self, class Object *old, class QoreQAbstractItemModel *qaim, ExceptionSink *xsink)
{
   xsink->raiseException("QABSTRACTITEMMODEL-COPY-ERROR", "objects of this class cannot be copied");
}

//virtual QModelIndex buddy ( const QModelIndex & index ) const
static QoreNode *QABSTRACTITEMMODEL_buddy(Object *self, QoreAbstractQAbstractItemModel *qaim, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQModelIndex *index = (p && p->type == NT_OBJECT) ? (QoreQModelIndex *)p->val.object->getReferencedPrivateData(CID_QMODELINDEX, xsink) : 0;
   if (!index) {
      if (!xsink->isException())
         xsink->raiseException("QABSTRACTITEMMODEL-BUDDY-PARAM-ERROR", "expecting a QModelIndex object as first argument to QAbstractItemModel::buddy()");
      return 0;
   }
   ReferenceHolder<QoreQModelIndex> indexHolder(index, xsink);
   Object *o_qmi = new Object(QC_QModelIndex, getProgram());
   QoreQModelIndex *q_qmi = new QoreQModelIndex(qaim->getQAbstractItemModel()->buddy(*(static_cast<QModelIndex *>(index))));
   o_qmi->setPrivate(CID_QMODELINDEX, q_qmi);
   return new QoreNode(o_qmi);
}

//virtual bool canFetchMore ( const QModelIndex & parent ) const
static QoreNode *QABSTRACTITEMMODEL_canFetchMore(Object *self, QoreAbstractQAbstractItemModel *qaim, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQModelIndex *parent = (p && p->type == NT_OBJECT) ? (QoreQModelIndex *)p->val.object->getReferencedPrivateData(CID_QMODELINDEX, xsink) : 0;
   if (!parent) {
      if (!xsink->isException())
         xsink->raiseException("QABSTRACTITEMMODEL-CANFETCHMORE-PARAM-ERROR", "expecting a QModelIndex object as first argument to QAbstractItemModel::canFetchMore()");
      return 0;
   }
   ReferenceHolder<QoreQModelIndex> parentHolder(parent, xsink);
   return new QoreNode(qaim->getQAbstractItemModel()->canFetchMore(*(static_cast<QModelIndex *>(parent))));
}

//virtual int columnCount ( const QModelIndex & parent = QModelIndex() ) const = 0
static QoreNode *QABSTRACTITEMMODEL_columnCount(Object *self, QoreAbstractQAbstractItemModel *qaim, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQModelIndex *parent = (p && p->type == NT_OBJECT) ? (QoreQModelIndex *)p->val.object->getReferencedPrivateData(CID_QMODELINDEX, xsink) : 0;
   if (!parent) {
      if (!xsink->isException())
         xsink->raiseException("QABSTRACTITEMMODEL-COLUMNCOUNT-PARAM-ERROR", "expecting a QModelIndex object as first argument to QAbstractItemModel::columnCount()");
      return 0;
   }
   ReferenceHolder<QoreQModelIndex> parentHolder(parent, xsink);
   return new QoreNode((int64)qaim->getQAbstractItemModel()->columnCount(*(static_cast<QModelIndex *>(parent))));
}

////virtual QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const = 0
//static QoreNode *QABSTRACTITEMMODEL_data(Object *self, QoreAbstractQAbstractItemModel *qaim, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   QoreQModelIndex *index = (p && p->type == NT_OBJECT) ? (QoreQModelIndex *)p->val.object->getReferencedPrivateData(CID_QMODELINDEX, xsink) : 0;
//   if (!index) {
//      if (!xsink->isException())
//         xsink->raiseException("QABSTRACTITEMMODEL-DATA-PARAM-ERROR", "expecting a QModelIndex object as first argument to QAbstractItemModel::data()");
//      return 0;
//   }
//   ReferenceHolder<QoreQModelIndex> indexHolder(index, xsink);
//   p = get_param(params, 1);
//   int role = p ? p->getAsInt() : 0;
//   ??? return new QoreNode((int64)qaim->getQAbstractItemModel()->data(*(static_cast<QModelIndex *>(index)), role));
//}

////virtual bool dropMimeData ( const QMimeData * data, Qt::DropAction action, int row, int column, const QModelIndex & parent )
//static QoreNode *QABSTRACTITEMMODEL_dropMimeData(Object *self, QoreAbstractQAbstractItemModel *qaim, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QMimeData* data = p;
//   p = get_param(params, 1);
//   Qt::DropAction action = (Qt::DropAction)(p ? p->getAsInt() : 0);
//   p = get_param(params, 2);
//   int row = p ? p->getAsInt() : 0;
//   p = get_param(params, 3);
//   int column = p ? p->getAsInt() : 0;
//   p = get_param(params, 4);
//   QoreQModelIndex *parent = (p && p->type == NT_OBJECT) ? (QoreQModelIndex *)p->val.object->getReferencedPrivateData(CID_QMODELINDEX, xsink) : 0;
//   if (!parent) {
//      if (!xsink->isException())
//         xsink->raiseException("QABSTRACTITEMMODEL-DROPMIMEDATA-PARAM-ERROR", "expecting a QModelIndex object as fifth argument to QAbstractItemModel::dropMimeData()");
//      return 0;
//   }
//   ReferenceHolder<QoreQModelIndex> parentHolder(parent, xsink);
//   return new QoreNode(qaim->getQAbstractItemModel()->dropMimeData(data, action, row, column, *(static_cast<QModelIndex *>(parent))));
//}

//virtual void fetchMore ( const QModelIndex & parent )
static QoreNode *QABSTRACTITEMMODEL_fetchMore(Object *self, QoreAbstractQAbstractItemModel *qaim, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQModelIndex *parent = (p && p->type == NT_OBJECT) ? (QoreQModelIndex *)p->val.object->getReferencedPrivateData(CID_QMODELINDEX, xsink) : 0;
   if (!parent) {
      if (!xsink->isException())
         xsink->raiseException("QABSTRACTITEMMODEL-FETCHMORE-PARAM-ERROR", "expecting a QModelIndex object as first argument to QAbstractItemModel::fetchMore()");
      return 0;
   }
   ReferenceHolder<QoreQModelIndex> parentHolder(parent, xsink);
   qaim->getQAbstractItemModel()->fetchMore(*(static_cast<QModelIndex *>(parent)));
   return 0;
}

//virtual Qt::ItemFlags flags ( const QModelIndex & index ) const
static QoreNode *QABSTRACTITEMMODEL_flags(Object *self, QoreAbstractQAbstractItemModel *qaim, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQModelIndex *index = (p && p->type == NT_OBJECT) ? (QoreQModelIndex *)p->val.object->getReferencedPrivateData(CID_QMODELINDEX, xsink) : 0;
   if (!index) {
      if (!xsink->isException())
         xsink->raiseException("QABSTRACTITEMMODEL-FLAGS-PARAM-ERROR", "expecting a QModelIndex object as first argument to QAbstractItemModel::flags()");
      return 0;
   }
   ReferenceHolder<QoreQModelIndex> indexHolder(index, xsink);
   return new QoreNode((int64)qaim->getQAbstractItemModel()->flags(*(static_cast<QModelIndex *>(index))));
}

//virtual bool hasChildren ( const QModelIndex & parent = QModelIndex() ) const
static QoreNode *QABSTRACTITEMMODEL_hasChildren(Object *self, QoreAbstractQAbstractItemModel *qaim, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQModelIndex *parent = (p && p->type == NT_OBJECT) ? (QoreQModelIndex *)p->val.object->getReferencedPrivateData(CID_QMODELINDEX, xsink) : 0;
   if (!parent) {
      if (!xsink->isException())
         xsink->raiseException("QABSTRACTITEMMODEL-HASCHILDREN-PARAM-ERROR", "expecting a QModelIndex object as first argument to QAbstractItemModel::hasChildren()");
      return 0;
   }
   ReferenceHolder<QoreQModelIndex> parentHolder(parent, xsink);
   return new QoreNode(qaim->getQAbstractItemModel()->hasChildren(*(static_cast<QModelIndex *>(parent))));
}

//bool hasIndex ( int row, int column, const QModelIndex & parent = QModelIndex() ) const
static QoreNode *QABSTRACTITEMMODEL_hasIndex(Object *self, QoreAbstractQAbstractItemModel *qaim, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int row = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int column = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   QoreQModelIndex *parent = (p && p->type == NT_OBJECT) ? (QoreQModelIndex *)p->val.object->getReferencedPrivateData(CID_QMODELINDEX, xsink) : 0;
   if (!parent) {
      if (!xsink->isException())
         xsink->raiseException("QABSTRACTITEMMODEL-HASINDEX-PARAM-ERROR", "expecting a QModelIndex object as third argument to QAbstractItemModel::hasIndex()");
      return 0;
   }
   ReferenceHolder<QoreQModelIndex> parentHolder(parent, xsink);
   return new QoreNode(qaim->getQAbstractItemModel()->hasIndex(row, column, *(static_cast<QModelIndex *>(parent))));
}

////virtual QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const
//static QoreNode *QABSTRACTITEMMODEL_headerData(Object *self, QoreAbstractQAbstractItemModel *qaim, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   int section = p ? p->getAsInt() : 0;
//   p = get_param(params, 1);
//   Qt::Orientation orientation = (Qt::Orientation)(p ? p->getAsInt() : 0);
//   p = get_param(params, 2);
//   int role = p ? p->getAsInt() : 0;
//   ??? return new QoreNode((int64)qaim->getQAbstractItemModel()->headerData(section, orientation, role));
//}

//virtual QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const = 0
static QoreNode *QABSTRACTITEMMODEL_index(Object *self, QoreAbstractQAbstractItemModel *qaim, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int row = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int column = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   QoreQModelIndex *parent = (p && p->type == NT_OBJECT) ? (QoreQModelIndex *)p->val.object->getReferencedPrivateData(CID_QMODELINDEX, xsink) : 0;
   if (!parent) {
      if (!xsink->isException())
         xsink->raiseException("QABSTRACTITEMMODEL-INDEX-PARAM-ERROR", "expecting a QModelIndex object as third argument to QAbstractItemModel::index()");
      return 0;
   }
   ReferenceHolder<QoreQModelIndex> parentHolder(parent, xsink);
   Object *o_qmi = new Object(QC_QModelIndex, getProgram());
   QoreQModelIndex *q_qmi = new QoreQModelIndex(qaim->getQAbstractItemModel()->index(row, column, *(static_cast<QModelIndex *>(parent))));
   o_qmi->setPrivate(CID_QMODELINDEX, q_qmi);
   return new QoreNode(o_qmi);
}

//bool insertColumn ( int column, const QModelIndex & parent = QModelIndex() )
static QoreNode *QABSTRACTITEMMODEL_insertColumn(Object *self, QoreAbstractQAbstractItemModel *qaim, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int column = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   QoreQModelIndex *parent = (p && p->type == NT_OBJECT) ? (QoreQModelIndex *)p->val.object->getReferencedPrivateData(CID_QMODELINDEX, xsink) : 0;
   if (!parent) {
      if (!xsink->isException())
         xsink->raiseException("QABSTRACTITEMMODEL-INSERTCOLUMN-PARAM-ERROR", "expecting a QModelIndex object as second argument to QAbstractItemModel::insertColumn()");
      return 0;
   }
   ReferenceHolder<QoreQModelIndex> parentHolder(parent, xsink);
   return new QoreNode(qaim->getQAbstractItemModel()->insertColumn(column, *(static_cast<QModelIndex *>(parent))));
}

//virtual bool insertColumns ( int column, int count, const QModelIndex & parent = QModelIndex() )
static QoreNode *QABSTRACTITEMMODEL_insertColumns(Object *self, QoreAbstractQAbstractItemModel *qaim, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int column = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int count = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   QoreQModelIndex *parent = (p && p->type == NT_OBJECT) ? (QoreQModelIndex *)p->val.object->getReferencedPrivateData(CID_QMODELINDEX, xsink) : 0;
   if (!parent) {
      if (!xsink->isException())
         xsink->raiseException("QABSTRACTITEMMODEL-INSERTCOLUMNS-PARAM-ERROR", "expecting a QModelIndex object as third argument to QAbstractItemModel::insertColumns()");
      return 0;
   }
   ReferenceHolder<QoreQModelIndex> parentHolder(parent, xsink);
   return new QoreNode(qaim->getQAbstractItemModel()->insertColumns(column, count, *(static_cast<QModelIndex *>(parent))));
}

//bool insertRow ( int row, const QModelIndex & parent = QModelIndex() )
static QoreNode *QABSTRACTITEMMODEL_insertRow(Object *self, QoreAbstractQAbstractItemModel *qaim, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int row = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   QoreQModelIndex *parent = (p && p->type == NT_OBJECT) ? (QoreQModelIndex *)p->val.object->getReferencedPrivateData(CID_QMODELINDEX, xsink) : 0;
   if (!parent) {
      if (!xsink->isException())
         xsink->raiseException("QABSTRACTITEMMODEL-INSERTROW-PARAM-ERROR", "expecting a QModelIndex object as second argument to QAbstractItemModel::insertRow()");
      return 0;
   }
   ReferenceHolder<QoreQModelIndex> parentHolder(parent, xsink);
   return new QoreNode(qaim->getQAbstractItemModel()->insertRow(row, *(static_cast<QModelIndex *>(parent))));
}

//virtual bool insertRows ( int row, int count, const QModelIndex & parent = QModelIndex() )
static QoreNode *QABSTRACTITEMMODEL_insertRows(Object *self, QoreAbstractQAbstractItemModel *qaim, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int row = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int count = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   QoreQModelIndex *parent = (p && p->type == NT_OBJECT) ? (QoreQModelIndex *)p->val.object->getReferencedPrivateData(CID_QMODELINDEX, xsink) : 0;
   if (!parent) {
      if (!xsink->isException())
         xsink->raiseException("QABSTRACTITEMMODEL-INSERTROWS-PARAM-ERROR", "expecting a QModelIndex object as third argument to QAbstractItemModel::insertRows()");
      return 0;
   }
   ReferenceHolder<QoreQModelIndex> parentHolder(parent, xsink);
   return new QoreNode(qaim->getQAbstractItemModel()->insertRows(row, count, *(static_cast<QModelIndex *>(parent))));
}

////virtual QMap<int, QVariant> itemData ( const QModelIndex & index ) const
//static QoreNode *QABSTRACTITEMMODEL_itemData(Object *self, QoreAbstractQAbstractItemModel *qaim, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   QoreQModelIndex *index = (p && p->type == NT_OBJECT) ? (QoreQModelIndex *)p->val.object->getReferencedPrivateData(CID_QMODELINDEX, xsink) : 0;
//   if (!index) {
//      if (!xsink->isException())
//         xsink->raiseException("QABSTRACTITEMMODEL-ITEMDATA-PARAM-ERROR", "expecting a QModelIndex object as first argument to QAbstractItemModel::itemData()");
//      return 0;
//   }
//   ReferenceHolder<QoreQModelIndex> indexHolder(index, xsink);
//   ??? return new QoreNode((int64)qaim->getQAbstractItemModel()->itemData(*(static_cast<QModelIndex *>(index))));
//}

////virtual QModelIndexList match ( const QModelIndex & start, int role, const QVariant & value, int hits = 1, Qt::MatchFlags flags = Qt::MatchFlags( Qt::MatchStartsWith | Qt::MatchWrap ) ) const
//static QoreNode *QABSTRACTITEMMODEL_match(Object *self, QoreAbstractQAbstractItemModel *qaim, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   QoreQModelIndex *start = (p && p->type == NT_OBJECT) ? (QoreQModelIndex *)p->val.object->getReferencedPrivateData(CID_QMODELINDEX, xsink) : 0;
//   if (!start) {
//      if (!xsink->isException())
//         xsink->raiseException("QABSTRACTITEMMODEL-MATCH-PARAM-ERROR", "expecting a QModelIndex object as first argument to QAbstractItemModel::match()");
//      return 0;
//   }
//   ReferenceHolder<QoreQModelIndex> startHolder(start, xsink);
//   p = get_param(params, 1);
//   int role = p ? p->getAsInt() : 0;
//   p = get_param(params, 2);
//   ??? QVariant value = p;
//   p = get_param(params, 3);
//   int hits = !is_nothing(p) ? p->getAsInt() : 1;
//   p = get_param(params, 4);
//   Qt::MatchFlags flags = (Qt::MatchFlags)(p ? p->getAsInt() : 0);
//   ??? return new QoreNode((int64)qaim->getQAbstractItemModel()->match(*(static_cast<QModelIndex *>(start)), role, value, hits, flags));
//}

////virtual QMimeData * mimeData ( const QModelIndexList & indexes ) const
//static QoreNode *QABSTRACTITEMMODEL_mimeData(Object *self, QoreAbstractQAbstractItemModel *qaim, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QModelIndexList indexes = p;
//   ??? return new QoreNode((int64)qaim->getQAbstractItemModel()->mimeData(indexes));
//}

////virtual QStringList mimeTypes () const
//static QoreNode *QABSTRACTITEMMODEL_mimeTypes(Object *self, QoreAbstractQAbstractItemModel *qaim, QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreNode((int64)qaim->getQAbstractItemModel()->mimeTypes());
//}

//virtual QModelIndex parent ( const QModelIndex & index ) const = 0
static QoreNode *QABSTRACTITEMMODEL_parent(Object *self, QoreAbstractQAbstractItemModel *qaim, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQModelIndex *index = (p && p->type == NT_OBJECT) ? (QoreQModelIndex *)p->val.object->getReferencedPrivateData(CID_QMODELINDEX, xsink) : 0;
   if (!index) {
      if (!xsink->isException())
         xsink->raiseException("QABSTRACTITEMMODEL-PARENT-PARAM-ERROR", "expecting a QModelIndex object as first argument to QAbstractItemModel::parent()");
      return 0;
   }
   ReferenceHolder<QoreQModelIndex> indexHolder(index, xsink);
   Object *o_qmi = new Object(QC_QModelIndex, getProgram());
   QoreQModelIndex *q_qmi = new QoreQModelIndex(qaim->getQAbstractItemModel()->parent(*(static_cast<QModelIndex *>(index))));
   o_qmi->setPrivate(CID_QMODELINDEX, q_qmi);
   return new QoreNode(o_qmi);
}

//bool removeColumn ( int column, const QModelIndex & parent = QModelIndex() )
static QoreNode *QABSTRACTITEMMODEL_removeColumn(Object *self, QoreAbstractQAbstractItemModel *qaim, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int column = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   QoreQModelIndex *parent = (p && p->type == NT_OBJECT) ? (QoreQModelIndex *)p->val.object->getReferencedPrivateData(CID_QMODELINDEX, xsink) : 0;
   if (!parent) {
      if (!xsink->isException())
         xsink->raiseException("QABSTRACTITEMMODEL-REMOVECOLUMN-PARAM-ERROR", "expecting a QModelIndex object as second argument to QAbstractItemModel::removeColumn()");
      return 0;
   }
   ReferenceHolder<QoreQModelIndex> parentHolder(parent, xsink);
   return new QoreNode(qaim->getQAbstractItemModel()->removeColumn(column, *(static_cast<QModelIndex *>(parent))));
}

//virtual bool removeColumns ( int column, int count, const QModelIndex & parent = QModelIndex() )
static QoreNode *QABSTRACTITEMMODEL_removeColumns(Object *self, QoreAbstractQAbstractItemModel *qaim, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int column = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int count = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   QoreQModelIndex *parent = (p && p->type == NT_OBJECT) ? (QoreQModelIndex *)p->val.object->getReferencedPrivateData(CID_QMODELINDEX, xsink) : 0;
   if (!parent) {
      if (!xsink->isException())
         xsink->raiseException("QABSTRACTITEMMODEL-REMOVECOLUMNS-PARAM-ERROR", "expecting a QModelIndex object as third argument to QAbstractItemModel::removeColumns()");
      return 0;
   }
   ReferenceHolder<QoreQModelIndex> parentHolder(parent, xsink);
   return new QoreNode(qaim->getQAbstractItemModel()->removeColumns(column, count, *(static_cast<QModelIndex *>(parent))));
}

//bool removeRow ( int row, const QModelIndex & parent = QModelIndex() )
static QoreNode *QABSTRACTITEMMODEL_removeRow(Object *self, QoreAbstractQAbstractItemModel *qaim, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int row = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   QoreQModelIndex *parent = (p && p->type == NT_OBJECT) ? (QoreQModelIndex *)p->val.object->getReferencedPrivateData(CID_QMODELINDEX, xsink) : 0;
   if (!parent) {
      if (!xsink->isException())
         xsink->raiseException("QABSTRACTITEMMODEL-REMOVEROW-PARAM-ERROR", "expecting a QModelIndex object as second argument to QAbstractItemModel::removeRow()");
      return 0;
   }
   ReferenceHolder<QoreQModelIndex> parentHolder(parent, xsink);
   return new QoreNode(qaim->getQAbstractItemModel()->removeRow(row, *(static_cast<QModelIndex *>(parent))));
}

//virtual bool removeRows ( int row, int count, const QModelIndex & parent = QModelIndex() )
static QoreNode *QABSTRACTITEMMODEL_removeRows(Object *self, QoreAbstractQAbstractItemModel *qaim, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int row = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int count = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   QoreQModelIndex *parent = (p && p->type == NT_OBJECT) ? (QoreQModelIndex *)p->val.object->getReferencedPrivateData(CID_QMODELINDEX, xsink) : 0;
   if (!parent) {
      if (!xsink->isException())
         xsink->raiseException("QABSTRACTITEMMODEL-REMOVEROWS-PARAM-ERROR", "expecting a QModelIndex object as third argument to QAbstractItemModel::removeRows()");
      return 0;
   }
   ReferenceHolder<QoreQModelIndex> parentHolder(parent, xsink);
   return new QoreNode(qaim->getQAbstractItemModel()->removeRows(row, count, *(static_cast<QModelIndex *>(parent))));
}

//virtual int rowCount ( const QModelIndex & parent = QModelIndex() ) const = 0
static QoreNode *QABSTRACTITEMMODEL_rowCount(Object *self, QoreAbstractQAbstractItemModel *qaim, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQModelIndex *parent = (p && p->type == NT_OBJECT) ? (QoreQModelIndex *)p->val.object->getReferencedPrivateData(CID_QMODELINDEX, xsink) : 0;
   if (!parent) {
      if (!xsink->isException())
         xsink->raiseException("QABSTRACTITEMMODEL-ROWCOUNT-PARAM-ERROR", "expecting a QModelIndex object as first argument to QAbstractItemModel::rowCount()");
      return 0;
   }
   ReferenceHolder<QoreQModelIndex> parentHolder(parent, xsink);
   return new QoreNode((int64)qaim->getQAbstractItemModel()->rowCount(*(static_cast<QModelIndex *>(parent))));
}

////virtual bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole )
//static QoreNode *QABSTRACTITEMMODEL_setData(Object *self, QoreAbstractQAbstractItemModel *qaim, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   QoreQModelIndex *index = (p && p->type == NT_OBJECT) ? (QoreQModelIndex *)p->val.object->getReferencedPrivateData(CID_QMODELINDEX, xsink) : 0;
//   if (!index) {
//      if (!xsink->isException())
//         xsink->raiseException("QABSTRACTITEMMODEL-SETDATA-PARAM-ERROR", "expecting a QModelIndex object as first argument to QAbstractItemModel::setData()");
//      return 0;
//   }
//   ReferenceHolder<QoreQModelIndex> indexHolder(index, xsink);
//   p = get_param(params, 1);
//   ??? QVariant value = p;
//   p = get_param(params, 2);
//   int role = p ? p->getAsInt() : 0;
//   return new QoreNode(qaim->getQAbstractItemModel()->setData(*(static_cast<QModelIndex *>(index)), value, role));
//}

////virtual bool setHeaderData ( int section, Qt::Orientation orientation, const QVariant & value, int role = Qt::EditRole )
//static QoreNode *QABSTRACTITEMMODEL_setHeaderData(Object *self, QoreAbstractQAbstractItemModel *qaim, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   int section = p ? p->getAsInt() : 0;
//   p = get_param(params, 1);
//   Qt::Orientation orientation = (Qt::Orientation)(p ? p->getAsInt() : 0);
//   p = get_param(params, 2);
//   ??? QVariant value = p;
//   p = get_param(params, 3);
//   int role = p ? p->getAsInt() : 0;
//   return new QoreNode(qaim->getQAbstractItemModel()->setHeaderData(section, orientation, value, role));
//}

////virtual bool setItemData ( const QModelIndex & index, const QMap<int, QVariant> & roles )
//static QoreNode *QABSTRACTITEMMODEL_setItemData(Object *self, QoreAbstractQAbstractItemModel *qaim, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   QoreQModelIndex *index = (p && p->type == NT_OBJECT) ? (QoreQModelIndex *)p->val.object->getReferencedPrivateData(CID_QMODELINDEX, xsink) : 0;
//   if (!index) {
//      if (!xsink->isException())
//         xsink->raiseException("QABSTRACTITEMMODEL-SETITEMDATA-PARAM-ERROR", "expecting a QModelIndex object as first argument to QAbstractItemModel::setItemData()");
//      return 0;
//   }
//   ReferenceHolder<QoreQModelIndex> indexHolder(index, xsink);
//   p = get_param(params, 1);
//   QAbstractItemModel::const^QMap<int const^qmap<int = (QAbstractItemModel::const^QMap<int)(p ? p->getAsInt() : 0);
//   p = get_param(params, 2);
//   ??? QVariant> roles = p;
//   return new QoreNode(qaim->getQAbstractItemModel()->setItemData(*(static_cast<QModelIndex *>(index)), const^qmap<int, roles));
//}

//void setSupportedDragActions ( Qt::DropActions actions )
static QoreNode *QABSTRACTITEMMODEL_setSupportedDragActions(Object *self, QoreAbstractQAbstractItemModel *qaim, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   Qt::DropActions actions = (Qt::DropActions)(p ? p->getAsInt() : 0);
   qaim->getQAbstractItemModel()->setSupportedDragActions(actions);
   return 0;
}

//QModelIndex sibling ( int row, int column, const QModelIndex & index ) const
static QoreNode *QABSTRACTITEMMODEL_sibling(Object *self, QoreAbstractQAbstractItemModel *qaim, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int row = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int column = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   QoreQModelIndex *index = (p && p->type == NT_OBJECT) ? (QoreQModelIndex *)p->val.object->getReferencedPrivateData(CID_QMODELINDEX, xsink) : 0;
   if (!index) {
      if (!xsink->isException())
         xsink->raiseException("QABSTRACTITEMMODEL-SIBLING-PARAM-ERROR", "expecting a QModelIndex object as third argument to QAbstractItemModel::sibling()");
      return 0;
   }
   ReferenceHolder<QoreQModelIndex> indexHolder(index, xsink);
   Object *o_qmi = new Object(QC_QModelIndex, getProgram());
   QoreQModelIndex *q_qmi = new QoreQModelIndex(qaim->getQAbstractItemModel()->sibling(row, column, *(static_cast<QModelIndex *>(index))));
   o_qmi->setPrivate(CID_QMODELINDEX, q_qmi);
   return new QoreNode(o_qmi);
}

//virtual void sort ( int column, Qt::SortOrder order = Qt::AscendingOrder )
static QoreNode *QABSTRACTITEMMODEL_sort(Object *self, QoreAbstractQAbstractItemModel *qaim, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int column = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   Qt::SortOrder order = (Qt::SortOrder)(p ? p->getAsInt() : 0);
   qaim->getQAbstractItemModel()->sort(column, order);
   return 0;
}

//virtual QSize span ( const QModelIndex & index ) const
static QoreNode *QABSTRACTITEMMODEL_span(Object *self, QoreAbstractQAbstractItemModel *qaim, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQModelIndex *index = (p && p->type == NT_OBJECT) ? (QoreQModelIndex *)p->val.object->getReferencedPrivateData(CID_QMODELINDEX, xsink) : 0;
   if (!index) {
      if (!xsink->isException())
         xsink->raiseException("QABSTRACTITEMMODEL-SPAN-PARAM-ERROR", "expecting a QModelIndex object as first argument to QAbstractItemModel::span()");
      return 0;
   }
   ReferenceHolder<QoreQModelIndex> indexHolder(index, xsink);
   Object *o_qs = new Object(QC_QSize, getProgram());
   QoreQSize *q_qs = new QoreQSize(qaim->getQAbstractItemModel()->span(*(static_cast<QModelIndex *>(index))));
   o_qs->setPrivate(CID_QSIZE, q_qs);
   return new QoreNode(o_qs);
}

//Qt::DropActions supportedDragActions () const
static QoreNode *QABSTRACTITEMMODEL_supportedDragActions(Object *self, QoreAbstractQAbstractItemModel *qaim, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qaim->getQAbstractItemModel()->supportedDragActions());
}

//virtual Qt::DropActions supportedDropActions () const
static QoreNode *QABSTRACTITEMMODEL_supportedDropActions(Object *self, QoreAbstractQAbstractItemModel *qaim, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qaim->getQAbstractItemModel()->supportedDropActions());
}

//virtual void revert ()
static QoreNode *QABSTRACTITEMMODEL_revert(Object *self, QoreAbstractQAbstractItemModel *qaim, QoreNode *params, ExceptionSink *xsink)
{
   qaim->getQAbstractItemModel()->revert();
   return 0;
}

//virtual bool submit ()
static QoreNode *QABSTRACTITEMMODEL_submit(Object *self, QoreAbstractQAbstractItemModel *qaim, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qaim->getQAbstractItemModel()->submit());
}

QoreClass *initQAbstractItemModelClass(QoreClass *qobject)
{
   QC_QAbstractItemModel = new QoreClass("QAbstractItemModel", QDOM_GUI);
   CID_QABSTRACTITEMMODEL = QC_QAbstractItemModel->getID();

   QC_QAbstractItemModel->addBuiltinVirtualBaseClass(qobject);

   QC_QAbstractItemModel->setConstructor(QABSTRACTITEMMODEL_constructor);
   QC_QAbstractItemModel->setCopy((q_copy_t)QABSTRACTITEMMODEL_copy);

   QC_QAbstractItemModel->addMethod("buddy",                       (q_method_t)QABSTRACTITEMMODEL_buddy);
   QC_QAbstractItemModel->addMethod("canFetchMore",                (q_method_t)QABSTRACTITEMMODEL_canFetchMore);
   QC_QAbstractItemModel->addMethod("columnCount",                 (q_method_t)QABSTRACTITEMMODEL_columnCount);
   //QC_QAbstractItemModel->addMethod("data",                        (q_method_t)QABSTRACTITEMMODEL_data);
   //QC_QAbstractItemModel->addMethod("dropMimeData",                (q_method_t)QABSTRACTITEMMODEL_dropMimeData);
   QC_QAbstractItemModel->addMethod("fetchMore",                   (q_method_t)QABSTRACTITEMMODEL_fetchMore);
   QC_QAbstractItemModel->addMethod("flags",                       (q_method_t)QABSTRACTITEMMODEL_flags);
   QC_QAbstractItemModel->addMethod("hasChildren",                 (q_method_t)QABSTRACTITEMMODEL_hasChildren);
   QC_QAbstractItemModel->addMethod("hasIndex",                    (q_method_t)QABSTRACTITEMMODEL_hasIndex);
   //QC_QAbstractItemModel->addMethod("headerData",                  (q_method_t)QABSTRACTITEMMODEL_headerData);
   QC_QAbstractItemModel->addMethod("index",                       (q_method_t)QABSTRACTITEMMODEL_index);
   QC_QAbstractItemModel->addMethod("insertColumn",                (q_method_t)QABSTRACTITEMMODEL_insertColumn);
   QC_QAbstractItemModel->addMethod("insertColumns",               (q_method_t)QABSTRACTITEMMODEL_insertColumns);
   QC_QAbstractItemModel->addMethod("insertRow",                   (q_method_t)QABSTRACTITEMMODEL_insertRow);
   QC_QAbstractItemModel->addMethod("insertRows",                  (q_method_t)QABSTRACTITEMMODEL_insertRows);
   //QC_QAbstractItemModel->addMethod("itemData",                    (q_method_t)QABSTRACTITEMMODEL_itemData);
   //QC_QAbstractItemModel->addMethod("match",                       (q_method_t)QABSTRACTITEMMODEL_match);
   //QC_QAbstractItemModel->addMethod("mimeData",                    (q_method_t)QABSTRACTITEMMODEL_mimeData);
   //QC_QAbstractItemModel->addMethod("mimeTypes",                   (q_method_t)QABSTRACTITEMMODEL_mimeTypes);
   QC_QAbstractItemModel->addMethod("parent",                      (q_method_t)QABSTRACTITEMMODEL_parent);
   QC_QAbstractItemModel->addMethod("removeColumn",                (q_method_t)QABSTRACTITEMMODEL_removeColumn);
   QC_QAbstractItemModel->addMethod("removeColumns",               (q_method_t)QABSTRACTITEMMODEL_removeColumns);
   QC_QAbstractItemModel->addMethod("removeRow",                   (q_method_t)QABSTRACTITEMMODEL_removeRow);
   QC_QAbstractItemModel->addMethod("removeRows",                  (q_method_t)QABSTRACTITEMMODEL_removeRows);
   QC_QAbstractItemModel->addMethod("rowCount",                    (q_method_t)QABSTRACTITEMMODEL_rowCount);
   //QC_QAbstractItemModel->addMethod("setData",                     (q_method_t)QABSTRACTITEMMODEL_setData);
   //QC_QAbstractItemModel->addMethod("setHeaderData",               (q_method_t)QABSTRACTITEMMODEL_setHeaderData);
   //QC_QAbstractItemModel->addMethod("setItemData",                 (q_method_t)QABSTRACTITEMMODEL_setItemData);
   QC_QAbstractItemModel->addMethod("setSupportedDragActions",     (q_method_t)QABSTRACTITEMMODEL_setSupportedDragActions);
   QC_QAbstractItemModel->addMethod("sibling",                     (q_method_t)QABSTRACTITEMMODEL_sibling);
   QC_QAbstractItemModel->addMethod("sort",                        (q_method_t)QABSTRACTITEMMODEL_sort);
   QC_QAbstractItemModel->addMethod("span",                        (q_method_t)QABSTRACTITEMMODEL_span);
   QC_QAbstractItemModel->addMethod("supportedDragActions",        (q_method_t)QABSTRACTITEMMODEL_supportedDragActions);
   QC_QAbstractItemModel->addMethod("supportedDropActions",        (q_method_t)QABSTRACTITEMMODEL_supportedDropActions);
   QC_QAbstractItemModel->addMethod("revert",                      (q_method_t)QABSTRACTITEMMODEL_revert);
   QC_QAbstractItemModel->addMethod("submit",                      (q_method_t)QABSTRACTITEMMODEL_submit);

   return QC_QAbstractItemModel;
}
