/*
 QC_QCoreApplication.cc
 
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

#include "QC_QCoreApplication.h"
#include "QC_QObject.h"
#include "QC_QTranslator.h"

#include "qore-qt.h"

int CID_QCOREAPPLICATION;
class QoreClass *QC_QCoreApplication = 0;

//QCoreApplication ( int & argc, char ** argv )
static void QCOREAPPLICATION_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_QCOREAPPLICATION, new QoreQCoreApplication(self));
}

static void QCOREAPPLICATION_copy(class QoreObject *self, class QoreObject *old, class QoreQCoreApplication *qca, ExceptionSink *xsink)
{
   xsink->raiseException("QCOREAPPLICATION-COPY-ERROR", "objects of this class cannot be copied");
}

/*
//bool filterEvent ( void * message, long * result )
static AbstractQoreNode *QCOREAPPLICATION_filterEvent(QoreObject *self, QoreAbstractQCoreApplication *qca, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? void* message = p;
   p = get_param(params, 1);
   ??? long* result = p;
   return new QoreBoolNode(qca->getQCoreApplication()->filterEvent(message, result));
}
*/

//virtual bool notify ( QObject * receiver, QEvent * event )
static AbstractQoreNode *QCOREAPPLICATION_notify(QoreObject *self, QoreAbstractQCoreApplication *qca, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreObject *o = test_object_param(params, 0);
   QoreAbstractQObject *receiver = o ? (QoreAbstractQObject *)o->getReferencedPrivateData(CID_QOBJECT, xsink) : 0;
   if (!receiver) {
      if (!xsink->isException())
         xsink->raiseException("QCOREAPPLICATION-NOTIFY-PARAM-ERROR", "expecting a QObject object as first argument to QCoreApplication::notify()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> receiverHolder(static_cast<AbstractPrivateData *>(receiver), xsink);

   o = test_object_param(params, 1);
   QoreQEvent *event = o ? (QoreQEvent *)o->getReferencedPrivateData(CID_QEVENT, xsink) : 0;
   if (!event) {
      if (!xsink->isException())
         xsink->raiseException("QCOREAPPLICATION-NOTIFY-PARAM-ERROR", "expecting a QEvent object as second argument to QCoreApplication::notify()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> eventHolder(static_cast<AbstractPrivateData *>(event), xsink);
   return new QoreBoolNode(qca->getQCoreApplication()->notify(receiver->getQObject(), static_cast<QEvent *>(event)));
}

//EventFilter setEventFilter ( EventFilter filter )
/*
static AbstractQoreNode *QCOREAPPLICATION_setEventFilter(QoreObject *self, QoreAbstractQCoreApplication *qca, const QoreListNode *params, ExceptionSink *xsink)
{
}
*/

/*
//virtual bool winEventFilter ( MSG * msg, long * result )
static AbstractQoreNode *QCOREAPPLICATION_winEventFilter(QoreObject *self, QoreAbstractQCoreApplication *qca, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? MSG* msg = p;
   p = get_param(params, 1);
   ??? long* result = p;
   return new QoreBoolNode(qca->getQCoreApplication()->winEventFilter(msg, result));
}
*/

QoreClass *initQCoreApplicationClass(QoreClass *qobject)
{
   QC_QCoreApplication = new QoreClass("QCoreApplication", QDOM_GUI);
   CID_QCOREAPPLICATION = QC_QCoreApplication->getID();

   QC_QCoreApplication->addBuiltinVirtualBaseClass(qobject);

   QC_QCoreApplication->setConstructor(QCOREAPPLICATION_constructor);
   QC_QCoreApplication->setCopy((q_copy_t)QCOREAPPLICATION_copy);

   //QC_QCoreApplication->addMethod("filterEvent",                 (q_method_t)QCOREAPPLICATION_filterEvent);
   QC_QCoreApplication->addMethod("notify",                      (q_method_t)QCOREAPPLICATION_notify);
   //QC_QCoreApplication->addMethod("setEventFilter",              (q_method_t)QCOREAPPLICATION_setEventFilter);
   //QC_QCoreApplication->addMethod("winEventFilter",              (q_method_t)QCOREAPPLICATION_winEventFilter);

   return QC_QCoreApplication;
}

//void addLibraryPath ( const QString & path )
static AbstractQoreNode *f_QCoreApplication_addLibraryPath(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QString path;
   if (get_qstring(p, path, xsink))
      return 0;
   QCoreApplication::addLibraryPath(path);
   return 0;
}

//QString applicationDirPath ()
static AbstractQoreNode *f_QCoreApplication_applicationDirPath(const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(QCoreApplication::applicationDirPath().toUtf8().data(), QCS_UTF8);
}

//QString applicationFilePath ()
static AbstractQoreNode *f_QCoreApplication_applicationFilePath(const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(QCoreApplication::applicationFilePath().toUtf8().data(), QCS_UTF8);
}

//QString applicationName ()
static AbstractQoreNode *f_QCoreApplication_applicationName(const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(QCoreApplication::applicationName().toUtf8().data(), QCS_UTF8);
}

//QStringList arguments ()
static AbstractQoreNode *f_QCoreApplication_arguments(const QoreListNode *params, ExceptionSink *xsink)
{
   QStringList strlist_rv = QCoreApplication::arguments();
   QoreListNode *l = new QoreListNode();
   for (QStringList::iterator i = strlist_rv.begin(), e = strlist_rv.end(); i != e; ++i)
      l->push(new QoreStringNode((*i).toUtf8().data(), QCS_UTF8));
   return l;
}

//bool closingDown ()
static AbstractQoreNode *f_QCoreApplication_closingDown(const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(QCoreApplication::closingDown());
}

//int exec ()
static AbstractQoreNode *f_QCoreApplication_exec(const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(QCoreApplication::exec());
}

//void exit ( int returnCode = 0 )
static AbstractQoreNode *f_QCoreApplication_exit(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int returnCode = !is_nothing(p) ? p->getAsInt() : 0;
   QCoreApplication::exit(returnCode);
   return 0;
}

//void flush ()
static AbstractQoreNode *f_QCoreApplication_flush(const QoreListNode *params, ExceptionSink *xsink)
{
   QCoreApplication::flush();
   return 0;
}

//bool hasPendingEvents ()
static AbstractQoreNode *f_QCoreApplication_hasPendingEvents(const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(QCoreApplication::hasPendingEvents());
}

//void installTranslator ( QTranslator * translationFile )
static AbstractQoreNode *f_QCoreApplication_installTranslator(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreObject *p = test_object_param(params, 0);
   QoreQTranslator *translationFile = p ? (QoreQTranslator *)p->getReferencedPrivateData(CID_QTRANSLATOR, xsink) : 0;
   if (!translationFile) {
      if (!xsink->isException())
         xsink->raiseException("QCOREAPPLICATION-INSTALLTRANSLATOR-PARAM-ERROR", "expecting a QTranslator object as first argument to QCoreApplication::installTranslator()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> translationFileHolder(static_cast<AbstractPrivateData *>(translationFile), xsink);
   QCoreApplication::installTranslator(static_cast<QTranslator *>(translationFile->qobj));
   return 0;
}

//QCoreApplication * instance ()
static AbstractQoreNode *f_QCoreApplication_instance(const QoreListNode *params, ExceptionSink *xsink)
{
   QCoreApplication *qt_qobj = QCoreApplication::instance();
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   QoreObject *rv_obj = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   if (rv_obj)
      rv_obj->ref();
   else {
      rv_obj = new QoreObject(QC_QCoreApplication, getProgram());
      QoreQtQCoreApplication *t_qobj = new QoreQtQCoreApplication(rv_obj, qt_qobj);
      rv_obj->setPrivate(CID_QCOREAPPLICATION, t_qobj);
   }
   return rv_obj;
}

//QStringList libraryPaths ()
static AbstractQoreNode *f_QCoreApplication_libraryPaths(const QoreListNode *params, ExceptionSink *xsink)
{
   QStringList strlist_rv = QCoreApplication::libraryPaths();
   QoreListNode *l = new QoreListNode();
   for (QStringList::iterator i = strlist_rv.begin(), e = strlist_rv.end(); i != e; ++i)
      l->push(new QoreStringNode((*i).toUtf8().data(), QCS_UTF8));
   return l;
}

//QString organizationDomain ()
static AbstractQoreNode *f_QCoreApplication_organizationDomain(const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(QCoreApplication::organizationDomain().toUtf8().data(), QCS_UTF8);
}

//QString organizationName ()
static AbstractQoreNode *f_QCoreApplication_organizationName(const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(QCoreApplication::organizationName().toUtf8().data(), QCS_UTF8);
}

//void postEvent ( QObject * receiver, QEvent * event )
//void postEvent ( QObject * receiver, QEvent * event, int priority )
static AbstractQoreNode *f_QCoreApplication_postEvent(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreObject *o = test_object_param(params, 0);
   QoreAbstractQObject *receiver = o ? (QoreAbstractQObject *)o->getReferencedPrivateData(CID_QOBJECT, xsink) : 0;
   if (!receiver) {
      if (!xsink->isException())
	 xsink->raiseException("QCOREAPPLICATION-POSTEVENT-PARAM-ERROR", "QCoreApplication::postEvent() was expecting a QObject passed as the first argument");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> receiverHolder(static_cast<AbstractPrivateData *>(receiver), xsink);

   o = test_object_param(params, 1);
   QoreQEvent *event = o ? (QoreQEvent *)o->getReferencedPrivateData(CID_QEVENT, xsink) : 0;
   if (!event) {
      if (!xsink->isException())
	 xsink->raiseException("QCOREAPPLICATION-POSTEVENT-PARAM-ERROR", "this version of QCoreApplication::postEvent() was expecting a QEvent as the second argument");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> eventHolder(static_cast<AbstractPrivateData *>(event), xsink);

   if (num_params(params) > 2) {
      const AbstractQoreNode *p = get_param(params, 2);
      int priority = p ? p->getAsInt() : 0;
      QCoreApplication::postEvent(receiver->getQObject(), static_cast<QEvent *>(event), priority);
   }
   else
      QCoreApplication::postEvent(receiver->getQObject(), static_cast<QEvent *>(event));
   return 0;
}

//void processEvents ( QEventLoop::ProcessEventsFlags flags = QEventLoop::AllEvents )
//void processEvents ( QEventLoop::ProcessEventsFlags flags, int maxtime )
static AbstractQoreNode *f_QCoreApplication_processEvents(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      QCoreApplication::processEvents();
      return 0;
   }
   QEventLoop::ProcessEventsFlags flags = (QEventLoop::ProcessEventsFlags)(p ? p->getAsInt() : QEventLoop::AllEvents);
   if (num_params(params) > 1) {
      p = get_param(params, 1);
      int maxtime = p ? p->getAsInt() : 0;
      QCoreApplication::processEvents(flags, maxtime);
   }
   else
      QCoreApplication::processEvents(flags);
   return 0;
}

//void removeLibraryPath ( const QString & path )
static AbstractQoreNode *f_QCoreApplication_removeLibraryPath(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QString path;
   if (get_qstring(p, path, xsink))
      return 0;
   QCoreApplication::removeLibraryPath(path);
   return 0;
}

//void removePostedEvents ( QObject * receiver )
//void removePostedEvents ( QObject * receiver, int eventType )
static AbstractQoreNode *f_QCoreApplication_removePostedEvents(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreObject *o = test_object_param(params, 0);
   QoreAbstractQObject *receiver = o ? (QoreAbstractQObject *)o->getReferencedPrivateData(CID_QOBJECT, xsink) : 0;
   if (!receiver) {
      if (!xsink->isException())
	 xsink->raiseException("QCOREAPPLICATION-REMOVEPOSTEDEVENTS-PARAM-ERROR", "QCoreApplication::removePostedEvents() was expecting a QObject as the first argument");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> receiverHolder(static_cast<AbstractPrivateData *>(receiver), xsink);
   if (num_params(params) > 1) {
      const AbstractQoreNode *p = get_param(params, 1);
      int eventType = p ? p->getAsInt() : 0;
      QCoreApplication::removePostedEvents(receiver->getQObject(), eventType);
   }
   else
      QCoreApplication::removePostedEvents(receiver->getQObject());
   return 0;
}

//void removeTranslator ( QTranslator * translationFile )
static AbstractQoreNode *f_QCoreApplication_removeTranslator(const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreQTranslator *translationFile = p ? (QoreQTranslator *)p->getReferencedPrivateData(CID_QTRANSLATOR, xsink) : 0;
   if (!translationFile) {
      if (!xsink->isException())
         xsink->raiseException("QCOREAPPLICATION-REMOVETRANSLATOR-PARAM-ERROR", "expecting a QTranslator object as first argument to QCoreApplication::removeTranslator()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> translationFileHolder(static_cast<AbstractPrivateData *>(translationFile), xsink);
   QCoreApplication::removeTranslator(static_cast<QTranslator *>(translationFile->qobj));
   return 0;
}

//bool sendEvent ( QObject * receiver, QEvent * event )
static AbstractQoreNode *f_QCoreApplication_sendEvent(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreObject *p = test_object_param(params, 0);
   QoreAbstractQObject *receiver = p ? (QoreAbstractQObject *)p->getReferencedPrivateData(CID_QOBJECT, xsink) : 0;
   if (!receiver) {
      if (!xsink->isException())
         xsink->raiseException("QCOREAPPLICATION-SENDEVENT-PARAM-ERROR", "expecting a QObject object as first argument to QCoreApplication::sendEvent()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> receiverHolder(static_cast<AbstractPrivateData *>(receiver), xsink);

   p = test_object_param(params, 1);
   QoreQEvent *event = p ? (QoreQEvent *)p->getReferencedPrivateData(CID_QEVENT, xsink) : 0;
   if (!event) {
      if (!xsink->isException())
         xsink->raiseException("QCOREAPPLICATION-SENDEVENT-PARAM-ERROR", "expecting a QEvent object as second argument to QCoreApplication::sendEvent()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> eventHolder(static_cast<AbstractPrivateData *>(event), xsink);
   return new QoreBoolNode(QCoreApplication::sendEvent(receiver->getQObject(), static_cast<QEvent *>(event)));
}

//void sendPostedEvents ( QObject * receiver, int event_type )
//void sendPostedEvents ()
static AbstractQoreNode *f_QCoreApplication_sendPostedEvents(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      QCoreApplication::sendPostedEvents();
      return 0;
   }
   const QoreObject *o = dynamic_cast<const QoreObject *>(p);
   QoreAbstractQObject *receiver = o ? (QoreAbstractQObject *)o->getReferencedPrivateData(CID_QOBJECT, xsink) : 0;
   if (!receiver) {
      if (!xsink->isException())
         xsink->raiseException("QCOREAPPLICATION-SENDPOSTEDEVENTS-PARAM-ERROR", "this version of QCoreApplication::sendPostedEvents() expects an object derived from QObject as the first argument");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> receiverHolder(static_cast<AbstractPrivateData *>(receiver), xsink);

   p = get_param(params, 1);
   int event_type = p ? p->getAsInt() : 0;
   QCoreApplication::sendPostedEvents(receiver->getQObject(), event_type);
   return 0;
}

//void setApplicationName ( const QString & application )
static AbstractQoreNode *f_QCoreApplication_setApplicationName(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QString application;
   if (get_qstring(p, application, xsink))
      return 0;
   QCoreApplication::setApplicationName(application);
   return 0;
}

//void setAttribute ( Qt::ApplicationAttribute attribute, bool on = true )
static AbstractQoreNode *f_QCoreApplication_setAttribute(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   Qt::ApplicationAttribute attribute = (Qt::ApplicationAttribute)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   bool on = !is_nothing(p) ? p->getAsBool() : true;
   QCoreApplication::setAttribute(attribute, on);
   return 0;
}

//void setLibraryPaths ( const QStringList & paths )
static AbstractQoreNode *f_QCoreApplication_setLibraryPaths(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreListNode *p = test_list_param(params, 0);
   if (!p) {
      xsink->raiseException("QCOREAPPLICATION-SETLIBRARYPATHS-PARAM-ERROR", "expecting a list as first argument to QCoreApplication::setLibraryPaths()");
      return 0;
   }
   QStringList paths;
   ConstListIterator li_paths(p);
   while (li_paths.next())
   {
      QoreStringNodeValueHelper str(li_paths.getValue());
      QString tmp;
      if (get_qstring(*str, tmp, xsink))
         return 0;
      paths.push_back(tmp);
   }
   QCoreApplication::setLibraryPaths(paths);
   return 0;
}

//void setOrganizationDomain ( const QString & orgDomain )
static AbstractQoreNode *f_QCoreApplication_setOrganizationDomain(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QString orgDomain;
   if (get_qstring(p, orgDomain, xsink))
      return 0;
   QCoreApplication::setOrganizationDomain(orgDomain);
   return 0;
}

//void setOrganizationName ( const QString & orgName )
static AbstractQoreNode *f_QCoreApplication_setOrganizationName(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QString orgName;
   if (get_qstring(p, orgName, xsink))
      return 0;
   QCoreApplication::setOrganizationName(orgName);
   return 0;
}

//bool startingUp ()
static AbstractQoreNode *f_QCoreApplication_startingUp(const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(QCoreApplication::startingUp());
}

//bool testAttribute ( Qt::ApplicationAttribute attribute )
static AbstractQoreNode *f_QCoreApplication_testAttribute(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   Qt::ApplicationAttribute attribute = (Qt::ApplicationAttribute)(p ? p->getAsInt() : 0);
   return new QoreBoolNode(QCoreApplication::testAttribute(attribute));
}

//QString translate ( const char * context, const char * sourceText, const char * comment, Encoding encoding, int n )
//QString translate ( const char * context, const char * sourceText, const char * comment = 0, Encoding encoding = CodecForTr )
static AbstractQoreNode *f_QCoreApplication_translate(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *p = test_string_param(params, 0);
   if (!p) {
      xsink->raiseException("QCOREAPPLICATION-TRANSLATE-PARAM-ERROR", "expecting a string as first argument to QCoreApplication::translate()");
      return 0;
   }
   const char *context = p->getBuffer();

   p = test_string_param(params, 1);
   if (!p) {
      xsink->raiseException("QCOREAPPLICATION-TRANSLATE-PARAM-ERROR", "expecting a string as second argument to QCoreApplication::translate()");
      return 0;
   }
   const char *sourceText = p->getBuffer();

   p = test_string_param(params, 2);
   const char *comment = p ? p->getBuffer() : 0;

   const AbstractQoreNode *pn = get_param(params, 3);
   QCoreApplication::Encoding encoding = (QCoreApplication::Encoding)(pn ? pn->getAsInt() : QCoreApplication::CodecForTr);
   if (num_params(params) < 5)
      return new QoreStringNode(QCoreApplication::translate(context, sourceText, comment, encoding).toUtf8().data(), QCS_UTF8);

   pn = get_param(params, 4);
   int n = pn ? pn->getAsInt() : 0;
   return new QoreStringNode(QCoreApplication::translate(context, sourceText, comment, encoding, n).toUtf8().data(), QCS_UTF8);
}

void initQCoreApplicationStaticFunctions()
{
   builtinFunctions.add("QCoreApplication_addLibraryPath",               f_QCoreApplication_addLibraryPath);
   builtinFunctions.add("QCoreApplication_applicationDirPath",           f_QCoreApplication_applicationDirPath);
   builtinFunctions.add("QCoreApplication_applicationFilePath",          f_QCoreApplication_applicationFilePath);
   builtinFunctions.add("QCoreApplication_applicationName",              f_QCoreApplication_applicationName);
   builtinFunctions.add("QCoreApplication_arguments",                    f_QCoreApplication_arguments);
   builtinFunctions.add("QCoreApplication_closingDown",                  f_QCoreApplication_closingDown);
   builtinFunctions.add("QCoreApplication_exec",                         f_QCoreApplication_exec);
   builtinFunctions.add("QCoreApplication_exit",                         f_QCoreApplication_exit);
   builtinFunctions.add("QCoreApplication_flush",                        f_QCoreApplication_flush);
   builtinFunctions.add("QCoreApplication_hasPendingEvents",             f_QCoreApplication_hasPendingEvents);
   builtinFunctions.add("QCoreApplication_installTranslator",            f_QCoreApplication_installTranslator);
   builtinFunctions.add("QCoreApplication_instance",                     f_QCoreApplication_instance);
   builtinFunctions.add("QCoreApplication_libraryPaths",                 f_QCoreApplication_libraryPaths);
   builtinFunctions.add("QCoreApplication_organizationDomain",           f_QCoreApplication_organizationDomain);
   builtinFunctions.add("QCoreApplication_organizationName",             f_QCoreApplication_organizationName);
   builtinFunctions.add("QCoreApplication_postEvent",                    f_QCoreApplication_postEvent);
   builtinFunctions.add("QCoreApplication_processEvents",                f_QCoreApplication_processEvents);
   builtinFunctions.add("QCoreApplication_removeLibraryPath",            f_QCoreApplication_removeLibraryPath);
   builtinFunctions.add("QCoreApplication_removePostedEvents",           f_QCoreApplication_removePostedEvents);
   builtinFunctions.add("QCoreApplication_removeTranslator",             f_QCoreApplication_removeTranslator);
   builtinFunctions.add("QCoreApplication_sendEvent",                    f_QCoreApplication_sendEvent);
   builtinFunctions.add("QCoreApplication_sendPostedEvents",             f_QCoreApplication_sendPostedEvents);
   builtinFunctions.add("QCoreApplication_setApplicationName",           f_QCoreApplication_setApplicationName);
   builtinFunctions.add("QCoreApplication_setAttribute",                 f_QCoreApplication_setAttribute);
   builtinFunctions.add("QCoreApplication_setLibraryPaths",              f_QCoreApplication_setLibraryPaths);
   builtinFunctions.add("QCoreApplication_setOrganizationDomain",        f_QCoreApplication_setOrganizationDomain);
   builtinFunctions.add("QCoreApplication_setOrganizationName",          f_QCoreApplication_setOrganizationName);
   builtinFunctions.add("QCoreApplication_startingUp",                   f_QCoreApplication_startingUp);
   builtinFunctions.add("QCoreApplication_testAttribute",                f_QCoreApplication_testAttribute);
   builtinFunctions.add("QCoreApplication_translate",                    f_QCoreApplication_translate);
}
