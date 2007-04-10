/*
  sybase_low_level_interface.cc

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

#include <qore/config.h>
#include <qore/support.h>
#include <qore/Exception.h>

#include <cstypes.h>
#include <ctpublic.h>
#include <assert.h>
#include <pthread.h>
#include <qore/minitest.hpp>
#include <qore/ScopeGuard.h>
#include <qore/charset.h>
#include <qore/QoreNode.h>
#include <qore/Namespace.h>
#include <qore/QoreType.h>
#include <qore/TypeConstants.h>
#include <qore/BinaryObject.h>
#include <qore/DateTime.h>
#include <string>

#include "sybase_low_level_interface.h"
#include "sybase_connection.h"
#include "sybase_query_parser.h"

#ifndef CS_MAX_CHAR
// from <cstypes.h>
#define CS_MAX_CHAR 256
#endif

//------------------------------------------------------------------------------
void sybase_low_level_execute_directly_command(CS_CONNECTION* conn, const char* sql_text, ExceptionSink* xsink)
{
  CS_COMMAND* cmd = 0;
  CS_RETCODE err = ct_cmd_alloc(conn, &cmd);
  if (err != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call ct_cmd_alloc() failed with error %d", (int)err);
    return;
  }
  ON_BLOCK_EXIT(ct_cmd_drop, cmd);
  ScopeGuard canceller = MakeGuard(ct_cancel, (CS_CONNECTION*)0, cmd, CS_CANCEL_ALL);

  err = ct_command(cmd, CS_LANG_CMD, (CS_CHAR*)sql_text, strlen(sql_text), CS_END);
  if (err != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call ct_command(\"%s\") failed with error %d", (int)err, sql_text);
    return;
  }
  err = ct_send(cmd);
  if (err != CS_SUCCEED) {
    // assert(false); - goes this way during tests
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call ct_send() failed with error %d", (int)err);
    return;
  }

  // no results expected
  CS_INT result_type;
  err = ct_results(cmd, &result_type);
  if (err != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call ct_result() failed with error %d", (int)err);
    return;
  }

  if (result_type != CS_CMD_SUCCEED) {
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call ct_results() for \"%s\" failed with error %d", sql_text, (int)err);
    return;
  }
  while((err = ct_results(cmd, &result_type)) == CS_SUCCEED);
  canceller.Dismiss();
}

//------------------------------------------------------------------------------
sybase_command_wrapper::sybase_command_wrapper(sybase_connection& conn, ExceptionSink* xsink)
: m_cmd(0),
  m_context(conn.getContext())
{
  CS_RETCODE err = ct_cmd_alloc(conn.getConnection(), &m_cmd);
  if (err != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call ct_cmd_alloc() failed with error %d", (int)err);
    return;
  }

  // a unique (within the connection) string identifier needs to be generated
  static unsigned counter = 0;
  ++counter;
  char aux[30];
  sprintf(aux, "my_cmd_%u_%u", (unsigned)pthread_self(), counter);
  m_string_id = aux;
}

//------------------------------------------------------------------------------
sybase_command_wrapper::~sybase_command_wrapper()
{
  if (!m_cmd) return;
  ct_cancel(0, m_cmd, CS_CANCEL_ALL);
  ct_cmd_drop(m_cmd);
}

//------------------------------------------------------------------------------
void sybase_low_level_prepare_command(const sybase_command_wrapper& wrapper, const char* sql_text, ExceptionSink* xsink)
{
  assert(sql_text && sql_text[0]);
 
  CS_RETCODE err = ct_dynamic(wrapper(), CS_PREPARE, wrapper.getStringId(), CS_NULLTERM, (CS_CHAR*)sql_text, CS_NULLTERM);
  if (err != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call ct_dynamic(CS_PREPARE, \"%s\") failed with error %d", sql_text, (int)err);
    return;
  }

  err = ct_send(wrapper());
  if (err != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call ct_send() for \"%s\" failed with error %d", sql_text, (int)err);
    return;
  }

  // no results expected
  CS_INT result_type;
  err = ct_results(wrapper(), &result_type);
  if (err != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call ct_result() failed with error %d", (int)err);
    return;
  }
  if (result_type != CS_CMD_SUCCEED) {
    assert(result_type == CS_CMD_FAIL);
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call ct_results() for ct_dynamic(CS_PREPARE, '%s') failed with error %d", sql_text, (int)err);
    return;
  }
  while((err = ct_results(wrapper(), &result_type)) == CS_SUCCEED);
}

//------------------------------------------------------------------------------
void sybase_low_level_initiate_lang_command(const sybase_command_wrapper& wrapper, const char* sql_text, ExceptionSink* xsink)
{
  assert(sql_text && sql_text[0]);

  CS_RETCODE err = ct_command(wrapper(), CS_LANG_CMD, (CS_CHAR*)sql_text, CS_NULLTERM, CS_UNUSED);
  if (err != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call ct_command(CS_LANG_CMD, \"%s\") failed with error %d", sql_text, (int)err);
    return;
  }
}

//------------------------------------------------------------------------------
std::vector<parameter_info_t> sybase_low_level_get_input_parameters_info(
  const sybase_command_wrapper& wrapper, ExceptionSink* xsink)
{
  typedef std::vector<parameter_info_t> result_t;
  result_t result;

printf("##### command ID = [%s]\n", wrapper.getStringId());
  CS_RETCODE err = ct_dynamic(wrapper(), CS_DESCRIBE_INPUT, wrapper.getStringId(), CS_NULLTERM, 0, CS_UNUSED);
  if (err != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call ct_dynamic(CS_DESCRIBE_INPUT) failed with error %d", (int)err);
    return result;
  }

  err = ct_send(wrapper());
  if (err != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call ct_send() failed with error %d", (int)err);
    return result;
  }

  CS_INT result_type;
  while ((err = ct_results(wrapper(), &result_type)) == CS_SUCCEED) {
    if (result_type != CS_DESCRIBE_RESULT) {
      continue;
    }

    CS_INT numparam = 0;
    CS_INT len;
    err = ct_res_info(wrapper(), CS_NUMDATA, &numparam, CS_UNUSED, &len); // get # of input params
    if (err != CS_SUCCEED) {
      assert(false);
      xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call to ct_res_info(CS_DESCRIBE_RESULT) failed with error %d", (int)err);
      return result;
    }
    result.reserve(numparam);

    CS_DATAFMT datafmt;
    for (CS_INT i = 1; i <= numparam; ++i) {
      err = ct_describe(wrapper(), i, &datafmt);
      if (err != CS_SUCCEED) {
        assert(false);
        xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call to ct_describe(%d) failed with error %d", i, (int)err);
        result.clear();
        return result;
      }
      std::string name;
      if (datafmt.name[0]) {
        name = datafmt.name;
      } 
      result.push_back(parameter_info_t(name, datafmt.datatype, datafmt.maxlength));
    }
  }

  return result;
}

//------------------------------------------------------------------------------
std::vector<parameter_info_t> sybase_low_level_get_output_data_info(
  const sybase_command_wrapper& wrapper, ExceptionSink* xsink)
{
  typedef std::vector<parameter_info_t> result_t;
  result_t result;

  CS_RETCODE err = ct_dynamic(wrapper(), CS_DESCRIBE_OUTPUT, wrapper.getStringId(), CS_NULLTERM, 0, CS_UNUSED);
  if (err != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call ct_dynamic(CS_DESCRIBE_OUTPUT) failed with error %d", (int)err);
    return result;
  }

  err = ct_send(wrapper());
  if (err != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call ct_send() failed with error %d", (int)err);
    return result;
  }

  CS_INT result_type;
  while ((err = ct_results(wrapper(), &result_type)) == CS_SUCCEED) {
    if (result_type != CS_DESCRIBE_RESULT) {
      continue;
    }

    CS_INT numparam = 0;
    CS_INT len;
    err = ct_res_info(wrapper(), CS_NUMDATA, &numparam, CS_UNUSED, &len); // get # of out columns
    if (err != CS_SUCCEED) {
      assert(false);
      xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call to ct_res_info(CS_DESCRIBE_RESULT) failed with error %d", (int)err);
      return result;
    }
    result.reserve(numparam);

    CS_DATAFMT datafmt;
    for (CS_INT i = 1; i <= numparam; ++i) {
      err = ct_describe(wrapper(), i, &datafmt);
      if (err != CS_SUCCEED) {
        assert(false);
        xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call to ct_describe(%d) failed with error %d", i, (int)err);
        result.clear();
        return result;
      }
      std::string name;
      if (datafmt.name[0]) {
        name = datafmt.name;
      } 
      result.push_back(parameter_info_t(name, datafmt.datatype, datafmt.maxlength));
    }
  }

  return result;
}

//------------------------------------------------------------------------------
std::string sybase_low_level_get_default_encoding(const sybase_connection& conn, ExceptionSink* xsink)
{
  CS_LOCALE* locale;
  CS_RETCODE err = cs_loc_alloc(conn.getContext(), &locale);
  if (err != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call cs_loc_alloc() returned error %d", (int)err);
    return std::string();
  }
  ON_BLOCK_EXIT(cs_loc_drop, conn.getContext(), locale);

  CS_CHAR encoding_str[100] = "";
  err = cs_locale(conn.getContext(), CS_GET, locale, CS_SYB_CHARSET, encoding_str, sizeof(encoding_str), 0);
  if (err != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call cs_locale() returned error %d", (int)err);
    return std::string();
  }

  if (!encoding_str[0]) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call cs_locale() returned empty string for encoding");
    return std::string();
  }
  return std::string(encoding_str);
}

//------------------------------------------------------------------------------
void sybase_low_level_bind_parameters(
  const sybase_command_wrapper& wrapper,
  const QoreEncoding* encoding,
  const char* command,
  const std::vector<bind_parameter_t>& parameters,
  ExceptionSink* xsink
  )
{
  sybase_ct_dynamic(wrapper, CS_EXECUTE, xsink);
  if (xsink->isException()) {
    return;
  }

  // bind parameters (by position)
printf("### trying to bind %d params\n", parameters.size());
  for (unsigned i = 0, n = parameters.size(); i != n; ++i) {
    sybase_ct_param(wrapper, i, encoding, parameters[i].m_type, parameters[i].m_node, xsink); 
    if (xsink->isException()) {
      return;
    }
  }

printf("### before ct_send\n");
  CS_RETCODE err = ct_send(wrapper());
  if (err != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call ct_send() failed with error %d", (int)err);
    return;
  }  
printf("#### after ct_send\n");
}

//------------------------------------------------------------------------------
void execute_RPC_call(
  const sybase_command_wrapper& wrapper,
  const QoreEncoding* encoding,
  const char* RPC_command, // just name, w/o "exec[ute]" or parameters list
  const std::vector<RPC_parameter_info_t>& parameters,
  ExceptionSink* xsink
  )
{
  CS_RETCODE err = ct_command(wrapper(), CS_RPC_CMD, (CS_CHAR*)RPC_command, CS_NULLTERM, CS_NO_RECOMPILE);
  if (err != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call ct_command(CS_RPC_CMD) failed with error %d", (int)err);
    return;
  }

  // add all parameters (they need to be specified
  for (unsigned i = 0, n = (unsigned)parameters.size(); i != n; ++i) {
    // get the data type
    CS_DATAFMT datafmt;
    memset(&datafmt, 0, sizeof(datafmt));
    datafmt.datatype = parameters[i].m_type;
/*TBD
    datafmt.maxlength = parameters[i].m_size;
    datafmt.status = parameters[i].m_is_input ? CS_INPUTVALUE : CS_RETURN;

    err = ct_param(wrapper(), &datafmt, (CS_VOID*)parameters[i].m_data, parameters[i].m_size, parameters[i].m_is_null ? -1 : 0);
*/
    if (err != CS_SUCCEED) {
      assert(false);
      xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call ct_param() #%d failed with error %d", i + 1, (int)err);
      return;
    }
  }

  err = ct_send(wrapper());
  if (err != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call ct_send() failed with error %d", (int)err);
    return;
  }  
}

