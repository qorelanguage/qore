/*
  ql_lib.cpp

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
#include <qore/intern/ql_lib.h>
#include <qore/intern/ExecArgList.h>
#include <qore/intern/QoreSignal.h>
#include <qore/minitest.hpp>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>

#ifdef HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif

#include <time.h>

#ifdef HAVE_GLOB_H
#include <glob.h>
#else
#include <qore/intern/glob.h>
#endif

extern bool threads_initialized;

#ifdef DEBUG_TESTS
// Unsorted unit tests are put here
#  include "tests/ReferenceHolder_tests.cpp"
#endif

AbstractQoreNode *missing_function_error(const char *func, ExceptionSink *xsink) {
   QoreString have(func);
   have.toupr();
   xsink->raiseException("MISSING-FEATURE-ERROR", "this system does not implement %s(); for maximum portability use the constant Option::HAVE_%s to check if this function is implemented before calling", func, have.getBuffer());
   return 0;
}

AbstractQoreNode *missing_function_error(const char *func, const char *opt, ExceptionSink *xsink) {
   xsink->raiseException("MISSING-FEATURE-ERROR", "this system does not implement %s(); for maximum portability use the constant Option::HAVE_%s to check if this function is implemented before calling", func, opt);
   return 0;
}

static AbstractQoreNode *f_exit(const QoreListNode *params, ExceptionSink *xsink) {
   const AbstractQoreNode *p0 = get_param(params, 0);
   qore_exit_process(p0 ? p0->getAsInt() : 0);
   return 0;   // to avoid warning
}

static AbstractQoreNode *f_abort(const QoreListNode *params, ExceptionSink *xsink) {
   threads_initialized = false;
   abort();
   return 0;
}

static AbstractQoreNode *f_exec(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p0, const QoreStringNode, params, 0);
   ExecArgList args(p0->getBuffer());
   execvp(args.getFile(), args.getArgs());
   
   xsink->raiseErrnoException("EXEC-ERROR", errno, "execvp() failed in child process for target '%s'", args.getFile());
   return 0;
}

#ifdef HAVE_SYSTEM
static int do_system(const QoreString &str) {
   QoreString c(str);
   // escape backslashes: replace '\' -> '\\'
   c.replaceAll("\\", "\\\\");
   // escape double quotes: replace '"' -> '\"'
   c.replaceAll("\"", "\\\"");

   c.concat('"');
   c.prepend("/bin/sh -c \"");
   
   return system(c.getBuffer());
}
#endif

// executes a command and returns exit status
static AbstractQoreNode *f_system(const QoreListNode *params, ExceptionSink *xsink) {
#ifdef HAVE_FORK
   HARD_QORE_PARAM(p0, const QoreStringNode, params, 0);

   int rc;
   // use system() if shell meta-characters are found
   if (strchrs(p0->getBuffer(), "$=*?><;|\"\\")) {
      rc = do_system(*p0);
   }
   else { // otherwise fork and exec
      pid_t pid;
      if (!(pid = fork())) {
	 ExecArgList args(p0->getBuffer());
	 execvp(args.getFile(), args.getArgs());
	 fprintf(stderr, "execvp() failed in child process for target '%s' with error code %d: %s\n", args.getFile(), errno, strerror(errno));
	 exit(-1);
	 //qore_exit_process(-1);
      }
      if (pid == -1)
	 rc = -1;
      else {
	 int status;
	 wait(&status);
	 if (WIFEXITED(status))
	    rc = WEXITSTATUS(status);
	 else
	    rc = -1;
      }
   }
   return new QoreBigIntNode(rc);
#else
#ifdef HAVE_SYSTEM
   HARD_QORE_PARAM(p0, const QoreStringNode, params, 0);
   return new QoreBigIntNode(do_system(*p0));
#else
   return missing_function_error("system", xsink);
#endif // HAVE_SYSTEM
#endif // HAVE_FORK
}

static AbstractQoreNode *f_getuid(const QoreListNode *params, ExceptionSink *xsink) {
#ifdef HAVE_GETUID
   return new QoreBigIntNode(getuid());
#else
   return missing_function_error("getuid", "UNIX_USERMGT", xsink);
#endif
}

static AbstractQoreNode *f_geteuid(const QoreListNode *params, ExceptionSink *xsink) {
#ifdef HAVE_GETEUID
   return new QoreBigIntNode(getuid());
#else
   return missing_function_error("geteuid", "UNIX_USERMGT", xsink);
#endif
}

static AbstractQoreNode *f_getgid(const QoreListNode *params, ExceptionSink *xsink) {
#ifdef HAVE_GETGID
   return new QoreBigIntNode(getgid());
#else
   return missing_function_error("getgid", "UNIX_USERMGT", xsink);
#endif
}

static AbstractQoreNode *f_getegid(const QoreListNode *params, ExceptionSink *xsink) {
#ifdef HAVE_GETEGID
   return new QoreBigIntNode(getgid());
#else
   return missing_function_error("getegid", "UNIX_USERMGT", xsink);
#endif
}

static AbstractQoreNode *f_sleep(const QoreListNode *params, ExceptionSink *xsink) {
   int timeout = getSecZeroInt(get_param(params, 0));
   if (!timeout)
      return 0;

   int64 us = (int64)timeout * 1000000ll;
   us = qore_usleep(us);
   return 0;
}

static AbstractQoreNode *f_usleep(const QoreListNode *params, ExceptionSink *xsink) {
   int64 timeout = getMicroSecZeroInt64(get_param(params, 0));
   if (!timeout)
      return 0;

   return new QoreBigIntNode(qore_usleep(timeout));
}

static AbstractQoreNode *f_getpid(const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreBigIntNode(getpid());
}

static AbstractQoreNode *f_getppid(const QoreListNode *params, ExceptionSink *xsink) {
#ifdef HAVE_GETPPID
   return new QoreBigIntNode(getppid());   
#else
   return missing_function_error("getppid", xsink);
#endif
}

extern int num_threads;
static AbstractQoreNode *f_fork(const QoreListNode *params, ExceptionSink *xsink) {
#ifdef HAVE_FORK
   int sh = (QSM.thread_running ? 1 : 0);
   if (num_threads > (1 + sh)) {   
      xsink->raiseException("ILLEGAL-FORK", "cannot fork() when other threads are running");
      return 0;
   }

#ifdef HAVE_SIGNAL_HANDLING
   // we may not fork from within a signal handler
   if (sh && gettid() == QSM.gettid()) {
      xsink->raiseException("ILLEGAL-FORK", "cannot fork() within a signal handler");
      return 0;
   }
   
   // stop signal handling thread and make sure it can't be restarted until fork is done
   QSM.pre_fork_block_and_stop();
#endif

   //printd(5, "stopped signal thread, about to fork pid %d\n", getpid()); fflush(stdout);
   int pid = fork();

#ifdef HAVE_SIGNAL_HANDLING
   // release signal handler lock
   QSM.post_fork_unblock_and_start(!pid, xsink);
#endif

   return new QoreBigIntNode(pid);
#else
   return missing_function_error("fork", xsink);
#endif
}

static AbstractQoreNode *f_kill(const QoreListNode *params, ExceptionSink *xsink) {
#ifdef HAVE_KILL
   int pid = (int)HARD_QORE_INT(params, 0);
   int sig = (int)HARD_QORE_INT(params, 1);
   return new QoreBigIntNode(kill(pid, sig));
#else
   return missing_function_error("kill", xsink);
#endif
}

/*
static AbstractQoreNode *f_wait(const QoreListNode *params, ExceptionSink *xsink)
{
}

static AbstractQoreNode *f_waitpid(const QoreListNode *params, ExceptionSink *xsink)
{
}
*/

