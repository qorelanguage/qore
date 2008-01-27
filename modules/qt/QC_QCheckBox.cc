/*
 QC_QCheckBox.cc
 
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

#include "QC_QCheckBox.h"
#include "QC_QWidget.h"

#include "qore-qt.h"

int CID_QCHECKBOX;
class QoreClass *QC_QCheckBox = 0;

//QCheckBox ( QWidget * parent = 0 )
//QCheckBox ( const QString & text, QWidget * parent = 0 )
static void QCHECKBOX_constructor(QoreObject *self, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QCHECKBOX, new QoreQCheckBox(self));
      return;
   }
   if (p && p->type == NT_OBJECT) {
      QoreQWidget *parent = (QoreQWidget *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink);
      if (*xsink)
         return;
      ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
      self->setPrivate(CID_QCHECKBOX, new QoreQCheckBox(self, parent ? static_cast<QWidget *>(parent->getQWidget()) : 0));
      return;
   }
   QString text;
   if (get_qstring(p, text, xsink))
      return;
   p = get_param(params, 1);
   QoreQWidget *parent = (p && p->type == NT_OBJECT) ? (QoreQWidget *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return;
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   self->setPrivate(CID_QCHECKBOX, new QoreQCheckBox(self, text, parent ? static_cast<QWidget *>(parent->getQWidget()) : 0));
   return;
}

static void QCHECKBOX_copy(class QoreObject *self, class QoreObject *old, class QoreQCheckBox *qcb, ExceptionSink *xsink)
{
   xsink->raiseException("QCHECKBOX-COPY-ERROR", "objects of this class cannot be copied");
}

//Qt::CheckState checkState () const
static QoreNode *QCHECKBOX_checkState(QoreObject *self, QoreAbstractQCheckBox *qcb, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qcb->getQCheckBox()->checkState());
}

//bool isTristate () const
static QoreNode *QCHECKBOX_isTristate(QoreObject *self, QoreAbstractQCheckBox *qcb, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode(qcb->getQCheckBox()->isTristate());
}

//void setCheckState ( Qt::CheckState state )
static QoreNode *QCHECKBOX_setCheckState(QoreObject *self, QoreAbstractQCheckBox *qcb, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   Qt::CheckState state = (Qt::CheckState)(p ? p->getAsInt() : 0);
   qcb->getQCheckBox()->setCheckState(state);
   return 0;
}

//void setTristate ( bool y = true )
static QoreNode *QCHECKBOX_setTristate(QoreObject *self, QoreAbstractQCheckBox *qcb, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool y = !is_nothing(p) ? p->getAsBool() : true;
   qcb->getQCheckBox()->setTristate(y);
   return 0;
}

//void initStyleOption ( QStyleOptionButton * option ) const
/*
static QoreNode *QCHECKBOX_initStyleOption(QoreObject *self, QoreAbstractQCheckBox *qcb, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQStyleOptionButton *option = (p && p->type == NT_OBJECT) ? (QoreQStyleOptionButton *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QSTYLEOPTIONBUTTON, xsink) : 0;
   if (!option) {
      if (!xsink->isException())
         xsink->raiseException("QCHECKBOX-INITSTYLEOPTION-PARAM-ERROR", "expecting a QStyleOptionButton object as first argument to QCheckBox::initStyleOption()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);
   qcb->getQCheckBox()->initStyleOption(static_cast<QStyleOptionButton *>(option));
   return 0;
}
*/

QoreClass *initQCheckBoxClass(QoreClass *qwidget)
{
   QC_QCheckBox = new QoreClass("QCheckBox", QDOM_GUI);
   CID_QCHECKBOX = QC_QCheckBox->getID();

   QC_QCheckBox->addBuiltinVirtualBaseClass(qwidget);

   QC_QCheckBox->setConstructor(QCHECKBOX_constructor);
   QC_QCheckBox->setCopy((q_copy_t)QCHECKBOX_copy);

   QC_QCheckBox->addMethod("checkState",                  (q_method_t)QCHECKBOX_checkState);
   QC_QCheckBox->addMethod("isTristate",                  (q_method_t)QCHECKBOX_isTristate);
   QC_QCheckBox->addMethod("setCheckState",               (q_method_t)QCHECKBOX_setCheckState);
   QC_QCheckBox->addMethod("setTristate",                 (q_method_t)QCHECKBOX_setTristate);

   //QC_QCheckBox->addMethod("initStyleOption",             (q_method_t)QCHECKBOX_initStyleOption, true);

   return QC_QCheckBox;
}
