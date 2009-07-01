/*
  DBI.h

  Qore Programming Language

  Copyright 2003 - 2009 David Nichols

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

#ifndef _QORE_DBI_H

#define _QORE_DBI_H

/** @file DBI.h
    described Qore's DBI interface for writing database drivers
 */

// DBI Driver capabilities
#define DBI_CAP_NONE                     0
#define DBI_CAP_CHARSET_SUPPORT          (1 << 1)
#define DBI_CAP_TRANSACTION_MANAGEMENT   (1 << 2)
#define DBI_CAP_STORED_PROCEDURES        (1 << 3)
#define DBI_CAP_LOB_SUPPORT              (1 << 4)
#define DBI_CAP_BIND_BY_VALUE            (1 << 5)
#define DBI_CAP_BIND_BY_PLACEHOLDER      (1 << 6)

#define BN_PLACEHOLDER  0
#define BN_VALUE        1

#define DBI_DEFAULT_STR_LEN 512

// DBI method codes
#define QDBI_METHOD_OPEN                      1
#define QDBI_METHOD_CLOSE                     2
#define QDBI_METHOD_SELECT                    3
#define QDBI_METHOD_SELECT_ROWS               4
#define QDBI_METHOD_EXEC                      5
#define QDBI_METHOD_COMMIT                    6
#define QDBI_METHOD_ROLLBACK                  7
#define QDBI_METHOD_BEGIN_TRANSACTION         8
#define QDBI_METHOD_ABORT_TRANSACTION_START   9
#define QDBI_METHOD_GET_SERVER_VERSION       10
#define QDBI_METHOD_GET_CLIENT_VERSION       11

#define QDBI_VALID_CODES 12

class Datasource;
class ExceptionSink;
class QoreString;
class QoreListNode;
class AbstractQoreNode;
class QoreHashNode;
class QoreNamespace;

// DBI method signatures - note that only get_client_version uses a "const Datasource" 
// the others do not so that automatic reconnects can be supported (which will normally
// require writing to the Datasource)

//! signature for the DBI "open" method - must be defined in each DBI driver
/** @param ds the Datasource for the connection
    @param xsink if any errors occur, error information should be added to this object
    @return 0 for OK, non-zero for error
 */
typedef int (*q_dbi_open_t)(Datasource *ds, ExceptionSink *xsink);

//! signature for the DBI "close" method - must be defined in each DBI driver
/** this function cannot throw an exception and currently any return error code is ignored
    @param ds the Datasource for the connection to close
    @return 0 for OK, non-zero for error
 */
typedef int (*q_dbi_close_t)(Datasource *ds);

//! signature for the DBI "select" method - must be defined in each DBI driver
/** 
    @param ds the Datasource for the connection
    @param str the SQL string to execute, may not be in the encoding of the Datasource
    @param args arguments for placeholders or DBI formatting codes in the SQL string
    @param xsink if any errors occur, error information should be added to this object
    @return the data returned by executing the SQL or 0
 */
typedef AbstractQoreNode *(*q_dbi_select_t)(Datasource *ds, const QoreString *str, const QoreListNode *args, ExceptionSink *xsink);

//! signature for the DBI "selectRows" method - must be defined in each DBI driver
/** 
    @param ds the Datasource for the connection
    @param str the SQL string to execute, may not be in the encoding of the Datasource
    @param args arguments for placeholders or DBI formatting codes in the SQL string
    @param xsink if any errors occur, error information should be added to this object
    @return the data returned by executing the SQL or 0
 */
typedef AbstractQoreNode *(*q_dbi_select_rows_t)(Datasource *ds, const QoreString *str, const QoreListNode *args, ExceptionSink *xsink);

//! signature for the DBI "execSQL" method - must be defined in each DBI driver
/** 
    @param ds the Datasource for the connection
    @param str the SQL string to execute, may not be in the encoding of the Datasource
    @param args arguments for placeholders or DBI formatting codes in the SQL string
    @param xsink if any errors occur, error information should be added to this object
    @return the data returned by executing the SQL or 0
 */
