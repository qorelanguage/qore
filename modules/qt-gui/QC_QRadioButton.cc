/*
 QC_QRadioButton.cc
 
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

#include "QC_QRadioButton.h"
#include "QC_QWidget.h"

#include "qore-qt-gui.h"

qore_classid_t CID_QRADIOBUTTON;
class QoreClass *QC_QRadioButton = 0;

//QRadioButton ( QWidget * parent = 0 )
//QRadioButton ( const QString & text, QWidget * parent = 0 )
static void QRADIOBUTTON_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QRADIOBUTTON, new QoreQRadioButton(self));
      return;
   }
   if (p && p->getType() == NT_OBJECT) {
      QoreQWidget *parent = (QoreQWidget *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink);
      if (*xsink)
	 return;
      ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
      self->setPrivate(CID_QRADIOBUTTON, new QoreQRadioButton(self, parent ? static_cast<QWidget *>(parent->getQWidget()) : 0));
      return;
   }
   QString text;
   if (get_qstring(p, text, xsink))
      return;

   const QoreObject *o = test_object_param(params, 1);
   QoreQWidget *parent = o ? (QoreQWidget *)o->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return;
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   self->setPrivate(CID_QRADIOBUTTON, new QoreQRadioButton(self, text, parent ? static_cast<QWidget *>(parent->getQWidget()) : 0));
   return;
}

static void QRADIOBUTTON_copy(class QoreObject *self, class QoreObject *old, class QoreQRadioButton *qrb, ExceptionSink *xsink)
{
   xsink->raiseException("QRADIOBUTTON-COPY-ERROR", "objects of this class cannot be copied");
}

//void initStyleOption ( QStyleOptionButton * option ) const
/*
static AbstractQoreNode *QRADIOBUTTON_initStyleOption(QoreObject *self, QoreAbstractQRadioButton *qrb, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreQStyleOptionButton *option = p ? (QoreQStyleOptionButton *)p->getReferencedPrivateData(CID_QSTYLEOPTIONBUTTON, xsink) : 0;
   if (!option) {
      if (!xsink->isException())
         xsink->raiseException("QRADIOBUTTON-INITSTYLEOPTION-PARAM-ERROR", "expecting a QStyleOptionButton object as first argument to QRadioButton::initStyleOption()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);
   qrb->getQRadioButton()->initStyleOption(static_cast<QStyleOptionButton *>(option));
   return 0;
}
*/

QoreClass *initQRadioButtonClass(QoreClass *qwidget)
{
   QC_QRadioButton = new QoreClass("QRadioButton", QDOM_GUI);
   CID_QRADIOBUTTON = QC_QRadioButton->getID();

   QC_QRadioButton->addBuiltinVirtualBaseClass(qwidget);

   QC_QRadioButton->setConstructor(QRADIOBUTTON_constructor);
   QC_QRadioButton->setCopy((q_copy_t)QRADIOBUTTON_copy);

   // private methods
   //QC_QRadioButton->addMethod("initStyleOption",             (q_method_t)QRADIOBUTTON_initStyleOption, true);

   return QC_QRadioButton;
}
