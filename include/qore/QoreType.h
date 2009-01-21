/*
  QoreType.h
  
  Qore Programming Language

  Copyright 2003 - 2009 David Nichols

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

#ifndef _QORE_QORETYPE_H

#define _QORE_QORETYPE_H

#include <qore/common.h>
#include <qore/node_types.h>

#include <map>

// global default values
DLLEXPORT extern QoreListNode *emptyList;
DLLEXPORT extern QoreHashNode *emptyHash;
DLLEXPORT extern QoreStringNode *NullString;
DLLEXPORT extern DateTimeNode *ZeroDate;

DLLEXPORT extern QoreString NothingTypeString, NullTypeString, TrueString, 
   FalseString, EmptyHashString, EmptyListString;

DLLEXPORT qore_type_t get_next_type_id();

DLLLOCAL void init_qore_types();
DLLLOCAL void delete_qore_types();

DLLEXPORT bool compareHard(const AbstractQoreNode *l, const AbstractQoreNode *r, class ExceptionSink *xsink);
DLLEXPORT bool compareSoft(const AbstractQoreNode *l, const AbstractQoreNode *r, class ExceptionSink *xsink);

static inline AbstractQoreNode *boolean_false()
{
   return &False;
}

static inline AbstractQoreNode *boolean_true()
{
   return &True;
}

static inline class QoreBigIntNode *zero()
{
   return new QoreBigIntNode();
}

static inline class AbstractQoreNode *zero_float()
{
   return new QoreFloatNode(0.0);
}

static inline DateTimeNode *zero_date()
{
   ZeroDate->ref();
   return ZeroDate;
}

static inline class QoreStringNode *null_string()
{
   NullString->ref();
   return NullString;
}

#endif // _QORE_QORETYPE_H
