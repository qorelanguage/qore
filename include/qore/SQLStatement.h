/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  SQLStatement.h

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

#ifndef _QORE_SQLSTATEMENT_H
#define _QORE_SQLSTATEMENT_H

//! This is the public class for DBI drivers supporting Qore's new prepared statement API
/** @see DBIDriver
 */
class SQLStatement {
   friend class DBActionHelper;
   friend class QoreSQLStatement;

private:
   struct sql_statement_private* priv; // private implementation

public:
   DLLLOCAL SQLStatement();

   DLLLOCAL SQLStatement(Datasource* ds, void* data);

   DLLLOCAL ~SQLStatement();

   //! returns the private DBI-specific data structure for this object
   DLLEXPORT void* getPrivateData() const;

   //! returns the private DBI-specific data structure for this object and clears the object
   DLLEXPORT void* takePrivateData();

   //! sets the private DBI-specific data structure for this object
   /** this should only be called once in the actual DBI driver code
       @param data the data for the DBI driver that holds the driver-specific state of the connection
   */
   DLLEXPORT void setPrivateData(void* data);

   //! returns the Datasource bound to this statement
   /** @return the Datasource bound to this statement
    */
   DLLEXPORT Datasource* getDatasource() const;
};

#endif
