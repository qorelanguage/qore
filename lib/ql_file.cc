/*
 ql_file.cc
 
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
#include <qore/intern/ql_file.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

static class QoreNode *check_stat(unsigned code, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNode *p0 = test_string_param(params, 0);

   if (!p0)
      return NULL;

   struct stat sbuf;
   int rc;
   
   if ((rc = stat(p0->getBuffer(), &sbuf)))
      return NULL;

   return (sbuf.st_mode & S_IFMT) == code ? boolean_true() : boolean_false();
}

static class QoreNode *check_lstat(unsigned code, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNode *p0;
   if (!(p0 = test_string_param(params, 0)))
      return NULL;
   
   struct stat sbuf;
   int rc;
   
   if ((rc = lstat(p0->getBuffer(), &sbuf)))
      return NULL;
   
   return (sbuf.st_mode & S_IFMT) == code ? boolean_true() : boolean_false();
}

static class QoreNode *f_is_file(const QoreListNode *params, ExceptionSink *xsink)
{
   return check_stat(S_IFREG, params, xsink);
}

static class QoreNode *f_is_dir(const QoreListNode *params, ExceptionSink *xsink)
{
   return check_stat(S_IFDIR, params, xsink);
}

static class QoreNode *f_is_socket(const QoreListNode *params, ExceptionSink *xsink)
{
   return check_stat(S_IFSOCK, params, xsink);
}

static class QoreNode *f_is_pipe(const QoreListNode *params, ExceptionSink *xsink)
{
   return check_stat(S_IFIFO, params, xsink);
}

static class QoreNode *f_is_cdev(const QoreListNode *params, ExceptionSink *xsink)
{
   return check_stat(S_IFCHR, params, xsink);
}

static class QoreNode *f_is_bdev(const QoreListNode *params, ExceptionSink *xsink)
{
   return check_stat(S_IFBLK, params, xsink);
}

static class QoreNode *f_is_dev(const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNode *p0;
   if (!(p0 = test_string_param(params, 0)))
      return NULL;
   
   struct stat sbuf;
   int rc;
   
   if ((rc = lstat(p0->getBuffer(), &sbuf)))
      return NULL;
   
   return ((sbuf.st_mode & S_IFMT) == S_IFCHR)
	   || ((sbuf.st_mode & S_IFMT) == S_IFBLK)
	  ? boolean_true() : boolean_false();
}

static class QoreNode *f_is_link(const QoreListNode *params, ExceptionSink *xsink)
{
   return check_lstat(S_IFLNK, params, xsink);
}

static class QoreNode *f_is_readable(const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNode *p0;
   if (!(p0 = test_string_param(params, 0)))
      return NULL;
   
   struct stat sbuf;
   int rc;
   
   if ((rc = stat(p0->getBuffer(), &sbuf)))
      return NULL;
   
   if (sbuf.st_mode & S_IROTH 
       || (geteuid() == sbuf.st_uid && (sbuf.st_mode & S_IRUSR)) 
       || (getegid() == sbuf.st_gid && (sbuf.st_mode & S_IRGRP)))
      return boolean_true();
   
   return boolean_false();
}

static class QoreNode *f_is_writeable(const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNode *p0;
   if (!(p0 = test_string_param(params, 0)))
      return NULL;
   
   tracein("f_stat()");
   struct stat sbuf;
   int rc;
   
   if ((rc = stat(p0->getBuffer(), &sbuf)))
      return NULL;
   
   if (sbuf.st_mode & S_IWOTH 
       || (geteuid() == sbuf.st_uid && (sbuf.st_mode & S_IWUSR)) 
       || (getegid() == sbuf.st_gid && (sbuf.st_mode & S_IWGRP)))
      return boolean_true();
   
   return boolean_false();
}

static class QoreNode *f_is_executable(const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNode *p0;
   if (!(p0 = test_string_param(params, 0)))
      return NULL;
   
   struct stat sbuf;
   int rc;
   
   if ((rc = stat(p0->getBuffer(), &sbuf)))
      return NULL;
   
   if (sbuf.st_mode & S_IXOTH 
       || (geteuid() == sbuf.st_uid && (sbuf.st_mode & S_IXUSR)) 
       || (getegid() == sbuf.st_gid && (sbuf.st_mode & S_IXGRP)))
      return boolean_true();
   
   return boolean_false();
}

void init_file_functions()
{
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
   builtinFunctions.add("is_writeable", f_is_writeable, QDOM_FILESYSTEM);
   builtinFunctions.add("is_executable", f_is_executable, QDOM_FILESYSTEM);
}
