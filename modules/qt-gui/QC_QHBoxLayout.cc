/*
 QC_QHBoxLayout.cc
 
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

#include "QC_QHBoxLayout.h"
#include "QC_QFont.h"
#include "QC_QWidget.h"

qore_classid_t CID_QHBOXLAYOUT;

static void QHBOXL_constructor(class QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreQHBoxLayout *qw;
   const QoreObject *p = test_object_param(params, 0);
   QoreAbstractQWidget *parent = p ? (QoreAbstractQWidget *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;

   if (!parent)
      qw = new QoreQHBoxLayout(self);
   else 
   {
      ReferenceHolder<QoreAbstractQWidget> holder(parent, xsink);
      qw = new QoreQHBoxLayout(self, parent->getQWidget());
   }

   self->setPrivate(CID_QHBOXLAYOUT, qw);
}

static void QHBOXL_copy(class QoreObject *self, class QoreObject *old, class QoreQHBoxLayout *ql, ExceptionSink *xsink)
{
   xsink->raiseException("QHBOXLAYOUT-COPY-ERROR", "objects of this class cannot be copied");
}

class QoreClass *initQHBoxLayoutClass(class QoreClass *qboxlayout)
{
   QORE_TRACE("initQHBoxLayoutClass()");
   
   class QoreClass *QC_QHBoxLayout = new QoreClass("QHBoxLayout", QDOM_GUI);
   CID_QHBOXLAYOUT = QC_QHBoxLayout->getID();

   QC_QHBoxLayout->addBuiltinVirtualBaseClass(qboxlayout);

   QC_QHBoxLayout->setConstructor(QHBOXL_constructor);
   QC_QHBoxLayout->setCopy((q_copy_t)QHBOXL_copy);


   return QC_QHBoxLayout;
}
