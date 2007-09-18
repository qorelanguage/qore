/*
 QC_QCDEStyle.cc
 
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

#include "QC_QCDEStyle.h"

int CID_QCDESTYLE;
class QoreClass *QC_QCDEStyle = 0;

//QCDEStyle ( bool useHighlightCols = false )
static void QCDESTYLE_constructor(Object *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool useHighlightCols = p ? p->getAsBool() : false;
   self->setPrivate(CID_QCDESTYLE, new QoreQCDEStyle(self, useHighlightCols));
   return;
}

static void QCDESTYLE_copy(class Object *self, class Object *old, class QoreQCDEStyle *qcdes, ExceptionSink *xsink)
{
   xsink->raiseException("QCDESTYLE-COPY-ERROR", "objects of this class cannot be copied");
}

QoreClass *initQCDEStyleClass(QoreClass *qmotifstyle)
{
   QC_QCDEStyle = new QoreClass("QCDEStyle", QDOM_GUI);
   CID_QCDESTYLE = QC_QCDEStyle->getID();

   QC_QCDEStyle->addBuiltinVirtualBaseClass(qmotifstyle);

   QC_QCDEStyle->setConstructor(QCDESTYLE_constructor);
   QC_QCDEStyle->setCopy((q_copy_t)QCDESTYLE_copy);


   return QC_QCDEStyle;
}
