/*
  QorePGConnection.cc
  
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

#include <qore/Qore.h>

#include "QorePGConnection.h"

#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>

#include <memory>

// declare static members
qore_pg_data_map_t QorePGResult::data_map;
qore_pg_array_data_map_t QorePGResult::array_data_map;
qore_pg_array_type_map_t QorePGResult::array_type_map;

// bind functions
static class QoreNode *qpg_data_bool(char *data, int type, int len, class QoreEncoding *enc)
{
   return new QoreNode(*((bool *)data));
}

static class QoreNode *qpg_data_bytea(char *data, int type, int len, class QoreEncoding *enc)
{
   void *dc = malloc(len);
   memcpy(dc, data, len);
   return new QoreNode(new BinaryObject(dc, len));
}

static class QoreNode *qpg_data_char(char *data, int type, int len, class QoreEncoding *enc)
{
   char *nstr = (char *)malloc(sizeof(char) * (len + 1));
   strncpy(nstr, (char *)data, len);
   nstr[len] = '\0';
   remove_trailing_blanks(nstr);
   class QoreString *str = new QoreString(enc);
   str->take(nstr);
   return new QoreNode(str);
}

static class QoreNode *qpg_data_int8(char *data, int type, int len, class QoreEncoding *enc)
{
   return new QoreNode(MSBi8(*((uint64_t *)data)));
}

static class QoreNode *qpg_data_int4(char *data, int type, int len, class QoreEncoding *enc)
{
   return new QoreNode((int64)ntohl(*((uint32_t *)data)));
}

static class QoreNode *qpg_data_int2(char *data, int type, int len, class QoreEncoding *enc)
{
   return new QoreNode((int64)ntohs(*((uint16_t *)data)));
}

static class QoreNode *qpg_data_text(char *data, int type, int len, class QoreEncoding *enc)
{
   return new QoreNode(new QoreString((char *)data, len, enc));
}

static class QoreNode *qpg_data_float4(char *data, int type, int len, class QoreEncoding *enc)
{
   int val = ntohl(*((uint32_t *)data));
   float *fv = (float *)&val;
   return new QoreNode((double)*fv);
}

static class QoreNode *qpg_data_float8(char *data, int type, int len, class QoreEncoding *enc)
{
   int64 val = MSBi8(*((uint64_t *)data));
   double *fv = (double *)&val;
   return new QoreNode(*fv);
}

static class QoreNode *qpg_data_abstime(char *data, int type, int len, class QoreEncoding *enc)
{
   int val = ntohl(*((uint32_t *)data));
   return new QoreNode(new DateTime((int64)val));
}

static class QoreNode *qpg_data_reltime(char *data, int type, int len, class QoreEncoding *enc)
{
   int val = ntohl(*((uint32_t *)data));
   return new QoreNode(new DateTime(0, 0, 0, 0, 0, val, 0, true));
}

static class QoreNode *qpg_data_timestamp(char *data, int type, int len, class QoreEncoding *enc)
{
#ifdef HAVE_INT64_TIMESTAMP
   int64 val = MSBi8(*((uint64_t *)data));
   
   // convert from u-secs to seconds
   val = val / 1000000000 + 10957 * 86400;
   return new QoreNode(new DateTime(val));
#else
   int64 val = MSBi8(*((uint64_t *)data));
   double *fv = (double *)&val;
   int nv = (int64)*fv;
   int ms = (int)((*fv - nv) * 1000.0);
   nv += 10957 * 86400;
   //printd(5, "time=%lld.%03d seconds\n", val, ms);
   return new QoreNode(new DateTime(nv, ms));
#endif
}

static class QoreNode *qpg_data_date(char *data, int type, int len, class QoreEncoding *enc)
{
   int val = (ntohl(*((uint32_t *)data)) + 10957) * 86400;      
   return new QoreNode(new DateTime((int64)val));
}

static class QoreNode *qpg_data_interval(char *data, int type, int len, class QoreEncoding *enc)
{
   Interval *iv = (Interval *)data;
   int ms;
   int64 val;
#ifdef HAVE_INT64_TIMESTAMP
   int64 us = MSBi8(*((uint64_t *)&iv->time));
   val = us / 10000000;
   ms = us / 1000 - (sec * 1000);
#else
   double f = MSBf8(*((double *)&iv->time));
   val = (int64)f;
   ms = (int)((f - (double)val) * 1000.0);   
   //printf("interval time=%g, day=%d, month=%d\n", val, ntohl(iv->day), ntohl(iv->month));
#endif
#if POSTGRES_VERSION_MAJOR >= 8
   return new QoreNode(new DateTime(0, ntohl(iv->month), ntohl(iv->day), 0, 0, (int64)val, ms, true));
#else
   return new QoreNode(new DateTime(0, ntohl(iv->month), 0, 0, 0, (int64)val, ms, true));
#endif
}

static class QoreNode *qpg_data_time(char *data, int type, int len, class QoreEncoding *enc)
{
#ifdef HAVE_INT64_TIMESTAMP
   int64 val = MSBi8(*((uint64_t *)data));
#else
   int64 tv = MSBi8(*((uint64_t *)data));
   double val = *((double *)&tv);
   //printf("val=%g\n", val);
#endif
   return new QoreNode(new DateTime((int64)val));
}

static class QoreNode *qpg_data_timetz(char *data, int type, int len, class QoreEncoding *enc)
{
   TimeTzADT *tm = (TimeTzADT *)data;
#ifdef HAVE_INT64_TIMESTAMP
   int64 val = MSBi8(*((uint64_t *)&tm->time));
#else
   int64 tv = MSBi8(*((uint64_t *)&tm->time));
   double val = *((double *)&tv);
   //printf("val=%g\n", val);
   // NOTE! timezone is ignored
#endif
   return new QoreNode(new DateTime((int64)val));
}

static class QoreNode *qpg_data_tinterval(char *data, int type, int len, class QoreEncoding *enc)
{
   //printd(5, "QorePGResult::getNode(row=%d, col=%d, type=%d) this=%08p len=%d\n", row, col, type, this, len);
   TimeIntervalData *td = (TimeIntervalData *)data;

   DateTime dt((int64)(int)ntohl(td->data[0]));
   class QoreString *str = new QoreString();
   str->sprintf("[\"%04d-%02d-%02d %02d:%02d:%02d\" ", dt.getYear(), dt.getMonth(), dt.getDay(), dt.getHour(), dt.getMinute(), dt.getSecond()); 
   dt.setDate((int64)ntohl(td->data[1]));
   str->sprintf("\"%04d-%02d-%02d %02d:%02d:%02d\"]", dt.getYear(), dt.getMonth(), dt.getDay(), dt.getHour(), dt.getMinute(), dt.getSecond()); 

   // NOTE: ignoring tinverval->status, it is assumed that any value here will be valid
   //printd(5, "status=%d s0=%d s1=%d\n", ntohl(td->status), s0, s1);

   return new QoreNode(str);
}

static class QoreNode *qpg_data_numeric(char *data, int type, int len, class QoreEncoding *enc)
{
   // note: we write directly to the data here
   qore_pg_numeric *nd = (qore_pg_numeric *)data;
   nd->ndigits = ntohs(nd->ndigits);
   nd->weight = ntohs(nd->weight);
   nd->sign = ntohs(nd->sign);
   nd->dscale = ntohs(nd->dscale);

   //printd(5, "(%d) ndigits=%d, weight=%d, sign=%d, dscale=%d\n", sizeof(NumericDigit), nd->ndigits, nd->weight, nd->sign, nd->dscale);
   class QoreString *str = new QoreString();
   if (nd->sign < 0)
      str->concat('-');
   for (int i = 0; i < nd->ndigits; i++)
   {
      if (i == nd->weight + 1)
	 str->concat('.');
      str->sprintf("%d", ntohs(nd->digits[i]));
      //printd(5, "digit %d: %d\n", i, ntohs(nd->digits[i]));
   }

   return new QoreNode(str);
}

static class QoreNode *qpg_data_cash(char *data, int type, int len, class QoreEncoding *enc)
{
   return new QoreNode((double)ntohl(*((uint32_t *)data)) / 100.0);
}

static class QoreNode *qpg_data_macaddr(char *data, int type, int len, class QoreEncoding *enc)
{
   class QoreString *str = new QoreString();
   for (int i = 0; i < 5; i++)
   {
      str->concatHex((char *)data + i, 1);
      str->concat(':');
   }
   str->concatHex((char *)data+5, 1);
   return new QoreNode(str);
}

static class QoreNode *qpg_data_inet(char *data, int type, int len, class QoreEncoding *enc)
{
   qore_pg_inet_struct *is = (qore_pg_inet_struct *)data;

   class QoreString *str = new QoreString();
   if (is->family == PGSQL_AF_INET)
   {
      for (int i = 0, e = is->length - 1; i < e; i++)
	 str->sprintf("%d.", is->ipaddr[i]);
      str->sprintf("%d/%d", is->ipaddr[3], is->bits);
   }
   else
   {
      short *sp;
      int i, val, e, last = 0;
      if (type == CIDROID)
	 e = is->bits / 8;
      else
	 e = is->length;
      if (e == 16)
      {
	 e -= 2;
	 last = 1;
      }
      for (i = 0; i < e; i += 2)
      {
	 sp = (short *)&is->ipaddr[i];
	 val = ntohs(*sp);
	 str->sprintf("%x:", val);
      }
      if (last)
      {
	 sp = (short *)&is->ipaddr[i];
	 val = ntohs(*sp);
	 str->sprintf("%x", val);
      }
      else
	 str->concat(':');
      str->sprintf("/%d", is->bits);
   }
   return new QoreNode(str);
}

static class QoreNode *qpg_data_tid(char *data, int type, int len, class QoreEncoding *enc)
{
   qore_pg_tuple_id *ti = (qore_pg_tuple_id *)data;
   unsigned block = ntohl(ti->block);
   unsigned index = ntohs(ti->index);
   class QoreString *str = new QoreString();
   str->sprintf("(%u,%u)", block, index);
   return new QoreNode(str);
}

static class QoreNode *qpg_data_bit(char *data, int type, int len, class QoreEncoding *enc)
{
   qore_pg_bit *bp = (qore_pg_bit *)data;
   int num = (ntohl(bp->size) - 1) / 8 + 1;
   BinaryObject *b = new BinaryObject();
   b->append(bp->data, num);
   return new QoreNode(b);
}

static class QoreNode *qpg_data_point(char *data, int type, int len, class QoreEncoding *enc)
{
   Point p;
   assign_point(p, (Point *)data);
   QoreString *str = new QoreString();
   str->sprintf("%g,%g", p.x, p.y);
   return new QoreNode(str);
}

static class QoreNode *qpg_data_lseg(char *data, int type, int len, class QoreEncoding *enc)
{
   Point p;
   assign_point(p, &((LSEG *)data)->p[0]);
   QoreString *str = new QoreString();
   str->sprintf("(%g,%g),", p.x, p.y);
   assign_point(p, &((LSEG *)data)->p[1]);
   str->sprintf("(%g,%g)", p.x, p.y);
   return new QoreNode(str);
}

// NOTE: This is functionally identical to LSEG above
static class QoreNode *qpg_data_box(char *data, int type, int len, class QoreEncoding *enc)
{
   Point p0, p1;
   assign_point(p0, &((BOX *)data)->high);
   assign_point(p1, &((BOX *)data)->low);
   QoreString *str = new QoreString();
   str->sprintf("(%g,%g),(%g,%g)", p0.x, p0.y, p1.x, p1.y);
   return new QoreNode(str);
}

static class QoreNode *qpg_data_path(char *data, int type, int len, class QoreEncoding *enc)
{
   unsigned npts = ntohl(*((int *)((char *)data + 1)));
   bool closed = ntohl(*((char *)data));
   //printd(5, "npts=%d closed=%d\n", npts, closed);
   QoreString *str = new QoreString();
   str->concat(closed ? '(' : '[');
   Point p;
   for (unsigned i = 0; i < npts; ++i)
   {
      assign_point(p, (Point *)(((char *)data) + 5 + (sizeof(Point)) * i));
      str->sprintf("(%g,%g)", p.x, p.y);
      if (i != (npts - 1))
	 str->concat(',');
   }
   str->concat(closed ? ')' : ']');
   return new QoreNode(str);
}

static class QoreNode *qpg_data_polygon(char *data, int type, int len, class QoreEncoding *enc)
{
   unsigned npts = ntohl(*((int *)data));
   QoreString *str = new QoreString('(');
   Point p;
   for (unsigned i = 0; i < npts; ++i)
   {
      assign_point(p, (Point *)(((char *)data) + 4 + (sizeof(Point)) * i));
      str->sprintf("(%g,%g)", p.x, p.y);
      if (i != (npts - 1))
	 str->concat(',');
   }
   str->concat(')');
   return new QoreNode(str);
}

static class QoreNode *qpg_data_circle(char *data, int type, int len, class QoreEncoding *enc)
{
   //printd(5, "QorePGResult::getNode(row=%d, col=%d, type=%d) this=%08p len=%d\n", row, col, type, this, len);
   QoreString *str = new QoreString();
   Point p;
   assign_point(p, &((CIRCLE *)data)->center);
   double radius = MSBf8(((CIRCLE *)data)->radius);
   str->sprintf("<(%g,%g),%g>", p.x, p.y, radius);
   return new QoreNode(str);
}

// static initialization
void QorePGResult::static_init()
{
   data_map[BOOLOID]        = qpg_data_bool;
   data_map[BYTEAOID]       = qpg_data_bytea;
   data_map[CHAROID]        = qpg_data_char;
   data_map[BPCHAROID]      = qpg_data_char;
   data_map[INT8OID]        = qpg_data_int8;
   data_map[INT4OID]        = qpg_data_int4;
   data_map[OIDOID]         = qpg_data_int4;
   data_map[XIDOID]         = qpg_data_int4;
   data_map[CIDOID]         = qpg_data_int4;
   //data_map[REGPROCOID]     = qpg_data_int4;
   data_map[INT2OID]        = qpg_data_int2;
   data_map[TEXTOID]        = qpg_data_text;
   data_map[VARCHAROID]     = qpg_data_text;
   data_map[NAMEOID]        = qpg_data_text;
   data_map[FLOAT4OID]      = qpg_data_float4;
   data_map[FLOAT8OID]      = qpg_data_float8;
   data_map[ABSTIMEOID]     = qpg_data_abstime;
   data_map[RELTIMEOID]     = qpg_data_reltime;

   data_map[TIMESTAMPOID]   = qpg_data_timestamp;
   data_map[TIMESTAMPTZOID] = qpg_data_timestamp;
   data_map[DATEOID]        = qpg_data_date;
   data_map[INTERVALOID]    = qpg_data_interval;
   data_map[TIMEOID]        = qpg_data_time;
   data_map[TIMETZOID]      = qpg_data_timetz;
   data_map[TINTERVALOID]   = qpg_data_tinterval;
   data_map[NUMERICOID]     = qpg_data_numeric;
   data_map[CASHOID]        = qpg_data_cash;
   data_map[MACADDROID]     = qpg_data_macaddr;
   data_map[INETOID]        = qpg_data_inet;
   data_map[CIDROID]        = qpg_data_inet;
   data_map[TIDOID]         = qpg_data_tid;
   data_map[BITOID]         = qpg_data_bit;
   data_map[VARBITOID]      = qpg_data_bit;
   data_map[POINTOID]       = qpg_data_point;
   data_map[LSEGOID]        = qpg_data_lseg;
   data_map[BOXOID]         = qpg_data_box;
   data_map[PATHOID]        = qpg_data_path;
   data_map[POLYGONOID]     = qpg_data_polygon;
   data_map[CIRCLEOID]      = qpg_data_circle;

   //data_map[INT2VECTOROID]  = qpg_data_int2vector;
   //data_map[OIDVECTOROID]   = qpg_data_oidvector;

   //array_data_map[INT2VECTOROID] = std::make_pair(INT2OID, qpg_data_int2);
   array_data_map[QPGT_INT4ARRAYOID]         = std::make_pair(INT4OID, qpg_data_int4);
   array_data_map[QPGT_CIRCLEARRAYOID]       = std::make_pair(CIRCLEOID, qpg_data_circle);
   array_data_map[QPGT_MONEYARRAYOID]        = std::make_pair(CASHOID, qpg_data_cash);
   array_data_map[QPGT_BOOLARRAYOID]         = std::make_pair(BOOLOID, qpg_data_bool);
   array_data_map[QPGT_BYTEAARRAYOID]        = std::make_pair(BYTEAOID, qpg_data_bytea);
   array_data_map[QPGT_NAMEARRAYOID]         = std::make_pair(NAMEOID, qpg_data_text);
   array_data_map[QPGT_INT2ARRAYOID]         = std::make_pair(INT2OID, qpg_data_int2);
   //array_data_map[QPGT_INT2VECTORARRAYOID]   = std::make_pair(OID, qpg_data_);
   //array_data_map[QPGT_REGPROCARRAYOID]      = std::make_pair(OID, qpg_data_);
   array_data_map[QPGT_TEXTARRAYOID]         = std::make_pair(TEXTOID, qpg_data_text);
   array_data_map[QPGT_OIDARRAYOID]          = std::make_pair(OIDOID, qpg_data_int4);
   array_data_map[QPGT_TIDARRAYOID]          = std::make_pair(TIDOID, qpg_data_tid);
   array_data_map[QPGT_XIDARRAYOID]          = std::make_pair(XIDOID, qpg_data_int4);
   array_data_map[QPGT_CIDARRAYOID]          = std::make_pair(CIDOID, qpg_data_int4);
   //array_data_map[QPGT_OIDVECTORARRAYOID]    = std::make_pair(OID, qpg_data_);
   array_data_map[QPGT_BPCHARARRAYOID]       = std::make_pair(BPCHAROID, qpg_data_char);
   array_data_map[QPGT_VARCHARARRAYOID]      = std::make_pair(VARCHAROID, qpg_data_text);
   array_data_map[QPGT_INT8ARRAYOID]         = std::make_pair(INT8OID, qpg_data_int8);
   array_data_map[QPGT_POINTARRAYOID]        = std::make_pair(POINTOID, qpg_data_point);
   array_data_map[QPGT_LSEGARRAYOID]         = std::make_pair(LSEGOID, qpg_data_lseg);
   array_data_map[QPGT_PATHARRAYOID]         = std::make_pair(PATHOID, qpg_data_path);
   array_data_map[QPGT_BOXARRAYOID]          = std::make_pair(BOXOID, qpg_data_box);
   array_data_map[QPGT_FLOAT4ARRAYOID]       = std::make_pair(FLOAT4OID, qpg_data_float4);
   array_data_map[QPGT_FLOAT8ARRAYOID]       = std::make_pair(FLOAT8OID, qpg_data_float8);
   array_data_map[QPGT_ABSTIMEARRAYOID]      = std::make_pair(ABSTIMEOID, qpg_data_abstime);
   array_data_map[QPGT_RELTIMEARRAYOID]      = std::make_pair(RELTIMEOID, qpg_data_reltime);
   array_data_map[QPGT_TINTERVALARRAYOID]    = std::make_pair(TINTERVALOID, qpg_data_tinterval);
   array_data_map[QPGT_POLYGONARRAYOID]      = std::make_pair(POLYGONOID, qpg_data_polygon);
   //array_data_map[QPGT_ACLITEMARRAYOID]      = std::make_pair(OID, qpg_data_);
   array_data_map[QPGT_MACADDRARRAYOID]      = std::make_pair(MACADDROID, qpg_data_macaddr);
   array_data_map[QPGT_INETARRAYOID]         = std::make_pair(INETOID, qpg_data_inet);
   array_data_map[QPGT_CIDRARRAYOID]         = std::make_pair(CIDROID, qpg_data_inet);
   array_data_map[QPGT_TIMESTAMPARRAYOID]    = std::make_pair(TIMESTAMPOID, qpg_data_timestamp);
   array_data_map[QPGT_DATEARRAYOID]         = std::make_pair(DATEOID, qpg_data_date);
   array_data_map[QPGT_TIMEARRAYOID]         = std::make_pair(TIMEOID, qpg_data_time);
   array_data_map[QPGT_TIMESTAMPTZARRAYOID]  = std::make_pair(TIMESTAMPTZOID, qpg_data_timestamp);
   array_data_map[QPGT_INTERVALARRAYOID]     = std::make_pair(INTERVALOID, qpg_data_interval);
   array_data_map[QPGT_NUMERICARRAYOID]      = std::make_pair(NUMERICOID, qpg_data_numeric);
   array_data_map[QPGT_TIMETZARRAYOID]       = std::make_pair(TIMETZOID, qpg_data_timetz);
   array_data_map[QPGT_BITARRAYOID]          = std::make_pair(BITOID, qpg_data_bit);
   array_data_map[QPGT_VARBITARRAYOID]       = std::make_pair(VARBITOID, qpg_data_bit);
   //array_data_map[QPGT_REFCURSORARRAYOID]    = std::make_pair(OID, qpg_data_);
   //array_data_map[QPGT_REGPROCEDUREARRAYOID] = std::make_pair(OID, qpg_data_);
   //array_data_map[QPGT_REGOPERARRAYOID]      = std::make_pair(OID, qpg_data_);
   //array_data_map[QPGT_REGOPERATORARRAYOID]  = std::make_pair(OID, qpg_data_);
   //array_data_map[QPGT_REGCLASSARRAYOID]     = std::make_pair(OID, qpg_data_);
   //array_data_map[QPGT_REGTYPEARRAYOID]      = std::make_pair(OID, qpg_data_);
   //array_data_map[QPGT_ANYARRAYOID]          = std::make_pair(OID, qpg_data_);

   array_type_map[INT4OID]                      = QPGT_INT4ARRAYOID;
   array_type_map[CIRCLEOID]                    = QPGT_CIRCLEARRAYOID;
   array_type_map[CASHOID]                      = QPGT_MONEYARRAYOID;
   array_type_map[BOOLOID]                      = QPGT_BOOLARRAYOID;
   array_type_map[BYTEAOID]                     = QPGT_BYTEAARRAYOID;
   array_type_map[NAMEOID]                      = QPGT_NAMEARRAYOID;
   array_type_map[INT2OID]                      = QPGT_INT2ARRAYOID;
   array_type_map[TEXTOID]                      = QPGT_TEXTARRAYOID;
   array_type_map[OIDOID]                       = QPGT_OIDARRAYOID;
   array_type_map[TIDOID]                       = QPGT_TIDARRAYOID;
   array_type_map[XIDOID]                       = QPGT_XIDARRAYOID;
   array_type_map[CIDOID]                       = QPGT_CIDARRAYOID;
   array_type_map[BPCHAROID]                    = QPGT_BPCHARARRAYOID;
   array_type_map[VARCHAROID]                   = QPGT_VARCHARARRAYOID;
   array_type_map[INT8OID]                      = QPGT_INT8ARRAYOID;
   array_type_map[POINTOID]                     = QPGT_POINTARRAYOID;
   array_type_map[LSEGOID]                      = QPGT_LSEGARRAYOID;
   array_type_map[PATHOID]                      = QPGT_PATHARRAYOID;
   array_type_map[BOXOID]                       = QPGT_BOXARRAYOID;
   array_type_map[FLOAT4OID]                    = QPGT_FLOAT4ARRAYOID;
   array_type_map[FLOAT8OID]                    = QPGT_FLOAT8ARRAYOID;
   array_type_map[ABSTIMEOID]                   = QPGT_ABSTIMEARRAYOID;
   array_type_map[RELTIMEOID]                   = QPGT_RELTIMEARRAYOID;
   array_type_map[TINTERVALOID]                 = QPGT_TINTERVALARRAYOID;
   array_type_map[POLYGONOID]                   = QPGT_POLYGONARRAYOID;
   array_type_map[MACADDROID]                   = QPGT_MACADDRARRAYOID;
   array_type_map[INETOID]                      = QPGT_INETARRAYOID;
   array_type_map[CIDROID]                      = QPGT_CIDRARRAYOID;
   array_type_map[TIMESTAMPOID]                 = QPGT_TIMESTAMPARRAYOID;
   array_type_map[DATEOID]                      = QPGT_DATEARRAYOID;
   array_type_map[TIMEOID]                      = QPGT_TIMEARRAYOID;
   array_type_map[TIMESTAMPTZOID]               = QPGT_TIMESTAMPTZARRAYOID;
   array_type_map[INTERVALOID]                  = QPGT_INTERVALARRAYOID;
   array_type_map[NUMERICOID]                   = QPGT_NUMERICARRAYOID;
   array_type_map[TIMETZOID]                    = QPGT_TIMETZARRAYOID;
   array_type_map[BITOID]                       = QPGT_BITARRAYOID;
   array_type_map[VARBITOID]                    = QPGT_VARBITARRAYOID;
}

QorePGResult::QorePGResult(class QoreEncoding *r_enc) : res(NULL), nParams(0), allocated(0), paramTypes(NULL), paramValues(NULL), 
							paramLengths(NULL), paramFormats(NULL), paramArray(NULL), enc(r_enc)

{
}

QorePGResult::~QorePGResult()
{
   if (res)
      PQclear(res);
   if (allocated)
   {
      parambuf_list_t::iterator i = parambuf_list.begin();
      for (int j = 0; j < nParams; i++, j++)
      {
	 if (paramTypes[j] == TEXTOID && (*i)->str)
	    free((*i)->str);
	 else if (paramArray[j] && (*i)->ptr)
	    free((*i)->ptr);
      }
      free(paramTypes);
      free(paramValues);
      free(paramLengths);
      free(paramFormats);
      free(paramArray);
   }
}

int QorePGResult::rowsAffected()
{
   char *num = PQcmdTuples(res);

   return atoi(num);
}

bool QorePGResult::hasResultData()
{
   return PQnfields(res);
}

class QoreNode *QorePGResult::getArray(int type, qore_pg_data_func_t func, char *&array_data, int current, int ndim, int dim[])
{
   //printd(5, "getArray(type=%d, array_data=%08p, current=%d, ndim=%d, dim[%d]=%d)\n", type, array_data, current, ndim, current, dim[current]);
   class List *l = new List();
   
   if (current != (ndim - 1))
   {
      for (int i = 0; i < dim[current]; ++i)
	 l->push(getArray(type, func, array_data, current + 1, ndim, dim));
   }
   else
      for (int i = 0; i < dim[current]; ++i)
      {
	 int length = ntohl(*((uint32_t *)(array_data)));
	 //printd(5, "length=%d\n", length);
	 array_data += 4;
	 if (length == -1) // NULL value
	    l->push(null());
	 else
	 {
	    l->push(func(array_data, type, length, enc));
	    array_data += length;
	 }
      }

   return new QoreNode(l);
}

// converts from PostgreSQL data types to Qore data
class QoreNode *QorePGResult::getNode(int row, int col, class ExceptionSink *xsink)
{
   void *data = PQgetvalue(res, row, col);
   int type = PQftype(res, col);
   //int mod = PQfmod(res, col); 
   int len = PQgetlength(res, row, col);

   if (PQgetisnull(res, row, col))
      return null();

   qore_pg_data_map_t::const_iterator i = data_map.find(type);
   if (i != data_map.end())
      return i->second((char *)data, type, len, enc);

   // otherwise, see if it's an array
   qore_pg_array_data_map_t::const_iterator ai = array_data_map.find(type);
   if (ai == array_data_map.end())
   {
      xsink->raiseException("DBI:PGSQL:TYPE-ERROR", "don't know how to handle type ID: %d", type);      
      return NULL;
   }

   //printd(5, "QorePGResult::getNode(row=%d, col=%d) ARRAY type=%d this=%08p len=%d\n", row, col, type, this, len);
   qore_pg_array_header *ah = (qore_pg_array_header *)data;
   int ndim = ntohl(ah->ndim);
   //int oid  = ntohl(ah->oid);
   //printd(5, "array dimensions %d, oid=%d\n", ndim, oid);
   int dim[ndim], lBound[ndim];
   for (int i = 0; i < ndim; ++i)
   {
      dim[i]    = ntohl(ah->info[i].dim);
      lBound[i] = ntohl(ah->info[i].lBound);
      //printd(5, "%d: dim=%d lBound=%d\n", i, dim[i], lBound[i]);
   }
   
   char *array_data = ((char *)data) + 12 + 8 * ndim;
   
   return getArray(ai->second.first, ai->second.second, array_data, 0, ndim, dim);
}

class Hash *QorePGResult::getHash(class ExceptionSink *xsink)
{
   class Hash *h = new Hash();

   int num_columns = PQnfields(res);

   for (int i = 0; i < num_columns; ++i)
      h->setKeyValue(PQfname(res, i), new QoreNode(new List()), NULL);

   //printd(5, "num_columns=%d num_rows=%d\n", num_columns, PQntuples(res));

   for (int i = 0, e = PQntuples(res); i < e; ++i)
   {
      for (int j = 0; j < num_columns; ++j)
      {
	 class QoreNode *n = getNode(i, j, xsink);
	 if (!n)
	 {
	    h->derefAndDelete(xsink);
	    return NULL;
	 }
	 h->getKeyValue(PQfname(res, j))->val.list->push(n);
      }
   }
   return h;
}

class List *QorePGResult::getList(class ExceptionSink *xsink)
{
   class List *l = new List();

   int num_columns = PQnfields(res);

   //printd(5, "num_columns=%d num_rows=%d\n", num_columns, PQntuples(res));

   for (int i = 0, e = PQntuples(res); i < e; ++i)
   {
      class Hash *h = new Hash();
      for (int j = 0; j < num_columns; ++j)
      {
	 class QoreNode *n = getNode(i, j, xsink);
	 if (!n)
	 {
	    h->derefAndDelete(xsink);
	    l->derefAndDelete(xsink);
	    return NULL;
	 }
	 h->setKeyValue(PQfname(res, j), n, NULL);
      }
      l->push(new QoreNode(h));
   }
   return l;
}

static int check_hash_type(class Hash *h, class ExceptionSink *xsink)
{
   class QoreNode *t = h->getKeyValue("^pgtype^");
   if (!t)
   {
      xsink->raiseException("DBI:PGSQL:BIND-ERROR", "missing '^pgtype^' value in bind hash");
      return -1;
   }
   if (t->type != NT_INT)
   {
      xsink->raiseException("DBI:PGSQL:BIND-ERROR", "'^pgtype^' key contains '%s' value, expecting integer", t->type->getName());
      return -1;
   }
   return t->val.intval;
}

int QorePGResult::add(class QoreNode *v, class ExceptionSink *xsink)
{
   parambuf *pb = new parambuf();
   parambuf_list.push_back(pb);

   int rc = 0;
   //printd(5, "nparams=%d, type=%s\n", nParams, v->type->getName());
   if (nParams == allocated)
   {
      allocated += 5;
      paramTypes   = (Oid *)realloc(paramTypes,    sizeof(Oid) * allocated);
      paramValues  = (char **)realloc(paramValues, sizeof(char *) * allocated);
      paramLengths = (int *)realloc(paramLengths,  sizeof(int) * allocated);
      paramFormats = (int *)realloc(paramFormats,  sizeof(int) * allocated);
      paramArray   = (int *)realloc(paramArray,    sizeof(int) * allocated);
      //printd(5, "allocated=%d, nparams=%d\n", allocated, nParams);
   }
   paramArray[nParams] = 0;
   paramFormats[nParams] = 1;
   if (is_nothing(v) || is_null(v))
      paramTypes[nParams] = 0;
   else if (v->type == NT_INT)
   {
      if (v->val.intval <= 32767 && v->val.intval > -32768)
      {
	 //printd(5, "i2: %d\n", (int)v->val.intval);
	 paramTypes[nParams]   = INT2OID;
	 pb->assign((short)v->val.intval);
	 paramValues[nParams]  = (char *)&pb->i2;
	 paramLengths[nParams] = sizeof(short);
      }
      else if (v->val.intval <= 2147483647 && v->val.intval > -2147483648)
      {
	 //printd(5, "i4: %d (%d, %d)\n", (int)v->val.intval, sizeof(uint32_t), sizeof(int));
	 paramTypes[nParams]   = INT4OID;
	 pb->assign((int)v->val.intval);
	 //pb->i4 = htonl((int)v->val.intval);
	 paramValues[nParams]  = (char *)&pb->i4;
	 paramLengths[nParams] = sizeof(int);
      }
      else
      {
	 //printd(5, "i8: %lld\n", v->val.intval);
	 paramTypes[nParams]   = INT8OID;
	 pb->assign(v->val.intval);
	 paramValues[nParams]  = (char *)&pb->i8;
	 paramLengths[nParams] = sizeof(int64);
      }
   }
   else if (v->type == NT_FLOAT)
   {
      paramTypes[nParams]   = FLOAT8OID;
      pb->assign(v->val.floatval);
      paramValues[nParams]  = (char *)&pb->f8;
      paramLengths[nParams] = sizeof(double);
   }
   else if (v->type == NT_STRING)
   {
      paramTypes[nParams]   = TEXTOID;
      pb->str = NULL;
      TempEncodingHelper tmp(v->val.String, enc, xsink);
      if (!tmp)
	 return -1;

      paramLengths[nParams] = tmp->strlen();
      paramValues[nParams]  = tmp->getBuffer();
      // grab and save the buffer if it's a temporary string to be free'd after the request
      if (tmp.is_temp())
	 pb->str = tmp->giveBuffer();
      paramFormats[nParams] = 0;
   }
   else if (v->type == NT_BOOLEAN)
   {
      paramTypes[nParams]   = BOOLOID;
      paramValues[nParams]  = (char *)&v->val.boolval;
      paramLengths[nParams] = sizeof(bool);
   }
   else if (v->type == NT_DATE)
   {
      class DateTime *d = v->val.date_time;
      if (d->isRelative())
      {
	 paramTypes[nParams] = INTERVALOID;
	 
	 pb->iv.month = htonl(d->getMonth());
#if POSTGRES_VERSION_MAJOR >= 8
	 pb->iv.day   = htonl(d->getDay());
#endif
#ifdef HAVE_INT64_TIMESTAMP
	 pb->iv.time = i8MSB(((d->getYear() * 365 * 24 * 3600) + d->getHour() * 24 * 3600 + d->getMinute() * 3600 + d->getSecond()) * 1000000 + d->getMillisecond() * 1000);
#else
	 //printd(5, "year=%d, hour=%d, minute=%d, second=%d, ms=%d, seconds = %g\n", d->getYear(), d->getHour(), d->getMinute(), d->getSecond(), d->getMillisecond(), time);
	 pb->iv.time = f8MSB((double)((d->getYear() * 365 * 24 * 3600) + d->getHour() * 3600 + d->getMinute() * 60 + d->getSecond()) + (double)d->getMillisecond() / 1000);
#endif
	 paramValues[nParams] = (char *)&pb->iv;
	 paramLengths[nParams] = sizeof(Interval);
      }
      else
      {
	 paramTypes[nParams] = TIMESTAMPOID;
#ifdef HAVE_INT64_TIMESTAMP
	 // get number of seconds offset from jan 1 2000 then make it microseconds and add ms
	 int64 val = (d->getEpochSeconds() - 10957 * 86400) * 1000000 + d->getMillisecond() * 1000;
	 pb->assign(val);
	 paramValues[nParams] = (char *)&pb->i8;
	 paramLengths[nParams] = sizeof(int64);
#else
	 double val = (double)((double)d->getEpochSeconds() - 10957 * 86400) + (double)(d->getMillisecond() / 1000.0);
	 //printd(5, "timestamp time=%9g\n", val);
	 pb->assign(val);
	 paramValues[nParams] = (char *)&pb->f8;
	 paramLengths[nParams] = sizeof(double);
#endif
      }
   }
   else if (v->type == NT_BINARY)
   {
      paramTypes[nParams] = BYTEAOID;
      paramValues[nParams] = (char *)v->val.bin->getPtr();
      paramLengths[nParams] = v->val.bin->size();
   }
   else if (v->type == NT_HASH)
   {
      Oid type = check_hash_type(v->val.hash, xsink);
      if (type < 0)
	 return -1;
      class QoreNode *t = v->val.hash->getKeyValue("^value^");
      if (is_nothing(t) || is_null(t))
	 paramTypes[nParams] = 0;
      else
      {
	 paramTypes[nParams] = type;
	 if (t->type != NT_STRING)
	 {
	    // convert to string and save buffer for later deletion
	    t = t->convert(NT_STRING, xsink);
	    if (xsink->isException())
	    {
	       if (t)
		  t->deref(xsink);
	       return -1;
	    }
	    paramLengths[nParams] = t->val.String->strlen();
	    pb->str = t->val.String->giveBuffer();
	    paramValues[nParams]  = pb->str;	    
	 }
	 else
	 {
	    pb->str = NULL;
	    paramValues[nParams]  = t->val.String->getBuffer();
	    paramLengths[nParams] = t->val.String->strlen();
	 }
      }
      paramFormats[nParams] = 0;
   }
   else if (v->type == NT_LIST)
   {
      int len = v->val.list->size();
      if (!len)
	 paramTypes[nParams] = 0;
      else
      {
	 std::auto_ptr<QorePGBindArray> ba(new QorePGBindArray);
	 if (ba->create_data(v->val.list, 0, enc, xsink))
	    return -1;
	 
	 paramArray[nParams] = 1;
	 paramTypes[nParams] = ba->getArrayOid();
	 paramLengths[nParams] = ba->getSize();
	 pb->ptr = ba->getHeader();
	 paramValues[nParams] = (char *)pb->ptr;
	 paramFormats[nParams] = ba->getFormat();
	 //printd(5, "QorePGResult::add() array size=%d, arrayoid=%d, data=%08p\n", ba->getSize(), ba->getArrayOid(), pb->ptr);
      }
   }
   else
   {
      paramTypes[nParams] = 0;
      xsink->raiseException("DBI:PGSQL:EXEC-EXCEPTION", "don't know how to bind type '%s'", v->type->getName());
      rc = -1;
   }

   nParams++;
   return rc;
}

QorePGBindArray::QorePGBindArray() : ndim(0), size(0), allocated(0), elements(0),
				     ptr(NULL), hdr(NULL), type(NULL), oid(0), arrayoid(0),
				     format(1)
{
}

QorePGBindArray::~QorePGBindArray()
{
   if (hdr)
      free(hdr);
}

int QorePGBindArray::getOid() const
{
   return oid;
}

int QorePGBindArray::getArrayOid() const
{
   return arrayoid;
}

int QorePGBindArray::getSize() const
{
   return size;
}

qore_pg_array_header *QorePGBindArray::getHeader()
{
   qore_pg_array_header *rv = hdr;
   hdr = NULL;
   return rv;
}

int QorePGBindArray::check_type(class QoreNode *n, class ExceptionSink *xsink)
{
   // skip null types
   if (is_nothing(n) || is_null(n))
   {
      //return 0;
      // FIXME: pgsql null binding in arrays should work according to the documentation
      // however with PG 8.1 it does not appear to work :-(
      xsink->raiseException("DBI:PGSQL:ARRAY-ERROR", "cannot bind NULL values within an array");
      return -1;
   }
   class QoreType *t = n->type;
   if (!type)
   {
      type = t;
      // check that type is supported
      if (type == NT_INT)
      {
	 arrayoid = QPGT_INT8ARRAYOID;
	 oid = INT8OID;
      }
      else if (type == NT_FLOAT)
      {
	 arrayoid = QPGT_FLOAT8ARRAYOID;
	 oid = FLOAT8OID;
      }
      else if (type == NT_BOOLEAN)
      {
	 arrayoid = QPGT_BOOLARRAYOID;
	 oid = BOOLOID;
      }
      else if (type == NT_STRING)
      {
	 arrayoid = QPGT_TEXTARRAYOID;
	 oid = TEXTOID;
	 //format = 0;
      }
      else if (type == NT_DATE)
      {
	 if (n->val.date_time->isRelative())
	 {
	    arrayoid = QPGT_INTERVALARRAYOID;
	    oid = INTERVALOID;
	 }
	 else
	 {
	    arrayoid = QPGT_TIMESTAMPARRAYOID;
	    oid = TIMESTAMPOID;
	 }
      }
      else if (type == NT_BINARY)
      {
	 arrayoid = QPGT_BYTEAARRAYOID;
	 oid = BYTEAOID;
      }
/*
      else if (type == NT_HASH)
      {
	 format = 0;
	 oid = check_hash_type(n->val.hash, xsink);
	 if (oid < 0)
	    return -1;
	 qore_pg_array_type_map_t::const_iterator i = QorePGResult::array_type_map.find(oid);
	 if (i == QorePGResult::array_type_map.end())
	 {
	    xsink->raiseException("DBI:PGSQL:ARRAY-ERROR", "don't know how to bind arrays of typeid %d", oid);
	    return -1;
	 }
	 arrayoid = i->second;
      }
*/
      else
      {
	 xsink->raiseException("DBI:PGSQL:ARRAY-ERROR", "don't know how to bind type '%s'", type->getName());
	 return -1;
      }
   }
   else if (t != type)
   {
      xsink->raiseException("DBI:PGSQL:ARRAY-ERROR", "array elements must be all of the same type for binding");
      return -1;
   } else if (t == NT_DATE)
   {
      if (n->val.date_time->isRelative() && (oid == TIMESTAMPOID))
      {
	 xsink->raiseException("DBI:PGSQL:ARRAY-ERROR", "array type was set to TIMESTAMP, but a relative date/time is present in the list");
	 return -1;
      }
      if (n->val.date_time->isAbsolute() && (oid == INTERVALOID))
      {
	 xsink->raiseException("DBI:PGSQL:ARRAY-ERROR", "array type was set to INTERVAL, but a absolute date/time is present in the list");
	 return -1;
      }
   }
   return 0;
}

