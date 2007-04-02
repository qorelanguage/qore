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

#include <qore/common.h>
#include <qore/support.h>
#include <qore/qore_thread.h>
#include <qore/List.h>
#include <qore/safe_dslist>
#include <qore/Datasource.h>

#include <stdlib.h>
#include <string.h>

// DBI Driver capabilities
#define DBI_CAP_NONE                     0
#define DBI_CAP_CHARSET_SUPPORT          (1 << 1)
#define DBI_CAP_TRANSACTION_MANAGEMENT   (1 << 2)
#define DBI_CAP_STORED_PROCEDURES        (1 << 3)
#define DBI_CAP_LOB_SUPPORT              (1 << 4)

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

#define QDBI_VALID_CODES 11

// DBI method signatures
typedef int (*q_dbi_open_t)(class Datasource *, class ExceptionSink *xsink);
typedef int (*q_dbi_close_t)(class Datasource *);
typedef class QoreNode *(*q_dbi_select_t)(class Datasource *, class QoreString *, class List *, class ExceptionSink *xsink);
typedef class QoreNode *(*q_dbi_select_rows_t)(class Datasource *, class QoreString *, class List *, class ExceptionSink *xsink);
typedef class QoreNode *(*q_dbi_exec_t)(class Datasource *, class QoreString *, class List *args, class ExceptionSink *xsink);
typedef int (*q_dbi_commit_t)(class Datasource *, class ExceptionSink *xsink);
typedef int (*q_dbi_rollback_t)(class Datasource *, class ExceptionSink *xsink);
typedef int (*q_dbi_begin_transaction_t)(class Datasource *, class ExceptionSink *xsink);
typedef int (*q_dbi_auto_commit_t)(class Datasource *, class ExceptionSink *xsink);
typedef int (*q_dbi_abort_transaction_start_t)(class Datasource *, class ExceptionSink *xsink);

typedef std::pair<int, void *> qore_dbi_method_t;

typedef safe_dslist<qore_dbi_method_t> dbi_method_list_t;

class qore_dbi_method_list : public dbi_method_list_t
{
public:
   // covers open, commit, rollback, and begin transaction
   DLLEXPORT void add(int code, q_dbi_open_t method)
   {
      push_back(std::make_pair(code, (void *)method));
   }
   // for close
   DLLEXPORT void add(int code, q_dbi_close_t method)
   {
      push_back(std::make_pair(code, (void *)method));
   }
   // covers select, select_rows. and exec
   DLLEXPORT void add(int code, q_dbi_select_t method)
   {
      push_back(std::make_pair(code, (void *)method));
   }
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
      }
};

class DBIDriver {
   private:
      DBIDriverFunctions f;
      int caps;
      const char *name;

   public:
      DLLLOCAL DBIDriver(const char *name, dbi_method_list_t &methods, int cps);
      DLLLOCAL ~DBIDriver();
      DLLLOCAL int init(class Datasource *ds, class ExceptionSink *xsink);
      DLLLOCAL int close(class Datasource *ds);
      DLLLOCAL class QoreNode *select(class Datasource *ds, class QoreString *sql, class List *args, class ExceptionSink *xsink);
      DLLLOCAL class QoreNode *selectRows(class Datasource *ds, class QoreString *sql, class List *args, class ExceptionSink *xsink);
      DLLLOCAL class QoreNode *execSQL(class Datasource *ds, class QoreString *sql, class List *args, class ExceptionSink *xsink);
      DLLLOCAL int commit(class Datasource *, class ExceptionSink *xsink);
      DLLLOCAL int rollback(class Datasource *, class ExceptionSink *xsink);
      DLLLOCAL int beginTransaction(class Datasource *, class ExceptionSink *xsink);
      DLLLOCAL int autoCommit(class Datasource *, class ExceptionSink *xsink);
      DLLLOCAL int abortTransactionStart(class Datasource *, class ExceptionSink *xsink);

      DLLLOCAL int getCaps() const;
      DLLLOCAL class List *getCapList() const;
      DLLEXPORT const char *getName() const;
};

typedef safe_dslist<class DBIDriver *> dbi_list_t;

// it's not necessary to lock this object because it will only be written to in one thread at a time
// note that a safe_dslist is used because it can be safely read in multiple threads while
// being written to (in the lock).  The list should never be that long so the penalty for searching
// a linked list with strcmp() against using a hash with explicit locking around all searches
// should be acceptable...
class DBIDriverList : public dbi_list_t
{
   DLLEXPORT DBIDriver *find_intern(const char *name) const;

public:
   DLLEXPORT class DBIDriver *registerDriver(const char *name, dbi_method_list_t &methods, int caps);
   DLLEXPORT DBIDriver *find(const char *name) const;

   DLLLOCAL ~DBIDriverList();
   DLLLOCAL class List *getDriverList() const;
};

DLLEXPORT extern class DBIDriverList DBI;
DLLEXPORT class Hash *parseDatasource(const char *ds, class ExceptionSink *xsink);

DLLLOCAL void init_dbi_functions();
DLLLOCAL class Namespace *getSQLNamespace();

#endif  // _QORE_DBI_H
