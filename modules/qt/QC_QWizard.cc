/*
 QC_QWizard.cc
 
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

#include "QC_QWizard.h"
#include "QC_QWidget.h"
#include "QC_QWizardPage.h"
#include "QC_QAbstractButton.h"
#include "QC_QPixmap.h"

#include "qore-qt.h"

int CID_QWIZARD;
class QoreClass *QC_QWizard = 0;

//QWizard ( QWidget * parent = 0, Qt::WindowFlags flags = 0 )
static void QWIZARD_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreObject *o = test_object_param(params, 0);
   QoreQWidget *parent = o ? (QoreQWidget *)o->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return;
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   const AbstractQoreNode *p = get_param(params, 1);
   Qt::WindowFlags flags = (Qt::WindowFlags)(!is_nothing(p) ? p->getAsInt() : 0);
   self->setPrivate(CID_QWIZARD, new QoreQWizard(self, parent ? static_cast<QWidget *>(parent->getQWidget()) : 0, flags));
   return;
}

static void QWIZARD_copy(class QoreObject *self, class QoreObject *old, class QoreAbstractQWizard *qw, ExceptionSink *xsink)
{
   xsink->raiseException("QWIZARD-COPY-ERROR", "objects of this class cannot be copied");
}

//int addPage ( QWizardPage * page )
static AbstractQoreNode *QWIZARD_addPage(QoreObject *self, QoreAbstractQWizard *qw, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreObject *o = test_object_param(params, 0);
   QoreAbstractQWizardPage *page = o ? (QoreAbstractQWizardPage *)o->getReferencedPrivateData(CID_QWIZARDPAGE, xsink) : 0;
   if (!page) {
      if (!xsink->isException())
         xsink->raiseException("QWIZARD-ADDPAGE-PARAM-ERROR", "expecting a QWizardPage object as first argument to QWizard::addPage()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> pageHolder(static_cast<AbstractPrivateData *>(page), xsink);
   return new QoreBigIntNode(qw->getQWizard()->addPage(page->getQWizardPage()));
}

//QAbstractButton * button ( WizardButton which ) const
static AbstractQoreNode *QWIZARD_button(QoreObject *self, QoreAbstractQWizard *qw, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QWizard::WizardButton which = (QWizard::WizardButton)(p ? p->getAsInt() : 0);
   QAbstractButton *qt_qobj = qw->getQWizard()->button(which);
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   QoreObject *rv_obj = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   if (rv_obj)
      rv_obj->ref();
   else {
      rv_obj = new QoreObject(QC_QAbstractButton, getProgram());
      QoreQtQAbstractButton *t_qobj = new QoreQtQAbstractButton(rv_obj, qt_qobj);
      rv_obj->setPrivate(CID_QABSTRACTBUTTON, t_qobj);
   }
   return rv_obj;
}

//QString buttonText ( WizardButton which ) const
static AbstractQoreNode *QWIZARD_buttonText(QoreObject *self, QoreAbstractQWizard *qw, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QWizard::WizardButton which = (QWizard::WizardButton)(p ? p->getAsInt() : 0);
   return new QoreStringNode(qw->getQWizard()->buttonText(which).toUtf8().data(), QCS_UTF8);
}

//int currentId () const
static AbstractQoreNode *QWIZARD_currentId(QoreObject *self, QoreAbstractQWizard *qw, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qw->getQWizard()->currentId());
}

//QWizardPage * currentPage () const
static AbstractQoreNode *QWIZARD_currentPage(QoreObject *self, QoreAbstractQWizard *qw, const QoreListNode *params, ExceptionSink *xsink)
{
   QWizardPage *qt_qobj = qw->getQWizard()->currentPage();
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   QoreObject *rv_obj = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   if (rv_obj)
      rv_obj->ref();
   else {
      rv_obj = new QoreObject(QC_QWizardPage, getProgram());
      QoreQtQWizardPage *t_qobj = new QoreQtQWizardPage(rv_obj, qt_qobj);
      rv_obj->setPrivate(CID_QWIZARDPAGE, t_qobj);
   }
   return rv_obj;
}

//QVariant field ( const QString & name ) const
static AbstractQoreNode *QWIZARD_field(QoreObject *self, QoreAbstractQWizard *qw, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QString name;
   if (get_qstring(p, name, xsink))
      return 0;
   return return_qvariant(qw->getQWizard()->field(name));
}

//bool hasVisitedPage ( int id ) const
static AbstractQoreNode *QWIZARD_hasVisitedPage(QoreObject *self, QoreAbstractQWizard *qw, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int id = p ? p->getAsInt() : 0;
   return get_bool_node(qw->getQWizard()->hasVisitedPage(id));
}

//virtual int nextId () const
static AbstractQoreNode *QWIZARD_nextId(QoreObject *self, QoreAbstractQWizard *qw, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qw->getQWizard()->nextId());
}

//WizardOptions options () const
static AbstractQoreNode *QWIZARD_options(QoreObject *self, QoreAbstractQWizard *qw, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qw->getQWizard()->options());
}

//QWizardPage * page ( int id ) const
static AbstractQoreNode *QWIZARD_page(QoreObject *self, QoreAbstractQWizard *qw, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int id = p ? p->getAsInt() : 0;
   QWizardPage *qt_qobj = qw->getQWizard()->page(id);
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   QoreObject *rv_obj = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   if (rv_obj)
      rv_obj->ref();
   else {
      rv_obj = new QoreObject(QC_QWizardPage, getProgram());
      QoreQtQWizardPage *t_qobj = new QoreQtQWizardPage(rv_obj, qt_qobj);
      rv_obj->setPrivate(CID_QWIZARDPAGE, t_qobj);
   }
   return rv_obj;
}

//QPixmap pixmap ( WizardPixmap which ) const
static AbstractQoreNode *QWIZARD_pixmap(QoreObject *self, QoreAbstractQWizard *qw, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QWizard::WizardPixmap which = (QWizard::WizardPixmap)(p ? p->getAsInt() : 0);
   QoreObject *o_qp = new QoreObject(QC_QPixmap, getProgram());
   QoreQPixmap *q_qp = new QoreQPixmap(qw->getQWizard()->pixmap(which));
   o_qp->setPrivate(CID_QPIXMAP, q_qp);
   return o_qp;
}

//void setButton ( WizardButton which, QAbstractButton * button )
static AbstractQoreNode *QWIZARD_setButton(QoreObject *self, QoreAbstractQWizard *qw, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QWizard::WizardButton which = (QWizard::WizardButton)(p ? p->getAsInt() : 0);
   const QoreObject *o = test_object_param(params, 1);
   QoreQAbstractButton *button = o ? (QoreQAbstractButton *)o->getReferencedPrivateData(CID_QABSTRACTBUTTON, xsink) : 0;
   if (!button) {
      if (!xsink->isException())
         xsink->raiseException("QWIZARD-SETBUTTON-PARAM-ERROR", "expecting a QAbstractButton object as second argument to QWizard::setButton()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> buttonHolder(static_cast<AbstractPrivateData *>(button), xsink);
   qw->getQWizard()->setButton(which, static_cast<QAbstractButton *>(button->qobj));
   return 0;
}

/*
//void setButtonLayout ( const QList<WizardButton> & layout )
static AbstractQoreNode *QWIZARD_setButtonLayout(QoreObject *self, QoreAbstractQWizard *qw, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? QList<WizardButton> layout = p;
   qw->getQWizard()->setButtonLayout(layout);
   return 0;
}
*/

