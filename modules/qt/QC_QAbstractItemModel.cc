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
#include "QC_QModelIndex.h"
#include "QC_QMimeData.h"
#include "QC_QSize.h"

#include "qore-qt.h"

int CID_QABSTRACTITEMMODEL;
class QoreClass *QC_QAbstractItemModel = 0;

//QAbstractItemModel ( QObject * parent = 0 )
static void QABSTRACTITEMMODEL_constructor(QoreObject *self, const QoreNode *params, ExceptionSink *xsink)
{
   xsink->raiseException("QABSTRACTITEMMODEL-CONSTRUCTOR-ERROR", "QAbstractItemModel is an abstract class");
   return;
}

static void QABSTRACTITEMMODEL_copy(class QoreObject *self, class QoreObject *old, class QoreQAbstractItemModel *qaim, ExceptionSink *xsink)
{
   xsink->raiseException("QABSTRACTITEMMODEL-COPY-ERROR", "objects of this class cannot be copied");
}

//virtual QModelIndex buddy ( const QModelIndex & index ) const
static QoreNode *QABSTRACTITEMMODEL_buddy(QoreObject *self, QoreAbstractQAbstractItemModel *qaim, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQModelIndex *index = (p && p->type == NT_OBJECT) ? (QoreQModelIndex *)p->val.object->getReferencedPrivateData(CID_QMODELINDEX, xsink) : 0;
   if (!index) {
      if (!xsink->isException())
         xsink->raiseException("QABSTRACTITEMMODEL-BUDDY-PARAM-ERROR", "expecting a QModelIndex object as first argument to QAbstractItemModel::buddy()");
      return 0;
   }
   ReferenceHolder<QoreQModelIndex> indexHolder(index, xsink);
   QoreObject *o_qmi = new QoreObject(QC_QModelIndex, getProgram());
   QoreQModelIndex *q_qmi = new QoreQModelIndex(qaim->getQAbstractItemModel()->buddy(*(static_cast<QModelIndex *>(index))));
   o_qmi->setPrivate(CID_QMODELINDEX, q_qmi);
   return new QoreNode(o_qmi);
}

//virtual bool canFetchMore ( const QModelIndex & parent ) const
static QoreNode *QABSTRACTITEMMODEL_canFetchMore(QoreObject *self, QoreAbstractQAbstractItemModel *qaim, const QoreNode *params, ExceptionSink *xsink)
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
static QoreNode *QABSTRACTITEMMODEL_columnCount(QoreObject *self, QoreAbstractQAbstractItemModel *qaim, const QoreNode *params, ExceptionSink *xsink)
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