static AbstractQoreNode *f_statvfs(const QoreListNode *params, ExceptionSink *xsink) {
#ifdef HAVE_SYS_STATVFS_H
   HARD_QORE_PARAM(p0, const QoreStringNode, params, 0);

   struct statvfs vfs;
   if (statvfs(p0->getBuffer(), &vfs))
      return 0;
   
   return statvfs_to_hash(vfs);
#else
   return missing_function_error("statvfs", xsink);
#endif
}

static AbstractQoreNode *f_stat(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p0, const QoreStringNode, params, 0);

   struct stat sbuf;
   if (stat(p0->getBuffer(), &sbuf))
      return 0;

   return stat_to_list(sbuf);
}

static AbstractQoreNode *f_lstat(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p0, const QoreStringNode, params, 0);

   struct stat sbuf;
#ifdef HAVE_LSTAT
   if (lstat(p0->getBuffer(), &sbuf))
#else
   if (stat(p0->getBuffer(), &sbuf))
#endif
      return 0;

   return stat_to_list(sbuf);
}

// *hash hstat(string $path)  
static AbstractQoreNode *f_hstat(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p0, const QoreStringNode, params, 0);

   struct stat sbuf;
   if (stat(p0->getBuffer(), &sbuf))
      return 0;

   return stat_to_hash(sbuf);
}

