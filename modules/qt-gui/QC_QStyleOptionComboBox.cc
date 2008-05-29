/*
 QC_QStyleOptionComboBox.cc
 
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

#include "QC_QStyleOptionComboBox.h"
#include "QC_QStyleOption.h"
#include "QC_QIcon.h"
#include "QC_QSize.h"
#include "QC_QRect.h"

qore_classid_t CID_QSTYLEOPTIONCOMBOBOX;
QoreClass *QC_QStyleOptionComboBox = 0;

int QStyleOptionComboBox_Notification(QoreObject *obj, QStyleOptionComboBox *qsocb, const char *mem, ExceptionSink *xsink)
{
   AbstractQoreNode *p;

   if (!strcmp(mem, "currentIcon")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink || !p || p->getType() != NT_OBJECT)
	 return 0;

      QoreObject *o = reinterpret_cast<QoreObject *>(p);
      QoreQIcon *icon = (QoreQIcon *)o->getReferencedPrivateData(CID_QICON, xsink);
      if (!icon)
	 return 0;
      ReferenceHolder<AbstractPrivateData> iconHolder(static_cast<AbstractPrivateData *>(icon), xsink);
      qsocb->currentIcon = *(static_cast<QIcon *>(icon));
      return 0;
   }

   if (!strcmp(mem, "currentText")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink)
	 return 0;

      QString text;
      if (get_qstring(p, text, xsink))
	 return 0;
      qsocb->currentText = text;
      return 0;
   }

   if (!strcmp(mem, "editable")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink)
	 return 0;

      qsocb->editable = p ? p->getAsBool() : 0;
      return 0;
   }

   if (!strcmp(mem, "frame")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink)
	 return 0;

      qsocb->frame = p ? p->getAsBool() : 0;
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
      qsocb->iconSize = *(static_cast<QSize *>(size));
      return 0;
   }

   if (!strcmp(mem, "popupRect")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink || !p || p->getType() != NT_OBJECT)
	 return 0;

      QoreObject *o = reinterpret_cast<QoreObject *>(p);
      QoreQRect *rect = (QoreQRect *)o->getReferencedPrivateData(CID_QRECT, xsink);
      if (!rect)
	 return 0;
      ReferenceHolder<AbstractPrivateData> rectHolder(static_cast<AbstractPrivateData *>(rect), xsink);
      qsocb->popupRect = *(static_cast<QRect *>(rect));      

      return 0;
   }

   return -1;
}

AbstractQoreNode *QStyleOptionComboBox_MemberGate(QStyleOptionComboBox *qsocb, const char *mem)
{
   if (!strcmp(mem, "currentIcon")) {
      QoreObject *o_qi = new QoreObject(QC_QIcon, getProgram());
      QoreQIcon *q_qi = new QoreQIcon(qsocb->currentIcon);
      o_qi->setPrivate(CID_QICON, q_qi);
      return o_qi;
   }

   if (!strcmp(mem, "currentText"))
      return new QoreStringNode(qsocb->currentText.toUtf8().data(), QCS_UTF8);

   if (!strcmp(mem, "editable"))
      return get_bool_node(qsocb->editable);

   if (!strcmp(mem, "frame"))
      return get_bool_node(qsocb->frame);

   if (!strcmp(mem, "iconSize")) {
      QoreObject *o_qs = new QoreObject(QC_QSize, getProgram());
      QoreQSize *q_qs = new QoreQSize(qsocb->iconSize);
      o_qs->setPrivate(CID_QSIZE, q_qs);
      return o_qs;
   }

   if (!strcmp(mem, "popupRect")) {
      QoreObject *o_qr = new QoreObject(QC_QRect, getProgram());
      QoreQRect *q_qr = new QoreQRect(qsocb->popupRect);
      o_qr->setPrivate(CID_QRECT, q_qr);
      return o_qr;
   }

   return 0;
}

static AbstractQoreNode *QSTYLEOPTIONCOMBOBOX_memberNotification(QoreObject *self, QoreQStyleOptionComboBox *qsocb, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *str = test_string_param(params, 0);
   if (!str || !str->strlen())
      return 0;

   const char *member = str->getBuffer();
   if (!QStyleOptionComboBox_Notification(self, qsocb, member, xsink) || *xsink)
      return 0;

   QStyleOption_Notification(self, qsocb, member, xsink);
   return 0;
}

static AbstractQoreNode *QSTYLEOPTIONCOMBOBOX_memberGate(QoreObject *self, QoreQStyleOptionComboBox *qsocb, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *str = test_string_param(params, 0);
   if (!str || !str->strlen())
      return 0;

   const char *member = str->getBuffer();
   AbstractQoreNode *rv = QStyleOptionComboBox_MemberGate(qsocb, member);
   if (rv)
      return rv;

   return QStyleOption_MemberGate(qsocb, member);
}

//QStyleOptionComboBox ()
//QStyleOptionComboBox ( const QStyleOptionComboBox & other )
static void QSTYLEOPTIONCOMBOBOX_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_QSTYLEOPTIONCOMBOBOX, new QoreQStyleOptionComboBox());
}

static void QSTYLEOPTIONCOMBOBOX_copy(class QoreObject *self, class QoreObject *old, class QoreQStyleOptionComboBox *qsocb, ExceptionSink *xsink)
{
   self->setPrivate(CID_QSTYLEOPTIONCOMBOBOX, new QoreQStyleOptionComboBox(*qsocb));
}

QoreClass *initQStyleOptionComboBoxClass(QoreClass *qstyleoptioncomplex)
{
   QC_QStyleOptionComboBox = new QoreClass("QStyleOptionComboBox", QDOM_GUI);
   CID_QSTYLEOPTIONCOMBOBOX = QC_QStyleOptionComboBox->getID();

   QC_QStyleOptionComboBox->addBuiltinVirtualBaseClass(qstyleoptioncomplex);

   QC_QStyleOptionComboBox->setConstructor(QSTYLEOPTIONCOMBOBOX_constructor);
   QC_QStyleOptionComboBox->setCopy((q_copy_t)QSTYLEOPTIONCOMBOBOX_copy);

   // add other special methods
   QC_QStyleOptionComboBox->addMethod("memberNotification",  (q_method_t)QSTYLEOPTIONCOMBOBOX_memberNotification);
   QC_QStyleOptionComboBox->addMethod("memberGate",          (q_method_t)QSTYLEOPTIONCOMBOBOX_memberGate);

   return QC_QStyleOptionComboBox;
}