int QorePGBindArray::check_oid(class Hash *h, class ExceptionSink *xsink)
{
   int o = check_hash_type(h, xsink);
   if (o < 0)
      return -1;

   if (!oid)
      oid = o;
   else if (o != oid)
   {
      xsink->raiseException("DBI:PGSQL:ARRAY-ERROR", "array elements must be all of the same type for binding");
      return -1;
   }
   return 0;
}

int QorePGBindArray::new_dimension(List *l, int current, class ExceptionSink *xsink)
{
   if (current >= MAXDIM)
   {
      xsink->raiseException("DBI:PGSQL:ARRAY-ERROR", "array exceeds maximum number of dimensions (%d)", MAXDIM);
      return -1;
   }
   ndim++;
   int len = l->size();
   if (!elements)
      elements = len;
   else
      elements *= len;
   dim[current] = len;
   return 0;
}

int QorePGBindArray::create_data(List *l, int current, class QoreEncoding *enc, class ExceptionSink *xsink)
{
   if (new_dimension(l, current, xsink))
      return -1;
   return process_list(l, current, enc, xsink);
}

void QorePGBindArray::check_size(int len)
{
   int space = (len == -1 ? 4 : len + 4);
   if (allocated < (size + space))
   {
      int offset = 12 + 8 * ndim;
      bool init = !allocated;
      if (init)
	 size = offset;
      allocated = size + space + (allocated / 3);
      hdr = (qore_pg_array_header *)realloc(hdr, allocated);
      // setup header
      //printd(5, "check_size(len=%d) ndim=%d, allocated=%d, offset=%d hdr=%08p\n", len, ndim, allocated, 12 + 8 * ndim, hdr);
      if (init)
      {
	 hdr->ndim  = htonl(ndim);
	 hdr->flags = 0; // htonl(0);
	 hdr->oid   = htonl(oid);
	 for (int i = 0; i < ndim; i++)
	 {
	    //printd(5, "this=%08p initializing header addr=%08p (offset=%d) ndim=%d, oid=%d, dim[%d]=%d, lBound[%d]=1\n", 
	    //       this, &hdr->info[i].dim, (char *)&hdr->info[i].dim - (char *)hdr, ndim, oid, i, dim[i], i);
	    hdr->info[i].dim = htonl(dim[i]);
	    hdr->info[i].lBound = htonl(1);
	 }
      }
      ptr = (char *)hdr + size;
   }
   //printd(5, "check_size(len=%d) space=%d ndim=%d, allocated=%d, size=%d (%d) offset=%d hdr=%08p ptr=%08p\n", len, space, ndim, allocated, size, ptr-(char *)hdr, 12 + 8 * ndim, hdr, ptr);
   int *length = (int *)ptr;
   *length = htonl(len);
   ptr += 4;
   size += space;
}