static AbstractQoreNode *f_hlstat(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p0, const QoreStringNode, params, 0);

   struct stat sbuf;
#ifdef HAVE_LSTAT
   if (lstat(p0->getBuffer(), &sbuf))
#else
   if (stat(p0->getBuffer(), &sbuf))
#endif
      return 0;

   return stat_to_hash(sbuf);
}

static AbstractQoreNode *f_glob(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p0, const QoreStringNode, params, 0);

   glob_t globbuf;
   if (glob(p0->getBuffer(), 0, 0, &globbuf)) {
      globfree(&globbuf);
      return 0;
   }

   QoreListNode *l = new QoreListNode;
   for (unsigned i = 0; i < globbuf.gl_pathc; ++i)
      l->push(new QoreStringNode(globbuf.gl_pathv[i]));
   
   globfree(&globbuf);
   return l;
}

static AbstractQoreNode *f_unlink(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p0, const QoreStringNode, params, 0);

   return new QoreBigIntNode(unlink(p0->getBuffer()));
}

static AbstractQoreNode *f_umask(const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreBigIntNode(umask((int)HARD_QORE_INT(params, 0)));
}

static AbstractQoreNode *f_rand(const QoreListNode *params, ExceptionSink *xsink) {
#ifdef HAVE_RANDOM
   // return a random 64-bit integer by calling random() twice
   return new QoreBigIntNode(random() | (((int64)random()) << 32));
#else
   return new QoreBigIntNode(rand() | (((int64)rand()) << 32));
#endif
}

static AbstractQoreNode *f_srand(const QoreListNode *params, ExceptionSink *xsink) {
#ifdef HAVE_RANDOM
   srandom((int)HARD_QORE_INT(params, 0));
#else
   srand((int)HARD_QORE_INT(params, 0));
#endif
   return 0;
}

static AbstractQoreNode *f_gethostname(const QoreListNode *params, ExceptionSink *xsink) {
   char buf[HOSTNAMEBUFSIZE + 1];

   if (gethostname(buf, HOSTNAMEBUFSIZE)) {
      xsink->raiseErrnoException("GETHOSTNAME-ERROR", errno, "gethostname() failed");
      return 0;
   }
   return new QoreStringNode(buf);
}

static AbstractQoreNode *f_errno(const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreBigIntNode(errno);
}

static AbstractQoreNode *f_strerror(const QoreListNode *params, ExceptionSink *xsink) {
   int err = (int)HARD_QORE_INT(params, 0);

   return q_strerror(err);
}

static AbstractQoreNode *f_basename(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p0, const QoreStringNode, params, 0);
   char *p = q_basename(p0->getBuffer());
   int len = strlen(p);
   return new QoreStringNode(p, len, len + 1, p0->getEncoding());
}

static AbstractQoreNode *f_dirname(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p0, const QoreStringNode, params, 0);
   char *p = q_dirname(p0->getBuffer());
   int len = strlen(p);
   return new QoreStringNode(p, len, len + 1, p0->getEncoding());
}

static AbstractQoreNode *f_mkdir(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p0, const QoreStringNode, params, 0);
   int mode = (int)HARD_QORE_INT(params, 1);
   return new QoreBigIntNode(mkdir(p0->getBuffer(), mode));
}

static AbstractQoreNode *f_rmdir(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p0, const QoreStringNode, params, 0);
   return new QoreBigIntNode(rmdir(p0->getBuffer()));
}

// usage: chmod(path, mode)
static AbstractQoreNode *f_chmod(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p0, const QoreStringNode, params, 0);
   return new QoreBigIntNode(chmod(p0->getBuffer(), (int)HARD_QORE_INT(params, 1)));
}

