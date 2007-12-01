/*
 QC_QMessageBox.cc
 
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

#include "QC_QMessageBox.h"
#include "QC_QPushButton.h"

int CID_QMESSAGEBOX;
class QoreClass *QC_QMessageBox = 0;

//QMessageBox ( QWidget * parent = 0 )
//QMessageBox ( Icon icon, const QString & title, const QString & text, StandardButtons buttons = NoButton, QWidget * parent = 0, Qt::WindowFlags f = Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint )
static void QMESSAGEBOX_constructor(Object *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QMESSAGEBOX, new QoreQMessageBox(self));
      return;
   }
   if (p && p->type == NT_OBJECT) {
      QoreQWidget *parent = (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink);
      if (*xsink)
         return;
      ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
      self->setPrivate(CID_QMESSAGEBOX, new QoreQMessageBox(self, parent ? static_cast<QWidget *>(parent->getQWidget()) : 0));
      return;
   }
   QMessageBox::Icon icon = (QMessageBox::Icon)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QString title;
   if (get_qstring(p, title, xsink))
      return;
   p = get_param(params, 2);
   QString text;
   if (get_qstring(p, text, xsink))
      return;
   p = get_param(params, 3);
   QMessageBox::StandardButtons buttons = !is_nothing(p) ? (QMessageBox::StandardButtons)p->getAsInt() : QMessageBox::NoButton;
   p = get_param(params, 4);
   QoreQWidget *parent = (p && p->type == NT_OBJECT) ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return;
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   p = get_param(params, 5);
   Qt::WindowFlags f = (Qt::WindowFlags)(p ? p->getAsInt() : 0);
   self->setPrivate(CID_QMESSAGEBOX, new QoreQMessageBox(self, icon, title, text, buttons, parent ? static_cast<QWidget *>(parent->getQWidget()) : 0, f));
   return;
}

static void QMESSAGEBOX_copy(class Object *self, class Object *old, class QoreQMessageBox *qmb, ExceptionSink *xsink)
{
   xsink->raiseException("QMESSAGEBOX-COPY-ERROR", "objects of this class cannot be copied");
}

//void addButton ( QAbstractButton * button, ButtonRole role )
//QPushButton * addButton ( const QString & text, ButtonRole role )
//QPushButton * addButton ( StandardButton button )
static QoreNode *QMESSAGEBOX_addButton(Object *self, QoreQMessageBox *qmb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQAbstractButton *button = (QoreQAbstractButton *)p->val.object->getReferencedPrivateData(CID_QABSTRACTBUTTON, xsink);
      if (!button) {
         if (!xsink->isException())
            xsink->raiseException("QMESSAGEBOX-ADDBUTTON-PARAM-ERROR", "QMessageBox::addButton() does not know how to handle arguments of class '%s' as passed as the first argument", p->val.object->getClass()->getName());
         return 0;
      }
      ReferenceHolder<AbstractPrivateData> buttonHolder(static_cast<AbstractPrivateData *>(button), xsink);
      p = get_param(params, 1);
      QMessageBox::ButtonRole role = (QMessageBox::ButtonRole)(p ? p->getAsInt() : 0);
      qmb->qobj->addButton(static_cast<QAbstractButton *>(button->qobj), role);
      return 0;
   }
   QPushButton *qt_qobj;

   if (p && p->type == NT_STRING) {
      QString text;
      if (get_qstring(p, text, xsink))
         return 0;
      p = get_param(params, 1);
      QMessageBox::ButtonRole role = (QMessageBox::ButtonRole)(p ? p->getAsInt() : 0);

      qt_qobj = qmb->qobj->addButton(text, role);
   }
   else {
      QMessageBox::StandardButton button = (QMessageBox::StandardButton)(p ? p->getAsInt() : 0);
      qt_qobj = qmb->qobj->addButton(button);
   }

   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   Object *rv_obj = reinterpret_cast<Object *>(qv_ptr.toULongLong());
   if (rv_obj)
      rv_obj->ref();
   else {
      rv_obj = new Object(QC_QPushButton, getProgram());
      QoreQtQPushButton *t_qobj = new QoreQtQPushButton(rv_obj, qt_qobj);
      rv_obj->setPrivate(CID_QPUSHBUTTON, t_qobj);
   }
   return new QoreNode(rv_obj);
}

//QAbstractButton * button ( StandardButton which ) const
static QoreNode *QMESSAGEBOX_button(Object *self, QoreQMessageBox *qmb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QMessageBox::StandardButton which = (QMessageBox::StandardButton)(p ? p->getAsInt() : 0);
   QAbstractButton *qab = qmb->qobj->button(which);
   if (!qab)
      return 0;

   QoreAbstractQAbstractButton *qaqab;

   QoreClass *qc;
   Object *pbo;
   QPushButton *pb = dynamic_cast<QPushButton *>(qab);
   if (pb) {
      qc = QC_QPushButton;
      pbo = new Object(qc, getProgram());
      qaqab = new QoreQtQPushButton(pbo, pb);
   }
   else {
      QCheckBox *cb = dynamic_cast<QCheckBox *>(qab);
      if (cb) {
	 qc = QC_QCheckBox;
	 pbo = new Object(qc, getProgram());
	 qaqab = new QoreQtQCheckBox(pbo, cb);
      }
      else {
	 QRadioButton *rb = dynamic_cast<QRadioButton *>(qab);
	 if (rb) {
	    qc = QC_QRadioButton;
	    pbo = new Object(qc, getProgram());
	    qaqab = new QoreQtQRadioButton(pbo, rb);
	 }
	 else {
	    QToolButton *tb = dynamic_cast<QToolButton *>(qab);
	    if (tb) {
	       qc = QC_QToolButton;
	       pbo = new Object(qc, getProgram());
	       qaqab = new QoreQtQToolButton(pbo, tb);
	    }
	    else {
	       qc = QC_QAbstractButton;
	       pbo = new Object(qc, getProgram());
	       qaqab = new QoreQtQAbstractButton(pbo, qab);
	    }
	 }
      }
   }

   pbo->setPrivate(qc->getID(), qaqab);
   return new QoreNode(pbo);
}

//QAbstractButton * clickedButton () const
static QoreNode *QMESSAGEBOX_clickedButton(Object *self, QoreQMessageBox *qmb, QoreNode *params, ExceptionSink *xsink)
{
   QAbstractButton *qt_qobj = qmb->qobj->clickedButton();
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   Object *rv_obj = reinterpret_cast<Object *>(qv_ptr.toULongLong());
   assert(rv_obj);
   rv_obj->ref();
   return new QoreNode(rv_obj);
}

//QPushButton * defaultButton () const
static QoreNode *QMESSAGEBOX_defaultButton(Object *self, QoreQMessageBox *qmb, QoreNode *params, ExceptionSink *xsink)
{
   QPushButton *qt_qobj = qmb->qobj->defaultButton();
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   Object *rv_obj = reinterpret_cast<Object *>(qv_ptr.toULongLong());
   if (rv_obj)
      rv_obj->ref();
   else {
      rv_obj = new Object(QC_QPushButton, getProgram());
      QoreQtQPushButton *t_qobj = new QoreQtQPushButton(rv_obj, qt_qobj);
      rv_obj->setPrivate(CID_QPUSHBUTTON, t_qobj);
   }
   return new QoreNode(rv_obj);
}

//QString detailedText () const
static QoreNode *QMESSAGEBOX_detailedText(Object *self, QoreQMessageBox *qmb, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(new QoreString(qmb->qobj->detailedText().toUtf8().data(), QCS_UTF8));
}

//QAbstractButton * escapeButton () const
static QoreNode *QMESSAGEBOX_escapeButton(Object *self, QoreQMessageBox *qmb, QoreNode *params, ExceptionSink *xsink)
{
   QAbstractButton *qt_qobj = qmb->qobj->escapeButton();
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   Object *rv_obj = reinterpret_cast<Object *>(qv_ptr.toULongLong());
   assert(rv_obj);
   rv_obj->ref();
   return new QoreNode(rv_obj);
}

//Icon icon () const
static QoreNode *QMESSAGEBOX_icon(Object *self, QoreQMessageBox *qmb, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qmb->qobj->icon());
}

//QPixmap iconPixmap () const
static QoreNode *QMESSAGEBOX_iconPixmap(Object *self, QoreQMessageBox *qmb, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qp = new Object(QC_QPixmap, getProgram());
   QoreQPixmap *q_qp = new QoreQPixmap(qmb->qobj->iconPixmap());
   o_qp->setPrivate(CID_QPIXMAP, q_qp);
   return new QoreNode(o_qp);
}

//QString informativeText () const
static QoreNode *QMESSAGEBOX_informativeText(Object *self, QoreQMessageBox *qmb, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(new QoreString(qmb->qobj->informativeText().toUtf8().data(), QCS_UTF8));
}

//void removeButton ( QAbstractButton * button )
static QoreNode *QMESSAGEBOX_removeButton(Object *self, QoreQMessageBox *qmb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQAbstractButton *button = (p && p->type == NT_OBJECT) ? (QoreQAbstractButton *)p->val.object->getReferencedPrivateData(CID_QABSTRACTBUTTON, xsink) : 0;
   if (!button) {
      if (!xsink->isException())
         xsink->raiseException("QMESSAGEBOX-REMOVEBUTTON-PARAM-ERROR", "expecting a QAbstractButton object as first argument to QMessageBox::removeButton()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> buttonHolder(static_cast<AbstractPrivateData *>(button), xsink);
   qmb->qobj->removeButton(static_cast<QAbstractButton *>(button->qobj));
   return 0;
}

////void setDefaultButton ( QPushButton * button )
////void setDefaultButton ( StandardButton button )
//static QoreNode *QMESSAGEBOX_setDefaultButton(Object *self, QoreQMessageBox *qmb, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   if (p && p->type == NT_???) {
//      ??? QPushButton* button = p;
//      qmb->qobj->setDefaultButton(button);
//      return 0;
//   }
//   QMessageBox::StandardButton button = (QMessageBox::StandardButton)(p ? p->getAsInt() : 0);
//   qmb->qobj->setDefaultButton(button);
//   return 0;
//}

//void setDetailedText ( const QString & text )
static QoreNode *QMESSAGEBOX_setDetailedText(Object *self, QoreQMessageBox *qmb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString text;
   if (get_qstring(p, text, xsink))
      return 0;
   qmb->qobj->setDetailedText(text);
   return 0;
}

//void setEscapeButton ( QAbstractButton * button )
//void setEscapeButton ( StandardButton button )
static QoreNode *QMESSAGEBOX_setEscapeButton(Object *self, QoreQMessageBox *qmb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQAbstractButton *button = (QoreQAbstractButton *)p->val.object->getReferencedPrivateData(CID_QABSTRACTBUTTON, xsink);
      if (!button) {
         if (!xsink->isException())
            xsink->raiseException("QMESSAGEBOX-SETESCAPEBUTTON-PARAM-ERROR", "QMessageBox::setEscapeButton() does not know how to handle arguments of class '%s' as passed as the first argument", p->val.object->getClass()->getName());
         return 0;
      }
      ReferenceHolder<AbstractPrivateData> buttonHolder(static_cast<AbstractPrivateData *>(button), xsink);
      qmb->qobj->setEscapeButton(static_cast<QAbstractButton *>(button->qobj));
      return 0;
   }
   QMessageBox::StandardButton button = (QMessageBox::StandardButton)(p ? p->getAsInt() : 0);
   qmb->qobj->setEscapeButton(button);
   return 0;
}

//void setIcon ( Icon )
static QoreNode *QMESSAGEBOX_setIcon(Object *self, QoreQMessageBox *qmb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QMessageBox::Icon icon = (QMessageBox::Icon)(p ? p->getAsInt() : 0);
   qmb->qobj->setIcon(icon);
   return 0;
}

//void setIconPixmap ( const QPixmap & pixmap )
static QoreNode *QMESSAGEBOX_setIconPixmap(Object *self, QoreQMessageBox *qmb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQPixmap *pixmap = (p && p->type == NT_OBJECT) ? (QoreQPixmap *)p->val.object->getReferencedPrivateData(CID_QPIXMAP, xsink) : 0;
   if (!pixmap) {
      if (!xsink->isException())
         xsink->raiseException("QMESSAGEBOX-SETICONPIXMAP-PARAM-ERROR", "expecting a QPixmap object as first argument to QMessageBox::setIconPixmap()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> pixmapHolder(static_cast<AbstractPrivateData *>(pixmap), xsink);
   qmb->qobj->setIconPixmap(*(static_cast<QPixmap *>(pixmap)));
   return 0;
}

//void setInformativeText ( const QString & text )
static QoreNode *QMESSAGEBOX_setInformativeText(Object *self, QoreQMessageBox *qmb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString text;
   if (get_qstring(p, text, xsink))
      return 0;
   qmb->qobj->setInformativeText(text);
   return 0;
}

//void setStandardButtons ( StandardButtons buttons )
static QoreNode *QMESSAGEBOX_setStandardButtons(Object *self, QoreQMessageBox *qmb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QMessageBox::StandardButtons buttons = (QMessageBox::StandardButtons)(p ? p->getAsInt() : 0);
   qmb->qobj->setStandardButtons(buttons);
   return 0;
}

//void setText ( const QString & text )
static QoreNode *QMESSAGEBOX_setText(Object *self, QoreQMessageBox *qmb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString text;
   if (get_qstring(p, text, xsink))
      return 0;
   qmb->qobj->setText(text);
   return 0;
}

//void setTextFormat ( Qt::TextFormat format )
static QoreNode *QMESSAGEBOX_setTextFormat(Object *self, QoreQMessageBox *qmb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   Qt::TextFormat format = (Qt::TextFormat)(p ? p->getAsInt() : 0);
   qmb->qobj->setTextFormat(format);
   return 0;
}

//void setWindowModality ( Qt::WindowModality windowModality )
static QoreNode *QMESSAGEBOX_setWindowModality(Object *self, QoreQMessageBox *qmb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   Qt::WindowModality windowModality = (Qt::WindowModality)(p ? p->getAsInt() : 0);
   qmb->qobj->setWindowModality(windowModality);
   return 0;
}

//void setWindowTitle ( const QString & title )
static QoreNode *QMESSAGEBOX_setWindowTitle(Object *self, QoreQMessageBox *qmb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString title;
   if (get_qstring(p, title, xsink))
      return 0;
   qmb->qobj->setWindowTitle(title);
   return 0;
}

////StandardButton standardButton ( QAbstractButton * button ) const
//static QoreNode *QMESSAGEBOX_standardButton(Object *self, QoreQMessageBox *qmb, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   QoreQAbstractButton *button = (p && p->type == NT_OBJECT) ? (QoreQAbstractButton *)p->val.object->getReferencedPrivateData(CID_QABSTRACTBUTTON, xsink) : 0;
//   if (!button) {
//      if (!xsink->isException())
//         xsink->raiseException("QMESSAGEBOX-STANDARDBUTTON-PARAM-ERROR", "expecting a QAbstractButton object as first argument to QMessageBox::standardButton()");
//      return 0;
//   }
//   ReferenceHolder<AbstractPrivateData> buttonHolder(static_cast<AbstractPrivateData *>(button), xsink);
//   ??? return new QoreNode((int64)qmb->qobj->standardButton(static_cast<QAbstractButton *>(button->qobj)));
//}

////StandardButtons standardButtons () const
//static QoreNode *QMESSAGEBOX_standardButtons(Object *self, QoreQMessageBox *qmb, QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreNode((int64)qmb->qobj->standardButtons());
//}

//QString text () const
static QoreNode *QMESSAGEBOX_text(Object *self, QoreQMessageBox *qmb, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(new QoreString(qmb->qobj->text().toUtf8().data(), QCS_UTF8));
}

//Qt::TextFormat textFormat () const
static QoreNode *QMESSAGEBOX_textFormat(Object *self, QoreQMessageBox *qmb, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qmb->qobj->textFormat());
}

//int exec ()
static QoreNode *QMESSAGEBOX_exec(Object *self, QoreQMessageBox *qmb, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qmb->qobj->exec());
}

QoreClass *initQMessageBoxClass(QoreClass *qdialog)
{
   QC_QMessageBox = new QoreClass("QMessageBox", QDOM_GUI);
   CID_QMESSAGEBOX = QC_QMessageBox->getID();

   QC_QMessageBox->addBuiltinVirtualBaseClass(qdialog);

   QC_QMessageBox->setConstructor(QMESSAGEBOX_constructor);
   QC_QMessageBox->setCopy((q_copy_t)QMESSAGEBOX_copy);

   QC_QMessageBox->addMethod("addButton",                   (q_method_t)QMESSAGEBOX_addButton);
   QC_QMessageBox->addMethod("button",                      (q_method_t)QMESSAGEBOX_button);
   QC_QMessageBox->addMethod("clickedButton",               (q_method_t)QMESSAGEBOX_clickedButton);
   QC_QMessageBox->addMethod("defaultButton",               (q_method_t)QMESSAGEBOX_defaultButton);
   QC_QMessageBox->addMethod("detailedText",                (q_method_t)QMESSAGEBOX_detailedText);
   QC_QMessageBox->addMethod("escapeButton",                (q_method_t)QMESSAGEBOX_escapeButton);
   QC_QMessageBox->addMethod("icon",                        (q_method_t)QMESSAGEBOX_icon);
   QC_QMessageBox->addMethod("iconPixmap",                  (q_method_t)QMESSAGEBOX_iconPixmap);
   QC_QMessageBox->addMethod("informativeText",             (q_method_t)QMESSAGEBOX_informativeText);
   QC_QMessageBox->addMethod("removeButton",                (q_method_t)QMESSAGEBOX_removeButton);
   //QC_QMessageBox->addMethod("setDefaultButton",            (q_method_t)QMESSAGEBOX_setDefaultButton);
   QC_QMessageBox->addMethod("setDetailedText",             (q_method_t)QMESSAGEBOX_setDetailedText);
   QC_QMessageBox->addMethod("setEscapeButton",             (q_method_t)QMESSAGEBOX_setEscapeButton);
   QC_QMessageBox->addMethod("setIcon",                     (q_method_t)QMESSAGEBOX_setIcon);
   QC_QMessageBox->addMethod("setIconPixmap",               (q_method_t)QMESSAGEBOX_setIconPixmap);
   QC_QMessageBox->addMethod("setInformativeText",          (q_method_t)QMESSAGEBOX_setInformativeText);
   QC_QMessageBox->addMethod("setStandardButtons",          (q_method_t)QMESSAGEBOX_setStandardButtons);
   QC_QMessageBox->addMethod("setText",                     (q_method_t)QMESSAGEBOX_setText);
   QC_QMessageBox->addMethod("setTextFormat",               (q_method_t)QMESSAGEBOX_setTextFormat);
   QC_QMessageBox->addMethod("setWindowModality",           (q_method_t)QMESSAGEBOX_setWindowModality);
   QC_QMessageBox->addMethod("setWindowTitle",              (q_method_t)QMESSAGEBOX_setWindowTitle);
   //QC_QMessageBox->addMethod("standardButton",              (q_method_t)QMESSAGEBOX_standardButton);
   //QC_QMessageBox->addMethod("standardButtons",             (q_method_t)QMESSAGEBOX_standardButtons);
   QC_QMessageBox->addMethod("text",                        (q_method_t)QMESSAGEBOX_text);
   QC_QMessageBox->addMethod("textFormat",                  (q_method_t)QMESSAGEBOX_textFormat);
   QC_QMessageBox->addMethod("exec",                        (q_method_t)QMESSAGEBOX_exec);

   return QC_QMessageBox;
}

//void about ( QWidget * parent, const QString & title, const QString & text )
static QoreNode *f_QMessageBox_about(QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQWidget *parent = (p && p->type == NT_OBJECT) ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!parent) {
      if (!xsink->isException())
         xsink->raiseException("QMESSAGEBOX-ABOUT-PARAM-ERROR", "expecting a QWidget object as first argument to QMessageBox_about()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   p = get_param(params, 1);
   QString title;
   if (get_qstring(p, title, xsink))
      return 0;
   p = get_param(params, 2);
   QString text;
   if (get_qstring(p, text, xsink))
      return 0;
   QMessageBox::about(parent->getQWidget(), title, text);
   return 0;
}

//void aboutQt ( QWidget * parent, const QString & title = QString() )
static QoreNode *f_QMessageBox_aboutQt(QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQWidget *parent = (p && p->type == NT_OBJECT) ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!parent) {
      if (!xsink->isException())
         xsink->raiseException("QMESSAGEBOX-ABOUTQT-PARAM-ERROR", "expecting a QWidget object as first argument to QMessageBox_aboutQt()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   p = get_param(params, 1);
   QString title;
   if (get_qstring(p, title, xsink, true))
      title = QString();
   QMessageBox::aboutQt(parent->getQWidget(), title);
   return 0;
}

//StandardButton critical ( QWidget * parent, const QString & title, const QString & text, StandardButtons buttons = Ok, StandardButton defaultButton = NoButton )
static QoreNode *f_QMessageBox_critical(QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQWidget *parent = (p && p->type == NT_OBJECT) ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   p = get_param(params, 1);
   QString title;
   if (get_qstring(p, title, xsink))
      return 0;
   p = get_param(params, 2);
   QString text;
   if (get_qstring(p, text, xsink))
      return 0;
   p = get_param(params, 3);
   QMessageBox::StandardButtons buttons = !is_nothing(p) ? (QMessageBox::StandardButtons)p->getAsInt() : QMessageBox::Ok;
   p = get_param(params, 4);
   QMessageBox::StandardButton defaultButton = !is_nothing(p) ? (QMessageBox::StandardButton)p->getAsInt() : QMessageBox::NoButton;
   return new QoreNode((int64)QMessageBox::critical(parent ? static_cast<QWidget *>(parent->getQWidget()) : 0, title, text, buttons, defaultButton));
}

//StandardButton information ( QWidget * parent, const QString & title, const QString & text, StandardButtons buttons = Ok, StandardButton defaultButton = NoButton )
static QoreNode *f_QMessageBox_information(QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQWidget *parent = (p && p->type == NT_OBJECT) ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   p = get_param(params, 1);
   QString title;
   if (get_qstring(p, title, xsink))
      return 0;
   p = get_param(params, 2);
   QString text;
   if (get_qstring(p, text, xsink))
      return 0;
   p = get_param(params, 3);
   QMessageBox::StandardButtons buttons = !is_nothing(p) ? (QMessageBox::StandardButtons)p->getAsInt() : QMessageBox::Ok;
   p = get_param(params, 4);
   QMessageBox::StandardButton defaultButton = !is_nothing(p) ? (QMessageBox::StandardButton)p->getAsInt() : QMessageBox::NoButton;
   return new QoreNode((int64)QMessageBox::information(parent ? static_cast<QWidget *>(parent->getQWidget()) : 0, title, text, buttons, defaultButton));
}

//StandardButton question ( QWidget * parent, const QString & title, const QString & text, StandardButtons buttons = Ok, StandardButton defaultButton = NoButton )
static QoreNode *f_QMessageBox_question(QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQWidget *parent = (p && p->type == NT_OBJECT) ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   p = get_param(params, 1);
   QString title;
   if (get_qstring(p, title, xsink))
      return 0;
   p = get_param(params, 2);
   QString text;
   if (get_qstring(p, text, xsink))
      return 0;
   p = get_param(params, 3);
   QMessageBox::StandardButtons buttons = !is_nothing(p) ? (QMessageBox::StandardButtons)p->getAsInt() : QMessageBox::Ok;
   p = get_param(params, 4);
   QMessageBox::StandardButton defaultButton = !is_nothing(p) ? (QMessageBox::StandardButton)p->getAsInt() : QMessageBox::NoButton;
   return new QoreNode((int64)QMessageBox::question(parent ? static_cast<QWidget *>(parent->getQWidget()) : 0, title, text, buttons, defaultButton));
}

//StandardButton warning ( QWidget * parent, const QString & title, const QString & text, StandardButtons buttons = Ok, StandardButton defaultButton = NoButton )
static QoreNode *f_QMessageBox_warning(QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQWidget *parent = (p && p->type == NT_OBJECT) ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   p = get_param(params, 1);
   QString title;
   if (get_qstring(p, title, xsink))
      return 0;
   p = get_param(params, 2);
   QString text;
   if (get_qstring(p, text, xsink))
      return 0;
   p = get_param(params, 3);
   QMessageBox::StandardButtons buttons = !is_nothing(p) ? (QMessageBox::StandardButtons)p->getAsInt() : QMessageBox::Ok;
   p = get_param(params, 4);
   QMessageBox::StandardButton defaultButton = !is_nothing(p) ? (QMessageBox::StandardButton)p->getAsInt() : QMessageBox::NoButton;
   return new QoreNode((int64)QMessageBox::warning(parent ? static_cast<QWidget *>(parent->getQWidget()) : 0, title, text, buttons, defaultButton));
}


void initQMessageBoxStaticFunctions()
{
   // add builtin functions
   builtinFunctions.add("QMessageBox_about",       f_QMessageBox_about);
   builtinFunctions.add("QMessageBox_aboutQt",     f_QMessageBox_aboutQt);
   builtinFunctions.add("QMessageBox_critical",    f_QMessageBox_critical);
   builtinFunctions.add("QMessageBox_information", f_QMessageBox_information);
   builtinFunctions.add("QMessageBox_question",    f_QMessageBox_question);
   builtinFunctions.add("QMessageBox_warning",     f_QMessageBox_warning);
}
