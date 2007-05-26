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
#include "get_row_description.h"
#include "set_up_output_buffers.h"
#include "output_buffers_to_QoreHash.h"
#include "connection.h"

//------------------------------------------------------------------------------
static void read_rows(command& cmd, QoreEncoding* encoding, QoreNode*& out_node, bool list, ExceptionSink* xsink)
{
  unsigned columns = cmd.get_column_count(xsink);
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
     QoreString str(encoding);
     for (unsigned i = 0, n = descriptions.size(); i != n; ++i) {

	const char *col_name;

	if (descriptions[i].name && descriptions[i].name[0]) {
	   col_name = descriptions[i].name;
	} else {
	   str.clear();
	   str.sprintf("column%d", i + 1);
	   col_name = str.getBuffer();
	} 

	h->setKeyValue(col_name, new QoreNode(new List()), 0);
     }
     out_node = new QoreNode(h);
  }

  while (cmd.fetch_row_into_buffers(xsink)) {

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
	      h->derefAndDelete(xsink);
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
}

//------------------------------------------------------------------------------
QoreNode* read_output(connection &conn, command& cmd, QoreEncoding* encoding, bool list, ExceptionSink* xsink)
{
   QoreNode* result = 0;

   CS_INT result_type;  
   CS_RETCODE err;
   while ((err = ct_results(cmd(), &result_type)) == CS_SUCCEED) 
   {
      //printd(0, "read_output() result_type = %d\n", result_type);

      switch (result_type) {
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
	       return result;
	    }
	   break;
	}
	
	case CS_CMD_DONE:
	{
	   if (!result)
	   {
	      CS_INT rowcount;
	      CS_RETCODE ret;
	      ret = ct_res_info(cmd(), CS_ROW_COUNT, (CS_VOID *)&rowcount, CS_UNUSED, 0);
	      if (ret != CS_SUCCEED)
		 conn.do_exception(xsink, "DBI:SYBASE:EXEC-ERROR", "ct_res_info(CS_ROW_COUNT) failed with return code %d", ret);

	      if (rowcount >= 0)
		 result = new QoreNode((int64)rowcount);
	      //printd(5, "rowcount=%d, result=%08p\n", (int)rowcount, result);
	   }
	   
	   goto finish;
	}
	
	case CS_CMD_SUCCEED:
	   // current command succeeded, there may be more. CS_CMD_DONE is when we should return
	   continue;
	   
	case CS_CMD_FAIL: // returned by the FreeTDS when used incorrectly
	   conn.do_exception(xsink, "DBI:SYBASE:EXEC-ERROR", "SQL command failed");
	   return result;
	   
	default:
	   conn.do_exception(xsink, "DBI:SYBASE:EXEC-ERROR", "ct_results() returned unexpected result type %d", (int)result_type);
	   return result;
      } // switch
   } // while
  
  finish:
   if (err != CS_END_RESULTS && err != CS_SUCCEED)
      conn.do_exception(xsink, "DBI:SYBASE:EXEC-ERROR", "ct_results() failed");
   else
      conn.purge_messages(xsink);
   return result;
}

#ifdef DEBUG
#  include "tests/read_output_simple_tests.cc"
#  include "tests/read_output_image_tests.cc"
#  include "tests/read_output_text_tests.cc"
#endif

// EOF


