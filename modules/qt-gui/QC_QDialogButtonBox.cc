/*
 QC_QDialogButtonBox.cc
 
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

#include "QC_QDialogButtonBox.h"
#include "QC_QWidget.h"
#include "QC_QAbstractButton.h"
#include "QC_QPushButton.h"

#include "qore-qt-gui.h"

qore_classid_t CID_QDIALOGBUTTONBOX;
class QoreClass *QC_QDialogButtonBox = 0;

//QDialogButtonBox ( QWidget * parent = 0 )
//QDialogButtonBox ( Qt::Orientation orientation, QWidget * parent = 0 )
//QDialogButtonBox ( StandardButtons buttons, Qt::Orientation orientation = Qt::Horizontal, QWidget * parent = 0 )
static void QDIALOGBUTTONBOX_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QDIALOGBUTTONBOX, new QoreQDialogButtonBox(self));
      return;
   }
   if (p && p->getType() == NT_OBJECT) {
      QoreQWidget *parent = (QoreQWidget *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink);
      if (*xsink)
         return;
      ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
      self->setPrivate(CID_QDIALOGBUTTONBOX, new QoreQDialogButtonBox(self, parent ? static_cast<QWidget *>(parent->getQWidget()) : 0));
      return;
   }
   int arg = p ? p->getAsInt() : 0;
   if (num_params(params) == 1) { // differentiate between StandardButton and Qt::Orientation
      if (arg > 0 && arg < 100)
	 self->setPrivate(CID_QDIALOGBUTTONBOX, new QoreQDialogButtonBox(self, (Qt::Orientation)arg));
      else
	 self->setPrivate(CID_QDIALOGBUTTONBOX, new QoreQDialogButtonBox(self, (QDialogButtonBox::StandardButton)arg));
      return;
   }
   p = get_param(params, 1);
   if (num_params(params) == 2) {
      if (p && p->getType() == NT_OBJECT) {
	 QoreQWidget *parent = (QoreQWidget *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink);
	 if (*xsink)
	    return;
	 if (!parent) {
	    xsink->raiseException("QDIALOGBUTTONBOX-CONSTRUCTOR-ERROR", "expecting an object derived from QWidget as ssecond argument of this version of QDialogButtonBox::constructor(), got class '%s' instead", (reinterpret_cast<const QoreObject *>(p))->getClassName());
	    return;
	 }
	 ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
	 self->setPrivate(CID_QDIALOGBUTTONBOX, new QoreQDialogButtonBox(self, (Qt::Orientation)arg, parent->getQWidget()));
      }
      else {
	 Qt::Orientation orientation = !is_nothing(p) ? (Qt::Orientation)p->getAsInt() : Qt::Horizontal;
	 self->setPrivate(CID_QDIALOGBUTTONBOX, new QoreQDialogButtonBox(self, (QDialogButtonBox::StandardButton)arg, orientation));
      }
      return;
   }
   QDialogButtonBox::StandardButtons buttons = (QDialogButtonBox::StandardButtons)arg;
   Qt::Orientation orientation = !is_nothing(p) ? (Qt::Orientation)p->getAsInt() : Qt::Horizontal;

   const QoreObject *o = test_object_param(params, 2);
   QoreQWidget *parent = o ? (QoreQWidget *)o->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return;
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   self->setPrivate(CID_QDIALOGBUTTONBOX, new QoreQDialogButtonBox(self, buttons, orientation, parent ? static_cast<QWidget *>(parent->getQWidget()) : 0));
}

static void QDIALOGBUTTONBOX_copy(class QoreObject *self, class QoreObject *old, class QoreQDialogButtonBox *qdbb, ExceptionSink *xsink)
{
   xsink->raiseException("QDIALOGBUTTONBOX-COPY-ERROR", "objects of this class cannot be copied");
}

//void addButton ( QAbstractButton * button, ButtonRole role )
//QPushButton * addButton ( const QString & text, ButtonRole role )
//QPushButton * addButton ( StandardButton button )
static AbstractQoreNode *QDIALOGBUTTONBOX_addButton(QoreObject *self, QoreQDialogButtonBox *qdbb, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (p && p->getType() == NT_OBJECT) {
      QoreQAbstractButton *button = (QoreQAbstractButton *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QABSTRACTBUTTON, xsink);
      if (!button) {
         if (!xsink->isException())
            xsink->raiseException("QDIALOGBUTTONBOX-ADDBUTTON-PARAM-ERROR", "QDialogButtonBox::addButton() does not know how to handle arguments of class '%s' as passed as the first argument", (reinterpret_cast<const QoreObject *>(p))->getClassName());
         return 0;
      }
      ReferenceHolder<AbstractPrivateData> buttonHolder(static_cast<AbstractPrivateData *>(button), xsink);
      p = get_param(params, 1);
      QDialogButtonBox::ButtonRole role = (QDialogButtonBox::ButtonRole)(p ? p->getAsInt() : 0);
      qdbb->qobj->addButton(static_cast<QAbstractButton *>(button->qobj), role);
      return 0;
   }
   QPushButton *qt_qobj;
   if (p && p->getType() == NT_STRING) {
      QString text;
      if (get_qstring(p, text, xsink))
         return 0;
      p = get_param(params, 1);
      QDialogButtonBox::ButtonRole role = (QDialogButtonBox::ButtonRole)(p ? p->getAsInt() : 0);
      qt_qobj = qdbb->qobj->addButton(text, role);
   }
   else {
      QDialogButtonBox::StandardButton button = (QDialogButtonBox::StandardButton)(p ? p->getAsInt() : 0);
      qt_qobj = qdbb->qobj->addButton(button);
   }

   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   QoreObject *rv_obj = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   if (rv_obj)
      rv_obj->ref();
   else {
      rv_obj = new QoreObject(QC_QPushButton, getProgram());
      QoreQtQPushButton *t_qobj = new QoreQtQPushButton(rv_obj, qt_qobj);
      rv_obj->setPrivate(CID_QPUSHBUTTON, t_qobj);
   }
   return rv_obj;
}

//QPushButton * button ( StandardButton which ) const
static AbstractQoreNode *QDIALOGBUTTONBOX_button(QoreObject *self, QoreQDialogButtonBox *qdbb, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QDialogButtonBox::StandardButton which = (QDialogButtonBox::StandardButton)(p ? p->getAsInt() : 0);
   QPushButton *qt_qobj = qdbb->qobj->button(which);
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   QoreObject *rv_obj = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   if (rv_obj)
      rv_obj->ref();
   else {
      rv_obj = new QoreObject(QC_QPushButton, getProgram());
      QoreQtQPushButton *t_qobj = new QoreQtQPushButton(rv_obj, qt_qobj);
      rv_obj->setPrivate(CID_QPUSHBUTTON, t_qobj);
   }
   return rv_obj;
}

//ButtonRole buttonRole ( QAbstractButton * button ) const
static AbstractQoreNode *QDIALOGBUTTONBOX_buttonRole(QoreObject *self, QoreQDialogButtonBox *qdbb, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreQAbstractButton *button = p ? (QoreQAbstractButton *)p->getReferencedPrivateData(CID_QABSTRACTBUTTON, xsink) : 0;
   if (!button) {
      if (!xsink->isException())
         xsink->raiseException("QDIALOGBUTTONBOX-BUTTONROLE-PARAM-ERROR", "expecting a QAbstractButton object as first argument to QDialogButtonBox::buttonRole()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> buttonHolder(static_cast<AbstractPrivateData *>(button), xsink);
   return new QoreBigIntNode(qdbb->qobj->buttonRole(static_cast<QAbstractButton *>(button->getQAbstractButton())));
}

//QList<QAbstractButton *> buttons () const
static AbstractQoreNode *QDIALOGBUTTONBOX_buttons(QoreObject *self, QoreQDialogButtonBox *qdbb, const QoreListNode *params, ExceptionSink *xsink)
{
   QList<QAbstractButton *> l = qdbb->qobj->buttons();
   QoreListNode *ql = new QoreListNode();
   for (QList<QAbstractButton *>::iterator i = l.begin(), e = l.end(); i != e; ++i) {
      ql->push(return_qabstractbutton(*i));
   }
   return ql;
}

//bool centerButtons () const
static AbstractQoreNode *QDIALOGBUTTONBOX_centerButtons(QoreObject *self, QoreQDialogButtonBox *qdbb, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qdbb->qobj->centerButtons());
}

//void clear ()
static AbstractQoreNode *QDIALOGBUTTONBOX_clear(QoreObject *self, QoreQDialogButtonBox *qdbb, const QoreListNode *params, ExceptionSink *xsink)
{
   qdbb->qobj->clear();
   return 0;
}

//Qt::Orientation orientation () const
static AbstractQoreNode *QDIALOGBUTTONBOX_orientation(QoreObject *self, QoreQDialogButtonBox *qdbb, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qdbb->qobj->orientation());
}

//void removeButton ( QAbstractButton * button )
static AbstractQoreNode *QDIALOGBUTTONBOX_removeButton(QoreObject *self, QoreQDialogButtonBox *qdbb, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreQAbstractButton *button = p ? (QoreQAbstractButton *)p->getReferencedPrivateData(CID_QABSTRACTBUTTON, xsink) : 0;
   if (!button) {
      if (!xsink->isException())
         xsink->raiseException("QDIALOGBUTTONBOX-REMOVEBUTTON-PARAM-ERROR", "expecting a QAbstractButton object as first argument to QDialogButtonBox::removeButton()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> buttonHolder(static_cast<AbstractPrivateData *>(button), xsink);
   qdbb->qobj->removeButton(static_cast<QAbstractButton *>(button->qobj));
   return 0;
}

//void setCenterButtons ( bool center )
static AbstractQoreNode *QDIALOGBUTTONBOX_setCenterButtons(QoreObject *self, QoreQDialogButtonBox *qdbb, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   bool center = p ? p->getAsBool() : false;
   qdbb->qobj->setCenterButtons(center);
   return 0;
}

//void setOrientation ( Qt::Orientation orientation )
static AbstractQoreNode *QDIALOGBUTTONBOX_setOrientation(QoreObject *self, QoreQDialogButtonBox *qdbb, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   Qt::Orientation orientation = (Qt::Orientation)(p ? p->getAsInt() : 0);
   qdbb->qobj->setOrientation(orientation);
   return 0;
}

//void setStandardButtons ( StandardButtons buttons )
static AbstractQoreNode *QDIALOGBUTTONBOX_setStandardButtons(QoreObject *self, QoreQDialogButtonBox *qdbb, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QDialogButtonBox::StandardButtons buttons = (QDialogButtonBox::StandardButtons)(p ? p->getAsInt() : 0);
   qdbb->qobj->setStandardButtons(buttons);
   return 0;
}

//StandardButton standardButton ( QAbstractButton * button ) const
static AbstractQoreNode *QDIALOGBUTTONBOX_standardButton(QoreObject *self, QoreQDialogButtonBox *qdbb, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreQAbstractButton *button = p ? (QoreQAbstractButton *)p->getReferencedPrivateData(CID_QABSTRACTBUTTON, xsink) : 0;
   if (!button) {
      if (!xsink->isException())
         xsink->raiseException("QDIALOGBUTTONBOX-STANDARDBUTTON-PARAM-ERROR", "expecting a QAbstractButton object as first argument to QDialogButtonBox::standardButton()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> buttonHolder(static_cast<AbstractPrivateData *>(button), xsink);
   return new QoreBigIntNode(qdbb->qobj->standardButton(static_cast<QAbstractButton *>(button->getQAbstractButton())));
}

//StandardButtons standardButtons () const
static AbstractQoreNode *QDIALOGBUTTONBOX_standardButtons(QoreObject *self, QoreQDialogButtonBox *qdbb, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qdbb->qobj->standardButtons());
}

static QoreClass *initQDialogButtonBoxClass(QoreClass *qwidget)
{
   QC_QDialogButtonBox = new QoreClass("QDialogButtonBox", QDOM_GUI);
   CID_QDIALOGBUTTONBOX = QC_QDialogButtonBox->getID();

   QC_QDialogButtonBox->addBuiltinVirtualBaseClass(qwidget);

   QC_QDialogButtonBox->setConstructor(QDIALOGBUTTONBOX_constructor);
   QC_QDialogButtonBox->setCopy((q_copy_t)QDIALOGBUTTONBOX_copy);

   QC_QDialogButtonBox->addMethod("addButton",                   (q_method_t)QDIALOGBUTTONBOX_addButton);
   QC_QDialogButtonBox->addMethod("button",                      (q_method_t)QDIALOGBUTTONBOX_button);
   QC_QDialogButtonBox->addMethod("buttonRole",                  (q_method_t)QDIALOGBUTTONBOX_buttonRole);
   QC_QDialogButtonBox->addMethod("buttons",                     (q_method_t)QDIALOGBUTTONBOX_buttons);
   QC_QDialogButtonBox->addMethod("centerButtons",               (q_method_t)QDIALOGBUTTONBOX_centerButtons);
   QC_QDialogButtonBox->addMethod("clear",                       (q_method_t)QDIALOGBUTTONBOX_clear);
   QC_QDialogButtonBox->addMethod("orientation",                 (q_method_t)QDIALOGBUTTONBOX_orientation);
   QC_QDialogButtonBox->addMethod("removeButton",                (q_method_t)QDIALOGBUTTONBOX_removeButton);
   QC_QDialogButtonBox->addMethod("setCenterButtons",            (q_method_t)QDIALOGBUTTONBOX_setCenterButtons);
   QC_QDialogButtonBox->addMethod("setOrientation",              (q_method_t)QDIALOGBUTTONBOX_setOrientation);
   QC_QDialogButtonBox->addMethod("setStandardButtons",          (q_method_t)QDIALOGBUTTONBOX_setStandardButtons);
   QC_QDialogButtonBox->addMethod("standardButton",              (q_method_t)QDIALOGBUTTONBOX_standardButton);
   QC_QDialogButtonBox->addMethod("standardButtons",             (q_method_t)QDIALOGBUTTONBOX_standardButtons);

   return QC_QDialogButtonBox;
}

QoreNamespace *initQDialogButtonBoxNS(QoreClass *qwidget)
{
   QoreNamespace *ns = new QoreNamespace("QDialogButtonBox");
   ns->addSystemClass(initQDialogButtonBoxClass(qwidget));

   // ButtonRole enum
   ns->addConstant("InvalidRole",              new QoreBigIntNode(QDialogButtonBox::InvalidRole));
   ns->addConstant("AcceptRole",               new QoreBigIntNode(QDialogButtonBox::AcceptRole));
   ns->addConstant("RejectRole",               new QoreBigIntNode(QDialogButtonBox::RejectRole));
   ns->addConstant("DestructiveRole",          new QoreBigIntNode(QDialogButtonBox::DestructiveRole));
   ns->addConstant("ActionRole",               new QoreBigIntNode(QDialogButtonBox::ActionRole));
   ns->addConstant("HelpRole",                 new QoreBigIntNode(QDialogButtonBox::HelpRole));
   ns->addConstant("YesRole",                  new QoreBigIntNode(QDialogButtonBox::YesRole));
   ns->addConstant("NoRole",                   new QoreBigIntNode(QDialogButtonBox::NoRole));
   ns->addConstant("ResetRole",                new QoreBigIntNode(QDialogButtonBox::ResetRole));
   ns->addConstant("ApplyRole",                new QoreBigIntNode(QDialogButtonBox::ApplyRole));
   ns->addConstant("NRoles",                   new QoreBigIntNode(QDialogButtonBox::NRoles));

   // StandardButton
   ns->addConstant("NoButton",                 new QoreBigIntNode(QDialogButtonBox::NoButton));
   ns->addConstant("Ok",                       new QoreBigIntNode(QDialogButtonBox::Ok));
   ns->addConstant("Save",                     new QoreBigIntNode(QDialogButtonBox::Save));
   ns->addConstant("SaveAll",                  new QoreBigIntNode(QDialogButtonBox::SaveAll));
   ns->addConstant("Open",                     new QoreBigIntNode(QDialogButtonBox::Open));
   ns->addConstant("Yes",                      new QoreBigIntNode(QDialogButtonBox::Yes));
   ns->addConstant("YesToAll",                 new QoreBigIntNode(QDialogButtonBox::YesToAll));
   ns->addConstant("No",                       new QoreBigIntNode(QDialogButtonBox::No));
   ns->addConstant("NoToAll",                  new QoreBigIntNode(QDialogButtonBox::NoToAll));
   ns->addConstant("Abort",                    new QoreBigIntNode(QDialogButtonBox::Abort));
   ns->addConstant("Retry",                    new QoreBigIntNode(QDialogButtonBox::Retry));
   ns->addConstant("Ignore",                   new QoreBigIntNode(QDialogButtonBox::Ignore));
   ns->addConstant("Close",                    new QoreBigIntNode(QDialogButtonBox::Close));
   ns->addConstant("Cancel",                   new QoreBigIntNode(QDialogButtonBox::Cancel));
   ns->addConstant("Discard",                  new QoreBigIntNode(QDialogButtonBox::Discard));
   ns->addConstant("Help",                     new QoreBigIntNode(QDialogButtonBox::Help));
   ns->addConstant("Apply",                    new QoreBigIntNode(QDialogButtonBox::Apply));
   ns->addConstant("Reset",                    new QoreBigIntNode(QDialogButtonBox::Reset));
   ns->addConstant("RestoreDefaults",          new QoreBigIntNode(QDialogButtonBox::RestoreDefaults));
   ns->addConstant("FirstButton",              new QoreBigIntNode(QDialogButtonBox::FirstButton));
   ns->addConstant("LastButton",               new QoreBigIntNode(QDialogButtonBox::LastButton));

   // ButtonLayout
   ns->addConstant("WinLayout",                new QoreBigIntNode(QDialogButtonBox::WinLayout));
   ns->addConstant("MacLayout",                new QoreBigIntNode(QDialogButtonBox::MacLayout));
   ns->addConstant("KdeLayout",                new QoreBigIntNode(QDialogButtonBox::KdeLayout));
   ns->addConstant("GnomeLayout",              new QoreBigIntNode(QDialogButtonBox::GnomeLayout));

   return ns;
}
