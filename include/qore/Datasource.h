/*
  Datasource.h

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006 David Nichols

  NOTE that 2 copies of connection values are kept in case
  the values are changed while a connection is in use

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

/* 
   FIXME: commit()s when autocommit=true should be made here, also after
          select()s (in case of a select for update, for example)
 */

#ifndef _QORE_DATASOURCE_H

#define _QORE_DATASOURCE_H

class Datasource
{
   protected:
      bool in_transaction;
      bool isopen;
      bool autocommit;
      class DBIDriver *dsl;
      class QoreEncoding *qorecharset;
      void *private_data;

      // for pending connection values
      char *p_username,  // for Oracle, MySQL
	 *p_password,    // for Oracle, MySQL
	 *p_dbname,      // for Oracle, MySQL
	 *p_db_encoding, // for Oracle, MySQL - database-specific name for the encoding for the connection
	 *p_hostname;    // for MySQL

      // actual connection values set by init() before the datasource is opened
      char *username,    // for Oracle, MySQL
	 *password,      // for Oracle, MySQL
	 *db_encoding,   // for ORacle, MySQL - database-specific name for the encoding for the connection
	 *dbname,        // for Oracle, MySQL
	 *hostname;      // for MySQL
      
      DLLLOCAL void freeConnectionValues();
      DLLLOCAL void setConnectionValues();

   public:
      DLLEXPORT bool getAutoCommit() const;
      DLLEXPORT char *getUsername() const;
      DLLEXPORT char *getPassword() const;
      DLLEXPORT char *getDBName() const;
      DLLEXPORT char *getDBEncoding() const;
      DLLEXPORT char *getOSEncoding() const;
      DLLEXPORT char *getHostName() const;
      DLLEXPORT void *getPrivateData() const;
      DLLEXPORT void setPrivateData(void *data);
      DLLEXPORT void setDBEncoding(char *name);
      DLLEXPORT class QoreEncoding *getQoreEncoding() const;
      DLLEXPORT void setQoreEncoding(class QoreEncoding *enc);

      DLLLOCAL Datasource(DBIDriver *);
      DLLLOCAL ~Datasource();
      DLLLOCAL class QoreNode *select(class QoreString *query_str, class List *args, ExceptionSink *xsink);
      DLLLOCAL class QoreNode *selectRows(class QoreString *query_str, class List *args, ExceptionSink *xsink);
      DLLLOCAL class QoreNode *exec(class QoreString *query_str, class List *args, ExceptionSink *xsink);
      //DLLLOCAL class Hash *describe(char *table_name, ExceptionSink *xsink);
      DLLLOCAL int commit(ExceptionSink *xsink);
      DLLLOCAL int rollback(ExceptionSink *xsink);
      DLLLOCAL int open(ExceptionSink *xsink);
      DLLLOCAL int close();
      DLLLOCAL void reset(ExceptionSink *xsink);
      DLLLOCAL List *getCapabilityList() const;
      DLLLOCAL int getCapabilities() const;
      DLLLOCAL void setPendingUsername(char *u);
      DLLLOCAL void setPendingPassword(char *p);
      DLLLOCAL void setPendingDBName(char *d);
      DLLLOCAL void setPendingDBEncoding(char *c);
      DLLLOCAL void setPendingHostName(char *h);
      DLLLOCAL QoreNode *getPendingUsername() const;
      DLLLOCAL QoreNode *getPendingPassword() const;
      DLLLOCAL QoreNode *getPendingDBName() const;
      DLLLOCAL QoreNode *getPendingDBEncoding() const;
      DLLLOCAL QoreNode *getPendingHostName() const;
      DLLLOCAL void setTransactionLockTimeout(int t);
      DLLLOCAL int getTransactionLockTimeout() const;
      // returns -1 for error, 0 for OK
      DLLLOCAL int beginTransaction(class ExceptionSink *xsink);
      DLLLOCAL bool isInTransaction() const;
      DLLLOCAL void setAutoCommit(bool ac);
      DLLLOCAL bool isOpen() const;
      DLLLOCAL Datasource *copy() const;
};

#endif // _QORE_DATASOURCE_H
