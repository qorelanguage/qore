/*
  ql_misc.h

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols

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

#ifndef QORE_LIB_MISC_H

#define QORE_LIB_MISC_H

DLLEXPORT class BinaryNode   *qore_deflate(void *ptr, unsigned long len, int level, ExceptionSink *xsink);
DLLEXPORT class QoreStringNode *qore_inflate_to_string(const BinaryNode *b, const class QoreEncoding *enc, ExceptionSink *xsink);
DLLEXPORT class BinaryNode   *qore_inflate_to_binary(const BinaryNode *b, class ExceptionSink *xsink);
DLLEXPORT class BinaryNode   *qore_gzip(void *ptr, unsigned long len, int level, ExceptionSink *xsink);
DLLEXPORT class QoreStringNode *qore_gunzip_to_string(const BinaryNode *bin, const class QoreEncoding *enc, ExceptionSink *xsink);
DLLEXPORT class BinaryNode   *qore_gunzip_to_binary(const BinaryNode *bin, ExceptionSink *xsink);

DLLLOCAL void init_misc_functions();

#endif