int QorePGBindArray::bind(class QoreNode *n, class QoreEncoding *enc, class ExceptionSink *xsink)
{
   // bind a NULL for NOTHING or NULL
   if (is_nothing(n) || is_null(n))
      check_size(-1);
   else if (type == NT_INT)
   {
      check_size(8);
      int64 *i8 = (int64 *)ptr;
      *i8 = i8MSB(n->val.intval);
      ptr += 8;
   }   
   else if (type == NT_FLOAT)
   {
      check_size(8);
      double *f8 = (double *)ptr;
      *f8 = f8MSB(n->val.floatval);
      ptr += 8;
   }
   else if (type == NT_BOOLEAN)
   {
      check_size(sizeof(bool));
      bool *b = (bool *)ptr;
      *b = htonl(n->val.boolval);
      ptr += sizeof(bool);
   }   
   else if (type == NT_STRING)
   {
      TempEncodingHelper tmp(n->val.String, enc, xsink);
      if (!tmp)
	 return -1;
      int len = tmp->strlen();
      check_size(len);
      memcpy(ptr, tmp->getBuffer(), len);
      ptr += len;
   }   
   else if (type == NT_DATE)
   {
      class DateTime *d = n->val.date_time;
      if (d->isRelative())
      {
	 check_size(sizeof(Interval));
	 Interval *i = (Interval *)ptr;
	 
	 i->month = htonl(d->getMonth());
#if POSTGRES_VERSION_MAJOR >= 8
	 i->day   = htonl(d->getDay());
#endif
#ifdef HAVE_INT64_TIMESTAMP
	 i->time = i8MSB(((d->getYear() * 365 * 24 * 3600) + d->getHour() * 24 * 3600 + d->getMinute() * 3600 + d->getSecond()) * 1000000 + d->getMillisecond() * 1000);
#else
	 //printd(5, "year=%d, hour=%d, minute=%d, second=%d, ms=%d, seconds = %g\n", d->getYear(), d->getHour(), d->getMinute(), d->getSecond(), d->getMillisecond(), time);
	 i->time = f8MSB((double)((d->getYear() * 365 * 24 * 3600) + d->getHour() * 3600 + d->getMinute() * 60 + d->getSecond()) + (double)d->getMillisecond() / 1000.0);
#endif
	 ptr += sizeof(Interval);
      }
      else
      {
	 check_size(8);
#ifdef HAVE_INT64_TIMESTAMP
	 int64 *i = (int64 *)ptr;
	 // get number of seconds offset from jan 1 2000 then make it microseconds and add ms
	 *i = i8MSB((d->getEpochSeconds() - 10957 * 86400) * 1000000 + d->getMillisecond() * 1000);
#else
	 double *f = (double *)ptr;
	 *f = f8MSB((double)((double)d->getEpochSeconds() - 10957 * 86400) + (double)(d->getMillisecond() / 1000.0));
	 //printd(5, "timestamp time=%9g\n", val);
#endif
	 ptr += 8;
      }
   }
   else if (type == NT_BINARY)
   {
      int len = n->val.bin->size();
      check_size(len);
      memcpy(ptr, n->val.bin->getPtr(), len);
      ptr += len;
   }
   else if (type == NT_HASH)
   {
      class QoreNode *t = n->val.hash->getKeyValue("^value^");
      if (is_nothing(t) || is_null(t))
	 check_size(-1);
      else
      {
	 QoreNodeStringHelper tmp(t, xsink);
	 if (!tmp)
	    return -1;
	 check_size(tmp.strlen());
	 memcpy(ptr, *tmp, tmp.strlen());
	 ptr += tmp.strlen();
      }
   }
   return 0;
}

