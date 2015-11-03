/*
 ql_file.cc
 
 Qore Programming Language
 
 Copyright 2003 - 2009 David Nichols
 
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

static AbstractQoreNode *check_stat(unsigned code, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = test_string_param(params, 0);

   if (!p0)
      return 0;

   struct stat sbuf;
   int rc;
   
   if ((rc = stat(p0->getBuffer(), &sbuf)))
      return 0;

   return (sbuf.st_mode & S_IFMT) == code ? boolean_true() : boolean_false();
}

static AbstractQoreNode *check_lstat(unsigned code, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0;
   if (!(p0 = test_string_param(params, 0)))
      return 0;
   
   struct stat sbuf;
   int rc;
   
   if ((rc = lstat(p0->getBuffer(), &sbuf)))
      return 0;
   
   return (sbuf.st_mode & S_IFMT) == code ? boolean_true() : boolean_false();
}

static AbstractQoreNode *f_is_file(const QoreListNode *params, ExceptionSink *xsink) {
   return check_stat(S_IFREG, params, xsink);
}

static AbstractQoreNode *f_is_dir(const QoreListNode *params, ExceptionSink *xsink) {
   return check_stat(S_IFDIR, params, xsink);
}

static AbstractQoreNode *f_is_socket(const QoreListNode *params, ExceptionSink *xsink) {
   return check_stat(S_IFSOCK, params, xsink);
}

static AbstractQoreNode *f_is_pipe(const QoreListNode *params, ExceptionSink *xsink) {
   return check_stat(S_IFIFO, params, xsink);
}

static AbstractQoreNode *f_is_cdev(const QoreListNode *params, ExceptionSink *xsink) {
   return check_stat(S_IFCHR, params, xsink);
}

static AbstractQoreNode *f_is_bdev(const QoreListNode *params, ExceptionSink *xsink) {
   return check_stat(S_IFBLK, params, xsink);
}

static AbstractQoreNode *f_is_dev(const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0;
   if (!(p0 = test_string_param(params, 0)))
      return 0;
   
   struct stat sbuf;
   int rc;
   
   if ((rc = lstat(p0->getBuffer(), &sbuf)))
      return 0;
   
   return ((sbuf.st_mode & S_IFMT) == S_IFCHR)
	   || ((sbuf.st_mode & S_IFMT) == S_IFBLK)
	  ? boolean_true() : boolean_false();
}

static AbstractQoreNode *f_is_link(const QoreListNode *params, ExceptionSink *xsink) {
   return check_lstat(S_IFLNK, params, xsink);
}

static AbstractQoreNode *f_is_readable(const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0;
   if (!(p0 = test_string_param(params, 0)))
      return 0;
   
   struct stat sbuf;
   int rc;
   
   if ((rc = stat(p0->getBuffer(), &sbuf)))
      return 0;
   
   uid_t euid = geteuid();
   if (!euid || sbuf.st_mode & S_IROTH 
       || (euid      == sbuf.st_uid && (sbuf.st_mode & S_IRUSR)) 
       || (getegid() == sbuf.st_gid && (sbuf.st_mode & S_IRGRP)))
      return boolean_true();
   
   return boolean_false();
}

static AbstractQoreNode *f_is_writable(const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0;
   if (!(p0 = test_string_param(params, 0)))
      return 0;
   
   QORE_TRACE("f_stat()");
   struct stat sbuf;
   int rc;
   
   if ((rc = stat(p0->getBuffer(), &sbuf)))
      return 0;
   
   uid_t euid = geteuid();
   if (!euid || sbuf.st_mode & S_IWOTH 
       || (euid      == sbuf.st_uid && (sbuf.st_mode & S_IWUSR)) 
       || (getegid() == sbuf.st_gid && (sbuf.st_mode & S_IWGRP)))
      return boolean_true();
   
   return boolean_false();
}

static AbstractQoreNode *f_is_executable(const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0;
   if (!(p0 = test_string_param(params, 0)))
      return 0;
   
   struct stat sbuf;
   int rc;
   
   if ((rc = stat(p0->getBuffer(), &sbuf)))
      return 0;
   
   if (sbuf.st_mode & S_IXOTH 
       || (geteuid() == sbuf.st_uid && (sbuf.st_mode & S_IXUSR)) 
       || (getegid() == sbuf.st_gid && (sbuf.st_mode & S_IXGRP)))
      return boolean_true();
   
   return boolean_false();
}

static AbstractQoreNode *f_rename(const QoreListNode *params, ExceptionSink *xsink) {
   // old file name
   const QoreStringNode *p0 = test_string_param(params, 0);
   if (p0 || !p0->strlen()) {
      xsink->raiseException("RENAME-ERROR", "missing path to current file name as first argument");
      return 0;
   }

   // new file name
   const QoreStringNode *p1 = test_string_param(params, 1);
   if (p1 || !p1->strlen()) {
      xsink->raiseException("RENAME-ERROR", "missing new file path as second argument");
      return 0;
   }

   int rc = rename(p0->getBuffer(), p1->getBuffer());
   if (rc)
      xsink->raiseException("RENAME-ERROR", strerror(errno));

   return 0;
}

void init_file_functions() {
   // register builtin functions in this file
   builtinFunctions.add("is_file", f_is_file, QDOM_FILESYSTEM);
   builtinFunctions.add("is_dir", f_is_dir, QDOM_FILESYSTEM);
   builtinFunctions.add("is_socket", f_is_socket, QDOM_FILESYSTEM);
   builtinFunctions.add("is_pipe", f_is_pipe, QDOM_FILESYSTEM);
   builtinFunctions.add("is_dev", f_is_dev, QDOM_FILESYSTEM);
   builtinFunctions.add("is_cdev", f_is_cdev, QDOM_FILESYSTEM);
   builtinFunctions.add("is_bdev", f_is_bdev, QDOM_FILESYSTEM);
   builtinFunctions.add("is_link", f_is_link, QDOM_FILESYSTEM);
   builtinFunctions.add("is_readable", f_is_readable, QDOM_FILESYSTEM);
   builtinFunctions.add("is_writable", f_is_writable, QDOM_FILESYSTEM);
   // backwards-compatible misspelling of "writable" :-)
   builtinFunctions.add("is_writeable", f_is_writable, QDOM_FILESYSTEM);
   builtinFunctions.add("is_executable", f_is_executable, QDOM_FILESYSTEM);

   builtinFunctions.add("rename", f_rename, QDOM_FILESYSTEM);

}
