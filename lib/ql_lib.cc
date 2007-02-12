/*
  ql_lib.cc

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
#include <qore/ql_lib.h>
#include <qore/ExecArgList.h>
#include <qore/minitest.hpp>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <time.h>
#include <glob.h>

#ifdef DEBUG
// Unsorted unit tests are put here
#  include "tests/ReferenceHolder_tests.cc"
#endif

static class QoreNode *f_exit(class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p0;

   if (!(p0 = get_param(params, 0)))
      exit(0);

   exit(p0->getAsInt());
   return NULL;
}

static class QoreNode *f_abort(class QoreNode *params, ExceptionSink *xsink)
{
   abort();
   return NULL;
}

static class QoreNode *f_exec(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0;

   if (!(p0 = test_param(params, NT_STRING, 0)))
      return NULL;

   class ExecArgList args(p0->val.String->getBuffer());
   execvp(args.getFile(), args.getArgs());
   
   xsink->raiseException("EXEC-ERROR", "execvp() failed with error code %d", errno);
   return NULL;
}

// executes a command and returns exit status
static class QoreNode *f_system(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0;

   if (!(p0 = test_param(params, NT_STRING, 0)))
      return NULL;

   int rc;
   // use system() if shell meta-characters are found
   if (strchrs(p0->val.String->getBuffer(), "*?><"))
   {
      QoreString c;
      c.sprintf("/bin/sh -c \"%s\"", p0->val.String->getBuffer());
      rc = system(c.getBuffer());
   }
   else // otherwise fork and exec
   {
      pid_t pid;
      if (!(pid = fork()))
      {
	 class ExecArgList args(p0->val.String->getBuffer());
	 execvp(args.getFile(), args.getArgs());
	 printf("execvp() failed with error code %d: %s\n", errno, strerror(errno));
	 exit(-1);
      }
      if (pid == -1)
	 rc = -1;
      else
      {
	 int status;
	 wait(&status);
	 if (WIFEXITED(status))
	    rc = WEXITSTATUS(status);
	 else
	    rc = -1;
      }
   }
   return new QoreNode((int64)rc);
}

static class QoreNode *f_getuid(class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)getuid());
}

static class QoreNode *f_geteuid(class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)getuid());
}

static class QoreNode *f_getgid(class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)getgid());
}

static class QoreNode *f_getegid(class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)getgid());
}

static class QoreNode *f_sleep(class QoreNode *params, ExceptionSink *xsink)
{
   int timeout = getSecZeroInt(get_param(params, 0));
   if (!timeout)
      return NULL;
   
   return new QoreNode((int64)sleep(timeout));
}

static class QoreNode *f_usleep(class QoreNode *params, ExceptionSink *xsink)
{
   int timeout = getMicroSecZeroInt(get_param(params, 0));
   if (!timeout)
      return NULL;

   return new QoreNode((int64)usleep(timeout));
}

static class QoreNode *f_getpid(class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)getpid());
}

static class QoreNode *f_getppid(class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)getppid());   
}

extern int num_threads;
static class QoreNode *f_fork(class QoreNode *params, ExceptionSink *xsink)
{
   if (num_threads > 1)
   {   
      xsink->raiseException("ILLEGAL-FORK", "cannot fork() when other threads are running");
      return NULL;
   }
   
   return new QoreNode((int64)fork());   
}

static class QoreNode *f_kill(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0, *p1;
   if (!(p0 = get_param(params, 0)))
      return NULL;

   int sig, pid;

   pid = p0->getAsInt();
   if ((p1 = get_param(params, 1)))
      sig = p1->getAsInt();
   else sig = SIGHUP;
   return new QoreNode((int64)kill(pid, sig));
}

/*
static class QoreNode *f_wait(class QoreNode *params, ExceptionSink *xsink)
{
}

static class QoreNode *f_waitpid(class QoreNode *params, ExceptionSink *xsink)
{
}
*/