int QorePGBindArray::process_list(List *l, int current, class QoreEncoding *enc, class ExceptionSink *xsink)
{
   ListIterator li(l);
   while (li.next())
   {
      class QoreType *type;
      class QoreNode *n = li.getValue();
      type = n ? n->type : NT_NOTHING;
      if (type == NT_LIST)
      {
	 if (li.first())
	    if (new_dimension(n->val.list, current + 1, xsink))
	       return -1;
	 if (process_list(n->val.list, current + 1, enc, xsink))
	    return -1;
      }
      else
      {
	 if (check_type(n, xsink))
	    return -1;
	 if (type == NT_HASH && check_oid(n->val.hash, xsink))
	    return -1;

	 if (bind(n, enc, xsink))
	    return -1;
      }
   }
   if (!oid)
   {
      xsink->raiseException("DBI:PGSQL:ARRAY-BIND-ERROR", "no type can be determined from the list");
      return -1;
   }
   return 0;
}

int QorePGResult::parse(class QoreString *str, class List *args, class ExceptionSink *xsink)
{
   char quote = 0;
   char *p = str->getBuffer();
   QoreString tmp;
   while (*p)
   {
      if (!quote && (*p) == '%') // found value marker
      {
         char *w = p;

         p++;
         if ((*p) != 'v')
         {
            xsink->raiseException("DBI-EXEC-PARSE-EXCEPTION", "invalid value specification (expecting '%v', got %%%c)", *p);
            return -1;
         }
         p++;
         if (isalpha(*p))
         {
            xsink->raiseException("DBI-EXEC-PARSE-EXCEPTION", "invalid value specification (expecting '%v', got %%v%c*)", *p);
            return -1;
         }
         if (!args || args->size() <= nParams)
         {
            xsink->raiseException("DBI-EXEC-PARSE-EXCEPTION", "too few arguments passed (%d) for value expression (%d)",
                                  args ? args->size() : 0, nParams + 1);
            return -1;
         }
         class QoreNode *v = args->retrieve_entry(nParams);

         // replace value marker with "$<num>"
         // find byte offset in case string buffer is reallocated with replace()
	 tmp.sprintf("$%d", nParams + 1); 
         int offset = w - str->getBuffer();
         str->replace(offset, 2, &tmp);
	 tmp.clear();
         p = str->getBuffer() + offset;
	 if (add(v, xsink))
	    return -1;
      }
      else if (((*p) == '\'') || ((*p) == '\"'))
      {
         if (!quote)
            quote = *p;
         else if (quote == (*p))
            quote = 0;
         p++;
      }
      else
         p++;
   }
   return 0;
}

