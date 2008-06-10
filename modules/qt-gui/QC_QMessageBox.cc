/*
 QC_QMessageBox.cc
 
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

#include "QC_QMessageBox.h"
#include "QC_QPushButton.h"
#include "QC_QWidget.h"
#include "QC_QCheckBox.h"
#include "QC_QRadioButton.h"
#include "QC_QToolButton.h"
#include "QC_QPixmap.h"
#include "QC_QAbstractButton.h"

#include "qore-qt-gui.h"

qore_classid_t CID_QMESSAGEBOX;
class QoreClass *QC_QMessageBox = 0;

//QMessageBox ( QWidget * parent = 0 )
//QMessageBox ( Icon icon, const QString & title, const QString & text, StandardButtons buttons = NoButton, QWidget * parent = 0, Qt::WindowFlags f = Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint )
static void QMESSAGEBOX_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QMESSAGEBOX, new QoreQMessageBox(self));
      return;
   }
   if (p && p->getType() == NT_OBJECT) {
      QoreQWidget *parent = (QoreQWidget *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink);
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

   const QoreObject *o = test_object_param(params, 4);
   QoreQWidget *parent = o ? (QoreQWidget *)o->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return;
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   p = get_param(params, 5);
   Qt::WindowFlags f = (Qt::WindowFlags)(p ? p->getAsInt() : 0);
   self->setPrivate(CID_QMESSAGEBOX, new QoreQMessageBox(self, icon, title, text, buttons, parent ? static_cast<QWidget *>(parent->getQWidget()) : 0, f));
   return;
}

static void QMESSAGEBOX_copy(class QoreObject *self, class QoreObject *old, class QoreQMessageBox *qmb, ExceptionSink *xsink)
{
   xsink->raiseException("QMESSAGEBOX-COPY-ERROR", "objects of this class cannot be copied");
}

//void addButton ( QAbstractButton * button, ButtonRole role )
//QPushButton * addButton ( const QString & text, ButtonRole role )
//QPushButton * addButton ( StandardButton button )
static AbstractQoreNode *QMESSAGEBOX_addButton(QoreObject *self, QoreQMessageBox *qmb, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (p && p->getType() == NT_OBJECT) {
      QoreQAbstractButton *button = (QoreQAbstractButton *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QABSTRACTBUTTON, xsink);
      if (!button) {
         if (!xsink->isException())
            xsink->raiseException("QMESSAGEBOX-ADDBUTTON-PARAM-ERROR", "QMessageBox::addButton() does not know how to handle arguments of class '%s' as passed as the first argument", (reinterpret_cast<const QoreObject *>(p))->getClassName());
         return 0;
      }
      ReferenceHolder<AbstractPrivateData> buttonHolder(static_cast<AbstractPrivateData *>(button), xsink);
      p = get_param(params, 1);
      QMessageBox::ButtonRole role = (QMessageBox::ButtonRole)(p ? p->getAsInt() : 0);
      qmb->qobj->addButton(static_cast<QAbstractButton *>(button->qobj), role);
      return 0;
   }
   QPushButton *qt_qobj;

   if (p && p->getType() == NT_STRING) {
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

//QAbstractButton * button ( StandardButton which ) const
static AbstractQoreNode *QMESSAGEBOX_button(QoreObject *self, QoreQMessageBox *qmb, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QMessageBox::StandardButton which = (QMessageBox::StandardButton)(p ? p->getAsInt() : 0);
   QAbstractButton *qab = qmb->qobj->button(which);
   if (!qab)
      return 0;

   QoreAbstractQAbstractButton *qaqab;

   QoreClass *qc;
   QoreObject *pbo;
   QPushButton *pb = dynamic_cast<QPushButton *>(qab);
   if (pb) {
      qc = QC_QPushButton;
      pbo = new QoreObject(qc, getProgram());
      qaqab = new QoreQtQPushButton(pbo, pb);
   }
   else {
      QCheckBox *cb = dynamic_cast<QCheckBox *>(qab);
      if (cb) {
	 qc = QC_QCheckBox;
	 pbo = new QoreObject(qc, getProgram());
	 qaqab = new QoreQtQCheckBox(pbo, cb);
      }
      else {
	 QRadioButton *rb = dynamic_cast<QRadioButton *>(qab);
	 if (rb) {
	    qc = QC_QRadioButton;
	    pbo = new QoreObject(qc, getProgram());
	    qaqab = new QoreQtQRadioButton(pbo, rb);
	 }
	 else {
	    QToolButton *tb = dynamic_cast<QToolButton *>(qab);
	    if (tb) {
	       qc = QC_QToolButton;
	       pbo = new QoreObject(qc, getProgram());
	       qaqab = new QoreQtQToolButton(pbo, tb);
	    }
	    else {
	       qc = QC_QAbstractButton;
	       pbo = new QoreObject(qc, getProgram());
	       qaqab = new QoreQtQAbstractButton(pbo, qab);
	    }
	 }
      }
   }

   pbo->setPrivate(qc->getID(), qaqab);
   return pbo;
}

//QAbstractButton * clickedButton () const
static AbstractQoreNode *QMESSAGEBOX_clickedButton(QoreObject *self, QoreQMessageBox *qmb, const QoreListNode *params, ExceptionSink *xsink)
{
   QAbstractButton *qt_qobj = qmb->qobj->clickedButton();
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   QoreObject *rv_obj = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   assert(rv_obj);
   rv_obj->ref();
   return rv_obj;
}

//QPushButton * defaultButton () const
static AbstractQoreNode *QMESSAGEBOX_defaultButton(QoreObject *self, QoreQMessageBox *qmb, const QoreListNode *params, ExceptionSink *xsink)
{
   QPushButton *qt_qobj = qmb->qobj->defaultButton();
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

//QString detailedText () const
static AbstractQoreNode *QMESSAGEBOX_detailedText(QoreObject *self, QoreQMessageBox *qmb, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qmb->qobj->detailedText().toUtf8().data(), QCS_UTF8);
}

//QAbstractButton * escapeButton () const
static AbstractQoreNode *QMESSAGEBOX_escapeButton(QoreObject *self, QoreQMessageBox *qmb, const QoreListNode *params, ExceptionSink *xsink)
{
   QAbstractButton *qt_qobj = qmb->qobj->escapeButton();
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   QoreObject *rv_obj = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   assert(rv_obj);
   rv_obj->ref();
   return rv_obj;
}

//Icon icon () const
static AbstractQoreNode *QMESSAGEBOX_icon(QoreObject *self, QoreQMessageBox *qmb, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qmb->qobj->icon());
}

//QPixmap iconPixmap () const
static AbstractQoreNode *QMESSAGEBOX_iconPixmap(QoreObject *self, QoreQMessageBox *qmb, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qp = new QoreObject(QC_QPixmap, getProgram());
   QoreQPixmap *q_qp = new QoreQPixmap(qmb->qobj->iconPixmap());
   o_qp->setPrivate(CID_QPIXMAP, q_qp);
   return o_qp;
}

//QString informativeText () const
static AbstractQoreNode *QMESSAGEBOX_informativeText(QoreObject *self, QoreQMessageBox *qmb, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qmb->qobj->informativeText().toUtf8().data(), QCS_UTF8);
}

//void removeButton ( QAbstractButton * button )
static AbstractQoreNode *QMESSAGEBOX_removeButton(QoreObject *self, QoreQMessageBox *qmb, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreQAbstractButton *button = p ? (QoreQAbstractButton *)p->getReferencedPrivateData(CID_QABSTRACTBUTTON, xsink) : 0;
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
//static AbstractQoreNode *QMESSAGEBOX_setDefaultButton(QoreObject *self, QoreQMessageBox *qmb, const QoreListNode *params, ExceptionSink *xsink)
//{
//   const AbstractQoreNode *p = get_param(params, 0);
//   if (p && p->getType() == NT_???) {
//      ??? QPushButton* button = p;
//      qmb->qobj->setDefaultButton(button);
//      return 0;
//   }
//   QMessageBox::StandardButton button = (QMessageBox::StandardButton)(p ? p->getAsInt() : 0);
//   qmb->qobj->setDefaultButton(button);
//   return 0;
//}

//void setDetailedText ( const QString & text )
static AbstractQoreNode *QMESSAGEBOX_setDetailedText(QoreObject *self, QoreQMessageBox *qmb, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QString text;
   if (get_qstring(p, text, xsink))
      return 0;
   qmb->qobj->setDetailedText(text);
   return 0;
}

//void setEscapeButton ( QAbstractButton * button )
//void setEscapeButton ( StandardButton button )
static AbstractQoreNode *QMESSAGEBOX_setEscapeButton(QoreObject *self, QoreQMessageBox *qmb, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (p && p->getType() == NT_OBJECT) {
      QoreQAbstractButton *button = (QoreQAbstractButton *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QABSTRACTBUTTON, xsink);
      if (!button) {
         if (!xsink->isException())
            xsink->raiseException("QMESSAGEBOX-SETESCAPEBUTTON-PARAM-ERROR", "QMessageBox::setEscapeButton() does not know how to handle arguments of class '%s' as passed as the first argument", (reinterpret_cast<const QoreObject *>(p))->getClassName());
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
static AbstractQoreNode *QMESSAGEBOX_setIcon(QoreObject *self, QoreQMessageBox *qmb, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QMessageBox::Icon icon = (QMessageBox::Icon)(p ? p->getAsInt() : 0);
   qmb->qobj->setIcon(icon);
   return 0;
}

//void setIconPixmap ( const QPixmap & pixmap )
static AbstractQoreNode *QMESSAGEBOX_setIconPixmap(QoreObject *self, QoreQMessageBox *qmb, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreQPixmap *pixmap = p ? (QoreQPixmap *)p->getReferencedPrivateData(CID_QPIXMAP, xsink) : 0;
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
static AbstractQoreNode *QMESSAGEBOX_setInformativeText(QoreObject *self, QoreQMessageBox *qmb, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QString text;
   if (get_qstring(p, text, xsink))
      return 0;
   qmb->qobj->setInformativeText(text);
   return 0;
}

//void setStandardButtons ( StandardButtons buttons )
static AbstractQoreNode *QMESSAGEBOX_setStandardButtons(QoreObject *self, QoreQMessageBox *qmb, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QMessageBox::StandardButtons buttons = (QMessageBox::StandardButtons)(p ? p->getAsInt() : 0);
   qmb->qobj->setStandardButtons(buttons);
   return 0;
}

//void setText ( const QString & text )
static AbstractQoreNode *QMESSAGEBOX_setText(QoreObject *self, QoreQMessageBox *qmb, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QString text;
   if (get_qstring(p, text, xsink))
      return 0;
   qmb->qobj->setText(text);
   return 0;
}

//void setTextFormat ( Qt::TextFormat format )
static AbstractQoreNode *QMESSAGEBOX_setTextFormat(QoreObject *self, QoreQMessageBox *qmb, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   Qt::TextFormat format = (Qt::TextFormat)(p ? p->getAsInt() : 0);
   qmb->qobj->setTextFormat(format);
   return 0;
}

//void setWindowModality ( Qt::WindowModality windowModality )
static AbstractQoreNode *QMESSAGEBOX_setWindowModality(QoreObject *self, QoreQMessageBox *qmb, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   Qt::WindowModality windowModality = (Qt::WindowModality)(p ? p->getAsInt() : 0);
   qmb->qobj->setWindowModality(windowModality);
   return 0;
}

//void setWindowTitle ( const QString & title )
static AbstractQoreNode *QMESSAGEBOX_setWindowTitle(QoreObject *self, QoreQMessageBox *qmb, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QString title;
   if (get_qstring(p, title, xsink))
      return 0;
   qmb->qobj->setWindowTitle(title);
   return 0;
}

////StandardButton standardButton ( QAbstractButton * button ) const
//static AbstractQoreNode *QMESSAGEBOX_standardButton(QoreObject *self, QoreQMessageBox *qmb, const QoreListNode *params, ExceptionSink *xsink)
//{
//   QoreObject *p = test_object_param(params, 0);
//   QoreQAbstractButton *button = p ? (QoreQAbstractButton *)p->getReferencedPrivateData(CID_QABSTRACTBUTTON, xsink) : 0;
//   if (!button) {
//      if (!xsink->isException())
//         xsink->raiseException("QMESSAGEBOX-STANDARDBUTTON-PARAM-ERROR", "expecting a QAbstractButton object as first argument to QMessageBox::standardButton()");
//      return 0;
//   }
//   ReferenceHolder<AbstractPrivateData> buttonHolder(static_cast<AbstractPrivateData *>(button), xsink);
//   ??? return new QoreBigIntNode(qmb->qobj->standardButton(static_cast<QAbstractButton *>(button->qobj)));
//}

////StandardButtons standardButtons () const
//static AbstractQoreNode *QMESSAGEBOX_standardButtons(QoreObject *self, QoreQMessageBox *qmb, const QoreListNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreBigIntNode(qmb->qobj->standardButtons());
//}

//QString text () const
static AbstractQoreNode *QMESSAGEBOX_text(QoreObject *self, QoreQMessageBox *qmb, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qmb->qobj->text().toUtf8().data(), QCS_UTF8);
}

//Qt::TextFormat textFormat () const
static AbstractQoreNode *QMESSAGEBOX_textFormat(QoreObject *self, QoreQMessageBox *qmb, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qmb->qobj->textFormat());
}

//int exec ()
static AbstractQoreNode *QMESSAGEBOX_exec(QoreObject *self, QoreQMessageBox *qmb, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qmb->qobj->exec());
}

//void about ( QWidget * parent, const QString & title, const QString & text )
static AbstractQoreNode *f_QMessageBox_about(const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o = test_object_param(params, 0);
   QoreQWidget *parent = o ? (QoreQWidget *)o->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!parent) {
      if (!xsink->isException())
         xsink->raiseException("QMESSAGEBOX-ABOUT-PARAM-ERROR", "expecting a QWidget object as first argument to QMessageBox_about()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);

   const AbstractQoreNode *p = get_param(params, 1);
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
static AbstractQoreNode *f_QMessageBox_aboutQt(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreObject *o = test_object_param(params, 0);
   QoreQWidget *parent = o ? (QoreQWidget *)o->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!parent) {
      if (!xsink->isException())
         xsink->raiseException("QMESSAGEBOX-ABOUTQT-PARAM-ERROR", "expecting a QWidget object as first argument to QMessageBox_aboutQt()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);

   const AbstractQoreNode *p = get_param(params, 1);
   QString title;
   if (get_qstring(p, title, xsink, true))
      title = QString();
   QMessageBox::aboutQt(parent->getQWidget(), title);
   return 0;
}

//StandardButton critical ( QWidget * parent, const QString & title, const QString & text, StandardButtons buttons = Ok, StandardButton defaultButton = NoButton )
static AbstractQoreNode *f_QMessageBox_critical(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreObject *o = test_object_param(params, 0);
   QoreQWidget *parent = o ? (QoreQWidget *)o->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);

   const AbstractQoreNode *p = get_param(params, 1);
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
   return new QoreBigIntNode(QMessageBox::critical(parent ? static_cast<QWidget *>(parent->getQWidget()) : 0, title, text, buttons, defaultButton));
}

//StandardButton information ( QWidget * parent, const QString & title, const QString & text, StandardButtons buttons = Ok, StandardButton defaultButton = NoButton )
static AbstractQoreNode *f_QMessageBox_information(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreObject *o = test_object_param(params, 0);
   QoreQWidget *parent = o ? (QoreQWidget *)o->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);

   const AbstractQoreNode *p = get_param(params, 1);
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
   return new QoreBigIntNode(QMessageBox::information(parent ? static_cast<QWidget *>(parent->getQWidget()) : 0, title, text, buttons, defaultButton));
}

//StandardButton question ( QWidget * parent, const QString & title, const QString & text, StandardButtons buttons = Ok, StandardButton defaultButton = NoButton )
static AbstractQoreNode *f_QMessageBox_question(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreObject *o = test_object_param(params, 0);
   QoreQWidget *parent = o ? (QoreQWidget *)o->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);

   const AbstractQoreNode *p = get_param(params, 1);
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
   return new QoreBigIntNode(QMessageBox::question(parent ? static_cast<QWidget *>(parent->getQWidget()) : 0, title, text, buttons, defaultButton));
}

//StandardButton warning ( QWidget * parent, const QString & title, const QString & text, StandardButtons buttons = Ok, StandardButton defaultButton = NoButton )
static AbstractQoreNode *f_QMessageBox_warning(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreObject *o = test_object_param(params, 0);
   QoreQWidget *parent = o ? (QoreQWidget *)o->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);

   const AbstractQoreNode *p = get_param(params, 1);
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
   return new QoreBigIntNode(QMessageBox::warning(parent ? static_cast<QWidget *>(parent->getQWidget()) : 0, title, text, buttons, defaultButton));
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

   // static methods
   QC_QMessageBox->addStaticMethod("about",       f_QMessageBox_about);
   QC_QMessageBox->addStaticMethod("aboutQt",     f_QMessageBox_aboutQt);
   QC_QMessageBox->addStaticMethod("critical",    f_QMessageBox_critical);
   QC_QMessageBox->addStaticMethod("information", f_QMessageBox_information);
   QC_QMessageBox->addStaticMethod("question",    f_QMessageBox_question);
   QC_QMessageBox->addStaticMethod("warning",     f_QMessageBox_warning);

   return QC_QMessageBox;
}
