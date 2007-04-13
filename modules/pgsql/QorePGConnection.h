/*
  QorePGConnection.h
  
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

#ifndef _QORE_QOREPGCONNECTION_H
#define _QORE_QOREPGCONNECTION_H

#include <qore/safe_dslist>

// necessary in order to avoid conflicts with qore's int64 type
#define HAVE_INT64

// undefine package constants to avoid extraneous warnings
#undef PACKAGE_BUGREPORT
#undef PACKAGE_NAME
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME
#undef PACKAGE_VERSION

#include <postgres.h>         // for most basic types                                            
#include <utils/nabstime.h>   // for abstime (AbsoluteTime), reltime (RelativeTime), tinterval (TimeInterval)
#include <utils/date.h>       // for date (DateADT)
#include <utils/timestamp.h>  // for interval (Interval*)
#include <storage/itemptr.h>  // for tid (ItemPointer)
#include <utils/date.h>       // for time (TimeADT), time with time zone (TimeTzADT), 
#include <utils/timestamp.h>  // for timestamp (Timestamp*)
#include <utils/numeric.h>
#include <utils/inet.h>
#include <utils/geo_decls.h>  // for point, etc
#include <catalog/pg_type.h>

#include <pgtypes_numeric.h>

#include <libpq-fe.h>

#include <map>

// missing defines for array types (from catalog/pg_type.h)
#define QPGT_CIRCLEARRAYOID       719
#define QPGT_MONEYARRAYOID        791
#define QPGT_BOOLARRAYOID         1000
#define QPGT_BYTEAARRAYOID        1001
#define QPGT_NAMEARRAYOID         1003
#define QPGT_INT2ARRAYOID         1005
#define QPGT_INT2VECTORARRAYOID   1006 
#ifdef INT4ARRAYOID
#define QPGT_INT4ARRAYOID         INT4ARRAYOID
#else
#define QPGT_INT4ARRAYOID         1007
#endif
#define QPGT_REGPROCARRAYOID      1008 
#define QPGT_TEXTARRAYOID         1009
#define QPGT_OIDARRAYOID          1028
#define QPGT_TIDARRAYOID          1010
#define QPGT_XIDARRAYOID          1011
#define QPGT_CIDARRAYOID          1012
#define QPGT_OIDVECTORARRAYOID    1013
#define QPGT_BPCHARARRAYOID       1014
#define QPGT_VARCHARARRAYOID      1015
#define QPGT_INT8ARRAYOID         1016
#define QPGT_POINTARRAYOID        1017
#define QPGT_LSEGARRAYOID         1018
#define QPGT_PATHARRAYOID         1019
#define QPGT_BOXARRAYOID          1020
#define QPGT_FLOAT4ARRAYOID       1021
#define QPGT_FLOAT8ARRAYOID       1022
#define QPGT_ABSTIMEARRAYOID      1023
#define QPGT_RELTIMEARRAYOID      1024
#define QPGT_TINTERVALARRAYOID    1025
#define QPGT_POLYGONARRAYOID      1027
#define QPGT_ACLITEMARRAYOID      1034
#define QPGT_MACADDRARRAYOID      1040
#define QPGT_INETARRAYOID         1041
#define QPGT_CIDRARRAYOID         651
#define QPGT_TIMESTAMPARRAYOID    1115
#define QPGT_DATEARRAYOID         1182
#define QPGT_TIMEARRAYOID         1183
#define QPGT_TIMESTAMPTZARRAYOID  1185 
#define QPGT_INTERVALARRAYOID     1187
#define QPGT_NUMERICARRAYOID      1231
#define QPGT_TIMETZARRAYOID       1270
#define QPGT_BITARRAYOID          1561
#define QPGT_VARBITARRAYOID       1563
#define QPGT_REFCURSORARRAYOID    2201
#define QPGT_REGPROCEDUREARRAYOID 2207
#define QPGT_REGOPERARRAYOID      2208
#define QPGT_REGOPERATORARRAYOID  2209
#define QPGT_REGCLASSARRAYOID     2210
#define QPGT_REGTYPEARRAYOID      2211
#define QPGT_ANYARRAYOID          2277


// NOTE: this seems to be the binary format for inet/cidr data from PGSQL
// however I can't find this definition anywhere in the header files!!!
// server/utils/inet.h has a different definition
struct qore_pg_inet_struct {
      unsigned char family;      // PGSQL_AF_INET or PGSQL_AF_INET6
      unsigned char bits;        // number of bits in netmask
      unsigned char type;        // 0 = inet, 1 = cidr */
      unsigned char length;      // length of the following field (NOTE: I added this field)
      unsigned char ipaddr[16];  // up to 128 bits of address
};

