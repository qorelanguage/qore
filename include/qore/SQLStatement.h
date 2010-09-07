/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  SQLStatement.h

  Qore Programming Language

  Copyright (C) 2006 - 2010 QoreTechnologies
  
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

#ifndef _QORE_SQLSTATEMENT_H
#define _QORE_SQLSTATEMENT_H

class DatasourceStatementHelper;

class SQLStatement {
   friend class QoreSQLStatement;

private:
   struct sql_statement_private *priv; // private implementation

public:
   DLLLOCAL SQLStatement();
   DLLLOCAL ~SQLStatement();

   //! returns the private DBI-specific data structure for this object
   DLLEXPORT void *getPrivateData() const;

   //! sets the private DBI-specific data structure for this object
   /** this should only be called once in the actual DBI driver code
       @param data the data for the DBI driver that holds the driver-specific state of the connection
   */
   DLLEXPORT void setPrivateData(void *data);

   //! returns the Datasource bound to this statement
   /** @return the Datasource bound to this statement
    */
   DLLEXPORT Datasource *getDatasource() const;

   //! sets the "in transaction" flag
   DLLEXPORT void setInTransaction();
};

#endif
