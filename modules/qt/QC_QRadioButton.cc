/*
 QC_QRadioButton.cc
 
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

#include "QC_QRadioButton.h"

int CID_QRADIOBUTTON;
class QoreClass *QC_QRadioButton = 0;

//QRadioButton ( QWidget * parent = 0 )
//QRadioButton ( const QString & text, QWidget * parent = 0 )
static void QRADIOBUTTON_constructor(Object *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QRADIOBUTTON, new QoreQRadioButton(self));
      return;      
   }

   QString text;

   bool got_text = !get_qstring(p, text, xsink, true);

   if (got_text)
      p = get_param(params, 1);

   QoreQWidget *parent = (p && p->type == NT_OBJECT) ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!parent && !is_nothing(p)) {
      if (!xsink->isException())
         xsink->raiseException("QRADIOBUTTON-CONSTRUCTOR-PARAM-ERROR", "this version of QRadioButton::constructor() expects an object derived from QWidget as the final argument", p->val.object->getClass()->getName());
      return;
   }

   ReferenceHolder<QoreQWidget> parentHolder(parent, xsink);
   if (got_text)
      self->setPrivate(CID_QRADIOBUTTON, new QoreQRadioButton(self, text, parent ? parent->getQWidget() : 0));
   else
      self->setPrivate(CID_QRADIOBUTTON, new QoreQRadioButton(self, parent ? parent->getQWidget() : 0));

   return;
}

static void QRADIOBUTTON_copy(class Object *self, class Object *old, class QoreQRadioButton *qrb, ExceptionSink *xsink)
{
   xsink->raiseException("QRADIOBUTTON-COPY-ERROR", "objects of this class cannot be copied");
}

QoreClass *initQRadioButtonClass(QoreClass *qabstractbutton)
{
   QC_QRadioButton = new QoreClass("QRadioButton", QDOM_GUI);
   CID_QRADIOBUTTON = QC_QRadioButton->getID();

   QC_QRadioButton->addBuiltinVirtualBaseClass(qabstractbutton);

   QC_QRadioButton->setConstructor(QRADIOBUTTON_constructor);
   QC_QRadioButton->setCopy((q_copy_t)QRADIOBUTTON_copy);


   return QC_QRadioButton;
}
