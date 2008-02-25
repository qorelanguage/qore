/*
 QC_QFileDialog.cc
 
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

#include "QC_QFileDialog.h"
#include "QC_QWidget.h"
#include "QC_QDir.h"
#include "QC_QByteArray.h"
#include "QC_QAbstractItemDelegate.h"

#include "qore-qt.h"

int CID_QFILEDIALOG;
class QoreClass *QC_QFileDialog = 0;

//QFileDialog ( QWidget * parent, Qt::WindowFlags flags )
//QFileDialog ( QWidget * parent = 0, const QString & caption = QString(), const QString & directory = QString(), const QString & filter = QString() )
static void QFILEDIALOG_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QFILEDIALOG, new QoreQFileDialog(self));
      return;
   }

   const QoreObject *o = dynamic_cast<const QoreObject *>(p);
   QoreQWidget *parent = o ? (QoreQWidget *)o->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return;
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);

   p = get_param(params, 1);
   if (p && p->getType() == NT_STRING) {
      QString caption;
      if (get_qstring(p, caption, xsink, true))
	 caption = QString();
      p = get_param(params, 2);
      QString directory;
      if (get_qstring(p, directory, xsink, true))
	 directory = QString();
      p = get_param(params, 3);
      QString filter;
      if (get_qstring(p, filter, xsink, true))
	 filter = QString();

      self->setPrivate(CID_QFILEDIALOG, new QoreQFileDialog(self, parent ? static_cast<QWidget *>(parent->getQWidget()) : 0, caption, directory, filter));
      return;
   }

   Qt::WindowFlags flags = (Qt::WindowFlags)(p ? p->getAsInt() : 0);
   self->setPrivate(CID_QFILEDIALOG, new QoreQFileDialog(self, parent ? static_cast<QWidget *>(parent->getQWidget()) : 0, flags));
   return;
}

static void QFILEDIALOG_copy(class QoreObject *self, class QoreObject *old, class QoreQFileDialog *qfd, ExceptionSink *xsink)
{
   xsink->raiseException("QFILEDIALOG-COPY-ERROR", "objects of this class cannot be copied");
}

//AcceptMode acceptMode () const
static AbstractQoreNode *QFILEDIALOG_acceptMode(QoreObject *self, QoreQFileDialog *qfd, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qfd->qobj->acceptMode());
}

//bool confirmOverwrite () const
static AbstractQoreNode *QFILEDIALOG_confirmOverwrite(QoreObject *self, QoreQFileDialog *qfd, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qfd->qobj->confirmOverwrite());
}

//QString defaultSuffix () const
static AbstractQoreNode *QFILEDIALOG_defaultSuffix(QoreObject *self, QoreQFileDialog *qfd, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qfd->qobj->defaultSuffix().toUtf8().data(), QCS_UTF8);
}

//QDir directory () const
static AbstractQoreNode *QFILEDIALOG_directory(QoreObject *self, QoreQFileDialog *qfd, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qd = new QoreObject(QC_QDir, getProgram());
   QoreQDir *q_qd = new QoreQDir(qfd->qobj->directory());
   o_qd->setPrivate(CID_QDIR, q_qd);
   return o_qd;
}

//FileMode fileMode () const
static AbstractQoreNode *QFILEDIALOG_fileMode(QoreObject *self, QoreQFileDialog *qfd, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qfd->qobj->fileMode());
}

//QStringList filters () const
static AbstractQoreNode *QFILEDIALOG_filters(QoreObject *self, QoreQFileDialog *qfd, const QoreListNode *params, ExceptionSink *xsink)
{
   QStringList strlist_rv = qfd->qobj->filters();
   QoreListNode *l = new QoreListNode();
   for (QStringList::iterator i = strlist_rv.begin(), e = strlist_rv.end(); i != e; ++i)
      l->push(new QoreStringNode((*i).toUtf8().data(), QCS_UTF8));
   return l;
}

//QStringList history () const
static AbstractQoreNode *QFILEDIALOG_history(QoreObject *self, QoreQFileDialog *qfd, const QoreListNode *params, ExceptionSink *xsink)
{
   QStringList strlist_rv = qfd->qobj->history();
   QoreListNode *l = new QoreListNode();
   for (QStringList::iterator i = strlist_rv.begin(), e = strlist_rv.end(); i != e; ++i)
      l->push(new QoreStringNode((*i).toUtf8().data(), QCS_UTF8));
   return l;
}

////QFileIconProvider * iconProvider () const
//static AbstractQoreNode *QFILEDIALOG_iconProvider(QoreObject *self, QoreQFileDialog *qfd, const QoreListNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreBigIntNode(qfd->qobj->iconProvider());
//}

//bool isReadOnly () const
static AbstractQoreNode *QFILEDIALOG_isReadOnly(QoreObject *self, QoreQFileDialog *qfd, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qfd->qobj->isReadOnly());
}

//QAbstractItemDelegate * itemDelegate () const
static AbstractQoreNode *QFILEDIALOG_itemDelegate(QoreObject *self, QoreQFileDialog *qfd, const QoreListNode *params, ExceptionSink *xsink)
{
   QAbstractItemDelegate *qt_qobj = qfd->qobj->itemDelegate();
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   QoreObject *rv_obj = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   assert(rv_obj);
   rv_obj->ref();
   return rv_obj;
}

//QString labelText ( DialogLabel label ) const
static AbstractQoreNode *QFILEDIALOG_labelText(QoreObject *self, QoreQFileDialog *qfd, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QFileDialog::DialogLabel label = (QFileDialog::DialogLabel)(p ? p->getAsInt() : 0);
   return new QoreStringNode(qfd->qobj->labelText(label).toUtf8().data(), QCS_UTF8);
}

////QAbstractProxyModel * proxyModel () const
//static AbstractQoreNode *QFILEDIALOG_proxyModel(QoreObject *self, QoreQFileDialog *qfd, const QoreListNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreBigIntNode(qfd->qobj->proxyModel());
//}

//bool resolveSymlinks () const
static AbstractQoreNode *QFILEDIALOG_resolveSymlinks(QoreObject *self, QoreQFileDialog *qfd, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qfd->qobj->resolveSymlinks());
}

//bool restoreState ( const QByteArray & state )
static AbstractQoreNode *QFILEDIALOG_restoreState(QoreObject *self, QoreQFileDialog *qfd, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QByteArray state;
   if (get_qbytearray(p, state, xsink))
      return 0;
   return get_bool_node(qfd->qobj->restoreState(state));
}

//QByteArray saveState () const
static AbstractQoreNode *QFILEDIALOG_saveState(QoreObject *self, QoreQFileDialog *qfd, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qba = new QoreObject(QC_QByteArray, getProgram());
   QoreQByteArray *q_qba = new QoreQByteArray(qfd->qobj->saveState());
   o_qba->setPrivate(CID_QBYTEARRAY, q_qba);
   return o_qba;
}

//void selectFile ( const QString & filename )
static AbstractQoreNode *QFILEDIALOG_selectFile(QoreObject *self, QoreQFileDialog *qfd, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QString filename;
   if (get_qstring(p, filename, xsink))
      return 0;
   qfd->qobj->selectFile(filename);
   return 0;
}

//void selectFilter ( const QString & filter )
static AbstractQoreNode *QFILEDIALOG_selectFilter(QoreObject *self, QoreQFileDialog *qfd, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QString filter;
   if (get_qstring(p, filter, xsink))
      return 0;
   qfd->qobj->selectFilter(filter);
   return 0;
}

//QStringList selectedFiles () const
static AbstractQoreNode *QFILEDIALOG_selectedFiles(QoreObject *self, QoreQFileDialog *qfd, const QoreListNode *params, ExceptionSink *xsink)
{
   QStringList strlist_rv = qfd->qobj->selectedFiles();
   QoreListNode *l = new QoreListNode();
   for (QStringList::iterator i = strlist_rv.begin(), e = strlist_rv.end(); i != e; ++i)
      l->push(new QoreStringNode((*i).toUtf8().data(), QCS_UTF8));
   return l;
}

//QString selectedFilter () const
static AbstractQoreNode *QFILEDIALOG_selectedFilter(QoreObject *self, QoreQFileDialog *qfd, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qfd->qobj->selectedFilter().toUtf8().data(), QCS_UTF8);
}

//void setAcceptMode ( AcceptMode mode )
static AbstractQoreNode *QFILEDIALOG_setAcceptMode(QoreObject *self, QoreQFileDialog *qfd, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QFileDialog::AcceptMode mode = (QFileDialog::AcceptMode)(p ? p->getAsInt() : 0);
   qfd->qobj->setAcceptMode(mode);
   return 0;
}

//void setConfirmOverwrite ( bool enabled )
static AbstractQoreNode *QFILEDIALOG_setConfirmOverwrite(QoreObject *self, QoreQFileDialog *qfd, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   bool enabled = p ? p->getAsBool() : false;
   qfd->qobj->setConfirmOverwrite(enabled);
   return 0;
}

//void setDefaultSuffix ( const QString & suffix )
static AbstractQoreNode *QFILEDIALOG_setDefaultSuffix(QoreObject *self, QoreQFileDialog *qfd, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QString suffix;
   if (get_qstring(p, suffix, xsink))
      return 0;
   qfd->qobj->setDefaultSuffix(suffix);
   return 0;
}

//void setDirectory ( const QString & directory )
//void setDirectory ( const QDir & directory )
static AbstractQoreNode *QFILEDIALOG_setDirectory(QoreObject *self, QoreQFileDialog *qfd, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (p && p->getType() == NT_OBJECT) {
      QoreQDir *directory = (QoreQDir *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QDIR, xsink);
      if (!directory) {
         if (!xsink->isException())
            xsink->raiseException("QFILEDIALOG-SETDIRECTORY-PARAM-ERROR", "QFileDialog::setDirectory() does not know how to handle arguments of class '%s' as passed as the first argument", (reinterpret_cast<const QoreObject *>(p))->getClassName());
         return 0;
      }
      ReferenceHolder<AbstractPrivateData> directoryHolder(static_cast<AbstractPrivateData *>(directory), xsink);
      qfd->qobj->setDirectory(*(static_cast<QDir *>(directory)));
      return 0;
   }
   QString directory;
   if (get_qstring(p, directory, xsink))
      return 0;
   qfd->qobj->setDirectory(directory);
   return 0;
}

//void setFileMode ( FileMode mode )
static AbstractQoreNode *QFILEDIALOG_setFileMode(QoreObject *self, QoreQFileDialog *qfd, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QFileDialog::FileMode mode = (QFileDialog::FileMode)(p ? p->getAsInt() : 0);
   qfd->qobj->setFileMode(mode);
   return 0;
}

//void setFilter ( const QString & filter )
static AbstractQoreNode *QFILEDIALOG_setFilter(QoreObject *self, QoreQFileDialog *qfd, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QString filter;
   if (get_qstring(p, filter, xsink))
      return 0;
   qfd->qobj->setFilter(filter);
   return 0;
}

//void setFilters ( const QStringList & filters )
static AbstractQoreNode *QFILEDIALOG_setFilters(QoreObject *self, QoreQFileDialog *qfd, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreListNode *p = test_list_param(params, 0);
   if (!p) {
      xsink->raiseException("QFILEDIALOG-SETFILTERS-PARAM-ERROR", "expecting a list as first argument to QFileDialog::setFilters()");
      return 0;
   }
   QStringList filters;
   ConstListIterator li_filters(p);
   while (li_filters.next())
   {
      QoreStringNodeValueHelper str(li_filters.getValue());
      QString tmp;
      if (get_qstring(*str, tmp, xsink))
         return 0;
      filters.push_back(tmp);
   }
   qfd->qobj->setFilters(filters);
   return 0;
}

//void setHistory ( const QStringList & paths )
static AbstractQoreNode *QFILEDIALOG_setHistory(QoreObject *self, QoreQFileDialog *qfd, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreListNode *p = test_list_param(params, 0);
   if (!p) {
      xsink->raiseException("QFILEDIALOG-SETHISTORY-PARAM-ERROR", "expecting a list as first argument to QFileDialog::setHistory()");
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
   qfd->qobj->setHistory(paths);
   return 0;
}

////void setIconProvider ( QFileIconProvider * provider )
//static AbstractQoreNode *QFILEDIALOG_setIconProvider(QoreObject *self, QoreQFileDialog *qfd, const QoreListNode *params, ExceptionSink *xsink)
//{
//   const AbstractQoreNode *p = get_param(params, 0);
//   ??? QFileIconProvider* provider = p;
//   qfd->qobj->setIconProvider(provider);
//   return 0;
//}

//void setItemDelegate ( QAbstractItemDelegate * delegate )
static AbstractQoreNode *QFILEDIALOG_setItemDelegate(QoreObject *self, QoreQFileDialog *qfd, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreAbstractQAbstractItemDelegate *delegate = p ? (QoreAbstractQAbstractItemDelegate *)p->getReferencedPrivateData(CID_QABSTRACTITEMDELEGATE, xsink) : 0;
   if (!delegate) {
      if (!xsink->isException())
         xsink->raiseException("QFILEDIALOG-SETITEMDELEGATE-PARAM-ERROR", "expecting a QAbstractItemDelegate object as first argument to QFileDialog::setItemDelegate()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> delegateHolder(static_cast<AbstractPrivateData *>(delegate), xsink);
   qfd->qobj->setItemDelegate(static_cast<QAbstractItemDelegate *>(delegate->getQAbstractItemDelegate()));
   return 0;
}

//void setLabelText ( DialogLabel label, const QString & text )
static AbstractQoreNode *QFILEDIALOG_setLabelText(QoreObject *self, QoreQFileDialog *qfd, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QFileDialog::DialogLabel label = (QFileDialog::DialogLabel)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QString text;
   if (get_qstring(p, text, xsink))
      return 0;
   qfd->qobj->setLabelText(label, text);
   return 0;
}

////void setProxyModel ( QAbstractProxyModel * proxyModel )
//static AbstractQoreNode *QFILEDIALOG_setProxyModel(QoreObject *self, QoreQFileDialog *qfd, const QoreListNode *params, ExceptionSink *xsink)
//{
//   const AbstractQoreNode *p = get_param(params, 0);
//   ??? QAbstractProxyModel* proxyModel = p;
//   qfd->qobj->setProxyModel(proxyModel);
//   return 0;
//}

//void setReadOnly ( bool enabled )
static AbstractQoreNode *QFILEDIALOG_setReadOnly(QoreObject *self, QoreQFileDialog *qfd, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   bool enabled = p ? p->getAsBool() : false;
   qfd->qobj->setReadOnly(enabled);
   return 0;
}

//void setResolveSymlinks ( bool enabled )
static AbstractQoreNode *QFILEDIALOG_setResolveSymlinks(QoreObject *self, QoreQFileDialog *qfd, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   bool enabled = p ? p->getAsBool() : false;
   qfd->qobj->setResolveSymlinks(enabled);
   return 0;
}

////void setSidebarUrls ( const QList<QUrl> & urls )
//static AbstractQoreNode *QFILEDIALOG_setSidebarUrls(QoreObject *self, QoreQFileDialog *qfd, const QoreListNode *params, ExceptionSink *xsink)
//{
//   const AbstractQoreNode *p = get_param(params, 0);
//   ??? QList<QUrl> urls = p;
//   qfd->qobj->setSidebarUrls(urls);
//   return 0;
//}

//void setViewMode ( ViewMode mode )
static AbstractQoreNode *QFILEDIALOG_setViewMode(QoreObject *self, QoreQFileDialog *qfd, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QFileDialog::ViewMode mode = (QFileDialog::ViewMode)(p ? p->getAsInt() : 0);
   qfd->qobj->setViewMode(mode);
   return 0;
}

////QList<QUrl> sidebarUrls () const
//static AbstractQoreNode *QFILEDIALOG_sidebarUrls(QoreObject *self, QoreQFileDialog *qfd, const QoreListNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreBigIntNode(qfd->qobj->sidebarUrls());
//}

//ViewMode viewMode () const
static AbstractQoreNode *QFILEDIALOG_viewMode(QoreObject *self, QoreQFileDialog *qfd, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qfd->qobj->viewMode());
}

QoreClass *initQFileDialogClass(QoreClass *qdialog)
{
   QC_QFileDialog = new QoreClass("QFileDialog", QDOM_GUI);
   CID_QFILEDIALOG = QC_QFileDialog->getID();

   QC_QFileDialog->addBuiltinVirtualBaseClass(qdialog);

   QC_QFileDialog->setConstructor(QFILEDIALOG_constructor);
   QC_QFileDialog->setCopy((q_copy_t)QFILEDIALOG_copy);

   QC_QFileDialog->addMethod("acceptMode",                  (q_method_t)QFILEDIALOG_acceptMode);
   QC_QFileDialog->addMethod("confirmOverwrite",            (q_method_t)QFILEDIALOG_confirmOverwrite);
   QC_QFileDialog->addMethod("defaultSuffix",               (q_method_t)QFILEDIALOG_defaultSuffix);
   QC_QFileDialog->addMethod("directory",                   (q_method_t)QFILEDIALOG_directory);
   QC_QFileDialog->addMethod("fileMode",                    (q_method_t)QFILEDIALOG_fileMode);
   QC_QFileDialog->addMethod("filters",                     (q_method_t)QFILEDIALOG_filters);
   QC_QFileDialog->addMethod("history",                     (q_method_t)QFILEDIALOG_history);
   //QC_QFileDialog->addMethod("iconProvider",                (q_method_t)QFILEDIALOG_iconProvider);
   QC_QFileDialog->addMethod("isReadOnly",                  (q_method_t)QFILEDIALOG_isReadOnly);
   QC_QFileDialog->addMethod("itemDelegate",                (q_method_t)QFILEDIALOG_itemDelegate);
   QC_QFileDialog->addMethod("labelText",                   (q_method_t)QFILEDIALOG_labelText);
   //QC_QFileDialog->addMethod("proxyModel",                  (q_method_t)QFILEDIALOG_proxyModel);
   QC_QFileDialog->addMethod("resolveSymlinks",             (q_method_t)QFILEDIALOG_resolveSymlinks);
   QC_QFileDialog->addMethod("restoreState",                (q_method_t)QFILEDIALOG_restoreState);
   QC_QFileDialog->addMethod("saveState",                   (q_method_t)QFILEDIALOG_saveState);
   QC_QFileDialog->addMethod("selectFile",                  (q_method_t)QFILEDIALOG_selectFile);
   QC_QFileDialog->addMethod("selectFilter",                (q_method_t)QFILEDIALOG_selectFilter);
   QC_QFileDialog->addMethod("selectedFiles",               (q_method_t)QFILEDIALOG_selectedFiles);
   QC_QFileDialog->addMethod("selectedFilter",              (q_method_t)QFILEDIALOG_selectedFilter);
   QC_QFileDialog->addMethod("setAcceptMode",               (q_method_t)QFILEDIALOG_setAcceptMode);
   QC_QFileDialog->addMethod("setConfirmOverwrite",         (q_method_t)QFILEDIALOG_setConfirmOverwrite);
   QC_QFileDialog->addMethod("setDefaultSuffix",            (q_method_t)QFILEDIALOG_setDefaultSuffix);
   QC_QFileDialog->addMethod("setDirectory",                (q_method_t)QFILEDIALOG_setDirectory);
   QC_QFileDialog->addMethod("setFileMode",                 (q_method_t)QFILEDIALOG_setFileMode);
   QC_QFileDialog->addMethod("setFilter",                   (q_method_t)QFILEDIALOG_setFilter);
   QC_QFileDialog->addMethod("setFilters",                  (q_method_t)QFILEDIALOG_setFilters);
   QC_QFileDialog->addMethod("setHistory",                  (q_method_t)QFILEDIALOG_setHistory);
   //QC_QFileDialog->addMethod("setIconProvider",             (q_method_t)QFILEDIALOG_setIconProvider);
   QC_QFileDialog->addMethod("setItemDelegate",             (q_method_t)QFILEDIALOG_setItemDelegate);
   QC_QFileDialog->addMethod("setLabelText",                (q_method_t)QFILEDIALOG_setLabelText);
   //QC_QFileDialog->addMethod("setProxyModel",               (q_method_t)QFILEDIALOG_setProxyModel);
   QC_QFileDialog->addMethod("setReadOnly",                 (q_method_t)QFILEDIALOG_setReadOnly);
   QC_QFileDialog->addMethod("setResolveSymlinks",          (q_method_t)QFILEDIALOG_setResolveSymlinks);
   //QC_QFileDialog->addMethod("setSidebarUrls",              (q_method_t)QFILEDIALOG_setSidebarUrls);
   QC_QFileDialog->addMethod("setViewMode",                 (q_method_t)QFILEDIALOG_setViewMode);
   //QC_QFileDialog->addMethod("sidebarUrls",                 (q_method_t)QFILEDIALOG_sidebarUrls);
   QC_QFileDialog->addMethod("viewMode",                    (q_method_t)QFILEDIALOG_viewMode);

   return QC_QFileDialog;
}

//QString getExistingDirectory ( QWidget * parent = 0, const QString & caption = QString(), const QString & dir = QString(), Options options = ShowDirsOnly )
static AbstractQoreNode *f_QFileDialog_getExistingDirectory(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreObject *o = test_object_param(params, 0);
   QoreQWidget *parent = o ? (QoreQWidget *)o->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);

   const AbstractQoreNode *p = get_param(params, 1);
   QString caption;
   if (get_qstring(p, caption, xsink, true))
      caption = QString();

   p = get_param(params, 2);
   QString dir;
   if (get_qstring(p, dir, xsink, true))
      dir = QString();

   p = get_param(params, 3);
   QFileDialog::Options options = !is_nothing(p) ? (QFileDialog::Options)p->getAsInt() : QFileDialog::ShowDirsOnly;
   return new QoreStringNode(QFileDialog::getExistingDirectory(parent ? static_cast<QWidget *>(parent->getQWidget()) : 0, caption, dir, options).toUtf8().data(), QCS_UTF8);
}

//QString getOpenFileName ( QWidget * parent = 0, const QString & caption = QString(), const QString & dir = QString(), const QString & filter = QString(), QString * selectedFilter = 0, Options options = 0 )
static AbstractQoreNode *f_QFileDialog_getOpenFileName(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreObject *o = test_object_param(params, 0);
   QoreQWidget *parent = o ? (QoreQWidget *)o->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);

   const AbstractQoreNode *p = get_param(params, 1);
   QString caption;
   if (get_qstring(p, caption, xsink, true))
      caption = QString();

   p = get_param(params, 2);
   QString dir;
   if (get_qstring(p, dir, xsink, true))
      dir = QString();

   p = get_param(params, 3);
   QString filter;
   if (get_qstring(p, filter, xsink, true))
      filter = QString();

   p = get_param(params, 4);
   QString selectedFilter;
   if (get_qstring(p, selectedFilter, xsink, true))
      selectedFilter = QString();

   p = get_param(params, 5);
   QFileDialog::Options options = (QFileDialog::Options)(!is_nothing(p) ? p->getAsInt() : 0);

   return new QoreStringNode(QFileDialog::getOpenFileName(parent ? static_cast<QWidget *>(parent->getQWidget()) : 0, caption, dir, filter, &selectedFilter, options).toUtf8().data(), QCS_UTF8);
}

//QStringList getOpenFileNames ( QWidget * parent = 0, const QString & caption = QString(), const QString & dir = QString(), const QString & filter = QString(), QString * selectedFilter = 0, Options options = 0 )
static AbstractQoreNode *f_QFileDialog_getOpenFileNames(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreObject *o = test_object_param(params, 0);
   QoreQWidget *parent = o ? (QoreQWidget *)o->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);

   const AbstractQoreNode *p = get_param(params, 1);
   QString caption;
   if (get_qstring(p, caption, xsink, true))
      caption = QString();
   p = get_param(params, 2);
   QString dir;
   if (get_qstring(p, dir, xsink, true))
      dir = QString();
   p = get_param(params, 3);
   QString filter;
   if (get_qstring(p, filter, xsink, true))
      filter = QString();
   p = get_param(params, 4);
   QString selectedFilter;
   if (get_qstring(p, selectedFilter, xsink, true))
      selectedFilter = QString();

   p = get_param(params, 5);
   QFileDialog::Options options = (QFileDialog::Options)(!is_nothing(p) ? p->getAsInt() : 0);
   QStringList strlist_rv = QFileDialog::getOpenFileNames(parent ? static_cast<QWidget *>(parent->getQWidget()) : 0, caption, dir, filter, &selectedFilter, options);
   QoreListNode *l = new QoreListNode();
   for (QStringList::iterator i = strlist_rv.begin(), e = strlist_rv.end(); i != e; ++i)
      l->push(new QoreStringNode((*i).toUtf8().data(), QCS_UTF8));
   return l;
}

//QString getSaveFileName ( QWidget * parent = 0, const QString & caption = QString(), const QString & dir = QString(), const QString & filter = QString(), QString * selectedFilter = 0, Options options = 0 )
static AbstractQoreNode *f_QFileDialog_getSaveFileName(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreObject *o = test_object_param(params, 0);
   QoreQWidget *parent = o ? (QoreQWidget *)o->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);

   const AbstractQoreNode *p = get_param(params, 1);
   QString caption;
   if (get_qstring(p, caption, xsink, true))
      caption = QString();

   p = get_param(params, 2);
   QString dir;
   if (get_qstring(p, dir, xsink, true))
      dir = QString();

   p = get_param(params, 3);
   QString filter;
   if (get_qstring(p, filter, xsink, true))
      filter = QString();

   p = get_param(params, 4);
   QString selectedFilter;
   if (get_qstring(p, selectedFilter, xsink, true))
      selectedFilter = QString();

   p = get_param(params, 5);
   QFileDialog::Options options = (QFileDialog::Options)(!is_nothing(p) ? p->getAsInt() : 0);
   return new QoreStringNode(QFileDialog::getSaveFileName(parent ? static_cast<QWidget *>(parent->getQWidget()) : 0, caption, dir, filter, &selectedFilter, options).toUtf8().data(), QCS_UTF8);
}

void initQFileDialogStaticFunctions()
{
   builtinFunctions.add("QFileDialog_getExistingDirectory",         f_QFileDialog_getExistingDirectory);
   builtinFunctions.add("QFileDialog_getOpenFileName",              f_QFileDialog_getOpenFileName);
   builtinFunctions.add("QFileDialog_getOpenFileNames",             f_QFileDialog_getOpenFileNames);
   builtinFunctions.add("QFileDialog_getSaveFileName",              f_QFileDialog_getSaveFileName);
}

