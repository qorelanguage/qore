/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  SQLStatement.h

  Qore Programming Language

  Copyright (C) 2006 - 2012 QoreTechnologies
  
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
#include <qore/intern/sql_statement_private.h>

SQLStatement::SQLStatement() : priv(new sql_statement_private) {
}

SQLStatement::~SQLStatement() {
   delete priv;
}

void *SQLStatement::getPrivateData() const {
   return priv->data;
}

void *SQLStatement::takePrivateData() {
   void *rv = priv->data;
   priv->data = 0;
   return rv;
}

void SQLStatement::setPrivateData(void *n_data) {
   assert(!priv->data || !n_data);
   priv->data = n_data;
}

Datasource *SQLStatement::getDatasource() const {
   return priv->ds;
}
