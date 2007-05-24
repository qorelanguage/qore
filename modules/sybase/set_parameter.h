/*
  set_parameter.h

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

#ifndef SYBASE_SET_PARAMETER_H_
#define SYBASE_SET_PARAMETER_H_

class ExceptionSink;
class command;

// returns 0=OK, -1 for error (exception raised)
DLLLOCAL extern int set_input_params(command& cmd, processed_language_command_t &query, class List *args, QoreEncoding* encoding, ExceptionSink* xsink);

// returns 0=OK, -1 for error (exception raised)
DLLLOCAL extern int set_input_parameter(command& cmd, unsigned parameter_index, int type,
  QoreNode* data, QoreEncoding* encoding, ExceptionSink* xsink);

// for RPC (placeholder)
extern void set_output_parameter(command& cmd, unsigned parameter_index, const char* name, 
  int type, ExceptionSink* xsink);
  

#endif

// EOF

