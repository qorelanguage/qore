/*
  QC_Dir.cpp

  Qore Programming Language

  Copyright (C) 2003 - 2010 David Nichols

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
#include <qore/intern/QC_Dir.h>
#include <qore/intern/QC_File.h>

#include <sys/types.h>
#include <errno.h>

qore_classid_t CID_DIR;
static QoreClass *QC_DIR;

static void DIR_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink) {
   // get character set encoding name if available
   const QoreEncoding *cs = get_encoding_param(params, 0);

   SimpleRefHolder<Dir> d(new Dir(xsink, cs));
   if (*xsink)
      return;
   self->setPrivate(CID_DIR, d.release());
}

static void DIR_copy(QoreObject *self, QoreObject *old, Dir *d, ExceptionSink *xsink) {
   SimpleRefHolder<Dir> nd(new Dir(xsink, *d));
   if (*xsink)
      return;
  
   self->setPrivate(CID_DIR, nd.release());
}

// chDir(dirname)
static AbstractQoreNode *DIR_chdir(QoreObject *self, Dir *d, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(params, 0);
   return get_bool_node(d->chdir(p0->getBuffer(), xsink) ? 0 : 1);
}

// path(): returns the actual directory name
static AbstractQoreNode *DIR_path(QoreObject *self, Dir *d, const QoreListNode *params, ExceptionSink *xsink) {
   return d->dirname();
}

// exists(): return 0 if the ch-dired directory exists
static AbstractQoreNode *DIR_exists(QoreObject *self, Dir *d, const QoreListNode *params, ExceptionSink *xsink) {
   return get_bool_node(d->checkPath() ? false : true); 
}

// create([mode]): create all the directories in the path
static AbstractQoreNode *DIR_create(QoreObject *self, Dir *d, const QoreListNode *params, ExceptionSink *xsink) {
   int mode = (int)HARD_QORE_INT(params, 0);
   // create all directories from / on
   int rc = d->create(mode, xsink); // throws exception
   return *xsink ? 0 : new QoreBigIntNode(rc); // throws exception
}

// chmod(mode)
static AbstractQoreNode *DIR_chmod(QoreObject *self, Dir *d, const QoreListNode *params, ExceptionSink *xsink) {
   d->chmod((int)HARD_QORE_INT(params, 0), xsink);
   return 0;
}

// chown(userid)
static AbstractQoreNode *DIR_chown_int(QoreObject *self, Dir *d, const QoreListNode *params, ExceptionSink *xsink) {
   uid_t uid = (uid_t)HARD_QORE_INT(params, 0);
   d->chown(uid, (gid_t)-1, xsink);
   return 0;
}

// chown(username)
static AbstractQoreNode *DIR_chown_str(QoreObject *self, Dir *d, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(params, 0);
   uid_t uid;

   // Try getting UID for username
   int rc = q_uname2uid(p0->getBuffer(), uid);
   if (rc) {
      xsink->raiseException("DIR-CHOWN-PARAMETER-ERROR", "no userid found for user '%s'", p0->getBuffer());
      return 0;
   }

   d->chown(uid, (gid_t)-1, xsink);
   return 0;
}

static AbstractQoreNode *DIR_chgrp_int(QoreObject *self, Dir *d, const QoreListNode *params, ExceptionSink *xsink) {
   gid_t gid = (gid_t)HARD_QORE_INT(params, 0);
   d->chown((uid_t)-1, gid, xsink);
   return 0;
}

// chgrp(groupname|groupid)
static AbstractQoreNode *DIR_chgrp_str(QoreObject *self, Dir *d, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(params, 0);
   gid_t gid;

   // Try getting GID for name
   int rc = q_gname2gid(p0->getBuffer(), gid);
   if (rc) {
      xsink->raiseException("DIR-CHGRP-PARAMETER-ERROR", "no groupid found for group '%s'", p0->getBuffer());
      return 0;
   }

   d->chown((uid_t)-1, gid, xsink);
   return 0;
}

// mkdir(dirname, [mode]): make subdirectory with given mode
static AbstractQoreNode *DIR_mkdir(QoreObject *self, Dir *d, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(params, 0);

   // check if there is a path delimiter in
   const char *dname = p0->getBuffer();
   if (strchr(dname, '/')) {
     xsink->raiseException("DIR-MKDIR-PARAMETER-ERROR", "only single, direct subdirectories are allowed");
     return 0;
   }

   // get mode parameter (if any, default = 0777)
   int mode = (int)HARD_QORE_INT(params, 1);

   d->mkdir(xsink, p0->getBuffer(), mode);
   return 0;
}

// rmdir(dirname): remove direct subdirectory
static AbstractQoreNode *DIR_rmdir(QoreObject *self, Dir *d, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(params, 0);
  
   // check if there is a path delimiter in
   const char *dname = p0->getBuffer();  
   if (strchr(dname, '/')) {
      xsink->raiseException("DIR-RMDIR-PARAMETER-ERROR", "only direct subdirectories are allowed");
      return 0;
   }

   d->rmdir(dname, xsink);
   return 0;
}

// list()
// lists all files and directories, but ignores '.' and '..'
static AbstractQoreNode *DIR_list(QoreObject *self, Dir *d, const QoreListNode *params, ExceptionSink *xsink) {
   return d->list(xsink, -1);
}

static AbstractQoreNode *DIR_list_str(QoreObject *self, Dir *d, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(params, 0);
   return d->list(xsink, -1, p0, 0);
}

static AbstractQoreNode *DIR_list_str_int(QoreObject *self, Dir *d, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(params, 0);
   int regex_options = (int)HARD_QORE_INT(params, 1);   
   return d->list(xsink, -1, p0, regex_options);
}

// listFiles()
static AbstractQoreNode *DIR_listFiles(QoreObject *self, Dir *d, const QoreListNode *params, ExceptionSink *xsink) {
   return d->list(xsink, S_IFMT^S_IFDIR);
}

// lists all files
static AbstractQoreNode *DIR_listFiles_str(QoreObject *self, Dir *d, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(params, 0);
   return d->list(xsink, S_IFMT^S_IFDIR, p0, 0);
}

// lists all files
static AbstractQoreNode *DIR_listFiles_str_int(QoreObject *self, Dir *d, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(params, 0);
   int regex_options = (int)HARD_QORE_INT(params, 1);
   return d->list(xsink, S_IFMT^S_IFDIR, p0, regex_options);
}

// listDirs()
static AbstractQoreNode *DIR_listDirs(QoreObject *self, Dir *d, const QoreListNode *params, ExceptionSink *xsink) {
   return d->list(xsink, S_IFDIR);
}

// lists all directoreis but ignore '.' and '..'
static AbstractQoreNode *DIR_listDirs_str(QoreObject *self, Dir *d, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(params, 0);
   return d->list(xsink, S_IFDIR, p0, 0);
}

// lists all directoreis but ignore '.' and '..'
static AbstractQoreNode *DIR_listDirs_str_int(QoreObject *self, Dir *d, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(params, 0);
   int regex_options = (int)HARD_QORE_INT(params, 1);
   return d->list(xsink, S_IFDIR, p0, regex_options);
}

// openFile(filename, [flags, mode, charset])
// throw exception from File::open2()
static AbstractQoreNode *DIR_openFile(QoreObject *self, Dir *d, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(params, 0);

   // check if there is a path delimiter in
   const char *fname = p0->getBuffer();  
   if (strchr(fname, '/')) {
      xsink->raiseException("DIR-OPENFILE-PARAMETER-ERROR", "only filenames without path (i.e. without '/' characters) are allowed");
      return 0;
   }
  
   int flags = (int)HARD_QORE_INT(params, 1);
   int mode = (int)HARD_QORE_INT(params, 2);

   const QoreEncoding *charset = get_encoding_param(params, 3);

   // open the file with exception
   ReferenceHolder<File> f(new File(charset), xsink);
   std::string path = d->getPath(fname);

   int r = f->open2(xsink, path.c_str(), flags, mode, charset);
   if (r != 0) {
      assert(*xsink);
      f.release(); // release the object
      return 0;
   }

   // create the QoreObject and set the File object as private data of the class tagged with the CID_FILE class ID
   QoreObject *o = new QoreObject(QC_FILE, getProgram());
   o->setPrivate(CID_FILE, f.release());
   return o;
}

// openDir(subdirectory, [encoding])
static AbstractQoreNode *DIR_openDir(QoreObject *self, Dir *d, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(params, 0);

   // check if there is a path delimiter in
   const char *dirname = p0->getBuffer();  
   if (strchr(dirname, '/')) {
      xsink->raiseException("DIR-OPENDIR-PARAMETER-ERROR", "only direct directory names without path (i.e. without '/' characters) are allowed");
      return 0;
   }
   
   const QoreEncoding *charset = get_encoding_param(params, 1);

   // open the file with exception
   ReferenceHolder<Dir> dc(new Dir(xsink, charset, d->getPath(dirname).c_str()), xsink);
   
   // create the qoreObject and set the Dir object as private data of the class tagged with the CID_DIR class ID
   QoreObject *o = new QoreObject(QC_DIR, getProgram());
   o->setPrivate(CID_DIR, dc.release());
   
   return o;
}

// removeFile(filename): remove the file
static AbstractQoreNode *DIR_removeFile(QoreObject *self, Dir *d, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(params, 0);

   // check if there is a path delimiter in
   const char *fname = p0->getBuffer();  
   if (strchr(fname, '/')) {
      xsink->raiseException("DIR-REMOVEFILE-PARAMETER-ERROR", "only filenames without path (i.e. without '/' characters) are allowed");
      return 0;
   }

   std::string path = d->getPath(fname);
   errno = 0; // clear errno flag
   if (unlink(path.c_str()) && errno != ENOENT) {
      xsink->raiseErrnoException("DIR-REMOVEFILE-ERROR", errno, "error removing file '%s'", p0->getBuffer());
      return 0;
   }

   // if an errno was set it must be ENOENT at this point.
   // so we return that no file is removed
   return get_bool_node(errno ? 0 : 1);
}

// init the class
QoreClass *initDirClass() {
   QORE_TRACE("initDirClass()");

   assert(QC_FILE);

   QC_DIR = new QoreClass("Dir", QDOM_FILESYSTEM);
   CID_DIR = QC_DIR->getID();

   QC_DIR->setConstructorExtended(DIR_constructor, false, QC_NO_FLAGS, QDOM_DEFAULT);
   QC_DIR->setConstructorExtended(DIR_constructor, false, QC_NO_FLAGS, QDOM_DEFAULT, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   QC_DIR->setCopy((q_copy_t)DIR_copy);

   QC_DIR->addMethodExtended("chdir",		(q_method_t)DIR_chdir, false, QC_NO_FLAGS, QDOM_DEFAULT, boolTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   // *string Dir::path()  
   QC_DIR->addMethodExtended("path",		(q_method_t)DIR_path, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, stringOrNothingTypeInfo);

   QC_DIR->addMethodExtended("exists",		(q_method_t)DIR_exists, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, boolTypeInfo);

   QC_DIR->addMethodExtended("create",		(q_method_t)DIR_create, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 1, softBigIntTypeInfo, new QoreBigIntNode(0777));

   QC_DIR->addMethodExtended("chmod",		(q_method_t)DIR_chmod, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 1, softBigIntTypeInfo, QORE_PARAM_NO_ARG);

   QC_DIR->addMethodExtended("chown",		(q_method_t)DIR_chown_int, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 1, softBigIntTypeInfo, QORE_PARAM_NO_ARG);
   QC_DIR->addMethodExtended("chown",		(q_method_t)DIR_chown_str, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   QC_DIR->addMethodExtended("chgrp",		(q_method_t)DIR_chgrp_int, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 1, softBigIntTypeInfo, QORE_PARAM_NO_ARG);
   QC_DIR->addMethodExtended("chgrp",		(q_method_t)DIR_chgrp_str, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   QC_DIR->addMethodExtended("mkdir",		(q_method_t)DIR_mkdir, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 2, stringTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, new QoreBigIntNode(0777));

   QC_DIR->addMethodExtended("rmdir",		(q_method_t)DIR_rmdir, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   QC_DIR->addMethodExtended("list",		(q_method_t)DIR_list, false, QC_NO_FLAGS, QDOM_DEFAULT, listTypeInfo);
   QC_DIR->addMethodExtended("list",		(q_method_t)DIR_list_str, false, QC_NO_FLAGS, QDOM_DEFAULT, listTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   QC_DIR->addMethodExtended("list",		(q_method_t)DIR_list_str_int, false, QC_NO_FLAGS, QDOM_DEFAULT, listTypeInfo, 2, stringTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, QORE_PARAM_NO_ARG);

   QC_DIR->addMethodExtended("listFiles",	(q_method_t)DIR_listFiles, false, QC_NO_FLAGS, QDOM_DEFAULT, listTypeInfo);
   QC_DIR->addMethodExtended("listFiles",	(q_method_t)DIR_listFiles_str, false, QC_NO_FLAGS, QDOM_DEFAULT, listTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   QC_DIR->addMethodExtended("listFiles",	(q_method_t)DIR_listFiles_str_int, false, QC_NO_FLAGS, QDOM_DEFAULT, listTypeInfo, 2, stringTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, QORE_PARAM_NO_ARG);

   QC_DIR->addMethodExtended("listDirs",	(q_method_t)DIR_listDirs, false, QC_NO_FLAGS, QDOM_DEFAULT, listTypeInfo);
   QC_DIR->addMethodExtended("listDirs",	(q_method_t)DIR_listDirs_str, false, QC_NO_FLAGS, QDOM_DEFAULT, listTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   QC_DIR->addMethodExtended("listDirs",	(q_method_t)DIR_listDirs_str_int, false, QC_NO_FLAGS, QDOM_DEFAULT, listTypeInfo, 2, stringTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, QORE_PARAM_NO_ARG);

   QC_DIR->addMethodExtended("openDir",		(q_method_t)DIR_openDir, false, QC_NO_FLAGS, QDOM_DEFAULT, QC_DIR->getTypeInfo(), 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   QC_DIR->addMethodExtended("openDir",		(q_method_t)DIR_openDir, false, QC_NO_FLAGS, QDOM_DEFAULT, QC_DIR->getTypeInfo(), 2, stringTypeInfo, QORE_PARAM_NO_ARG, stringTypeInfo, QORE_PARAM_NO_ARG);

   QC_DIR->addMethodExtended("openFile",	(q_method_t)DIR_openFile, false, QC_NO_FLAGS, QDOM_DEFAULT, QC_FILE->getTypeInfo(), 3, stringTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, new QoreBigIntNode(O_RDONLY), softBigIntTypeInfo, new QoreBigIntNode(0666));
   QC_DIR->addMethodExtended("openFile",	(q_method_t)DIR_openFile, false, QC_NO_FLAGS, QDOM_DEFAULT, QC_FILE->getTypeInfo(), 4, stringTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, new QoreBigIntNode(O_RDONLY), softBigIntTypeInfo, new QoreBigIntNode(0666), stringTypeInfo, QORE_PARAM_NO_ARG);

   QC_DIR->addMethodExtended("removeFile",	(q_method_t)DIR_removeFile, false, QC_NO_FLAGS, QDOM_DEFAULT, boolTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   return QC_DIR;
}