static List *map_sbuf_to_list(struct stat *sbuf)
{
   List *l = new List();

   l->push(new QoreNode((int64)sbuf->st_dev));
   l->push(new QoreNode((int64)sbuf->st_ino));
   l->push(new QoreNode((int64)sbuf->st_mode));
   l->push(new QoreNode((int64)sbuf->st_nlink));
   l->push(new QoreNode((int64)sbuf->st_uid));
   l->push(new QoreNode((int64)sbuf->st_gid));
   l->push(new QoreNode((int64)sbuf->st_rdev));
   l->push(new QoreNode((int64)sbuf->st_size));
   
   struct tm tms;
   DateTime *adt = new DateTime(q_localtime(&sbuf->st_atime, &tms));
   l->push(new QoreNode(adt));

   DateTime *mdt = new DateTime(q_localtime(&sbuf->st_mtime, &tms));
   l->push(new QoreNode(mdt));

   DateTime *cdt = new DateTime(q_localtime(&sbuf->st_ctime, &tms));
   l->push(new QoreNode(cdt));

   l->push(new QoreNode((int64)sbuf->st_blksize));
   l->push(new QoreNode((int64)sbuf->st_blocks));

   return l;
}

static class Hash *map_sbuf_to_hash(struct stat *sbuf)
{
   Hash *h = new Hash();

   h->setKeyValue("dev",     new QoreNode((int64)sbuf->st_dev), NULL);
   h->setKeyValue("inode",   new QoreNode((int64)sbuf->st_ino), NULL);
   h->setKeyValue("mode",    new QoreNode((int64)sbuf->st_mode), NULL);
   h->setKeyValue("nlink",   new QoreNode((int64)sbuf->st_nlink), NULL);
   h->setKeyValue("uid",     new QoreNode((int64)sbuf->st_uid), NULL);
   h->setKeyValue("gid",     new QoreNode((int64)sbuf->st_gid), NULL);
   h->setKeyValue("rdev",    new QoreNode((int64)sbuf->st_rdev), NULL);
   h->setKeyValue("size",    new QoreNode((int64)sbuf->st_size), NULL);
   
   struct tm tms;
   DateTime *adt = new DateTime(q_localtime(&sbuf->st_atime, &tms));
   h->setKeyValue("atime",   new QoreNode(adt), NULL);

   DateTime *mdt = new DateTime(q_localtime(&sbuf->st_mtime, &tms));
   h->setKeyValue("mtime",   new QoreNode(mdt), NULL);

   DateTime *cdt = new DateTime(q_localtime(&sbuf->st_ctime, &tms));
   h->setKeyValue("ctime",   new QoreNode(cdt), NULL);

   h->setKeyValue("blksize", new QoreNode((int64)sbuf->st_blksize), NULL);
   h->setKeyValue("blocks",  new QoreNode((int64)sbuf->st_blocks), NULL);

   char *type;
   if (S_ISBLK(sbuf->st_mode))
      type = "BLOCK-DEVICE";
   else if (S_ISDIR(sbuf->st_mode))
      type = "DIRECTORY";
   else if (S_ISCHR(sbuf->st_mode))
      type = "CHARACTER-DEVICE";
   else if (S_ISFIFO(sbuf->st_mode))
      type = "FIFO";
   else if (S_ISLNK(sbuf->st_mode))
      type = "SYMBOLIC-LINK";
   else if (S_ISSOCK(sbuf->st_mode))
      type = "SOCKET";
   else if (S_ISCHR(sbuf->st_mode))
      type = "CHARACTER-DEVICE";
   else if (S_ISREG(sbuf->st_mode))
      type = "REGULAR";
   else
      type = "UNKNOWN";

   h->setKeyValue("type",  new QoreNode(type), NULL);
   return h;
}

static class QoreNode *f_stat(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0;
   if (!(p0 = test_param(params, NT_STRING, 0)))
      return NULL;

   tracein("f_stat()");
   struct stat sbuf;
   int rc;

   if ((rc = stat(p0->val.String->getBuffer(), &sbuf)))
      return NULL;

   traceout("f_stat()");
   return new QoreNode(map_sbuf_to_list(&sbuf));
}

