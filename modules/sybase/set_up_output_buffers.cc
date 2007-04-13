/*
  set_up_output_buffers.cc

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

#include <assert.h>

#include "set_up_output_buffers.h"
#include "command.h"
#include <memory>

//------------------------------------------------------------------------------
output_value_buffer::output_value_buffer()
: indicator(0),
  value(0),
  value_len(0)
{
}

//------------------------------------------------------------------------------
output_value_buffer::~output_value_buffer()
{
  delete[] value;
}

//------------------------------------------------------------------------------
void output_value_buffer::allocate_buffer()
{
  delete[] value;
  value = 0;
  if (value_len) {
    value = new char[value_len + 1]; // one byte added for possible string terminator
  }
}

//------------------------------------------------------------------------------
row_output_buffers::~row_output_buffers()
{
  for (unsigned i = 0, n = m_buffers.size(); i != n; ++i) {
    assert(m_buffers[i]);
    delete m_buffers[i];
  }
}

//------------------------------------------------------------------------------
void set_up_output_buffers(command& cmd,
  const std::vector<CS_DATAFMT>& input_row_descriptions,
  row_output_buffers& result, 
  ExceptionSink* xsink)
{
  for (unsigned i = 0, n = input_row_descriptions.size(); i != n; ++i) {
    unsigned size = input_row_descriptions[i].maxlength;
    output_value_buffer* buffer = new output_value_buffer;
    result.m_buffers.push_back(buffer);

    CS_RETCODE err = ct_bind(cmd(), i + 1, (CS_DATAFMT*)&input_row_descriptions[i], buffer->value, &buffer->value_len, &buffer->indicator);
    if (err != CS_SUCCEED) {
      assert(false);
      xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call ct_bind() failed with error %d", (int)err);
      return;
    }
  }
}

// EOF

