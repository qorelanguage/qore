/*
  QoreBigIntNode.cc
  
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

QoreBigIntNode::QoreBigIntNode() : SimpleValueQoreNode(NT_INT), val(0)
{
}

QoreBigIntNode::QoreBigIntNode(int64 v) : SimpleValueQoreNode(NT_INT), val(v)
{
}

QoreBigIntNode::~QoreBigIntNode()
{
}

// get the value of the type in a string context (default implementation = del = false and returns NullString)
// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
// use the QoreStringValueHelper class (defined in QoreStringNode.h) instead of using this function directly
QoreString *QoreBigIntNode::getStringRepresentation(bool &del) const
{
   del = true;
   return new QoreString(val);
}

// concatenate string representation to a QoreString (no action for complex types = default implementation)
void QoreBigIntNode::getStringRepresentation(QoreString &str) const
{
   str.sprintf("%lld", val);
}

// if del is true, then the returned DateTime * should be deleted, if false, then it should not
DateTime *QoreBigIntNode::getDateTimeRepresentation(bool &del) const
{
   del = true;
   return new DateTime(val);
}

// assign date representation to a DateTime (no action for complex types = default implementation)
void QoreBigIntNode::getDateTimeRepresentation(DateTime &dt) const
{
   dt.setDate(val);
}

bool QoreBigIntNode::getAsBoolImpl() const
{
   return (bool)val;
}

int QoreBigIntNode::getAsIntImpl() const
{
   return val;
}

int64 QoreBigIntNode::getAsBigIntImpl() const
{
   return val;
}

double QoreBigIntNode::getAsFloatImpl() const
{
   return (double)val;
}

// get string representation (for %n and %N), foff is for multi-line formatting offset, -1 = no line breaks
// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
// the ExceptionSink is only needed for QoreObject where a method may be executed
// use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using this function directly
QoreString *QoreBigIntNode::getAsString(bool &del, int foff, ExceptionSink *xsink) const
{
   return getStringRepresentation(del);
}

int QoreBigIntNode::getAsString(QoreString &str, int foff, ExceptionSink *xsink) const
{
   getStringRepresentation(str);
   return 0;
}

AbstractQoreNode *QoreBigIntNode::realCopy() const
{
   return new QoreBigIntNode(val);
}

// performs a lexical compare, return -1, 0, or 1 if the "this" value is less than, equal, or greater than
// the "val" passed
//DLLLOCAL virtual int compare(const AbstractQoreNode *val) const;
// the type passed must always be equal to the current type
bool QoreBigIntNode::is_equal_soft(const AbstractQoreNode *v, ExceptionSink *xsink) const
{
   return v->getAsBigInt() == val;
}

bool QoreBigIntNode::is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const
{
   const QoreBigIntNode *i = dynamic_cast<const QoreBigIntNode *>(v);
   if (!i)
      return false;

   return i->val == val;
}

// returns the type name as a c string
const char *QoreBigIntNode::getTypeName() const
{
   return getStaticTypeName();
}
