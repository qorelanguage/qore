/*
 QC_QDateEdit.cc
 
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

#include "QC_QDateEdit.h"

int CID_QDATEEDIT;
class QoreClass *QC_QDateEdit = 0;

//QDateEdit ( QWidget * parent = 0 )
//QDateEdit ( const QDate & date, QWidget * parent = 0 )
static void QDATEEDIT_constructor(QoreObject *self, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QDATEEDIT, new QoreQDateEdit(self));
      return;
   }

   QoreQWidget *parent = (p->type == NT_OBJECT) ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (parent) {
      ReferenceHolder<QoreQWidget> parentHolder(parent, xsink);
      self->setPrivate(CID_QDATEEDIT, new QoreQDateEdit(self, parent->getQWidget()));
      return;
   }

   QDate date;
   if (get_qdate(p, date, xsink))
      return;
   
   QoreNode *p1 = test_param(params, NT_OBJECT, 1);
   parent = (p1 && p1->type == NT_OBJECT) ? (QoreQWidget *)p1->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   ReferenceHolder<QoreQWidget> parentHolder(parent, xsink);
   
   self->setPrivate(CID_QDATEEDIT, new QoreQDateEdit(self, date, parent ? parent->getQWidget() : 0));      
}

static void QDATEEDIT_copy(class QoreObject *self, class QoreObject *old, class QoreQDateEdit *qde, ExceptionSink *xsink)
{
   xsink->raiseException("QDATEEDIT-COPY-ERROR", "objects of this class cannot be copied");
}

QoreClass *initQDateEditClass(QoreClass *qdatetimeedit)
{
   QC_QDateEdit = new QoreClass("QDateEdit", QDOM_GUI);
   CID_QDATEEDIT = QC_QDateEdit->getID();

   QC_QDateEdit->addBuiltinVirtualBaseClass(qdatetimeedit);

   QC_QDateEdit->setConstructor(QDATEEDIT_constructor);
   QC_QDateEdit->setCopy((q_copy_t)QDATEEDIT_copy);


   return QC_QDateEdit;
}
