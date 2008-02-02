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
#include <qore/intern/ql_lib.h>
#include <qore/intern/ExecArgList.h>
#include <qore/intern/QoreSignal.h>
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

#ifdef DEBUG_TESTS
// Unsorted unit tests are put here
#  include "tests/ReferenceHolder_tests.cc"
#endif

static class AbstractQoreNode *f_exit(const QoreListNode *params, ExceptionSink *xsink)
{
   class AbstractQoreNode *p0;

   if (!(p0 = get_param(params, 0)))
      exit(0);

   exit(p0->getAsInt());
   return NULL;
}

static class AbstractQoreNode *f_abort(const QoreListNode *params, ExceptionSink *xsink)
{
   abort();
   return NULL;
}

static class AbstractQoreNode *f_exec(const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNode *p0;

   if (!(p0 = test_string_param(params, 0)))
      return NULL;

   class ExecArgList args(p0->getBuffer());
   execvp(args.getFile(), args.getArgs());
   
   xsink->raiseException("EXEC-ERROR", "execvp() failed with error code %d", errno);
   return NULL;
}

// executes a command and returns exit status
static class AbstractQoreNode *f_system(const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNode *p0;

   if (!(p0 = test_string_param(params, 0)))
      return NULL;

   int rc;
   // use system() if shell meta-characters are found
   if (strchrs(p0->getBuffer(), "*?><;"))
   {
      QoreString c;
      c.sprintf("/bin/sh -c \"%s\"", p0->getBuffer());
      rc = system(c.getBuffer());
   }
   else // otherwise fork and exec
   {
      pid_t pid;
      if (!(pid = fork()))
      {
	 class ExecArgList args(p0->getBuffer());
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
   return new QoreBigIntNode(rc);
}

static class AbstractQoreNode *f_getuid(const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(getuid());
}

static class AbstractQoreNode *f_geteuid(const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(getuid());
}

static class AbstractQoreNode *f_getgid(const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(getgid());
}

static class AbstractQoreNode *f_getegid(const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(getgid());
}

static class AbstractQoreNode *f_sleep(const QoreListNode *params, ExceptionSink *xsink)
{
   int timeout = getSecZeroInt(get_param(params, 0));
   if (!timeout)
      return NULL;
   
   return new QoreBigIntNode(sleep(timeout));
}

static class AbstractQoreNode *f_usleep(const QoreListNode *params, ExceptionSink *xsink)
{
   int timeout = getMicroSecZeroInt(get_param(params, 0));
   if (!timeout)
      return NULL;

   return new QoreBigIntNode(usleep(timeout));
}

static class AbstractQoreNode *f_getpid(const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(getpid());
}

static class AbstractQoreNode *f_getppid(const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(getppid());   
}

extern int num_threads;
static class AbstractQoreNode *f_fork(const QoreListNode *params, ExceptionSink *xsink)
{
   int sh = (QoreSignalManager::thread_running ? 1 : 0);
   if (num_threads > (1 + sh))
   {   
      xsink->raiseException("ILLEGAL-FORK", "cannot fork() when other threads are running");
      return NULL;
   }

   // we may not fork from within a signal handler
   if (sh && gettid() == QoreSignalManager::gettid())
   {
      xsink->raiseException("ILLEGAL-FORK", "cannot fork() within a signal handler");
      return NULL;
   }
   
   // stop signal handling thread and make sure it can't be restarted until fork is done
   QoreSignalManager::pre_fork_block_and_stop();
   //printd(5, "stopped signal thread, about to fork pid %d\n", getpid()); fflush(stdout);
   int pid = fork();
   class AbstractQoreNode *rv = new QoreBigIntNode(pid);
   // release signal handler lock
   QoreSignalManager::post_fork_unblock_and_start(!pid, xsink);
   return rv;
}

static class AbstractQoreNode *f_kill(const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p0, *p1;
   if (!(p0 = get_param(params, 0)))
      return NULL;

   int sig, pid;

   pid = p0->getAsInt();
   if ((p1 = get_param(params, 1)))
      sig = p1->getAsInt();
   else sig = SIGHUP;
   return new QoreBigIntNode(kill(pid, sig));
}

/*
static class AbstractQoreNode *f_wait(const QoreListNode *params, ExceptionSink *xsink)
{
}

static class AbstractQoreNode *f_waitpid(const QoreListNode *params, ExceptionSink *xsink)
{
}
*/

static QoreListNode *map_sbuf_to_list(struct stat *sbuf)
{
   QoreListNode *l = new QoreListNode();

   // note that dev_t on Linux is an unsigned 64-bit integer, so we could lose precision here
   l->push(new QoreBigIntNode((int64)sbuf->st_dev));
   l->push(new QoreBigIntNode(sbuf->st_ino));
   l->push(new QoreBigIntNode(sbuf->st_mode));
   l->push(new QoreBigIntNode(sbuf->st_nlink));
   l->push(new QoreBigIntNode(sbuf->st_uid));
   l->push(new QoreBigIntNode(sbuf->st_gid));
   // note that dev_t on Linux is an unsigned 64-bit integer, so we could lose precision here
   l->push(new QoreBigIntNode((int64)sbuf->st_rdev));
   l->push(new QoreBigIntNode(sbuf->st_size));
   
   struct tm tms;
   l->push(new DateTimeNode(q_localtime(&sbuf->st_atime, &tms)));
   l->push(new DateTimeNode(q_localtime(&sbuf->st_mtime, &tms)));
   l->push(new DateTimeNode(q_localtime(&sbuf->st_ctime, &tms)));

   l->push(new QoreBigIntNode(sbuf->st_blksize));
   l->push(new QoreBigIntNode(sbuf->st_blocks));

   return l;
}

static class QoreHashNode *map_sbuf_to_hash(struct stat *sbuf)
{
   QoreHashNode *h = new QoreHashNode();

   // note that dev_t on Linux is an unsigned 64-bit integer, so we could lose precision here
   h->setKeyValue("dev",     new QoreBigIntNode((int64)sbuf->st_dev), NULL);
   h->setKeyValue("inode",   new QoreBigIntNode(sbuf->st_ino), NULL);
   h->setKeyValue("mode",    new QoreBigIntNode(sbuf->st_mode), NULL);
   h->setKeyValue("nlink",   new QoreBigIntNode(sbuf->st_nlink), NULL);
   h->setKeyValue("uid",     new QoreBigIntNode(sbuf->st_uid), NULL);
   h->setKeyValue("gid",     new QoreBigIntNode(sbuf->st_gid), NULL);
   // note that dev_t on Linux is an unsigned 64-bit integer, so we could lose precision here
   h->setKeyValue("rdev",    new QoreBigIntNode((int64)sbuf->st_rdev), NULL);
   h->setKeyValue("size",    new QoreBigIntNode(sbuf->st_size), NULL);
   
   struct tm tms;
   h->setKeyValue("atime",   new DateTimeNode(q_localtime(&sbuf->st_atime, &tms)), NULL);
   h->setKeyValue("mtime",   new DateTimeNode(q_localtime(&sbuf->st_mtime, &tms)), NULL);
   h->setKeyValue("ctime",   new DateTimeNode(q_localtime(&sbuf->st_ctime, &tms)), NULL);

   h->setKeyValue("blksize", new QoreBigIntNode(sbuf->st_blksize), NULL);
   h->setKeyValue("blocks",  new QoreBigIntNode(sbuf->st_blocks), NULL);

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

   h->setKeyValue("type",  new QoreStringNode(type), NULL);
   return h;
}

static class AbstractQoreNode *f_stat(const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNode *p0;
   if (!(p0 = test_string_param(params, 0)))
      return NULL;

   tracein("f_stat()");
   struct stat sbuf;
   int rc;

   if ((rc = stat(p0->getBuffer(), &sbuf)))
      return NULL;

   traceout("f_stat()");
   return map_sbuf_to_list(&sbuf);
}

static class AbstractQoreNode *f_lstat(const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNode *p0;
   if (!(p0 = test_string_param(params, 0)))
      return NULL;

   tracein("f_lstat()");
   struct stat sbuf;
   int rc;

   if ((rc = lstat(p0->getBuffer(), &sbuf)))
      return NULL;

   traceout("f_lstat()");
   return map_sbuf_to_list(&sbuf);
}

static class AbstractQoreNode *f_hstat(const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNode *p0;
   if (!(p0 = test_string_param(params, 0)))
      return NULL;

   tracein("f_hstat()");
   struct stat sbuf;
   int rc;

   if ((rc = stat(p0->getBuffer(), &sbuf)))
      return NULL;

   traceout("f_hstat()");
   return map_sbuf_to_hash(&sbuf);
}

static class AbstractQoreNode *f_hlstat(const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNode *p0;
   if (!(p0 = test_string_param(params, 0)))
      return NULL;

   tracein("f_hlstat()");
   struct stat sbuf;
   int rc;

   if ((rc = lstat(p0->getBuffer(), &sbuf)))
      return NULL;

   traceout("f_hstat()");
   return map_sbuf_to_hash(&sbuf);
}

static class AbstractQoreNode *f_glob(const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNode *p0;
   if (!(p0 = test_string_param(params, 0)))
      return NULL;

   glob_t globbuf;
   if (glob(p0->getBuffer(), 0, NULL, &globbuf))
   {
      globfree(&globbuf);
      return NULL;
   }

   QoreListNode *l = new QoreListNode();
   for (int i = 0; i < (int)globbuf.gl_pathc; i++)
      l->push(new QoreStringNode(globbuf.gl_pathv[i]));
   
   globfree(&globbuf);
   return l;
}

static class AbstractQoreNode *f_unlink(const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNode *p0;
   if (!(p0 = test_string_param(params, 0)))
      return NULL;

   return new QoreBigIntNode(unlink(p0->getBuffer()));
}

static class AbstractQoreNode *f_umask(const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p0;
   if (!(p0 = get_param(params, 0)))
      return NULL;
   
   return new QoreBigIntNode(umask(p0->getAsInt()));
}

static class AbstractQoreNode *f_rand(const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(random());
}

static class AbstractQoreNode *f_srand(const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p0;
   if (!(p0 = get_param(params, 0)))
      return NULL;
   
   srandom(p0->getAsInt());
   return NULL;
}

static class AbstractQoreNode *f_gethostname(const QoreListNode *params, ExceptionSink *xsink)
{
   char buf[HOSTNAMEBUFSIZE + 1];

   if (gethostname(buf, HOSTNAMEBUFSIZE))
      return NULL;
   return new QoreStringNode(buf);
}

static class AbstractQoreNode *f_errno(const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(errno);
}


static class AbstractQoreNode *f_strerror(const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p0 = get_param(params, 0);
   if (!p0)
      return NULL;
#ifdef NEED_STRERROR_R
#define STRERR_BUFSIZE 512
   char buf[STRERR_BUFSIZE];
   if (strerror_r(p0->getAsInt(), buf, STRERR_BUFSIZE))
      return NULL;
   return new QoreStringNode(buf);
#else
   return new QoreStringNode(strerror(p0->getAsInt()));
#endif
}

static class AbstractQoreNode *f_basename(const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNode *p0 = test_string_param(params, 0);
   if (!p0)
      return NULL;

   char *p = q_basename(p0->getBuffer());
   int len = strlen(p);
   return new QoreStringNode(p, len, len + 1, p0->getEncoding());
}

static class AbstractQoreNode *f_mkdir(const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNode *p0 = test_string_param(params, 0);
   if (!p0)
   {
      xsink->raiseException("MKDIR-PARAMETER-ERROR", "expecting string as first parameter of mkdir");
      return NULL;
   }

   int mode;
   AbstractQoreNode *p1 = get_param(params, 1);
   if (p1)
      mode = p1->getAsInt();
   else
      mode = 0777;

   return new QoreBigIntNode(mkdir(p0->getBuffer(), mode));
}

static class AbstractQoreNode *f_rmdir(const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNode *p0 = test_string_param(params, 0);
   if (!p0)
   {
      xsink->raiseException("RMDIR-PARAMETER-ERROR", "expecting string as first parameter of rmdir");
      return NULL;
   }

   return new QoreBigIntNode(rmdir(p0->getBuffer()));
}

// usage: chmod(path, mode)
static class AbstractQoreNode *f_chmod(const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNode *p0 = test_string_param(params, 0);
   if (!p0)
   {
      xsink->raiseException("CHMOD-PARAMETER-ERROR", "expecting string as first parameter of chmod");
      return NULL;
   }

   AbstractQoreNode *p1 = get_param(params, 1);
   if (!p1)
   {
      xsink->raiseException("CHMOD-PARAMETER-ERROR", "expecting mode as second parameter of chmod");
      return NULL;
   }

   return new QoreBigIntNode(chmod(p0->getBuffer(), p1->getAsInt()));
}

static class AbstractQoreNode *f_chdir(const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNode *p0 = test_string_param(params, 0);
   if (!p0)
   {
      xsink->raiseException("CHDIR-PARAMETER-ERROR", "expecting string as first parameter of chdir");
      return NULL;
   }

   return new QoreBigIntNode(chdir(p0->getBuffer()));
}

static class AbstractQoreNode *f_getcwd(const QoreListNode *params, ExceptionSink *xsink)
{
   int bs = 512;
   char *buf = (char *)malloc(sizeof(char) * bs);
 
   do {
      bs += 512;
      buf = (char *)realloc(buf, sizeof(char) * bs);
   } while (getcwd(buf, bs));

   return new QoreStringNode(buf, strlen(buf), bs, QCS_DEFAULT);
}

/*
// need an easier to use function here
// usage: mknod(path, mode)
static class AbstractQoreNode *f_mknod(const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNode *p0 = test_string_param(params, 0);
   if (!p0)
   {
      xsink->raiseException("MKNOD-PARAMETER-ERROR", "expecting string as first parameter of mknod");
      return NULL;
   }

   AbstractQoreNode *p1 = get_param(params, 1);
   if (is_nothing(p1))
   {
      xsink->raiseException("MKNOD-PARAMETER-ERROR", "expecting mode as second parameter of mknod");
      return NULL;
   }

   AbstractQoreNode *p2 = get_param(params, 2);
   if (is_nothing(p2))
   {
      xsink->raiseException("MKNOD-PARAMETER-ERROR", "expecting device as second parameter of mknod");
      return NULL;
   }

   return new QoreBigIntNode(mknod(p0->getBuffer(), p1->getAsInt(), p2->getAsInt()));
}
*/

static class AbstractQoreNode *f_mkfifo(const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNode *p0;
   if (!(p0 = test_string_param(params, 0)))
      return NULL;

   const char *fn = p0->getBuffer();
   int mode;

   AbstractQoreNode *p1;
   p1 = get_param(params, 1);
   mode = p1 ? p1->getAsInt() : 0600;

   return new QoreBigIntNode(mkfifo(fn, mode));
}

static class AbstractQoreNode *f_setuid(const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p0 = get_param(params, 0);

   if (is_nothing(p0))
   {
      xsink->raiseException("SETUID-ERROR", "missing user ID");
      return NULL;
   }

   return new QoreBigIntNode(setuid(p0->getAsInt()));
}

#ifdef HAVE_SETEUID
static class AbstractQoreNode *f_seteuid(const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p0 = get_param(params, 0);
   
   if (is_nothing(p0))
   {
      xsink->raiseException("SETEUID-ERROR", "missing user ID");
      return NULL;
   }
   
   return new QoreBigIntNode(seteuid(p0->getAsInt()));
}
#endif

static class AbstractQoreNode *f_setgid(const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p0 = get_param(params, 0);
   
   if (is_nothing(p0))
   {
      xsink->raiseException("SETGID-ERROR", "missing group ID");
      return NULL;
   }
   
   return new QoreBigIntNode(setgid(p0->getAsInt()));
}

#ifdef HAVE_SETEGID
static class AbstractQoreNode *f_setegid(const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p0 = get_param(params, 0);
   
   if (is_nothing(p0))
   {
      xsink->raiseException("SETEGID-ERROR", "missing group ID");
      return NULL;
   }
   
   return new QoreBigIntNode(setegid(p0->getAsInt()));
}
#endif

static class AbstractQoreNode *f_gethostbyname(const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNode *p = test_string_param(params, 0);
   if (!p)
      return NULL;

   return q_gethostbyname_to_string(p->getBuffer());
}

static class AbstractQoreNode *f_gethostbyaddr(const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNode *p0 = test_string_param(params, 0);
   if (!p0)
      return NULL;

   AbstractQoreNode *p1 = get_param(params, 1);
   int type = p1 ? p1->getAsInt() : 0;
   if (!type) type = AF_INET;

   return q_gethostbyaddr_to_string(xsink, p0->getBuffer(), type);
}

static class AbstractQoreNode *f_gethostbyname_long(const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNode *p = test_string_param(params, 0);
   if (!p)
      return NULL;

   return q_gethostbyname_to_hash(p->getBuffer());
}

static class AbstractQoreNode *f_gethostbyaddr_long(const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNode *p0 = test_string_param(params, 0);
   if (!p0)
      return NULL;

   AbstractQoreNode *p1 = get_param(params, 1);
   int type = p1 ? p1->getAsInt() : 0;
   if (!type) type = AF_INET;

   return q_gethostbyaddr_to_hash(xsink, p0->getBuffer(), type);
}

#ifdef DEBUG
static AbstractQoreNode* runQoreTests(const QoreListNode *params, ExceptionSink *xsink)
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

static AbstractQoreNode* runRecentQoreTests(const QoreListNode *params, ExceptionSink *xsink)
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
   builtinFunctions.add("kill",        f_kill, QDOM_EXTERNAL_PROCESS);
   // builtinFunctions.add("wait",        f_wait);
   // builtinFunctions.add("waitpid",     f_waitpid);
   builtinFunctions.add("stat",        f_stat, QDOM_FILESYSTEM);
   builtinFunctions.add("lstat",       f_lstat, QDOM_FILESYSTEM);
   builtinFunctions.add("glob",        f_glob, QDOM_FILESYSTEM);
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
   builtinFunctions.add("mkfifo",      f_mkfifo, QDOM_FILESYSTEM);
   builtinFunctions.add("hstat",       f_hstat, QDOM_FILESYSTEM);
   builtinFunctions.add("hlstat",      f_hlstat, QDOM_FILESYSTEM);
   builtinFunctions.add("exec",        f_exec, QDOM_EXTERNAL_PROCESS | QDOM_PROCESS);
   builtinFunctions.add("setuid",      f_setuid);
   builtinFunctions.add("setgid",      f_setgid);
#ifdef HAVE_SETEUID
   builtinFunctions.add("seteuid",     f_seteuid);
#endif
#ifdef HAVE_SETEGID
   builtinFunctions.add("setegid",     f_setegid);
#endif
   builtinFunctions.add("gethostbyname",       f_gethostbyname);
   builtinFunctions.add("gethostbyaddr",       f_gethostbyaddr);
   builtinFunctions.add("gethostbyname_long",  f_gethostbyname_long);
   builtinFunctions.add("gethostbyaddr_long",  f_gethostbyaddr_long);

   builtinFunctions.add("getcwd",      f_getcwd);

#ifdef DEBUG
   builtinFunctions.add("runQoreTests", runQoreTests);
   builtinFunctions.add("runRecentQoreTests", runRecentQoreTests);
#endif
}
