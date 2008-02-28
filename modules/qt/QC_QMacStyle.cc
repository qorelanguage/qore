/*
 QC_QMacStyle.cc
 
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

#include "QC_QMacStyle.h"

qore_classid_t CID_QMACSTYLE;
class QoreClass *QC_QMacStyle = 0;

//QMacStyle ()
static void QMACSTYLE_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_QMACSTYLE, new QoreQMacStyle(self));
   return;
}

static void QMACSTYLE_copy(class QoreObject *self, class QoreObject *old, class QoreQMacStyle *qms, ExceptionSink *xsink)
{
   xsink->raiseException("QMACSTYLE-COPY-ERROR", "objects of this class cannot be copied");
}

QoreClass *initQMacStyleClass(QoreClass *qwindowsstyle)
{
   QC_QMacStyle = new QoreClass("QMacStyle", QDOM_GUI);
   CID_QMACSTYLE = QC_QMacStyle->getID();

   QC_QMacStyle->addBuiltinVirtualBaseClass(qwindowsstyle);

   QC_QMacStyle->setConstructor(QMACSTYLE_constructor);
   QC_QMacStyle->setCopy((q_copy_t)QMACSTYLE_copy);


   return QC_QMacStyle;
}