struct qore_pg_tuple_id {
      unsigned int block;
      unsigned short index;
};

struct qore_pg_bit {
      unsigned int size;
      unsigned char data[1];
};

struct qore_pg_tinterval {
      int status, t1, t2;
};

struct qore_pg_array_info {
      int dim;
      int lBound;
};

struct qore_pg_array_header {
      int ndim, flags, oid;
      struct qore_pg_array_info info[1];
};

struct qore_pg_numeric {
      short ndigits;
      short weight;
      short sign;
      short dscale;
      unsigned short digits[1];
};

union qore_pg_time {
      int64 i;
      double f;
};

struct qore_pg_interval {
      qore_pg_time time;
      union {
	    int month;
	    struct {
		  int day;
		  int month;
	    } with_day;
      } rest;
};

/*
typedef void (*qore_pg_bind_func_t)(class QoreNode *v, char *data);
typedef int (*qore_pg_bind_size_func_t)(class QoreNode *v);

struct qore_bind_info {
      qore_pg_bind_func func;
      qore_pg_bind_size_func_t size_func;
      int size;
      bool in_place;
};

typedef std::map<QoreType *, qore_bind_info> qore_pg_bind_map;
*/

typedef class QoreNode *(*qore_pg_data_func_t)(char *data, int type, int size, class QorePGConnection *conn, class QoreEncoding *enc);

typedef std::map<int, qore_pg_data_func_t> qore_pg_data_map_t;
typedef std::pair<int, qore_pg_data_func_t> qore_pg_array_data_info_t;
typedef std::map<int, qore_pg_array_data_info_t> qore_pg_array_data_map_t;
typedef std::map<int, int> qore_pg_array_type_map_t;

static inline void assign_point(Point &p, Point *raw)
{
   p.x = MSBf8(raw->x);
   p.y = MSBf8(raw->y);
};

class QorePGConnection
{
   private:
      PGconn *pc;
      bool interval_has_day, integer_datetimes;

   public:
      DLLLOCAL QorePGConnection(const char *str, class ExceptionSink *xsink);
      DLLLOCAL ~QorePGConnection();
      DLLLOCAL int setPGEncoding(const char *enc, class ExceptionSink *xsink);

      DLLLOCAL int commit(class Datasource *ds, ExceptionSink *xsink);
      DLLLOCAL int rollback(class Datasource *ds, ExceptionSink *xsink);
      DLLLOCAL class QoreNode *select(class Datasource *ds, QoreString *qstr, class List *args, class ExceptionSink *xsink);
      DLLLOCAL class QoreNode *select_rows(class Datasource *ds, QoreString *qstr, class List *args, class ExceptionSink *xsink);
      DLLLOCAL class QoreNode *exec(class Datasource *ds, QoreString *qstr, class List *args, class ExceptionSink *xsink);
      DLLLOCAL int begin_transaction(class Datasource *ds, ExceptionSink *xsink);
      DLLLOCAL bool has_interval_day() const { return interval_has_day; }
      DLLLOCAL bool has_integer_datetimes() const { return integer_datetimes; }
      DLLLOCAL int get_server_version() const;
};

union parambuf {
      short i2;
      int i4;
      int64 i8;
      double f8;
      char *str;
      void *ptr;
      qore_pg_interval iv;