//------------------------------------------------------------------------------
void sybase_ct_dynamic(
  const sybase_command_wrapper& wrapper,
  int command_type_code,
  ExceptionSink* xsink
  )
{
  CS_RETCODE err = ct_dynamic(wrapper(), command_type_code, wrapper.getStringId(), CS_NULLTERM, 0, CS_UNUSED);
  if (err != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call ct_dynamic(%d) failed with error %d", command_type_code, (int)err);
  }
}

//------------------------------------------------------------------------------
// e.g. return "Apr" for 4
static const char* getMonthString(int mon)
{
  switch (mon) {
  case 1: return "Jan";
  case 2: return "Feb";
  case 3: return "Mar";
  case 4: return "Apr";
  case 5: return "May";
  case 6: return "Jun";
  case 7: return "Jul";
  case 8: return "Aug";
  case 9: return "Sep";
  case 10: return "Oct";
  case 11: return "Nov";
  case 12: return "Dec";
  default:
    assert(false);
    return "";
  }
}

//------------------------------------------------------------------------------
// create string in default Sybase format for datetimes to be converted by cs_convert later to DATETIME[4]
static std::string QoreDateTime2SybaseStringFormat(DateTime* dt)
{
  const char* mon = getMonthString(dt->getMonth());
  int day = dt->getDay();
  int year = dt->getYear();
  int hour = dt->getHour();
  const char* am_pm;
  if (hour > 12) {
    hour -= 12;
    am_pm = "PM";
  } else {
    am_pm = "AM";
  }
  int min = dt->getMinute();
  int sec = dt->getSecond();
  int millis = dt->getMillisecond();

  char buffer[100];
  // default Transact SQL datetime style
  sprintf(buffer, "%s %d %d %02d:%02d:%02d:%03d %s", mon, day, year, hour, min, sec, millis, am_pm);
  return std::string(buffer);
}