//void setButtonText ( WizardButton which, const QString & text )
static AbstractQoreNode *QWIZARD_setButtonText(QoreObject *self, QoreAbstractQWizard *qw, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QWizard::WizardButton which = (QWizard::WizardButton)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QString text;
   if (get_qstring(p, text, xsink))
      return 0;
   qw->getQWizard()->setButtonText(which, text);
   return 0;
}

//void setDefaultProperty ( const char * className, const char * property, const char * changedSignal )
static AbstractQoreNode *QWIZARD_setDefaultProperty(QoreObject *self, QoreAbstractQWizard *qw, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *str = test_string_param(params, 0);
   if (!str) {
      xsink->raiseException("QWIZARD-SETDEFAULTPROPERTY-PARAM-ERROR", "expecting a string as first argument to QWizard::setDefaultProperty()");
      return 0;
   }
   const char *className = str->getBuffer();

   str = test_string_param(params, 1);
   if (!str) {
      xsink->raiseException("QWIZARD-SETDEFAULTPROPERTY-PARAM-ERROR", "expecting a string as second argument to QWizard::setDefaultProperty()");
      return 0;
   }
   const char *property = str->getBuffer();

   str = test_string_param(params, 2);
   if (!str) {
      xsink->raiseException("QWIZARD-SETDEFAULTPROPERTY-PARAM-ERROR", "expecting a string as third argument to QWizard::setDefaultProperty()");
      return 0;
   }
   const char *changedSignal = str->getBuffer();

   qw->getQWizard()->setDefaultProperty(className, property, changedSignal);
   return 0;
}