      DLLLOCAL void assign(short i)
      {
	 i2 = htons(i);
      }
      DLLLOCAL void assign(int i)
      {
	 i4 = htonl(i);
      }
      DLLLOCAL void assign(int64 i)
      {
	 i8 = i8MSB(i);
      }
      DLLLOCAL void assign(double f)
      {
	 f8 = f8MSB(f);
      }
};

class parambuf_list_t : public safe_dslist<parambuf *>
{
   public:
      DLLLOCAL ~parambuf_list_t()
      {
	 std::for_each(begin(), end(), simple_delete<parambuf>());
      }
};

class QorePGResult {
   private:
      static qore_pg_data_map_t data_map;
      static qore_pg_array_data_map_t array_data_map;

      PGresult *res;
      int nParams, allocated;
      Oid *paramTypes;
      char **paramValues;
      int *paramLengths, *paramFormats;
      int *paramArray;
      parambuf_list_t parambuf_list;
      class QorePGConnection *conn;
      class QoreEncoding *enc;

      DLLLOCAL class QoreNode *getNode(int row, int col, class ExceptionSink *xsink);
      // returns 0 for OK, -1 for error
      DLLLOCAL int parse(class QoreString *str, class List *args, class ExceptionSink *xsink);
      DLLLOCAL int add(class QoreNode *v, class ExceptionSink *xsink);
      DLLLOCAL class QoreNode *getArray(int type, qore_pg_data_func_t func, char *&array_data, int current, int ndim, int dim[]);
      //DLLLOCAL int bind();

   public:
      static qore_pg_array_type_map_t array_type_map;

      DLLLOCAL QorePGResult(class QorePGConnection *r_conn, class QoreEncoding *r_enc);
      DLLLOCAL ~QorePGResult();

      // returns 0 for OK, -1 for error
      DLLLOCAL int exec(PGconn *pc, class QoreString *str, class List *args, class ExceptionSink *xsink);
      // returns 0 for OK, -1 for error
      DLLLOCAL int exec(PGconn *pc, const char *cmd, class ExceptionSink *xsink);
      DLLLOCAL class Hash *getHash(class ExceptionSink *xsink);
      DLLLOCAL class List *getList(class ExceptionSink *xsink);
      DLLLOCAL int rowsAffected();
      DLLLOCAL bool hasResultData();
      DLLLOCAL bool checkIntegerDateTimes(PGconn *pc, class ExceptionSink *xsink);

      // static functions
      static DLLLOCAL void static_init();
};

class QorePGBindArray {
   private:
      int ndim, size, allocated, elements;
      int dim[MAXDIM];
      char *ptr;
      qore_pg_array_header *hdr;
      QoreType *type;
      int oid, arrayoid, format;
      class QorePGConnection *conn;

      // returns -1 for exception, 0 for OK
      DLLLOCAL int check_type(class QoreNode *n, class ExceptionSink *xsink);
      // returns -1 for exception, 0 for OK
      DLLLOCAL int check_oid(Hash *h, class ExceptionSink *xsink);
      // returns -1 for exception, 0 for OK
      DLLLOCAL int new_dimension(List *l, int current, class ExceptionSink *xsink);
      // returns -1 for exception, 0 for OK
      DLLLOCAL int process_list(List *l, int current, class QoreEncoding *enc, class ExceptionSink *xsink);
      // returns -1 for exception, 0 for OK
      DLLLOCAL int bind(class QoreNode *n, class QoreEncoding *enc, class ExceptionSink *xsink);
      DLLLOCAL void check_size(int size);

   public:
      DLLLOCAL QorePGBindArray(class QorePGConnection *r_conn);
      DLLLOCAL ~QorePGBindArray();
      // returns -1 for exception, 0 for OK
      DLLLOCAL int create_data(class List *l, int current, class QoreEncoding *enc, class ExceptionSink *xsink);
      DLLLOCAL int getOid() const;
      DLLLOCAL int getArrayOid() const;
      DLLLOCAL int getSize() const;
      DLLLOCAL int getFormat() const { return format; }
      DLLLOCAL qore_pg_array_header *getHeader();
};

#endif
