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
#include <qore/QoreDir.h>

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


struct qore_qd_private {
  const QoreEncoding *charset;
  //std::string dirname;
  char *dirname;
  
  DLLLOCAL qore_qd_private(const QoreEncoding *cs) {
    // set the directory to the cwd
    char *cwd=(char*)malloc(sizeof(char)*PATH_MAX);
    if(!cwd) { // error in malloc
      return;
    }
    if(!getcwd(cwd, (size_t)PATH_MAX)) { // error in cwd
      return;
    }
    dirname=cwd;

    // the character set
    charset = cs;
  }

  DLLLOCAL ~qore_qd_private() {
    if(dirname) {
      free(dirname);
    }
  }
};


const QoreEncoding *QoreDir::getEncoding() const {
  return priv->charset;
}

QoreDir::QoreDir(const QoreEncoding *cs) : priv(new qore_qd_private(cs)) {}

QoreDir::~QoreDir() {
  //close();
  delete priv;
}


// ------------


// return the actual dirname
const char* QoreDir::dirname() const { 
  return priv->dirname;
}

// check if the path in dirname exists
// return 0 if the path exists
// return errno of the opendir function
int QoreDir::checkPath() {
  return verifyDirectory(priv->dirname);
}
 
// check if the given directory is accessable
// return errno of opendir function
int QoreDir::verifyDirectory(const char* dir) {
  DIR *dptr=opendir(dir);
  if(!dptr) {
    return errno;
  }

  // free again
  closedir(dptr);

  return 0;
}

// change directory from current location on
// return 0 if directory exists and is openable
int QoreDir::chdir(const char* ndir) {
  int ret=0;

  // if relative path then join with the old path and strip the path
  std::string ds;
  if(ndir[0]!='/') { // relative path
    ds=std::string(priv->dirname)+"/"+std::string(ndir);
  }
  else {
    ds=ndir;
  }
  ds=stripPath(ds);

  // clean up old dirname
  free(priv->dirname);
  // set the new dirname
  priv->dirname=strdup(strdup(ds.c_str()));

  // look if the dirname exists
  /*
  DIR *dptr=opendir(priv->dirname);
  if(dptr) {
    // free again
    closedir(dptr);
  }
  else {
    ret=errno;
  }
  */
  ret=checkPath();

  return ret;
}

// tokenize the strings by the delimiter
void QoreDir::tokenize(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiter = "/") {
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
const std::string QoreDir::stripPath(const std::string& odir) {
  // tokenize the string
  std::vector<std::string> ptoken, dirs;
  tokenize(odir, ptoken);

  // push them to the new path
  std::vector<std::string>::iterator it;
  for(it=ptoken.begin(); it<ptoken.end(); it++) {
    std::string d=*it;
    if(d=="." || d=="") { // ignore
      continue;
    }

    if(d==".." && !dirs.empty()) { // step back one step
      dirs.pop_back();
    }
    else {
      dirs.push_back(d);
    }
  }

  // create string out of rest..
  std::string ret;
  for(it=dirs.begin(); it<dirs.end(); it++) {
    ret+="/"+(*it);
  }

  return ret;
}

// create the directory with all the parent directories if they do not exist
// return amount of created directoreis, -1 if error
int QoreDir::create(int mode, ExceptionSink *xsink) {
  // split the directory in its subdirectories tree
  std::vector<std::string> dirs;
  //  tokenize(std::string(dirname), dirs);
  //  std::string dstr=std::string(dirname());
  //tokenize(dstr, dirs);
  tokenize(std::string(dirname()), dirs);

  // iterate through all directories and try to create them if
  // they do not exist (should happen only on the first level)
  std::vector<std::string>::iterator it;
  std::string path;
  int cnt=0;
  const char *path_str;
  for(it=dirs.begin(); it<dirs.end(); it++) {
    path+="/"+(*it); // the actual path
    path_str=path.c_str();
    if(verifyDirectory(path_str)) { // not existing
      if(mkdir(path_str, mode)) { // failed
	xsink->raiseException("DIR-CREATE-ERROR", "cannot mkdir '%s': %s", path_str, strerror(errno));
	return -1;
      }
      cnt++;
    }
  }

  return cnt;
}



// list entries of the directory where d points to
// the filter willl be applied to struct dirent->d_type for filtering out
// directories '.' and '..' will be skipped
//QoreListNode* QoreDir::list(Dir *d, int d_filter, ExceptionSink *xsink) {
QoreListNode* QoreDir::list(int d_filter, ExceptionSink *xsink) {
  //QoreListNode *lst=new QoreListNode();
  // avoid memory leaks...
  ReferenceHolder<QoreListNode> lst(new QoreListNode(), xsink);

  DIR *dptr=opendir(dirname());
  if(!dptr) {
    xsink->raiseException("DIR-READ-ERROR", "error opening directory for reading: %s", strerror(errno));
    return 0;
  }

  struct dirent *de;
  while((de=readdir(dptr))) {
    if(strcmp(de->d_name, ".") && strcmp(de->d_name, "..")) {
      if(de->d_type & d_filter) {
	lst->push(new QoreStringNode(de->d_name));
      }
    }
  }
  // check for error of readdir
  if(errno) {
    xsink->raiseException("DIR-READ-ERROR", "error while reading directory: %s", strerror(errno));
    // but anyhow: close the dir pointer and ignore the message
    closedir(dptr);
    return 0;
  }

  // close
  closedir(dptr);

  return lst.release();
}