static int do_pg_error(const char *err, class ExceptionSink *xsink)
{
   const char *e;
   if (!strncmp(err, "ERROR:  ", 8))
      e = err + 8;
   else
      e = err;
   QoreString *desc = new QoreString(e);
   desc->terminate(desc->strlen() - 1);
   xsink->raiseException("DBI:PGSQL:ERROR", desc);
   return -1;
}


// Note that we can write to the str argument; it is always a copy
int QorePGResult::exec(PGconn *pc, class QoreString *str, class List *args, class ExceptionSink *xsink)
{
   if (parse(str, args, xsink))
      return -1;

   //printd(5, "nParams=%d sql=%s\n", nParams, str->getBuffer());
  
   res = PQexecParams(pc, str->getBuffer(), nParams, paramTypes, paramValues, paramLengths, paramFormats, 1);
   ExecStatusType rc = PQresultStatus(res);
   if (rc != PGRES_COMMAND_OK && rc != PGRES_TUPLES_OK)
      return do_pg_error(PQerrorMessage(pc), xsink);
   return 0;
}

int QorePGResult::exec(PGconn *pc, const char *cmd, class ExceptionSink *xsink)
{
   res = PQexecParams(pc, cmd, 0, NULL, NULL, NULL, NULL, 1);
   ExecStatusType rc = PQresultStatus(res);
   if (rc != PGRES_COMMAND_OK && rc != PGRES_TUPLES_OK)
      return do_pg_error(PQerrorMessage(pc), xsink);
   return 0;
}

