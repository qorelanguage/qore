/*
 QC_QTimeEdit.cc
 
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

#include "QC_QTimeEdit.h"
#include "QC_QWidget.h"

#include "qore-qt.h"

int CID_QTIMEEDIT;
class QoreClass *QC_QTimeEdit = 0;

//QTimeEdit ( QWidget * parent = 0 )
//QTimeEdit ( const QTime & time, QWidget * parent = 0 )
static void QTIMEEDIT_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QTIMEEDIT, new QoreQTimeEdit(self));
      return;
   }

   QoreQWidget *parent = (p && p->type == NT_OBJECT) ? (QoreQWidget *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return;
   if (parent) {
      ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
      self->setPrivate(CID_QTIMEEDIT, new QoreQTimeEdit(self, parent->getQWidget()));
      return;
   }
   QTime time;
   if (get_qtime(p, time, xsink))
      return;
   p = get_param(params, 1);
   parent = (p && p->type == NT_OBJECT) ? (QoreQWidget *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return;
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   self->setPrivate(CID_QTIMEEDIT, new QoreQTimeEdit(self, time, parent ? static_cast<QWidget *>(parent->getQWidget()) : 0));
   return;
}

static void QTIMEEDIT_copy(class QoreObject *self, class QoreObject *old, class QoreQTimeEdit *qte, ExceptionSink *xsink)
{
   xsink->raiseException("QTIMEEDIT-COPY-ERROR", "objects of this class cannot be copied");
}

QoreClass *initQTimeEditClass(QoreClass *qdatetimeedit)
{
   QC_QTimeEdit = new QoreClass("QTimeEdit", QDOM_GUI);
   CID_QTIMEEDIT = QC_QTimeEdit->getID();

   QC_QTimeEdit->addBuiltinVirtualBaseClass(qdatetimeedit);

   QC_QTimeEdit->setConstructor(QTIMEEDIT_constructor);
   QC_QTimeEdit->setCopy((q_copy_t)QTIMEEDIT_copy);

   return QC_QTimeEdit;
}
