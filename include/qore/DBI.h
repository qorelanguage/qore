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

#ifndef _QORE_DBI_H

#define _QORE_DBI_H

#include <qore/common.h>
#include <qore/support.h>
#include <qore/config.h>
#include <qore/thread.h>
#include <qore/List.h>

#include <stdlib.h>
#include <string.h>

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

void init_dbi_functions();
class Namespace *getSQLNamespace();

struct dbi_cap_hash
{
      int cap;
      char *desc;
};

extern struct dbi_cap_hash dbi_cap_list[];

class Hash *parseDatasource(char *ds, class ExceptionSink *xsink);

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

      DBIDriverFunctions(q_dbi_init_t p_init, q_dbi_close_t p_close, q_dbi_select_t p_select, q_dbi_select_rows_t p_selectRows,
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

class DBIDriver {
   private:
      DBIDriverFunctions *f;
      int caps;

   public:
      char *name;
      inline DBIDriver(char *name, DBIDriverFunctions *funcs, int cps);
      inline ~DBIDriver();
      inline int init(class Datasource *ds, class ExceptionSink *xsink);
      inline int close(class Datasource *ds);
      inline class QoreNode *select(class Datasource *ds, class QoreString *sql, class List *args, class ExceptionSink *xsink);
      inline class QoreNode *selectRows(class Datasource *ds, class QoreString *sql, class List *args, class ExceptionSink *xsink);
      inline class QoreNode *execSQL(class Datasource *ds, class QoreString *sql, class List *args, class ExceptionSink *xsink);
      inline int commit(class Datasource *, class ExceptionSink *xsink);
      inline int rollback(class Datasource *, class ExceptionSink *xsink);
      inline int getCaps()
      {
	 return caps;
      }
      inline List *getCapList()
      {
	 List *l = new List();
	 for (int i = 0; i < NUM_DBI_CAPS; i++)
	    if (caps & dbi_cap_list[i].cap)
	       l->push(new QoreNode(dbi_cap_list[i].desc));
	 return l;
      }
      DBIDriver *next;
};

class DBIDriverList {
   private:
      class DBIDriver *head, *tail;
      
   public:
      inline DBIDriverList()
      {
	 head = NULL;
      }
      inline ~DBIDriverList()
      {
	 DBIDriver *w = head;
	 while (w)
	 {
	    DBIDriver *n = w->next;
	    delete w;
	    w = n;
	 }
      }
      inline DBIDriver *find(char *name)
      {
	 DBIDriver *w = head;
	 
	 while (w)
	 {
	    //printd(5, "find(%s) %08p=%s (next=%08p)\n", name, w, w->name, w->next); 
	    if (!strcmp(name, w->name))
	       break;
	    w = w->next;
	 }
	 return w;
      }
      class DBIDriver *registerDriver(char *name, DBIDriverFunctions *f, int caps)
      {
	 DBIDriver *w = find(name);
	 if (w)
	 {
#ifdef DEBUG
	    run_time_error("ERROR: database driver already registered \"%s\"", name);
#endif
	    return NULL;
	 }
	 DBIDriver *dd = new DBIDriver(name, f, caps);
	 dd->next = head;
	 head = dd;
	 return dd;
      }
      class List *getDriverList()
      {
	 if (!head)
	    return NULL;

	 class List *l = new List();

	 DBIDriver *w = head;
	 while (w)
	 {
	    l->push(new QoreNode(w->name));
	    w = w->next;
	 }
	 return l;
      }
};

inline DBIDriver::DBIDriver(char *nme, DBIDriverFunctions *funcs, int cps)
{
   name = nme;
   f = funcs;   
   caps = cps;
}

inline DBIDriver::~DBIDriver()
{
   delete f;
}

inline int DBIDriver::init(class Datasource *ds, class ExceptionSink *xsink)
{
   return f->init(ds, xsink);
}

inline int DBIDriver::close(class Datasource *ds)
{
   return f->close(ds);
}

inline class QoreNode *DBIDriver::select(class Datasource *ds, class QoreString *sql, class List *args, class ExceptionSink *xsink)
{
   return f->select(ds, sql, args, xsink);
}

inline class QoreNode *DBIDriver::selectRows(class Datasource *ds, class QoreString *sql, class List *args, class ExceptionSink *xsink)
{
   return f->selectRows(ds, sql, args, xsink);
}

inline class QoreNode *DBIDriver::execSQL(class Datasource *ds, class QoreString *sql, class List *args, class ExceptionSink *xsink)
{
   return f->execSQL(ds, sql, args, xsink);
}

inline int DBIDriver::commit(class Datasource *ds, class ExceptionSink *xsink)
{
   return f->commit(ds, xsink);
}

inline int DBIDriver::rollback(class Datasource *ds, class ExceptionSink *xsink)
{
   return f->rollback(ds, xsink);
}

extern class DBIDriverList DBI;

#endif  // _QORE_DBI_H
