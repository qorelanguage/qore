/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  ql_json.h

  qore JSON support functions

  Qore Programming Language

  Copyright 2003 - 2010 David Nichols

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

#ifndef _QORE_QL_JSON_H

#define _QORE_QL_JSON_H

DLLEXPORT QoreStringNode *makeJSONRPC11RequestStringArgs(const QoreListNode *params, ExceptionSink *xsink);
DLLEXPORT QoreStringNode *makeJSONRPC11RequestString(const QoreListNode *params, ExceptionSink *xsink);
DLLEXPORT AbstractQoreNode *parseJSONValue(const QoreString *str, class ExceptionSink *xsink);

DLLLOCAL void init_json_functions();

#endif // _QORE_QL_JSON_H