//void setField ( const QString & name, const QVariant & value )
static AbstractQoreNode *QWIZARD_setField(QoreObject *self, QoreAbstractQWizard *qw, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QString name;
   if (get_qstring(p, name, xsink))
      return 0;
   p = get_param(params, 1);
   QVariant value;
   if (get_qvariant(p, value, xsink))
      return 0;
   qw->getQWizard()->setField(name, value);
   return 0;
}

//void setOption ( WizardOption option, bool on = true )
static AbstractQoreNode *QWIZARD_setOption(QoreObject *self, QoreAbstractQWizard *qw, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QWizard::WizardOption option = (QWizard::WizardOption)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   bool on = !is_nothing(p) ? p->getAsBool() : true;
   qw->getQWizard()->setOption(option, on);
   return 0;
}

//void setOptions ( WizardOptions options )
static AbstractQoreNode *QWIZARD_setOptions(QoreObject *self, QoreAbstractQWizard *qw, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QWizard::WizardOptions options = (QWizard::WizardOptions)(p ? p->getAsInt() : 0);
   qw->getQWizard()->setOptions(options);
   return 0;
}

//void setPage ( int id, QWizardPage * page )
static AbstractQoreNode *QWIZARD_setPage(QoreObject *self, QoreAbstractQWizard *qw, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int id = p ? p->getAsInt() : 0;
   const QoreObject *o = test_object_param(params, 1);
   QoreAbstractQWizardPage *page = o ? (QoreAbstractQWizardPage *)o->getReferencedPrivateData(CID_QWIZARDPAGE, xsink) : 0;
   if (!page) {
      if (!xsink->isException())
         xsink->raiseException("QWIZARD-SETPAGE-PARAM-ERROR", "expecting a QWizardPage object as second argument to QWizard::setPage()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> pageHolder(static_cast<AbstractPrivateData *>(page), xsink);
   qw->getQWizard()->setPage(id, page->getQWizardPage());
   return 0;
}

//void setPixmap ( WizardPixmap which, const QPixmap & pixmap )
static AbstractQoreNode *QWIZARD_setPixmap(QoreObject *self, QoreAbstractQWizard *qw, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QWizard::WizardPixmap which = (QWizard::WizardPixmap)(p ? p->getAsInt() : 0);
   const QoreObject *o = test_object_param(params, 1);
   QoreQPixmap *pixmap = o ? (QoreQPixmap *)o->getReferencedPrivateData(CID_QPIXMAP, xsink) : 0;
   if (!pixmap) {
      if (!xsink->isException())
         xsink->raiseException("QWIZARD-SETPIXMAP-PARAM-ERROR", "expecting a QPixmap object as second argument to QWizard::setPixmap()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> pixmapHolder(static_cast<AbstractPrivateData *>(pixmap), xsink);
   qw->getQWizard()->setPixmap(which, *(static_cast<QPixmap *>(pixmap)));
   return 0;
}

//void setStartId ( int id )
static AbstractQoreNode *QWIZARD_setStartId(QoreObject *self, QoreAbstractQWizard *qw, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int id = p ? p->getAsInt() : 0;
   qw->getQWizard()->setStartId(id);
   return 0;
}

//void setSubTitleFormat ( Qt::TextFormat format )
static AbstractQoreNode *QWIZARD_setSubTitleFormat(QoreObject *self, QoreAbstractQWizard *qw, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   Qt::TextFormat format = (Qt::TextFormat)(p ? p->getAsInt() : 0);
   qw->getQWizard()->setSubTitleFormat(format);
   return 0;
}

//void setTitleFormat ( Qt::TextFormat format )
static AbstractQoreNode *QWIZARD_setTitleFormat(QoreObject *self, QoreAbstractQWizard *qw, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   Qt::TextFormat format = (Qt::TextFormat)(p ? p->getAsInt() : 0);
   qw->getQWizard()->setTitleFormat(format);
   return 0;
}

//void setWizardStyle ( WizardStyle style )
static AbstractQoreNode *QWIZARD_setWizardStyle(QoreObject *self, QoreAbstractQWizard *qw, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QWizard::WizardStyle style = (QWizard::WizardStyle)(p ? p->getAsInt() : 0);
   qw->getQWizard()->setWizardStyle(style);
   return 0;
}

//int startId () const
static AbstractQoreNode *QWIZARD_startId(QoreObject *self, QoreAbstractQWizard *qw, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qw->getQWizard()->startId());
}

//Qt::TextFormat subTitleFormat () const
static AbstractQoreNode *QWIZARD_subTitleFormat(QoreObject *self, QoreAbstractQWizard *qw, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qw->getQWizard()->subTitleFormat());
}

//bool testOption ( WizardOption option ) const
static AbstractQoreNode *QWIZARD_testOption(QoreObject *self, QoreAbstractQWizard *qw, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QWizard::WizardOption option = (QWizard::WizardOption)(p ? p->getAsInt() : 0);
   return get_bool_node(qw->getQWizard()->testOption(option));
}

//Qt::TextFormat titleFormat () const
static AbstractQoreNode *QWIZARD_titleFormat(QoreObject *self, QoreAbstractQWizard *qw, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qw->getQWizard()->titleFormat());
}

//virtual bool validateCurrentPage ()
static AbstractQoreNode *QWIZARD_validateCurrentPage(QoreObject *self, QoreAbstractQWizard *qw, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qw->getQWizard()->validateCurrentPage());
}