static AbstractQoreNode *f_chdir(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p0, const QoreStringNode, params, 0);
   return new QoreBigIntNode(chdir(p0->getBuffer()));
}

static AbstractQoreNode *f_getcwd_intern(ExceptionSink *xsink = 0) {
   int bs = 512;
   char *buf = (char *)malloc(sizeof(char) * bs);
 
   while (true) {
      char *b = getcwd(buf, bs);
      if (!b) {
	  if (errno == ERANGE) {
	      bs *= 2;
	      buf = (char *)q_realloc(buf, sizeof(char) * bs);
	      if (!buf) {
		  xsink->outOfMemory();
		  return 0;
	      }	  
	      continue;
	  }
	  if (xsink)
	     xsink->raiseErrnoException("GETCWD2-ERROR", errno, "getcwd() failed");
	  return 0;
      }
      break;
   }

   return new QoreStringNode(buf, strlen(buf), bs, QCS_DEFAULT);
}

static AbstractQoreNode *f_getcwd(const QoreListNode *params, ExceptionSink *xsink) {
   return f_getcwd_intern();
}

static AbstractQoreNode *f_getcwd2(const QoreListNode *params, ExceptionSink *xsink) {
   return f_getcwd_intern(xsink);
}

/*
// need an easier to use function here
// usage: mknod(path, mode)
static AbstractQoreNode *f_mknod(const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = test_string_param(params, 0);
   if (!p0) {
      xsink->raiseException("MKNOD-PARAMETER-ERROR", "expecting string as first parameter of mknod");
      return 0;
   }

   const AbstractQoreNode *p1 = get_param(params, 1);
   if (is_nothing(p1)) {
      xsink->raiseException("MKNOD-PARAMETER-ERROR", "expecting mode as second parameter of mknod");
      return 0;
   }

   const AbstractQoreNode *p2 = get_param(params, 2);
   if (is_nothing(p2)) {
      xsink->raiseException("MKNOD-PARAMETER-ERROR", "expecting device as second parameter of mknod");
      return 0;
   }

   return new QoreBigIntNode(mknod(p0->getBuffer(), p1->getAsInt(), p2->getAsInt()));
}
*/

static AbstractQoreNode *f_mkfifo(const QoreListNode *params, ExceptionSink *xsink) {
#ifdef HAVE_MKFIFO
   HARD_QORE_PARAM(p0, const QoreStringNode, params, 0);
   const char *fn = p0->getBuffer();

   int mode = (int)HARD_QORE_INT(params, 1);
   return new QoreBigIntNode(mkfifo(fn, mode));
#else
   return missing_function_error("mkfifo", "UNIX_FILEMGT", xsink);
#endif
}

static AbstractQoreNode *f_setuid(const QoreListNode *params, ExceptionSink *xsink) {
#ifdef HAVE_SETUID
   return new QoreBigIntNode(setuid((int)HARD_QORE_INT(params, 0)));
#else
   return missing_function_error("setuid", "UNIX_USERMGT", xsink);
#endif
}

static AbstractQoreNode *f_seteuid(const QoreListNode *params, ExceptionSink *xsink) {
#ifdef HAVE_SETEUID
   return new QoreBigIntNode(seteuid((int)HARD_QORE_INT(params, 0)));
#else
   return missing_function_error("seteuid", xsink);
#endif
}

static AbstractQoreNode *f_setgid(const QoreListNode *params, ExceptionSink *xsink) {
#ifdef HAVE_SETGID
   return new QoreBigIntNode(setgid((int)HARD_QORE_INT(params, 0)));
#else
   return missing_function_error("setgid", "UNIX_USERMGT", xsink);
#endif
}

static AbstractQoreNode *f_setegid(const QoreListNode *params, ExceptionSink *xsink) {
#ifdef HAVE_SETEGID
   return new QoreBigIntNode(setegid((int)HARD_QORE_INT(params, 0)));
#else
   return missing_function_error("setegid", xsink);
#endif
}

static AbstractQoreNode *f_setsid(const QoreListNode *params, ExceptionSink *xsink) {
#ifdef HAVE_SETSID
    return new QoreBigIntNode(setsid());
#else
   return missing_function_error("setsid", xsink);
#endif
}

