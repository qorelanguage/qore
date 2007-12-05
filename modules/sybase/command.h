/*
  command.h

  Sybase DB layer for QORE
  uses Sybase OpenClient C library

  Qore Programming language

  Copyright (C) 2007 Qore Technologies sro

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

#ifndef SYBASE_COMMAND_H_
#define SYBASE_COMMAND_H_

#include <ctpublic.h>

#include "sybase_query.h"
#include "row_output_buffers.h"
#include "conversions.h"

class connection;

typedef std::vector<CS_DATAFMT> row_result_t;

// Wrapper for Sybase CS_COMMAND. When the wrapper object
// is destroyed it automatically cleans up held resources.
class command
{ 
      connection& m_conn;
      CS_COMMAND* m_cmd;

      // returns 0=OK, -1=error (exception raised)
      int get_row_description(row_result_t &result, unsigned column_count, class ExceptionSink *xsink);
      int setup_output_buffers(const row_result_t &input_row_descriptions, row_output_buffers &result, class ExceptionSink *xsink);
      class QoreNode *read_rows(PlaceholderList *placeholder_list, bool list, ExceptionSink* xsink);
      int append_buffers_to_list(PlaceholderList *placeholder_list, row_result_t &column_info, row_output_buffers& all_buffers, class QoreHash *h, ExceptionSink* xsink);
      class QoreHash *output_buffers_to_hash(PlaceholderList *placeholder_list, row_result_t column_info, row_output_buffers& all_buffers, ExceptionSink* xsink);
      class QoreNode *get_node(const CS_DATAFMT& datafmt, const output_value_buffer& buffer, ExceptionSink* xsink);

   public:
      DLLLOCAL command(connection& conn, ExceptionSink* xsink);
      DLLLOCAL ~command();

      DLLLOCAL CS_COMMAND* operator()() const { return m_cmd; }
      DLLLOCAL connection& getConnection() const { return m_conn; }

      // returns 0=OK, -1=error (exception raised)
      DLLLOCAL int send(ExceptionSink* xsink);
      // returns 0=OK, -1=error (exception raised)
      DLLLOCAL int initiate_language_command(const char *cmd_text, class ExceptionSink *xsink);
      // returns true if data returned, false if not
      DLLLOCAL bool fetch_row_into_buffers(class ExceptionSink *xsink);
      // returns the number of columns in the result
      DLLLOCAL unsigned get_column_count(ExceptionSink *xsink);
      // returns 0=OK, -1=error (exception raised)
      DLLLOCAL int set_params(sybase_query &query, class QoreList *args, ExceptionSink *xsink);
      DLLLOCAL QoreNode *read_output(PlaceholderList &placeholder_list, bool list, ExceptionSink* xsink);
};

#endif

