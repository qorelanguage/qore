/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QC_File.h

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

#ifndef _QORE_CLASS_FILE_H

#define _QORE_CLASS_FILE_H

DLLEXPORT extern qore_classid_t CID_FILE;
DLLEXPORT extern QoreClass *QC_FILE;
DLLEXPORT extern QoreClass *QC_READONLYFILE;

DLLLOCAL QoreClass *initFileClass(QoreNamespace &qorens);

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

inline int check_terminal_io(QoreObject* self, const char* m, ExceptionSink* xsink) {
   // check for no-terminal-io at runtime with system objecs
   if (self->isSystemObject() && runtime_check_parse_option(PO_NO_TERMINAL_IO)) {
      xsink->raiseException("ILLEGAL-EXPRESSION", "%s() cannot be called with a system constant object when 'no-terminal-io' is set", m);
      return -1;
   }
   return 0;
}

#endif // _QORE_CLASS_FILE_H
