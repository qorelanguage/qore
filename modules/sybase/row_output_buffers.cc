/*
  row_output_buffers.cc

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

#include <assert.h>

#include "row_output_buffers.h"

output_value_buffer::output_value_buffer(unsigned size) : indicator(0), value_len(0)
{
   if (size < 7) size = 7; // ensure at least 8 bytes are allocated
   value = size ? new char[size + 1] : 0; // terminator for strings
}

output_value_buffer::~output_value_buffer()
{
   delete[] value;
}

row_output_buffers::~row_output_buffers()
{
   for (unsigned i = 0, n = m_buffers.size(); i != n; ++i) 
   {
      assert(m_buffers[i]);
      delete m_buffers[i];
   }
}

