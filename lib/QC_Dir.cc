/*
  QC_Dir.cc

  Qore Programming Language

  Copyright (C) 2003 - 2009 David Nichols

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

#include <errno.h>

qore_classid_t CID_DIR;

static void DIR_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink) {
  // get character set name if available
  const QoreEncoding *cs;
  const QoreStringNode *p0 = test_string_param(params, 0);
  if (p0) {
    cs = QEM.findCreate(p0);
  }
  else {
    cs = QCS_DEFAULT;
  }

  SimpleRefHolder<Dir> d(new Dir(xsink, cs));
  if (*xsink)
     return;
  self->setPrivate(CID_DIR, d.release());
}

static void DIR_copy(QoreObject *self, QoreObject *old, class Dir *d, ExceptionSink *xsink) {
   SimpleRefHolder<Dir> nd(new Dir(xsink, *d));
   if (*xsink)
      return;
  
   self->setPrivate(CID_DIR, nd.release());
}

// chDir(dirname)
static AbstractQoreNode *DIR_chdir(QoreObject *self, class Dir *d, const QoreListNode *params, ExceptionSink *xsink) {
  const QoreStringNode *p0;

  p0 = test_string_param(params, 0);
  if (!p0) {
    xsink->raiseException("DIR-OPEN-PARAMETER-ERROR", "expecting string dirname argument for Dir::chDir()");
    return 0;
  }

  return get_bool_node(d->chdir(p0->getBuffer(), xsink) ? 0 : 1);
}

// path(): returns the actual directory name
static AbstractQoreNode *DIR_path(QoreObject *self, class Dir *d, const QoreListNode *params, ExceptionSink *xsink) {
   return d->dirname();
}

// exists(): return 0 if the ch-dired directory exists
static AbstractQoreNode *DIR_exists(QoreObject *self, class Dir *d, const QoreListNode *params, ExceptionSink *xsink) {
  return get_bool_node(d->checkPath()? 0: 1); 
}

// create([mode]): create all the directories in the path
static AbstractQoreNode *DIR_create(QoreObject *self, class Dir *d, const QoreListNode *params, ExceptionSink *xsink) {
  const AbstractQoreNode *p = get_param(params, 0);
  int mode;
  if (!is_nothing(p))
    mode = p->getAsInt();
  else
    mode = 0777;

  // create all directories from / on
  return new QoreBigIntNode(d->create(mode, xsink)); // throws exception
}

// chmod(mode)
static AbstractQoreNode *DIR_chmod(QoreObject *self, class Dir *d, const QoreListNode *params, ExceptionSink *xsink) {
  const AbstractQoreNode *p0 = get_param(params, 0);
  if (is_nothing(p0)) {
    xsink->raiseException("DIR-CHMOD-PARAMETER-ERROR", "expecting integer mode as sole argument to Dir::chmod()");
    return 0;
  }

  d->chmod(p0->getAsInt(), xsink);

  return 0;
}

// chown(username|userid)
static AbstractQoreNode *DIR_chown(QoreObject *self, class Dir *d, const QoreListNode *params, ExceptionSink *xsink) {
   const AbstractQoreNode *p = get_param(params, 0);
   uid_t uid;
   if (is_nothing(p)) {
      xsink->raiseException("DIR-CHOWN-PARAMETER-ERROR", "expecting username or userid as parameter of Dir::chown()");
      return 0;
   }

   if (p->getType()==NT_INT) {
      uid=p->getAsInt();
   }
   else if (p->getType()==NT_STRING) {
      // Try getting UID for username
      int rc = q_uname2uid(reinterpret_cast<const QoreStringNode *>(p)->getBuffer(), uid);
      if (rc) {
	 xsink->raiseException("DIR-CHOWN-PARAMETER-ERROR", "no userid found for user '%s'", ((QoreStringNode*)p)->getBuffer());
	 return 0;
      }
   }
   else {
      xsink->raiseException("DIR-CHOWN-PARAMETER-ERROR", "expecting username or userid as parameter of Dir::chown()");
      return 0;
   }

   d->chown(uid, (gid_t)-1, xsink);

   return 0;
}

// chgrp(groupname|groupid)
static AbstractQoreNode *DIR_chgrp(QoreObject *self, class Dir *d, const QoreListNode *params, ExceptionSink *xsink) {
  const AbstractQoreNode *p = get_param(params, 0);
  gid_t gid;
  if (is_nothing(p)) {
    xsink->raiseException("DIR-CHGRP-PARAMETER-ERROR", "expecting groupname or groupid as parameter of Dir::chgrp()");
    return 0;
  }

  if (p->getType()==NT_INT) {
    gid=p->getAsInt();
  }
  else if (p->getType()==NT_STRING) {
     // Try getting GID for name
     int rc = q_gname2gid(reinterpret_cast<const QoreStringNode *>(p)->getBuffer(), gid);
     if (rc) {
	xsink->raiseException("DIR-CHGRP-PARAMETER-ERROR", "no groupid found for group '%s'", ((QoreStringNode*)p)->getBuffer());
	return 0;
     }
  }
  else {
    xsink->raiseException("DIR-CHGRP-PARAMETER-ERROR", "expecting groupname or groupid as parameter of Dir::chgrp()");
    return 0;
  }

  d->chown((uid_t)-1, gid, xsink);

  return 0;
}

// mkdir(dirname, [mode]): make subdirectory with given mode
static AbstractQoreNode *DIR_mkdir(QoreObject *self, class Dir *d, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = test_string_param(params, 0);
   if (!p0) {
      xsink->raiseException("DIR-MKDIR-PARAMETER-ERROR", "expecting string as first parameter of mkdir");
      return 0;
   }

   // check if there is a path delimiter in
   const char *dname=p0->getBuffer();
   if (strchr(dname, '/')) {
     xsink->raiseException("DIR-MKDIR-PARAMETER-ERROR", "only single, direct subdirectories are allowed");
     return 0;
   }

   // get mode parameter (if any, default = 0777)
   const AbstractQoreNode *p1 = get_param(params, 1);

   std::string path = d->getPath(dname);
   d->mkdir(xsink, path.c_str(), p1 ? p1->getAsInt() : 0777);
   return 0;
}

// rmdir(dirname): remove direct subdirectory
static AbstractQoreNode *DIR_rmdir(QoreObject *self, class Dir *d, const QoreListNode *params, ExceptionSink *xsink) {
  const QoreStringNode *p0 = test_string_param(params, 0);
  if (!p0) {
    xsink->raiseException("DIR-RMDIR-PARAMETER-ERROR", "expecting string as first parameter of rmdir");
    return 0;
  }
  
  // check if there is a path delimiter in
  const char *dname=p0->getBuffer();  
  if (strchr(dname, '/')) {
    xsink->raiseException("DIR-RMDIR-PARAMETER-ERROR", "only direct subdirectories are allowed");
    return 0;
  }

  d->rmdir(dname, xsink);

  return 0;
}

// list()
// lists all files and directories, but ignores '.' and '..'
static AbstractQoreNode *DIR_list(QoreObject *self, class Dir *d, const QoreListNode *params, ExceptionSink *xsink) {
   // check for optional regular expression string
   const QoreStringNode *p0 = test_string_param(params, 0);
   if (p0) {
      const AbstractQoreNode *p1 = get_param(params, 1);
      return d->list(xsink, -1, p0, p1 ? p1->getAsInt() : 0);
   }
   return d->list(xsink, -1);
}

// listFiles()
// lists all files
static AbstractQoreNode *DIR_listFiles(QoreObject *self, class Dir *d, const QoreListNode *params, ExceptionSink *xsink) {
   // check for optional regular expression string
   const QoreStringNode *p0 = test_string_param(params, 0);
   if (p0) {
      const AbstractQoreNode *p1 = get_param(params, 1);
      return d->list(xsink, S_IFMT^S_IFDIR, p0, p1 ? p1->getAsInt() : 0);
   }
   return d->list(xsink, S_IFMT^S_IFDIR);
}


// listDirs()
// lists all directoreis but ignore '.' and '..'
static AbstractQoreNode *DIR_listDirs(QoreObject *self, class Dir *d, const QoreListNode *params, ExceptionSink *xsink) {
   // check for optional regular expression string
   const QoreStringNode *p0 = test_string_param(params, 0);
   if (p0) {
      const AbstractQoreNode *p1 = get_param(params, 1);
      return d->list(xsink, S_IFDIR, p0, p1 ? p1->getAsInt() : 0);
   }
   return d->list(xsink, S_IFDIR);
}

// openFile(filename, [flags, mode, charset])
// throw exception from File::open2()
static AbstractQoreNode *DIR_openFile(QoreObject *self, class Dir *d, const QoreListNode *params, ExceptionSink *xsink) {
  const QoreStringNode *p0 = test_string_param(params, 0);
  if (!p0) {
    xsink->raiseException("DIR-OPENFILE-PARAMETER-ERROR", "expecting string filename as first argument of Dir::openFile()");
    return 0;
  }

  // check if there is a path delimiter in
  const char *fname=p0->getBuffer();  
  if (strchr(fname, '/')) {
    xsink->raiseException("DIR-OPENFILE-PARAMETER-ERROR", "only filenames without path (i.e. without '/' characters) are allowed");
    return 0;
  }
  
  int flags, mode;
  const AbstractQoreNode *p = get_param(params, 1);
  if (!is_nothing(p))
    flags = p->getAsInt();
  else
    flags = O_RDONLY;
  
  p = get_param(params, 2);
  if (!is_nothing(p))
    mode = p->getAsInt();
  else
    mode = 0666;
  
  const QoreStringNode *pstr = test_string_param(params, 3);
  const QoreEncoding *charset;
  if (pstr)
    charset = QEM.findCreate(pstr);
  else
    charset = QCS_DEFAULT;

  // open the file with exception
  ReferenceHolder<File> f(new File(charset), xsink);
  std::string path = d->getPath(fname);

  int r = f->open2(xsink, path.c_str(), flags, mode, charset);
  if (r!=0) {
     f.release(); // release the object
     return 0;
  }

  // create the QoreObject and set the File object as private data of the class tagged with the CID_FILE class ID
  QoreObject *o = new QoreObject(QC_File, getProgram());
  o->setPrivate(CID_FILE, f.release());
  return o;
}

// openDir(subdirectory, [encoding])
static AbstractQoreNode *DIR_openDir(QoreObject *self, class Dir *d, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0;
   p0 = test_string_param(params, 0);
   if (!p0) {
      xsink->raiseException("DIR-OPENDIR-PARAMETER-ERROR", "expecting string directory name as first argument of Dir::openDir()");
      return 0;
   }

   // check if there is a path delimiter in
   const char *dirname=p0->getBuffer();  
   if (strchr(dirname, '/')) {
      xsink->raiseException("DIR-OPENDIR-PARAMETER-ERROR", "only direct directory names without path (i.e. without '/' characters) are allowed");
      return 0;
   }
   
   const QoreStringNode *pstr = test_string_param(params, 1);
   const QoreEncoding *charset;
   if (pstr)
      charset = QEM.findCreate(pstr);
   else
      charset = d->getEncoding();

   // open the file with exception
   ReferenceHolder<Dir> dc(new Dir(xsink, charset, d->getPath(dirname).c_str()), xsink);
   
   // create the qoreObject and set the File object as private data of the class tagged with the CID_FILE class ID
   QoreObject *o=new QoreObject(getRootNS()->rootFindClass("Dir"), getProgram());
   o->setPrivate(CID_DIR, dc.release());
   
   return o;
}

// removeFile(filename): remove the file
static AbstractQoreNode *DIR_removeFile(QoreObject *self, class Dir *d, const QoreListNode *params, ExceptionSink *xsink) {
  const QoreStringNode *p0;
  if (!(p0 = test_string_param(params, 0))) {
    return 0;
  }

  // check if there is a path delimiter in
  const char *fname = p0->getBuffer();  
  if (strchr(fname, '/')) {
    xsink->raiseException("DIR-REMOVEFILE-PARAMETER-ERROR", "only filenames without path (i.e. without '/' characters) are allowed");
    return 0;
  }

  std::string path = d->getPath(fname);
  errno = 0; // clear errno flag
  if (unlink(path.c_str()) && errno != ENOENT) {
     xsink->raiseException("DIR-REMOVEFILE-ERROR", "error on removing file '%s': %s", p0->getBuffer(), strerror(errno));
     return 0;
  }

  // if an errno was set it must be ENOENT at this point.
  // so we return that no file is removed
  return get_bool_node(errno ? 0 : 1);
}

// init the class
QoreClass *initDirClass() {
   QORE_TRACE("initDirClass()");

   QoreClass *QC_DIR = new QoreClass("Dir", QDOM_FILESYSTEM);
   CID_DIR = QC_DIR->getID();

   QC_DIR->setConstructor(DIR_constructor);
   QC_DIR->setCopy((q_copy_t)DIR_copy);

   QC_DIR->addMethod("chdir",		(q_method_t)DIR_chdir);
   QC_DIR->addMethod("path",		(q_method_t)DIR_path);
   QC_DIR->addMethod("exists",		(q_method_t)DIR_exists);

   QC_DIR->addMethod("create",		(q_method_t)DIR_create);
   QC_DIR->addMethod("chown",		(q_method_t)DIR_chown);
   QC_DIR->addMethod("chgrp",		(q_method_t)DIR_chgrp);
   QC_DIR->addMethod("chmod",		(q_method_t)DIR_chmod);

   QC_DIR->addMethod("mkdir",		(q_method_t)DIR_mkdir);
   QC_DIR->addMethod("rmdir",		(q_method_t)DIR_rmdir);

   QC_DIR->addMethod("list",		(q_method_t)DIR_list);
   QC_DIR->addMethod("listFiles",	(q_method_t)DIR_listFiles);
   QC_DIR->addMethod("listDirs",	(q_method_t)DIR_listDirs);

   QC_DIR->addMethod("openDir",		(q_method_t)DIR_openDir);
   QC_DIR->addMethod("openFile",	(q_method_t)DIR_openFile);
   QC_DIR->addMethod("removeFile",	(q_method_t)DIR_removeFile);


   return QC_DIR;
}
