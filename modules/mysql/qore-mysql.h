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

class MyBindNode {
   public:
      class MyBindNode *next;

};

class MyBindGroup {
   private:
      MyBindNode *head, *tail;
      QoreString *str;

      // returns -1 for error, 0 for OK
      inline int parse(class List *args, class ExceptionSink *xsink);

   public:
      MyBindGroup(class Datasource *ods, class QoreString *ostr, class List *args, class ExceptionSink *xsink);
      inline ~MyBindGroup()
      {
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
};


#endif // HAVE_MYSQL_STMT

#endif // _QORE_MYSQL_H