static class QoreNode *f_lstat(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0;
   if (!(p0 = test_param(params, NT_STRING, 0)))
      return NULL;

   tracein("f_lstat()");
   struct stat sbuf;
   int rc;

   if ((rc = lstat(p0->val.String->getBuffer(), &sbuf)))
      return NULL;

   traceout("f_lstat()");
   return new QoreNode(map_sbuf_to_list(&sbuf));
}

static class QoreNode *f_hstat(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0;
   if (!(p0 = test_param(params, NT_STRING, 0)))
      return NULL;

   tracein("f_hstat()");
   struct stat sbuf;
   int rc;

   if ((rc = stat(p0->val.String->getBuffer(), &sbuf)))
      return NULL;

   traceout("f_hstat()");
   return new QoreNode(map_sbuf_to_hash(&sbuf));
}

static class QoreNode *f_hlstat(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0;
   if (!(p0 = test_param(params, NT_STRING, 0)))
      return NULL;

   tracein("f_hlstat()");
   struct stat sbuf;
   int rc;

   if ((rc = lstat(p0->val.String->getBuffer(), &sbuf)))
      return NULL;

   traceout("f_hstat()");
   return new QoreNode(map_sbuf_to_hash(&sbuf));
}

static class QoreNode *f_glob(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0;
   if (!(p0 = test_param(params, NT_STRING, 0)))
      return NULL;

   glob_t globbuf;
   if (glob(p0->val.String->getBuffer(), 0, NULL, &globbuf))
   {
      globfree(&globbuf);
      return NULL;
   }

   List *l = new List();
   for (int i = 0; i < (int)globbuf.gl_pathc; i++)
      l->push(new QoreNode(globbuf.gl_pathv[i]));
   
   globfree(&globbuf);
   return new QoreNode(l);
}

static class QoreNode *f_unlink(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0;
   if (!(p0 = test_param(params, NT_STRING, 0)))
      return NULL;

   return new QoreNode((int64)unlink(p0->val.String->getBuffer()));
}

static class QoreNode *f_umask(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0;
   if (!(p0 = get_param(params, 0)))
      return NULL;
   
   return new QoreNode((int64)umask(p0->getAsInt()));
}

static class QoreNode *f_rand(class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)random());
}

static class QoreNode *f_srand(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0;
   if (!(p0 = get_param(params, 0)))
      return NULL;
   
   srandom(p0->getAsInt());
   return NULL;
}

static class QoreNode *f_gethostname(class QoreNode *params, ExceptionSink *xsink)
{
   char buf[HOSTNAMEBUFSIZE + 1];

   if (gethostname(buf, HOSTNAMEBUFSIZE))
      return NULL;
   return new QoreNode(buf);
}

static class QoreNode *f_errno(class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)errno);
}


static class QoreNode *f_strerror(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = get_param(params, 0);
   if (!p0)
      return NULL;
#ifdef NEED_STRERROR_R
#define STRERR_BUFSIZE 512
   char buf[STRERR_BUFSIZE];
   if (strerror_r(p0->getAsInt(), buf, STRERR_BUFSIZE))
      return NULL;
   return new QoreNode(buf);
#else
   return new QoreNode(strerror(p0->getAsInt()));
#endif
}

static class QoreNode *f_basename(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = test_param(params, NT_STRING, 0);
   if (!p0)
      return NULL;

   class QoreString *str = new QoreString();
   str->take(q_basename(p0->val.String->getBuffer()), p0->val.String->getEncoding());
   return new QoreNode(str);
}

static class QoreNode *f_mkdir(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = test_param(params, NT_STRING, 0);
   if (!p0)
   {
      xsink->raiseException("MKDIR-PARAMETER-ERROR", "expecting string as first parameter of mkdir");
      return NULL;
   }

   int mode;
   QoreNode *p1 = get_param(params, 1);
   if (p1)
      mode = p1->getAsInt();
   else
      mode = 0777;

   return new QoreNode((int64)mkdir(p0->val.String->getBuffer(), mode));
}

