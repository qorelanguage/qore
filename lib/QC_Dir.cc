/*
  QC_Dir.cc

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
#include <qore/intern/QC_Dir.h>
#include <qore/intern/QC_File.h>

qore_classid_t CID_DIR;



static void DIR_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink) {
  // get character set name if available
  const QoreEncoding *cs;
  const QoreStringNode *p0 = test_string_param(params, 0);
  if(p0) {
    cs = QEM.findCreate(p0);
  }
  else {
    cs = QCS_DEFAULT;
  }
  
  SimpleRefHolder<Dir> d(new Dir(cs, xsink));
  if (*xsink)
     return;
  self->setPrivate(CID_DIR, d.release());
}

static void DIR_copy(QoreObject *self, QoreObject *old, class Dir *d, ExceptionSink *xsink) {
  //self->setPrivate(CID_DIR, new Dir(d->getEncoding()));
  SimpleRefHolder<Dir> nd(new Dir(d->getEncoding(), xsink));
  if (*xsink)
     return;
  if (d->dirname())
     nd->chdir(d->dirname(), xsink);
  self->setPrivate(CID_DIR, nd.release());
}


// ---------------------------------


// chDir(dirname)
static AbstractQoreNode *DIR_chdir(QoreObject *self, class Dir *d, const QoreListNode *params, ExceptionSink *xsink) {
  const QoreStringNode *p0;

  p0 = test_string_param(params, 0);
  if(!p0) {
    xsink->raiseException("DIR-OPEN-PARAMETER-ERROR", "expecting string dirname argument for Dir::chDir()");
    return 0;
  }

  return get_bool_node(d->chdir(p0->getBuffer(), xsink) ? 0: 1);
}

// path(): returns the actual directory name
static AbstractQoreNode *DIR_getdirname(QoreObject *self, class Dir *d, const QoreListNode *params, ExceptionSink *xsink) {
   const char *dir = d->dirname();
   return dir ? new QoreStringNode(dir) : 0;
}

// exists(): return 0 if the ch-dired directory exists
static AbstractQoreNode *DIR_exists(QoreObject *self, class Dir *d, const QoreListNode *params, ExceptionSink *xsink) {
  return get_bool_node(d->checkPath()? 0: 1); 
}


// ---------------------------------


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
  if(!p0) {
    xsink->raiseException("DIR-CHMOD-PARAMETER-ERROR", "expecting mode as parameter of Dir::chmod()");
    return 0;
  }

  if(chmod(d->dirname(), p0->getAsInt())) {
    xsink->raiseException("DIR-CHMOD-ERROR", "error in Dir::chmod(): %s", strerror(errno));
    return 0;
  }

  return 0;
}

// chown(username|userid)
static AbstractQoreNode *DIR_chown(QoreObject *self, class Dir *d, const QoreListNode *params, ExceptionSink *xsink) {
  const AbstractQoreNode *p=get_param(params, 0);
  int uid;
  if(is_nothing(p)) {
    xsink->raiseException("DIR-CHOWN-PARAMETER-ERROR", "expecting username or userid as parameter of Dir::chown()");
    return 0;
  }

  if(p->getType()==NT_INT) {
    uid=p->getAsInt();
  }
  else if(p->getType()==NT_STRING) {
    struct passwd *pwd=getpwnam(((QoreStringNode*)p)->getBuffer());   // Try getting UID for username
    if(pwd == NULL) {
      xsink->raiseException("DIR-CHOWN-PARAMETER-ERROR", "no userid found for user '%s'", ((QoreStringNode*)p)->getBuffer());
      return 0;
    }    
    uid = pwd->pw_uid;
  }
  else {
    xsink->raiseException("DIR-CHOWN-PARAMETER-ERROR", "expecting username or userid as parameter of Dir::chown()");
    return 0;
  }

  if(chown(d->dirname(), uid, (gid_t)-1)) {
    xsink->raiseException("DIR-CHOWN-ERROR", "error in Dir::chown(): %s", strerror(errno));
    return 0;
  }

  return 0;
}

// chgrp(groupname|groupid)
static AbstractQoreNode *DIR_chgrp(QoreObject *self, class Dir *d, const QoreListNode *params, ExceptionSink *xsink) {
  const AbstractQoreNode *p=get_param(params, 0);
  int gid;
  if(is_nothing(p)) {
    xsink->raiseException("DIR-CHGRP-PARAMETER-ERROR", "expecting groupname or groupid as parameter of Dir::chgrp()");
    return 0;
  }

  if(p->getType()==NT_INT) {
    gid=p->getAsInt();
  }
  else if(p->getType()==NT_STRING) {
    struct passwd *pwd=getpwnam(((QoreStringNode*)p)->getBuffer());   // Try getting GID for name
    if(pwd == NULL) {
      xsink->raiseException("DIR-CHGRP-PARAMETER-ERROR", "no groupid found for group '%s'", ((QoreStringNode*)p)->getBuffer());
      return 0;
    }    
    gid = pwd->pw_gid;
  }
  else {
    xsink->raiseException("DIR-CHGRP-PARAMETER-ERROR", "expecting groupname or groupid as parameter of Dir::chgrp()");
    return 0;
  }

  if(chown(d->dirname(), (uid_t)-1, gid)) {
    xsink->raiseException("DIR-CHGRP-ERROR", "error in Dir::chgrp(): %s", strerror(errno));
    return 0;
  }

  return 0;
}


// ---------------------------------


// mkdir(dirname, [mode]): make subdirectory with given mode
static AbstractQoreNode *DIR_mkdir(QoreObject *self, class Dir *d, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = test_string_param(params, 0);
   if(!p0) {
      xsink->raiseException("DIR-MKDIR-PARAMETER-ERROR", "expecting string as first parameter of mkdir");
      return 0;
   }

   // check if there is a path delimiter in
   const char *dname=p0->getBuffer();
   if(strchr(dname, '/')) {
     xsink->raiseException("DIR-MKDIR-PARAMETER-ERROR", "only direct subdirectories are allowed");
     return 0;
   }

   int mode;
   const AbstractQoreNode *p1 = get_param(params, 1);
   if(p1) {
      mode = p1->getAsInt();
   }
   else {
      mode = 0777;
   }

  std::string path=std::string(d->dirname())+"/"+std::string(dname);
  if(mkdir(path.c_str(), mode)) {
    xsink->raiseException("DIR-MKDIR-ERROR", "error on creating subdirectory '%s' in '%s': %s", p0->getBuffer(), d->dirname(), strerror(errno));
    return 0;
  }

  return 0;
}

// rmdir(dirname): remove direct subdirectory
static AbstractQoreNode *DIR_rmdir(QoreObject *self, class Dir *d, const QoreListNode *params, ExceptionSink *xsink) {
  const QoreStringNode *p0 = test_string_param(params, 0);
  if(!p0) {
    xsink->raiseException("DIR-RMDIR-PARAMETER-ERROR", "expecting string as first parameter of rmdir");
    return 0;
  }
  
  // check if there is a path delimiter in
  const char *dname=p0->getBuffer();  
  if(strchr(dname, '/')) {
    xsink->raiseException("DIR-RMDIR-PARAMETER-ERROR", "only direct subdirectories are allowed");
    return 0;
  }
  
  std::string path=std::string(d->dirname())+"/"+std::string(dname);
  if(rmdir(path.c_str())) {
    xsink->raiseException("DIR-RMDIR-ERROR", "error on removing subdirectory '%s' in '%s': %s", p0->getBuffer(), d->dirname(), strerror(errno));
    return 0;
  }

  return 0;
}


// ---------------------------------


// list()
// lists all files and directories, but ignores '.' and '..'
static AbstractQoreNode *DIR_list(QoreObject *self, class Dir *d, const QoreListNode *params, ExceptionSink *xsink) {
  return d->list(-1, xsink);
}


// listFiles()
// lists all files
static AbstractQoreNode *DIR_listfiles(QoreObject *self, class Dir *d, const QoreListNode *params, ExceptionSink *xsink) {
  return d->list(-1^DT_DIR, xsink);
}


// listDirs()
// lists all directoreis but ignore '.' and '..'
static AbstractQoreNode *DIR_listdirs(QoreObject *self, class Dir *d, const QoreListNode *params, ExceptionSink *xsink) {
  return d->list(DT_DIR, xsink);
}


// ---------------------------------


// openFile(filename, [flags, mode, charset])
// throw exception from File::open2()
static AbstractQoreNode *DIR_openfile(QoreObject *self, class Dir *d, const QoreListNode *params, ExceptionSink *xsink) {

  const QoreStringNode *p0;
  p0 = test_string_param(params, 0);
  if(!p0) {
    xsink->raiseException("DIR-OPENFILE-PARAMETER-ERROR", "expecting string filename as first argument of Dir::openFile()");
    return 0;
  }

  // check if there is a path delimiter in
  const char *p0_str=p0->getBuffer();  
  if(strchr(p0_str, '/')) {
    xsink->raiseException("DIR-OPENFILE-PARAMETER-ERROR", "only filenames without path are allowed");
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
  std::string fname=std::string(d->dirname())+"/"+std::string(p0_str);
  int r=f->open2(xsink, fname.c_str(), flags, mode, charset);
  if(r!=0) {
    f.release(); // release the object
    return 0;
  }

  // create the qoreObject and set the File object as private data of the class tagged with the CID_FILE class ID
  //QoreObject *o=new QoreObject(QC_File, getProgram()); // does not work. QC_File not defined
  QoreObject *o=new QoreObject(getRootNS()->rootFindClass("File"), getProgram());
  o->setPrivate(CID_FILE, f.release());

  return o;
}


// unlink(filename): remove the file
static AbstractQoreNode *DIR_unlink(QoreObject *self, class Dir *d, const QoreListNode *params, ExceptionSink *xsink) {
  const QoreStringNode *p0;
  if(!(p0 = test_string_param(params, 0))) {
    return 0;
  }

  // check if there is a path delimiter in
  const char *p0_str=p0->getBuffer();  
  if(strchr(p0_str, '/')) {
    xsink->raiseException("DIR-REMOVEFILE-PARAMETER-ERROR", "only filenames without path are allowed");
    return 0;
  }

  std::string fname=std::string(d->dirname())+"/"+std::string(p0_str);
  errno=0; // clear errno flag
  if(unlink(fname.c_str()) && errno!=ENOENT) {
    xsink->raiseException("DIR-REMOVEFILE-ERROR", "error on removing file '%s': %s", p0->getBuffer(), strerror(errno));
    return 0;
  }
  // if an errno was set it must be ENOENT at this point.
  // so we return that no file is removed
  return get_bool_node(errno? 0: 1);
}


// ---------------------------------


// init the class
class QoreClass *initDirClass() {
   tracein("initDirClass()");

   class QoreClass *QC_DIR = new QoreClass("Dir", QDOM_FILESYSTEM);
   CID_DIR = QC_DIR->getID();

   //QC_DIR->setSystemConstructor(DIR_system_constructor);
   QC_DIR->setConstructor(DIR_constructor);
   QC_DIR->setCopy((q_copy_t)DIR_copy);

   QC_DIR->addMethod("chdir",		(q_method_t)DIR_chdir);
   QC_DIR->addMethod("path",		(q_method_t)DIR_getdirname);
   QC_DIR->addMethod("exists",		(q_method_t)DIR_exists);

   QC_DIR->addMethod("create",		(q_method_t)DIR_create);
   QC_DIR->addMethod("chown",		(q_method_t)DIR_chown);
   QC_DIR->addMethod("chgrp",		(q_method_t)DIR_chgrp);
   QC_DIR->addMethod("chmod",		(q_method_t)DIR_chmod);

   QC_DIR->addMethod("mkdir",		(q_method_t)DIR_mkdir);
   QC_DIR->addMethod("rmdir",		(q_method_t)DIR_rmdir);

   QC_DIR->addMethod("list",		(q_method_t)DIR_list);
   QC_DIR->addMethod("listFiles",	(q_method_t)DIR_listfiles);
   QC_DIR->addMethod("listDirs",	(q_method_t)DIR_listdirs);

   QC_DIR->addMethod("openFile",	(q_method_t)DIR_openfile);
   QC_DIR->addMethod("removeFile",	(q_method_t)DIR_unlink);

   traceout("initDirClass()");
   return QC_DIR;
}
