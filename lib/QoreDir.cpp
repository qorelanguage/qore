/*
  QoreDir.cpp

  Directory functions

  Qore Programming Language

  Copyright (C) 2003 - 2015 David Nichols

  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.

  Note that the Qore library is released under a choice of three open-source
  licenses: MIT (as above), LGPL 2+, or GPL 2+; see README-LICENSE for more
  information.
*/

#include <qore/Qore.h>
#include <qore/intern/QoreDir.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>

#include <string>

#if (defined _WIN32 || defined __WIN32__) && ! defined __CYGWIN__ 
static int mkdir(const char *path, mode_t mode) {
   return mkdir(path);
}
#endif

class qore_qd_private {
protected:  
   const QoreEncoding *enc;
   std::string dirname;
   mutable QoreThreadLock m;

   // tokenize the strings by the delimiter
   DLLLOCAL static void tokenize(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiter = QORE_DIR_SEP_STR) {
      // accommodate case when the string consists of only the delimiter (ex: "/")
      if (str == delimiter) {
	 tokens.push_back(str);
	 return;
      }

      // Skip delimiters at beginning.
      std::string::size_type lastPos = str.find_first_not_of(delimiter, 0);
      // Find first "non-delimiter".
      std::string::size_type pos     = str.find_first_of(delimiter, lastPos);
	 
      while (std::string::npos != pos || std::string::npos != lastPos) {
	 // Found a token, add it to the vector.
	 tokens.push_back(str.substr(lastPos, pos - lastPos));
	 // Skip delimiters.  Note the "not_of"
	 lastPos = str.find_first_not_of(delimiter, pos);
	 // Find next "non-delimiter"
	 pos = str.find_first_of(delimiter, lastPos);
      }
   }

   // tokenizes the string (path) and 
   DLLLOCAL static const std::string stripPath(const std::string& odir) {
      // tokenize the string
      std::vector<std::string> ptoken, dirs;
      tokenize(odir, ptoken);

      // push them to the new path
      std::vector<std::string>::iterator it;
      for (it = ptoken.begin(); it < ptoken.end(); it++) {
	 std::string d = *it;
	 if (d == "." || d == "") { // ignore
	    continue;
	 }
	    
	 if (d == ".." && !dirs.empty()) { // step back one step
	    dirs.pop_back();
	 }
	 else {
	    dirs.push_back(d);
	 }
      }

      // create string out of rest..
      std::string ret;
      for (it = dirs.begin(); it < dirs.end(); it++) {
	 ret += QORE_DIR_SEP_STR + (*it);
      }

      return ret;
   }

   // check if the given directory is accessable
   // return errno of opendir function
   // unlocked
   DLLLOCAL static int verifyDirectory(const std::string &dir) {
      DIR *dptr = opendir(dir.c_str());
      if (!dptr)
	 return errno;

      // free again
      closedir(dptr);
	 
      return 0;
   }

   // unlocked
   DLLLOCAL std::string getPathIntern(const char *sub) const {
      if (!dirname.empty())
	 return dirname + "/" + std::string(sub);
      return std::string(sub);
   }

   // check if the path in dirname exists
   // return 0 if the path exists
   // return errno of the opendir function
   // unlocked
   DLLLOCAL int checkPathIntern() const {
      return dirname.empty() ? -1 : verifyDirectory(dirname);
   }

public:
   DLLLOCAL qore_qd_private(ExceptionSink *xsink, const QoreEncoding *cs, const char *dir) : enc(cs) {
      if (dir) {
	 dirname = dir;
	 return;
      }

      // set the directory to the cwd
      char *cwd = (char*)malloc(sizeof(char)*QORE_PATH_MAX);
      if (!cwd) { // error in malloc
	 xsink->outOfMemory();
	 return;
      }
      ON_BLOCK_EXIT(free, cwd);

      if (getcwd(cwd, (size_t)QORE_PATH_MAX))
	 dirname = cwd;
   }

   DLLLOCAL qore_qd_private(ExceptionSink *xsink, const qore_qd_private &old) {
      AutoLocker al(old.m);
      enc = old.enc;
      dirname = old.dirname;
   }
      
   DLLLOCAL QoreStringNode *get_dir_string() const {
      AutoLocker al(m);
      return !dirname.empty() ? new QoreStringNode(dirname, enc) : 0;
   }

