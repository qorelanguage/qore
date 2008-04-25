/*
 QC_QStyleOptionViewItem.cc
 
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

#include "QC_QStyleOptionViewItem.h"

qore_classid_t CID_QSTYLEOPTIONVIEWITEM;
QoreClass *QC_QStyleOptionViewItem = 0;

int QStyleOptionViewItem_Notification(QoreObject *obj, QStyleOptionViewItem *qsovi, const char *mem, ExceptionSink *xsink)
{
   AbstractQoreNode *p;

   if (!strcmp(mem, "decorationAlignment")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink)
	 return 0;

      Qt::Alignment decorationAlignment = (Qt::Alignment)(p ? p->getAsInt() : 0);
      qsovi->decorationAlignment = decorationAlignment;
      return 0;
   }

   if (!strcmp(mem, "decorationPosition")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink)
	 return 0;

      QStyleOptionViewItem::Position decorationPosition = (QStyleOptionViewItem::Position)(p ? p->getAsInt() : 0);
      qsovi->decorationPosition = decorationPosition;
      return 0;
   }

   if (!strcmp(mem, "decorationSize")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink || !p || p->getType() != NT_OBJECT)
	 return 0;

      QoreObject *o = reinterpret_cast<QoreObject *>(p);
      QoreQSize *decorationSize = (QoreQSize *)o->getReferencedPrivateData(CID_QSIZE, xsink);
      if (!decorationSize)
	 return 0;
      ReferenceHolder<AbstractPrivateData> sizeHolder(static_cast<AbstractPrivateData *>(decorationSize), xsink);
      qsovi->decorationSize = *(static_cast<QSize *>(decorationSize));
      return 0;
   }

   if (!strcmp(mem, "displayAlignment")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink)
	 return 0;

      Qt::Alignment displayAlignment = (Qt::Alignment)(p ? p->getAsInt() : 0);
      qsovi->displayAlignment = displayAlignment;
      return 0;
   }

   if (!strcmp(mem, "font")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink || !p || p->getType() != NT_OBJECT)
	 return 0;

      QoreObject *o = reinterpret_cast<QoreObject *>(p);
      QoreQFont *font = (QoreQFont *)o->getReferencedPrivateData(CID_QFONT, xsink);
      if (!font)
	 return 0;
      ReferenceHolder<AbstractPrivateData> fontHolder(static_cast<AbstractPrivateData *>(font), xsink);
      qsovi->font = *(static_cast<QFont *>(font));
      return 0;
   }

   if (!strcmp(mem, "showDecorationSelected")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink)
	 return 0;

      bool showDecorationSelected = p ? p->getAsBool() : 0;
      qsovi->showDecorationSelected = showDecorationSelected;
      return 0;
   }

   if (!strcmp(mem, "textElideMode")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink)
	 return 0;

      Qt::TextElideMode textElideMode = (Qt::TextElideMode)(p ? p->getAsInt() : 0);
      qsovi->textElideMode = textElideMode;
      return 0;
   }

   return -1;
}

AbstractQoreNode *QStyleOptionViewItem_MemberGate(QStyleOptionViewItem *qsovi, const char *mem)
{
   if (!strcmp(mem, "decorationAlignment"))
      return new QoreBigIntNode(qsovi->decorationAlignment);

   if (!strcmp(mem, "decorationPosition"))
      return new QoreBigIntNode(qsovi->decorationPosition);

   if (!strcmp(mem, "decorationSize"))
      return return_object(QC_QSize, new QoreQSize(qsovi->decorationSize));

   if (!strcmp(mem, "displayAlignment"))
      return new QoreBigIntNode(qsovi->displayAlignment);

   if (!strcmp(mem, "font"))
      return return_object(QC_QFont, new QoreQFont(qsovi->font));

   if (!strcmp(mem, "showDecorationSelected"))
      return get_bool_node(qsovi->showDecorationSelected);

   if (!strcmp(mem, "textElideMode"))
      return new QoreBigIntNode(qsovi->textElideMode);

   return 0;
}

static AbstractQoreNode *QSTYLEOPTIONVIEWITEM_memberNotification(QoreObject *self, QoreQStyleOptionViewItem *qsovi, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *str = test_string_param(params, 0);
   if (!str || !str->strlen())
      return 0;

   const char *member = str->getBuffer();
   if (!QStyleOptionViewItem_Notification(self, qsovi, member, xsink) || *xsink)
      return 0;

   QStyleOption_Notification(self, qsovi, member, xsink);
   return 0;
}

static AbstractQoreNode *QSTYLEOPTIONVIEWITEM_memberGate(QoreObject *self, QoreQStyleOptionViewItem *qsovi, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *str = test_string_param(params, 0);
   if (!str || !str->strlen())
      return 0;

   const char *member = str->getBuffer();
   AbstractQoreNode *rv = QStyleOptionViewItem_MemberGate(qsovi, member);
   if (rv)
      return rv;

   return QStyleOption_MemberGate(qsovi, member);
}

//QStyleOptionViewItem ()
//QStyleOptionViewItem ( const QStyleOptionViewItem & other )
static void QSTYLEOPTIONVIEWITEM_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_QSTYLEOPTIONVIEWITEM, new QoreQStyleOptionViewItem());
}

static void QSTYLEOPTIONVIEWITEM_copy(class QoreObject *self, class QoreObject *old, class QoreQStyleOptionViewItem *qsovi, ExceptionSink *xsink)
{
   self->setPrivate(CID_QSTYLEOPTIONVIEWITEM, new QoreQStyleOptionViewItem(*qsovi));
}

QoreClass *initQStyleOptionViewItemClass(QoreClass *qstyleoption)
{
   QC_QStyleOptionViewItem = new QoreClass("QStyleOptionViewItem", QDOM_GUI);
   CID_QSTYLEOPTIONVIEWITEM = QC_QStyleOptionViewItem->getID();

   QC_QStyleOptionViewItem->addBuiltinVirtualBaseClass(qstyleoption);

   QC_QStyleOptionViewItem->setConstructor(QSTYLEOPTIONVIEWITEM_constructor);
   QC_QStyleOptionViewItem->setCopy((q_copy_t)QSTYLEOPTIONVIEWITEM_copy);

   // add special methods
   QC_QStyleOptionViewItem->addMethod("memberNotification",          (q_method_t)QSTYLEOPTIONVIEWITEM_memberNotification);
   QC_QStyleOptionViewItem->addMethod("memberGate",                  (q_method_t)QSTYLEOPTIONVIEWITEM_memberGate);

   return QC_QStyleOptionViewItem;
}