//virtual QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const = 0
static QoreNode *QABSTRACTITEMMODEL_data(QoreObject *self, QoreAbstractQAbstractItemModel *qaim, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQModelIndex *index = (p && p->type == NT_OBJECT) ? (QoreQModelIndex *)p->val.object->getReferencedPrivateData(CID_QMODELINDEX, xsink) : 0;
   if (!index) {
      if (!xsink->isException())
         xsink->raiseException("QABSTRACTITEMMODEL-DATA-PARAM-ERROR", "expecting a QModelIndex object as first argument to QAbstractItemModel::data()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> indexHolder(static_cast<AbstractPrivateData *>(index), xsink);
   p = get_param(params, 1);
   int role = !is_nothing(p) ? p->getAsInt() : Qt::DisplayRole;
   return return_qvariant(qaim->getQAbstractItemModel()->data(*(static_cast<QModelIndex *>(index)), role));
}

//virtual bool dropMimeData ( const QMimeData * data, Qt::DropAction action, int row, int column, const QModelIndex & parent )
static QoreNode *QABSTRACTITEMMODEL_dropMimeData(QoreObject *self, QoreAbstractQAbstractItemModel *qaim, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQMimeData *data = (p && p->type == NT_OBJECT) ? (QoreQMimeData *)p->val.object->getReferencedPrivateData(CID_QMIMEDATA, xsink) : 0;
   if (!data) {
      if (!xsink->isException())
         xsink->raiseException("QABSTRACTITEMMODEL-DROPMIMEDATA-PARAM-ERROR", "expecting a QMimeData object as first argument to QAbstractItemModel::dropMimeData()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> dataHolder(static_cast<AbstractPrivateData *>(data), xsink);
   p = get_param(params, 1);
   Qt::DropAction action = (Qt::DropAction)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   int row = p ? p->getAsInt() : 0;
   p = get_param(params, 3);
   int column = p ? p->getAsInt() : 0;
   p = get_param(params, 4);
   QoreQModelIndex *parent = (p && p->type == NT_OBJECT) ? (QoreQModelIndex *)p->val.object->getReferencedPrivateData(CID_QMODELINDEX, xsink) : 0;
   if (!parent) {
      if (!xsink->isException())
         xsink->raiseException("QABSTRACTITEMMODEL-DROPMIMEDATA-PARAM-ERROR", "expecting a QModelIndex object as fifth argument to QAbstractItemModel::dropMimeData()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   return new QoreNode(qaim->getQAbstractItemModel()->dropMimeData(static_cast<QMimeData *>(data->qobj), action, row, column, *(static_cast<QModelIndex *>(parent))));
}

//virtual void fetchMore ( const QModelIndex & parent )
static QoreNode *QABSTRACTITEMMODEL_fetchMore(QoreObject *self, QoreAbstractQAbstractItemModel *qaim, const QoreNode *params, ExceptionSink *xsink)
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
static QoreNode *QABSTRACTITEMMODEL_flags(QoreObject *self, QoreAbstractQAbstractItemModel *qaim, const QoreNode *params, ExceptionSink *xsink)
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
static QoreNode *QABSTRACTITEMMODEL_hasChildren(QoreObject *self, QoreAbstractQAbstractItemModel *qaim, const QoreNode *params, ExceptionSink *xsink)
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
static QoreNode *QABSTRACTITEMMODEL_hasIndex(QoreObject *self, QoreAbstractQAbstractItemModel *qaim, const QoreNode *params, ExceptionSink *xsink)
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
static QoreNode *QABSTRACTITEMMODEL_headerData(QoreObject *self, QoreAbstractQAbstractItemModel *qaim, const QoreNode *params, ExceptionSink *xsink)
  {
   QoreNode *p = get_param(params, 0);
   int section = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   Qt::Orientation orientation = (Qt::Orientation)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   int role = !is_nothing(p) ? p->getAsInt() : Qt::DisplayRole;
   return return_qvariant(qaim->getQAbstractItemModel()->headerData(section, orientation, role));
}

//virtual QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const = 0
static QoreNode *QABSTRACTITEMMODEL_index(QoreObject *self, QoreAbstractQAbstractItemModel *qaim, const QoreNode *params, ExceptionSink *xsink)
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
   QoreObject *o_qmi = new QoreObject(QC_QModelIndex, getProgram());
   QoreQModelIndex *q_qmi = new QoreQModelIndex(qaim->getQAbstractItemModel()->index(row, column, *(static_cast<QModelIndex *>(parent))));
   o_qmi->setPrivate(CID_QMODELINDEX, q_qmi);
   return new QoreNode(o_qmi);
}

//bool insertColumn ( int column, const QModelIndex & parent = QModelIndex() )
static QoreNode *QABSTRACTITEMMODEL_insertColumn(QoreObject *self, QoreAbstractQAbstractItemModel *qaim, const QoreNode *params, ExceptionSink *xsink)
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
static QoreNode *QABSTRACTITEMMODEL_insertColumns(QoreObject *self, QoreAbstractQAbstractItemModel *qaim, const QoreNode *params, ExceptionSink *xsink)
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
static QoreNode *QABSTRACTITEMMODEL_insertRow(QoreObject *self, QoreAbstractQAbstractItemModel *qaim, const QoreNode *params, ExceptionSink *xsink)
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
static QoreNode *QABSTRACTITEMMODEL_insertRows(QoreObject *self, QoreAbstractQAbstractItemModel *qaim, const QoreNode *params, ExceptionSink *xsink)
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
//static QoreNode *QABSTRACTITEMMODEL_itemData(QoreObject *self, QoreAbstractQAbstractItemModel *qaim, const QoreNode *params, ExceptionSink *xsink)
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
//static QoreNode *QABSTRACTITEMMODEL_match(QoreObject *self, QoreAbstractQAbstractItemModel *qaim, const QoreNode *params, ExceptionSink *xsink)
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
//static QoreNode *QABSTRACTITEMMODEL_mimeData(QoreObject *self, QoreAbstractQAbstractItemModel *qaim, const QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QModelIndexList indexes = p;
//   ??? return new QoreNode((int64)qaim->getQAbstractItemModel()->mimeData(indexes));
//}

////virtual QStringList mimeTypes () const
static QoreNode *QABSTRACTITEMMODEL_mimeTypes(QoreObject *self, QoreAbstractQAbstractItemModel *qaim, const QoreNode *params, ExceptionSink *xsink)
{
   QStringList strlist_rv = qaim->getQAbstractItemModel()->mimeTypes();
   QoreList *l = new QoreList();
   for (QStringList::iterator i = strlist_rv.begin(), e = strlist_rv.end(); i != e; ++i)
      l->push(new QoreStringNode((*i).toUtf8().data(), QCS_UTF8));
   return new QoreNode(l);
}

//virtual QModelIndex parent ( const QModelIndex & index ) const = 0
static QoreNode *QABSTRACTITEMMODEL_parent(QoreObject *self, QoreAbstractQAbstractItemModel *qaim, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQModelIndex *index = (p && p->type == NT_OBJECT) ? (QoreQModelIndex *)p->val.object->getReferencedPrivateData(CID_QMODELINDEX, xsink) : 0;
   if (!index) {
      if (!xsink->isException())
         xsink->raiseException("QABSTRACTITEMMODEL-PARENT-PARAM-ERROR", "expecting a QModelIndex object as first argument to QAbstractItemModel::parent()");
      return 0;
   }
   ReferenceHolder<QoreQModelIndex> indexHolder(index, xsink);
   QoreObject *o_qmi = new QoreObject(QC_QModelIndex, getProgram());
   QoreQModelIndex *q_qmi = new QoreQModelIndex(qaim->getQAbstractItemModel()->parent(*(static_cast<QModelIndex *>(index))));
   o_qmi->setPrivate(CID_QMODELINDEX, q_qmi);
   return new QoreNode(o_qmi);
}

//bool removeColumn ( int column, const QModelIndex & parent = QModelIndex() )
static QoreNode *QABSTRACTITEMMODEL_removeColumn(QoreObject *self, QoreAbstractQAbstractItemModel *qaim, const QoreNode *params, ExceptionSink *xsink)
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
static QoreNode *QABSTRACTITEMMODEL_removeColumns(QoreObject *self, QoreAbstractQAbstractItemModel *qaim, const QoreNode *params, ExceptionSink *xsink)
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
static QoreNode *QABSTRACTITEMMODEL_removeRow(QoreObject *self, QoreAbstractQAbstractItemModel *qaim, const QoreNode *params, ExceptionSink *xsink)
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
static QoreNode *QABSTRACTITEMMODEL_removeRows(QoreObject *self, QoreAbstractQAbstractItemModel *qaim, const QoreNode *params, ExceptionSink *xsink)
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
static QoreNode *QABSTRACTITEMMODEL_rowCount(QoreObject *self, QoreAbstractQAbstractItemModel *qaim, const QoreNode *params, ExceptionSink *xsink)
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

//virtual bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole )
static QoreNode *QABSTRACTITEMMODEL_setData(QoreObject *self, QoreAbstractQAbstractItemModel *qaim, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQModelIndex *index = (p && p->type == NT_OBJECT) ? (QoreQModelIndex *)p->val.object->getReferencedPrivateData(CID_QMODELINDEX, xsink) : 0;
   if (!index) {
      if (!xsink->isException())
         xsink->raiseException("QABSTRACTITEMMODEL-SETDATA-PARAM-ERROR", "expecting a QModelIndex object as first argument to QAbstractItemModel::setData()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> indexHolder(static_cast<AbstractPrivateData *>(index), xsink);
   p = get_param(params, 1);
   QVariant value;
   if (get_qvariant(p, value, xsink))
      return 0;
   p = get_param(params, 2);
   int role = !is_nothing(p) ? p->getAsInt() : Qt::EditRole;
   return new QoreNode(qaim->getQAbstractItemModel()->setData(*(static_cast<QModelIndex *>(index)), value, role));
}

//virtual bool setHeaderData ( int section, Qt::Orientation orientation, const QVariant & value, int role = Qt::EditRole )
static QoreNode *QABSTRACTITEMMODEL_setHeaderData(QoreObject *self, QoreAbstractQAbstractItemModel *qaim, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int section = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   Qt::Orientation orientation = (Qt::Orientation)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   QVariant value;
   if (get_qvariant(p, value, xsink))
      return 0;
   p = get_param(params, 3);
   int role = !is_nothing(p) ? p->getAsInt() : Qt::EditRole;
   return new QoreNode(qaim->getQAbstractItemModel()->setHeaderData(section, orientation, value, role));
}

////virtual bool setItemData ( const QModelIndex & index, const QMap<int, QVariant> & roles )
//static QoreNode *QABSTRACTITEMMODEL_setItemData(QoreObject *self, QoreAbstractQAbstractItemModel *qaim, const QoreNode *params, ExceptionSink *xsink)
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
static QoreNode *QABSTRACTITEMMODEL_setSupportedDragActions(QoreObject *self, QoreAbstractQAbstractItemModel *qaim, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   Qt::DropActions actions = (Qt::DropActions)(p ? p->getAsInt() : 0);
   qaim->getQAbstractItemModel()->setSupportedDragActions(actions);
   return 0;
}

//QModelIndex sibling ( int row, int column, const QModelIndex & index ) const
static QoreNode *QABSTRACTITEMMODEL_sibling(QoreObject *self, QoreAbstractQAbstractItemModel *qaim, const QoreNode *params, ExceptionSink *xsink)
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
   QoreObject *o_qmi = new QoreObject(QC_QModelIndex, getProgram());
   QoreQModelIndex *q_qmi = new QoreQModelIndex(qaim->getQAbstractItemModel()->sibling(row, column, *(static_cast<QModelIndex *>(index))));
   o_qmi->setPrivate(CID_QMODELINDEX, q_qmi);
   return new QoreNode(o_qmi);
}

//virtual void sort ( int column, Qt::SortOrder order = Qt::AscendingOrder )
static QoreNode *QABSTRACTITEMMODEL_sort(QoreObject *self, QoreAbstractQAbstractItemModel *qaim, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int column = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   Qt::SortOrder order = (Qt::SortOrder)(p ? p->getAsInt() : 0);
   qaim->getQAbstractItemModel()->sort(column, order);
   return 0;
}

//virtual QSize span ( const QModelIndex & index ) const
static QoreNode *QABSTRACTITEMMODEL_span(QoreObject *self, QoreAbstractQAbstractItemModel *qaim, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQModelIndex *index = (p && p->type == NT_OBJECT) ? (QoreQModelIndex *)p->val.object->getReferencedPrivateData(CID_QMODELINDEX, xsink) : 0;
   if (!index) {
      if (!xsink->isException())
         xsink->raiseException("QABSTRACTITEMMODEL-SPAN-PARAM-ERROR", "expecting a QModelIndex object as first argument to QAbstractItemModel::span()");
      return 0;
   }
   ReferenceHolder<QoreQModelIndex> indexHolder(index, xsink);
   QoreObject *o_qs = new QoreObject(QC_QSize, getProgram());
   QoreQSize *q_qs = new QoreQSize(qaim->getQAbstractItemModel()->span(*(static_cast<QModelIndex *>(index))));
   o_qs->setPrivate(CID_QSIZE, q_qs);
   return new QoreNode(o_qs);
}

//Qt::DropActions supportedDragActions () const
static QoreNode *QABSTRACTITEMMODEL_supportedDragActions(QoreObject *self, QoreAbstractQAbstractItemModel *qaim, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qaim->getQAbstractItemModel()->supportedDragActions());
}

//virtual Qt::DropActions supportedDropActions () const
static QoreNode *QABSTRACTITEMMODEL_supportedDropActions(QoreObject *self, QoreAbstractQAbstractItemModel *qaim, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qaim->getQAbstractItemModel()->supportedDropActions());
}

//virtual void revert ()
static QoreNode *QABSTRACTITEMMODEL_revert(QoreObject *self, QoreAbstractQAbstractItemModel *qaim, const QoreNode *params, ExceptionSink *xsink)
{
   qaim->getQAbstractItemModel()->revert();
   return 0;
}

//virtual bool submit ()
static QoreNode *QABSTRACTITEMMODEL_submit(QoreObject *self, QoreAbstractQAbstractItemModel *qaim, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qaim->getQAbstractItemModel()->submit());
}

//void beginInsertColumns ( const QModelIndex & parent, int first, int last )
static QoreNode *QABSTRACTITEMMODEL_beginInsertColumns(QoreObject *self, QoreAbstractQAbstractItemModel *qaim, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQModelIndex *parent = (p && p->type == NT_OBJECT) ? (QoreQModelIndex *)p->val.object->getReferencedPrivateData(CID_QMODELINDEX, xsink) : 0;
   if (!parent) {
      if (!xsink->isException())
         xsink->raiseException("QABSTRACTITEMMODEL-BEGININSERTCOLUMNS-PARAM-ERROR", "expecting a QModelIndex object as first argument to QAbstractItemModel::beginInsertColumns()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   p = get_param(params, 1);
   int first = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   int last = p ? p->getAsInt() : 0;
   qaim->beginInsertColumns(*(static_cast<QModelIndex *>(parent)), first, last);
   return 0;
}

//void beginInsertRows ( const QModelIndex & parent, int first, int last )
static QoreNode *QABSTRACTITEMMODEL_beginInsertRows(QoreObject *self, QoreAbstractQAbstractItemModel *qaim, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQModelIndex *parent = (p && p->type == NT_OBJECT) ? (QoreQModelIndex *)p->val.object->getReferencedPrivateData(CID_QMODELINDEX, xsink) : 0;
   if (!parent) {
      if (!xsink->isException())
         xsink->raiseException("QABSTRACTITEMMODEL-BEGININSERTROWS-PARAM-ERROR", "expecting a QModelIndex object as first argument to QAbstractItemModel::beginInsertRows()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   p = get_param(params, 1);
   int first = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   int last = p ? p->getAsInt() : 0;
   qaim->beginInsertRows(*(static_cast<QModelIndex *>(parent)), first, last);
   return 0;
}

//void beginRemoveColumns ( const QModelIndex & parent, int first, int last )
static QoreNode *QABSTRACTITEMMODEL_beginRemoveColumns(QoreObject *self, QoreAbstractQAbstractItemModel *qaim, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQModelIndex *parent = (p && p->type == NT_OBJECT) ? (QoreQModelIndex *)p->val.object->getReferencedPrivateData(CID_QMODELINDEX, xsink) : 0;
   if (!parent) {
      if (!xsink->isException())
         xsink->raiseException("QABSTRACTITEMMODEL-BEGINREMOVECOLUMNS-PARAM-ERROR", "expecting a QModelIndex object as first argument to QAbstractItemModel::beginRemoveColumns()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   p = get_param(params, 1);
   int first = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   int last = p ? p->getAsInt() : 0;
   qaim->beginRemoveColumns(*(static_cast<QModelIndex *>(parent)), first, last);
   return 0;
}

//void beginRemoveRows ( const QModelIndex & parent, int first, int last )
static QoreNode *QABSTRACTITEMMODEL_beginRemoveRows(QoreObject *self, QoreAbstractQAbstractItemModel *qaim, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQModelIndex *parent = (p && p->type == NT_OBJECT) ? (QoreQModelIndex *)p->val.object->getReferencedPrivateData(CID_QMODELINDEX, xsink) : 0;
   if (!parent) {
      if (!xsink->isException())
         xsink->raiseException("QABSTRACTITEMMODEL-BEGINREMOVEROWS-PARAM-ERROR", "expecting a QModelIndex object as first argument to QAbstractItemModel::beginRemoveRows()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   p = get_param(params, 1);
   int first = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   int last = p ? p->getAsInt() : 0;
   qaim->beginRemoveRows(*(static_cast<QModelIndex *>(parent)), first, last);
   return 0;
}

//void changePersistentIndex ( const QModelIndex & from, const QModelIndex & to )
static QoreNode *QABSTRACTITEMMODEL_changePersistentIndex(QoreObject *self, QoreAbstractQAbstractItemModel *qaim, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQModelIndex *from = (p && p->type == NT_OBJECT) ? (QoreQModelIndex *)p->val.object->getReferencedPrivateData(CID_QMODELINDEX, xsink) : 0;
   if (!from) {
      if (!xsink->isException())
         xsink->raiseException("QABSTRACTITEMMODEL-CHANGEPERSISTENTINDEX-PARAM-ERROR", "expecting a QModelIndex object as first argument to QAbstractItemModel::changePersistentIndex()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> fromHolder(static_cast<AbstractPrivateData *>(from), xsink);
   p = get_param(params, 1);
   QoreQModelIndex *to = (p && p->type == NT_OBJECT) ? (QoreQModelIndex *)p->val.object->getReferencedPrivateData(CID_QMODELINDEX, xsink) : 0;
   if (!to) {
      if (!xsink->isException())
         xsink->raiseException("QABSTRACTITEMMODEL-CHANGEPERSISTENTINDEX-PARAM-ERROR", "expecting a QModelIndex object as second argument to QAbstractItemModel::changePersistentIndex()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> toHolder(static_cast<AbstractPrivateData *>(to), xsink);
   qaim->changePersistentIndex(*(static_cast<QModelIndex *>(from)), *(static_cast<QModelIndex *>(to)));
   return 0;
}

////void changePersistentIndexList ( const QModelIndexList & from, const QModelIndexList & to )
//static QoreNode *QABSTRACTITEMMODEL_changePersistentIndexList(QoreObject *self, QoreAbstractQAbstractItemModel *qaim, const QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QModelIndexList from = p;
//   p = get_param(params, 1);
//   ??? QModelIndexList to = p;
//   qaim->changePersistentIndexList(from, to);
//   return 0;
//}

////QModelIndex createIndex ( int row, int column, void * ptr = 0 ) const
////QModelIndex createIndex ( int row, int column, quint32 id ) const
//static QoreNode *QABSTRACTITEMMODEL_createIndex(QoreObject *self, QoreAbstractQAbstractItemModel *qaim, const QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   int row = p ? p->getAsInt() : 0;
//   p = get_param(params, 1);
//   int column = p ? p->getAsInt() : 0;
//   p = get_param(params, 2);
//   if (p && p->type == NT_???) {
//      ??? void* ptr = p;
//      QoreObject *o_qmi = new QoreObject(QC_QModelIndex, getProgram());
//      QoreQModelIndex *q_qmi = new QoreQModelIndex(qaim->getQAbstractItemModel()->createIndex(row, column, ptr));
//      o_qmi->setPrivate(CID_QMODELINDEX, q_qmi);
//      return new QoreNode(o_qmi);
//   }
//   unsigned id = p ? p->getAsBigInt() : 0;
//   QoreObject *o_qmi = new QoreObject(QC_QModelIndex, getProgram());
//   QoreQModelIndex *q_qmi = new QoreQModelIndex(qaim->createIndex(row, column, id));
//   o_qmi->setPrivate(CID_QMODELINDEX, q_qmi);
//   return new QoreNode(o_qmi);
//}

//void endInsertColumns ()
static QoreNode *QABSTRACTITEMMODEL_endInsertColumns(QoreObject *self, QoreAbstractQAbstractItemModel *qaim, const QoreNode *params, ExceptionSink *xsink)
{
   qaim->endInsertColumns();
   return 0;
}

//void endInsertRows ()
static QoreNode *QABSTRACTITEMMODEL_endInsertRows(QoreObject *self, QoreAbstractQAbstractItemModel *qaim, const QoreNode *params, ExceptionSink *xsink)
{
   qaim->endInsertRows();
   return 0;
}

//void endRemoveColumns ()
static QoreNode *QABSTRACTITEMMODEL_endRemoveColumns(QoreObject *self, QoreAbstractQAbstractItemModel *qaim, const QoreNode *params, ExceptionSink *xsink)
{
   qaim->endRemoveColumns();
   return 0;
}

//void endRemoveRows ()
static QoreNode *QABSTRACTITEMMODEL_endRemoveRows(QoreObject *self, QoreAbstractQAbstractItemModel *qaim, const QoreNode *params, ExceptionSink *xsink)
{
   qaim->endRemoveRows();
   return 0;
}

////QModelIndexList persistentIndexList () const
//static QoreNode *QABSTRACTITEMMODEL_persistentIndexList(QoreObject *self, QoreAbstractQAbstractItemModel *qaim, const QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreNode((int64)qaim->persistentIndexList());
//}

//void reset ()
static QoreNode *QABSTRACTITEMMODEL_reset(QoreObject *self, QoreAbstractQAbstractItemModel *qaim, const QoreNode *params, ExceptionSink *xsink)
{
   qaim->reset();
   return 0;
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
   QC_QAbstractItemModel->addMethod("data",                        (q_method_t)QABSTRACTITEMMODEL_data);
   QC_QAbstractItemModel->addMethod("dropMimeData",                (q_method_t)QABSTRACTITEMMODEL_dropMimeData);
   QC_QAbstractItemModel->addMethod("fetchMore",                   (q_method_t)QABSTRACTITEMMODEL_fetchMore);
   QC_QAbstractItemModel->addMethod("flags",                       (q_method_t)QABSTRACTITEMMODEL_flags);
   QC_QAbstractItemModel->addMethod("hasChildren",                 (q_method_t)QABSTRACTITEMMODEL_hasChildren);
   QC_QAbstractItemModel->addMethod("hasIndex",                    (q_method_t)QABSTRACTITEMMODEL_hasIndex);
   QC_QAbstractItemModel->addMethod("headerData",                  (q_method_t)QABSTRACTITEMMODEL_headerData);
   QC_QAbstractItemModel->addMethod("index",                       (q_method_t)QABSTRACTITEMMODEL_index);
   QC_QAbstractItemModel->addMethod("insertColumn",                (q_method_t)QABSTRACTITEMMODEL_insertColumn);
   QC_QAbstractItemModel->addMethod("insertColumns",               (q_method_t)QABSTRACTITEMMODEL_insertColumns);
   QC_QAbstractItemModel->addMethod("insertRow",                   (q_method_t)QABSTRACTITEMMODEL_insertRow);
   QC_QAbstractItemModel->addMethod("insertRows",                  (q_method_t)QABSTRACTITEMMODEL_insertRows);
   //QC_QAbstractItemModel->addMethod("itemData",                    (q_method_t)QABSTRACTITEMMODEL_itemData);
   //QC_QAbstractItemModel->addMethod("match",                       (q_method_t)QABSTRACTITEMMODEL_match);
   //QC_QAbstractItemModel->addMethod("mimeData",                    (q_method_t)QABSTRACTITEMMODEL_mimeData);
   QC_QAbstractItemModel->addMethod("mimeTypes",                   (q_method_t)QABSTRACTITEMMODEL_mimeTypes);
   QC_QAbstractItemModel->addMethod("parent",                      (q_method_t)QABSTRACTITEMMODEL_parent);
   QC_QAbstractItemModel->addMethod("removeColumn",                (q_method_t)QABSTRACTITEMMODEL_removeColumn);
   QC_QAbstractItemModel->addMethod("removeColumns",               (q_method_t)QABSTRACTITEMMODEL_removeColumns);
   QC_QAbstractItemModel->addMethod("removeRow",                   (q_method_t)QABSTRACTITEMMODEL_removeRow);
   QC_QAbstractItemModel->addMethod("removeRows",                  (q_method_t)QABSTRACTITEMMODEL_removeRows);
   QC_QAbstractItemModel->addMethod("rowCount",                    (q_method_t)QABSTRACTITEMMODEL_rowCount);
   QC_QAbstractItemModel->addMethod("setData",                     (q_method_t)QABSTRACTITEMMODEL_setData);
   QC_QAbstractItemModel->addMethod("setHeaderData",               (q_method_t)QABSTRACTITEMMODEL_setHeaderData);
   //QC_QAbstractItemModel->addMethod("setItemData",                 (q_method_t)QABSTRACTITEMMODEL_setItemData);
   QC_QAbstractItemModel->addMethod("setSupportedDragActions",     (q_method_t)QABSTRACTITEMMODEL_setSupportedDragActions);
   QC_QAbstractItemModel->addMethod("sibling",                     (q_method_t)QABSTRACTITEMMODEL_sibling);
   QC_QAbstractItemModel->addMethod("sort",                        (q_method_t)QABSTRACTITEMMODEL_sort);
   QC_QAbstractItemModel->addMethod("span",                        (q_method_t)QABSTRACTITEMMODEL_span);
   QC_QAbstractItemModel->addMethod("supportedDragActions",        (q_method_t)QABSTRACTITEMMODEL_supportedDragActions);
   QC_QAbstractItemModel->addMethod("supportedDropActions",        (q_method_t)QABSTRACTITEMMODEL_supportedDropActions);
   QC_QAbstractItemModel->addMethod("revert",                      (q_method_t)QABSTRACTITEMMODEL_revert);
   QC_QAbstractItemModel->addMethod("submit",                      (q_method_t)QABSTRACTITEMMODEL_submit);

   // private methods
   QC_QAbstractItemModel->addMethod("beginInsertColumns",          (q_method_t)QABSTRACTITEMMODEL_beginInsertColumns, true);
   QC_QAbstractItemModel->addMethod("beginInsertRows",             (q_method_t)QABSTRACTITEMMODEL_beginInsertRows, true);
   QC_QAbstractItemModel->addMethod("beginRemoveColumns",          (q_method_t)QABSTRACTITEMMODEL_beginRemoveColumns, true);
   QC_QAbstractItemModel->addMethod("beginRemoveRows",             (q_method_t)QABSTRACTITEMMODEL_beginRemoveRows, true);
   QC_QAbstractItemModel->addMethod("changePersistentIndex",       (q_method_t)QABSTRACTITEMMODEL_changePersistentIndex, true);
   //QC_QAbstractItemModel->addMethod("changePersistentIndexList",   (q_method_t)QABSTRACTITEMMODEL_changePersistentIndexList, true);
   //QC_QAbstractItemModel->addMethod("createIndex",                 (q_method_t)QABSTRACTITEMMODEL_createIndex, true);
   QC_QAbstractItemModel->addMethod("endInsertColumns",            (q_method_t)QABSTRACTITEMMODEL_endInsertColumns, true);
   QC_QAbstractItemModel->addMethod("endInsertRows",               (q_method_t)QABSTRACTITEMMODEL_endInsertRows, true);
   QC_QAbstractItemModel->addMethod("endRemoveColumns",            (q_method_t)QABSTRACTITEMMODEL_endRemoveColumns, true);
   QC_QAbstractItemModel->addMethod("endRemoveRows",               (q_method_t)QABSTRACTITEMMODEL_endRemoveRows, true);
   //QC_QAbstractItemModel->addMethod("persistentIndexList",         (q_method_t)QABSTRACTITEMMODEL_persistentIndexList, true);
   QC_QAbstractItemModel->addMethod("reset",                       (q_method_t)QABSTRACTITEMMODEL_reset, true);

   return QC_QAbstractItemModel;
}
