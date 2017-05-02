/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QC_Condition.h
   
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

#ifndef _QORE_CLASS_CONDITION_H

#define _QORE_CLASS_CONDITION_H

#include <qore/Qore.h>
#include <qore/QoreCondition.h>
#include "qore/intern/AbstractSmartLock.h"
#include "qore/intern/SmartMutex.h"

DLLEXPORT extern qore_classid_t CID_CONDITION;
DLLLOCAL extern QoreClass* QC_CONDITION;

QoreClass *initConditionClass(QoreNamespace& ns);

class Condition : public AbstractPrivateData {
private:
   QoreCondition cond;

protected:
   DLLLOCAL virtual ~Condition() {}

public:
   DLLLOCAL int wait(AbstractSmartLock *m, int64 timeout, ExceptionSink *xsink) {
      return m->extern_wait(&cond, xsink, timeout);
   }
   DLLLOCAL int wait(AbstractSmartLock *m, ExceptionSink *xsink) {
      return m->extern_wait(&cond, xsink);
   }
   DLLLOCAL int signal() {
      return cond.signal();
   }
   DLLLOCAL int broadcast() {
      return cond.broadcast();
   }
   DLLLOCAL int wait_count(AbstractSmartLock *m) {
      return m->cond_count(&cond);
   }
};

#endif // _QORE_CLASS_CONDITION_H