   DLLLOCAL std::string getPath(const char *sub) const {
      AutoLocker al(m);

      return getPathIntern(sub);
   }
 
   DLLLOCAL int checkPath() const {
      AutoLocker al(m);

      return checkPathIntern();
   }

   DLLLOCAL int chdir(const char *ndir, ExceptionSink *xsink) {
      assert(ndir);

      // if changing to the current directory, then ignore
      if (ndir[0] == '.') {
	 const char* p = ndir + 1;
	 while (*p && *p == QORE_DIR_SEP)
	    ++p;
	 if (!*p)
	    return 0;
      }
      
      // if relative path then join with the old path and strip the path
      std::string ds;

      AutoLocker al(m);
      if (!q_absolute_path(ndir)) {
	 if (dirname.empty()) {
	    xsink->raiseException("DIR-CHDIR-ERROR", "cannot change to relative directory because no current directory is set");
	    return -1;
	 }
      
	 ds = dirname + QORE_DIR_SEP + std::string(ndir);
      }
      else
	 ds = ndir;

      ds = stripPath(ds);
      dirname = ds;

      //printf("chdir() ndir: '%s' ds: '%s'\n", ndir, ds.c_str());
 
      return checkPathIntern();
   }

   DLLLOCAL int mkdir(const char *subdir, int mode, ExceptionSink *xsink) const {
      assert(subdir);
      AutoLocker al(m);

      std::string path = getPathIntern(subdir);
      if (::mkdir(path.c_str(), mode)) {
	 xsink->raiseErrnoException("DIR-MKDIR-FAILURE", errno, "error creating directory '%s'", path.c_str());
	 return -1;
      }
      return 0;
   }

   DLLLOCAL int rmdir(const char *subdir, ExceptionSink *xsink) const {
      assert(subdir);
      AutoLocker al(m);

      std::string path = getPathIntern(subdir);
      if (::rmdir(path.c_str())) {
	 xsink->raiseErrnoException("DIR-RMDIR-FAILURE", errno, "error removing directory '%s'", path.c_str());
	 return -1;
      }
	 
      return 0;
   }

   DLLLOCAL QoreListNode *list(ExceptionSink *xsink, int stat_filter, const QoreString *regex, int regex_options, bool full) const {
      AutoLocker al(m);

      if (dirname.empty()) {
	 xsink->raiseException("DIR-READ-ERROR", "cannot list directory; no directory is set");
	 return 0;
      }
   
      SimpleRefHolder<QoreRegexNode> re(0);
   
      if (regex) {
	 re = new QoreRegexNode(*regex, regex_options, xsink);
	 if (*xsink)
	    return 0;
      }
      // avoid memory leaks...
      ReferenceHolder<QoreListNode> lst(new QoreListNode, xsink);
	 
      DIR *dptr = opendir(dirname.c_str());
      if (!dptr) {
	 xsink->raiseErrnoException("DIR-READ-FAILURE", errno, "error opening directory '%s' for reading", dirname.c_str());
	 return 0;
      }
      ON_BLOCK_EXIT(closedir, dptr);
	 
      struct dirent *de;
      while ((de = readdir(dptr))) {
	 if (!strcmp(de->d_name, ".") || !strcmp(de->d_name, ".."))
	    continue;

	 // if there is a regular expression, see if the name matches
	 if (regex) {
	    QoreString targ(de->d_name, enc);
	    bool b = re->exec(&targ, xsink);
	    if (*xsink)
	       return 0;
	    if (!b)
	       continue;
	 }

	 // if we are filtering out directories, then we have to stat the file
	 if (full || stat_filter != -1) {
	    QoreString fname(dirname);
	    fname.concat(QORE_DIR_SEP);
	    fname.concat(de->d_name);
	    struct stat buf;
	    if (!full) {
	       if (stat(fname.getBuffer(), buf, xsink))
		  return 0;

	       if (!(buf.st_mode & stat_filter))
		  continue;

	       lst->push(new QoreStringNode(de->d_name, enc));
	       continue;
	    }
#ifdef HAVE_LSTAT
	    if (lstat(fname.getBuffer(), buf, xsink))
	       return 0;

	    SimpleRefHolder<QoreStringNode> lpath;

	    if (S_ISLNK(buf.st_mode)) {
	       {
		  char lbuf[QORE_PATH_MAX + 1];
		  qore_offset_t len = readlink(fname.getBuffer(), lbuf, QORE_PATH_MAX);
		  if (len < 0) {
		     xsink->raiseErrnoException("DIR-READ-FAILURE", errno, "readlink('%s') failed", fname.getBuffer());
		     return 0;
		  }
		  assert(len <= QORE_PATH_MAX);
		  lbuf[len] = '\0';
		  lpath = new QoreStringNode(lbuf);
	       }

	       if (stat(fname.getBuffer(), buf, xsink))
		  return 0;	       
	    }
#else
	    if (stat(fname.getBuffer(), buf, xsink))
	       return 0;
#endif
	    if (stat_filter != -1 && !(buf.st_mode & stat_filter))
	       continue;
	    QoreHashNode* h = stat_to_hash(buf);
	    h->setKeyValue("name", new QoreStringNode(de->d_name, enc), 0);
#ifdef HAVE_LSTAT
	    if (*lpath)
	       h->setKeyValue("link", lpath.release(), 0);
#endif
	    lst->push(h);
	    continue;
	 }

	 // not full, no filter
	 lst->push(new QoreStringNode(de->d_name, enc));
	 continue;
      }
	    
      return lst.release();
   }