//QList<int> visitedPages () const
static AbstractQoreNode *QWIZARD_visitedPages(QoreObject *self, QoreAbstractQWizard *qw, const QoreListNode *params, ExceptionSink *xsink)
{
   QList<int> ilist_rv = qw->getQWizard()->visitedPages();
   QoreListNode *l = new QoreListNode();
   for (QList<int>::iterator i = ilist_rv.begin(), e = ilist_rv.end(); i != e; ++i)
      l->push(new QoreBigIntNode((*i)));
   return l;
}

//WizardStyle wizardStyle () const
static AbstractQoreNode *QWIZARD_wizardStyle(QoreObject *self, QoreAbstractQWizard *qw, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qw->getQWizard()->wizardStyle());
}

//void back () const
static AbstractQoreNode *QWIZARD_back(QoreObject *self, QoreAbstractQWizard *qw, const QoreListNode *params, ExceptionSink *xsink)
{
   qw->getQWizard()->back();
   return 0;
}

//void next ()
static AbstractQoreNode *QWIZARD_next(QoreObject *self, QoreAbstractQWizard *qw, const QoreListNode *params, ExceptionSink *xsink)
{
   qw->getQWizard()->next();
   return 0;
}

//void restart ()
static AbstractQoreNode *QWIZARD_restart(QoreObject *self, QoreAbstractQWizard *qw, const QoreListNode *params, ExceptionSink *xsink)
{
   qw->getQWizard()->restart();
   return 0;
}

