/*
  qore-mysql.h

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006

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

#ifndef _QORE_MYSQL_H

#define _QORE_MYSQL_H

#include <qore/config.h>
#include <qore/Exception.h>
#include <qore/QoreString.h>
#include <qore/StringList.h>

#include <mysql.h>

#ifdef HAVE_MYSQL_STMT
class MyResult {
   private:
      MYSQL_FIELD *field;
      int num_fields;
      int type;
      MYSQL_BIND *bindbuf;
      struct bindInfo {
	    my_bool mnull;
	    long unsigned int mlen;
      } *bi;

   public:
      inline MyResult(MYSQL_RES *res)
      {
	 field = mysql_fetch_fields(res);
	 num_fields = mysql_num_fields(res);
	 mysql_free_result(res);

	 bindbuf = NULL;
	 bi = NULL;
      }

      inline ~MyResult()
      {
	 if (bindbuf)
	 {
	    // delete buffer
	    for (int i = 0; i < num_fields; i++)
	       if (bindbuf[i].buffer_type == MYSQL_TYPE_DOUBLE || bindbuf[i].buffer_type == MYSQL_TYPE_LONGLONG)
		  free(bindbuf[i].buffer);
	       else if (bindbuf[i].buffer_type == MYSQL_TYPE_STRING)
		  delete [] (char *)bindbuf[i].buffer;
	       else if (bindbuf[i].buffer_type == MYSQL_TYPE_DATETIME)
		  delete (MYSQL_TIME *)bindbuf[i].buffer;
	    delete [] bindbuf;
	 }
	 if (bi)
	    delete [] bi;
      }

      void bind(MYSQL_STMT *stmt);
      class QoreNode *getBoundColumnValue(class QoreEncoding *csid, int i);

      inline char *getFieldName(int i)
      {
	 return field[i].name;
      }

      inline int getNumFields()
      {
	 return num_fields;
      }

};

// FIXME: do not assume byte widths
union my_val {
      MYSQL_TIME time;
      int i4;
      int64 i8;
      double f8;
      void *ptr;

      void assign(class DateTime *d)
      {
	 time.year = d->getYear();
	 time.month = d->getMonth();
	 time.day = d->getDay();
	 time.hour = d->getHour();
	 time.minute = d->getMinute();
	 time.second = d->getSecond();
	 time.neg = false;
      }
};

class MyBindNode {
   private:

   public:
      int bindtype;
      unsigned long len;

      struct {
	    class QoreNode *value;   // value to be bound
	    class QoreString *tstr;   // temporary string to be deleted
      } data;

      union my_val vbuf;
      class MyBindNode *next;

      // for value nodes
      inline MyBindNode(class QoreNode *v)
      {
	 bindtype = BN_VALUE;
	 data.value = v;
	 data.tstr = NULL;
	 next = NULL;
      }

      inline ~MyBindNode()
      {
	 if (data.tstr)
	    delete data.tstr;
      }
     
      int bindValue(class QoreEncoding *enc, MYSQL_BIND *buf, class ExceptionSink *xsink);
};

class MyBindGroup {
   private:
      MyBindNode *head, *tail;
      QoreString *str;
      MYSQL_STMT *stmt;
      bool hasOutput;
      MYSQL_BIND *bind;
      MYSQL *db;
      Datasource *ds;
      int len;
      class StringList phl;

      // returns -1 for error, 0 for OK
      inline int parse(class List *args, class ExceptionSink *xsink);
      inline void add(class MyBindNode *c)
      {
	 len++;
	 if (!tail)
	    head = c;
	 else
	    tail->next = c;
	 tail = c;
      }

      inline class QoreNode *getOutputHash(class ExceptionSink *xsink);
      class QoreNode *execIntern(class ExceptionSink *xsink);

   public:
      MyBindGroup(class Datasource *ods, class QoreString *ostr, class List *args, class ExceptionSink *xsink);
      inline ~MyBindGroup()
      {
	 if (bind)
	    delete [] bind;

	 if (stmt)
	    mysql_stmt_close(stmt);

	 if (str)
	    delete str;

	 class MyBindNode *w = head;
	 while (w)
	 {
	    head = w->next;
	    delete w;
	    w = head;
	 }
      }

      inline void add(class QoreNode *v)
      {
	 add(new MyBindNode(v));
	 printd(5, "MyBindGroup::add() value=%08p\n", v);
      }

      inline void add(char *name)
      {
	 phl.append(name);
	 printd(5, "MyBindGroup::add() placeholder '%s' %d %s\n", name);
	 hasOutput = true;
      }
      class QoreNode *exec(class ExceptionSink *xsink);
      class QoreNode *select(class ExceptionSink *xsink);
      class QoreNode *selectRows(class ExceptionSink *xsink);
};


#endif // HAVE_MYSQL_STMT

#endif // _QORE_MYSQL_H
