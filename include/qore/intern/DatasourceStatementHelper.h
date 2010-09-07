/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  DatasourceStatementHelper.h

  Qore Programming Language

  Copyright (C) 2006 - 2010 Qore Technologies, sro
  
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

#ifndef _QORE_DATASOURCESTATEMENTHELPER_H
#define _QORE_DATASOURCESTATEMENTHELPER_H

class DatasourceStatementHelper {
public:
   DLLLOCAL DatasourceStatementHelper() {
   }

   DLLLOCAL virtual ~DatasourceStatementHelper() {
   }

   // must dereference the datasource-providing object
   virtual void helperDestructor(ExceptionSink *xsink) = 0;
   virtual Datasource *helperGetDatasource(ExceptionSink *xsink) = 0;
   virtual int helperReleaseDatasource(bool started_transaction, ExceptionSink *xsink) = 0;
   virtual QoreThreadLock *helperGetConnectionLock() = 0;
};

#endif