static AbstractQoreNode *f_gethostbyname(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p, const QoreStringNode, params, 0);
   return q_gethostbyname_to_string(p->getBuffer());
}

static AbstractQoreNode *f_gethostbyaddr(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p0, const QoreStringNode, params, 0);
   int type = (int)HARD_QORE_INT(params, 1);
   return q_gethostbyaddr_to_string(xsink, p0->getBuffer(), type);
}

static AbstractQoreNode *f_gethostbyname_long(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p, const QoreStringNode, params, 0);
   return q_gethostbyname_to_hash(p->getBuffer());
}

static AbstractQoreNode *f_gethostbyaddr_long(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p0, const QoreStringNode, params, 0);
   int type = (int)HARD_QORE_INT(params, 1);
   return q_gethostbyaddr_to_hash(xsink, p0->getBuffer(), type);
}

// list getaddrinfo(*string $node, *softstring $service, softint $family = AF_UNSPEC, softint $flags = 0)
static AbstractQoreNode *f_getaddrinfo(const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *node = test_string_param(params, 0);
   const QoreStringNode *service = test_string_param(params, 1);

   if ((!node || !node->strlen())
       && (!service || !service->strlen())) {
      xsink->raiseException("SOCKET-BIND-ERROR", "both node (first parameter) and service (second parameter) were either not present or empty strings; at least one of the first 2 parameters must be present for a successful call to getaddrinfo()");
      return 0;
   }

   int family = (int)HARD_QORE_INT(params, 2);
   int flags = (int)HARD_QORE_INT(params, 3);
   return q_getaddrinfo_to_list(xsink, node ? node->getBuffer() : 0, service ? service->getBuffer() : 0, family, flags);
}

//int chown (const char *path, uid_t owner, gid_t group);
static AbstractQoreNode *f_chown(const QoreListNode *params, ExceptionSink *xsink) {
#ifdef HAVE_CHOWN
   HARD_QORE_PARAM(p0, const QoreStringNode, params, 0);
   const char *path = p0->getBuffer();
   uid_t owner = (uid_t)HARD_QORE_INT(params, 1);
   gid_t group = (gid_t)HARD_QORE_INT(params, 2);
   return new QoreBigIntNode(chown(path, owner, group));
#else
   return missing_function_error("chown", "UNIX_FILEMGT", xsink);
#endif
}

//int lchown (const char *path, uid_t owner, gid_t group);
static AbstractQoreNode *f_lchown(const QoreListNode *params, ExceptionSink *xsink) {
#ifdef HAVE_LCHOWN
   HARD_QORE_PARAM(p0, const QoreStringNode, params, 0);
   const char *path = p0->getBuffer();
   uid_t owner = (uid_t)HARD_QORE_INT(params, 1);
   gid_t group = (gid_t)HARD_QORE_INT(params, 2);
   return new QoreBigIntNode(lchown(path, owner, group));
#else
   return missing_function_error("lchown", "UNIX_FILEMGT", xsink);
#endif
}

static AbstractQoreNode *f_readlink(const QoreListNode *params, ExceptionSink *xsink) {
#ifdef HAVE_READLINK
   HARD_QORE_PARAM(p0, const QoreStringNode, params, 0);
   
   char buf[QORE_PATH_MAX + 1];
   qore_offset_t len = readlink(p0->getBuffer(), buf, QORE_PATH_MAX);
   if (len < 0) {
      xsink->raiseErrnoException("READLINK-ERROR", errno, p0->getBuffer());
      return 0;
   }
   assert(len <= QORE_PATH_MAX);
   buf[len] = '\0';
   return new QoreStringNode(buf);
#else
   return missing_function_error("readlink", "UNIX_FILEMGT", xsink);
#endif
}

#ifdef DEBUG
static AbstractQoreNode *runQoreTests(const QoreListNode *params, ExceptionSink *xsink) {
   minitest::result res = minitest::execute_all_tests();
   if (res.all_tests_succeeded) {
      printf("Qore runtime: %d tests succeeded\n", res.sucessful_tests_count);
      return 0;
   }
   
   xsink->raiseException("A Qore test failed", "Qore test in file %s, line %d threw an exception.",
			 res.failed_test_file, res.failed_test_line);
   return 0;
}

