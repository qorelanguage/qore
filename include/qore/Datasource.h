/*
  Datasource.h

  Qore Programming Language
 
  Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols
 
  The Datasource class provides the low-level interface to Qore DBI drivers.
  
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

#include <qore/QoreThreadLock.h>

#include <string>

//! the base class for accessing databases in Qore through a Qore DBI driver
/** This class is not thread-safe or even thread-aware.  Thread safety and thread
    resource management is implemented in higher-level classes such as ManagedDatasource
    and DatasourcePool (classes are currently internal)

    Two copies of connection values are kept in case the values are changed while a
    connection is in use.
    @see DBIDriver
 */
class Datasource
{
   private:
      struct qore_ds_private *priv; // private implementation

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL Datasource(const Datasource&);

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL Datasource& operator=(const Datasource&);
      
   protected:
      //! frees all connection values
      DLLEXPORT void freeConnectionValues();
      
      //! copies pending values to current values
      DLLEXPORT void setConnectionValues();

      //! sets the transaction status
      DLLEXPORT void setTransactionStatus(bool);

      //! copies all pending connection values to another Datasource
      DLLEXPORT void setPendingConnectionValues(const Datasource *other);

      //! calls the "begin_implicit_transaction" DBI method if it exists
      DLLEXPORT int beginImplicitTransaction(class ExceptionSink *xsink);

   public:
      //! creates the object and binds it to a particular DBIDriver
      /**
	 @param driver the DBIDriver object to use for the connection
       */
      DLLEXPORT Datasource(class DBIDriver *driver);

      //! the Datasource is closed if it's still open and the object is destroyed
      DLLEXPORT virtual ~Datasource();

      //! returns the autocommit status
      /**
	 @return the autocommit status of the object
       */
      DLLEXPORT bool getAutoCommit() const;

      //! returns the username used for the last connection
      /**
	 @return
       */
      DLLEXPORT const char *getUsername() const;

      //! returns the password used for the last connection
      /**
	 @return the password used for the last connection (or 0 if none)
       */
      DLLEXPORT const char *getPassword() const;

      //! returns the database (or schema) name used for the last connection
      /**
	 @return the database (or schema) name used for the last connection (or 0 if none)
       */
      DLLEXPORT const char *getDBName() const;

      //! returns the database-specific character encoding name used for the last connection
      /**
	 @return the database-specific character encoding name used for the last connection
       */
      DLLEXPORT const char *getDBEncoding() const;

      //! returns the OS (or Qore) character encoding name used for the last connection
      /**
	 @return the OS (or Qore) character encoding name used for the last connection
       */
      DLLEXPORT const char *getOSEncoding() const;

      //! returns the host name used for the last connection
      /**
	 @return the host name used for the last connection (or 0 if none)
       */
      DLLEXPORT const char *getHostName() const;

      //! returns the private DBI-specific data structure for this object
      DLLEXPORT void *getPrivateData() const;

      //! sets the private DBI-specific data structure for this object
      /** this should only be called once in the actual DBI driver code
	  @param data the data for the DBI driver that holds the driver-specific state of the connection
       */
      DLLEXPORT void setPrivateData(void *data);

      //! sets the database-specific character encoding name used for the current connection
      /** this function should only be called by the DBI driver when a connection is established
	  @param name the database-specific character encoding name used for the current connection
       */
      DLLEXPORT void setDBEncoding(const char *name);

      //! returns the QoreEncoding pointer used for this connection
      DLLEXPORT const class QoreEncoding *getQoreEncoding() const;

      //! sets the QoreEncoding used for this connection
      /** this function should only be called by the DBI driver when a connection is established
	  @param enc the QoreEncoding used for the current connection
       */
      DLLEXPORT void setQoreEncoding(const class QoreEncoding *enc);

      //! sets the name for the QoreEncoding used for this connection
      /** this function should only be called by the DBI driver when a connection is established
	  @param name the name for the QoreEncoding used for the current connection
       */
      DLLEXPORT void setQoreEncoding(const char *name);

      //! sets the username to be used for the next connection
      /**
	 @param u the username to be used for the next connection
       */
      DLLEXPORT void setPendingUsername(const char *u);

      //! sets the password to be used for the next connection
      /**
	 @param p the password to be used for the next connection
       */
      DLLEXPORT void setPendingPassword(const char *p);

      //! sets the database (or schema) name to be used for the next connection
      /**
	 @param d the database (or schema) name to be used for the next connection
       */
      DLLEXPORT void setPendingDBName(const char *d);

      //! sets the database-specific name of the character-encoding  to be used for the next connection
      /**
	 @param c the database-specific name of the character-encoding to be used for the next connection
       */
      DLLEXPORT void setPendingDBEncoding(const char *c);

      //! sets the hostname to be used for the next connection
      /**
	 @param h the hostname to be used for the next connection
       */
      DLLEXPORT void setPendingHostName(const char *h);

      DLLEXPORT void setAutoCommit(bool ac);

