/*
  QoreDir.cc

  Directory functions

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

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

class qore_qd_private {
   public:
      // check if the given directory is accessable
      // return errno of opendir function
      // unlocked
      DLLLOCAL static int verifyDirectory(const char* dir)
      {
	 DIR *dptr = opendir(dir);
	 if (!dptr) {
	    return errno;
	 }

	 // free again
	 closedir(dptr);
	 
	 return 0;
      }

      // tokenize the strings by the delimiter
      DLLLOCAL static void tokenize(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiter = "/") {
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
	 for (it = ptoken.begin(); it<ptoken.end(); it++) {
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
	 for (it = dirs.begin(); it<dirs.end(); it++) {
	    ret+= "/"+(*it);
	 }

	 return ret;
      }

   protected:
      // unlocked
      DLLLOCAL std::string getPathIntern(const char *sub) const
      {
	 if (dirname)
	    return std::string(dirname) + "/" + std::string(sub);
	 return std::string(sub);
      }

      // check if the path in dirname exists
      // return 0 if the path exists
      // return errno of the opendir function
      // unlocked
      DLLLOCAL int checkPathIntern() const
      {
	 return !dirname ? -1 : verifyDirectory(dirname);
      }

   public:
      const QoreEncoding *charset;
      char *dirname;
      mutable QoreThreadLock m;
  
      DLLLOCAL qore_qd_private(ExceptionSink *xsink, const QoreEncoding *cs, const char *dir) : charset(cs), dirname(dir ? strdup(dir) : 0)
      {
	 if (!dirname) {
	    // set the directory to the cwd
	    char *cwd = (char*)malloc(sizeof(char)*PATH_MAX);
	    if (!cwd) { // error in malloc
	       xsink->outOfMemory();
	       return;
	    }

	    if (!getcwd(cwd, (size_t)PATH_MAX)) { // error in cwd
	       free(cwd);
	       return;
	    }

	    dirname = cwd;
	 }
      }

      DLLLOCAL qore_qd_private(ExceptionSink *xsink, const qore_qd_private &old)
      {
	 AutoLocker al(old.m);
	 charset = old.charset;
	 if (old.dirname) {
	    dirname = strdup(old.dirname);
	    if (!dirname)
	       xsink->outOfMemory();
	 }
	 else
	    dirname = 0;
      }
      
      DLLLOCAL ~qore_qd_private() {
	 if (dirname) {
	    free(dirname);
	 }
      }

      DLLLOCAL QoreStringNode *get_dir_string() const
      {
	 AutoLocker al(m);
	 return dirname ? new QoreStringNode(dirname, charset) : 0;
      }

      DLLLOCAL std::string getPath(const char *sub) const
      {
	 AutoLocker al(m);

	 return getPathIntern(sub);
      }
 
      DLLLOCAL int checkPath() const
      {
	 AutoLocker al(m);

	 return checkPathIntern();
      }

      DLLLOCAL int chdir(const char* ndir, ExceptionSink *xsink)
      {
	 assert(ndir);

	 // if relative path then join with the old path and strip the path
	 std::string ds;

	 AutoLocker al(m);
	 if (ndir[0] != '/') { // relative path
	    if (!dirname) {
	       xsink->raiseException("DIR-CHDIR-ERROR", "cannot change to relative directory; no directory is set");
	       return -1;
	    }
      
	    ds = std::string(dirname) + "/" + std::string(ndir);
	 }
	 else {
	    ds = ndir;
	 }
	 ds = stripPath(ds);

	 // clean up old dirname
	 if (dirname)
	    free(dirname);
	 // set the new dirname
	 dirname = strdup(ds.c_str());
 
	 return checkPathIntern();
      }

      DLLLOCAL int mkdir(const char *subdir, int mode, ExceptionSink *xsink) const
      {
	 assert(subdir);
	 AutoLocker al(m);

	 std::string path = getPathIntern(subdir);
	 if (::mkdir(path.c_str(), mode)) {
	    xsink->raiseException("DIR-MKDIR-ERROR", "error on creating subdirectory '%s' in '%s': %s", subdir, dirname, strerror(errno));
	    return -1;
	 }
	 return 0;
      }

      DLLLOCAL int rmdir(const char *subdir, ExceptionSink *xsink) const
      {
	 assert(subdir);
	 AutoLocker al(m);

	 std::string path = getPathIntern(subdir);
	 if (::rmdir(path.c_str())) {
	    xsink->raiseException("DIR-RMDIR-ERROR", "error on removing subdirectory '%s' in '%s': %s", subdir, dirname, strerror(errno));
	    return -1;
	 }
	 
	 return 0;
      }

      DLLLOCAL QoreListNode *list(ExceptionSink *xsink, int stat_filter, const QoreString *regex, int regex_options) const
      {
	 AutoLocker al(m);

	 if (!dirname) {
	    xsink->raiseException("DIR-READ-ERROR", "cannot list directory; no directory is set");
	    return 0;
	 }
   
	 SimpleRefHolder<QoreRegexNode> re(0);
   
	 if (regex) {
	    re = new QoreRegexNode(regex, regex_options, xsink);
	    if (*xsink)
	       return 0;
	 }
	 //QoreListNode *lst = new QoreListNode();
	 // avoid memory leaks...
	 ReferenceHolder<QoreListNode> lst(new QoreListNode(), xsink);
	 
	 DIR *dptr = opendir(dirname);
	 if (!dptr) {
	    xsink->raiseException("DIR-READ-ERROR", "error opening directory for reading: %s", strerror(errno));
	    return 0;
	 }
	 ON_BLOCK_EXIT(closedir, dptr);
	 
	 struct dirent *de;
	 while ((de = readdir(dptr))) {
	    if (strcmp(de->d_name, ".") && strcmp(de->d_name, "..")) {
	       bool ok = true;
	       // if we are filtering out directories, then we have to stat the file
	       if (stat_filter != -1) {
		  QoreString fname(dirname);
		  fname.concat('/');
		  fname.concat(de->d_name);
		  struct stat buf;
		  int rc = stat(fname.getBuffer(), &buf);
		  if (rc) {
		     xsink->raiseException("DIR-READ-ERROR", "stat() failed on '%s': %s", fname.getBuffer(), strerror(errno));
		     return 0;
		  }
		  ok = (bool)(buf.st_mode & stat_filter);
	       }
	       if (ok) {
		  // if there is a regular expression, see if the name matches
		  if (regex) {
		     QoreString targ(de->d_name, charset);
		     bool b = re->exec(&targ, xsink);
		     if (*xsink)
			return 0;
		     if (!b)
			continue;
		  }
		  lst->push(new QoreStringNode(de->d_name, charset));
	       }
	    }
	 }
	    
#if 0
	 // check for error of readdir - not necessary???
	 if (errno) {
	    xsink->raiseException("DIR-READ-ERROR", "error while reading directory: %s", strerror(errno));
	    // but anyhow: close the dir pointer and ignore the message
	    return 0;
	 }
#endif
	 return lst.release();
      }

      DLLLOCAL int create(int mode, ExceptionSink *xsink) const
      {
	 AutoLocker al(m);

	 if (!dirname) {
	    xsink->raiseException("DIR-CREATE-ERROR", "cannot create directory; no directory is set");
	    return -1;
	 }

	 // split the directory in its subdirectories tree
	 std::vector<std::string> dirs;
	 tokenize(std::string(dirname), dirs);
	 
	 // iterate through all directories and try to create them if
	 // they do not exist (should happen only on the first level)
	 std::vector<std::string>::iterator it;
	 std::string path;
	 int cnt = 0;
	 const char *path_str;
	 for (it = dirs.begin(); it < dirs.end(); it++) {
	    path += "/" + (*it); // the actual path
	    path_str = path.c_str();
	    if (verifyDirectory(path_str)) { // not existing
	       if (::mkdir(path_str, mode)) { // failed
		  xsink->raiseException("DIR-CREATE-ERROR", "cannot mkdir '%s': %s", path_str, strerror(errno));
		  return -1;
	       }
	       cnt++;
	    }
	 }
	 
	 return cnt;
      }

      DLLLOCAL int chmod(int mode, ExceptionSink *xsink) const 
      {
	 AutoLocker al(m);

	 if (!dirname) {
	    xsink->raiseException("DIR-CHMOD-ERROR", "cannot change directory mode; no directory is set");
	    return -1;
	 }

	 if (::chmod(dirname, mode)) {
	    xsink->raiseException("DIR-CHMOD-ERROR", "error in Dir::chmod(): %s", strerror(errno));
	    return -1;
	 }

	 return 0;
      }

      DLLLOCAL int chown(uid_t uid, gid_t gid, ExceptionSink *xsink) const
      {
	 AutoLocker al(m);

	 if (!dirname) {
	    xsink->raiseException("DIR-CHOWN-ERROR", "cannot change directory ownership; no directory is set");
	    return -1;
	 }

	 if (::chown(dirname, uid, gid)) {
	    xsink->raiseException("DIR-CHOWN-ERROR", "error in Dir::chown(): %s", strerror(errno));
	    return 0;
	 }

	 return 0;
      }

};

const QoreEncoding *QoreDir::getEncoding() const {
  return priv->charset;
}

QoreDir::QoreDir(ExceptionSink *xsink, const QoreEncoding *cs, const char *dir) : priv(new qore_qd_private(xsink, cs, dir)) {}

QoreDir::QoreDir(ExceptionSink *xsink, const QoreDir &old) : priv(new qore_qd_private(xsink, *old.priv))
{
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
QoreListNode* QoreDir::list(ExceptionSink *xsink, int stat_filter, const QoreString *regex, int regex_options) const {
   return priv->list(xsink, stat_filter, regex, regex_options);
}

int QoreDir::mkdir(ExceptionSink *xsink, const char *subdir, int mode) const
{
   return priv->mkdir(subdir, mode, xsink);
}

int QoreDir::rmdir(const char *subdir, ExceptionSink *xsink) const
{
   return priv->rmdir(subdir, xsink);
}

std::string QoreDir::getPath(const char *sub) const
{
   return priv->getPath(sub);
}

int QoreDir::checkPath() const
{
   return priv->checkPath();
}

int QoreDir::chmod(int mode, ExceptionSink *xsink) const
{
   return priv->chmod(mode, xsink);
}

int QoreDir::chown(uid_t uid, gid_t gid, ExceptionSink *xsink) const
{
   return priv->chown(uid, gid, xsink);
}