   DLLLOCAL int create(int mode, ExceptionSink *xsink) const {
      AutoLocker al(m);

      if (dirname.empty()) {
	 xsink->raiseException("DIR-CREATE-ERROR", "cannot create directory; no directory is set");
	 return -1;
      }

      // split the directory in its subdirectories tree
      std::vector<std::string> dirs;
      tokenize(dirname, dirs);
	 
      // iterate through all directories and try to create them if
      // they do not exist (should happen only on the first level)
      std::vector<std::string>::iterator it;
      std::string path;
      int cnt = 0;
      const char *path_str;
      for (it = dirs.begin(); it < dirs.end(); it++) {
	 path += "/" + (*it); // the actual path
	 path_str = path.c_str();
	 if (verifyDirectory(path)) { // not existing
	    if (::mkdir(path_str, mode)) { // failed
	       xsink->raiseErrnoException("DIR-CREATE-FAILURE", errno, "cannot mkdir '%s'", path_str);
	       return -1;
	    }
	    cnt++;
	 }
      }
	 
      return cnt;
   }

   DLLLOCAL int chmod(int mode, ExceptionSink *xsink) const {
      AutoLocker al(m);

      if (dirname.empty()) {
	 xsink->raiseException("DIR-CHMOD-ERROR", "cannot change directory mode; no directory is set");
	 return -1;
      }

      if (::chmod(dirname.c_str(), mode)) {
	 xsink->raiseErrnoException("DIR-CHMOD-FAILURE", errno, "error in Dir::chmod() on '%s'", dirname.c_str());
	 return -1;
      }

      return 0;
   }

#ifdef HAVE_PWD_H
   DLLLOCAL int chown(uid_t uid, gid_t gid, ExceptionSink *xsink) const {
      AutoLocker al(m);

      if (dirname.empty()) {
	 xsink->raiseException("DIR-CHOWN-ERROR", "cannot change directory ownership; no directory is set");
	 return -1;
      }

      if (::chown(dirname.c_str(), uid, gid)) {
	 xsink->raiseErrnoException("DIR-CHOWN-FAILURE", errno, "error in Dir::chown() on '%s'", dirname.c_str());
	 return 0;
      }

      return 0;
   }
#endif

   DLLLOCAL QoreListNode *stat(ExceptionSink *xsink) const {
      AutoLocker al(m);

      if (dirname.empty()) {
	 xsink->raiseException("DIR-STAT-ERROR", "cannot stat; no directory is set");
	 return 0;
      }

      struct stat sbuf;
      if (::stat(dirname.c_str(), &sbuf)) {
	 xsink->raiseErrnoException("DIR-STAT-FAILURE", errno, "stat() call failed on '%s'", dirname.c_str());
	 return 0;
      }

      return stat_to_list(sbuf);
   }

