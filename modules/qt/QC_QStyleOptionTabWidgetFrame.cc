/*
 QC_QStyleOptionTabWidgetFrame.cc
 
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

#include "QC_QStyleOptionTabWidgetFrame.h"

qore_classid_t CID_QSTYLEOPTIONTABWIDGETFRAME;
class QoreClass *QC_QStyleOptionTabWidgetFrame = 0;

//QStyleOptionTabWidgetFrame ()
//QStyleOptionTabWidgetFrame ( const QStyleOptionTabWidgetFrame & other )
static void QSTYLEOPTIONTABWIDGETFRAME_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_QSTYLEOPTIONTABWIDGETFRAME, new QoreQStyleOptionTabWidgetFrame());
}

static void QSTYLEOPTIONTABWIDGETFRAME_copy(class QoreObject *self, class QoreObject *old, class QoreQStyleOptionTabWidgetFrame *qsotwf, ExceptionSink *xsink)
{
   self->setPrivate(CID_QSTYLEOPTIONTABWIDGETFRAME, new QoreQStyleOptionTabWidgetFrame(*qsotwf));
}

QoreClass *initQStyleOptionTabWidgetFrameClass(QoreClass *qstyleoption)
{
   QC_QStyleOptionTabWidgetFrame = new QoreClass("QStyleOptionTabWidgetFrame", QDOM_GUI);
   CID_QSTYLEOPTIONTABWIDGETFRAME = QC_QStyleOptionTabWidgetFrame->getID();

   QC_QStyleOptionTabWidgetFrame->addBuiltinVirtualBaseClass(qstyleoption);

   QC_QStyleOptionTabWidgetFrame->setConstructor(QSTYLEOPTIONTABWIDGETFRAME_constructor);
   QC_QStyleOptionTabWidgetFrame->setCopy((q_copy_t)QSTYLEOPTIONTABWIDGETFRAME_copy);


   return QC_QStyleOptionTabWidgetFrame;
}
