/*
 QC_QCheckBox.cc
 
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

#include "QC_QCheckBox.h"

int CID_QCHECKBOX;
class QoreClass *QC_QCheckBox = 0;

//QCheckBox ( QWidget * parent = 0 )
//QCheckBox ( const QString & text, QWidget * parent = 0 )
static void QCHECKBOX_constructor(Object *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QCHECKBOX, new QoreQCheckBox(self));
      return;
   }

   const char *text = 0; 
   if (p && p->type == NT_STRING) {
      text = p->val.String->getBuffer();
      p = test_param(params, NT_OBJECT, 1);
   }

   QoreQWidget *parent = p ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   ReferenceHolder<QoreQWidget> parentHolder(parent, xsink);

   if (text)
      self->setPrivate(CID_QCHECKBOX, new QoreQCheckBox(self, text, parent ? static_cast<QWidget *>(parent->getQWidget()) : 0));
   else
      self->setPrivate(CID_QCHECKBOX, new QoreQCheckBox(self, parent ? static_cast<QWidget *>(parent->getQWidget()) : 0));
   return;
}

static void QCHECKBOX_copy(class Object *self, class Object *old, class QoreQCheckBox *qcb, ExceptionSink *xsink)
{
   xsink->raiseException("QCHECKBOX-COPY-ERROR", "objects of this class cannot be copied");
}

//Qt::CheckState checkState () const
static QoreNode *QCHECKBOX_checkState(Object *self, QoreQCheckBox *qcb, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qcb->qobj->checkState());
}

//bool isTristate () const
static QoreNode *QCHECKBOX_isTristate(Object *self, QoreQCheckBox *qcb, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qcb->qobj->isTristate());
}

//void setCheckState ( Qt::CheckState state )
static QoreNode *QCHECKBOX_setCheckState(Object *self, QoreQCheckBox *qcb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   Qt::CheckState state = (Qt::CheckState)(p ? p->getAsInt() : 0);
   qcb->qobj->setCheckState(state);
   return 0;
}

//void setTristate ( bool y = true )
static QoreNode *QCHECKBOX_setTristate(Object *self, QoreQCheckBox *qcb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool y = !is_nothing(p) ? p->getAsBool() : true;
   qcb->qobj->setTristate(y);
   return 0;
}

QoreClass *initQCheckBoxClass(QoreClass *qabstractbutton)
{
   QC_QCheckBox = new QoreClass("QCheckBox", QDOM_GUI);
   CID_QCHECKBOX = QC_QCheckBox->getID();

   QC_QCheckBox->addBuiltinVirtualBaseClass(qabstractbutton);

   QC_QCheckBox->setConstructor(QCHECKBOX_constructor);
   QC_QCheckBox->setCopy((q_copy_t)QCHECKBOX_copy);

   QC_QCheckBox->addMethod("checkState",                  (q_method_t)QCHECKBOX_checkState);
   QC_QCheckBox->addMethod("isTristate",                  (q_method_t)QCHECKBOX_isTristate);
   QC_QCheckBox->addMethod("setCheckState",               (q_method_t)QCHECKBOX_setCheckState);
   QC_QCheckBox->addMethod("setTristate",                 (q_method_t)QCHECKBOX_setTristate);

   return QC_QCheckBox;
}