//------------------------------------------------------------------------------
// cs_convert() via string is the only available CT API function to create DATETIME
void convert_QoreDatetime2SybaseDatetime(CS_CONTEXT* context, DateTime* dt, CS_DATETIME& out, ExceptionSink* xsink)
{
  std::string string_dt = QoreDateTime2SybaseStringFormat(dt);
  CS_DATAFMT srcfmt;
  memset(&srcfmt, 0, sizeof(srcfmt));
  srcfmt.datatype = CS_CHAR_TYPE;
  srcfmt.maxlength = string_dt.size();
  srcfmt.format = CS_FMT_NULLTERM;

  CS_DATAFMT destfmt;
  memset(&destfmt, 0, sizeof(destfmt));
  destfmt.datatype = CS_DATETIME_TYPE;
  destfmt.maxlength = sizeof(CS_DATETIME);

  CS_INT aux;
  CS_RETCODE err = cs_convert(context, &srcfmt, (CS_BYTE*)string_dt.c_str(), &destfmt, (CS_BYTE*)&out, &aux);
  if (err != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "cs_convert() failed to convert date [%s] into Sybase CS_DATETIME, err %d", string_dt.c_str(), (int)err);
    return;
  }
}

//------------------------------------------------------------------------------
// cs_convert() via string is the only available CT API function to create DATETIME
void convert_QoreDatetime2SybaseDatetime4(CS_CONTEXT* context, DateTime* dt, CS_DATETIME4& out, ExceptionSink* xsink)
{
  std::string string_dt = QoreDateTime2SybaseStringFormat(dt);
  CS_DATAFMT srcfmt;
  memset(&srcfmt, 0, sizeof(srcfmt));
  srcfmt.datatype = CS_CHAR_TYPE;
  srcfmt.maxlength = string_dt.size();
  srcfmt.format = CS_FMT_NULLTERM;

  CS_DATAFMT destfmt;
  memset(&destfmt, 0, sizeof(destfmt));
  destfmt.datatype = CS_DATETIME4_TYPE;
  destfmt.maxlength = sizeof(CS_DATETIME4);

  CS_INT outlen;
  CS_RETCODE err = cs_convert(context, &srcfmt, (CS_BYTE*)string_dt.c_str(), &destfmt, (CS_BYTE*)&out, &outlen);
  if (err != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "cs_convert() failed to convert date [%s] into Sybase CS_DATETIME4, err %d", string_dt.c_str(), (int)err);
    return;
  }
}

