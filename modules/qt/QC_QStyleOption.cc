/*
 QC_QStyleOption.cc
 
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

#include "QC_QStyleOption.h"

int CID_QSTYLEOPTION;
class QoreClass *QC_QStyleOption = 0;

//QStyleOption ( int version = QStyleOption::Version, int type = SO_Default )
//QStyleOption ( const QStyleOption & other )
static void QSTYLEOPTION_constructor(Object *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QSTYLEOPTION, new QoreQStyleOption());
      return;
   }
   int version = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int type = p ? p->getAsInt() : 0;
   self->setPrivate(CID_QSTYLEOPTION, new QoreQStyleOption(version, type));
   return;
}

static void QSTYLEOPTION_copy(class Object *self, class Object *old, class QoreQStyleOption *qso, ExceptionSink *xsink)
{
   xsink->raiseException("QSTYLEOPTION-COPY-ERROR", "objects of this class cannot be copied");
}

//void initFrom ( const QWidget * widget )
static QoreNode *QSTYLEOPTION_initFrom(Object *self, QoreQStyleOption *qso, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreAbstractQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreAbstractQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!widget) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLEOPTION-INITFROM-PARAM-ERROR", "expecting a QWidget object as first argument to QStyleOption::initFrom()");
      return 0;
   }
   ReferenceHolder<QoreAbstractQWidget> widgetHolder(widget, xsink);
   qso->initFrom(widget->getQWidget());
   return 0;
}

QoreClass *initQStyleOptionClass()
{
   QC_QStyleOption = new QoreClass("QStyleOption", QDOM_GUI);
   CID_QSTYLEOPTION = QC_QStyleOption->getID();

   QC_QStyleOption->setConstructor(QSTYLEOPTION_constructor);
   QC_QStyleOption->setCopy((q_copy_t)QSTYLEOPTION_copy);

   QC_QStyleOption->addMethod("initFrom",                    (q_method_t)QSTYLEOPTION_initFrom);

   return QC_QStyleOption;
}
