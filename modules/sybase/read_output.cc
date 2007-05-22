/*
  read_output.cc

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
#include <qore/Hash.h>
#include <qore/List.h>
#include <qore/QoreNode.h>
#include <qore/minitest.hpp>

#include <assert.h>
#include <cstypes.h>
#include <ctpublic.h>

#include "read_output.h"
#include "command.h"
#include "get_columns_count.h"
#include "get_row_description.h"
#include "set_up_output_buffers.h"
#include "fetch_row_into_buffers.h"
#include "output_buffers_to_QoreHash.h"

//------------------------------------------------------------------------------
static void read_rows(command& cmd, QoreEncoding* encoding, QoreNode*& out_node, bool list, ExceptionSink* xsink)
{
  unsigned columns = get_columns_count(cmd, xsink);
  if (xsink->isException()) {
    return;
  }
  
  std::vector<CS_DATAFMT> descriptions = get_row_description(cmd, columns, xsink);
  if (xsink->isException()) {
    return;
  }

  row_output_buffers out_buffers;
  set_up_output_buffers(cmd, descriptions, out_buffers, xsink);
  if (xsink->isException()) {
    return;
  }

  // setup hash of lists if necessary
  if (!list)
  {
     Hash *h = new Hash();
     for (unsigned i = 0, n = descriptions.size(); i != n; ++i) {
	assert(descriptions[i].name && descriptions[i].name[0]);
	h->setKeyValue(descriptions[i].name, new QoreNode(new List()), 0);
     }
     out_node = new QoreNode(h);
  }

  while (fetch_row_into_buffers(cmd, xsink)) {
     if (!list)
     {
	if (append_buffers_to_List(cmd, descriptions, out_buffers, encoding, out_node->val.hash, xsink))
	{
	   out_node->deref(xsink);
	   return;
	}
     }
     else
     {
	Hash* h =  output_buffers_to_QoreHash(cmd, descriptions, out_buffers, encoding, xsink);
	if (xsink->isException()) {
	   if (h) {
	      QoreNode* dummy = new QoreNode(h);
	      dummy->deref(xsink);
	   }
	   return;
	}
	if (out_node) {
	   if (out_node->type == NT_HASH) {
	      // nonvert to hash - several rows
	      QoreNode* aux = new QoreNode(new List);
	      aux->val.list->push(out_node);
	      aux->val.list->push(new QoreNode(h));
	      out_node = aux;
	   } else {
	      assert(out_node->type == NT_LIST);
	      out_node->val.list->push(new QoreNode(h));
	   }
	} else {
	   out_node = new QoreNode(h);
	}
     }
  } // while
  if (xsink->isException()) {
    return;
  }
}

//------------------------------------------------------------------------------
QoreNode* read_output(command& cmd, QoreEncoding* encoding, bool list, ExceptionSink* xsink)
{
  QoreNode* result = 0;

  CS_INT result_type;  
  CS_RETCODE err;
  while ((err = ct_results(cmd(), &result_type)) == CS_SUCCEED) {
    switch (result_type) {
    case CS_CURSOR_RESULT:
      assert(false); // cannot happen, bug in driver
      xsink->raiseException("DBI-EXEC-EXCEPTION", "Unexpected CS_CURSOR_RESULT: returned by ct_results()");
      return result;

    case CS_COMPUTE_RESULT:
    case CS_PARAM_RESULT: // procedure call
    case CS_ROW_RESULT:
      // 0 or more rows
       read_rows(cmd, encoding, result, list, xsink);
       if (xsink->isException()) {
	  return result;
       }
       break;

   case CS_STATUS_RESULT:
   { // status return codes are not used by Qore
      QoreNode* dummy = 0;
      read_rows(cmd, encoding, dummy, list, xsink);
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

    case CS_CMD_SUCCEED:
      // current command succeeded, there may be more. CS_CMD_DONE is when we should return
      continue;

    case CS_CMD_FAIL: // returned by the FreeTDS when used incorrectly
      xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call ct_results() failed with result code CS_CMD_FAIL");
      return result;

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
#  include "tests/read_output_simple_tests.cc"
#  include "tests/read_output_image_tests.cc"
#  include "tests/read_output_text_tests.cc"
#endif

// EOF