//------------------------------------------------------------------------------
DateTime* convert_SybaseDatetime2QoreDatetime(CS_CONTEXT* context, CS_DATETIME& dt, ExceptionSink* xsink)
{
  CS_DATEREC x;
  memset(&x, 0, sizeof(x));  
  CS_RETCODE err = cs_dt_crack(context, CS_DATETIME_TYPE, &dt, &x);
  if (err != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "cs_dt_crack() failed with error %d", (int)err);
    return 0;
  }

  return new DateTime(x.dateyear, x.datemonth, x.datedmonth, x.datehour, x.dateminute, x.datesecond, x.datemsecond, false);
}

//------------------------------------------------------------------------------
DateTime* convert_SybaseDatetime4_2QoreDatetime(CS_CONTEXT* context, CS_DATETIME4& dt, ExceptionSink* xsink)
{
  CS_DATEREC x;
  memset(&x, 0, sizeof(x));
  CS_RETCODE err = cs_dt_crack(context, CS_DATETIME4_TYPE, &dt, &x);
  if (err != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "cs_dt_crack() failed with error %d", (int)err);
    return 0;
  }

  return new DateTime(x.dateyear, x.datemonth, x.datedmonth, x.datehour, x.dateminute, x.datesecond, x.datemsecond, false);
}

//------------------------------------------------------------------------------
void convert_float2SybaseMoney(CS_CONTEXT* context, double val, CS_MONEY& out, ExceptionSink* xsink)
{
  CS_DATAFMT srcfmt;
  memset(&srcfmt, 0, sizeof(srcfmt));
  srcfmt.datatype = CS_FLOAT_TYPE;
  srcfmt.maxlength = sizeof(CS_FLOAT);

  CS_DATAFMT destfmt;
  memset(&destfmt, 0, sizeof(destfmt));
  destfmt.datatype = CS_MONEY_TYPE;
  destfmt.maxlength = sizeof(CS_MONEY);

  CS_INT outlen;
  CS_RETCODE err = cs_convert(context, &srcfmt, (CS_BYTE*)&val, &destfmt, (CS_BYTE*)&out, &outlen);
  if (err != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "cs_convert() failed to convert a float value into Sybase MONEY, err %d", (int)err);
    return;
  }
}

//------------------------------------------------------------------------------
void convert_float2SybaseMoney4(CS_CONTEXT* context, double val, CS_MONEY4& out, ExceptionSink* xsink)
{
  CS_DATAFMT srcfmt;
  memset(&srcfmt, 0, sizeof(srcfmt));
  srcfmt.datatype = CS_FLOAT_TYPE;
  srcfmt.maxlength = sizeof(CS_FLOAT);

  CS_DATAFMT destfmt;
  memset(&destfmt, 0, sizeof(destfmt));
  destfmt.datatype = CS_MONEY4_TYPE;
  destfmt.maxlength = sizeof(CS_MONEY4);

  CS_INT outlen;
  CS_RETCODE err = cs_convert(context, &srcfmt, (CS_BYTE*)&val, &destfmt, (CS_BYTE*)&out, &outlen);
  if (err != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "cs_convert() failed to convert a float value into Sybase MONEY4, err %d", (int)err);
    return;
  }
}

//------------------------------------------------------------------------------
double convert_SybaseMoney2float(CS_CONTEXT* context, CS_MONEY& m, ExceptionSink* xsink)
{
  CS_DATAFMT srcfmt;
  memset(&srcfmt, 0, sizeof(srcfmt));
  srcfmt.datatype = CS_MONEY_TYPE;
  srcfmt.maxlength = sizeof(CS_MONEY);

  CS_DATAFMT destfmt;
  memset(&destfmt, 0, sizeof(destfmt));
  destfmt.datatype = CS_FLOAT_TYPE;
  destfmt.maxlength = sizeof(CS_FLOAT);

  CS_INT outlen;
  CS_FLOAT result = 0.0;
  CS_RETCODE err = cs_convert(context, &srcfmt, (CS_BYTE*)&m, &destfmt, (CS_BYTE*)&result, &outlen);
  if (err != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "cs_convert() failed to convert Sybase MONEY into FLOAT, err %d", (int)err);
  }

  return result;
}