static QoreClass *initQWizardClass(QoreClass *qdialog)
{
   QC_QWizard = new QoreClass("QWizard", QDOM_GUI);
   CID_QWIZARD = QC_QWizard->getID();

   QC_QWizard->addBuiltinVirtualBaseClass(qdialog);

   QC_QWizard->setConstructor(QWIZARD_constructor);
   QC_QWizard->setCopy((q_copy_t)QWIZARD_copy);

   QC_QWizard->addMethod("addPage",                     (q_method_t)QWIZARD_addPage);
   QC_QWizard->addMethod("button",                      (q_method_t)QWIZARD_button);
   QC_QWizard->addMethod("buttonText",                  (q_method_t)QWIZARD_buttonText);
   QC_QWizard->addMethod("currentId",                   (q_method_t)QWIZARD_currentId);
   QC_QWizard->addMethod("currentPage",                 (q_method_t)QWIZARD_currentPage);
   QC_QWizard->addMethod("field",                       (q_method_t)QWIZARD_field);
   QC_QWizard->addMethod("hasVisitedPage",              (q_method_t)QWIZARD_hasVisitedPage);
   QC_QWizard->addMethod("nextId",                      (q_method_t)QWIZARD_nextId);
   QC_QWizard->addMethod("options",                     (q_method_t)QWIZARD_options);
   QC_QWizard->addMethod("page",                        (q_method_t)QWIZARD_page);
   QC_QWizard->addMethod("pixmap",                      (q_method_t)QWIZARD_pixmap);
   QC_QWizard->addMethod("setButton",                   (q_method_t)QWIZARD_setButton);
   //QC_QWizard->addMethod("setButtonLayout",             (q_method_t)QWIZARD_setButtonLayout);
   QC_QWizard->addMethod("setButtonText",               (q_method_t)QWIZARD_setButtonText);
   QC_QWizard->addMethod("setDefaultProperty",          (q_method_t)QWIZARD_setDefaultProperty);
   QC_QWizard->addMethod("setField",                    (q_method_t)QWIZARD_setField);
   QC_QWizard->addMethod("setOption",                   (q_method_t)QWIZARD_setOption);
   QC_QWizard->addMethod("setOptions",                  (q_method_t)QWIZARD_setOptions);
   QC_QWizard->addMethod("setPage",                     (q_method_t)QWIZARD_setPage);
   QC_QWizard->addMethod("setPixmap",                   (q_method_t)QWIZARD_setPixmap);
   QC_QWizard->addMethod("setStartId",                  (q_method_t)QWIZARD_setStartId);
   QC_QWizard->addMethod("setSubTitleFormat",           (q_method_t)QWIZARD_setSubTitleFormat);
   QC_QWizard->addMethod("setTitleFormat",              (q_method_t)QWIZARD_setTitleFormat);
   QC_QWizard->addMethod("setWizardStyle",              (q_method_t)QWIZARD_setWizardStyle);
   QC_QWizard->addMethod("startId",                     (q_method_t)QWIZARD_startId);
   QC_QWizard->addMethod("subTitleFormat",              (q_method_t)QWIZARD_subTitleFormat);
   QC_QWizard->addMethod("testOption",                  (q_method_t)QWIZARD_testOption);
   QC_QWizard->addMethod("titleFormat",                 (q_method_t)QWIZARD_titleFormat);
   QC_QWizard->addMethod("validateCurrentPage",         (q_method_t)QWIZARD_validateCurrentPage);
   QC_QWizard->addMethod("visitedPages",                (q_method_t)QWIZARD_visitedPages);
   QC_QWizard->addMethod("wizardStyle",                 (q_method_t)QWIZARD_wizardStyle);

   QC_QWizard->addMethod("back",                        (q_method_t)QWIZARD_back);
   QC_QWizard->addMethod("next",                        (q_method_t)QWIZARD_next);
   QC_QWizard->addMethod("restart",                     (q_method_t)QWIZARD_restart);

   return QC_QWizard;
}