static AbstractQoreNode *runRecentQoreTests(const QoreListNode *params, ExceptionSink *xsink) {
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
TEST() {
  // just an example of empty test
}
}
#endif

void init_lib_functions() {
   builtinFunctions.add2("exit",        f_exit, QC_NO_FLAGS, QDOM_PROCESS, nothingTypeInfo, 1, anyTypeInfo, zero());
   builtinFunctions.add2("abort",       f_abort, QC_NO_FLAGS, QDOM_PROCESS, nothingTypeInfo);

   builtinFunctions.add2("system",      f_noop, QC_RUNTIME_NOOP, QDOM_EXTERNAL_PROCESS, nothingTypeInfo);
   builtinFunctions.add2("system",      f_system, QC_NO_FLAGS, QDOM_EXTERNAL_PROCESS, bigIntTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("getuid",      f_getuid, QC_CONSTANT, QDOM_EXTERNAL_INFO, bigIntTypeInfo);
   builtinFunctions.add2("geteuid",     f_geteuid, QC_CONSTANT, QDOM_EXTERNAL_INFO, bigIntTypeInfo);
   builtinFunctions.add2("getgid",      f_getgid, QC_CONSTANT, QDOM_EXTERNAL_INFO, bigIntTypeInfo);
   builtinFunctions.add2("getegid",     f_getegid, QC_CONSTANT, QDOM_EXTERNAL_INFO, bigIntTypeInfo);

   builtinFunctions.add2("sleep",       f_noop, QC_RUNTIME_NOOP, QDOM_PROCESS, nothingTypeInfo);
   builtinFunctions.add2("sleep",       f_sleep, QC_NO_FLAGS, QDOM_PROCESS, bigIntTypeInfo, 1, softBigIntTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("sleep",       f_sleep, QC_NO_FLAGS, QDOM_PROCESS, bigIntTypeInfo, 1, dateTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("usleep",      f_noop, QC_RUNTIME_NOOP, QDOM_PROCESS, nothingTypeInfo);
   builtinFunctions.add2("usleep",      f_usleep, QC_NO_FLAGS, QDOM_PROCESS, bigIntTypeInfo, 1, softBigIntTypeInfo, QORE_PARAM_NO_ARG);
   builtinFunctions.add2("usleep",      f_usleep, QC_NO_FLAGS, QDOM_PROCESS, bigIntTypeInfo, 1, dateTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("getpid",      f_getpid, QC_CONSTANT, QDOM_EXTERNAL_INFO, bigIntTypeInfo);
   builtinFunctions.add2("getppid",     f_getppid, QC_CONSTANT, QDOM_EXTERNAL_INFO, bigIntTypeInfo);

   builtinFunctions.add2("fork",        f_fork, QC_NO_FLAGS, QDOM_PROCESS, bigIntTypeInfo);

   builtinFunctions.add2("kill",        f_noop, QC_RUNTIME_NOOP, QDOM_EXTERNAL_PROCESS, nothingTypeInfo);
#ifndef SIGHUP
#define SIGHUP -1
#endif
   builtinFunctions.add2("kill",        f_kill, QC_NO_FLAGS, QDOM_EXTERNAL_PROCESS, bigIntTypeInfo, 2, softBigIntTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, new QoreBigIntNode(SIGHUP));

   // builtinFunctions.add("wait",        f_wait);
   // builtinFunctions.add("waitpid",     f_waitpid);

   // *hash statvfs(string $path)  
   builtinFunctions.add2("statvfs",     f_statvfs, QC_CONSTANT, QDOM_FILESYSTEM, hashOrNothingTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("stat",        f_noop, QC_RUNTIME_NOOP, QDOM_FILESYSTEM, nothingTypeInfo);
   // *list stat(string $path)  
   builtinFunctions.add2("stat",        f_stat, QC_CONSTANT, QDOM_FILESYSTEM, listOrNothingTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("lstat",       f_noop, QC_RUNTIME_NOOP, QDOM_FILESYSTEM, nothingTypeInfo);
   // *list lstat(string $path)  
   builtinFunctions.add2("lstat",       f_lstat, QC_CONSTANT, QDOM_FILESYSTEM, listOrNothingTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("hstat",       f_noop, QC_RUNTIME_NOOP, QDOM_FILESYSTEM, nothingTypeInfo);
   // *hash hstat(string $path)  
   builtinFunctions.add2("hstat",       f_hstat, QC_CONSTANT, QDOM_FILESYSTEM, hashOrNothingTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("hlstat",      f_noop, QC_RUNTIME_NOOP, QDOM_FILESYSTEM, nothingTypeInfo);
   // *hash hlstat(string $path)  
   builtinFunctions.add2("hlstat",      f_hlstat, QC_CONSTANT, QDOM_FILESYSTEM, hashOrNothingTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   // *list glob(string $str)  
   builtinFunctions.add2("glob",        f_noop, QC_RUNTIME_NOOP, QDOM_FILESYSTEM, nothingTypeInfo);
   builtinFunctions.add2("glob",        f_glob, QC_NO_FLAGS, QDOM_FILESYSTEM, listOrNothingTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("unlink",      f_noop, QC_RUNTIME_NOOP, QDOM_FILESYSTEM, nothingTypeInfo);
   builtinFunctions.add2("unlink",      f_unlink, QC_NO_FLAGS, QDOM_FILESYSTEM, bigIntTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("umask",       f_noop, QC_RUNTIME_NOOP, QDOM_PROCESS, nothingTypeInfo);
   builtinFunctions.add2("umask",       f_umask, QC_NO_FLAGS, QDOM_PROCESS, bigIntTypeInfo, 1, softBigIntTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("rand",        f_rand, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo);

   builtinFunctions.add2("srand",       f_noop, QC_RUNTIME_NOOP, QDOM_PROCESS, nothingTypeInfo);
   builtinFunctions.add2("srand",       f_srand, QC_NO_FLAGS, QDOM_PROCESS, nothingTypeInfo, 1, softBigIntTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("gethostname", f_gethostname, QC_NO_FLAGS, QDOM_EXTERNAL_INFO, stringTypeInfo);

   builtinFunctions.add2("errno",       f_errno, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo);

   builtinFunctions.add2("strerror",    f_noop, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("strerror",    f_strerror, QC_CONSTANT, QDOM_DEFAULT, stringTypeInfo, 1, softBigIntTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("basename",    f_noop, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("basename",    f_basename, QC_CONSTANT, QDOM_DEFAULT, stringTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("dirname",     f_noop, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   builtinFunctions.add2("dirname",     f_dirname, QC_CONSTANT, QDOM_DEFAULT, stringTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("mkdir",       f_mkdir, QC_NO_FLAGS, QDOM_FILESYSTEM, bigIntTypeInfo, 2, stringTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, new QoreBigIntNode(0777));

   builtinFunctions.add2("rmdir",       f_rmdir, QC_NO_FLAGS, QDOM_FILESYSTEM, bigIntTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("chmod",       f_chmod, QC_NO_FLAGS, QDOM_FILESYSTEM, bigIntTypeInfo, 2, stringTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("chdir",       f_chdir, QC_NO_FLAGS, QDOM_FILESYSTEM, bigIntTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   // builtinFunctions.add("mknod",       f_mknod, QDOM_FILESYSTEM);

   builtinFunctions.add2("mkfifo",      f_mkfifo, QC_NO_FLAGS, QDOM_FILESYSTEM, bigIntTypeInfo, 2, stringTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, new QoreBigIntNode(0600));

   builtinFunctions.add2("exec",        f_noop, QC_RUNTIME_NOOP, QDOM_EXTERNAL_PROCESS | QDOM_PROCESS, nothingTypeInfo);
   builtinFunctions.add2("exec",        f_exec, QC_NO_FLAGS, QDOM_EXTERNAL_PROCESS | QDOM_PROCESS, nothingTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   builtinFunctions.add2("setuid",      f_setuid, QC_NO_FLAGS, QDOM_PROCESS, bigIntTypeInfo, 1, softBigIntTypeInfo);

   builtinFunctions.add2("setgid",      f_setgid, QC_NO_FLAGS, QDOM_PROCESS, bigIntTypeInfo, 1, softBigIntTypeInfo);
   builtinFunctions.add2("seteuid",     f_seteuid, QC_NO_FLAGS, QDOM_PROCESS, bigIntTypeInfo, 1, softBigIntTypeInfo);
   builtinFunctions.add2("setegid",     f_setegid, QC_NO_FLAGS, QDOM_PROCESS, bigIntTypeInfo, 1, softBigIntTypeInfo);

   builtinFunctions.add2("setsid",      f_setsid, QC_NO_FLAGS, QDOM_PROCESS, bigIntTypeInfo);

   // *string gethostbyname(string $name)  
   builtinFunctions.add2("gethostbyname",       f_noop, QC_RUNTIME_NOOP, QDOM_EXTERNAL_INFO, nothingTypeInfo);
   builtinFunctions.add2("gethostbyname",       f_gethostbyname, QC_CONSTANT, QDOM_EXTERNAL_INFO, stringOrNothingTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   // *string gethostbyaddr(string $addr, softint $type = AF_INET)  
   builtinFunctions.add2("gethostbyaddr",       f_noop, QC_RUNTIME_NOOP, QDOM_EXTERNAL_INFO, nothingTypeInfo);
   builtinFunctions.add2("gethostbyaddr",       f_gethostbyaddr, QC_CONSTANT, QDOM_EXTERNAL_INFO, stringOrNothingTypeInfo, 2, stringTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, new QoreBigIntNode(AF_INET));

   // *hash gethostbyname_long(string $name)  
   builtinFunctions.add2("gethostbyname_long",  f_noop, QC_RUNTIME_NOOP, QDOM_EXTERNAL_INFO, nothingTypeInfo);
   builtinFunctions.add2("gethostbyname_long",  f_gethostbyname_long, QC_CONSTANT, QDOM_EXTERNAL_INFO, hashOrNothingTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   // *hash gethostbyaddr_long(string $addr, softint $type = AF_INET)  
   builtinFunctions.add2("gethostbyaddr_long",  f_noop, QC_RUNTIME_NOOP, QDOM_EXTERNAL_INFO, nothingTypeInfo);
   builtinFunctions.add2("gethostbyaddr_long",  f_gethostbyaddr_long, QC_CONSTANT, QDOM_EXTERNAL_INFO, hashOrNothingTypeInfo, 2, stringTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, new QoreBigIntNode(AF_INET));

   // list getaddrinfo(*string $node, *softstring $service, softint $family = AF_UNSPEC, softint $flags = 0)
   builtinFunctions.add2("getaddrinfo",         f_getaddrinfo, QC_RET_VALUE_ONLY, QDOM_EXTERNAL_INFO, listTypeInfo, 4, stringOrNothingTypeInfo, QORE_PARAM_NO_ARG, softStringOrNothingTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, new QoreBigIntNode(AF_UNSPEC), softBigIntTypeInfo, zero());

   // getcwd can return NOTHING if an error occurs
   // *string getcwd()  
   builtinFunctions.add2("getcwd",      f_getcwd, QC_CONSTANT, QDOM_FILESYSTEM | QDOM_EXTERNAL_INFO, stringOrNothingTypeInfo);

   // getcwd2 throws an exception if an error occurs
   builtinFunctions.add2("getcwd2",     f_getcwd2, QC_NO_FLAGS, QDOM_FILESYSTEM | QDOM_EXTERNAL_INFO, stringTypeInfo);

   builtinFunctions.add2("chown",       f_chown, QC_NO_FLAGS, QDOM_FILESYSTEM, bigIntTypeInfo, 3, stringTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, new QoreBigIntNode(-1), softBigIntTypeInfo, new QoreBigIntNode(-1));
   builtinFunctions.add2("lchown",      f_lchown, QC_NO_FLAGS, QDOM_FILESYSTEM, bigIntTypeInfo, 3, stringTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, new QoreBigIntNode(-1), softBigIntTypeInfo, new QoreBigIntNode(-1));

   builtinFunctions.add2("readlink",    f_readlink, QC_NO_FLAGS, QDOM_FILESYSTEM, stringTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

#ifdef DEBUG
   builtinFunctions.add2("runQoreTests", runQoreTests);
   builtinFunctions.add2("runRecentQoreTests", runRecentQoreTests);
#endif
}
