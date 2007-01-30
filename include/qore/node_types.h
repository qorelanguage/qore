/*
  node_types.h

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

#ifndef _QORE_NODE_TYPES_H

#define _QORE_NODE_TYPES_H

// global system types
// NOTE that value types must come first to support the operator
// matrix optimization
extern class QoreType *NT_NOTHING, *NT_INT, *NT_FLOAT, *NT_STRING, *NT_DATE,
   *NT_BOOLEAN, *NT_NULL, *NT_BINARY, *NT_LIST, *NT_HASH,
   *NT_OBJECT, *NT_FLIST, *NT_BACKQUOTE, *NT_CONTEXTREF, *NT_COMPLEXCONTEXTREF,
   *NT_VARREF, *NT_TREE, *NT_FIND, *NT_FUNCTION_CALL, *NT_SELF_VARREF,
   *NT_SCOPE_REF, *NT_CONSTANT, *NT_BAREWORD, *NT_REFERENCE, *NT_CONTEXT_ROW,
   *NT_REGEX_SUBST, *NT_REGEX_TRANS, *NT_VLIST, *NT_REGEX, *NT_CLASSREF;

#define NUM_SIMPLE_TYPES 8
#define NUM_VALUE_TYPES 11

#define NT_NONE  (class QoreType *)-1
#define NT_ALL   (class QoreType *)-2

#endif
