/*
 QC_QDir.cc
 
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

#include "QC_QDir.h"

#include "qore-qt.h"

int CID_QDIR;
class QoreClass *QC_QDir = 0;

//QDir ( const QDir & dir )
//QDir ( const QString & path = QString() )
//QDir ( const QString & path, const QString & nameFilter, SortFlags sort = SortFlags( Name | IgnoreCase ), Filters filters = AllEntries )
static void QDIR_constructor(QoreObject *self, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QDIR, new QoreQDir());
      return;
   }
   QString path;
   if (get_qstring(p, path, xsink))
      return;
   p = get_param(params, 1);
   QString nameFilter;
   if (get_qstring(p, nameFilter, xsink))
      return;
   p = get_param(params, 2);
   QDir::SortFlags sort = (QDir::SortFlags)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   QDir::Filters filters = !is_nothing(p) ? (QDir::Filters)p->getAsInt() : QDir::AllEntries;
   self->setPrivate(CID_QDIR, new QoreQDir(path, nameFilter, sort, filters));
   return;
}

static void QDIR_copy(class QoreObject *self, class QoreObject *old, class QoreQDir *qd, ExceptionSink *xsink)
{
   self->setPrivate(CID_QDIR, new QoreQDir(*qd));
}

//QString absoluteFilePath ( const QString & fileName ) const
static QoreNode *QDIR_absoluteFilePath(QoreObject *self, QoreQDir *qd, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString fileName;
   if (get_qstring(p, fileName, xsink))
      return 0;
   return new QoreStringNode(qd->absoluteFilePath(fileName).toUtf8().data(), QCS_UTF8);
}

//QString absolutePath () const
static QoreNode *QDIR_absolutePath(QoreObject *self, QoreQDir *qd, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qd->absolutePath().toUtf8().data(), QCS_UTF8);
}

//QString canonicalPath () const
static QoreNode *QDIR_canonicalPath(QoreObject *self, QoreQDir *qd, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qd->canonicalPath().toUtf8().data(), QCS_UTF8);
}

//bool cd ( const QString & dirName )
static QoreNode *QDIR_cd(QoreObject *self, QoreQDir *qd, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString dirName;
   if (get_qstring(p, dirName, xsink))
      return 0;
   return new QoreNode(qd->cd(dirName));
}

//bool cdUp ()
static QoreNode *QDIR_cdUp(QoreObject *self, QoreQDir *qd, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qd->cdUp());
}

//uint count () const
static QoreNode *QDIR_count(QoreObject *self, QoreQDir *qd, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qd->count());
}

//QString dirName () const
static QoreNode *QDIR_dirName(QoreObject *self, QoreQDir *qd, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qd->dirName().toUtf8().data(), QCS_UTF8);
}

////QFileInfoList entryInfoList ( const QStringList & nameFilters, Filters filters = NoFilter, SortFlags sort = NoSort ) const
////QFileInfoList entryInfoList ( Filters filters = NoFilter, SortFlags sort = NoSort ) const
//static QoreNode *QDIR_entryInfoList(QoreObject *self, QoreQDir *qd, const QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   if (is_nothing(p)) {
//      ??? return new QoreNode((int64)qd->entryInfoList());
//   }
//   if (p && p->type == NT_???) {
//      if (!p || p->type != NT_LIST) {
//         xsink->raiseException("QDIR-ENTRYINFOLIST-PARAM-ERROR", "expecting a list as first argument to QDir::entryInfoList()");
//         return 0;
//      }
//      QStringList nameFilters;
//      ListIterator li_nameFilters(p->val.list);
//      while (li_nameFilters.next())
//      {
//         QoreStringNodeValueHelper str(li_nameFileters.getValue());
//         QString tmp;
//         if (get_qstring(*str, tmp, xsink))
//            return 0;
//         nameFilters.push_back(tmp);
//      }
//   p = get_param(params, 1);
//   SortFlags sort = !is_nothing(p) ? (SortFlags)p->getAsInt() : NoSort;
//   ??? return new QoreNode((int64)qd->entryInfoList(nameFilters, sort));
//   }
//   Filters filters = !is_nothing(p) ? (Filters)p->getAsInt() : NoFilter;
//   p = get_param(params, 1);
//   SortFlags sort = !is_nothing(p) ? (SortFlags)p->getAsInt() : NoSort;
//   ??? return new QoreNode((int64)qd->entryInfoList(filters, sort));
//}

////QStringList entryList ( const QStringList & nameFilters, Filters filters = NoFilter, SortFlags sort = NoSort ) const
////QStringList entryList ( Filters filters = NoFilter, SortFlags sort = NoSort ) const
//static QoreNode *QDIR_entryList(QoreObject *self, QoreQDir *qd, const QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   if (is_nothing(p)) {
//      QStringList strlist_rv = qd->entryList();
//      QoreList *l = new QoreList();
//      for (QStringList::iterator i = strlist_rv.begin(), e = strlist_rv.end(); i != e; ++i)
//         l->push(new QoreStringNode((*i).toUtf8().data(), QCS_UTF8));
//      return new QoreNode(l);
//   }
//   if (p && p->type == NT_???) {
//      if (!p || p->type != NT_LIST) {
//         xsink->raiseException("QDIR-ENTRYLIST-PARAM-ERROR", "expecting a list as first argument to QDir::entryList()");
//         return 0;
//      }
//      QStringList nameFilters;
//      ListIterator li_nameFilters(p->val.list);
//      while (li_nameFilters.next())
//      {
//         QoreStringNodeValueHelper str(li_nameFileters.getValue());
//         if (*xsink)
//            return 0;
//         QString tmp;
//         if (get_qstring(*str, tmp, xsink))
//            return 0;
//         nameFilters.push_back(tmp);
//      }
//   p = get_param(params, 1);
//   SortFlags sort = !is_nothing(p) ? (SortFlags)p->getAsInt() : NoSort;
//   QStringList strlist_rv = qd->entryList(nameFilters, sort);
//   QoreList *l = new QoreList();
//   for (QStringList::iterator i = strlist_rv.begin(), e = strlist_rv.end(); i != e; ++i)
//      l->push(new QoreStringNode((*i).toUtf8().data(), QCS_UTF8));
//   return new QoreNode(l);
//   }
//   Filters filters = !is_nothing(p) ? (Filters)p->getAsInt() : NoFilter;
//   p = get_param(params, 1);
//   SortFlags sort = !is_nothing(p) ? (SortFlags)p->getAsInt() : NoSort;
//   QStringList strlist_rv = qd->entryList(filters, sort);
//   QoreList *l = new QoreList();
//   for (QStringList::iterator i = strlist_rv.begin(), e = strlist_rv.end(); i != e; ++i)
//      l->push(new QoreStringNode((*i).toUtf8().data(), QCS_UTF8));
//   return new QoreNode(l);
//}

//bool exists ( const QString & name ) const
//bool exists () const
static QoreNode *QDIR_exists(QoreObject *self, QoreQDir *qd, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      return new QoreNode(qd->exists());
   }
   QString name;
   if (get_qstring(p, name, xsink))
      return 0;
   return new QoreNode(qd->exists(name));
}

//QString filePath ( const QString & fileName ) const
static QoreNode *QDIR_filePath(QoreObject *self, QoreQDir *qd, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString fileName;
   if (get_qstring(p, fileName, xsink))
      return 0;
   return new QoreStringNode(qd->filePath(fileName).toUtf8().data(), QCS_UTF8);
}

////Filters filter () const
//static QoreNode *QDIR_filter(QoreObject *self, QoreQDir *qd, const QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreNode((int64)qd->filter());
//}

//bool isAbsolute () const
static QoreNode *QDIR_isAbsolute(QoreObject *self, QoreQDir *qd, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qd->isAbsolute());
}

//bool isReadable () const
static QoreNode *QDIR_isReadable(QoreObject *self, QoreQDir *qd, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qd->isReadable());
}

//bool isRelative () const
static QoreNode *QDIR_isRelative(QoreObject *self, QoreQDir *qd, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qd->isRelative());
}

//bool isRoot () const
static QoreNode *QDIR_isRoot(QoreObject *self, QoreQDir *qd, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qd->isRoot());
}

//bool makeAbsolute ()
static QoreNode *QDIR_makeAbsolute(QoreObject *self, QoreQDir *qd, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qd->makeAbsolute());
}

//bool mkdir ( const QString & dirName ) const
static QoreNode *QDIR_mkdir(QoreObject *self, QoreQDir *qd, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString dirName;
   if (get_qstring(p, dirName, xsink))
      return 0;
   return new QoreNode(qd->mkdir(dirName));
}

//bool mkpath ( const QString & dirPath ) const
static QoreNode *QDIR_mkpath(QoreObject *self, QoreQDir *qd, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString dirPath;
   if (get_qstring(p, dirPath, xsink))
      return 0;
   return new QoreNode(qd->mkpath(dirPath));
}

//QStringList nameFilters () const
static QoreNode *QDIR_nameFilters(QoreObject *self, QoreQDir *qd, const QoreNode *params, ExceptionSink *xsink)
{
   QStringList strlist_rv = qd->nameFilters();
   QoreList *l = new QoreList();
   for (QStringList::iterator i = strlist_rv.begin(), e = strlist_rv.end(); i != e; ++i)
      l->push(new QoreStringNode((*i).toUtf8().data(), QCS_UTF8));
   return new QoreNode(l);
}

//QString path () const
static QoreNode *QDIR_path(QoreObject *self, QoreQDir *qd, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qd->path().toUtf8().data(), QCS_UTF8);
}

//void refresh () const
static QoreNode *QDIR_refresh(QoreObject *self, QoreQDir *qd, const QoreNode *params, ExceptionSink *xsink)
{
   qd->refresh();
   return 0;
}

//QString relativeFilePath ( const QString & fileName ) const
static QoreNode *QDIR_relativeFilePath(QoreObject *self, QoreQDir *qd, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString fileName;
   if (get_qstring(p, fileName, xsink))
      return 0;
   return new QoreStringNode(qd->relativeFilePath(fileName).toUtf8().data(), QCS_UTF8);
}

//bool remove ( const QString & fileName )
static QoreNode *QDIR_remove(QoreObject *self, QoreQDir *qd, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString fileName;
   if (get_qstring(p, fileName, xsink))
      return 0;
   return new QoreNode(qd->remove(fileName));
}

//bool rename ( const QString & oldName, const QString & newName )
static QoreNode *QDIR_rename(QoreObject *self, QoreQDir *qd, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString oldName;
   if (get_qstring(p, oldName, xsink))
      return 0;
   p = get_param(params, 1);
   QString newName;
   if (get_qstring(p, newName, xsink))
      return 0;
   return new QoreNode(qd->rename(oldName, newName));
}

//bool rmdir ( const QString & dirName ) const
static QoreNode *QDIR_rmdir(QoreObject *self, QoreQDir *qd, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString dirName;
   if (get_qstring(p, dirName, xsink))
      return 0;
   return new QoreNode(qd->rmdir(dirName));
}

//bool rmpath ( const QString & dirPath ) const
static QoreNode *QDIR_rmpath(QoreObject *self, QoreQDir *qd, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString dirPath;
   if (get_qstring(p, dirPath, xsink))
      return 0;
   return new QoreNode(qd->rmpath(dirPath));
}

//void setFilter ( Filters filters )
static QoreNode *QDIR_setFilter(QoreObject *self, QoreQDir *qd, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QDir::Filters filters = (QDir::Filters)(p ? p->getAsInt() : 0);
   qd->setFilter(filters);
   return 0;
}

//void setNameFilters ( const QStringList & nameFilters )
static QoreNode *QDIR_setNameFilters(QoreObject *self, QoreQDir *qd, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (!p || p->type != NT_LIST) {
      xsink->raiseException("QDIR-SETNAMEFILTERS-PARAM-ERROR", "expecting a list as first argument to QDir::setNameFilters()");
      return 0;
   }
   QStringList nameFilters;
   ListIterator li_nameFilters(p->val.list);
   while (li_nameFilters.next())
   {
      QoreStringNodeValueHelper str(li_nameFilters.getValue());
      QString tmp;
      if (get_qstring(*str, tmp, xsink))
         return 0;
      nameFilters.push_back(tmp);
   }
   qd->setNameFilters(nameFilters);
   return 0;
}

//void setPath ( const QString & path )
static QoreNode *QDIR_setPath(QoreObject *self, QoreQDir *qd, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString path;
   if (get_qstring(p, path, xsink))
      return 0;
   qd->setPath(path);
   return 0;
}

//void setSorting ( SortFlags sort )
static QoreNode *QDIR_setSorting(QoreObject *self, QoreQDir *qd, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QDir::SortFlags sort = (QDir::SortFlags)(p ? p->getAsInt() : 0);
   qd->setSorting(sort);
   return 0;
}

////SortFlags sorting () const
//static QoreNode *QDIR_sorting(QoreObject *self, QoreQDir *qd, const QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreNode((int64)qd->sorting());
//}

QoreClass *initQDirClass()
{
   QC_QDir = new QoreClass("QDir", QDOM_GUI);
   CID_QDIR = QC_QDir->getID();

   QC_QDir->setConstructor(QDIR_constructor);
   QC_QDir->setCopy((q_copy_t)QDIR_copy);

   QC_QDir->addMethod("absoluteFilePath",            (q_method_t)QDIR_absoluteFilePath);
   QC_QDir->addMethod("absolutePath",                (q_method_t)QDIR_absolutePath);
   QC_QDir->addMethod("canonicalPath",               (q_method_t)QDIR_canonicalPath);
   QC_QDir->addMethod("cd",                          (q_method_t)QDIR_cd);
   QC_QDir->addMethod("cdUp",                        (q_method_t)QDIR_cdUp);
   QC_QDir->addMethod("count",                       (q_method_t)QDIR_count);
   QC_QDir->addMethod("dirName",                     (q_method_t)QDIR_dirName);
   //QC_QDir->addMethod("entryInfoList",               (q_method_t)QDIR_entryInfoList);
   //QC_QDir->addMethod("entryList",                   (q_method_t)QDIR_entryList);
   QC_QDir->addMethod("exists",                      (q_method_t)QDIR_exists);
   QC_QDir->addMethod("filePath",                    (q_method_t)QDIR_filePath);
   //QC_QDir->addMethod("filter",                      (q_method_t)QDIR_filter);
   QC_QDir->addMethod("isAbsolute",                  (q_method_t)QDIR_isAbsolute);
   QC_QDir->addMethod("isReadable",                  (q_method_t)QDIR_isReadable);
   QC_QDir->addMethod("isRelative",                  (q_method_t)QDIR_isRelative);
   QC_QDir->addMethod("isRoot",                      (q_method_t)QDIR_isRoot);
   QC_QDir->addMethod("makeAbsolute",                (q_method_t)QDIR_makeAbsolute);
   QC_QDir->addMethod("mkdir",                       (q_method_t)QDIR_mkdir);
   QC_QDir->addMethod("mkpath",                      (q_method_t)QDIR_mkpath);
   QC_QDir->addMethod("nameFilters",                 (q_method_t)QDIR_nameFilters);
   QC_QDir->addMethod("path",                        (q_method_t)QDIR_path);
   QC_QDir->addMethod("refresh",                     (q_method_t)QDIR_refresh);
   QC_QDir->addMethod("relativeFilePath",            (q_method_t)QDIR_relativeFilePath);
   QC_QDir->addMethod("remove",                      (q_method_t)QDIR_remove);
   QC_QDir->addMethod("rename",                      (q_method_t)QDIR_rename);
   QC_QDir->addMethod("rmdir",                       (q_method_t)QDIR_rmdir);
   QC_QDir->addMethod("rmpath",                      (q_method_t)QDIR_rmpath);
   QC_QDir->addMethod("setFilter",                   (q_method_t)QDIR_setFilter);
   QC_QDir->addMethod("setNameFilters",              (q_method_t)QDIR_setNameFilters);
   QC_QDir->addMethod("setPath",                     (q_method_t)QDIR_setPath);
   QC_QDir->addMethod("setSorting",                  (q_method_t)QDIR_setSorting);
   //QC_QDir->addMethod("sorting",                     (q_method_t)QDIR_sorting);

   return QC_QDir;
}

//void addSearchPath ( const QString & prefix, const QString & path )
static QoreNode *f_QDir_addSearchPath(const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString prefix;
   if (get_qstring(p, prefix, xsink))
      return 0;
   p = get_param(params, 1);
   QString path;
   if (get_qstring(p, path, xsink))
      return 0;
   QDir::addSearchPath(prefix, path);
   return 0;
}

//QString cleanPath ( const QString & path )
static QoreNode *f_QDir_cleanPath(const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString path;
   if (get_qstring(p, path, xsink))
      return 0;
   return new QoreStringNode(QDir::cleanPath(path).toUtf8().data(), QCS_UTF8);
}

//QDir current ()
static QoreNode *f_QDir_current(const QoreNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qd = new QoreObject(QC_QDir, getProgram());
   QoreQDir *q_qd = new QoreQDir(QDir::current());
   o_qd->setPrivate(CID_QDIR, q_qd);
   return new QoreNode(o_qd);
}

//QString currentPath ()
static QoreNode *f_QDir_currentPath(const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(QDir::currentPath().toUtf8().data(), QCS_UTF8);
}

////QFileInfoList drives ()
//static QoreNode *f_QDir_drives(const QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreNode((int64)QDir::drives());
//}

//QString fromNativeSeparators ( const QString & pathName )
static QoreNode *f_QDir_fromNativeSeparators(const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString pathName;
   if (get_qstring(p, pathName, xsink))
      return 0;
   return new QoreStringNode(QDir::fromNativeSeparators(pathName).toUtf8().data(), QCS_UTF8);
}

//QDir home ()
static QoreNode *f_QDir_home(const QoreNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qd = new QoreObject(QC_QDir, getProgram());
   QoreQDir *q_qd = new QoreQDir(QDir::home());
   o_qd->setPrivate(CID_QDIR, q_qd);
   return new QoreNode(o_qd);
}

//QString homePath ()
static QoreNode *f_QDir_homePath(const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(QDir::homePath().toUtf8().data(), QCS_UTF8);
}

//bool isAbsolutePath ( const QString & path )
static QoreNode *f_QDir_isAbsolutePath(const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString path;
   if (get_qstring(p, path, xsink))
      return 0;
   return new QoreNode(QDir::isAbsolutePath(path));
}

//bool isRelativePath ( const QString & path )
static QoreNode *f_QDir_isRelativePath(const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString path;
   if (get_qstring(p, path, xsink))
      return 0;
   return new QoreNode(QDir::isRelativePath(path));
}

//bool match ( const QString & filter, const QString & fileName )
//bool match ( const QStringList & filters, const QString & fileName )
static QoreNode *f_QDir_match(const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString filter;
   if (get_qstring(p, filter, xsink))
      return 0;
   p = get_param(params, 1);
   QString fileName;
   if (get_qstring(p, fileName, xsink))
      return 0;
   return new QoreNode(QDir::match(filter, fileName));
}

//QDir root ()
static QoreNode *f_QDir_root(const QoreNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qd = new QoreObject(QC_QDir, getProgram());
   QoreQDir *q_qd = new QoreQDir(QDir::root());
   o_qd->setPrivate(CID_QDIR, q_qd);
   return new QoreNode(o_qd);
}

//QString rootPath ()
static QoreNode *f_QDir_rootPath(const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(QDir::rootPath().toUtf8().data(), QCS_UTF8);
}

//QStringList searchPaths ( const QString & prefix )
static QoreNode *f_QDir_searchPaths(const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString prefix;
   if (get_qstring(p, prefix, xsink))
      return 0;
   QStringList strlist_rv = QDir::searchPaths(prefix);
   QoreList *l = new QoreList();
   for (QStringList::iterator i = strlist_rv.begin(), e = strlist_rv.end(); i != e; ++i)
      l->push(new QoreStringNode((*i).toUtf8().data(), QCS_UTF8));
   return new QoreNode(l);
}

//QChar separator ()
static QoreNode *f_QDir_separator(const QoreNode *params, ExceptionSink *xsink)
{
   QoreStringNode *rv_str = new QoreStringNode(QCS_UTF8);
   QChar rv_qc = QDir::separator();
   rv_str->concatUTF8FromUnicode(rv_qc.unicode());
   return rv_str;
}

//bool setCurrent ( const QString & path )
static QoreNode *f_QDir_setCurrent(const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString path;
   if (get_qstring(p, path, xsink))
      return 0;
   return new QoreNode(QDir::setCurrent(path));
}

//void setSearchPaths ( const QString & prefix, const QStringList & searchPaths )
static QoreNode *f_QDir_setSearchPaths(const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString prefix;
   if (get_qstring(p, prefix, xsink))
      return 0;
   p = get_param(params, 1);
   if (!p || p->type != NT_LIST) {
      xsink->raiseException("QDIR-SETSEARCHPATHS-PARAM-ERROR", "expecting a list as second argument to QDir::setSearchPaths()");
      return 0;
   }
   QStringList searchPaths;
   ListIterator li_searchPaths(p->val.list);
   while (li_searchPaths.next())
   {
      QoreStringNodeValueHelper str(li_searchPaths.getValue());
      QString tmp;
      if (get_qstring(*str, tmp, xsink))
         return 0;
      searchPaths.push_back(tmp);
   }
   QDir::setSearchPaths(prefix, searchPaths);
   return 0;
}
//QDir temp ()
static QoreNode *f_QDir_temp(const QoreNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qd = new QoreObject(QC_QDir, getProgram());
   QoreQDir *q_qd = new QoreQDir(QDir::temp());
   o_qd->setPrivate(CID_QDIR, q_qd);
   return new QoreNode(o_qd);
}

//QString tempPath ()
static QoreNode *f_QDir_tempPath(const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(QDir::tempPath().toUtf8().data(), QCS_UTF8);
}

//QString toNativeSeparators ( const QString & pathName )
static QoreNode *f_QDir_toNativeSeparators(const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString pathName;
   if (get_qstring(p, pathName, xsink))
      return 0;
   return new QoreStringNode(QDir::toNativeSeparators(pathName).toUtf8().data(), QCS_UTF8);
}

void initQDirStaticFunctions()
{
   builtinFunctions.add("QDir_addSearchPath",                f_QDir_addSearchPath);
   builtinFunctions.add("QDir_cleanPath",                    f_QDir_cleanPath);
   builtinFunctions.add("QDir_current",                      f_QDir_current);
   builtinFunctions.add("QDir_currentPath",                  f_QDir_currentPath);
   //builtinFunctions.add("QDir_drives",                       f_QDir_drives);
   builtinFunctions.add("QDir_fromNativeSeparators",         f_QDir_fromNativeSeparators);
   builtinFunctions.add("QDir_home",                         f_QDir_home);
   builtinFunctions.add("QDir_homePath",                     f_QDir_homePath);
   builtinFunctions.add("QDir_isAbsolutePath",               f_QDir_isAbsolutePath);
   builtinFunctions.add("QDir_isRelativePath",               f_QDir_isRelativePath);
   builtinFunctions.add("QDir_match",                        f_QDir_match);
   builtinFunctions.add("QDir_root",                         f_QDir_root);
   builtinFunctions.add("QDir_rootPath",                     f_QDir_rootPath);
   builtinFunctions.add("QDir_searchPaths",                  f_QDir_searchPaths);
   builtinFunctions.add("QDir_separator",                    f_QDir_separator);
   builtinFunctions.add("QDir_setCurrent",                   f_QDir_setCurrent);
   builtinFunctions.add("QDir_setSearchPaths",               f_QDir_setSearchPaths);
   builtinFunctions.add("QDir_temp",                         f_QDir_temp);
   builtinFunctions.add("QDir_tempPath",                     f_QDir_tempPath);
   builtinFunctions.add("QDir_toNativeSeparators",           f_QDir_toNativeSeparators);
}
