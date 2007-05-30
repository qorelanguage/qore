/*
  command.cc

  Sybase DB layer for QORE
  uses Sybase OpenClient C library

  Qore Programming language

  Copyright (C) 2007 Qore Technologies

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
#include <qore/minitest.hpp>

#include <assert.h>
#include <cstypes.h>

#include "command.h"
#include "connection.h"

command::command(connection& conn, ExceptionSink* xsink) : m_conn(conn), m_cmd(0)
{
  CS_RETCODE err = ct_cmd_alloc(m_conn.getConnection(), &m_cmd);
  if (err != CS_SUCCEED) {
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call ct_cmd_alloc() failed with error %d", (int)err);
    return;
  }
}

//------------------------------------------------------------------------------
command::~command()
{
  if (!m_cmd) return;
  ct_cancel(0, m_cmd, CS_CANCEL_ALL);
  ct_cmd_drop(m_cmd);
}

int command::send(class ExceptionSink *xsink)
{
   CS_RETCODE err = ct_send(m_cmd);
   if (err != CS_SUCCEED) {
      m_conn.do_exception(xsink, "DBI:SYBASE:EXEC-ERROR", "ct_send() failed");
      return -1;
   } 
   return 0;
}

int command::initiate_language_command(const char *cmd_text, class ExceptionSink *xsink)
{
   assert(cmd_text && cmd_text[0]);
   CS_RETCODE err = ct_command(m_cmd, CS_LANG_CMD, (CS_CHAR*)cmd_text, CS_NULLTERM, CS_UNUSED);
   if (err != CS_SUCCEED) {
      m_conn.do_exception(xsink, "DBI:SYBASE:EXEC-ERROR", "ct_command(CS_LANG_CMD, '%s') failed with error %d", cmd_text, (int)err);
      return -1;
   }
   return 0;
}

bool command::fetch_row_into_buffers(class ExceptionSink *xsink)
{
   CS_INT rows_read;
   CS_RETCODE err = ct_fetch(m_cmd, CS_UNUSED, CS_UNUSED, CS_UNUSED, &rows_read);
   //printd(5, "ct_fetch() returned %d rows_read=%d\n", err, rows_read);
   if (err == CS_SUCCEED) {
      if (rows_read != 1) {
	 m_conn.do_exception(xsink, "DBI:SYBASE:EXEC-ERROR", "ct_fetch() returned %d rows (expected 1)", (int)rows_read);
	 return false;
      }
      return true;
   }
   if (err == CS_END_DATA) {
      return false;
   }
   m_conn.do_exception(xsink, "DBI-EXEC-EXCEPTION", "ct_fetch() returned errno %d", (int)err);
   return false;
}

unsigned command::get_column_count(ExceptionSink* xsink)
{
   CS_INT num_cols;
   CS_RETCODE err = ct_res_info(m_cmd, CS_NUMDATA, &num_cols, CS_UNUSED, NULL);
   if (err != CS_SUCCEED) {
      m_conn.do_exception(xsink, "DBI-EXEC-EXCEPTION", "ct_res_info() failed with error %d", (int)err);
      return 0;
   }
   if (num_cols <= 0) {
      m_conn.do_exception(xsink, "DBI-EXEC-EXCEPTION", "ct_res_info() failed");
      return 0;
   }
   return num_cols;
}

// FIXME: use ct_setparam to avoid copying data
int command::set_params(sybase_query &query, class List *args, ExceptionSink *xsink)
{
   unsigned nparams = query.param_list.size();
   //printd(5, "query=%s\n", query.m_cmd.getBuffer());

   for (unsigned i = 0; i < nparams; ++i)
   {
      //printd(5, "set_params() param %d = %d\n", i, query.param_list[i].type);

      if (query.param_list[i] == 'd') 
	 continue;

      class QoreNode *val = args ? args->retrieve_entry(i) : NULL;
      //printd(5, "set_params() param %d = value %08p (%s)\n", i, val, val ? val->type->getName() : "n/a");

      CS_DATAFMT datafmt;
      memset(&datafmt, 0, sizeof(datafmt));
      datafmt.status = CS_INPUTVALUE;
      datafmt.namelen = CS_NULLTERM;
      datafmt.maxlength = CS_UNUSED;
      datafmt.count = 1;

      CS_RETCODE err;

      if (!val || is_null(val) || is_nothing(val))
      {
#ifdef FREETDS
	 // it seems to be necessary to specify a type like 
	 // this to get a null value to be bound with freetds
	 datafmt.datatype = CS_CHAR_TYPE;
	 datafmt.format = CS_FMT_NULLTERM;
	 datafmt.maxlength = 1;
#endif
	 // SQL NULL value
	 err = ct_param(m_cmd, &datafmt, 0, CS_UNUSED, -1);
	 if (err != CS_SUCCEED) {
	    m_conn.do_exception(xsink, "DBI:SYBASE:EXEC-ERROR", "ct_param() for 'null' failed for parameter %u with error %d", i, (int)err);
	    return -1;
	 }
      }
      else if (val->type == NT_STRING)
      {
	 // ensure we bind with the proper encoding for the connection
	 TempEncodingHelper s(val->val.String, m_conn.getEncoding(), xsink);
	 if (!s)
	    return -1;

	 int slen = s->strlen();
	 datafmt.datatype = CS_CHAR_TYPE;
	 datafmt.format = CS_FMT_NULLTERM;
	 // NOTE: setting large sizes here like 2GB works for sybase ctlib, not for freetds
	 datafmt.maxlength = slen + 1;
	 err = ct_param(m_cmd, &datafmt, (CS_VOID*)s->getBuffer(), slen, 0);
	 if (err != CS_SUCCEED) {
	    m_conn.do_exception(xsink, "DBI:SYBASE:EXEC-ERROR", "ct_param() for string parameter %u failed with error", i, (int)err);
	    return -1;
	 }
      }
      else if (val->type == NT_DATE)
      {
	 CS_DATETIME dt;
	 if (DateTime_to_DATETIME(val->val.date_time, dt, xsink))
	    return -1;
	
	 datafmt.datatype = CS_DATETIME_TYPE;
	 err = ct_param(m_cmd, &datafmt, &dt, sizeof(dt), 0);
	 if (err != CS_SUCCEED) {
	    m_conn.do_exception(xsink, "DBI:SYBASE:EXEC-ERROR", "ct_param() for date/time parameter %u failed with error", i, (int)err);
	    return -1;
	 }
      }
      else if (val->type == NT_INT)
      {
#ifdef CS_BIGINT_TYPE
	 datafmt.datatype = CS_BIGINT_TYPE;
	 err = ct_param(m_cmd, &datafmt, &val->val.intval, sizeof(int64), 0);
#else
	 int64 ival = val->val.intval;
	 // if it's a 32-bit integer, bind as integer
	 if (ival <= 2147483647 && ival >= -2147483647)
	 {
	    datafmt.datatype = CS_INT_TYPE;
	    CS_INT vint = ival;
	    err = ct_param(m_cmd, &datafmt, &vint, sizeof(CS_INT), 0);
	 }
	 else // bind as float
	 {
	    CS_FLOAT fval = ival;
	    datafmt.datatype = CS_FLOAT_TYPE;
	    err = ct_param(m_cmd, &datafmt, &fval, sizeof(CS_FLOAT), 0); 
	 }
#endif

	 if (err != CS_SUCCEED) {
	    m_conn.do_exception(xsink, "DBI:SYBASE:EXEC-ERROR", "ct_param() for integer parameter %u (%lld) failed with error", i, val->val.intval, (int)err);
	    return -1;
	 }
      }
      else if (val->type == NT_BOOLEAN)
      {
	 CS_BIT bval = val->getAsBool();
	 datafmt.datatype = CS_BIT_TYPE;
	 err = ct_param(m_cmd, &datafmt, &bval, sizeof(val), 0);
	 if (err != CS_SUCCEED) {
	    m_conn.do_exception(xsink, "DBI:SYBASE:EXEC-ERROR", "ct_param() for boolean parameter %u (%s) failed with error", i, bval ? "True" : "False", (int)err);
	    return -1;
	 }
      }
      else if (val->type == NT_FLOAT)
      {
	 CS_FLOAT fval = val->val.floatval;
	 datafmt.datatype = CS_FLOAT_TYPE;
	 err = ct_param(m_cmd, &datafmt, &fval, sizeof(CS_FLOAT), 0); 
	 if (err != CS_SUCCEED) {
	    m_conn.do_exception(xsink, "DBI:SYBASE:EXEC-ERROR", "ct_param() for float parameter %u (%g) failed with error", i, val->val.floatval, (int)err);
	    return -1;
	 }
     }
      else if (val->type == NT_BINARY)
      {
	 datafmt.datatype = CS_BINARY_TYPE;
	 datafmt.maxlength = val->val.bin->size();
	 datafmt.count = 1;
	 err = ct_param(m_cmd, &datafmt, (void *)val->val.bin->getPtr(), val->val.bin->size(), 0);
	 if (err != CS_SUCCEED) {
	    m_conn.do_exception(xsink, "DBI:SYBASE:EXEC-ERROR", "ct_param() for binary parameter %u failed with error", i, (int)err);
	    return -1;
	 }
      }
      else
      {
	 xsink->raiseException("DBI:SYBASE:BIND-ERROR", "do not know how to bind values of type '%s'", val->type->getName());
	 return -1;
      }
   }
   return 0;
}

QoreNode *command::read_output(PlaceholderList &placeholder_list, bool list, ExceptionSink* xsink)
{
   ReferenceHolder<QoreNode> query_result(xsink), param_result(xsink), status_result(xsink);
   CS_INT result_type, rowcount = -1;
   CS_RETCODE err;
   int result_count = 0;

   while (true)
   {
      err = ct_results(m_cmd, &result_type);
      if (err == CS_END_RESULTS)
	 break;

      if (err != CS_SUCCEED)
      {
	 m_conn.do_exception(xsink, "DBI:SYBASE:EXEC-ERROR", "ct_results() returned error code %d", err);
	 return 0;
      }

      //printd(5, "read_output() result_type = %d\n", result_type);
      
      switch (result_type) {
	 case CS_COMPUTE_RESULT:
	 case CS_PARAM_RESULT: // procedure call
	    assert(!param_result);
	    param_result = read_rows(&placeholder_list, true, xsink);
	    if (xsink->isException())
	       return 0;
	    break;
	    
	 case CS_ROW_RESULT:
	 {
	    class QoreNode *t = read_rows(0, list, xsink);
	    if (xsink->isException())
	       return 0;
	    
	    if (result_count)
	    {
	       // put the results already read into hash key "query0"
	       if (result_count == 1)
	       {
		  Hash *h = new Hash();
		  h->setKeyValue("query0", query_result.release(), 0);
		  h->setKeyValue("query1", t, 0);
		  query_result = new QoreNode(h);
	       }
	       else
	       {
		  QoreString tmp;
		  tmp.sprintf("query%d", result_count);
		  query_result->val.hash->setKeyValue(tmp.getBuffer(), t, 0);
	       }
	    }
	    else
	       query_result = t;
	    ++result_count;
	    break;
	 }

	 case CS_STATUS_RESULT:
#ifdef SYBASE
	    assert(!status_result);
	    status_result = read_rows(0, true, xsink);
	    if (xsink->isException())
	       return 0;
#endif
	    break;
	    
	 case CS_CMD_DONE:
	 {
	    if (!query_result || param_result)
	    {
	       CS_RETCODE ret;
	       ret = ct_res_info(m_cmd, CS_ROW_COUNT, (CS_VOID *)&rowcount, CS_UNUSED, 0);
	       if (ret != CS_SUCCEED)
	       {
		  m_conn.do_exception(xsink, "DBI:SYBASE:EXEC-ERROR", "ct_res_info(CS_ROW_COUNT) failed with return code %d", ret);
		  return 0;
	       }
	    }   
	    break;
	 }
	 
	 case CS_CMD_SUCCEED:
	    // current command succeeded; there may be more
	    continue;
	    
	 case CS_CMD_FAIL:
	    m_conn.do_exception(xsink, "DBI:SYBASE:EXEC-ERROR", "SQL command failed");
	    return 0;
	    
	 default:
	    m_conn.do_exception(xsink, "DBI:SYBASE:EXEC-ERROR", "ct_results() returned unexpected result type %d", (int)result_type);
	    return 0;
      } // switch
   } // while

   //printd(5, "read_output() q=%d, p=%d\n", (bool)query_result, (bool)param_result);

   m_conn.purge_messages(xsink);   
   class QoreNode *rv = 0;
   if (!query_result)
   {
      if (param_result)
      {
	 rv = param_result.release();
	 assert(rv->type == NT_HASH);
      }
      else if (rowcount != -1)
	 return new QoreNode((int64)rowcount);

      if (rowcount != -1)
	 rv->val.hash->setKeyValue("rowcount", new QoreNode((int64)rowcount), xsink);
      return rv;
   }

   rv = query_result.release();
   if (!param_result)
      return rv;

   class Hash *h = new Hash();
   h->setKeyValue("query", rv, xsink);
   rv = new QoreNode(h);

   if (param_result)
      rv->val.hash->setKeyValue("params", param_result.release(), xsink);

   if (rowcount != -1)
      rv->val.hash->setKeyValue("rowcount", new QoreNode((int64)rowcount), xsink);

   return rv;
}

class QoreNode *command::read_rows(PlaceholderList *placeholder_list, bool list, ExceptionSink* xsink)
{
   unsigned columns = get_column_count(xsink);
   if (xsink->isException())
      return 0;
  
   row_result_t descriptions;
   if (get_row_description(descriptions, columns, xsink))
      return 0;

   row_output_buffers out_buffers;
   setup_output_buffers(descriptions, out_buffers, xsink);
   if (xsink->isException())
      return 0;
   
   class QoreEncoding *encoding = m_conn.getEncoding();
   class QoreNode *rv = 0;
   // setup hash of lists if necessary
   if (!list)
   {
      Hash *h = new Hash();
      QoreString str(encoding);
      for (unsigned i = 0, n = descriptions.size(); i != n; ++i) {

	 const char *col_name;

	 if (descriptions[i].name && descriptions[i].name[0]) {
	    col_name = descriptions[i].name;
	 } else {
	    if (!placeholder_list || !(col_name = placeholder_list->getName()))
	    {
	       str.clear();
	       str.sprintf("%d", i);
	       col_name = str.getBuffer();
	    }
	 }

	 h->setKeyValue(col_name, new QoreNode(new List()), 0);
      }
      rv = new QoreNode(h);
   }

   while (fetch_row_into_buffers(xsink)) {

      if (!list)
      {
	 if (append_buffers_to_list(placeholder_list, descriptions, out_buffers, rv->val.hash, xsink))
	 {
	    rv->deref(xsink);
	    return 0;
	 }
      }
      else
      {
	 Hash* h = output_buffers_to_hash(placeholder_list, descriptions, out_buffers, xsink);
	 if (xsink->isException()) {
	    if (rv)
	       rv->deref(xsink);
	    return 0;
	 }
	 if (rv)
	 {
	    if (rv->type == NT_HASH)
	    {
	       // convert to hash - several rows
	       class List *l = new List();
	       l->push(rv);
	       rv = new QoreNode(l);
	    }
	    rv->val.list->push(new QoreNode(h));
	 } else {
	    rv = new QoreNode(h);
	 }
      }
   } // while
   return rv;
}

// returns 0=OK, -1=error (exception raised)
int command::get_row_description(row_result_t &result, unsigned column_count, ExceptionSink* xsink)
{
   for (unsigned i = 0; i < column_count; ++i) 
   {
      CS_DATAFMT datafmt;
      memset(&datafmt, 0, sizeof(datafmt));
     
      CS_RETCODE err = ct_describe(m_cmd, i + 1, &datafmt);
      if (err != CS_SUCCEED)
      {
	 m_conn.do_exception(xsink, "DBI_SYBASE:EXEC-ERROR", "ct_describe() failed with error %d", (int)err);
	 return -1;
      }
      datafmt.count = 1; // fetch just single row per every ct_fetch()

      //printd(5, "command::get_row_description(): name=%s type=%d usertype=%d\n", datafmt.name, datafmt.datatype, datafmt.usertype);

      switch (datafmt.datatype) {
	 // we map DECIMAL types to strings so we have no conversion to do
	 case CS_DECIMAL_TYPE:
	 case CS_NUMERIC_TYPE: 
	    datafmt.maxlength = 50;

	 case CS_UNICHAR_TYPE:
	    datafmt.datatype = CS_CHAR_TYPE;

	 case CS_LONGCHAR_TYPE:
	 case CS_VARCHAR_TYPE:
	 case CS_TEXT_TYPE:
	    datafmt.format = CS_FMT_NULLTERM;
	    break;

	    // freetds only works with CS_FMT_PADNULL with CS_CHAR columns it seems
	    // however this is also compatible with Sybase's ct-lib
	 case CS_CHAR_TYPE:
	    datafmt.format = CS_FMT_PADNULL;
	    break;

#ifdef FREETDS
	    // FreeTDS seems to return DECIMAL types as FLOAT for some reason
	 case CS_FLOAT_TYPE:
	    // can't find a defined USER_TYPE_* for 26
	    if (datafmt.usertype == 26)
	    {
	       datafmt.maxlength = 50;
	       datafmt.datatype = CS_CHAR_TYPE;
	       datafmt.format = CS_FMT_NULLTERM;
	       break;
	    }
#endif

	 case CS_MONEY_TYPE:
	 case CS_MONEY4_TYPE:
	    datafmt.datatype = CS_FLOAT_TYPE;

	 default:
	    datafmt.format = CS_FMT_UNUSED;
	    break;
      }

      //printd(5, "command::get_row_description(): name=%s type=%d usertype=%d maxlength=%d\n", datafmt.name, datafmt.datatype, datafmt.usertype, datafmt.maxlength);

      result.push_back(datafmt); 
   }
   return 0;
}

int command::setup_output_buffers(const row_result_t &input_row_descriptions, row_output_buffers &result, class ExceptionSink *xsink)
{
   for (unsigned i = 0, n = input_row_descriptions.size(); i != n; ++i) 
   {
      unsigned size = input_row_descriptions[i].maxlength;
      output_value_buffer *out = new output_value_buffer(size);
      result.m_buffers.push_back(out);
      
      CS_RETCODE err = ct_bind(m_cmd, i + 1, (CS_DATAFMT*)&input_row_descriptions[i], out->value, &out->value_len, &out->indicator);
      if (err != CS_SUCCEED) 
      {
	 m_conn.do_exception(xsink, "DBI:SYBASE:EXEC-ERROR", "ct_bind() failed with error %d", (int)err);
	 return -1;
      }
   }
   return 0;
}

int command::append_buffers_to_list(PlaceholderList *placeholder_list, row_result_t &column_info, 
				    row_output_buffers& all_buffers, 
				    class Hash *h, ExceptionSink *xsink)
{
   //class QoreEncoding *encoding = m_conn.getEncoding();

   HashIterator hi(h);
   for (unsigned i = 0, n = column_info.size(); i != n; ++i) {
      hi.next();

      const output_value_buffer& buff = *(all_buffers.m_buffers[i]);
      QoreNode* value = get_node(column_info[i], buff, xsink);
      if (xsink->isException()) {
	 if (value) value->deref(xsink);
	 return -1;
      }

      hi.getValue()->val.list->push(value);      
   } // for
   
   return 0;
}

class Hash *command::output_buffers_to_hash(PlaceholderList *placeholder_list, row_result_t column_info, row_output_buffers& all_buffers, ExceptionSink* xsink)
{
   Hash *result = new Hash;
   if (placeholder_list)
      placeholder_list->reset();
   for (unsigned i = 0, n = column_info.size(); i != n; ++i) 
   {
      const output_value_buffer& buff = *(all_buffers.m_buffers[i]);
      QoreNode *value = get_node(column_info[i], buff, xsink);
      if (xsink->isException()) {
	 if (value) value->deref(xsink);
	 result->derefAndDelete(xsink);
	 return 0;
      }

      const char *column_name;
      if (column_info[i].name && column_info[i].name[0]) {
	 column_name = column_info[i].name;
      }
      else if (!placeholder_list || !(column_name = placeholder_list->getName()))
      {
	 char aux[20];
	 sprintf(aux, "%d", i);
	 result->setKeyValue(aux, value, xsink);
	 continue;
      }

      result->setKeyValue(column_name, value, xsink);
   } // for

   return result;
}

class QoreNode *command::get_node(const CS_DATAFMT& datafmt, const output_value_buffer& buffer, ExceptionSink* xsink)
{
  if (buffer.indicator == -1) { // SQL NULL
     return null();
  }

  class QoreEncoding *encoding = m_conn.getEncoding();

  //printd(5, "get_node() encoding=%s name=%s type=%d format=%d usertype=%d value_len=%d\n", encoding->getCode(), datafmt.name, datafmt.datatype, datafmt.format, datafmt.usertype, buffer.value_len);

  switch (datafmt.datatype) {
     case CS_CHAR_TYPE: // varchar
     {
	CS_CHAR* value = (CS_CHAR*)(buffer.value);
	QoreString *s;
	// sometimes the strings are not null terminated, both with sybase and freetds#
	// but sometimes they are?  something funny is going on
	if (!memchr(value, '\0', buffer.value_len))
	{
	   s = new QoreString(value, buffer.value_len - 1, encoding);
	   printd(5, "get_node() oops, no null in %s='%s'\n", datafmt.name, s->getBuffer());
	}
	else
	   s = new QoreString(value, encoding);
	s->trim_trailing_blanks();
	//printd(5, "name=%s vlen=%d strlen=%d len=%d str='%s'\n", datafmt.name, buffer.value_len, s->strlen(), s->length(), s->getBuffer());
	return new QoreNode(s);
     }

     case CS_LONGCHAR_TYPE:
     case CS_VARCHAR_TYPE:
     case CS_TEXT_TYPE:
     {
	CS_CHAR* value = (CS_CHAR*)(buffer.value);

	QoreString *s;
	// see if we need to strip trailing newlines (could not find a defined USER_TYPE_* for this!)
	if (datafmt.usertype == 1)
	{
#ifdef FREETDS_x
	   // sometimes freetds values are not coming with null termination for some reason
	   // see if there is a null value
	   if (!memchr(value, '\0', buffer.value_len))
	   {
	      s = new QoreString(value, buffer.value_len - 1, encoding);
	      printd(5, "get_node() no null in %s='%s'\n", datafmt.name, s->getBuffer());
	   }
	   else
	      s = new QoreString(value, encoding);
#else
	   s = new QoreString(value, encoding);
#endif
	   s->trim_trailing_blanks();
	}
	else
	   s = new QoreString(value, buffer.value_len - 1, encoding);
	printd(5, "name=%s type=%d strlen=%d vallen=%d len=%d str='%s'\n", datafmt.name, datafmt.datatype, buffer.value_len, s->strlen(), s->length(), s->getBuffer());
	return new QoreNode(s);
     }

     case CS_VARBINARY_TYPE:
     case CS_BINARY_TYPE:
     case CS_LONGBINARY_TYPE:
     case CS_IMAGE_TYPE:
     {
	CS_BINARY* value = (CS_BINARY*)(buffer.value);
	int size = buffer.value_len;
	void* block = malloc(size);
	if (!block) {
	   xsink->outOfMemory();
	   return 0;
	}
	memcpy(block, value, size);
	BinaryObject* bin = new BinaryObject(block, size);
	return new QoreNode(bin);
     }

    case CS_TINYINT_TYPE:
    {
      CS_TINYINT* value = (CS_TINYINT*)(buffer.value);
      return new QoreNode((int64)*value);
    }

    case CS_SMALLINT_TYPE:
    {
      CS_SMALLINT* value = (CS_SMALLINT*)(buffer.value);
      return new QoreNode((int64)*value);
    }

    case CS_INT_TYPE:
    {
      CS_INT* value = (CS_INT*)(buffer.value);
      return new QoreNode((int64)*value);
    }

    case CS_REAL_TYPE:
    {
      CS_REAL* value = (CS_REAL*)(buffer.value);
      return new QoreNode((double)*value);
    }

    case CS_FLOAT_TYPE:
    {
      CS_FLOAT* value = (CS_FLOAT*)(buffer.value);
      return new QoreNode((double)*value);
    }

    case CS_BIT_TYPE:
    {
      CS_BIT* value = (CS_BIT*)(buffer.value);
      return new QoreNode(*value != 0);
    }

    case CS_DATETIME_TYPE:
    {
      CS_DATETIME* value = (CS_DATETIME*)(buffer.value);

       // NOTE: can't find a USER_* define for 38!
       if (datafmt.usertype == 38)
	  return new QoreNode(TIME_to_DateTime(*value));

      return new QoreNode(DATETIME_to_DateTime(*value));
    }
    case CS_DATETIME4_TYPE:
    {
      CS_DATETIME4* value = (CS_DATETIME4*)(buffer.value);
      DateTime* dt = DATETIME4_to_DateTime(*value, xsink);
      return new QoreNode(dt);
    }

/*
    // money and money4 are retrieved as float
    case CS_MONEY_TYPE:
    {
      CS_MONEY* value = (CS_MONEY*)(buffer.value);
      double d = MONEY_to_double(m_conn, *value, xsink);
      if (xsink->isException()) {
        return 0;
      }
      return new QoreNode(d);
    }

    case CS_MONEY4_TYPE:
    {
      CS_MONEY4* value = (CS_MONEY4*)(buffer.value);
      double d = MONEY4_to_double(m_conn, *value, xsink);
      if (xsink->isException()) {
        return 0;
      }
      return new QoreNode(d);
    }

    // numeric/decimal are retrieved as strings
    case CS_NUMERIC_TYPE:
    case CS_DECIMAL_TYPE:
    {
      CS_DECIMAL* value = (CS_DECIMAL*)(buffer.value);
      QoreString *str = DECIMAL_to_string(m_conn, *value, xsink);
      return str ? new QoreNode(str) : 0;
    }
*/
    default:
      xsink->raiseException("DBI-EXEC-EXCEPTION", "Unknown data type %d", (int)datafmt.datatype);
      return 0;
  } // switch
}


#ifdef DEBUG
#  include "tests/send_command_tests.cc"
#  include "tests/command_tests.cc"
//#  include "tests/initiate_rpc_command_tests.cc"
#endif

// EOF