typedef AbstractQoreNode *(*q_dbi_exec_t)(Datasource *ds, const QoreString *str, const QoreListNode *args, ExceptionSink *xsink);

//! signature for the DBI "commit" method - must be defined in each DBI driver
/** 
    @param ds the Datasource for the connection
    @param xsink if any errors occur, error information should be added to this object
    @return 0 for OK, non-zero for error
 */
typedef int (*q_dbi_commit_t)(Datasource *ds, ExceptionSink *xsink);

//! signature for the DBI "rollback" method - must be defined in each DBI driver
/** 
    @param ds the Datasource for the connection
    @param xsink if any errors occur, error information should be added to this object
    @return 0 for OK, non-zero for error
 */
typedef int (*q_dbi_rollback_t)(Datasource *ds, ExceptionSink *xsink);

//! signature for the DBI "begin_transaction" method, should only be defined for drivers needing this to explicitly start a transaction 
/** 
    @param ds the Datasource for the connection
    @param xsink if any errors occur, error information should be added to this object
    @return 0 for OK, non-zero for error
 */
typedef int (*q_dbi_begin_transaction_t)(Datasource *ds, ExceptionSink *xsink);

//! signature for the rollback method to be executed when the first statement in an explicit transaction started implicitly with the DBI "begin_transaction" method fails
/** this should just be a pointer to the rollback method for those drivers that need it (ex: pgsql)
    @param ds the Datasource for the connection
    @param xsink if any errors occur, error information should be added to this object
    @return 0 for OK, non-zero for error
 */
typedef int (*q_dbi_abort_transaction_start_t)(Datasource *ds, ExceptionSink *xsink);

//! signature for the "get_server_version" method
/** 
    @param ds the Datasource for the connection
    @param xsink if any errors occur, error information should be added to this object
    @return a value describing the server's version
 */
typedef AbstractQoreNode *(*q_dbi_get_server_version_t)(Datasource *ds, ExceptionSink *xsink);

//! signature for the "get_client_version" method
/** 
    @param ds the Datasource for the connection
    @param xsink if any errors occur, error information should be added to this object
    @return a value describing the client's version
 */
typedef AbstractQoreNode *(*q_dbi_get_client_version_t)(const Datasource *ds, ExceptionSink *xsink);

typedef std::pair<int, void *> qore_dbi_method_t;

typedef safe_dslist<qore_dbi_method_t> dbi_method_list_t;

//! this is the data structure Qore DBI drivers will use to pass the supported DBI methods
/** the minimum methods that must be supported are: open, close, select, selectRows, execSQL, commit, and rollback
 */
class qore_dbi_method_list
{
   private:
      struct qore_dbi_mlist_private *priv; // private implementation

      // not implemented
      DLLLOCAL qore_dbi_method_list(const qore_dbi_method_list&);
      DLLLOCAL qore_dbi_method_list& operator=(const qore_dbi_method_list&);

   public:
      DLLEXPORT qore_dbi_method_list();
      DLLEXPORT ~qore_dbi_method_list();

      // covers open, commit, rollback, and begin transaction
      DLLEXPORT void add(int code, q_dbi_open_t method);
      // for close
      DLLEXPORT void add(int code, q_dbi_close_t method);
      // covers select, select_rows. and exec
      DLLEXPORT void add(int code, q_dbi_select_t method);
      // covers get_server_version
      DLLEXPORT void add(int code, q_dbi_get_server_version_t method);
      // covers get_client_version
      DLLEXPORT void add(int code, q_dbi_get_client_version_t method);

      // internal interface
      DLLLOCAL dbi_method_list_t *getMethods() const;
};

//! this class provides the internal link to the database driver for Qore's DBI layer
/**
   most of these functions are not exported; the Datasource class should be used 
   instead of using the DBIDriver class directly
   @see Datasource
*/
class DBIDriver {
   private:
      //! private implementation
      struct qore_dbi_private *priv;

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL DBIDriver(const DBIDriver&);
      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL DBIDriver& operator=(const DBIDriver&);

   public:
      //! this is the only public exported function available in this class
      /**
	 @return the name of the driver (ex: "oracle")
       */
      DLLEXPORT const char *getName() const;