   DLLLOCAL QoreHashNode *hstat(ExceptionSink *xsink) const {
      AutoLocker al(m);

      if (dirname.empty()) {
	 xsink->raiseException("DIR-HSTAT-ERROR", "cannot stat; no directory is set");
	 return 0;
      }

      struct stat sbuf;
      if (::stat(dirname.c_str(), &sbuf)) {
	 xsink->raiseErrnoException("DIR-HSTAT-FAILURE", errno, "stat() call failed on '%s'", dirname.c_str());
	 return 0;
      }

      return stat_to_hash(sbuf);
   }

#ifdef HAVE_SYS_STATVFS_H
   DLLLOCAL QoreHashNode *statvfs(ExceptionSink *xsink) const {
      AutoLocker al(m);

      if (dirname.empty()) {
	 xsink->raiseException("DIR-STATVFS-ERROR", "cannot execute File::statvfs(); no directory is set");
	 return 0;
      }
  
      struct statvfs vfs;
      if (::statvfs(dirname.c_str(), &vfs)) {
	 xsink->raiseErrnoException("DIR-STATVFS-FAILURE", errno, "statvfs() call failed on '%s'", dirname.c_str());
	 return 0;
      }

      return statvfs_to_hash(vfs);
   }
#endif

   const QoreEncoding *getEncoding() const {
      return enc;
   }

#ifdef HAVE_LSTAT
   DLLLOCAL static int lstat(const char* str, struct stat& buf, ExceptionSink *xsink) {
      int rc = ::lstat(str, &buf);
      if (rc) {
	 xsink->raiseErrnoException("DIR-READ-FAILURE", errno, "lstat() failed on '%s'", str);
	 return -1;
      }
      return 0;
   }
#endif

   DLLLOCAL static int stat(const char* str, struct stat& buf, ExceptionSink *xsink) {
      int rc = ::stat(str, &buf);
      if (rc) {
	 xsink->raiseErrnoException("DIR-READ-FAILURE", errno, "stat() failed on '%s'", str);
	 return -1;
      }
      return 0;
   }
};

const QoreEncoding *QoreDir::getEncoding() const {
   return priv->getEncoding();
}

QoreDir::QoreDir(ExceptionSink *xsink, const QoreEncoding *cs, const char *dir) : priv(new qore_qd_private(xsink, cs, dir)) {}

QoreDir::QoreDir(ExceptionSink *xsink, const QoreDir &old) : priv(new qore_qd_private(xsink, *old.priv)) {
}

QoreDir::~QoreDir() {
   delete priv;
}

// return the actual dirname
QoreStringNode *QoreDir::dirname() const { 
   return priv->get_dir_string();
}
 
// change directory from current location on
// return 0 if directory exists and is openable
int QoreDir::chdir(const char* ndir, ExceptionSink *xsink) {
   return priv->chdir(ndir, xsink);
}

// create the directory with all the parent directories if they do not exist
// return amount of created directories, -1 if error
int QoreDir::create(int mode, ExceptionSink *xsink) const {
   return priv->create(mode, xsink);
}

// list entries of the directory where d points to
// the filter will be applied to the file's mode for filtering.
// directories '.' and '..' will be skipped
QoreListNode* QoreDir::list(ExceptionSink *xsink, int stat_filter, const QoreString *regex, int regex_options, bool full) const {
   return priv->list(xsink, stat_filter, regex, regex_options, full);
}

int QoreDir::mkdir(ExceptionSink *xsink, const char *subdir, int mode) const {
   return priv->mkdir(subdir, mode, xsink);
}

int QoreDir::rmdir(const char *subdir, ExceptionSink *xsink) const {
   return priv->rmdir(subdir, xsink);
}

std::string QoreDir::getPath(const char *sub) const {
   return priv->getPath(sub);
}

int QoreDir::checkPath() const {
   return priv->checkPath();
}

int QoreDir::chmod(int mode, ExceptionSink *xsink) const {
   return priv->chmod(mode, xsink);
}

#ifdef HAVE_PWD_H
int QoreDir::chown(uid_t uid, gid_t gid, ExceptionSink *xsink) const {
   return priv->chown(uid, gid, xsink);
}
#endif

QoreListNode *QoreDir::stat(ExceptionSink *xsink) const {
   return priv->stat(xsink);
}

QoreHashNode *QoreDir::hstat(ExceptionSink *xsink) const {
   return priv->hstat(xsink);
}

#ifdef HAVE_SYS_STATVFS_H
QoreHashNode *QoreDir::statvfs(ExceptionSink *xsink) const {
   return priv->statvfs(xsink);
}
#endif
