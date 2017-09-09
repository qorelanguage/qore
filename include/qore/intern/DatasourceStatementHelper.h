/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  DatasourceStatementHelper.h

  Qore Programming Language

  Copyright (C) 2006 - 2017 Qore Technologies, s.r.o.

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

#ifndef _QORE_DATASOURCESTATEMENTHELPER_H
#define _QORE_DATASOURCESTATEMENTHELPER_H

class QoreSQLStatement;

class DatasourceStatementHelper {
public:
   DLLLOCAL DatasourceStatementHelper() {}
   DLLLOCAL virtual ~DatasourceStatementHelper() {}

   DLLLOCAL Datasource* helperStartAction(ExceptionSink* xsink, bool& new_transaction) {
      return helperStartActionImpl(xsink, new_transaction);
   }

   DLLLOCAL void helperDestructor(QoreSQLStatement* s, ExceptionSink* xsink) {
      helperDestructorImpl(s, xsink);
   }

   DLLLOCAL Datasource* helperEndAction(char cmd, bool new_transaction, ExceptionSink* xsink) {
      return helperEndActionImpl(cmd, new_transaction, xsink);
   }

   DLLLOCAL DatasourceStatementHelper* helperRefSelf() {
      return helperRefSelfImpl();
   }

   // must dereference the datasource-providing object
   virtual void helperDestructorImpl(QoreSQLStatement* s, ExceptionSink* xsink) = 0;
   virtual Datasource* helperStartActionImpl(ExceptionSink* xsink, bool& new_transaction) = 0;
   virtual Datasource* helperEndActionImpl(char cmd, bool new_transaction, ExceptionSink* xsink) = 0;
   virtual DatasourceStatementHelper* helperRefSelfImpl() = 0;

};

#endif
