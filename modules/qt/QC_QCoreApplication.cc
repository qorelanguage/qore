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

int CID_QCOREAPPLICATION;
class QoreClass *QC_QCoreApplication = 0;

//QCoreApplication ( int & argc, char ** argv )
static void QCOREAPPLICATION_constructor(QoreObject *self, const QoreNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_QCOREAPPLICATION, new QoreQCoreApplication(self));
}

static void QCOREAPPLICATION_copy(class QoreObject *self, class QoreObject *old, class QoreQCoreApplication *qca, ExceptionSink *xsink)
{
   xsink->raiseException("QCOREAPPLICATION-COPY-ERROR", "objects of this class cannot be copied");
}

/*
//bool filterEvent ( void * message, long * result )
static QoreNode *QCOREAPPLICATION_filterEvent(QoreObject *self, QoreAbstractQCoreApplication *qca, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   ??? void* message = p;
   p = get_param(params, 1);
   ??? long* result = p;
   return new QoreNode(qca->getQCoreApplication()->filterEvent(message, result));
}
*/

//virtual bool notify ( QObject * receiver, QEvent * event )
static QoreNode *QCOREAPPLICATION_notify(QoreObject *self, QoreAbstractQCoreApplication *qca, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreAbstractQObject *receiver = (p && p->type == NT_OBJECT) ? (QoreAbstractQObject *)p->val.object->getReferencedPrivateData(CID_QOBJECT, xsink) : 0;
   if (!receiver) {
      if (!xsink->isException())
         xsink->raiseException("QCOREAPPLICATION-NOTIFY-PARAM-ERROR", "expecting a QObject object as first argument to QCoreApplication::notify()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> receiverHolder(static_cast<AbstractPrivateData *>(receiver), xsink);
   p = get_param(params, 1);
   QoreQEvent *event = (p && p->type == NT_OBJECT) ? (QoreQEvent *)p->val.object->getReferencedPrivateData(CID_QEVENT, xsink) : 0;
   if (!event) {
      if (!xsink->isException())
         xsink->raiseException("QCOREAPPLICATION-NOTIFY-PARAM-ERROR", "expecting a QEvent object as second argument to QCoreApplication::notify()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> eventHolder(static_cast<AbstractPrivateData *>(event), xsink);
   return new QoreNode(qca->getQCoreApplication()->notify(receiver->getQObject(), static_cast<QEvent *>(event)));
}

//EventFilter setEventFilter ( EventFilter filter )
static QoreNode *QCOREAPPLICATION_setEventFilter(QoreObject *self, QoreAbstractQCoreApplication *qca, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QCoreApplication::EventFilter filter = (QCoreApplication::EventFilter)(p ? p->getAsInt() : 0);
   return new QoreNode((int64)qca->getQCoreApplication()->setEventFilter(filter));
}

/*
//virtual bool winEventFilter ( MSG * msg, long * result )
static QoreNode *QCOREAPPLICATION_winEventFilter(QoreObject *self, QoreAbstractQCoreApplication *qca, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   ??? MSG* msg = p;
   p = get_param(params, 1);
   ??? long* result = p;
   return new QoreNode(qca->getQCoreApplication()->winEventFilter(msg, result));
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
   QC_QCoreApplication->addMethod("setEventFilter",              (q_method_t)QCOREAPPLICATION_setEventFilter);
   //QC_QCoreApplication->addMethod("winEventFilter",              (q_method_t)QCOREAPPLICATION_winEventFilter);

   return QC_QCoreApplication;
}

//void addLibraryPath ( const QString & path )
static QoreNode *f_QCoreApplication_addLibraryPath(const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString path;
   if (get_qstring(p, path, xsink))
      return 0;
   QCoreApplication::addLibraryPath(path);
   return 0;
}

//QString applicationDirPath ()
static QoreNode *f_QCoreApplication_applicationDirPath(const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(new QoreString(QCoreApplication::applicationDirPath().toUtf8().data(), QCS_UTF8));
}

//QString applicationFilePath ()
static QoreNode *f_QCoreApplication_applicationFilePath(const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(new QoreString(QCoreApplication::applicationFilePath().toUtf8().data(), QCS_UTF8));
}

//QString applicationName ()
static QoreNode *f_QCoreApplication_applicationName(const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(new QoreString(QCoreApplication::applicationName().toUtf8().data(), QCS_UTF8));
}

//QStringList arguments ()
static QoreNode *f_QCoreApplication_arguments(const QoreNode *params, ExceptionSink *xsink)
{
   QStringList strlist_rv = QCoreApplication::arguments();
   QoreList *l = new QoreList();
   for (QStringList::iterator i = strlist_rv.begin(), e = strlist_rv.end(); i != e; ++i)
      l->push(new QoreNode(new QoreString((*i).toUtf8().data(), QCS_UTF8)));
   return new QoreNode(l);
}

//bool closingDown ()
static QoreNode *f_QCoreApplication_closingDown(const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(QCoreApplication::closingDown());
}

//int exec ()
static QoreNode *f_QCoreApplication_exec(const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)QCoreApplication::exec());
}

//void exit ( int returnCode = 0 )
static QoreNode *f_QCoreApplication_exit(const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int returnCode = !is_nothing(p) ? p->getAsInt() : 0;
   QCoreApplication::exit(returnCode);
   return 0;
}

//void flush ()
static QoreNode *f_QCoreApplication_flush(const QoreNode *params, ExceptionSink *xsink)
{
   QCoreApplication::flush();
   return 0;
}

//bool hasPendingEvents ()
static QoreNode *f_QCoreApplication_hasPendingEvents(const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(QCoreApplication::hasPendingEvents());
}

//void installTranslator ( QTranslator * translationFile )
static QoreNode *f_QCoreApplication_installTranslator(const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQTranslator *translationFile = (p && p->type == NT_OBJECT) ? (QoreQTranslator *)p->val.object->getReferencedPrivateData(CID_QTRANSLATOR, xsink) : 0;
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
static QoreNode *f_QCoreApplication_instance(const QoreNode *params, ExceptionSink *xsink)
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
   return new QoreNode(rv_obj);
}

//QStringList libraryPaths ()
static QoreNode *f_QCoreApplication_libraryPaths(const QoreNode *params, ExceptionSink *xsink)
{
   QStringList strlist_rv = QCoreApplication::libraryPaths();
   QoreList *l = new QoreList();
   for (QStringList::iterator i = strlist_rv.begin(), e = strlist_rv.end(); i != e; ++i)
      l->push(new QoreNode(new QoreString((*i).toUtf8().data(), QCS_UTF8)));
   return new QoreNode(l);
}

//QString organizationDomain ()
static QoreNode *f_QCoreApplication_organizationDomain(const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(new QoreString(QCoreApplication::organizationDomain().toUtf8().data(), QCS_UTF8));
}

//QString organizationName ()
static QoreNode *f_QCoreApplication_organizationName(const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(new QoreString(QCoreApplication::organizationName().toUtf8().data(), QCS_UTF8));
}

//void postEvent ( QObject * receiver, QEvent * event )
//void postEvent ( QObject * receiver, QEvent * event, int priority )
static QoreNode *f_QCoreApplication_postEvent(const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_OBJECT, 0);
   QoreAbstractQObject *receiver = p ? (QoreAbstractQObject *)p->val.object->getReferencedPrivateData(CID_QOBJECT, xsink) : 0;
   if (!receiver) {
      if (!xsink->isException())
	 xsink->raiseException("QCOREAPPLICATION-POSTEVENT-PARAM-ERROR", "QCoreApplication::postEvent() was expecting a QObject passed as the first argument");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> receiverHolder(static_cast<AbstractPrivateData *>(receiver), xsink);
   p = get_param(params, 1);
   QoreQEvent *event = (p && p->type == NT_OBJECT) ? (QoreQEvent *)p->val.object->getReferencedPrivateData(CID_QEVENT, xsink) : 0;
   if (!event) {
      if (!xsink->isException())
	 xsink->raiseException("QCOREAPPLICATION-POSTEVENT-PARAM-ERROR", "this version of QCoreApplication::postEvent() was expecting a QEvent as the second argument");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> eventHolder(static_cast<AbstractPrivateData *>(event), xsink);
   if (num_params(params) > 2) {
      p = get_param(params, 2);
      int priority = p ? p->getAsInt() : 0;
      QCoreApplication::postEvent(receiver->getQObject(), static_cast<QEvent *>(event), priority);
   }
   else
      QCoreApplication::postEvent(receiver->getQObject(), static_cast<QEvent *>(event));
   return 0;
}

//void processEvents ( QEventLoop::ProcessEventsFlags flags = QEventLoop::AllEvents )
//void processEvents ( QEventLoop::ProcessEventsFlags flags, int maxtime )
static QoreNode *f_QCoreApplication_processEvents(const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
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
static QoreNode *f_QCoreApplication_removeLibraryPath(const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString path;
   if (get_qstring(p, path, xsink))
      return 0;
   QCoreApplication::removeLibraryPath(path);
   return 0;
}

//void removePostedEvents ( QObject * receiver )
//void removePostedEvents ( QObject * receiver, int eventType )
static QoreNode *f_QCoreApplication_removePostedEvents(const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_OBJECT, 0);
   QoreAbstractQObject *receiver = p ? (QoreAbstractQObject *)p->val.object->getReferencedPrivateData(CID_QOBJECT, xsink) : 0;
   if (!receiver) {
      if (!xsink->isException())
	 xsink->raiseException("QCOREAPPLICATION-REMOVEPOSTEDEVENTS-PARAM-ERROR", "QCoreApplication::removePostedEvents() was expecting a QObject as the first argument");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> receiverHolder(static_cast<AbstractPrivateData *>(receiver), xsink);
   if (num_params(params) > 1) {
      p = get_param(params, 1);
      int eventType = p ? p->getAsInt() : 0;
      QCoreApplication::removePostedEvents(receiver->getQObject(), eventType);
   }
   else
      QCoreApplication::removePostedEvents(receiver->getQObject());
   return 0;
}

//void removeTranslator ( QTranslator * translationFile )
static QoreNode *f_QCoreApplication_removeTranslator(const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQTranslator *translationFile = (p && p->type == NT_OBJECT) ? (QoreQTranslator *)p->val.object->getReferencedPrivateData(CID_QTRANSLATOR, xsink) : 0;
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
static QoreNode *f_QCoreApplication_sendEvent(const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreAbstractQObject *receiver = (p && p->type == NT_OBJECT) ? (QoreAbstractQObject *)p->val.object->getReferencedPrivateData(CID_QOBJECT, xsink) : 0;
   if (!receiver) {
      if (!xsink->isException())
         xsink->raiseException("QCOREAPPLICATION-SENDEVENT-PARAM-ERROR", "expecting a QObject object as first argument to QCoreApplication::sendEvent()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> receiverHolder(static_cast<AbstractPrivateData *>(receiver), xsink);
   p = get_param(params, 1);
   QoreQEvent *event = (p && p->type == NT_OBJECT) ? (QoreQEvent *)p->val.object->getReferencedPrivateData(CID_QEVENT, xsink) : 0;
   if (!event) {
      if (!xsink->isException())
         xsink->raiseException("QCOREAPPLICATION-SENDEVENT-PARAM-ERROR", "expecting a QEvent object as second argument to QCoreApplication::sendEvent()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> eventHolder(static_cast<AbstractPrivateData *>(event), xsink);
   return new QoreNode(QCoreApplication::sendEvent(receiver->getQObject(), static_cast<QEvent *>(event)));
}

//void sendPostedEvents ( QObject * receiver, int event_type )
//void sendPostedEvents ()
static QoreNode *f_QCoreApplication_sendPostedEvents(const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      QCoreApplication::sendPostedEvents();
      return 0;
   }
   QoreAbstractQObject *receiver = (p && p->type == NT_OBJECT) ? (QoreAbstractQObject *)p->val.object->getReferencedPrivateData(CID_QOBJECT, xsink) : 0;
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
static QoreNode *f_QCoreApplication_setApplicationName(const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString application;
   if (get_qstring(p, application, xsink))
      return 0;
   QCoreApplication::setApplicationName(application);
   return 0;
}

//void setAttribute ( Qt::ApplicationAttribute attribute, bool on = true )
static QoreNode *f_QCoreApplication_setAttribute(const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   Qt::ApplicationAttribute attribute = (Qt::ApplicationAttribute)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   bool on = !is_nothing(p) ? p->getAsBool() : true;
   QCoreApplication::setAttribute(attribute, on);
   return 0;
}

//void setLibraryPaths ( const QStringList & paths )
static QoreNode *f_QCoreApplication_setLibraryPaths(const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (!p || p->type != NT_LIST) {
      xsink->raiseException("QCOREAPPLICATION-SETLIBRARYPATHS-PARAM-ERROR", "expecting a list as first argument to QCoreApplication::setLibraryPaths()");
      return 0;
   }
   QStringList paths;
   ListIterator li_paths(p->val.list);
   while (li_paths.next())
   {
      QoreNodeTypeHelper str(li_paths.getValue(), NT_STRING, xsink);
      if (*xsink)
         return 0;
      QString tmp;
      if (get_qstring(*str, tmp, xsink))
         return 0;
      paths.push_back(tmp);
   }
   QCoreApplication::setLibraryPaths(paths);
   return 0;
}

//void setOrganizationDomain ( const QString & orgDomain )
static QoreNode *f_QCoreApplication_setOrganizationDomain(const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString orgDomain;
   if (get_qstring(p, orgDomain, xsink))
      return 0;
   QCoreApplication::setOrganizationDomain(orgDomain);
   return 0;
}

//void setOrganizationName ( const QString & orgName )
static QoreNode *f_QCoreApplication_setOrganizationName(const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString orgName;
   if (get_qstring(p, orgName, xsink))
      return 0;
   QCoreApplication::setOrganizationName(orgName);
   return 0;
}

//bool startingUp ()
static QoreNode *f_QCoreApplication_startingUp(const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(QCoreApplication::startingUp());
}

//bool testAttribute ( Qt::ApplicationAttribute attribute )
static QoreNode *f_QCoreApplication_testAttribute(const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   Qt::ApplicationAttribute attribute = (Qt::ApplicationAttribute)(p ? p->getAsInt() : 0);
   return new QoreNode(QCoreApplication::testAttribute(attribute));
}

//QString translate ( const char * context, const char * sourceText, const char * comment, Encoding encoding, int n )
//QString translate ( const char * context, const char * sourceText, const char * comment = 0, Encoding encoding = CodecForTr )
static QoreNode *f_QCoreApplication_translate(const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (!p || p->type != NT_STRING) {
      xsink->raiseException("QCOREAPPLICATION-TRANSLATE-PARAM-ERROR", "expecting a string as first argument to QCoreApplication::translate()");
      return 0;
   }
   const char *context = p->val.String->getBuffer();
   p = get_param(params, 1);
   if (!p || p->type != NT_STRING) {
      xsink->raiseException("QCOREAPPLICATION-TRANSLATE-PARAM-ERROR", "expecting a string as second argument to QCoreApplication::translate()");
      return 0;
   }
   const char *sourceText = p->val.String->getBuffer();
   p = test_param(params, NT_STRING, 2);
   const char *comment = p ? p->val.String->getBuffer() : 0;
   p = get_param(params, 3);
   QCoreApplication::Encoding encoding = (QCoreApplication::Encoding)(p ? p->getAsInt() : QCoreApplication::CodecForTr);
   if (num_params(params) < 5)
      return new QoreNode(new QoreString(QCoreApplication::translate(context, sourceText, comment, encoding).toUtf8().data(), QCS_UTF8));

   p = get_param(params, 4);
   int n = p ? p->getAsInt() : 0;
   return new QoreNode(new QoreString(QCoreApplication::translate(context, sourceText, comment, encoding, n).toUtf8().data(), QCS_UTF8));
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
