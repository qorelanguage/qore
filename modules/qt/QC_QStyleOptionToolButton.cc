/*
 QC_QStyleOptionToolButton.cc
 
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

#include "QC_QStyleOptionToolButton.h"

qore_classid_t CID_QSTYLEOPTIONTOOLBUTTON;
QoreClass *QC_QStyleOptionToolButton = 0;

int QStyleOptionToolButton_Notification(QoreObject *obj, QStyleOptionToolButton *qsotb, const char *mem, ExceptionSink *xsink)
{
   AbstractQoreNode *p;

   if (!strcmp(mem, "arrowType")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink)
	 return 0;

      Qt::ArrowType arrowType = (Qt::ArrowType)(p ? p->getAsInt() : 0);
      qsotb->arrowType = arrowType;
      return 0;
   }

   if (!strcmp(mem, "features")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink)
	 return 0;

      QStyleOptionToolButton::ToolButtonFeatures features = (QStyleOptionToolButton::ToolButtonFeatures)(p ? p->getAsInt() : 0);
      qsotb->features = features;
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
      qsotb->font = *(static_cast<QFont *>(font));
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
      qsotb->icon = *(static_cast<QIcon *>(icon));
      return 0;
   }

   if (!strcmp(mem, "iconSize")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink || !p || p->getType() != NT_OBJECT)
	 return 0;

      QoreObject *o = reinterpret_cast<QoreObject *>(p);
      QoreQSize *size = (QoreQSize *)o->getReferencedPrivateData(CID_QSIZE, xsink);
      if (!size)
	 return 0;
      ReferenceHolder<AbstractPrivateData> sizeHolder(static_cast<AbstractPrivateData *>(size), xsink);
      qsotb->iconSize = *(static_cast<QSize *>(size));
      return 0;
   }

   if (!strcmp(mem, "pos")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink || !p || p->getType() != NT_OBJECT)
	 return 0;

      QoreObject *o = reinterpret_cast<QoreObject *>(p);
      QoreQPoint *pos = (QoreQPoint *)o->getReferencedPrivateData(CID_QPOINT, xsink);
      if (!pos)
	 return 0;
      ReferenceHolder<AbstractPrivateData> posHolder(static_cast<AbstractPrivateData *>(pos), xsink);
      qsotb->pos = *(static_cast<QPoint *>(pos));
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
      qsotb->text = text;
      return 0;
   }

   if (!strcmp(mem, "toolButtonStyle")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink)
	 return 0;

      Qt::ToolButtonStyle toolButtonStyle = (Qt::ToolButtonStyle)(p ? p->getAsInt() : 0);
      qsotb->toolButtonStyle = toolButtonStyle;
      return 0;
   }

   return -1;
}

AbstractQoreNode *QStyleOptionToolButton_MemberGate(QStyleOptionToolButton *qsotb, const char *mem)
{
   if (!strcmp(mem, "arrowType"))
      return new QoreBigIntNode(qsotb->arrowType);

   if (!strcmp(mem, "features"))
      return new QoreBigIntNode(qsotb->features);

   if (!strcmp(mem, "font"))
      return return_object(QC_QFont, new QoreQFont(qsotb->font));

   if (!strcmp(mem, "icon"))
      return return_object(QC_QIcon, new QoreQIcon(qsotb->icon));

   if (!strcmp(mem, "iconSize"))
      return return_object(QC_QSize, new QoreQSize(qsotb->iconSize));

   if (!strcmp(mem, "pos"))
      return return_object(QC_QPoint, new QoreQPoint(qsotb->pos));

   if (!strcmp(mem, "text"))
      return new QoreStringNode(qsotb->text.toUtf8().data(), QCS_UTF8);

   if (!strcmp(mem, "toolButtonStyle"))
      return new QoreBigIntNode(qsotb->toolButtonStyle);

   return 0;
}

static AbstractQoreNode *QSTYLEOPTIONTOOLBUTTON_memberNotification(QoreObject *self, QoreQStyleOptionToolButton *qsotb, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *str = test_string_param(params, 0);
   if (!str || !str->strlen())
      return 0;

   const char *member = str->getBuffer();
   if (!QStyleOptionToolButton_Notification(self, qsotb, member, xsink) || *xsink)
      return 0;

   QStyleOption_Notification(self, qsotb, member, xsink);
   return 0;
}

static AbstractQoreNode *QSTYLEOPTIONTOOLBUTTON_memberGate(QoreObject *self, QoreQStyleOptionToolButton *qsotb, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *str = test_string_param(params, 0);
   if (!str || !str->strlen())
      return 0;

   const char *member = str->getBuffer();
   AbstractQoreNode *rv = QStyleOptionToolButton_MemberGate(qsotb, member);
   if (rv)
      return rv;

   return QStyleOption_MemberGate(qsotb, member);
}

//QStyleOptionToolButton ()
//QStyleOptionToolButton ( const QStyleOptionToolButton & other )
static void QSTYLEOPTIONTOOLBUTTON_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_QSTYLEOPTIONTOOLBUTTON, new QoreQStyleOptionToolButton());
}

static void QSTYLEOPTIONTOOLBUTTON_copy(class QoreObject *self, class QoreObject *old, class QoreQStyleOptionToolButton *qsotb, ExceptionSink *xsink)
{
   self->setPrivate(CID_QSTYLEOPTIONTOOLBUTTON, new QoreQStyleOptionToolButton(*qsotb));
}

QoreClass *initQStyleOptionToolButtonClass(QoreClass *qstyleoptioncomplex)
{
   QC_QStyleOptionToolButton = new QoreClass("QStyleOptionToolButton", QDOM_GUI);
   CID_QSTYLEOPTIONTOOLBUTTON = QC_QStyleOptionToolButton->getID();

   QC_QStyleOptionToolButton->addBuiltinVirtualBaseClass(qstyleoptioncomplex);

   QC_QStyleOptionToolButton->setConstructor(QSTYLEOPTIONTOOLBUTTON_constructor);
   QC_QStyleOptionToolButton->setCopy((q_copy_t)QSTYLEOPTIONTOOLBUTTON_copy);

   // add special methods
   QC_QStyleOptionToolButton->addMethod("memberNotification",   (q_method_t)QSTYLEOPTIONTOOLBUTTON_memberNotification);
   QC_QStyleOptionToolButton->addMethod("memberGate",           (q_method_t)QSTYLEOPTIONTOOLBUTTON_memberGate);

   return QC_QStyleOptionToolButton;
}