      //! opens a connection to the database
      /** calls the DBI driver's "open" method
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLEXPORT int open(ExceptionSink *xsink);

      //! executes SQL throught the "select" function of the DBI driver and returns the result, makes an implicit connection if necessary
      /** this function is not "const" to allow for implicit connections (and reconnections)
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLEXPORT class AbstractQoreNode *select(const class QoreString *query_str, const QoreListNode *args, ExceptionSink *xsink);

      //! executes SQL throught the "selectRows" function of the DBI driver and returns the result, makes an implicit connection if necessary
      /** this function is not "const" to allow for implicit connections (and reconnections)
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLEXPORT class AbstractQoreNode *selectRows(const class QoreString *query_str, const QoreListNode *args, ExceptionSink *xsink);

      //! executes SQL throught the "exec" function of the DBI driver and returns the result, makes an implicit connection if necessary
      /** The "in_transaction" flag will be set to true if this method executes without
	  throwing an exception and the object was not already in a transaction.
	  this function is not "const" to allow for implicit connections (and reconnections)
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLEXPORT class AbstractQoreNode *exec(const class QoreString *query_str, const QoreListNode *args, ExceptionSink *xsink);

      //! commits the current transaction to the database
      /** Calls the DBI driver's "commit" method.
	  this function is not "const" to allow for implicit connections (and reconnections)
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLEXPORT int commit(ExceptionSink *xsink);

      //! rolls back the current transaction to the database
      /** Calls the DBI driver's "rollback" method.
	  this function is not "const" to allow for implicit connections (and reconnections)
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLEXPORT int rollback(ExceptionSink *xsink);

      //! closes the connection
      DLLEXPORT int close();

      //! closes and opens the connection
      DLLEXPORT void reset(ExceptionSink *xsink);

      //! returns a QoreListNode object of all capability strings of the current driver, the caller owns the reference count in the object returned
      /**
	 @return list of capability strings of the driver
       */
      DLLEXPORT QoreListNode *getCapabilityList() const;

      //! returns the capability mask of the current driver
      /**
	 @return the capability mask of the current driver
       */
      DLLEXPORT int getCapabilities() const;

      //! returns the pending username for the next connection
      /** caller owns the AbstractQoreNode pointer's reference count returned
       */
      DLLEXPORT QoreStringNode *getPendingUsername() const;

      //! returns the pending password for the next connection
      /** caller owns the AbstractQoreNode pointer's reference count returned
       */
      DLLEXPORT QoreStringNode *getPendingPassword() const;

      //! returns the pending database (or schema) name for the next connection
      /** caller owns the AbstractQoreNode pointer's reference count returned
       */
      DLLEXPORT QoreStringNode *getPendingDBName() const;

      //! returns the pending database-specific character encoding name for the next connection
      /** caller owns the AbstractQoreNode pointer's reference count returned
       */
      DLLEXPORT QoreStringNode *getPendingDBEncoding() const;

      //! returns the pending host name for the next connection
      /** caller owns the AbstractQoreNode pointer's reference count returned
       */
      DLLEXPORT QoreStringNode *getPendingHostName() const;

      /** sets the "in_transaction" flag to true if autocommit is not set
	  throws an exception if autocommit is true
	  @param xsink if an error occurs, the Qore-language exception information will be added here
	  @return -1 for error, 0 for OK
      */
      DLLEXPORT int beginTransaction(class ExceptionSink *xsink);

      //! returns the transaction status
      /**
	 @return the transaction status
      */
      DLLEXPORT bool isInTransaction() const;

      //! returns true if the connection is currently open
      /**
	 @return the connection status (true if open)
      */
      DLLEXPORT bool isOpen() const;

      //! returns a copy of this object with the same DBIDriver and pending connection values
      /**
	 return a copy of this object
       */
      DLLEXPORT Datasource *copy() const;

      //! returns the name of the current DBI driver
      DLLEXPORT const char *getDriverName() const;

      //! executes the "get_server_version" function of the driver, if any, and returns the result, makes an implicit connection if necessary
      /** the caller owns the AbstractQoreNode pointer's reference count returned (if the pointer is not 0)
	  this function is not "const" to allow for implicit connections (and reconnections)
	  @param xsink if an error occurs, the Qore-language exception information will be added here
      */
      DLLEXPORT class AbstractQoreNode *getServerVersion(class ExceptionSink *xsink);

      //! executes the "get_client_version" function of the driver, if any, and returns the result
      /** the caller owns the AbstractQoreNode pointer's reference count returned (if the pointer is not 0)
	  @param xsink if an error occurs, the Qore-language exception information will be added here
      */
      DLLEXPORT class AbstractQoreNode *getClientVersion(class ExceptionSink *xsink) const;

      //! returns the DBIDriver pointer used for this object
      /**
	 @param the DBIDriver pointer used for this object
       */
      DLLEXPORT const class DBIDriver *getDriver() const;
};

#endif // _QORE_DATASOURCE_H
