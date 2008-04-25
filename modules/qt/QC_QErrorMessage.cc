/*
 QC_QErrorMessage.cc
 
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

#include "QC_QErrorMessage.h"

int CID_QERRORMESSAGE;
class QoreClass *QC_QErrorMessage = 0;

//QErrorMessage ( QWidget * parent = 0 )
static void QERRORMESSAGE_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQWidget *parent = (p && p->getType() == NT_OBJECT) ? (QoreQWidget *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return;
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   self->setPrivate(CID_QERRORMESSAGE, new QoreQErrorMessage(self, parent ? static_cast<QWidget *>(parent->getQWidget()) : 0));
   return;
}

static void QERRORMESSAGE_copy(QoreObject *self, QoreObject *old, QoreQErrorMessage *qem, ExceptionSink *xsink)
{
   xsink->raiseException("QERRORMESSAGE-COPY-ERROR", "objects of this class cannot be copied");
}

//void showMessage ( const QString & message )
static AbstractQoreNode *QERRORMESSAGE_showMessage(QoreObject *self, QoreQErrorMessage *qem, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QString message;
   if (get_qstring(p, message, xsink))
      return 0;
   qem->qobj->showMessage(message);
   return 0;
}

QoreClass *initQErrorMessageClass(QoreClass *qdialog)
{
   QC_QErrorMessage = new QoreClass("QErrorMessage", QDOM_GUI);
   CID_QERRORMESSAGE = QC_QErrorMessage->getID();

   QC_QErrorMessage->addBuiltinVirtualBaseClass(qdialog);

   QC_QErrorMessage->setConstructor(QERRORMESSAGE_constructor);
   QC_QErrorMessage->setCopy((q_copy_t)QERRORMESSAGE_copy);

   QC_QErrorMessage->addMethod("showMessage",                 (q_method_t)QERRORMESSAGE_showMessage);

   return QC_QErrorMessage;
}
