/*
  sybase_read_output.cc

  Sybase DB layer for QORE
  uses Sybase OpenClient C library

  Qore Programming language

  Copyright (C) 2007

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
#include <qore/charset.h>
#include <qore/QoreNode.h>
#include <qore/Hash.h>
#include <qore/List.h>
#include <qore/QoreString.h>
#include <qore/QoreType.h>
#include <qore/DateTime.h>

#include <assert.h>
#include <qore/minitest.hpp>
#include <qore/ScopeGuard.h>
#include <ctpublic.h>

#include "sybase_read_output.h"
#include "sybase_low_level_interface.h"

//------------------------------------------------------------------------------
static unsigned sybase_get_columns_count(const sybase_command_wrapper& wrapper, ExceptionSink* xsink)
{
  CS_INT num_cols;
  CS_RETCODE err = ct_res_info(wrapper(), CS_NUMDATA, &num_cols, CS_UNUSED, NULL);
  if (err != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call ct_res_info() failed to get number of columns, error %d", (int)err);
    return 0;
  }
  if (num_cols <= 0) {
    assert(false); // cannot happen
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Internal error: no columns returned");
    return 0;
  }
  return num_cols;
}

//------------------------------------------------------------------------------
// From exutils.h in samples.
// Used to keep the read results
typedef struct _ex_column_data
{
  CS_SMALLINT     indicator;
  CS_CHAR*        value; // owned
  CS_INT          valuelen;
} EX_COLUMN_DATA;

//------------------------------------------------------------------------------
// helper to free allocated data
static void free_coldata(EX_COLUMN_DATA* coldata, CS_INT cnt)
{
  for (CS_INT i = 0; i < cnt; ++i) {
    if (coldata[i].value) {
      free(coldata[i].value);
    }
  }
  free(coldata);
}

//------------------------------------------------------------------------------
static void extract_row_data_to_Hash(const sybase_command_wrapper& wrapper, Hash* out, unsigned col_index, CS_DATAFMT* datafmt, EX_COLUMN_DATA* coldata, const QoreEncoding* encoding, std::vector<parameter_info_t>& outputs_info, ExceptionSink* xsink)
{
  std::string column_name;
  assert(col_index < outputs_info.size());
  if (!outputs_info[col_index].m_name.empty()) {
    column_name = outputs_info[col_index].m_name;
  } else {
    char buffer[20];
    sprintf(buffer, "column%d", (int)(col_index + 1));
    column_name = buffer;
  }

  std::auto_ptr<QoreString> key(new QoreString(column_name.c_str()));
  QoreNode* v = 0;

  if (coldata->indicator == -1) { // NULL
    out->setKeyValue(key.get(), new QoreNode(NT_NULL), xsink);
    return;
  }

  switch (datafmt->datatype) {
  case CS_LONGCHAR_TYPE:
  case CS_VARCHAR_TYPE:
  case CS_TEXT_TYPE:
  case CS_CHAR_TYPE: // varchar
  {
    CS_CHAR* value = (CS_CHAR*)(coldata->value);
    QoreString* s = new QoreString(value, coldata->valuelen, (QoreEncoding*)encoding);
    v = new QoreNode(s);
    break;
  }
  case CS_VARBINARY_TYPE:
  case CS_BINARY_TYPE:
  case CS_LONGBINARY_TYPE:
  case CS_IMAGE_TYPE:
    // TBD
    assert(false);
  case CS_TINYINT_TYPE:
  {
    CS_TINYINT* value = (CS_TINYINT*)(coldata->value);
    v = new QoreNode((int64)*value);
    break;
  }
  case CS_SMALLINT_TYPE:
  {
    CS_SMALLINT* value = (CS_SMALLINT*)(coldata->value);
    v = new QoreNode((int64)*value);
    break;
  }
  case CS_INT_TYPE:
  {
printf("### I READ INT NOW\n");
    CS_INT* value = (CS_INT*)(coldata->value);
    v = new QoreNode((int64)*value);
    break;
  }
  case CS_REAL_TYPE:
  {
    CS_REAL* value = (CS_REAL*)(coldata->value);
    v = new QoreNode((double)*value);
    break;
  }
  case CS_FLOAT_TYPE:
  {
    CS_FLOAT* value = (CS_FLOAT*)(coldata->value);
    v = new QoreNode((double)*value);
    break;
  }
  case CS_BIT_TYPE:
  {
    CS_BIT* value = (CS_BIT*)(coldata->value);
    v = new QoreNode(*value != 0);
    break;
  }
  case CS_DATETIME_TYPE:
  {
    CS_DATETIME* value = (CS_DATETIME*)(coldata->value);
    DateTime* dt = convert_SybaseDatetime2QoreDatetime(wrapper.getContext(), *value, xsink);
    if (xsink->isException()) {
      if (dt) delete dt;
      return;
    }
    v = new QoreNode(dt);
    break;
  }
  case CS_DATETIME4_TYPE:
  {
    CS_DATETIME4* value = (CS_DATETIME4*)(coldata->value);
    DateTime* dt = convert_SybaseDatetime4_2QoreDatetime(wrapper.getContext(), *value, xsink);
    if (xsink->isException()) {
      if (dt) delete dt;
      return;
    }
    v = new QoreNode(dt);
    break;
  }
  case CS_MONEY_TYPE:
  {
    CS_MONEY* value = (CS_MONEY*)(coldata->value);
    double d = convert_SybaseMoney2float(wrapper.getContext(), *value, xsink);
    if (xsink->isException()) {
      return;
    }
    v = new QoreNode(d);
    break;
  }
  case CS_MONEY4_TYPE:
  {
    CS_MONEY4* value = (CS_MONEY4*)(coldata->value);
    double d = convert_SybaseMoney4_2float(wrapper.getContext(), *value, xsink);
    if (xsink->isException()) {
      return;
    }
    v = new QoreNode(d);
    break;
  }
  case CS_NUMERIC_TYPE:
  {
    CS_NUMERIC* value = (CS_NUMERIC*)(coldata->value);
    double d = convert_SybaseNumeric2float(wrapper.getContext(), *value, xsink);
    if (xsink->isException()) {
      return;
    }
    v = new QoreNode(d);
    break;
  }
  case CS_DECIMAL_TYPE:
  {
    CS_DECIMAL* value = (CS_DECIMAL*)(coldata->value);
    double d = convert_SybaseDecimal2float(wrapper.getContext(), *value, xsink);
    if (xsink->isException()) {
      return;
    }
    v = new QoreNode(d);
    break;
  }
  default:
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Unknown data type %d", (int)datafmt->datatype);
    return;
  }
  assert(out);
  out->setKeyValue(key.get(), v, xsink);
}

//------------------------------------------------------------------------------
// read single row
static void sybase_read_row(const sybase_command_wrapper& wrapper, QoreNode*& out, const QoreEncoding* encoding, std::vector<parameter_info_t>& outputs_info, ExceptionSink* xsink)
{
  unsigned num_cols = sybase_get_columns_count(wrapper, xsink);
  if (xsink->isException()) {
    return;
  }
  if (num_cols != outputs_info.size()) {
    // something strange: at least the out column names are not reliable 
    // so generated names will be always used
printf("###!!!!!! expected # of outputs = %d, actual # of outputs = %d\n", outputs_info.size(), num_cols);
//###    assert(false);
//###    outputs_info.clear();
  }

  // allocate helper structures for read data 
  EX_COLUMN_DATA* coldata = (EX_COLUMN_DATA *)malloc(num_cols * sizeof (EX_COLUMN_DATA));
  if (!coldata) {
    xsink->outOfMemory();
    return;
  }
  memset(coldata, 0, num_cols * sizeof(EX_COLUMN_DATA));
  ON_BLOCK_EXIT(free_coldata, coldata, num_cols);

  CS_DATAFMT* datafmt = (CS_DATAFMT *)malloc(num_cols * sizeof (CS_DATAFMT));
  if (!datafmt) {
    assert(false);
    xsink->outOfMemory();
    return;
  }
  ON_BLOCK_EXIT(free, datafmt);
  memset(datafmt, 0, num_cols * sizeof(CS_DATAFMT));

  CS_RETCODE err;
  // bind the data buffers
  for (unsigned i = 0; i < num_cols; ++i) {
    err = ct_describe(wrapper(), i + 1, &datafmt[i]);
    if (err != CS_SUCCEED) {
      assert(false);
      xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call ct_describe() failed with error %d", (int)err);
      return;
    }
    datafmt[i].count = 1; // fetch just single item
    assert(datafmt[i].maxlength < 100000); // guess, if invalid then app semnatic is wrong

    datafmt[i].maxlength += 4; // some padding for zero terminator, 4 is safe bet
    coldata[i].value = (CS_CHAR*)malloc(datafmt[i].maxlength);
    if (!coldata[i].value) {
      assert(false);
      xsink->outOfMemory();
      return;
    }
    
    switch (datafmt[i].datatype) {
    case CS_LONGCHAR_TYPE:
    case CS_VARCHAR_TYPE:
    case CS_CHAR_TYPE:
    case CS_TEXT_TYPE:
      datafmt[i].format = CS_FMT_NULLTERM;
      break;
    case CS_DECIMAL_TYPE:
    case CS_NUMERIC_TYPE: // the same datatypes
      datafmt[i].precision = 30; // guess, total # of digits
      datafmt[i].scale = 15; // guess, # of digits after decimal dot
      datafmt[i].format = CS_FMT_UNUSED;
      break;

    default:
      datafmt[i].format = CS_FMT_UNUSED;
      break;
    }

    err = ct_bind(wrapper(), i + 1, &datafmt[i], coldata[i].value, &coldata[i].valuelen, &coldata[i].indicator);
    if (err != CS_SUCCEED) {
      assert(false);
      xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call ct_bind() failed with error %d", (int)err);
      return;
    }
  } // for

  // read the row
  CS_INT rows_read = 0;
  while ((err = ct_fetch(wrapper(), CS_UNUSED, CS_UNUSED, CS_UNUSED, &rows_read)) == CS_SUCCEED) {
    // process the row
    Hash* h = new Hash;

    for (unsigned j = 0; j < num_cols; ++j) {
      extract_row_data_to_Hash(wrapper, h, j, &datafmt[j], &coldata[j], encoding, outputs_info, xsink);
      if (xsink->isException()) {
        assert(false);
        QoreNode* aux = new QoreNode(h);
        aux->deref(xsink);
        return;
      }
      assert(h->size() > 0);
    }

    if (out == 0) {
      out = new QoreNode(h);
    } else
    if (out->type == NT_HASH) {
      // convert to list
      List* l = new List;
      l->push(out);
      l->push(new QoreNode(h));
      out = new QoreNode(l);
    } else {
      assert(out->type == NT_LIST);
      out->val.list->push(new QoreNode(h));
    }
  }

  if (err != CS_END_DATA) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call ct_fetch() returned erro %d", (int)err);
    return;
  }
}


//------------------------------------------------------------------------------
QoreNode* convert_sybase_output_to_Qore(const sybase_command_wrapper& wrapper, const QoreEncoding* encoding, const processed_sybase_query& query_info, std::vector<parameter_info_t>& outputs_info, ExceptionSink* xsink)
{
  QoreNode* result = 0;
  CS_RETCODE err;

  CS_INT result_type; 
  while ((err = ct_results(wrapper(), &result_type)) == CS_SUCCEED) {
printf("#### cs_results() retured %d\n", (int)result_type);
    switch (result_type) {
    case CS_CURSOR_RESULT:
      assert(false); // cannot happen, bug in driver
      xsink->raiseException("DBI-EXEC-EXCEPTION", "Unexpected CS_CURSOR_RESULT: returned by ct_results()");
      return result;

    case CS_COMPUTE_RESULT:
    case CS_PARAM_RESULT: // procedure call
    case CS_ROW_RESULT:
      // 0 or more rows
      sybase_read_row(wrapper, result, encoding, outputs_info, xsink);
      if (xsink->isException()) {
        return result;
      }
      break;

   case CS_STATUS_RESULT:
   { // status return codes are not used by Qore
      QoreNode* dummy = 0;
      sybase_read_row(wrapper, dummy, encoding, outputs_info, xsink);
printf("#### startsu result = %s\n", dummy->type->getAsString(dummy, 0, 0)->getBuffer());
Hash* h = dummy->val.hash;
HashIterator it(h);
while (it.next()) {
printf("### HASH VALUE\n");
const char* name = it.getKey();
QoreNode* vvv = it.getValue();

const char* type;
if (vvv->type == NT_INT) type = "int"; else
if (vvv->type == NT_FLOAT) type = "float";
else type = "unknown";
const char* v = vvv->type->getAsString(vvv, 0, 0)->getBuffer();
printf("### single hash value name [%s], type [%s] = [%s]\n", name, type, v);
}
printf("### end of hash values\n");
      if (dummy) dummy->deref(xsink);
      if (xsink->isException()) {
        assert(false);
        return result;
      }
      break;
   }

    case CS_COMPUTEFMT_RESULT:
      // Sybase bug???
      assert(false);
      xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call ct_results() failed with result code CS_COMPUTE_FMT_RESULT");
      return result;

    case CS_MSG_RESULT:
      // Sybase bug???
      assert(false);
      xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call ct_results() failed with result code CS_MSG_RESULT");
      return result;

    case CS_ROWFMT_RESULT:
      // Sybase bug???
      assert(false);
      xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call ct_results() failed with result code CS_ROW_FMT_RESULT");
      return result;

    case CS_DESCRIBE_RESULT:
      // not expected here
      assert(false);
      xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call ct_results() failed with result code CS_DESCRIBE_RESULTS");
      return result;

    case CS_CMD_DONE:
      // e.g. update, ct_res_info() could be used to get # of affected rows
      goto finish;

    case CS_CMD_FAIL:
      assert(false);
      xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call ct_results() failed with result code CS_CMD_FAIL");
      return result;

    case CS_CMD_SUCCEED:
      // current command succeeded, there may be more. CS_CMD_DONE is when we should return
      continue; 

    default:
      assert(false);
      xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call ct_results() gave unknown result type %d", (int)result_type);
      return result;
    } // switch
  } // while

finish:
  if (err != CS_END_RESULTS && err != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call ct_results() finished with unexpected result %d", (int)err); 
  }
  return result;
}

#ifdef DEBUG
#  include "tests/sybase_read_output_tests.cc"
#endif

// EOF

