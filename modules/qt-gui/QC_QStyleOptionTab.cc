/*
 QC_QStyleOptionTab.cc
 
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

#include "QC_QStyleOptionTab.h"
#include "QC_QStyleOption.h"
#include "QC_QIcon.h"

qore_classid_t CID_QSTYLEOPTIONTAB;
class QoreClass *QC_QStyleOptionTab = 0;

int QStyleOptionTab_Notification(QoreObject *obj, QStyleOptionTab *qsot, const char *mem, ExceptionSink *xsink)
{
   AbstractQoreNode *p;

   if (!strcmp(mem, "cornerWidgets")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink)
	 return 0;

      QStyleOptionTab::CornerWidgets cornerWidgets = (QStyleOptionTab::CornerWidgets)(p ? p->getAsInt() : 0);
      qsot->cornerWidgets = cornerWidgets;
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
      qsot->icon = *(static_cast<QIcon *>(icon));
      return 0;
   }

   if (!strcmp(mem, "position")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink)
	 return 0;

      QStyleOptionTab::TabPosition position = (QStyleOptionTab::TabPosition)(p ? p->getAsInt() : 0);
      qsot->position = position;
      return 0;
   }

   if (!strcmp(mem, "row")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink)
	 return 0;

      int row = p ? p->getAsInt() : 0;
      qsot->row = row;
      return 0;
   }

   if (!strcmp(mem, "selectedPosition")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink)
	 return 0;

      QStyleOptionTab::SelectedPosition selectedPosition = (QStyleOptionTab::SelectedPosition)(p ? p->getAsInt() : 0);
      qsot->selectedPosition = selectedPosition;
      return 0;
   }

   if (!strcmp(mem, "shape")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink)
	 return 0;

      QTabBar::Shape shape = (QTabBar::Shape)(p ? p->getAsInt() : 0);
      qsot->shape = shape;
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
      qsot->text = text;
      return 0;
   }

   return -1;
}

AbstractQoreNode *QStyleOptionTab_MemberGate(QStyleOptionTab *qsot, const char *mem)
{
   if (!strcmp(mem, "cornerWidgets"))
      return new QoreBigIntNode(qsot->cornerWidgets);

   if (!strcmp(mem, "icon"))
      return return_object(QC_QIcon, new QoreQIcon(qsot->icon));

   if (!strcmp(mem, "position"))
      return new QoreBigIntNode(qsot->position);

   if (!strcmp(mem, "row"))
      return new QoreBigIntNode(qsot->row);

   if (!strcmp(mem, "selectedPosition"))
      return new QoreBigIntNode(qsot->selectedPosition);

   if (!strcmp(mem, "shape"))
      return new QoreBigIntNode(qsot->shape);

   if (!strcmp(mem, "text"))
      return new QoreStringNode(qsot->text.toUtf8().data(), QCS_UTF8);

   return 0;
}

static AbstractQoreNode *QSTYLEOPTIONTAB_memberNotification(QoreObject *self, QoreQStyleOptionTab *qsot, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *str = test_string_param(params, 0);
   if (!str || !str->strlen())
      return 0;

   const char *member = str->getBuffer();
   if (!QStyleOptionTab_Notification(self, qsot, member, xsink) || *xsink)
      return 0;

   QStyleOption_Notification(self, qsot, member, xsink);
   return 0;
}

static AbstractQoreNode *QSTYLEOPTIONTAB_memberGate(QoreObject *self, QoreQStyleOptionTab *qsot, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *str = test_string_param(params, 0);
   if (!str || !str->strlen())
      return 0;

   const char *member = str->getBuffer();
   AbstractQoreNode *rv = QStyleOptionTab_MemberGate(qsot, member);
   if (rv)
      return rv;

   return QStyleOption_MemberGate(qsot, member);
}

//QStyleOptionTab ()
//QStyleOptionTab ( const QStyleOptionTab & other )
static void QSTYLEOPTIONTAB_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_QSTYLEOPTIONTAB, new QoreQStyleOptionTab());
}

static void QSTYLEOPTIONTAB_copy(class QoreObject *self, class QoreObject *old, class QoreQStyleOptionTab *qsot, ExceptionSink *xsink)
{
   self->setPrivate(CID_QSTYLEOPTIONTAB, new QoreQStyleOptionTab(*qsot));
}

QoreClass *initQStyleOptionTabClass(QoreClass *qstyleoption)
{
   QC_QStyleOptionTab = new QoreClass("QStyleOptionTab", QDOM_GUI);
   CID_QSTYLEOPTIONTAB = QC_QStyleOptionTab->getID();

   QC_QStyleOptionTab->addBuiltinVirtualBaseClass(qstyleoption);

   QC_QStyleOptionTab->setConstructor(QSTYLEOPTIONTAB_constructor);
   QC_QStyleOptionTab->setCopy((q_copy_t)QSTYLEOPTIONTAB_copy);

   // add special methods
   QC_QStyleOptionTab->addMethod("memberNotification",          (q_method_t)QSTYLEOPTIONTAB_memberNotification);
   QC_QStyleOptionTab->addMethod("memberGate",                  (q_method_t)QSTYLEOPTIONTAB_memberGate);

   return QC_QStyleOptionTab;
}
