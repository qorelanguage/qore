/*
 QC_QStyleOptionGroupBox.cc
 
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

#include "QC_QStyleOptionGroupBox.h"

qore_classid_t CID_QSTYLEOPTIONGROUPBOX;
class QoreClass *QC_QStyleOptionGroupBox = 0;

int QStyleOptionGroupBox_Notification(QoreObject *obj, QStyleOptionGroupBox *qsogb, const char *mem, ExceptionSink *xsink)
{
   AbstractQoreNode *p;

   if (!strcmp(mem, "features")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink)
	 return 0;

      QStyleOptionFrameV2::FrameFeatures features = (QStyleOptionFrameV2::FrameFeatures)(p ? p->getAsInt() : 0);
      qsogb->features = features;
      return 0;
   }

   if (!strcmp(mem, "lineWidth")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink)
	 return 0;

      int lineWidth = p ? p->getAsInt() : 0;
      qsogb->lineWidth = lineWidth;
      return 0;
   }

   if (!strcmp(mem, "midLineWidth")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink)
	 return 0;

      int midLineWidth = p ? p->getAsInt() : 0;
      qsogb->midLineWidth = midLineWidth;
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
      qsogb->text = text;
      return 0;
   }

   if (!strcmp(mem, "textAlignment")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink)
	 return 0;

      Qt::Alignment textAlignment = (Qt::Alignment)(p ? p->getAsInt() : 0);
      qsogb->textAlignment = textAlignment;
      return 0;
   }

   if (!strcmp(mem, "textColor")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink || !p || p->getType() != NT_OBJECT)
	 return 0;

      QoreObject *o = reinterpret_cast<QoreObject *>(p);
      QoreQColor *textColor = (QoreQColor *)o->getReferencedPrivateData(CID_QCOLOR, xsink);
      if (!textColor)
	 return 0;
      ReferenceHolder<AbstractPrivateData> textColorHolder(static_cast<AbstractPrivateData *>(textColor), xsink);
      qsogb->textColor = *(static_cast<QColor *>(textColor));
      return 0;
   }

   return -1;
}

AbstractQoreNode *QStyleOptionGroupBox_MemberGate(QStyleOptionGroupBox *qsogb, const char *mem)
{
   if (!strcmp(mem, "features"))
      return new QoreBigIntNode(qsogb->features);

   if (!strcmp(mem, "lineWidth"))
      return new QoreBigIntNode(qsogb->lineWidth);

   if (!strcmp(mem, "midLineWidth"))
      return new QoreBigIntNode(qsogb->midLineWidth);

   if (!strcmp(mem, "text"))
      return new QoreStringNode(qsogb->text.toUtf8().data(), QCS_UTF8);

   if (!strcmp(mem, "textAlignment"))
      return new QoreBigIntNode(qsogb->textAlignment);

   if (!strcmp(mem, "textColor"))
      return_object(QC_QColor, new QoreQColor(qsogb->textColor));

   return 0;
}

static AbstractQoreNode *QSTYLEOPTIONGROUPBOX_memberNotification(QoreObject *self, QoreQStyleOptionGroupBox *qsogb, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *str = test_string_param(params, 0);
   if (!str || !str->strlen())
      return 0;

   const char *member = str->getBuffer();
   if (!QStyleOptionGroupBox_Notification(self, qsogb, member, xsink) || *xsink)
      return 0;

   QStyleOption_Notification(self, qsogb, member, xsink);
   return 0;
}

static AbstractQoreNode *QSTYLEOPTIONGROUPBOX_memberGate(QoreObject *self, QoreQStyleOptionGroupBox *qsogb, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *str = test_string_param(params, 0);
   if (!str || !str->strlen())
      return 0;

   const char *member = str->getBuffer();
   AbstractQoreNode *rv = QStyleOptionGroupBox_MemberGate(qsogb, member);
   if (rv)
      return rv;

   return QStyleOption_MemberGate(qsogb, member);
}

//QStyleOptionGroupBox ()
//QStyleOptionGroupBox ( const QStyleOptionGroupBox & other )
static void QSTYLEOPTIONGROUPBOX_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_QSTYLEOPTIONGROUPBOX, new QoreQStyleOptionGroupBox());
}

static void QSTYLEOPTIONGROUPBOX_copy(class QoreObject *self, class QoreObject *old, class QoreQStyleOptionGroupBox *qsogb, ExceptionSink *xsink)
{
   self->setPrivate(CID_QSTYLEOPTIONGROUPBOX, new QoreQStyleOptionGroupBox(*qsogb));
}

QoreClass *initQStyleOptionGroupBoxClass(QoreClass *qstyleoptioncomplex)
{
   QC_QStyleOptionGroupBox = new QoreClass("QStyleOptionGroupBox", QDOM_GUI);
   CID_QSTYLEOPTIONGROUPBOX = QC_QStyleOptionGroupBox->getID();

   QC_QStyleOptionGroupBox->addBuiltinVirtualBaseClass(qstyleoptioncomplex);

   QC_QStyleOptionGroupBox->setConstructor(QSTYLEOPTIONGROUPBOX_constructor);
   QC_QStyleOptionGroupBox->setCopy((q_copy_t)QSTYLEOPTIONGROUPBOX_copy);

   // add special methods
   QC_QStyleOptionGroupBox->addMethod("memberNotification",  (q_method_t)QSTYLEOPTIONGROUPBOX_memberNotification);
   QC_QStyleOptionGroupBox->addMethod("memberGate",          (q_method_t)QSTYLEOPTIONGROUPBOX_memberGate);

   return QC_QStyleOptionGroupBox;
}
