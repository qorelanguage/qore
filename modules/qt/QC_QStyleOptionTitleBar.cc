/*
 QC_QStyleOptionTitleBar.cc
 
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

#include "QC_QStyleOptionTitleBar.h"

qore_classid_t CID_QSTYLEOPTIONTITLEBAR;
QoreClass *QC_QStyleOptionTitleBar = 0;

int QStyleOptionTitleBar_Notification(QoreObject *obj, QStyleOptionTitleBar *qsotb, const char *mem, ExceptionSink *xsink)
{
   AbstractQoreNode *p;

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

   if (!strcmp(mem, "titleBarFlags")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink)
	 return 0;

      Qt::WindowFlags titleBarFlags = (Qt::WindowFlags)(p ? p->getAsInt() : 0);
      qsotb->titleBarFlags = titleBarFlags;
      return 0;
   }

   if (!strcmp(mem, "titleBarState")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink)
	 return 0;

      int titleBarState = p ? p->getAsInt() : 0;
      qsotb->titleBarState = titleBarState;
      return 0;
   }

   return -1;
}

AbstractQoreNode *QStyleOptionTitleBar_MemberGate(QStyleOptionTitleBar *qsotb, const char *mem)
{
   if (!strcmp(mem, "icon"))
      return return_object(QC_QIcon, new QoreQIcon(qsotb->icon));

   if (!strcmp(mem, "text"))
      return new QoreStringNode(qsotb->text.toUtf8().data(), QCS_UTF8);

   if (!strcmp(mem, "fetitleBarFlagsatures"))
      return new QoreBigIntNode(qsotb->titleBarFlags);

   if (!strcmp(mem, "titleBarState"))
      return new QoreBigIntNode(qsotb->titleBarState);

   return 0;
}

static AbstractQoreNode *QSTYLEOPTIONTITLEBAR_memberNotification(QoreObject *self, QoreQStyleOptionTitleBar *qsotb, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *str = test_string_param(params, 0);
   if (!str || !str->strlen())
      return 0;

   const char *member = str->getBuffer();
   if (!QStyleOptionTitleBar_Notification(self, qsotb, member, xsink) || *xsink)
      return 0;

   QStyleOption_Notification(self, qsotb, member, xsink);
   return 0;
}

static AbstractQoreNode *QSTYLEOPTIONTITLEBAR_memberGate(QoreObject *self, QoreQStyleOptionTitleBar *qsotb, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *str = test_string_param(params, 0);
   if (!str || !str->strlen())
      return 0;

   const char *member = str->getBuffer();
   AbstractQoreNode *rv = QStyleOptionTitleBar_MemberGate(qsotb, member);
   if (rv)
      return rv;

   return QStyleOption_MemberGate(qsotb, member);
}

//QStyleOptionTitleBar ()
//QStyleOptionTitleBar ( const QStyleOptionTitleBar & other )
static void QSTYLEOPTIONTITLEBAR_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_QSTYLEOPTIONTITLEBAR, new QoreQStyleOptionTitleBar());
}

static void QSTYLEOPTIONTITLEBAR_copy(class QoreObject *self, class QoreObject *old, class QoreQStyleOptionTitleBar *qsotb, ExceptionSink *xsink)
{
   self->setPrivate(CID_QSTYLEOPTIONTITLEBAR, new QoreQStyleOptionTitleBar(*qsotb));
}

QoreClass *initQStyleOptionTitleBarClass(QoreClass *qstyleoptioncomplex)
{
   QC_QStyleOptionTitleBar = new QoreClass("QStyleOptionTitleBar", QDOM_GUI);
   CID_QSTYLEOPTIONTITLEBAR = QC_QStyleOptionTitleBar->getID();

   QC_QStyleOptionTitleBar->addBuiltinVirtualBaseClass(qstyleoptioncomplex);

   QC_QStyleOptionTitleBar->setConstructor(QSTYLEOPTIONTITLEBAR_constructor);
   QC_QStyleOptionTitleBar->setCopy((q_copy_t)QSTYLEOPTIONTITLEBAR_copy);

   // add special methods
   QC_QStyleOptionTitleBar->addMethod("memberNotification",   (q_method_t)QSTYLEOPTIONTITLEBAR_memberNotification);
   QC_QStyleOptionTitleBar->addMethod("memberGate",           (q_method_t)QSTYLEOPTIONTITLEBAR_memberGate);

   return QC_QStyleOptionTitleBar;
}
