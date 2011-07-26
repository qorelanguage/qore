/*
 ql_file.cpp
 
 Qore Programming Language
 
 Copyright 2003 - 2011 David Nichols
 
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
#include <qore/intern/ql_file.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

static AbstractQoreNode *check_stat(unsigned code, const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p0, const QoreStringNode, args, 0);
   struct stat sbuf;
   int rc;
   
   if ((rc = stat(p0->getBuffer(), &sbuf)))
      return boolean_false();

   return (sbuf.st_mode & S_IFMT) == code ? boolean_true() : boolean_false();
}

#ifdef HAVE_LSTAT
static AbstractQoreNode *check_lstat(unsigned code, const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p0, const QoreStringNode, args, 0);   
   struct stat sbuf;
   int rc;
   
   if ((rc = lstat(p0->getBuffer(), &sbuf)))
      return boolean_false();
   
   return (sbuf.st_mode & S_IFMT) == code ? boolean_true() : boolean_false();
}
#endif

static AbstractQoreNode *f_is_file(const QoreListNode *args, ExceptionSink *xsink) {
   return check_stat(S_IFREG, args, xsink);
}

static AbstractQoreNode *f_is_dir(const QoreListNode *args, ExceptionSink *xsink) {
   return check_stat(S_IFDIR, args, xsink);
}

static AbstractQoreNode *f_is_socket(const QoreListNode *args, ExceptionSink *xsink) {
#ifdef S_IFSOCK
   return check_stat(S_IFSOCK, args, xsink);
#else
   return missing_function_error("is_socket", xsink);
#endif
}

static AbstractQoreNode *f_is_pipe(const QoreListNode *args, ExceptionSink *xsink) {
   return check_stat(S_IFIFO, args, xsink);
}

static AbstractQoreNode *f_is_cdev(const QoreListNode *args, ExceptionSink *xsink) {
   return check_stat(S_IFCHR, args, xsink);
}

static AbstractQoreNode *f_is_bdev(const QoreListNode *args, ExceptionSink *xsink) {
   return check_stat(S_IFBLK, args, xsink);
}

static AbstractQoreNode *f_is_dev(const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p0, const QoreStringNode, args, 0);   
   
   struct stat sbuf;
   int rc;
   
#ifdef HAVE_LSTAT
   if ((rc = lstat(p0->getBuffer(), &sbuf)))
#else
   if ((rc = stat(p0->getBuffer(), &sbuf)))
#endif
      return boolean_false();
   
   return ((sbuf.st_mode & S_IFMT) == S_IFCHR)
	   || ((sbuf.st_mode & S_IFMT) == S_IFBLK)
	  ? boolean_true() : boolean_false();
}

static AbstractQoreNode *f_is_link(const QoreListNode *args, ExceptionSink *xsink) {
#ifdef HAVE_LSTAT
   return check_lstat(S_IFLNK, args, xsink);
#else
   return missing_function_error("is_link", xsink);
#endif
}

static AbstractQoreNode *f_is_readable(const QoreListNode *args, ExceptionSink *xsink) {
#ifdef HAVE_PWD_H
   HARD_QORE_PARAM(p0, const QoreStringNode, args, 0);   
   
   struct stat sbuf;
   int rc;
   
   if ((rc = stat(p0->getBuffer(), &sbuf)))
      return boolean_false();
   
   uid_t euid = geteuid();
   if (!euid || sbuf.st_mode & S_IROTH 
       || (euid      == sbuf.st_uid && (sbuf.st_mode & S_IRUSR)) 
       || (getegid() == sbuf.st_gid && (sbuf.st_mode & S_IRGRP)))
      return boolean_true();
   
   return boolean_false();
#else
   return missing_function_error("is_readable", xsink);
#endif
}

static AbstractQoreNode *f_is_writable(const QoreListNode *args, ExceptionSink *xsink) {
#ifdef HAVE_PWD_H
   HARD_QORE_PARAM(p0, const QoreStringNode, args, 0);   
   
   QORE_TRACE("f_stat()");
   struct stat sbuf;
   int rc;
   
   if ((rc = stat(p0->getBuffer(), &sbuf)))
      return boolean_false();
   
   uid_t euid = geteuid();
   if (!euid || sbuf.st_mode & S_IWOTH 
       || (euid      == sbuf.st_uid && (sbuf.st_mode & S_IWUSR)) 
       || (getegid() == sbuf.st_gid && (sbuf.st_mode & S_IWGRP)))
      return boolean_true();
   
   return boolean_false();
#else
   return missing_function_error("is_writable", xsink);
#endif
}

static AbstractQoreNode *f_is_executable(const QoreListNode *args, ExceptionSink *xsink) {
#ifdef HAVE_PWD_H
   HARD_QORE_PARAM(p0, const QoreStringNode, args, 0);   
   
   struct stat sbuf;
   int rc;
   
   if ((rc = stat(p0->getBuffer(), &sbuf)))
      return 0;
   
   if (sbuf.st_mode & S_IXOTH 
       || (geteuid() == sbuf.st_uid && (sbuf.st_mode & S_IXUSR)) 
       || (getegid() == sbuf.st_gid && (sbuf.st_mode & S_IXGRP)))
      return boolean_true();
   
   return boolean_false();
#else
   return missing_function_error("is_executable", xsink);
#endif
}

static AbstractQoreNode *f_rename(const QoreListNode *args, ExceptionSink *xsink) {
   // old file name
   HARD_QORE_PARAM(p0, const QoreStringNode, args, 0);   
   HARD_QORE_PARAM(p1, const QoreStringNode, args, 1);   

   if (!p0->strlen()) {
      xsink->raiseException("RENAME-ERROR", "empty path to current file name given as first argument");
      return 0;
   }

   // new file name
   if (!p1->strlen()) {
      xsink->raiseException("RENAME-ERROR", "empty new file path given as second argument");
      return 0;
   }

   int rc = rename(p0->getBuffer(), p1->getBuffer());
   if (rc)
      xsink->raiseException("RENAME-ERROR", q_strerror(errno));

   return 0;
}

void init_file_functions() {
   // register builtin functions in this file
   builtinFunctions.add2("is_file", f_is_file, QC_CONSTANT, QDOM_FILESYSTEM, boolTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("is_dir", f_is_dir, QC_CONSTANT, QDOM_FILESYSTEM, boolTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("is_socket", f_is_socket, QC_CONSTANT, QDOM_FILESYSTEM, boolTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("is_pipe", f_is_pipe, QC_CONSTANT, QDOM_FILESYSTEM, boolTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("is_dev", f_is_dev, QC_CONSTANT, QDOM_FILESYSTEM, boolTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("is_cdev", f_is_cdev, QC_CONSTANT, QDOM_FILESYSTEM, boolTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("is_bdev", f_is_bdev, QC_CONSTANT, QDOM_FILESYSTEM, boolTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("is_link", f_is_link, QC_CONSTANT, QDOM_FILESYSTEM, boolTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("is_readable", f_is_readable, QC_CONSTANT, QDOM_FILESYSTEM, boolTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("is_writable", f_is_writable, QC_CONSTANT, QDOM_FILESYSTEM, boolTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   // backwards-compatible misspelling of "writable" :-)
   builtinFunctions.add2("is_writeable", f_is_writable, QC_CONSTANT, QDOM_FILESYSTEM, boolTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("is_executable", f_is_executable, QC_CONSTANT, QDOM_FILESYSTEM, boolTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("rename", f_rename, QC_NO_FLAGS, QDOM_FILESYSTEM, nothingTypeInfo, 2, stringTypeInfo, QORE_PARAM_NO_ARG, stringTypeInfo, QORE_PARAM_NO_ARG);
}
