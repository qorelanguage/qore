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

#include <qore/Qore.h>
#include <qore/DBI.h>

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
      DLLLOCAL inline MyResult(MYSQL_RES *res)
      {
	 field = mysql_fetch_fields(res);
	 num_fields = mysql_num_fields(res);
	 mysql_free_result(res);

	 bindbuf = NULL;
	 bi = NULL;
      }

      DLLLOCAL inline ~MyResult()
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

      DLLLOCAL void bind(MYSQL_STMT *stmt);
      DLLLOCAL class AbstractQoreNode *getBoundColumnValue(const QoreEncoding *csid, int i);

      DLLLOCAL inline char *getFieldName(int i)
      {
	 return field[i].name;
      }

      DLLLOCAL inline int getNumFields()
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

      DLLLOCAL void assign(const DateTime *d)
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
	    const AbstractQoreNode *value;   // value to be bound
	    class QoreStringNode *tstr;   // temporary string to be deleted
      } data;

      union my_val vbuf;
      class MyBindNode *next;

      // for value nodes
      DLLLOCAL inline MyBindNode(const AbstractQoreNode *v)
      {
	 bindtype = BN_VALUE;
	 data.value = v;
	 data.tstr = NULL;
	 next = NULL;
      }

      DLLLOCAL inline ~MyBindNode()
      {
	 if (data.tstr)
	    data.tstr->deref();
      }
     
      DLLLOCAL int bindValue(const QoreEncoding *enc, MYSQL_BIND *buf, class ExceptionSink *xsink);
};

class MyBindGroup {
   private:
      MyBindNode *head, *tail;
      QoreString *str;
      MYSQL_STMT *stmt;
      bool hasOutput;
      MYSQL_BIND *bind;
      Datasource *ds;
      class MySQLConnection *mydata;
      int len;
      class TempCharPtrStore phl;
      //bool locked;

      // returns -1 for error, 0 for OK
      DLLLOCAL inline int parse(const QoreListNode *args, class ExceptionSink *xsink);
      DLLLOCAL inline void add(class MyBindNode *c)
      {
	 len++;
	 if (!tail)
	    head = c;
	 else
	    tail->next = c;
	 tail = c;
      }

      DLLLOCAL inline class AbstractQoreNode *getOutputHash(class ExceptionSink *xsink);
      DLLLOCAL class AbstractQoreNode *execIntern(class ExceptionSink *xsink);

   public:
      DLLLOCAL MyBindGroup(Datasource *ods, const class QoreString *ostr, const QoreListNode *args, class ExceptionSink *xsink);
      DLLLOCAL ~MyBindGroup();

      DLLLOCAL inline void add(const AbstractQoreNode *v)
      {
	 add(new MyBindNode(v));
	 printd(5, "MyBindGroup::add() value=%08p\n", v);
      }

      DLLLOCAL inline void add(char *name)
      {
	 phl.push_back(name);
	 printd(5, "MyBindGroup::add() placeholder '%s' %d %s\n", name);
	 hasOutput = true;
      }
      DLLLOCAL class AbstractQoreNode *exec(class ExceptionSink *xsink);
      DLLLOCAL class AbstractQoreNode *select(class ExceptionSink *xsink);
      DLLLOCAL class AbstractQoreNode *selectRows(class ExceptionSink *xsink);
};


#endif // HAVE_MYSQL_STMT

#endif // _QORE_MYSQL_H