QorePGConnection::QorePGConnection(char *str, class ExceptionSink *xsink)
{
   pc = PQconnectdb(str);
   if (PQstatus(pc) != CONNECTION_OK)
      do_pg_error(PQerrorMessage(pc), xsink);
}

QorePGConnection::~QorePGConnection()
{
   if (pc)
      PQfinish(pc);
}

int QorePGConnection::setPGEncoding(const char *enc, class ExceptionSink *xsink)
{
   SafeLocker sl(this);
   if (PQsetClientEncoding(pc, enc))
   {
      xsink->raiseException("DBI:PGSQL:ENCODING-ERROR", "invalid PostgreSQL encoding '%s'", enc);
      return -1;
   }
   return 0;
}

int QorePGConnection::commit(class Datasource *ds, ExceptionSink *xsink)
{
   SafeLocker sl(this);
   QorePGResult res(ds->getQoreEncoding());
   return res.exec(pc, "commit", xsink);
}

int QorePGConnection::rollback(class Datasource *ds, ExceptionSink *xsink)
{
   SafeLocker sl(this);
   QorePGResult res(ds->getQoreEncoding());
   return res.exec(pc, "rollback", xsink);
}

int QorePGConnection::begin_transaction(class Datasource *ds, ExceptionSink *xsink)
{
   SafeLocker sl(this);
   QorePGResult res(ds->getQoreEncoding());
   return res.exec(pc, "begin", xsink);
}

