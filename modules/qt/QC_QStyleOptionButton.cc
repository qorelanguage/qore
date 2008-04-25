/*
 QC_QStyleOptionButton.cc
 
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

#include "QC_QStyleOptionButton.h"
#include "QC_QIcon.h"
#include "QC_QSize.h"

#include "qore-qt.h"

qore_classid_t CID_QSTYLEOPTIONBUTTON;
QoreClass *QC_QStyleOptionButton = 0;

int QStyleOptionButton_Notification(QoreObject *obj, QStyleOptionButton *qsob, const char *mem, ExceptionSink *xsink)
{
   AbstractQoreNode *p;

   if (!strcmp(mem, "features")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink)
	 return 0;

      QStyleOptionButton::ButtonFeatures features = (QStyleOptionButton::ButtonFeatures)(p ? p->getAsInt() : 0);
      qsob->features = features;
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
      qsob->icon = *(static_cast<QIcon *>(icon));
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
      qsob->iconSize = *(static_cast<QSize *>(size));
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
      qsob->text = text;
      return 0;
   }

   return -1;
}

AbstractQoreNode *QStyleOptionButton_MemberGate(QStyleOptionButton *qsob, const char *mem)
{
   if (!strcmp(mem, "features"))
      return new QoreBigIntNode(qsob->features);

   if (!strcmp(mem, "icon"))
      return return_object(QC_QIcon, new QoreQIcon(qsob->icon));

   if (!strcmp(mem, "iconSize")) {
      QoreObject *o_qs = new QoreObject(QC_QSize, getProgram());
      QoreQSize *q_qs = new QoreQSize(qsob->iconSize);
      o_qs->setPrivate(CID_QSIZE, q_qs);
      return o_qs;
   }

   if (!strcmp(mem, "text"))
      return new QoreStringNode(qsob->text.toUtf8().data(), QCS_UTF8);

   return 0;
}

static AbstractQoreNode *QSTYLEOPTIONBUTTON_memberNotification(QoreObject *self, QoreQStyleOptionButton *qsob, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *str = test_string_param(params, 0);
   if (!str || !str->strlen())
      return 0;

   const char *member = str->getBuffer();
   if (!QStyleOptionButton_Notification(self, qsob, member, xsink) || *xsink)
      return 0;

   QStyleOption_Notification(self, qsob, member, xsink);
   return 0;
}

static AbstractQoreNode *QSTYLEOPTIONBUTTON_memberGate(QoreObject *self, QoreQStyleOptionButton *qsob, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *str = test_string_param(params, 0);
   if (!str || !str->strlen())
      return 0;

   const char *member = str->getBuffer();
   AbstractQoreNode *rv = QStyleOptionButton_MemberGate(qsob, member);
   if (rv)
      return rv;

   return QStyleOption_MemberGate(qsob, member);
}

//QStyleOptionButton ()
//QStyleOptionButton ( const QStyleOptionButton & other )
static void QSTYLEOPTIONBUTTON_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_QSTYLEOPTIONBUTTON, new QoreQStyleOptionButton());
}

static void QSTYLEOPTIONBUTTON_copy(class QoreObject *self, class QoreObject *old, class QoreQStyleOptionButton *qsob, ExceptionSink *xsink)
{
   self->setPrivate(CID_QSTYLEOPTIONBUTTON, new QoreQStyleOptionButton(*qsob));
}

static QoreClass *initQStyleOptionButtonClass(QoreClass *qstyleoption)
{
   QC_QStyleOptionButton = new QoreClass("QStyleOptionButton", QDOM_GUI);
   CID_QSTYLEOPTIONBUTTON = QC_QStyleOptionButton->getID();

   QC_QStyleOptionButton->addBuiltinVirtualBaseClass(qstyleoption);

   QC_QStyleOptionButton->setConstructor(QSTYLEOPTIONBUTTON_constructor);
   QC_QStyleOptionButton->setCopy((q_copy_t)QSTYLEOPTIONBUTTON_copy);

   // add special methods
   QC_QStyleOptionButton->addMethod("memberNotification",          (q_method_t)QSTYLEOPTIONBUTTON_memberNotification);
   QC_QStyleOptionButton->addMethod("memberGate",                  (q_method_t)QSTYLEOPTIONBUTTON_memberGate);

   return QC_QStyleOptionButton;
}

QoreNamespace *initQStyleOptionButtonNS(QoreClass *qstyleoption)
{
   QoreNamespace *ns = new QoreNamespace("QStyleOptionButton");

   ns->addSystemClass(initQStyleOptionButtonClass(qstyleoption));

   // StyleOptionType enum
   ns->addConstant("Type",                     new QoreBigIntNode(QStyleOptionButton::Type));

   // StyleOptionVersion enum
   ns->addConstant("Version",                  new QoreBigIntNode(QStyleOptionButton::Version));

   // ButtonFeature enum
   ns->addConstant("None",                     new QoreBigIntNode(QStyleOptionButton::None));
   ns->addConstant("Flat",                     new QoreBigIntNode(QStyleOptionButton::Flat));
   ns->addConstant("HasMenu",                  new QoreBigIntNode(QStyleOptionButton::HasMenu));
   ns->addConstant("DefaultButton",            new QoreBigIntNode(QStyleOptionButton::DefaultButton));
   ns->addConstant("AutoDefaultButton",        new QoreBigIntNode(QStyleOptionButton::AutoDefaultButton));
   ns->addConstant("CommandLinkButton",        new QoreBigIntNode(QStyleOptionButton::CommandLinkButton));

   return ns;
}
