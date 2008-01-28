/*
 QC_QPlastiqueStyle.cc
 
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

#include "QC_QPlastiqueStyle.h"

int CID_QPLASTIQUESTYLE;
class QoreClass *QC_QPlastiqueStyle = 0;

//QPlastiqueStyle ()
static void QPLASTIQUESTYLE_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_QPLASTIQUESTYLE, new QoreQPlastiqueStyle(self));
   return;
}

static void QPLASTIQUESTYLE_copy(class QoreObject *self, class QoreObject *old, class QoreQPlastiqueStyle *qps, ExceptionSink *xsink)
{
   xsink->raiseException("QPLASTIQUESTYLE-COPY-ERROR", "objects of this class cannot be copied");
}

QoreClass *initQPlastiqueStyleClass(QoreClass *qobject)
{
   QC_QPlastiqueStyle = new QoreClass("QPlastiqueStyle", QDOM_GUI);
   CID_QPLASTIQUESTYLE = QC_QPlastiqueStyle->getID();

   QC_QPlastiqueStyle->addBuiltinVirtualBaseClass(qobject);

   QC_QPlastiqueStyle->setConstructor(QPLASTIQUESTYLE_constructor);
   QC_QPlastiqueStyle->setCopy((q_copy_t)QPLASTIQUESTYLE_copy);


   return QC_QPlastiqueStyle;
}
