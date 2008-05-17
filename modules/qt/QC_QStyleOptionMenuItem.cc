/*
 QC_QStyleOptionMenuItem.cc
 
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

#include "qore-qt.h"

#include "QC_QStyleOptionMenuItem.h"
#include "QC_QStyleOption.h"
#include "QC_QFont.h"
#include "QC_QIcon.h"
#include "QC_QRect.h"

qore_classid_t CID_QSTYLEOPTIONMENUITEM;
QoreClass *QC_QStyleOptionMenuItem = 0;

int QStyleOptionMenuItem_Notification(QoreObject *obj, QStyleOptionMenuItem *qsomi, const char *mem, ExceptionSink *xsink)
{
   AbstractQoreNode *p;

   if (!strcmp(mem, "checkType")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink)
	 return 0;

      QStyleOptionMenuItem::CheckType checkType = (QStyleOptionMenuItem::CheckType)(p ? p->getAsInt() : 0);
      qsomi->checkType = checkType;
      return 0;
   }

   if (!strcmp(mem, "checked")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink)
	 return 0;

      bool checked = p ? p->getAsBool() : false;
      qsomi->checked = checked;
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
      qsomi->font = *(static_cast<QFont *>(font));
      return 0;
   }

   if (!strcmp(mem, "icon")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink || !p || p->getType() != NT_OBJECT)
	 return 0;

      QoreObject *o = reinterpret_cast<QoreObject *>(p);
      QoreQIcon *icon = (QoreQIcon *)o->getReferencedPrivateData(CID_QICON, xsink);
      if (!icon)
	 return 0;
      ReferenceHolder<AbstractPrivateData> iconHolder(static_cast<AbstractPrivateData *>(icon), xsink);
      qsomi->icon = *(static_cast<QIcon *>(icon));
      return 0;
   }

   if (!strcmp(mem, "maxIconWidth")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink)
	 return 0;

      int maxIconWidth = p ? p->getAsBool() : false;
      qsomi->maxIconWidth = maxIconWidth;
      return 0;
   }

   if (!strcmp(mem, "menuHasCheckableItems")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink)
	 return 0;

      bool menuHasCheckableItems = p ? p->getAsBool() : false;
      qsomi->menuHasCheckableItems = menuHasCheckableItems;
      return 0;
   }

   if (!strcmp(mem, "menuItemType")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink || !p || p->getType() != NT_OBJECT)
	 return 0;

      QStyleOptionMenuItem::MenuItemType menuItemType = (QStyleOptionMenuItem::MenuItemType)(p ? p->getAsInt() : 0);
      qsomi->menuItemType = menuItemType;
      return 0;
   }

   if (!strcmp(mem, "menuRect")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink || !p || p->getType() != NT_OBJECT)
	 return 0;

      QoreObject *o = reinterpret_cast<QoreObject *>(p);
      QoreQRect *menuRect = (QoreQRect *)o->getReferencedPrivateData(CID_QRECT, xsink);
      if (!menuRect)
	 return 0;
      ReferenceHolder<AbstractPrivateData> menuRectHolder(static_cast<AbstractPrivateData *>(menuRect), xsink);
      qsomi->menuRect = *(static_cast<QRect *>(menuRect));      

      return 0;
   }

   if (!strcmp(mem, "iconSize")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink)
	 return 0;

      qsomi->tabWidth = p ? p->getAsInt() : 0;
      return 0;
   }

   if (!strcmp(mem, "text")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink)
	 return 0;

      QString text;
      if (get_qstring(p, text, xsink))
	 return 0;
      qsomi->text = text;
      return 0;
   }

   return -1;
}

AbstractQoreNode *QStyleOptionMenuItem_MemberGate(QStyleOptionMenuItem *qsomi, const char *mem)
{
   if (!strcmp(mem, "checkType"))
      return new QoreBigIntNode(qsomi->checkType);

   if (!strcmp(mem, "checked"))
      return get_bool_node(qsomi->checked);

   if (!strcmp(mem, "font"))
      return return_object(QC_QFont, new QoreQFont(qsomi->font));

   if (!strcmp(mem, "icon"))
      return return_object(QC_QIcon, new QoreQIcon(qsomi->icon));

   if (!strcmp(mem, "maxIconWidth"))
      return new QoreBigIntNode(qsomi->maxIconWidth);

   if (!strcmp(mem, "menuHasCheckableItems"))
      return get_bool_node(qsomi->menuHasCheckableItems);

   if (!strcmp(mem, "menuItemType"))
      return new QoreBigIntNode(qsomi->menuItemType);

   if (!strcmp(mem, "menuRect"))
      return return_object(QC_QRect, new QoreQRect(qsomi->menuRect));

   if (!strcmp(mem, "tabWidth"))
      return new QoreBigIntNode(qsomi->tabWidth);

   if (!strcmp(mem, "text"))
      return new QoreStringNode(qsomi->text.toUtf8().data(), QCS_UTF8);

   return 0;
}

static AbstractQoreNode *QSTYLEOPTIONMENUITEM_memberNotification(QoreObject *self, QoreQStyleOptionMenuItem *qsomi, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *str = test_string_param(params, 0);
   if (!str || !str->strlen())
      return 0;

   const char *member = str->getBuffer();
   if (!QStyleOptionMenuItem_Notification(self, qsomi, member, xsink) || *xsink)
      return 0;

   QStyleOption_Notification(self, qsomi, member, xsink);
   return 0;
}

static AbstractQoreNode *QSTYLEOPTIONMENUITEM_memberGate(QoreObject *self, QoreQStyleOptionMenuItem *qsomi, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *str = test_string_param(params, 0);
   if (!str || !str->strlen())
      return 0;

   const char *member = str->getBuffer();
   AbstractQoreNode *rv = QStyleOptionMenuItem_MemberGate(qsomi, member);
   if (rv)
      return rv;

   return QStyleOption_MemberGate(qsomi, member);
}

//QStyleOptionMenuItem ()
//QStyleOptionMenuItem ( const QStyleOptionMenuItem & other )
static void QSTYLEOPTIONMENUITEM_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_QSTYLEOPTIONMENUITEM, new QoreQStyleOptionMenuItem());
}

static void QSTYLEOPTIONMENUITEM_copy(class QoreObject *self, class QoreObject *old, class QoreQStyleOptionMenuItem *qsomi, ExceptionSink *xsink)
{
   self->setPrivate(CID_QSTYLEOPTIONMENUITEM, new QoreQStyleOptionMenuItem(*qsomi));
}

QoreClass *initQStyleOptionMenuItemClass(QoreClass *qstyleoption)
{
   QC_QStyleOptionMenuItem = new QoreClass("QStyleOptionMenuItem", QDOM_GUI);
   CID_QSTYLEOPTIONMENUITEM = QC_QStyleOptionMenuItem->getID();

   QC_QStyleOptionMenuItem->addBuiltinVirtualBaseClass(qstyleoption);

   QC_QStyleOptionMenuItem->setConstructor(QSTYLEOPTIONMENUITEM_constructor);
   QC_QStyleOptionMenuItem->setCopy((q_copy_t)QSTYLEOPTIONMENUITEM_copy);

   // add special methods
   QC_QStyleOptionMenuItem->addMethod("memberNotification",          (q_method_t)QSTYLEOPTIONMENUITEM_memberNotification);
   QC_QStyleOptionMenuItem->addMethod("memberGate",                  (q_method_t)QSTYLEOPTIONMENUITEM_memberGate);

   return QC_QStyleOptionMenuItem;
}