QoreNamespace *initQWizardNS(class QoreClass *qdialog)
{
   QoreNamespace *ns = new QoreNamespace("QWizard");

   ns->addSystemClass(initQWizardClass(qdialog));

   // WizardButton enum
   ns->addConstant("BackButton",               new QoreBigIntNode(QWizard::BackButton));
   ns->addConstant("NextButton",               new QoreBigIntNode(QWizard::NextButton));
   ns->addConstant("CommitButton",             new QoreBigIntNode(QWizard::CommitButton));
   ns->addConstant("FinishButton",             new QoreBigIntNode(QWizard::FinishButton));
   ns->addConstant("CancelButton",             new QoreBigIntNode(QWizard::CancelButton));
   ns->addConstant("HelpButton",               new QoreBigIntNode(QWizard::HelpButton));
   ns->addConstant("CustomButton1",            new QoreBigIntNode(QWizard::CustomButton1));
   ns->addConstant("CustomButton2",            new QoreBigIntNode(QWizard::CustomButton2));
   ns->addConstant("CustomButton3",            new QoreBigIntNode(QWizard::CustomButton3));
   ns->addConstant("Stretch",                  new QoreBigIntNode(QWizard::Stretch));
   ns->addConstant("NoButton",                 new QoreBigIntNode(QWizard::NoButton));
   ns->addConstant("NStandardButtons",         new QoreBigIntNode(QWizard::NStandardButtons));
   ns->addConstant("NButtons",                 new QoreBigIntNode(QWizard::NButtons));

   // WizardPixmap enum
   ns->addConstant("WatermarkPixmap",          new QoreBigIntNode(QWizard::WatermarkPixmap));
   ns->addConstant("LogoPixmap",               new QoreBigIntNode(QWizard::LogoPixmap));
   ns->addConstant("BannerPixmap",             new QoreBigIntNode(QWizard::BannerPixmap));
   ns->addConstant("BackgroundPixmap",         new QoreBigIntNode(QWizard::BackgroundPixmap));
   ns->addConstant("NPixmaps",                 new QoreBigIntNode(QWizard::NPixmaps));

   // WizardStyle enum
   ns->addConstant("ClassicStyle",             new QoreBigIntNode(QWizard::ClassicStyle));
   ns->addConstant("ModernStyle",              new QoreBigIntNode(QWizard::ModernStyle));
   ns->addConstant("MacStyle",                 new QoreBigIntNode(QWizard::MacStyle));
   ns->addConstant("AeroStyle",                new QoreBigIntNode(QWizard::AeroStyle));
   ns->addConstant("NStyles",                  new QoreBigIntNode(QWizard::NStyles));

   // WizardOption enum
   ns->addConstant("IndependentPages",         new QoreBigIntNode(QWizard::IndependentPages));
   ns->addConstant("IgnoreSubTitles",          new QoreBigIntNode(QWizard::IgnoreSubTitles));
   ns->addConstant("ExtendedWatermarkPixmap",  new QoreBigIntNode(QWizard::ExtendedWatermarkPixmap));
   ns->addConstant("NoDefaultButton",          new QoreBigIntNode(QWizard::NoDefaultButton));
   ns->addConstant("NoBackButtonOnStartPage",  new QoreBigIntNode(QWizard::NoBackButtonOnStartPage));
   ns->addConstant("NoBackButtonOnLastPage",   new QoreBigIntNode(QWizard::NoBackButtonOnLastPage));
   ns->addConstant("DisabledBackButtonOnLastPage", new QoreBigIntNode(QWizard::DisabledBackButtonOnLastPage));
   ns->addConstant("HaveNextButtonOnLastPage", new QoreBigIntNode(QWizard::HaveNextButtonOnLastPage));
   ns->addConstant("HaveFinishButtonOnEarlyPages", new QoreBigIntNode(QWizard::HaveFinishButtonOnEarlyPages));
   ns->addConstant("NoCancelButton",           new QoreBigIntNode(QWizard::NoCancelButton));
   ns->addConstant("CancelButtonOnLeft",       new QoreBigIntNode(QWizard::CancelButtonOnLeft));
   ns->addConstant("HaveHelpButton",           new QoreBigIntNode(QWizard::HaveHelpButton));
   ns->addConstant("HelpButtonOnRight",        new QoreBigIntNode(QWizard::HelpButtonOnRight));
   ns->addConstant("HaveCustomButton1",        new QoreBigIntNode(QWizard::HaveCustomButton1));
   ns->addConstant("HaveCustomButton2",        new QoreBigIntNode(QWizard::HaveCustomButton2));
   ns->addConstant("HaveCustomButton3",        new QoreBigIntNode(QWizard::HaveCustomButton3));

   return ns;
}
