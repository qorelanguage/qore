/*
 QC_QWindowsXPStyle.cc
 
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

#include "QC_QWindowsXPStyle.h"

int CID_QWINDOWSXPSTYLE;
class QoreClass *QC_QWindowsXPStyle = 0;

//QWindowsXPStyle ()
static void QWINDOWSXPSTYLE_constructor(QoreObject *self, const QoreNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_QWINDOWSXPSTYLE, new QoreQWindowsXPStyle(self));
   return;
}

static void QWINDOWSXPSTYLE_copy(class QoreObject *self, class QoreObject *old, class QoreQWindowsXPStyle *qwxps, ExceptionSink *xsink)
{
   xsink->raiseException("QWINDOWSXPSTYLE-COPY-ERROR", "objects of this class cannot be copied");
}

QoreClass *initQWindowsXPStyleClass(QoreClass *qobject)
{
   QC_QWindowsXPStyle = new QoreClass("QWindowsXPStyle", QDOM_GUI);
   CID_QWINDOWSXPSTYLE = QC_QWindowsXPStyle->getID();

   QC_QWindowsXPStyle->addBuiltinVirtualBaseClass(qobject);

   QC_QWindowsXPStyle->setConstructor(QWINDOWSXPSTYLE_constructor);
   QC_QWindowsXPStyle->setCopy((q_copy_t)QWINDOWSXPSTYLE_copy);


   return QC_QWindowsXPStyle;
}
