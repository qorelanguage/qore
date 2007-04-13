/*
  buffer_to_QoreNode.h

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

#ifndef SYBASE_BUFFER_TO_QORE_NODE_H_
#define SYBASE_BUFFER_TO_QORE_NODE_H_

#include "set_up_output_buffers.h"

class ExceptionSink;
class QoreEncoding;
class QoreNode;
class connection;

extern QoreNode* buffer_to_QoreNode(command& cmd, const CS_DATAFMT& datafmt, const output_value_buffer& buffer, QoreEncoding* encoding, ExceptionSink* xsink);

#endif

// EOF