      DLLLOCAL DBIDriver(const char *name, const dbi_method_list_t &methods, int cps);
      DLLLOCAL ~DBIDriver();
      DLLLOCAL int init(Datasource *ds, ExceptionSink *xsink);
      DLLLOCAL int close(Datasource *ds);
      DLLLOCAL AbstractQoreNode *select(Datasource *ds, const QoreString *sql, const QoreListNode *args, ExceptionSink *xsink);
      DLLLOCAL AbstractQoreNode *selectRows(Datasource *ds, const QoreString *sql, const QoreListNode *args, ExceptionSink *xsink);
      DLLLOCAL AbstractQoreNode *execSQL(Datasource *ds, const QoreString *sql, const QoreListNode *args, ExceptionSink *xsink);
      DLLLOCAL int commit(Datasource *, ExceptionSink *xsink);
      DLLLOCAL int rollback(Datasource *, ExceptionSink *xsink);
      DLLLOCAL int autoCommit(Datasource *, ExceptionSink *xsink);
      DLLLOCAL int beginTransaction(Datasource *, ExceptionSink *xsink);
      DLLLOCAL int abortTransactionStart(Datasource *, ExceptionSink *xsink);
      DLLLOCAL AbstractQoreNode *getServerVersion(Datasource *, ExceptionSink *xsink);
      DLLLOCAL AbstractQoreNode *getClientVersion(const Datasource *, ExceptionSink *xsink);

      DLLLOCAL int getCaps() const;
      DLLLOCAL QoreListNode *getCapList() const;
};

struct qore_dbi_dlist_private;

//! this class is used to register and find DBI drivers loaded in qore
/**
   this class will all use the ModuleManager to try and load a driver if it is not already loaded when find() is called
   @see ModuleManager
 */
class DBIDriverList
{
   private:
      //! private implementation
      struct qore_dbi_dlist_private *priv;

      DLLLOCAL DBIDriver *find_intern(const char *name) const;

public:
      //! registers a new DBI driver
      /**
	 @param name the name of the driver (ex: "oracle")
	 @param methods the list of methods the driver supports
	 @param caps the capabilities the driver supports
	 @return the DBIDriver object created
       */
      DLLEXPORT class DBIDriver *registerDriver(const char *name, const struct qore_dbi_method_list &methods, int caps);

      //! finds a driver, will try to load the driver using the ModuleManager if no such driver is already present
      /**
	 @param name the name of the driver to find (or load)
	 @return the DBIDriver found or 0 if not found and was not loaded
	 @see ModuleManager
       */
      DLLEXPORT DBIDriver *find(const char *name) const;

      //! finds a driver, will try to load the driver using the ModuleManager if no such driver is already present
      /** 
	 @param name the name of the driver to find (or load)
	 @param xsink Qore-language exceptions saved here if any occur
	 @return the DBIDriver found or 0 if not found and was not loaded
	 @see ModuleManager
       */
      DLLEXPORT DBIDriver *find(const char *name, ExceptionSink *xsink) const;

      DLLLOCAL DBIDriverList();
      DLLLOCAL ~DBIDriverList();
      DLLLOCAL QoreListNode *getDriverList() const;
};

//! list of DBI drivers currently reigsted by the Qore library
DLLEXPORT extern DBIDriverList DBI;

//! parses a datasource string and returns a hash of the component parts
DLLEXPORT QoreHashNode *parseDatasource(const char *ds, ExceptionSink *xsink);

//! concatenates a numeric value to the QoreString from the QoreNode
DLLEXPORT void DBI_concat_numeric(QoreString *str, const AbstractQoreNode *v);

//! concatenates a string value to the QoreString from the AbstractQoreNode
/** NOTE: no escaping is done here
    this function is most useful for table prefixes, etc in queries
*/
DLLEXPORT int DBI_concat_string(QoreString *str, const AbstractQoreNode *v, ExceptionSink *xsink);

DLLLOCAL void init_dbi_functions();
DLLLOCAL QoreNamespace *getSQLNamespace();

#endif  // _QORE_DBI_H