static class QoreNode *f_rmdir(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = test_param(params, NT_STRING, 0);
   if (!p0)
   {
      xsink->raiseException("RMDIR-PARAMETER-ERROR", "expecting string as first parameter of rmdir");
      return NULL;
   }

   return new QoreNode((int64)rmdir(p0->val.String->getBuffer()));
}

// usage: chmod(path, mode)
static class QoreNode *f_chmod(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = test_param(params, NT_STRING, 0);
   if (!p0)
   {
      xsink->raiseException("CHMOD-PARAMETER-ERROR", "expecting string as first parameter of chmod");
      return NULL;
   }

   QoreNode *p1 = get_param(params, 1);
   if (!p1)
   {
      xsink->raiseException("CHMOD-PARAMETER-ERROR", "expecting mode as second parameter of chmod");
      return NULL;
   }

   return new QoreNode((int64)chmod(p0->val.String->getBuffer(), p1->getAsInt()));
}

static class QoreNode *f_chdir(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = test_param(params, NT_STRING, 0);
   if (!p0)
   {
      xsink->raiseException("CHDIR-PARAMETER-ERROR", "expecting string as first parameter of chdir");
      return NULL;
   }

   return new QoreNode((int64)chdir(p0->val.String->getBuffer()));
}


/*
// need an easier to use function here
// usage: mknod(path, mode)
static class QoreNode *f_mknod(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = test_param(params, NT_STRING, 0);
   if (!p0)
   {
      xsink->raiseException("MKNOD-PARAMETER-ERROR", "expecting string as first parameter of mknod");
      return NULL;
   }

   QoreNode *p1 = get_param(params, 1);
   if (!p1)
   {
      xsink->raiseException("MKNOD-PARAMETER-ERROR", "expecting mode as second parameter of mknod");
      return NULL;
   }

   QoreNode *p2 = get_param(params, 2);
   if (!p2)
   {
      xsink->raiseException("MKNOD-PARAMETER-ERROR", "expecting device as second parameter of mknod");
      return NULL;
   }

   return new QoreNode((int64)mknod(p0->val.String->getBuffer(), p1->getAsInt(), p2->getAsInt()));
}
*/

static class QoreNode *f_mkfifo(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0, *p1;

   if (!(p0 = test_param(params, NT_STRING, 0)))
      return NULL;

   char *fn = p0->val.String->getBuffer();
   int mode;

   p1 = test_param(params, NT_INT, 1);
   if (p1)
      mode = p1->val.intval;
   else
      mode = 0600;

   return new QoreNode(NT_INT, mkfifo(fn, mode));
}

static class QoreNode *f_setuid(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = get_param(params, 0);

   if (is_nothing(p0))
   {
      xsink->raiseException("SETUID-ERROR", "missing user ID");
      return NULL;
   }

   return new QoreNode((int64)setuid(p0->getAsInt()));
}

#ifdef HAVE_SETEUID
static class QoreNode *f_seteuid(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = get_param(params, 0);
   
   if (is_nothing(p0))
   {
      xsink->raiseException("SETEUID-ERROR", "missing user ID");
      return NULL;
   }
   
   return new QoreNode((int64)seteuid(p0->getAsInt()));
}
#endif

static class QoreNode *f_setgid(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = get_param(params, 0);
   
   if (is_nothing(p0))
   {
      xsink->raiseException("SETGID-ERROR", "missing group ID");
      return NULL;
   }
   
   return new QoreNode((int64)setgid(p0->getAsInt()));
}

#ifdef HAVE_SETEGID
static class QoreNode *f_setegid(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = get_param(params, 0);
   
   if (is_nothing(p0))
   {
      xsink->raiseException("SETEGID-ERROR", "missing group ID");
      return NULL;
   }
   
   return new QoreNode((int64)setegid(p0->getAsInt()));
}
#endif