class QoreNode *QorePGConnection::select(class Datasource *ds, QoreString *qstr, class List *args, class ExceptionSink *xsink)
{
   SafeLocker sl(this);
   QorePGResult res(ds->getQoreEncoding());
   if (res.exec(pc, qstr, args, xsink))
      return NULL;

   sl.unlock();

   class Hash *h = res.getHash(xsink);
   return h ? new QoreNode(h) : NULL;
}

class QoreNode *QorePGConnection::select_rows(class Datasource *ds, QoreString *qstr, class List *args, class ExceptionSink *xsink)
{
   SafeLocker sl(this);
   QorePGResult res(ds->getQoreEncoding());
   if (res.exec(pc, qstr, args, xsink))
      return NULL;

   sl.unlock();
   class List *l = res.getList(xsink);
   return l ? new QoreNode(l) : NULL;
}

class QoreNode *QorePGConnection::exec(class Datasource *ds, QoreString *qstr, class List *args, class ExceptionSink *xsink)
{
   SafeLocker sl(this);
   QorePGResult res(ds->getQoreEncoding());
   if (res.exec(pc, qstr, args, xsink))
      return NULL;

   sl.unlock();
   if (res.hasResultData())
   {
      class Hash *h = res.getHash(xsink);
      return h ? new QoreNode(h) : NULL;
   }

   return new QoreNode((int64)res.rowsAffected());
}