//------------------------------------------------------------------------------
double convert_SybaseMoney4_2float(CS_CONTEXT* context, CS_MONEY4& m, ExceptionSink* xsink)
{
  CS_DATAFMT srcfmt;
  memset(&srcfmt, 0, sizeof(srcfmt));
  srcfmt.datatype = CS_MONEY4_TYPE;
  srcfmt.maxlength = sizeof(CS_MONEY4);

  CS_DATAFMT destfmt;
  memset(&destfmt, 0, sizeof(destfmt));
  destfmt.datatype = CS_FLOAT_TYPE;
  destfmt.maxlength = sizeof(CS_FLOAT);

  CS_INT outlen;
  CS_FLOAT result = 0.0;
  CS_RETCODE err = cs_convert(context, &srcfmt, (CS_BYTE*)&m, &destfmt, (CS_BYTE*)&result, &outlen);
  if (err != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "cs_convert() failed to convert Sybase MONEY4 into FLOAT, err %d", (int)err);
  }

  return result;
}

//------------------------------------------------------------------------------
void convert_float2SybaseDecimal(CS_CONTEXT* context, double val, CS_DECIMAL& out, ExceptionSink* xsink)
{
  CS_DATAFMT srcfmt;
  memset(&srcfmt, 0, sizeof(srcfmt));
  srcfmt.datatype = CS_FLOAT_TYPE;
  srcfmt.maxlength = sizeof(CS_FLOAT);

  CS_DATAFMT destfmt;
  memset(&destfmt, 0, sizeof(destfmt));
  destfmt.datatype = CS_DECIMAL_TYPE;
  destfmt.maxlength = 35; // recommended by docs
  destfmt.scale = 15; // # of digits after decimal point, guess 
  destfmt.precision = 30; // total # of digits in number, also guess
 
#ifdef DEBUG
  memset(&out, 0, sizeof(CS_DECIMAL));
#endif 
  CS_INT outlen;
  CS_RETCODE err = cs_convert(context, &srcfmt, (CS_BYTE*)&val, &destfmt, (CS_BYTE*)&out, &outlen);
  if (err != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "cs_convert() failed to convert a float value into Sybase DECIMAL, err %d", (int)err);
    return;
  }
  assert(out.precision);
  assert(out.scale);
  assert(out.precision > out.scale);
}

//------------------------------------------------------------------------------
double convert_SybaseDecimal2float(CS_CONTEXT* context, CS_DECIMAL& m, ExceptionSink* xsink)
{
  CS_DATAFMT srcfmt;
  memset(&srcfmt, 0, sizeof(srcfmt));
  srcfmt.datatype = CS_DECIMAL_TYPE;
  srcfmt.maxlength = 35; // recommended by docs
  srcfmt.scale = 15; // guess, keep the same as above
  srcfmt.precision = 30; // also guess


  CS_DATAFMT destfmt;
  memset(&destfmt, 0, sizeof(destfmt));
  destfmt.datatype = CS_FLOAT_TYPE;
  destfmt.maxlength = sizeof(CS_FLOAT);

  CS_INT outlen;
  CS_FLOAT result = 0.0;
  CS_RETCODE err = cs_convert(context, &srcfmt, (CS_BYTE*)&m, &destfmt, (CS_BYTE*)&result, &outlen);
  if (err != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "cs_convert() failed to convert Sybase DECIMAL into FLOAT, err %d", (int)err);
  }

  return result;
}

//------------------------------------------------------------------------------
void convert_float2SybaseNumeric(CS_CONTEXT* context, double val, CS_NUMERIC& out, ExceptionSink* xsink)
{
  CS_DATAFMT srcfmt;
  memset(&srcfmt, 0, sizeof(srcfmt));
  srcfmt.datatype = CS_FLOAT_TYPE;
  srcfmt.maxlength = sizeof(CS_FLOAT);

  CS_DATAFMT destfmt;
  memset(&destfmt, 0, sizeof(destfmt));
  destfmt.datatype = CS_NUMERIC_TYPE;
  destfmt.maxlength = 35; // recommended by docs
  destfmt.scale = 15; // # of digits after decimal point, guess
  destfmt.precision = 30; // total # of digits in number, also guess

  CS_INT outlen;
  CS_RETCODE err = cs_convert(context, &srcfmt, (CS_BYTE*)&val, &destfmt, (CS_BYTE*)&out, &outlen);
  if (err != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "cs_convert() failed to convert a float value into Sybase NUMERIC, err %d", (int)err);
    return;
  }
}

//------------------------------------------------------------------------------
double convert_SybaseNumeric2float(CS_CONTEXT* context, CS_NUMERIC& m, ExceptionSink* xsink)
{
  CS_DATAFMT srcfmt;
  memset(&srcfmt, 0, sizeof(srcfmt));
  srcfmt.datatype = CS_NUMERIC_TYPE;
  srcfmt.maxlength = 35; // recommended by docs
  srcfmt.scale = 15; // guess, keep the same as above
  srcfmt.precision = 30; // also guess


  CS_DATAFMT destfmt;
  memset(&destfmt, 0, sizeof(destfmt));
  destfmt.datatype = CS_FLOAT_TYPE;
  destfmt.maxlength = sizeof(CS_FLOAT);

  CS_INT outlen;
  CS_FLOAT result = 0.0;
  CS_RETCODE err = cs_convert(context, &srcfmt, (CS_BYTE*)&m, &destfmt, (CS_BYTE*)&result, &outlen);
  if (err != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "cs_convert() failed to convert Sybase NUMERIC into FLOAT, err %d", (int)err);
  }

  return result;
}