#ifdef DEBUG
static QoreNode* runQoreTests(QoreNode* params, ExceptionSink* xsink)
{
  minitest::result res = minitest::execute_all_tests();
  if (res.all_tests_succeeded) {
    printf("Qore runtime: %d tests succeeded\n", res.sucessful_tests_count);
    return 0;
  }

  xsink->raiseException("A Qore test failed", "Qore test in file %s, line %d threw an exception.",
    res.failed_test_file, res.failed_test_line);
  return 0;
}

static QoreNode* runRecentQoreTests(QoreNode* params, ExceptionSink* xsink)
{
  minitest::result res = minitest::test_last_changed_files(3); // 3 last modified files
  if (res.all_tests_succeeded) {
    printf("Qore runtime: %d recent tests succeeded\n", res.sucessful_tests_count);
    return 0;
  }

  xsink->raiseException("A Qore test failed", "Qore test in file %s, line %d threw an exception.",
    res.failed_test_file, res.failed_test_line);
  return 0;
}

namespace {
TEST()
{
  // just an example of empty test
}
}
#endif

void init_lib_functions()
{
   builtinFunctions.add("exit",        f_exit, QDOM_PROCESS);
   builtinFunctions.add("abort",       f_abort, QDOM_PROCESS);
   builtinFunctions.add("system",      f_system, QDOM_EXTERNAL_PROCESS);
   builtinFunctions.add("getuid",      f_getuid);
   builtinFunctions.add("geteuid",     f_geteuid);
   builtinFunctions.add("getgid",      f_getgid);
   builtinFunctions.add("getegid",     f_getegid);
   builtinFunctions.add("sleep",       f_sleep, QDOM_PROCESS);
   builtinFunctions.add("usleep",      f_usleep, QDOM_PROCESS);
   builtinFunctions.add("getpid",      f_getpid);
   builtinFunctions.add("getppid",     f_getppid);
   builtinFunctions.add("fork",        f_fork, QDOM_PROCESS);
   builtinFunctions.add("kill",        f_kill, QDOM_EXTERNAL_PROCESS | QDOM_PROCESS);
   // builtinFunctions.add("wait",        f_wait);
   // builtinFunctions.add("waitpid",     f_waitpid);
   builtinFunctions.add("stat",        f_stat);
   builtinFunctions.add("lstat",       f_lstat);
   builtinFunctions.add("glob",        f_glob);
   builtinFunctions.add("unlink",      f_unlink, QDOM_FILESYSTEM);
   builtinFunctions.add("umask",       f_umask, QDOM_FILESYSTEM);
   builtinFunctions.add("rand",        f_rand);
   builtinFunctions.add("srand",       f_srand);
   builtinFunctions.add("gethostname", f_gethostname);
   builtinFunctions.add("errno",       f_errno);
   builtinFunctions.add("strerror",    f_strerror);
   builtinFunctions.add("basename",    f_basename);
   builtinFunctions.add("mkdir",       f_mkdir, QDOM_FILESYSTEM);
   builtinFunctions.add("rmdir",       f_rmdir, QDOM_FILESYSTEM);
   builtinFunctions.add("chmod",       f_chmod, QDOM_FILESYSTEM);
   builtinFunctions.add("chdir",       f_chdir, QDOM_FILESYSTEM);
   // builtinFunctions.add("mknod",       f_mknod, QDOM_FILESYSTEM);
   builtinFunctions.add("mkfifo", f_mkfifo);
   builtinFunctions.add("hstat",       f_hstat);
   builtinFunctions.add("hlstat",      f_hlstat);
   builtinFunctions.add("exec",        f_exec, QDOM_EXTERNAL_PROCESS | QDOM_PROCESS);
   builtinFunctions.add("setuid",      f_setuid);
   builtinFunctions.add("setgid",      f_setgid);
#ifdef HAVE_SETEUID
   builtinFunctions.add("seteuid",     f_seteuid);
#endif
#ifdef HAVE_SETEGID
   builtinFunctions.add("setegid",     f_setegid);
#endif

#ifdef DEBUG
   builtinFunctions.add("runQoreTests", runQoreTests);
   builtinFunctions.add("runRecentQoreTests", runRecentQoreTests);
#endif
}
