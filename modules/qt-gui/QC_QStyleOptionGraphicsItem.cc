/*
 QC_QStyleOptionGraphicsItem.cc
 
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

#include "qore-qt-gui.h"

#include "QC_QStyleOptionGraphicsItem.h"
#include "QC_QStyleOption.h"
#include "QC_QRectF.h"
#include "QC_QMatrix.h"

qore_classid_t CID_QSTYLEOPTIONGRAPHICSITEM;
QoreClass *QC_QStyleOptionGraphicsItem = 0;

int QStyleOptionGraphicsItem_Notification(QoreObject *obj, QStyleOptionGraphicsItem *qsogi, const char *mem, ExceptionSink *xsink)
{
   AbstractQoreNode *p;

   if (!strcmp(mem, "exposedRect")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink || !p || p->getType() != NT_OBJECT)
	 return 0;

      QoreObject *o = reinterpret_cast<QoreObject *>(p);
      QoreQRectF *rect = (QoreQRectF *)o->getReferencedPrivateData(CID_QRECTF, xsink);
      if (!rect)
	 return 0;
      ReferenceHolder<AbstractPrivateData> rectHolder(static_cast<AbstractPrivateData *>(rect), xsink);
      qsogi->exposedRect = *(static_cast<QRectF *>(rect));      

      return 0;
   }

   if (!strcmp(mem, "levelOfDetail")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink)
	 return 0;

      qsogi->levelOfDetail = p ? p->getAsFloat() : 0.0;
      return 0;
   }

   if (!strcmp(mem, "matrix")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink || !p || p->getType() != NT_OBJECT)
	 return 0;

      QoreObject *o = reinterpret_cast<QoreObject *>(p);
      QoreQMatrix *matrix = (QoreQMatrix *)o->getReferencedPrivateData(CID_QMATRIX, xsink);
      if (!matrix)
	 return 0;
      ReferenceHolder<AbstractPrivateData> matrixHolder(static_cast<AbstractPrivateData *>(matrix), xsink);
      qsogi->matrix = *(static_cast<QMatrix *>(matrix));
      return 0;
   }

   return -1;
}

AbstractQoreNode *QStyleOptionGraphicsItem_MemberGate(QStyleOptionGraphicsItem *qsogi, const char *mem)
{
   if (!strcmp(mem, "exposedRect"))
      return_object(QC_QRectF, new QoreQRectF(qsogi->exposedRect));

   if (!strcmp(mem, "levelOfDetail"))
      return new QoreFloatNode(qsogi->levelOfDetail);

   if (!strcmp(mem, "matrix"))
      return return_object(QC_QMatrix, new QoreQMatrix(qsogi->matrix));

   return 0;
}

static AbstractQoreNode *QSTYLEOPTIONGRAPHICSITEM_memberNotification(QoreObject *self, QoreQStyleOptionGraphicsItem *qsogi, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *str = test_string_param(params, 0);
   if (!str || !str->strlen())
      return 0;

   const char *member = str->getBuffer();
   if (!QStyleOptionGraphicsItem_Notification(self, qsogi, member, xsink) || *xsink)
      return 0;

   QStyleOption_Notification(self, qsogi, member, xsink);
   return 0;
}

static AbstractQoreNode *QSTYLEOPTIONGRAPHICSITEM_memberGate(QoreObject *self, QoreQStyleOptionGraphicsItem *qsogi, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *str = test_string_param(params, 0);
   if (!str || !str->strlen())
      return 0;

   const char *member = str->getBuffer();
   AbstractQoreNode *rv = QStyleOptionGraphicsItem_MemberGate(qsogi, member);
   if (rv)
      return rv;

   return QStyleOption_MemberGate(qsogi, member);
}

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

static QoreClass *initQStyleOptionGraphicsItemClass(QoreClass *qstyleoption)
{
   QC_QStyleOptionGraphicsItem = new QoreClass("QStyleOptionGraphicsItem", QDOM_GUI);
   CID_QSTYLEOPTIONGRAPHICSITEM = QC_QStyleOptionGraphicsItem->getID();

   QC_QStyleOptionGraphicsItem->addBuiltinVirtualBaseClass(qstyleoption);

   QC_QStyleOptionGraphicsItem->setConstructor(QSTYLEOPTIONGRAPHICSITEM_constructor);
   QC_QStyleOptionGraphicsItem->setCopy((q_copy_t)QSTYLEOPTIONGRAPHICSITEM_copy);

   QC_QStyleOptionGraphicsItem->addMethod("memberNotification",  (q_method_t)QSTYLEOPTIONGRAPHICSITEM_memberNotification);
   QC_QStyleOptionGraphicsItem->addMethod("memberGate",          (q_method_t)QSTYLEOPTIONGRAPHICSITEM_memberGate);

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
