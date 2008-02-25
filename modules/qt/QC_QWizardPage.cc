/*
 QC_QWizardPage.cc
 
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

#include "QC_QWizardPage.h"
#include "QC_QWizard.h"
#include "QC_QWidget.h"
#include "QC_QPixmap.h"

#include "qore-qt.h"

int CID_QWIZARDPAGE;
class QoreClass *QC_QWizardPage = 0;

//QWizardPage ( QWidget * parent = 0 )
static void QWIZARDPAGE_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreObject *p = test_object_param(params, 0);
   QoreQWidget *parent = p ? (QoreQWidget *)p->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return;
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   self->setPrivate(CID_QWIZARDPAGE, new QoreQWizardPage(self, parent ? static_cast<QWidget *>(parent->getQWidget()) : 0));
   return;
}

static void QWIZARDPAGE_copy(class QoreObject *self, class QoreObject *old, class QoreQWizardPage *qwp, ExceptionSink *xsink)
{
   xsink->raiseException("QWIZARDPAGE-COPY-ERROR", "objects of this class cannot be copied");
}

//QString buttonText ( QWizard::WizardButton which ) const
static AbstractQoreNode *QWIZARDPAGE_buttonText(QoreObject *self, QoreAbstractQWizardPage *qwp, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QWizard::WizardButton which = (QWizard::WizardButton)(p ? p->getAsInt() : 0);
   return new QoreStringNode(qwp->getQWizardPage()->buttonText(which).toUtf8().data(), QCS_UTF8);
}

//virtual void cleanupPage ()
static AbstractQoreNode *QWIZARDPAGE_cleanupPage(QoreObject *self, QoreAbstractQWizardPage *qwp, const QoreListNode *params, ExceptionSink *xsink)
{
   qwp->getQWizardPage()->cleanupPage();
   return 0;
}

//virtual void initializePage ()
static AbstractQoreNode *QWIZARDPAGE_initializePage(QoreObject *self, QoreAbstractQWizardPage *qwp, const QoreListNode *params, ExceptionSink *xsink)
{
   qwp->getQWizardPage()->initializePage();
   return 0;
}

//bool isCommitPage () const
static AbstractQoreNode *QWIZARDPAGE_isCommitPage(QoreObject *self, QoreAbstractQWizardPage *qwp, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qwp->getQWizardPage()->isCommitPage());
}

//virtual bool isComplete () const
static AbstractQoreNode *QWIZARDPAGE_isComplete(QoreObject *self, QoreAbstractQWizardPage *qwp, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qwp->getQWizardPage()->isComplete());
}

//bool isFinalPage () const
static AbstractQoreNode *QWIZARDPAGE_isFinalPage(QoreObject *self, QoreAbstractQWizardPage *qwp, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qwp->getQWizardPage()->isFinalPage());
}

//virtual int nextId () const
static AbstractQoreNode *QWIZARDPAGE_nextId(QoreObject *self, QoreAbstractQWizardPage *qwp, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qwp->getQWizardPage()->nextId());
}

//QPixmap pixmap ( QWizard::WizardPixmap which ) const
static AbstractQoreNode *QWIZARDPAGE_pixmap(QoreObject *self, QoreAbstractQWizardPage *qwp, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QWizard::WizardPixmap which = (QWizard::WizardPixmap)(p ? p->getAsInt() : 0);
   QoreObject *o_qp = new QoreObject(QC_QPixmap, getProgram());
   QoreQPixmap *q_qp = new QoreQPixmap(qwp->getQWizardPage()->pixmap(which));
   o_qp->setPrivate(CID_QPIXMAP, q_qp);
   return o_qp;
}

//void setButtonText ( QWizard::WizardButton which, const QString & text )
static AbstractQoreNode *QWIZARDPAGE_setButtonText(QoreObject *self, QoreAbstractQWizardPage *qwp, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QWizard::WizardButton which = (QWizard::WizardButton)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QString text;
   if (get_qstring(p, text, xsink))
      return 0;
   qwp->getQWizardPage()->setButtonText(which, text);
   return 0;
}

//void setCommitPage ( bool commitPage )
static AbstractQoreNode *QWIZARDPAGE_setCommitPage(QoreObject *self, QoreAbstractQWizardPage *qwp, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   bool commitPage = p ? p->getAsBool() : false;
   qwp->getQWizardPage()->setCommitPage(commitPage);
   return 0;
}

//void setFinalPage ( bool finalPage )
static AbstractQoreNode *QWIZARDPAGE_setFinalPage(QoreObject *self, QoreAbstractQWizardPage *qwp, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   bool finalPage = p ? p->getAsBool() : false;
   qwp->getQWizardPage()->setFinalPage(finalPage);
   return 0;
}

//void setPixmap ( QWizard::WizardPixmap which, const QPixmap & pixmap )
static AbstractQoreNode *QWIZARDPAGE_setPixmap(QoreObject *self, QoreAbstractQWizardPage *qwp, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QWizard::WizardPixmap which = (QWizard::WizardPixmap)(p ? p->getAsInt() : 0);
   const QoreObject *o = test_object_param(params, 1);
   QoreQPixmap *pixmap = o ? (QoreQPixmap *)o->getReferencedPrivateData(CID_QPIXMAP, xsink) : 0;
   if (!pixmap) {
      if (!xsink->isException())
         xsink->raiseException("QWIZARDPAGE-SETPIXMAP-PARAM-ERROR", "expecting a QPixmap object as second argument to QWizardPage::setPixmap()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> pixmapHolder(static_cast<AbstractPrivateData *>(pixmap), xsink);
   qwp->getQWizardPage()->setPixmap(which, *(static_cast<QPixmap *>(pixmap)));
   return 0;
}

//void setSubTitle ( const QString & subTitle )
static AbstractQoreNode *QWIZARDPAGE_setSubTitle(QoreObject *self, QoreAbstractQWizardPage *qwp, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QString subTitle;
   if (get_qstring(p, subTitle, xsink))
      return 0;
   qwp->getQWizardPage()->setSubTitle(subTitle);
   return 0;
}

//void setTitle ( const QString & title )
static AbstractQoreNode *QWIZARDPAGE_setTitle(QoreObject *self, QoreAbstractQWizardPage *qwp, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QString title;
   if (get_qstring(p, title, xsink))
      return 0;
   qwp->getQWizardPage()->setTitle(title);
   return 0;
}

//QString subTitle () const
static AbstractQoreNode *QWIZARDPAGE_subTitle(QoreObject *self, QoreAbstractQWizardPage *qwp, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qwp->getQWizardPage()->subTitle().toUtf8().data(), QCS_UTF8);
}

//QString title () const
static AbstractQoreNode *QWIZARDPAGE_title(QoreObject *self, QoreAbstractQWizardPage *qwp, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qwp->getQWizardPage()->title().toUtf8().data(), QCS_UTF8);
}

//virtual bool validatePage ()
static AbstractQoreNode *QWIZARDPAGE_validatePage(QoreObject *self, QoreAbstractQWizardPage *qwp, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qwp->getQWizardPage()->validatePage());
}

//QVariant field ( const QString & name ) const
static AbstractQoreNode *QWIZARDPAGE_field(QoreObject *self, QoreAbstractQWizardPage *qwp, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QString name;
   if (get_qstring(p, name, xsink))
      return 0;
   return return_qvariant(qwp->field(name));
}

//void registerField ( const QString & name, QWidget * widget, const char * property = 0, const char * changedSignal = 0 )
static AbstractQoreNode *QWIZARDPAGE_registerField(QoreObject *self, QoreAbstractQWizardPage *qwp, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QString name;
   if (get_qstring(p, name, xsink))
      return 0;
   const QoreObject *o = test_object_param(params, 1);
   QoreQWidget *widget = o ? (QoreQWidget *)o->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!widget) {
      if (!xsink->isException())
         xsink->raiseException("QWIZARDPAGE-REGISTERFIELD-PARAM-ERROR", "expecting a QWidget object as second argument to QWizardPage::registerField()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   const QoreStringNode *str = test_string_param(params, 2);
   const char *property = str ? str->getBuffer() : 0;
   str = test_string_param(params, 3);
   const char *changedSignal = str ? str->getBuffer() : 0;
   qwp->registerField(name, static_cast<QWidget *>(widget->getQWidget()), property, changedSignal);
   return 0;
}

//void setField ( const QString & name, const QVariant & value )
static AbstractQoreNode *QWIZARDPAGE_setField(QoreObject *self, QoreAbstractQWizardPage *qwp, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QString name;
   if (get_qstring(p, name, xsink))
      return 0;
   p = get_param(params, 1);
   QVariant value;
   if (get_qvariant(p, value, xsink))
      return 0;
   qwp->setField(name, value);
   return 0;
}

//QWizard * wizard () const
static AbstractQoreNode *QWIZARDPAGE_wizard(QoreObject *self, QoreAbstractQWizardPage *qwp, const QoreListNode *params, ExceptionSink *xsink)
{
   QWizard *qt_qobj = qwp->wizard();
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   QoreObject *rv_obj = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   if (rv_obj)
      rv_obj->ref();
   else {
      rv_obj = new QoreObject(QC_QWizard, getProgram());
      QoreQtQWizard *t_qobj = new QoreQtQWizard(rv_obj, qt_qobj);
      rv_obj->setPrivate(CID_QWIZARD, t_qobj);
   }
   return rv_obj;
}

QoreClass *initQWizardPageClass(QoreClass *qwidget)
{
   QC_QWizardPage = new QoreClass("QWizardPage", QDOM_GUI);
   CID_QWIZARDPAGE = QC_QWizardPage->getID();

   QC_QWizardPage->addBuiltinVirtualBaseClass(qwidget);

   QC_QWizardPage->setConstructor(QWIZARDPAGE_constructor);
   QC_QWizardPage->setCopy((q_copy_t)QWIZARDPAGE_copy);

   QC_QWizardPage->addMethod("buttonText",                  (q_method_t)QWIZARDPAGE_buttonText);
   QC_QWizardPage->addMethod("cleanupPage",                 (q_method_t)QWIZARDPAGE_cleanupPage);
   QC_QWizardPage->addMethod("initializePage",              (q_method_t)QWIZARDPAGE_initializePage);
   QC_QWizardPage->addMethod("isCommitPage",                (q_method_t)QWIZARDPAGE_isCommitPage);
   QC_QWizardPage->addMethod("isComplete",                  (q_method_t)QWIZARDPAGE_isComplete);
   QC_QWizardPage->addMethod("isFinalPage",                 (q_method_t)QWIZARDPAGE_isFinalPage);
   QC_QWizardPage->addMethod("nextId",                      (q_method_t)QWIZARDPAGE_nextId);
   QC_QWizardPage->addMethod("pixmap",                      (q_method_t)QWIZARDPAGE_pixmap);
   QC_QWizardPage->addMethod("setButtonText",               (q_method_t)QWIZARDPAGE_setButtonText);
   QC_QWizardPage->addMethod("setCommitPage",               (q_method_t)QWIZARDPAGE_setCommitPage);
   QC_QWizardPage->addMethod("setFinalPage",                (q_method_t)QWIZARDPAGE_setFinalPage);
   QC_QWizardPage->addMethod("setPixmap",                   (q_method_t)QWIZARDPAGE_setPixmap);
   QC_QWizardPage->addMethod("setSubTitle",                 (q_method_t)QWIZARDPAGE_setSubTitle);
   QC_QWizardPage->addMethod("setTitle",                    (q_method_t)QWIZARDPAGE_setTitle);
   QC_QWizardPage->addMethod("subTitle",                    (q_method_t)QWIZARDPAGE_subTitle);
   QC_QWizardPage->addMethod("title",                       (q_method_t)QWIZARDPAGE_title);
   QC_QWizardPage->addMethod("validatePage",                (q_method_t)QWIZARDPAGE_validatePage);

   // protected methods
   QC_QWizardPage->addMethod("field",                       (q_method_t)QWIZARDPAGE_field, true);
   QC_QWizardPage->addMethod("registerField",               (q_method_t)QWIZARDPAGE_registerField, true);
   QC_QWizardPage->addMethod("setField",                    (q_method_t)QWIZARDPAGE_setField, true);
   QC_QWizardPage->addMethod("wizard",                      (q_method_t)QWIZARDPAGE_wizard, true);

   return QC_QWizardPage;
}
