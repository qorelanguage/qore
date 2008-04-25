/*
 QC_QGraphicsItemGroup.cc
 
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

#include "QC_QGraphicsItemGroup.h"

qore_classid_t CID_QGRAPHICSITEMGROUP;
QoreClass *QC_QGraphicsItemGroup = 0;

//QGraphicsItemGroup ( QGraphicsItem * parent = 0 )
static void QGRAPHICSITEMGROUP_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQGraphicsItem *parent = (p && p->getType() == NT_OBJECT) ? (QoreQGraphicsItem *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QGRAPHICSITEM, xsink) : 0;
   if (*xsink)
      return;
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   self->setPrivate(CID_QGRAPHICSITEMGROUP, new QoreQGraphicsItemGroup(self, parent ? static_cast<QGraphicsItem *>(parent->getQGraphicsItem()) : 0));
   return;
}

static void QGRAPHICSITEMGROUP_copy(QoreObject *self, QoreObject *old, QoreQGraphicsItemGroup *qgig, ExceptionSink *xsink)
{
   xsink->raiseException("QGRAPHICSITEMGROUP-COPY-ERROR", "objects of this class cannot be copied");
}

//void addToGroup ( QGraphicsItem * item )
static AbstractQoreNode *QGRAPHICSITEMGROUP_addToGroup(QoreObject *self, QoreAbstractQGraphicsItemGroup *qgig, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQGraphicsItem *item = (p && p->getType() == NT_OBJECT) ? (QoreQGraphicsItem *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QGRAPHICSITEM, xsink) : 0;
   if (!item) {
      if (!xsink->isException())
         xsink->raiseException("QGRAPHICSITEMGROUP-ADDTOGROUP-PARAM-ERROR", "expecting a QGraphicsItem object as first argument to QGraphicsItemGroup::addToGroup()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> itemHolder(static_cast<AbstractPrivateData *>(item), xsink);
   qgig->getQGraphicsItemGroup()->addToGroup(static_cast<QGraphicsItem *>(item->getQGraphicsItem()));
   return 0;
}

//void removeFromGroup ( QGraphicsItem * item )
static AbstractQoreNode *QGRAPHICSITEMGROUP_removeFromGroup(QoreObject *self, QoreAbstractQGraphicsItemGroup *qgig, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQGraphicsItem *item = (p && p->getType() == NT_OBJECT) ? (QoreQGraphicsItem *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QGRAPHICSITEM, xsink) : 0;
   if (!item) {
      if (!xsink->isException())
         xsink->raiseException("QGRAPHICSITEMGROUP-REMOVEFROMGROUP-PARAM-ERROR", "expecting a QGraphicsItem object as first argument to QGraphicsItemGroup::removeFromGroup()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> itemHolder(static_cast<AbstractPrivateData *>(item), xsink);
   qgig->getQGraphicsItemGroup()->removeFromGroup(static_cast<QGraphicsItem *>(item->getQGraphicsItem()));
   return 0;
}

QoreClass *initQGraphicsItemGroupClass(QoreClass *qgraphicsitem)
{
   QC_QGraphicsItemGroup = new QoreClass("QGraphicsItemGroup", QDOM_GUI);
   CID_QGRAPHICSITEMGROUP = QC_QGraphicsItemGroup->getID();

   QC_QGraphicsItemGroup->addBuiltinVirtualBaseClass(qgraphicsitem);

   QC_QGraphicsItemGroup->setConstructor(QGRAPHICSITEMGROUP_constructor);
   QC_QGraphicsItemGroup->setCopy((q_copy_t)QGRAPHICSITEMGROUP_copy);

   QC_QGraphicsItemGroup->addMethod("addToGroup",                  (q_method_t)QGRAPHICSITEMGROUP_addToGroup);
   QC_QGraphicsItemGroup->addMethod("removeFromGroup",             (q_method_t)QGRAPHICSITEMGROUP_removeFromGroup);

   return QC_QGraphicsItemGroup;
}
