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
static void QWIZARD_constructor(QoreObject *self, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQWidget *parent = (p && p->type == NT_OBJECT) ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return;
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   p = get_param(params, 1);
   Qt::WindowFlags flags = (Qt::WindowFlags)(!is_nothing(p) ? p->getAsInt() : 0);
   self->setPrivate(CID_QWIZARD, new QoreQWizard(self, parent ? static_cast<QWidget *>(parent->getQWidget()) : 0, flags));
   return;
}

static void QWIZARD_copy(class QoreObject *self, class QoreObject *old, class QoreAbstractQWizard *qw, ExceptionSink *xsink)
{
   xsink->raiseException("QWIZARD-COPY-ERROR", "objects of this class cannot be copied");
}

//int addPage ( QWizardPage * page )
static QoreNode *QWIZARD_addPage(QoreObject *self, QoreAbstractQWizard *qw, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreAbstractQWizardPage *page = (p && p->type == NT_OBJECT) ? (QoreAbstractQWizardPage *)p->val.object->getReferencedPrivateData(CID_QWIZARDPAGE, xsink) : 0;
   if (!page) {
      if (!xsink->isException())
         xsink->raiseException("QWIZARD-ADDPAGE-PARAM-ERROR", "expecting a QWizardPage object as first argument to QWizard::addPage()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> pageHolder(static_cast<AbstractPrivateData *>(page), xsink);
   return new QoreNode((int64)qw->getQWizard()->addPage(page->getQWizardPage()));
}

//QAbstractButton * button ( WizardButton which ) const
static QoreNode *QWIZARD_button(QoreObject *self, QoreAbstractQWizard *qw, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
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
   return new QoreNode(rv_obj);
}

//QString buttonText ( WizardButton which ) const
static QoreNode *QWIZARD_buttonText(QoreObject *self, QoreAbstractQWizard *qw, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QWizard::WizardButton which = (QWizard::WizardButton)(p ? p->getAsInt() : 0);
   return new QoreStringNode(qw->getQWizard()->buttonText(which).toUtf8().data(), QCS_UTF8);
}

//int currentId () const
static QoreNode *QWIZARD_currentId(QoreObject *self, QoreAbstractQWizard *qw, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qw->getQWizard()->currentId());
}

//QWizardPage * currentPage () const
static QoreNode *QWIZARD_currentPage(QoreObject *self, QoreAbstractQWizard *qw, const QoreList *params, ExceptionSink *xsink)
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
   return new QoreNode(rv_obj);
}

//QVariant field ( const QString & name ) const
static QoreNode *QWIZARD_field(QoreObject *self, QoreAbstractQWizard *qw, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString name;
   if (get_qstring(p, name, xsink))
      return 0;
   return return_qvariant(qw->getQWizard()->field(name));
}

//bool hasVisitedPage ( int id ) const
static QoreNode *QWIZARD_hasVisitedPage(QoreObject *self, QoreAbstractQWizard *qw, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int id = p ? p->getAsInt() : 0;
   return new QoreNode(qw->getQWizard()->hasVisitedPage(id));
}

//virtual int nextId () const
static QoreNode *QWIZARD_nextId(QoreObject *self, QoreAbstractQWizard *qw, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qw->getQWizard()->nextId());
}

//WizardOptions options () const
static QoreNode *QWIZARD_options(QoreObject *self, QoreAbstractQWizard *qw, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qw->getQWizard()->options());
}

//QWizardPage * page ( int id ) const
static QoreNode *QWIZARD_page(QoreObject *self, QoreAbstractQWizard *qw, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
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
   return new QoreNode(rv_obj);
}

//QPixmap pixmap ( WizardPixmap which ) const
static QoreNode *QWIZARD_pixmap(QoreObject *self, QoreAbstractQWizard *qw, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QWizard::WizardPixmap which = (QWizard::WizardPixmap)(p ? p->getAsInt() : 0);
   QoreObject *o_qp = new QoreObject(QC_QPixmap, getProgram());
   QoreQPixmap *q_qp = new QoreQPixmap(qw->getQWizard()->pixmap(which));
   o_qp->setPrivate(CID_QPIXMAP, q_qp);
   return new QoreNode(o_qp);
}

