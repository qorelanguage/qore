/*
  Datasource.h

  Qore Programming Language
 
  Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols
 
  The Datasource class provides the low-level interface to Qore DBI drivers.
 
  NOTE that this class is *not* thread-safe.  To use this class in a multi-
  threaded context, per-thread connection locking must be done at a level
  above this class...
 
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

#ifndef _QORE_DATASOURCE_H

#define _QORE_DATASOURCE_H

#include <qore/LockedObject.h>

#include <string>

class Datasource
{
   protected:
      bool in_transaction;
      bool isopen;
      bool autocommit;
      class DBIDriver *dsl;
      class QoreEncoding *qorecharset;
      void *private_data;               // driver private data per connection
      
      // for pending connection values
      std::string p_username,  // for Oracle, MySQL
	 p_password,    // for Oracle, MySQL
	 p_dbname,      // for Oracle, MySQL
	 p_db_encoding, // for Oracle, MySQL - database-specific name for the encoding for the connection
	 p_hostname;    // for MySQL

      // actual connection values set by init() before the datasource is opened
      std::string username,    // for Oracle, MySQL
	 password,      // for Oracle, MySQL
	 db_encoding,   // for ORacle, MySQL - database-specific name for the encoding for the connection
	 dbname,        // for Oracle, MySQL
	 hostname;      // for MySQL
      
      DLLLOCAL void freeConnectionValues();
      DLLLOCAL void setConnectionValues();

   public:
      DLLLOCAL Datasource(DBIDriver *);
      DLLLOCAL virtual ~Datasource();
      DLLLOCAL bool getAutoCommit() const;
      DLLLOCAL const char *getUsername() const;
      DLLLOCAL const char *getPassword() const;
      DLLLOCAL const char *getDBName() const;
      DLLLOCAL const char *getDBEncoding() const;
      DLLLOCAL const char *getOSEncoding() const;
      DLLLOCAL const char *getHostName() const;
      DLLLOCAL void *getPrivateData() const;
      DLLLOCAL void setPrivateData(void *data);
      DLLLOCAL void setDBEncoding(const char *name);
      DLLLOCAL class QoreEncoding *getQoreEncoding() const;
      DLLLOCAL void setQoreEncoding(class QoreEncoding *enc);
      DLLLOCAL void setQoreEncoding(const char *name);
      DLLLOCAL void setPendingUsername(const char *u);
      DLLLOCAL void setPendingPassword(const char *p);
      DLLLOCAL void setPendingDBName(const char *d);
      DLLLOCAL void setPendingDBEncoding(const char *c);
      DLLLOCAL void setPendingHostName(const char *h);
      DLLLOCAL void setAutoCommit(bool ac);
      DLLLOCAL int open(ExceptionSink *xsink);
      DLLLOCAL class QoreNode *select(class QoreString *query_str, class QoreList *args, ExceptionSink *xsink);
      DLLLOCAL class QoreNode *selectRows(class QoreString *query_str, class QoreList *args, ExceptionSink *xsink);
      DLLLOCAL class QoreNode *exec(class QoreString *query_str, class QoreList *args, ExceptionSink *xsink);
      //DLLLOCAL class QoreHash *describe(char *table_name, ExceptionSink *xsink);
      DLLLOCAL int commit(ExceptionSink *xsink);
      DLLLOCAL int rollback(ExceptionSink *xsink);
      DLLLOCAL int close();
      DLLLOCAL void reset(ExceptionSink *xsink);
      DLLLOCAL QoreList *getCapabilityList() const;
      DLLLOCAL int getCapabilities() const;
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
      DLLLOCAL bool isOpen() const;
      DLLLOCAL Datasource *copy() const;
      DLLLOCAL const char *getDriverName() const;
      DLLLOCAL class QoreNode *getServerVersion(class ExceptionSink *xsink);
      DLLLOCAL class QoreNode *getClientVersion(class ExceptionSink *xsink);

      DLLLOCAL class DBIDriver *getDriver() const;
};

#endif // _QORE_DATASOURCE_H
