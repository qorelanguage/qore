/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QC_File.h
  
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

#ifndef _QORE_CLASS_FILE_H

#define _QORE_CLASS_FILE_H

DLLEXPORT extern qore_classid_t CID_FILE;
DLLEXPORT extern QoreClass *QC_FILE;

DLLLOCAL QoreClass *initFileClass(QoreNamespace &qorens);
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

#ifdef HAVE_STRUCT_FLOCK
   // file lock constants
   ns->addConstant("F_RDLCK",       new QoreBigIntNode(F_RDLCK));
   ns->addConstant("F_WRLCK",       new QoreBigIntNode(F_WRLCK));
   ns->addConstant("F_UNLCK",       new QoreBigIntNode(F_UNLCK));
#endif

   // file seek constants
   ns->addConstant("SEEK_SET",      new QoreBigIntNode(SEEK_SET));
   ns->addConstant("SEEK_CUR",      new QoreBigIntNode(SEEK_CUR));
   ns->addConstant("SEEK_END",      new QoreBigIntNode(SEEK_END));
}

#endif // _QORE_CLASS_FILE_H
