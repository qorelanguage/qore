/*
  DBI.h

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols

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
#define QDBI_METHOD_AUTO_COMMIT               9
#define QDBI_METHOD_ABORT_TRANSACTION_START  10
#define QDBI_METHOD_GET_SERVER_VERSION       11
#define QDBI_METHOD_GET_CLIENT_VERSION       12

#define QDBI_VALID_CODES 13

// DBI method signatures - note that only get_client_version uses a "const Datasource" 
// the others do not so that automatic reconnects can be supported (which will normally
// require writing to the Datasource)
typedef int (*q_dbi_open_t)(class Datasource *, class ExceptionSink *xsink);
typedef int (*q_dbi_close_t)(class Datasource *);
typedef class AbstractQoreNode *(*q_dbi_select_t)(class Datasource *, const class QoreString *, const class QoreListNode *, class ExceptionSink *xsink);
typedef class AbstractQoreNode *(*q_dbi_select_rows_t)(class Datasource *, const class QoreString *, const class QoreListNode *, class ExceptionSink *xsink);
typedef class AbstractQoreNode *(*q_dbi_exec_t)(class Datasource *, const class QoreString *, const class QoreListNode *args, class ExceptionSink *xsink);
typedef int (*q_dbi_commit_t)(class Datasource *, class ExceptionSink *xsink);
typedef int (*q_dbi_rollback_t)(class Datasource *, class ExceptionSink *xsink);
typedef int (*q_dbi_begin_transaction_t)(class Datasource *, class ExceptionSink *xsink);
typedef int (*q_dbi_auto_commit_t)(class Datasource *, class ExceptionSink *xsink);
typedef int (*q_dbi_abort_transaction_start_t)(class Datasource *, class ExceptionSink *xsink);
typedef class AbstractQoreNode *(*q_dbi_get_server_version_t)(class Datasource *, class ExceptionSink *xsink);
typedef class AbstractQoreNode *(*q_dbi_get_client_version_t)(const class Datasource *, class ExceptionSink *xsink);

typedef std::pair<int, void *> qore_dbi_method_t;

typedef safe_dslist<qore_dbi_method_t> dbi_method_list_t;

struct qore_dbi_mlist_private;

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

class DBIDriverFunctions {
   public:
      q_dbi_open_t open;
      q_dbi_close_t close;
      q_dbi_select_t select;
      q_dbi_select_rows_t selectRows;
      q_dbi_exec_t execSQL;
      q_dbi_commit_t commit;
      q_dbi_rollback_t rollback;
      q_dbi_begin_transaction_t begin_transaction; // for DBI drivers that require explicit transaction starts
      q_dbi_auto_commit_t auto_commit;             // for DBI drivers that require an explicit commit 
      q_dbi_abort_transaction_start_t abort_transaction_start;  // for DBI drivers that require a rollback in order to use
							        // the connection after an exception as the first statement
							        // in a transaction
      q_dbi_get_server_version_t get_server_version;
      q_dbi_get_client_version_t get_client_version;
      
      DLLLOCAL DBIDriverFunctions()
      {
	 open = NULL;
	 close = NULL;
	 select = NULL;
	 selectRows = NULL;
	 execSQL = NULL;
	 commit = NULL;
	 rollback = NULL;
	 begin_transaction = NULL;
	 auto_commit = NULL;
	 abort_transaction_start = NULL;
	 get_server_version = NULL;
	 get_client_version = NULL;
      }
};

struct qore_dbi_private;

// most of these functions are not exported; the Datasource class should be used 
// instead of using the DBIDriver class directly
class DBIDriver {
   private:
      struct qore_dbi_private *priv;

      // not implemented
      DLLLOCAL DBIDriver(const DBIDriver&);
      DLLLOCAL DBIDriver& operator=(const DBIDriver&);

   public:
      DLLEXPORT const char *getName() const;

      DLLLOCAL DBIDriver(const char *name, const dbi_method_list_t &methods, int cps);
      DLLLOCAL ~DBIDriver();
      DLLLOCAL int init(class Datasource *ds, class ExceptionSink *xsink);
      DLLLOCAL int close(class Datasource *ds);
      DLLLOCAL class AbstractQoreNode *select(class Datasource *ds, const class QoreString *sql, const class QoreListNode *args, class ExceptionSink *xsink);
      DLLLOCAL class AbstractQoreNode *selectRows(class Datasource *ds, const class QoreString *sql, const class QoreListNode *args, class ExceptionSink *xsink);
      DLLLOCAL class AbstractQoreNode *execSQL(class Datasource *ds, const class QoreString *sql, const class QoreListNode *args, class ExceptionSink *xsink);
      DLLLOCAL int commit(class Datasource *, class ExceptionSink *xsink);
      DLLLOCAL int rollback(class Datasource *, class ExceptionSink *xsink);
      DLLLOCAL int beginTransaction(class Datasource *, class ExceptionSink *xsink);
      DLLLOCAL int autoCommit(class Datasource *, class ExceptionSink *xsink);
      DLLLOCAL int abortTransactionStart(class Datasource *, class ExceptionSink *xsink);
      DLLLOCAL class AbstractQoreNode *getServerVersion(class Datasource *, class ExceptionSink *xsink);
      DLLLOCAL class AbstractQoreNode *getClientVersion(const class Datasource *, class ExceptionSink *xsink);

      DLLLOCAL int getCaps() const;
      DLLLOCAL class QoreListNode *getCapList() const;
};

struct qore_dbi_dlist_private;

class DBIDriverList
{
   private:
      struct qore_dbi_dlist_private *priv;

      DLLLOCAL DBIDriver *find_intern(const char *name) const;

public:
      DLLEXPORT class DBIDriver *registerDriver(const char *name, const struct qore_dbi_method_list &methods, int caps);
      DLLEXPORT DBIDriver *find(const char *name) const;

      DLLLOCAL DBIDriverList();
      DLLLOCAL ~DBIDriverList();
      DLLLOCAL class QoreListNode *getDriverList() const;
};

// 
DLLEXPORT extern class DBIDriverList DBI;
DLLEXPORT class QoreHashNode *parseDatasource(const char *ds, class ExceptionSink *xsink);
// concatenates a numeric value to the QoreString from the QoreNode
DLLEXPORT void DBI_concat_numeric(class QoreString *str, const class AbstractQoreNode *v);
// concatenates a string value to the QoreString from the AbstractQoreNode, note that no escaping is done here
// this function is most useful for table prefixes, etc in queries
DLLEXPORT int DBI_concat_string(class QoreString *str, const class AbstractQoreNode *v, class ExceptionSink *xsink);

DLLLOCAL void init_dbi_functions();
DLLLOCAL class QoreNamespace *getSQLNamespace();

#endif  // _QORE_DBI_H