//------------------------------------------------------------------------------
void sybase_ct_param(
  const sybase_command_wrapper& wrapper,
  unsigned parameter_index,
  const QoreEncoding* encoding,
  int type, // like CS_INT_TYPE
  QoreNode* data,
  ExceptionSink* xsink
  )
{
  CS_DATAFMT datafmt;
  memset(&datafmt, 0, sizeof(datafmt));
  datafmt.status = CS_INPUTVALUE;
  datafmt.namelen = CS_NULLTERM;
  datafmt.maxlength = CS_UNUSED;
  datafmt.count = 1;
  
  CS_RETCODE err;
  
  if (data->type == NT_NULL || data->type == NT_NOTHING) {
    // SQL NULL value
    err = ct_param(wrapper(), &datafmt, 0, CS_UNUSED, -1);
    if (err != CS_SUCCEED) {
      assert(false);
      xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call ct_param(NULL) failed for parameter #%u with error %d", parameter_index + 1, (int)err);
    }
    return;
  }

  switch (type) {
  case CS_LONGCHAR_TYPE: // Sybase types char, varchar (also image when used in WHERE x LIKE ? - this is likely a bug)
  case CS_VARCHAR_TYPE:
  case CS_CHAR_TYPE: 
  case CS_TEXT_TYPE: // text could be used only with LIKE in WHERE statement, nowhere else
  {
    if (data->type != NT_STRING) {
      assert(false);
      xsink->raiseException("DBI-EXEC-EXCEPTION", "Incorrect type for string parameter #%u", parameter_index + 1);
      return;
    }

    TempEncodingHelper s(data->val.String, (QoreEncoding*)encoding, xsink);
    if (xsink->isException()) {
      return;
    }

    const char* s2 = "";
    if (*s && s->getBuffer()) {
      s2 = s->getBuffer();
    }
    datafmt.datatype = type;
    datafmt.format = CS_FMT_NULLTERM;
    datafmt.maxlength = (2 * 1024 * 1024 * 1024 - 1); // 2GB for text

    err = ct_param(wrapper(), &datafmt, (CS_VOID*)s2, strlen(s2), 0);
    if (err != CS_SUCCEED) {
      assert(false);
      xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase function ct_param() for string parameter #%u failed with error", parameter_index + 1, (int)err);
        return;
      }
  }
  return;

  case CS_TINYINT_TYPE:
  {
    if (data->type != NT_INT) {
      assert(false);
      xsink->raiseException("DBI-EXEC-EXCEPTION", "Incorrect type for integer parameter #%u", parameter_index + 1);
      return;
    }
    if (data->val.intval < -128 || data->val.intval >= 128) {
      assert(false);
      xsink->raiseException("DBI-EXEC-EXCEPTION", "Integer value (#%d parameter) is out of range for Sybase datatype", parameter_index + 1);
      return;
    }

    CS_TINYINT val = data->val.intval;
    datafmt.datatype = CS_TINYINT_TYPE;
    err = ct_param(wrapper(), &datafmt, &val, sizeof(val), 0);
    if (err != CS_SUCCEED) {
      assert(false);
      xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase function ct_param() for integer parameter #%u failed with error", parameter_index + 1, (int)err);
      return;
    }
  }
  return;

  case CS_SMALLINT_TYPE:
  {
    if (data->type != NT_INT) {
      assert(false);
      xsink->raiseException("DBI-EXEC-EXCEPTION", "Incorrect type for integer parameter #%u", parameter_index + 1);
      return;
    }
    if (data->val.intval < -32 * 1024 || data->val.intval >= 32 * 1024) {
      assert(false);
      xsink->raiseException("DBI-EXEC-EXCEPTION", "Integer value (#%d parameter) is out of range for Sybase datatype", parameter_index + 1);
      return;
    }

    CS_SMALLINT val = data->val.intval;
    datafmt.datatype = CS_SMALLINT_TYPE;
    err = ct_param(wrapper(), &datafmt, &val, sizeof(val), 0);
    if (err != CS_SUCCEED) {
      assert(false);
      xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase function ct_param() for integer parameter #%u failed with error", parameter_index + 1, (int)err);
      return;
    }
  }
  return;

  case CS_INT_TYPE:
  {
    if (data->type != NT_INT) {
      assert(false);
      xsink->raiseException("DBI-EXEC-EXCEPTION", "Incorrect type for integer parameter #%u", parameter_index + 1);
      return;
    }
    CS_INT val = data->val.intval;
    datafmt.datatype = CS_INT_TYPE;
    err = ct_param(wrapper(), &datafmt, &val, sizeof(val), 0);
    if (err != CS_SUCCEED) {
      assert(false);
      xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase function ct_param() for integer parameter #%u failed with error", parameter_index + 1, (int)err);
      return;
    }
  }
  return;

  case CS_IMAGE_TYPE: // image could be used only with LIKE in WHERE statement, nowhere else
printf("######### IMAGE TYPE FOR PREPARE\n");
  case CS_LONGBINARY_TYPE:
printf("###### IS LONG BINARY ######################################################\n");
  case CS_BINARY_TYPE:
  {
    if (data->type != NT_BINARY) {
      assert(false);
      xsink->raiseException("DBI-EXEC-EXCEPTION", "Incorrect type for binary parameter #%u", parameter_index + 1);
      return;
    }

    datafmt.datatype = type;
    datafmt.maxlength = data->val.bin->size();
    datafmt.count = 1;
    err = ct_param(wrapper(), &datafmt, data->val.bin->getPtr(), data->val.bin->size(), 0);
    if (err != CS_SUCCEED) {
      assert(false);
      xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase function ct_param() for binary parameter #%u failed with error", parameter_index + 1, (int)err);
      return;
    }
  }
  return;

  case CS_VARBINARY_TYPE:
  {
    assert(false); // should never be actually returned
    if (data->type != NT_BINARY) {
      assert(false);
      xsink->raiseException("DBI-EXEC-EXCEPTION", "Incorrect type for varbinary parameter #%u", parameter_index + 1);
      return;
    }
    int size = data->val.bin->size();
    if (size > CS_MAX_CHAR) {
      assert(false);
      xsink->raiseException("DBI-EXEC-EXCEPTION", "Parameter #%u: varbinary supports up to %d B (attempt to insert %d B)", parameter_index + 1, CS_MAX_CHAR, size);
      return;
    }

    datafmt.datatype = type;
    datafmt.maxlength = size;
    CS_VARBINARY varbin; // required by ct_param()
    varbin.len = size;
    memcpy(varbin.array, data->val.bin->getPtr(), size);

    err = ct_param(wrapper(), &datafmt, &varbin, size, 0);
    if (err != CS_SUCCEED) {
      assert(false);
      xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase function ct_param() for binary parameter #%u failed with error", parameter_index + 1, (int)err);
      return;
    }
  }
  return;

  case CS_REAL_TYPE:
  {
    if (data->type != NT_FLOAT && data->type != NT_INT) {
      assert(false);
      xsink->raiseException("DBI-EXEC-EXCEPTION", "Incorrect type for float parameter #%u", parameter_index + 1);
      return;
    }
    CS_REAL val;
    if (data->type == NT_FLOAT) {
      val = data->val.floatval;
    } else {
      val = data->val.intval;
    }
    datafmt.datatype = CS_REAL_TYPE;
    err = ct_param(wrapper(), &datafmt, &val, sizeof(val), 0);
    if (err != CS_SUCCEED) {
      assert(false);
      xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase function ct_param() for float parameter #%u failed with error", parameter_index + 1, (int)err);
      return;
    }
  }
  return;

  case CS_FLOAT_TYPE:
  {
    if (data->type != NT_FLOAT && data->type != NT_INT) {
      assert(false);
      xsink->raiseException("DBI-EXEC-EXCEPTION", "Incorrect type for float parameter #%u", parameter_index + 1);
      return;
    }
    CS_FLOAT val;
    if (data->type == NT_FLOAT) {
      val = data->val.floatval;
    } else {
      val = data->val.intval;
    }
    datafmt.datatype = CS_FLOAT_TYPE;
    err = ct_param(wrapper(), &datafmt, &val, sizeof(val), 0);
    if (err != CS_SUCCEED) {
      assert(false);
      xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase function ct_param() for float parameter #%u failed with error", parameter_index + 1, (int)err);
      return;
    }
  }
  return;

  case CS_BIT_TYPE:
  {
    if (data->type != NT_BOOLEAN) {
      assert(false);
      xsink->raiseException("DBI-EXEC-EXCEPTION", "Incorrect type for boolean parameter #%u", parameter_index + 1);
      return;
    }
    CS_BIT val = data->val.boolval ? 1 : 0;
    datafmt.datatype = CS_BIT_TYPE;
    err = ct_param(wrapper(), &datafmt, &val, sizeof(val), 0);
    if (err != CS_SUCCEED) {
      assert(false);
      xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase function ct_param() for bool parameter #%u failed with error", parameter_index, (int)err);
      return;
    }
  }
  return;

  case CS_DATETIME_TYPE:
  {
    if (data->type != NT_DATE) {
      assert(false);
      xsink->raiseException("DBI-EXEC-EXCEPTION", "Incorrect type for datetime parameter #%u", parameter_index + 1);
      return;
    }

    CS_DATETIME dt;
    convert_QoreDatetime2SybaseDatetime(wrapper.getContext(), data->val.date_time, dt, xsink);
    if (xsink->isException()) {
      assert(false);
      return;
    }

    datafmt.datatype = CS_DATETIME_TYPE;
    err = ct_param(wrapper(), &datafmt, &dt, sizeof(dt), 0);
    if (err != CS_SUCCEED) {
      assert(false);
      xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase function ct_param() for datetime parameter #%u failed with error", parameter_index, (int)err);
      return;
    }
  }
  return;

  case CS_DATETIME4_TYPE:
  {
    if (data->type != NT_DATE) {
      assert(false);
      xsink->raiseException("DBI-EXEC-EXCEPTION", "Incorrect type for datetime parameter #%u", parameter_index + 1);
      return;
    }

    CS_DATETIME4 dt;
    convert_QoreDatetime2SybaseDatetime4(wrapper.getContext(), data->val.date_time, dt, xsink);
    if (xsink->isException()) {      
      assert(false);
      return;
    }

    datafmt.datatype = CS_DATETIME4_TYPE;
    err = ct_param(wrapper(), &datafmt, &dt, sizeof(dt), 0);
    if (err != CS_SUCCEED) {
      assert(false);
      xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase function ct_param() for datetime parameter #%u failed with error", parameter_index, (int)err);
      return;
    }
  }
  return;

  case CS_MONEY_TYPE:
  {
    if (data->type != NT_FLOAT && data->type != NT_INT) {
      assert(false);
      xsink->raiseException("DBI-EXEC-EXCEPTION", "Incorrect type for money parameter #%u (integer or float expected)", parameter_index + 1);
      return;
    }
    double val;
    if (data->type == NT_FLOAT) {
      val = data->val.floatval;
    } else {
      val = data->val.intval;
    }
    CS_MONEY m;
    convert_float2SybaseMoney(wrapper.getContext(), val, m, xsink);
    if (xsink->isException()) {
      return;
    }
    datafmt.datatype = CS_MONEY_TYPE;
    err = ct_param(wrapper(), &datafmt, &m, sizeof(m), 0);
    if (err != CS_SUCCEED) {
      assert(false);
      xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase function ct_param() for money parameter #%u failed with error", parameter_index, (int)err);
      return;
    }
  }
  return;

  case CS_MONEY4_TYPE:
  {
    if (data->type != NT_FLOAT && data->type != NT_INT) {
      assert(false);
      xsink->raiseException("DBI-EXEC-EXCEPTION", "Incorrect type for smallmoney parameter #%u (integer or float expected)", parameter_index + 1);
      return;
    }
    double val;
    if (data->type == NT_FLOAT) {
      val = data->val.floatval;
    } else {
      val = data->val.intval;
    }
    CS_MONEY4 m;
    convert_float2SybaseMoney4(wrapper.getContext(), val, m, xsink);
    if (xsink->isException()) {
      return;
    }
    datafmt.datatype = CS_MONEY4_TYPE;
    err = ct_param(wrapper(), &datafmt, &m, sizeof(m), 0);
    if (err != CS_SUCCEED) {
      assert(false);
      xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase function ct_param() for smallmoney parameter #%u failed with error", parameter_index, (int)err);
      return;
    }
  }
  return;

  case CS_DECIMAL_TYPE:
  {
    if (data->type != NT_FLOAT && data->type != NT_INT) {
      assert(false);
      xsink->raiseException("DBI-EXEC-EXCEPTION", "Incorrect type for decimal parameter #%u (integer or float expected)", parameter_index + 1);
      return;
    }
    double val;
    if (data->type == NT_FLOAT) {
      val = data->val.floatval;
    } else {
      val = data->val.intval;
    }
    CS_DECIMAL dc;
    convert_float2SybaseDecimal(wrapper.getContext(), val, dc, xsink);
    if (xsink->isException()) {
      return;
    }
    datafmt.datatype = CS_DECIMAL_TYPE;
    datafmt.precision = 30; // guess
    datafmt.scale = 15; // guess
    err = ct_param(wrapper(), &datafmt, &dc, sizeof(dc), 0);
    if (err != CS_SUCCEED) {
      assert(false);
      xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase function ct_param() for decimal parameter #%u failed with error", parameter_index, (int)err);
      return;
    }
  }
  return;

  case CS_NUMERIC_TYPE:
  {
    if (data->type != NT_FLOAT && data->type != NT_INT) {
      assert(false);
      xsink->raiseException("DBI-EXEC-EXCEPTION", "Incorrect type for numeric parameter #%u (integer or float expected)", parameter_index + 1);
      return;
    }
    double val;
    if (data->type == NT_FLOAT) {
      val = data->val.floatval;
    } else {
      val = data->val.intval;
    }
    CS_NUMERIC dc;
    convert_float2SybaseNumeric(wrapper.getContext(), val, dc, xsink);
    if (xsink->isException()) {
      return;
    }
    datafmt.datatype = CS_NUMERIC_TYPE;
    datafmt.precision = 30; // guess
    datafmt.scale = 15; // guess
    err = ct_param(wrapper(), &datafmt, &dc, sizeof(dc), 0);
    if (err != CS_SUCCEED) {
      assert(false);
      xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase function ct_param() for numeric parameter #%u failed with error", parameter_index, (int)err);
      return;
    }
  }
  return;

  default:
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Unrecognized type %d of Sybase parameter # %u", type, parameter_index + 1);
    return;
  } // switch 
}

#ifdef DEBUG
//#  include "tests/sybase_low_level_interface_tests.cc"
#endif

// EOF

