/*
 QC_QStyleOptionGraphicsItem.cc
 
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

#include "QC_QStyleOptionGraphicsItem.h"

qore_classid_t CID_QSTYLEOPTIONGRAPHICSITEM;
QoreClass *QC_QStyleOptionGraphicsItem = 0;

//QStyleOptionGraphicsItem ()
//QStyleOptionGraphicsItem ( const QStyleOptionGraphicsItem & other )
static void QSTYLEOPTIONGRAPHICSITEM_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_QSTYLEOPTIONGRAPHICSITEM, new QoreQStyleOptionGraphicsItem());
   return;
}

static void QSTYLEOPTIONGRAPHICSITEM_copy(QoreObject *self, QoreObject *old, QoreQStyleOptionGraphicsItem *qsogi, ExceptionSink *xsink)
{
   xsink->raiseException("QSTYLEOPTIONGRAPHICSITEM-COPY-ERROR", "objects of this class cannot be copied");
}

//QRectF exposedRect ()
static AbstractQoreNode *QSTYLEOPTIONGRAPHICSITEM_exposedRect(QoreObject *self, QoreQStyleOptionGraphicsItem *qsogi, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QRectF, new QoreQRectF(qsogi->exposedRect));
}

//qreal levelOfDetail ()
static AbstractQoreNode *QSTYLEOPTIONGRAPHICSITEM_levelOfDetail(QoreObject *self, QoreQStyleOptionGraphicsItem *qsogi, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreFloatNode(qsogi->levelOfDetail);
}

//QMatrix matrix ()
static AbstractQoreNode *QSTYLEOPTIONGRAPHICSITEM_matrix(QoreObject *self, QoreQStyleOptionGraphicsItem *qsogi, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QMatrix, new QoreQMatrix(qsogi->matrix));
}

//void setMatrix ( QMatrix * matrix )
static AbstractQoreNode *QSTYLEOPTIONGRAPHICSITEM_setMatrix(QoreObject *self, QoreQStyleOptionGraphicsItem *qsogi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQMatrix *matrix = (p && p->getType() == NT_OBJECT) ? (QoreQMatrix *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QMATRIX, xsink) : 0;
   if (!matrix) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLEOPTIONGRAPHICSITEM-SETMATRIX-PARAM-ERROR", "expecting a QMatrix object as first argument to QStyleOptionGraphicsItem::setMatrix()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> matrixHolder(static_cast<AbstractPrivateData *>(matrix), xsink);
   qsogi->matrix = *(static_cast<QMatrix *>(matrix));
   return 0;
}

//void setExposedRect ( const QRectF & rect )
static AbstractQoreNode *QSTYLEOPTIONGRAPHICSITEM_setExposedRect(QoreObject *self, QoreQStyleOptionGraphicsItem *qsogi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQRectF *rect = (p && p->getType() == NT_OBJECT) ? (QoreQRectF *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QRECTF, xsink) : 0;
   if (!rect) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLEOPTIONGRAPHICSITEM-SETEXPOSEDRECT-PARAM-ERROR", "expecting a QRectF object as first argument to QStyleOptionGraphicsItem::setExposedRect()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> rectHolder(static_cast<AbstractPrivateData *>(rect), xsink);
   qsogi->exposedRect = *(static_cast<QRectF *>(rect));
   return 0;
}

//void setLevelOfDetail ( qreal level )
static AbstractQoreNode *QSTYLEOPTIONGRAPHICSITEM_setLevelOfDetail(QoreObject *self, QoreQStyleOptionGraphicsItem *qsogi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   qreal level = p ? p->getAsFloat() : 0.0;
   qsogi->levelOfDetail = level;
   return 0;
}

static QoreClass *initQStyleOptionGraphicsItemClass(QoreClass *qstyleoption)
{
   QC_QStyleOptionGraphicsItem = new QoreClass("QStyleOptionGraphicsItem", QDOM_GUI);
   CID_QSTYLEOPTIONGRAPHICSITEM = QC_QStyleOptionGraphicsItem->getID();

   QC_QStyleOptionGraphicsItem->addBuiltinVirtualBaseClass(qstyleoption);

   QC_QStyleOptionGraphicsItem->setConstructor(QSTYLEOPTIONGRAPHICSITEM_constructor);
   QC_QStyleOptionGraphicsItem->setCopy((q_copy_t)QSTYLEOPTIONGRAPHICSITEM_copy);

   QC_QStyleOptionGraphicsItem->addMethod("exposedRect",                 (q_method_t)QSTYLEOPTIONGRAPHICSITEM_exposedRect);
   QC_QStyleOptionGraphicsItem->addMethod("levelOfDetail",               (q_method_t)QSTYLEOPTIONGRAPHICSITEM_levelOfDetail);
   QC_QStyleOptionGraphicsItem->addMethod("matrix",                      (q_method_t)QSTYLEOPTIONGRAPHICSITEM_matrix);
   QC_QStyleOptionGraphicsItem->addMethod("setMatrix",                   (q_method_t)QSTYLEOPTIONGRAPHICSITEM_setMatrix);
   QC_QStyleOptionGraphicsItem->addMethod("setExposedRect",              (q_method_t)QSTYLEOPTIONGRAPHICSITEM_setExposedRect);
   QC_QStyleOptionGraphicsItem->addMethod("setLevelOfDetail",            (q_method_t)QSTYLEOPTIONGRAPHICSITEM_setLevelOfDetail);

   return QC_QStyleOptionGraphicsItem;
}

QoreNamespace *initQStyleOptionGraphicsItemNS(QoreClass *qstyleoption)
{
   QoreNamespace *ns = new QoreNamespace("QStyleOptionGraphicsItem");
   ns->addSystemClass(initQStyleOptionGraphicsItemClass(qstyleoption));

   // Type enum
   ns->addConstant("Type",        new QoreBigIntNode(QStyleOptionGraphicsItem::Type));
   
   // Version enum
   ns->addConstant("Version",     new QoreBigIntNode(QStyleOptionGraphicsItem::Version));

   return ns;
}
