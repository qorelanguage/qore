/*
  outpuut_buffers_to_QoreHash.cc

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
#include <qore/minitest.hpp>
#include <qore/Hash.h>
#include <qore/QoreNode.h>
#include <qore/ScopeGuard.h>
#include <qore/QoreString.h>

#include <assert.h>
#include <memory>
#include <cstypes.h>

#include "output_buffers_to_QoreHash.h"
#include "command.h"
#include "set_up_output_buffers.h" // for row_output_buffers
#include "buffer_to_QoreNode.h"

//------------------------------------------------------------------------------
Hash* output_buffers_to_QoreHash(command& cmd, const std::vector<CS_DATAFMT>& columns_info,
				 row_output_buffers& all_buffers, QoreEncoding* encoding, ExceptionSink* xsink)
{
  Hash* result = new Hash;
  for (unsigned i = 0, n = columns_info.size(); i != n; ++i) {

    std::string column_name;
    if (columns_info[i].name && columns_info[i].name[0]) {
      column_name = columns_info[i].name;
    } else {
      char aux[20];
      sprintf(aux, "column%d", i + 1);
      column_name = (const char*)aux;
    } 

    const output_value_buffer& buff = *(all_buffers.m_buffers[i]);
    QoreNode* value = buffer_to_QoreNode(cmd, columns_info[i], buff, encoding, xsink);
    if (xsink->isException()) {
      if (value) value->deref(xsink);
      return result;
    }

    result->setKeyValue(column_name.c_str(), value, xsink);
    if (xsink->isException()) {
      assert(false);
      return result;
    }
  } // for

  return result;
}

int append_buffers_to_List(command& cmd, const std::vector<CS_DATAFMT>& columns_info,
			   row_output_buffers& all_buffers, QoreEncoding* encoding, class Hash *h, ExceptionSink* xsink)
{
  for (unsigned i = 0, n = columns_info.size(); i != n; ++i) {
     assert(columns_info[i].name && columns_info[i].name[0]);

     class List *l = h->getKeyValue(columns_info[i].name)->val.list;

     const output_value_buffer& buff = *(all_buffers.m_buffers[i]);
     QoreNode* value = buffer_to_QoreNode(cmd, columns_info[i], buff, encoding, xsink);
     if (xsink->isException()) {
	if (value) value->deref(xsink);
	return -1;
     }

     l->push(value);
  } // for

  return 0;
}

// EOF

