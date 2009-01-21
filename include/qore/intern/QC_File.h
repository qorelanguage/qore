/*
  QC_File.h
  
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

#ifndef _QORE_CLASS_FILE_H

#define _QORE_CLASS_FILE_H

DLLEXPORT extern qore_classid_t CID_FILE;
DLLEXPORT extern QoreClass *QC_File;

DLLLOCAL QoreClass *initFileClass();
static inline void addFileConstants(QoreNamespace *ns);

#include <qore/QoreFile.h>
#include <qore/AbstractPrivateData.h>

class File : public AbstractPrivateData, public QoreFile {
   protected:
      DLLLOCAL virtual ~File() {}

   public:
      DLLLOCAL File(const QoreEncoding *cs) : QoreFile(cs) {}

      DLLLOCAL virtual void deref(ExceptionSink *xsink) {
         if (ROdereference()) {
            cleanup(xsink);
            delete this;
         }
      }

      DLLLOCAL virtual void deref() {
         if (ROdereference()) {
            ExceptionSink xsink;
            cleanup(&xsink);
            delete this;
         }
      }
};

static inline void addFileConstants(QoreNamespace *ns) {
#ifdef O_ACCMODE
   ns->addConstant("O_ACCMODE", new QoreBigIntNode(O_ACCMODE));
#endif
#ifdef O_RDONLY
   ns->addConstant("O_RDONLY", new QoreBigIntNode(O_RDONLY));
#endif
#ifdef O_WRONLY
   ns->addConstant("O_WRONLY", new QoreBigIntNode(O_WRONLY));
#endif
#ifdef O_RDWR
   ns->addConstant("O_RDWR", new QoreBigIntNode(O_RDWR));
#endif
#ifdef O_CREAT
   ns->addConstant("O_CREAT", new QoreBigIntNode(O_CREAT));
#endif
#ifdef O_EXCL
   ns->addConstant("O_EXCL", new QoreBigIntNode(O_EXCL));
#endif
#ifdef O_NOCTTY
   ns->addConstant("O_NOCTTY", new QoreBigIntNode(O_NOCTTY));
#endif
#ifdef O_TRUNC
   ns->addConstant("O_TRUNC", new QoreBigIntNode(O_TRUNC));
#endif
#ifdef O_APPEND
   ns->addConstant("O_APPEND", new QoreBigIntNode(O_APPEND));
#endif
#ifdef O_NONBLOCK
   ns->addConstant("O_NONBLOCK", new QoreBigIntNode(O_NONBLOCK));
#endif
#ifdef O_NDELAY
   ns->addConstant("O_NDELAY", new QoreBigIntNode(O_NDELAY));
#endif
#ifdef O_SYNC
   ns->addConstant("O_SYNC", new QoreBigIntNode(O_SYNC));
#endif
#ifdef O_DIRECT
   ns->addConstant("O_DIRECT", new QoreBigIntNode(O_DIRECT));
#endif
#ifdef O_LARGEFILE
   ns->addConstant("O_LARGEFILE", new QoreBigIntNode(O_LARGEFILE));
#endif
#ifdef O_DIRECTORY
   ns->addConstant("O_DIRECTORY", new QoreBigIntNode(O_DIRECTORY));
#endif
#ifdef O_NOFOLLOW
   ns->addConstant("O_NOFOLLOW", new QoreBigIntNode(O_NOFOLLOW));
#endif
#ifdef O_ATOMICLOOKUP
   ns->addConstant("O_ATOMICLOOKUP", new QoreBigIntNode(O_ATOMICLOOKUP));
#endif
#ifdef S_IRUSR
   ns->addConstant("S_IRUSR", new QoreBigIntNode(S_IRUSR));
#endif
#ifdef S_IWUSR
   ns->addConstant("S_IWUSR", new QoreBigIntNode(S_IWUSR));
#endif
#ifdef S_IXUSR
   ns->addConstant("S_IXUSR", new QoreBigIntNode(S_IXUSR));
#endif
#ifdef S_IRWXU
   ns->addConstant("S_IRWXU", new QoreBigIntNode(S_IRWXU));
#endif
#ifdef S_IREAD
   ns->addConstant("S_IREAD", new QoreBigIntNode(S_IREAD));
#endif
#ifdef S_IWRITE
   ns->addConstant("S_IWRITE", new QoreBigIntNode(S_IWRITE));
#endif
#ifdef S_IRGRP
   ns->addConstant("S_IRGRP", new QoreBigIntNode(S_IRGRP));
#endif
#ifdef S_IWGRP
   ns->addConstant("S_IWGRP", new QoreBigIntNode(S_IWGRP));
#endif
#ifdef S_IXGRP
   ns->addConstant("S_IXGRP", new QoreBigIntNode(S_IXGRP));
#endif
#ifdef S_IRWXG
   ns->addConstant("S_IRWXG", new QoreBigIntNode(S_IRWXG));
#endif
#ifdef S_IROTH
   ns->addConstant("S_IROTH", new QoreBigIntNode(S_IROTH));
#endif
#ifdef S_IWOTH
   ns->addConstant("S_IWOTH", new QoreBigIntNode(S_IWOTH));
#endif
#ifdef S_IXOTH
   ns->addConstant("S_IXOTH", new QoreBigIntNode(S_IXOTH));
#endif
#ifdef S_IRWXO
   ns->addConstant("S_IRWXO", new QoreBigIntNode(S_IRWXO));
#endif

   // other file constants
   ns->addConstant("F_RDLCK",       new QoreBigIntNode(F_RDLCK));
   ns->addConstant("F_WRLCK",       new QoreBigIntNode(F_WRLCK));

   ns->addConstant("SEEK_SET",      new QoreBigIntNode(SEEK_SET));
   ns->addConstant("SEEK_CUR",      new QoreBigIntNode(SEEK_CUR));
   ns->addConstant("SEEK_END",      new QoreBigIntNode(SEEK_END));
}

#endif // _QORE_CLASS_FILE_H
