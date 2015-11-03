/*
  QoreNothingNode.cc

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

#include <qore/Qore.h>

#ifdef DEBUG
static bool nothing_flag = 0;
#endif

QoreNothingNode::QoreNothingNode() : UniqueValueQoreNode(NT_NOTHING)
{
#ifdef DEBUG
   assert(!nothing_flag);
   nothing_flag = true;
#endif
}

QoreNothingNode::~QoreNothingNode()
{
   //printd(0, "QoreNothingNode::~QoreNothingNode() called this=%08p\n", this);
}

AbstractQoreNode *QoreNothingNode::evalImpl(class ExceptionSink *xsink) const
{
   assert(false);
   return 0;
}

// get string representation (for %n and %N), foff is for multi-line formatting offset, -1 = no line breaks
// the ExceptionSink is only needed for QoreObject where a method may be executed
// use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using these functions directly
// returns -1 for exception raised, 0 = OK
int QoreNothingNode::getAsString(QoreString &str, int foff, ExceptionSink *xsink) const
{
   str.concat(&NothingTypeString);
   return 0;
}

// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
QoreString *QoreNothingNode::getAsString(bool &del, int foff, ExceptionSink *xsink) const
{
   del = false;
   return &NothingTypeString;
}

// performs a lexical compare, return -1, 0, or 1 if the "this" value is less than, equal, or greater than
// the "val" passed
//DLLLOCAL int QoreNothingNode::compare(const AbstractQoreNode *val) const;
// the type passed must always be equal to the current type
bool QoreNothingNode::is_equal_soft(const AbstractQoreNode *v, ExceptionSink *xsink) const
{
   return is_nothing(v);
}

bool QoreNothingNode::is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const
{
   return is_nothing(v);
}

// returns the type name as a c string
const char *QoreNothingNode::getTypeName() const
{
   return getStaticTypeName();
}
