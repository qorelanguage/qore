/*
  QC_File.h
  
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

#ifndef _QORE_CLASS_FILE_H

#define _QORE_CLASS_FILE_H

DLLEXPORT extern int CID_FILE;
DLLLOCAL class QoreClass *initFileClass();
static inline void addFileConstants(class QoreNamespace *ns);

#include <qore/QoreFile.h>
#include <qore/AbstractPrivateData.h>

class File : public AbstractPrivateData, public QoreFile
{
   protected:
      DLLLOCAL virtual ~File() {}

   public:
      DLLLOCAL inline File(const class QoreEncoding *cs) : QoreFile(cs) {}
};

static inline void addFileConstants(class QoreNamespace *ns)
{
#ifdef O_ACCMODE
   ns->addConstant("O_ACCMODE", new QoreNode(NT_INT, O_ACCMODE));
#endif
#ifdef O_RDONLY
   ns->addConstant("O_RDONLY", new QoreNode(NT_INT, O_RDONLY));
#endif
#ifdef O_WRONLY
   ns->addConstant("O_WRONLY", new QoreNode(NT_INT, O_WRONLY));
#endif
#ifdef O_RDWR
   ns->addConstant("O_RDWR", new QoreNode(NT_INT, O_RDWR));
#endif
#ifdef O_CREAT
   ns->addConstant("O_CREAT", new QoreNode(NT_INT, O_CREAT));
#endif
#ifdef O_EXCL
   ns->addConstant("O_EXCL", new QoreNode(NT_INT, O_EXCL));
#endif
#ifdef O_NOCTTY
   ns->addConstant("O_NOCTTY", new QoreNode(NT_INT, O_NOCTTY));
#endif
#ifdef O_TRUNC
   ns->addConstant("O_TRUNC", new QoreNode(NT_INT, O_TRUNC));
#endif
#ifdef O_APPEND
   ns->addConstant("O_APPEND", new QoreNode(NT_INT, O_APPEND));
#endif
#ifdef O_NONBLOCK
   ns->addConstant("O_NONBLOCK", new QoreNode(NT_INT, O_NONBLOCK));
#endif
#ifdef O_NDELAY
   ns->addConstant("O_NDELAY", new QoreNode(NT_INT, O_NDELAY));
#endif
#ifdef O_SYNC
   ns->addConstant("O_SYNC", new QoreNode(NT_INT, O_SYNC));
#endif
#ifdef O_DIRECT
   ns->addConstant("O_DIRECT", new QoreNode(NT_INT, O_DIRECT));
#endif
#ifdef O_LARGEFILE
   ns->addConstant("O_LARGEFILE", new QoreNode(NT_INT, O_LARGEFILE));
#endif
#ifdef O_DIRECTORY
   ns->addConstant("O_DIRECTORY", new QoreNode(NT_INT, O_DIRECTORY));
#endif
#ifdef O_NOFOLLOW
   ns->addConstant("O_NOFOLLOW", new QoreNode(NT_INT, O_NOFOLLOW));
#endif
#ifdef O_ATOMICLOOKUP
   ns->addConstant("O_ATOMICLOOKUP", new QoreNode(NT_INT, O_ATOMICLOOKUP));
#endif
#ifdef S_IRUSR
   ns->addConstant("S_IRUSR", new QoreNode(NT_INT, S_IRUSR));
#endif
#ifdef S_IWUSR
   ns->addConstant("S_IWUSR", new QoreNode(NT_INT, S_IWUSR));
#endif
#ifdef S_IXUSR
   ns->addConstant("S_IXUSR", new QoreNode(NT_INT, S_IXUSR));
#endif
#ifdef S_IRWXU
   ns->addConstant("S_IRWXU", new QoreNode(NT_INT, S_IRWXU));
#endif
#ifdef S_IREAD
   ns->addConstant("S_IREAD", new QoreNode(NT_INT, S_IREAD));
#endif
#ifdef S_IWRITE
   ns->addConstant("S_IWRITE", new QoreNode(NT_INT, S_IWRITE));
#endif
#ifdef S_IRGRP
   ns->addConstant("S_IRGRP", new QoreNode(NT_INT, S_IRGRP));
#endif
#ifdef S_IWGRP
   ns->addConstant("S_IWGRP", new QoreNode(NT_INT, S_IWGRP));
#endif
#ifdef S_IXGRP
   ns->addConstant("S_IXGRP", new QoreNode(NT_INT, S_IXGRP));
#endif
#ifdef S_IRWXG
   ns->addConstant("S_IRWXG", new QoreNode(NT_INT, S_IRWXG));
#endif
#ifdef S_IROTH
   ns->addConstant("S_IROTH", new QoreNode(NT_INT, S_IROTH));
#endif
#ifdef S_IWOTH
   ns->addConstant("S_IWOTH", new QoreNode(NT_INT, S_IWOTH));
#endif
#ifdef S_IXOTH
   ns->addConstant("S_IXOTH", new QoreNode(NT_INT, S_IXOTH));
#endif
#ifdef S_IRWXO
   ns->addConstant("S_IRWXO", new QoreNode(NT_INT, S_IRWXO));
#endif
}

#endif // _QORE_CLASS_FILE_H
