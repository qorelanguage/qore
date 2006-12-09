/*
  DBI.h

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006 David Nichols

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

// FIXME! DBI driver registration should be done with a forward-compatible mechanism

#ifndef _QORE_DBI_H

#define _QORE_DBI_H

#include <qore/common.h>
#include <qore/support.h>
#include <qore/config.h>
#include <qore/qore_thread.h>
#include <qore/List.h>

#include <stdlib.h>
#include <string.h>

#include <map>

// DBI Driver capabilities
#define DBI_CAP_NONE                     0
#define DBI_CAP_CHARSET_SUPPORT          (1 << 1)
#define DBI_CAP_TRANSACTION_MANAGEMENT   (1 << 2)
#define DBI_CAP_STORED_PROCEDURES        (1 << 3)
#define DBI_CAP_LOB_SUPPORT              (1 << 4)

#define NUM_DBI_CAPS 4

#define BN_PLACEHOLDER  0
#define BN_VALUE        1

#define DBI_DEFAULT_STR_LEN 512

DLLLOCAL void init_dbi_functions();
DLLLOCAL class Namespace *getSQLNamespace();

struct dbi_cap_hash
{
      int cap;
      char *desc;
};

extern struct dbi_cap_hash dbi_cap_list[];

DLLEXPORT class Hash *parseDatasource(char *ds, class ExceptionSink *xsink);

typedef int (*q_dbi_init_t)(class Datasource *, class ExceptionSink *xsink);
typedef int (*q_dbi_close_t)(class Datasource *);
typedef class QoreNode *(*q_dbi_select_t)(class Datasource *, class QoreString *, class List *, class ExceptionSink *xsink);
typedef class QoreNode *(*q_dbi_select_rows_t)(class Datasource *, class QoreString *, class List *, class ExceptionSink *xsink);
typedef class QoreNode *(*q_dbi_exec_t)(class Datasource *, class QoreString *, class List *args, class ExceptionSink *xsink);
typedef int (*q_dbi_commit_t)(class Datasource *, class ExceptionSink *xsink);
typedef int (*q_dbi_rollback_t)(class Datasource *, class ExceptionSink *xsink);

class DBIDriverFunctions {
   public:
      q_dbi_init_t init;
      q_dbi_close_t close;
      q_dbi_select_t select;
      q_dbi_select_rows_t selectRows;
      q_dbi_exec_t execSQL;
      q_dbi_commit_t commit;
      q_dbi_rollback_t rollback;

      DLLEXPORT DBIDriverFunctions(q_dbi_init_t p_init, q_dbi_close_t p_close, q_dbi_select_t p_select, q_dbi_select_rows_t p_selectRows,
				   q_dbi_exec_t p_execSQL, q_dbi_commit_t p_commit, q_dbi_rollback_t p_rollback)
      {
	 init = p_init;
	 close = p_close;
	 select = p_select;
	 selectRows = p_selectRows;
	 execSQL = p_execSQL;
	 commit = p_commit;
	 rollback = p_rollback;
      }
};

// it's not necessary to lock this object because it will only be written to in one thread at a time
class DBIDriver {
   private:
      DBIDriverFunctions *f;
      int caps;

   public:
      char *name;

      DLLLOCAL DBIDriver(char *name, DBIDriverFunctions *funcs, int cps);
      DLLLOCAL ~DBIDriver();
      DLLLOCAL int init(class Datasource *ds, class ExceptionSink *xsink);
      DLLLOCAL int close(class Datasource *ds);
      DLLLOCAL class QoreNode *select(class Datasource *ds, class QoreString *sql, class List *args, class ExceptionSink *xsink);
      DLLLOCAL class QoreNode *selectRows(class Datasource *ds, class QoreString *sql, class List *args, class ExceptionSink *xsink);
      DLLLOCAL class QoreNode *execSQL(class Datasource *ds, class QoreString *sql, class List *args, class ExceptionSink *xsink);
      DLLLOCAL int commit(class Datasource *, class ExceptionSink *xsink);
      DLLLOCAL int rollback(class Datasource *, class ExceptionSink *xsink);
      DLLLOCAL int getCaps() const;
      DLLLOCAL List *getCapList() const;
      DLLLOCAL char *getName() const;
};

typedef std::map<char *, class DBIDriver *, class ltstr> dbi_map_t;

class DBIDriverList : public dbi_map_t
{
public:
   DLLEXPORT class DBIDriver *registerDriver(char *name, DBIDriverFunctions *f, int caps);

   DLLLOCAL ~DBIDriverList();
   DLLLOCAL DBIDriver *find(char *name) const;
   DLLLOCAL class List *getDriverList() const;
};

extern class DBIDriverList DBI;

#endif  // _QORE_DBI_H
