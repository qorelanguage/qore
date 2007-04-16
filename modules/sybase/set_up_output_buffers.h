/*
  set_up_output_buffers.h

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

#ifndef SYBASE_SET_UP_OUTPUT_BUFFERS_H_
#define SYBASE_SET_UP_OUTPUT_BUFFERS_H_

#include <cstypes.h>
#include <vector>
#include <vector>

class command;
class ExceptionSink;

//------------------------------------------------------------------------------
// Buffer for single column value. Noncopyable, nonassignable.
class output_value_buffer
{
public:
  output_value_buffer(const output_value_buffer&); // not implemented
  output_value_buffer& operator=(output_value_buffer&); // not implemented  
public:
  output_value_buffer(unsigned size);
  ~output_value_buffer();

  CS_SMALLINT indicator;
  CS_CHAR* value;          // owned
  CS_INT value_len;  
};

//------------------------------------------------------------------------------
// helds buffers for a single row
class row_output_buffers
{
private:
  row_output_buffers(const row_output_buffers&); // not implemented
  row_output_buffers& operator=(const row_output_buffers&); // not implemented

public:
  row_output_buffers() {}
  ~row_output_buffers();

  std::vector<output_value_buffer*> m_buffers;
};

//------------------------------------------------------------------------------
extern void set_up_output_buffers(
  command& cmd, 
  const std::vector<CS_DATAFMT>& input_row_descriptions,
  row_output_buffers& result, // out variable (since it is non-copyable)
  ExceptionSink* xsink);

#endif

// EOF