//void setButton ( WizardButton which, QAbstractButton * button )
static QoreNode *QWIZARD_setButton(QoreObject *self, QoreAbstractQWizard *qw, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QWizard::WizardButton which = (QWizard::WizardButton)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QoreQAbstractButton *button = (p && p->type == NT_OBJECT) ? (QoreQAbstractButton *)p->val.object->getReferencedPrivateData(CID_QABSTRACTBUTTON, xsink) : 0;
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
static QoreNode *QWIZARD_setButtonLayout(QoreObject *self, QoreAbstractQWizard *qw, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   ??? QList<WizardButton> layout = p;
   qw->getQWizard()->setButtonLayout(layout);
   return 0;
}
*/

//void setButtonText ( WizardButton which, const QString & text )
static QoreNode *QWIZARD_setButtonText(QoreObject *self, QoreAbstractQWizard *qw, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QWizard::WizardButton which = (QWizard::WizardButton)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QString text;
   if (get_qstring(p, text, xsink))
      return 0;
   qw->getQWizard()->setButtonText(which, text);
   return 0;
}

//void setDefaultProperty ( const char * className, const char * property, const char * changedSignal )
static QoreNode *QWIZARD_setDefaultProperty(QoreObject *self, QoreAbstractQWizard *qw, const QoreList *params, ExceptionSink *xsink)
{
   QoreStringNode *str = test_string_param(params, 0);
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
static QoreNode *QWIZARD_setField(QoreObject *self, QoreAbstractQWizard *qw, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
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
static QoreNode *QWIZARD_setOption(QoreObject *self, QoreAbstractQWizard *qw, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QWizard::WizardOption option = (QWizard::WizardOption)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   bool on = !is_nothing(p) ? p->getAsBool() : true;
   qw->getQWizard()->setOption(option, on);
   return 0;
}

//void setOptions ( WizardOptions options )
static QoreNode *QWIZARD_setOptions(QoreObject *self, QoreAbstractQWizard *qw, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QWizard::WizardOptions options = (QWizard::WizardOptions)(p ? p->getAsInt() : 0);
   qw->getQWizard()->setOptions(options);
   return 0;
}

//void setPage ( int id, QWizardPage * page )
static QoreNode *QWIZARD_setPage(QoreObject *self, QoreAbstractQWizard *qw, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int id = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   QoreAbstractQWizardPage *page = (p && p->type == NT_OBJECT) ? (QoreAbstractQWizardPage *)p->val.object->getReferencedPrivateData(CID_QWIZARDPAGE, xsink) : 0;
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
static QoreNode *QWIZARD_setPixmap(QoreObject *self, QoreAbstractQWizard *qw, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QWizard::WizardPixmap which = (QWizard::WizardPixmap)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QoreQPixmap *pixmap = (p && p->type == NT_OBJECT) ? (QoreQPixmap *)p->val.object->getReferencedPrivateData(CID_QPIXMAP, xsink) : 0;
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
static QoreNode *QWIZARD_setStartId(QoreObject *self, QoreAbstractQWizard *qw, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int id = p ? p->getAsInt() : 0;
   qw->getQWizard()->setStartId(id);
   return 0;
}

//void setSubTitleFormat ( Qt::TextFormat format )
static QoreNode *QWIZARD_setSubTitleFormat(QoreObject *self, QoreAbstractQWizard *qw, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   Qt::TextFormat format = (Qt::TextFormat)(p ? p->getAsInt() : 0);
   qw->getQWizard()->setSubTitleFormat(format);
   return 0;
}

//void setTitleFormat ( Qt::TextFormat format )
static QoreNode *QWIZARD_setTitleFormat(QoreObject *self, QoreAbstractQWizard *qw, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   Qt::TextFormat format = (Qt::TextFormat)(p ? p->getAsInt() : 0);
   qw->getQWizard()->setTitleFormat(format);
   return 0;
}

//void setWizardStyle ( WizardStyle style )
static QoreNode *QWIZARD_setWizardStyle(QoreObject *self, QoreAbstractQWizard *qw, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QWizard::WizardStyle style = (QWizard::WizardStyle)(p ? p->getAsInt() : 0);
   qw->getQWizard()->setWizardStyle(style);
   return 0;
}

//int startId () const
static QoreNode *QWIZARD_startId(QoreObject *self, QoreAbstractQWizard *qw, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qw->getQWizard()->startId());
}

//Qt::TextFormat subTitleFormat () const
static QoreNode *QWIZARD_subTitleFormat(QoreObject *self, QoreAbstractQWizard *qw, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qw->getQWizard()->subTitleFormat());
}

//bool testOption ( WizardOption option ) const
static QoreNode *QWIZARD_testOption(QoreObject *self, QoreAbstractQWizard *qw, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QWizard::WizardOption option = (QWizard::WizardOption)(p ? p->getAsInt() : 0);
   return new QoreNode(qw->getQWizard()->testOption(option));
}

//Qt::TextFormat titleFormat () const
static QoreNode *QWIZARD_titleFormat(QoreObject *self, QoreAbstractQWizard *qw, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qw->getQWizard()->titleFormat());
}

//virtual bool validateCurrentPage ()
static QoreNode *QWIZARD_validateCurrentPage(QoreObject *self, QoreAbstractQWizard *qw, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode(qw->getQWizard()->validateCurrentPage());
}

//QList<int> visitedPages () const
static QoreNode *QWIZARD_visitedPages(QoreObject *self, QoreAbstractQWizard *qw, const QoreList *params, ExceptionSink *xsink)
{
   QList<int> ilist_rv = qw->getQWizard()->visitedPages();
   QoreList *l = new QoreList();
   for (QList<int>::iterator i = ilist_rv.begin(), e = ilist_rv.end(); i != e; ++i)
      l->push(new QoreNode((int64)(*i)));
   return l;
}

//WizardStyle wizardStyle () const
static QoreNode *QWIZARD_wizardStyle(QoreObject *self, QoreAbstractQWizard *qw, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qw->getQWizard()->wizardStyle());
}

//void back () const
static QoreNode *QWIZARD_back(QoreObject *self, QoreAbstractQWizard *qw, const QoreList *params, ExceptionSink *xsink)
{
   qw->getQWizard()->back();
   return 0;
}

//void next ()
static QoreNode *QWIZARD_next(QoreObject *self, QoreAbstractQWizard *qw, const QoreList *params, ExceptionSink *xsink)
{
   qw->getQWizard()->next();
   return 0;
}

//void restart ()
static QoreNode *QWIZARD_restart(QoreObject *self, QoreAbstractQWizard *qw, const QoreList *params, ExceptionSink *xsink)
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
   ns->addConstant("BackButton",               new QoreNode((int64)QWizard::BackButton));
   ns->addConstant("NextButton",               new QoreNode((int64)QWizard::NextButton));
   ns->addConstant("CommitButton",             new QoreNode((int64)QWizard::CommitButton));
   ns->addConstant("FinishButton",             new QoreNode((int64)QWizard::FinishButton));
   ns->addConstant("CancelButton",             new QoreNode((int64)QWizard::CancelButton));
   ns->addConstant("HelpButton",               new QoreNode((int64)QWizard::HelpButton));
   ns->addConstant("CustomButton1",            new QoreNode((int64)QWizard::CustomButton1));
   ns->addConstant("CustomButton2",            new QoreNode((int64)QWizard::CustomButton2));
   ns->addConstant("CustomButton3",            new QoreNode((int64)QWizard::CustomButton3));
   ns->addConstant("Stretch",                  new QoreNode((int64)QWizard::Stretch));
   ns->addConstant("NoButton",                 new QoreNode((int64)QWizard::NoButton));
   ns->addConstant("NStandardButtons",         new QoreNode((int64)QWizard::NStandardButtons));
   ns->addConstant("NButtons",                 new QoreNode((int64)QWizard::NButtons));

   // WizardPixmap enum
   ns->addConstant("WatermarkPixmap",          new QoreNode((int64)QWizard::WatermarkPixmap));
   ns->addConstant("LogoPixmap",               new QoreNode((int64)QWizard::LogoPixmap));
   ns->addConstant("BannerPixmap",             new QoreNode((int64)QWizard::BannerPixmap));
   ns->addConstant("BackgroundPixmap",         new QoreNode((int64)QWizard::BackgroundPixmap));
   ns->addConstant("NPixmaps",                 new QoreNode((int64)QWizard::NPixmaps));

   // WizardStyle enum
   ns->addConstant("ClassicStyle",             new QoreNode((int64)QWizard::ClassicStyle));
   ns->addConstant("ModernStyle",              new QoreNode((int64)QWizard::ModernStyle));
   ns->addConstant("MacStyle",                 new QoreNode((int64)QWizard::MacStyle));
   ns->addConstant("AeroStyle",                new QoreNode((int64)QWizard::AeroStyle));
   ns->addConstant("NStyles",                  new QoreNode((int64)QWizard::NStyles));

   // WizardOption enum
   ns->addConstant("IndependentPages",         new QoreNode((int64)QWizard::IndependentPages));
   ns->addConstant("IgnoreSubTitles",          new QoreNode((int64)QWizard::IgnoreSubTitles));
   ns->addConstant("ExtendedWatermarkPixmap",  new QoreNode((int64)QWizard::ExtendedWatermarkPixmap));
   ns->addConstant("NoDefaultButton",          new QoreNode((int64)QWizard::NoDefaultButton));
   ns->addConstant("NoBackButtonOnStartPage",  new QoreNode((int64)QWizard::NoBackButtonOnStartPage));
   ns->addConstant("NoBackButtonOnLastPage",   new QoreNode((int64)QWizard::NoBackButtonOnLastPage));
   ns->addConstant("DisabledBackButtonOnLastPage", new QoreNode((int64)QWizard::DisabledBackButtonOnLastPage));
   ns->addConstant("HaveNextButtonOnLastPage", new QoreNode((int64)QWizard::HaveNextButtonOnLastPage));
   ns->addConstant("HaveFinishButtonOnEarlyPages", new QoreNode((int64)QWizard::HaveFinishButtonOnEarlyPages));
   ns->addConstant("NoCancelButton",           new QoreNode((int64)QWizard::NoCancelButton));
   ns->addConstant("CancelButtonOnLeft",       new QoreNode((int64)QWizard::CancelButtonOnLeft));
   ns->addConstant("HaveHelpButton",           new QoreNode((int64)QWizard::HaveHelpButton));
   ns->addConstant("HelpButtonOnRight",        new QoreNode((int64)QWizard::HelpButtonOnRight));
   ns->addConstant("HaveCustomButton1",        new QoreNode((int64)QWizard::HaveCustomButton1));
   ns->addConstant("HaveCustomButton2",        new QoreNode((int64)QWizard::HaveCustomButton2));
   ns->addConstant("HaveCustomButton3",        new QoreNode((int64)QWizard::HaveCustomButton3));

   return ns;
}
