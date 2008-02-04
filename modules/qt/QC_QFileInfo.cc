/*
 QC_QFileInfo.cc
 
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

#include "QC_QFileInfo.h"
#include "QC_QDir.h"
#include "QC_QDate.h"
#include "QC_QTime.h"

#include "qore-qt.h"

int CID_QFILEINFO;
class QoreClass *QC_QFileInfo = 0;

//QFileInfo ()
//QFileInfo ( const QString & file )
//QFileInfo ( const QFile & file )
//QFileInfo ( const QDir & dir, const QString & file )
//QFileInfo ( const QFileInfo & fileinfo )
static void QFILEINFO_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QFILEINFO, new QoreQFileInfo());
      return;
   }
   if (p && p->type == NT_OBJECT) {
      QoreQDir *dir = (QoreQDir *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QDIR, xsink);
      if (!dir) {
         if (!xsink->isException())
            xsink->raiseException("QFILEINFO-CONSTRUCTOR-PARAM-ERROR", "QFileInfo::constructor() does not know how to handle arguments of class '%s' as passed as the first argument", (reinterpret_cast<const QoreObject *>(p))->getClassName());
         return;
      }
      ReferenceHolder<AbstractPrivateData> dirHolder(static_cast<AbstractPrivateData *>(dir), xsink);
      p = get_param(params, 1);
      QString file;
      if (get_qstring(p, file, xsink))
         return;
      self->setPrivate(CID_QFILEINFO, new QoreQFileInfo(*(static_cast<QDir *>(dir)), file));
      return;
   }
   QString file;
   if (get_qstring(p, file, xsink))
      return;
   self->setPrivate(CID_QFILEINFO, new QoreQFileInfo(file));
   return;
}

static void QFILEINFO_copy(class QoreObject *self, class QoreObject *old, class QoreQFileInfo *qfi, ExceptionSink *xsink)
{
   self->setPrivate(CID_QFILEINFO, new QoreQFileInfo(*qfi));
}

//QDir absoluteDir () const
static AbstractQoreNode *QFILEINFO_absoluteDir(QoreObject *self, QoreQFileInfo *qfi, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qd = new QoreObject(QC_QDir, getProgram());
   QoreQDir *q_qd = new QoreQDir(qfi->absoluteDir());
   o_qd->setPrivate(CID_QDIR, q_qd);
   return o_qd;
}

//QString absoluteFilePath () const
static AbstractQoreNode *QFILEINFO_absoluteFilePath(QoreObject *self, QoreQFileInfo *qfi, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qfi->absoluteFilePath().toUtf8().data(), QCS_UTF8);
}

//QString absolutePath () const
static AbstractQoreNode *QFILEINFO_absolutePath(QoreObject *self, QoreQFileInfo *qfi, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qfi->absolutePath().toUtf8().data(), QCS_UTF8);
}

//QString baseName () const
static AbstractQoreNode *QFILEINFO_baseName(QoreObject *self, QoreQFileInfo *qfi, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qfi->baseName().toUtf8().data(), QCS_UTF8);
}

//QString bundleName () const
static AbstractQoreNode *QFILEINFO_bundleName(QoreObject *self, QoreQFileInfo *qfi, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qfi->bundleName().toUtf8().data(), QCS_UTF8);
}

//bool caching () const
static AbstractQoreNode *QFILEINFO_caching(QoreObject *self, QoreQFileInfo *qfi, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qfi->caching());
}

//QString canonicalFilePath () const
static AbstractQoreNode *QFILEINFO_canonicalFilePath(QoreObject *self, QoreQFileInfo *qfi, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qfi->canonicalFilePath().toUtf8().data(), QCS_UTF8);
}

//QString canonicalPath () const
static AbstractQoreNode *QFILEINFO_canonicalPath(QoreObject *self, QoreQFileInfo *qfi, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qfi->canonicalPath().toUtf8().data(), QCS_UTF8);
}

//QString completeBaseName () const
static AbstractQoreNode *QFILEINFO_completeBaseName(QoreObject *self, QoreQFileInfo *qfi, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qfi->completeBaseName().toUtf8().data(), QCS_UTF8);
}

//QString completeSuffix () const
static AbstractQoreNode *QFILEINFO_completeSuffix(QoreObject *self, QoreQFileInfo *qfi, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qfi->completeSuffix().toUtf8().data(), QCS_UTF8);
}

//QDateTime created () const
static AbstractQoreNode *QFILEINFO_created(QoreObject *self, QoreQFileInfo *qfi, const QoreListNode *params, ExceptionSink *xsink)
{
   QDateTime rv_dt = qfi->created();
   QDate rv_d = rv_dt.date();
   QTime rv_t = rv_dt.time();
   return new DateTimeNode(rv_d.year(), rv_d.month(), rv_d.day(), rv_t.hour(), rv_t.minute(), rv_t.second(), rv_t.msec());
}

//QDir dir () const
static AbstractQoreNode *QFILEINFO_dir(QoreObject *self, QoreQFileInfo *qfi, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qd = new QoreObject(QC_QDir, getProgram());
   QoreQDir *q_qd = new QoreQDir(qfi->dir());
   o_qd->setPrivate(CID_QDIR, q_qd);
   return o_qd;
}

//bool exists () const
static AbstractQoreNode *QFILEINFO_exists(QoreObject *self, QoreQFileInfo *qfi, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qfi->exists());
}

//QString fileName () const
static AbstractQoreNode *QFILEINFO_fileName(QoreObject *self, QoreQFileInfo *qfi, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qfi->fileName().toUtf8().data(), QCS_UTF8);
}

//QString filePath () const
static AbstractQoreNode *QFILEINFO_filePath(QoreObject *self, QoreQFileInfo *qfi, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qfi->filePath().toUtf8().data(), QCS_UTF8);
}

//QString group () const
static AbstractQoreNode *QFILEINFO_group(QoreObject *self, QoreQFileInfo *qfi, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qfi->group().toUtf8().data(), QCS_UTF8);
}

//uint groupId () const
static AbstractQoreNode *QFILEINFO_groupId(QoreObject *self, QoreQFileInfo *qfi, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qfi->groupId());
}

//bool isAbsolute () const
static AbstractQoreNode *QFILEINFO_isAbsolute(QoreObject *self, QoreQFileInfo *qfi, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qfi->isAbsolute());
}

//bool isBundle () const
static AbstractQoreNode *QFILEINFO_isBundle(QoreObject *self, QoreQFileInfo *qfi, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qfi->isBundle());
}

//bool isDir () const
static AbstractQoreNode *QFILEINFO_isDir(QoreObject *self, QoreQFileInfo *qfi, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qfi->isDir());
}

//bool isExecutable () const
static AbstractQoreNode *QFILEINFO_isExecutable(QoreObject *self, QoreQFileInfo *qfi, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qfi->isExecutable());
}

//bool isFile () const
static AbstractQoreNode *QFILEINFO_isFile(QoreObject *self, QoreQFileInfo *qfi, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qfi->isFile());
}

//bool isHidden () const
static AbstractQoreNode *QFILEINFO_isHidden(QoreObject *self, QoreQFileInfo *qfi, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qfi->isHidden());
}

//bool isReadable () const
static AbstractQoreNode *QFILEINFO_isReadable(QoreObject *self, QoreQFileInfo *qfi, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qfi->isReadable());
}

//bool isRelative () const
static AbstractQoreNode *QFILEINFO_isRelative(QoreObject *self, QoreQFileInfo *qfi, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qfi->isRelative());
}

//bool isRoot () const
static AbstractQoreNode *QFILEINFO_isRoot(QoreObject *self, QoreQFileInfo *qfi, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qfi->isRoot());
}

//bool isSymLink () const
static AbstractQoreNode *QFILEINFO_isSymLink(QoreObject *self, QoreQFileInfo *qfi, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qfi->isSymLink());
}

//bool isWritable () const
static AbstractQoreNode *QFILEINFO_isWritable(QoreObject *self, QoreQFileInfo *qfi, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qfi->isWritable());
}

//QDateTime lastModified () const
static AbstractQoreNode *QFILEINFO_lastModified(QoreObject *self, QoreQFileInfo *qfi, const QoreListNode *params, ExceptionSink *xsink)
{
   QDateTime rv_dt = qfi->lastModified();
   QDate rv_d = rv_dt.date();
   QTime rv_t = rv_dt.time();
   return new DateTimeNode(rv_d.year(), rv_d.month(), rv_d.day(), rv_t.hour(), rv_t.minute(), rv_t.second(), rv_t.msec());
}

//QDateTime lastRead () const
static AbstractQoreNode *QFILEINFO_lastRead(QoreObject *self, QoreQFileInfo *qfi, const QoreListNode *params, ExceptionSink *xsink)
{
   QDateTime rv_dt = qfi->lastRead();
   QDate rv_d = rv_dt.date();
   QTime rv_t = rv_dt.time();
   return new DateTimeNode(rv_d.year(), rv_d.month(), rv_d.day(), rv_t.hour(), rv_t.minute(), rv_t.second(), rv_t.msec());
}

//bool makeAbsolute ()
static AbstractQoreNode *QFILEINFO_makeAbsolute(QoreObject *self, QoreQFileInfo *qfi, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qfi->makeAbsolute());
}

//QString owner () const
static AbstractQoreNode *QFILEINFO_owner(QoreObject *self, QoreQFileInfo *qfi, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qfi->owner().toUtf8().data(), QCS_UTF8);
}

//uint ownerId () const
static AbstractQoreNode *QFILEINFO_ownerId(QoreObject *self, QoreQFileInfo *qfi, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qfi->ownerId());
}

//QString path () const
static AbstractQoreNode *QFILEINFO_path(QoreObject *self, QoreQFileInfo *qfi, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qfi->path().toUtf8().data(), QCS_UTF8);
}

//bool permission ( QFile::Permissions permissions ) const
static AbstractQoreNode *QFILEINFO_permission(QoreObject *self, QoreQFileInfo *qfi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QFile::Permissions permissions = (QFile::Permissions)(p ? p->getAsInt() : 0);
   return new QoreBoolNode(qfi->permission(permissions));
}

//QFile::Permissions permissions () const
static AbstractQoreNode *QFILEINFO_permissions(QoreObject *self, QoreQFileInfo *qfi, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qfi->permissions());
}

//void refresh ()
static AbstractQoreNode *QFILEINFO_refresh(QoreObject *self, QoreQFileInfo *qfi, const QoreListNode *params, ExceptionSink *xsink)
{
   qfi->refresh();
   return 0;
}

//void setCaching ( bool enable )
static AbstractQoreNode *QFILEINFO_setCaching(QoreObject *self, QoreQFileInfo *qfi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   bool enable = p ? p->getAsBool() : false;
   qfi->setCaching(enable);
   return 0;
}

//void setFile ( const QString & file )
//void setFile ( const QFile & file )
//void setFile ( const QDir & dir, const QString & file )
static AbstractQoreNode *QFILEINFO_setFile(QoreObject *self, QoreQFileInfo *qfi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQDir *dir = (QoreQDir *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QDIR, xsink);
      if (!dir) {
         if (!xsink->isException())
            xsink->raiseException("QFILEINFO-SETFILE-PARAM-ERROR", "QFileInfo::setFile() does not know how to handle arguments of class '%s' as passed as the first argument", (reinterpret_cast<const QoreObject *>(p))->getClassName());
         return 0;
      }
      ReferenceHolder<AbstractPrivateData> dirHolder(static_cast<AbstractPrivateData *>(dir), xsink);
      p = get_param(params, 1);
      QString file;
      if (get_qstring(p, file, xsink))
         return 0;
      qfi->setFile(*(static_cast<QDir *>(dir)), file);
      return 0;
   }
   QString file;
   if (get_qstring(p, file, xsink))
      return 0;
   qfi->setFile(file);
   return 0;
}

//qint64 size () const
static AbstractQoreNode *QFILEINFO_size(QoreObject *self, QoreQFileInfo *qfi, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qfi->size());
}

//QString suffix () const
static AbstractQoreNode *QFILEINFO_suffix(QoreObject *self, QoreQFileInfo *qfi, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qfi->suffix().toUtf8().data(), QCS_UTF8);
}

//QString symLinkTarget () const
static AbstractQoreNode *QFILEINFO_symLinkTarget(QoreObject *self, QoreQFileInfo *qfi, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qfi->symLinkTarget().toUtf8().data(), QCS_UTF8);
}

QoreClass *initQFileInfoClass()
{
   QC_QFileInfo = new QoreClass("QFileInfo", QDOM_GUI);
   CID_QFILEINFO = QC_QFileInfo->getID();

   QC_QFileInfo->setConstructor(QFILEINFO_constructor);
   QC_QFileInfo->setCopy((q_copy_t)QFILEINFO_copy);

   QC_QFileInfo->addMethod("absoluteDir",                 (q_method_t)QFILEINFO_absoluteDir);
   QC_QFileInfo->addMethod("absoluteFilePath",            (q_method_t)QFILEINFO_absoluteFilePath);
   QC_QFileInfo->addMethod("absolutePath",                (q_method_t)QFILEINFO_absolutePath);
   QC_QFileInfo->addMethod("baseName",                    (q_method_t)QFILEINFO_baseName);
   QC_QFileInfo->addMethod("bundleName",                  (q_method_t)QFILEINFO_bundleName);
   QC_QFileInfo->addMethod("caching",                     (q_method_t)QFILEINFO_caching);
   QC_QFileInfo->addMethod("canonicalFilePath",           (q_method_t)QFILEINFO_canonicalFilePath);
   QC_QFileInfo->addMethod("canonicalPath",               (q_method_t)QFILEINFO_canonicalPath);
   QC_QFileInfo->addMethod("completeBaseName",            (q_method_t)QFILEINFO_completeBaseName);
   QC_QFileInfo->addMethod("completeSuffix",              (q_method_t)QFILEINFO_completeSuffix);
   QC_QFileInfo->addMethod("created",                     (q_method_t)QFILEINFO_created);
   QC_QFileInfo->addMethod("dir",                         (q_method_t)QFILEINFO_dir);
   QC_QFileInfo->addMethod("exists",                      (q_method_t)QFILEINFO_exists);
   QC_QFileInfo->addMethod("fileName",                    (q_method_t)QFILEINFO_fileName);
   QC_QFileInfo->addMethod("filePath",                    (q_method_t)QFILEINFO_filePath);
   QC_QFileInfo->addMethod("group",                       (q_method_t)QFILEINFO_group);
   QC_QFileInfo->addMethod("groupId",                     (q_method_t)QFILEINFO_groupId);
   QC_QFileInfo->addMethod("isAbsolute",                  (q_method_t)QFILEINFO_isAbsolute);
   QC_QFileInfo->addMethod("isBundle",                    (q_method_t)QFILEINFO_isBundle);
   QC_QFileInfo->addMethod("isDir",                       (q_method_t)QFILEINFO_isDir);
   QC_QFileInfo->addMethod("isExecutable",                (q_method_t)QFILEINFO_isExecutable);
   QC_QFileInfo->addMethod("isFile",                      (q_method_t)QFILEINFO_isFile);
   QC_QFileInfo->addMethod("isHidden",                    (q_method_t)QFILEINFO_isHidden);
   QC_QFileInfo->addMethod("isReadable",                  (q_method_t)QFILEINFO_isReadable);
   QC_QFileInfo->addMethod("isRelative",                  (q_method_t)QFILEINFO_isRelative);
   QC_QFileInfo->addMethod("isRoot",                      (q_method_t)QFILEINFO_isRoot);
   QC_QFileInfo->addMethod("isSymLink",                   (q_method_t)QFILEINFO_isSymLink);
   QC_QFileInfo->addMethod("isWritable",                  (q_method_t)QFILEINFO_isWritable);
   QC_QFileInfo->addMethod("lastModified",                (q_method_t)QFILEINFO_lastModified);
   QC_QFileInfo->addMethod("lastRead",                    (q_method_t)QFILEINFO_lastRead);
   QC_QFileInfo->addMethod("makeAbsolute",                (q_method_t)QFILEINFO_makeAbsolute);
   QC_QFileInfo->addMethod("owner",                       (q_method_t)QFILEINFO_owner);
   QC_QFileInfo->addMethod("ownerId",                     (q_method_t)QFILEINFO_ownerId);
   QC_QFileInfo->addMethod("path",                        (q_method_t)QFILEINFO_path);
   QC_QFileInfo->addMethod("permission",                  (q_method_t)QFILEINFO_permission);
   QC_QFileInfo->addMethod("permissions",                 (q_method_t)QFILEINFO_permissions);
   QC_QFileInfo->addMethod("refresh",                     (q_method_t)QFILEINFO_refresh);
   QC_QFileInfo->addMethod("setCaching",                  (q_method_t)QFILEINFO_setCaching);
   QC_QFileInfo->addMethod("setFile",                     (q_method_t)QFILEINFO_setFile);
   QC_QFileInfo->addMethod("size",                        (q_method_t)QFILEINFO_size);
   QC_QFileInfo->addMethod("suffix",                      (q_method_t)QFILEINFO_suffix);
   QC_QFileInfo->addMethod("symLinkTarget",               (q_method_t)QFILEINFO_symLinkTarget);

   return QC_QFileInfo;
}
