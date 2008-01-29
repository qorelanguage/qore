/*
  QoreFloatNode.cc
  
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

#include <qore/QoreFloatNode.h>


QoreFloatNode::QoreFloatNode(double n_f) : SimpleQoreNode(NT_FLOAT), f(n_f)
{
}

QoreFloatNode::~QoreFloatNode()
{
}

// get the value of the type in a string context (default implementation = del = false and returns NullString)
// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
// use the QoreStringValueHelper class (defined in QoreStringNode.h) instead of using this function directly
QoreString *QoreFloatNode::getStringRepresentation(bool &del) const
{
   del = true;
   return new QoreString(f);
}

// concatenate string representation to a QoreString (no action for complex types = default implementation)
void QoreFloatNode::getStringRepresentation(QoreString &str) const
{
   str.sprintf("%.9g", f);
}

// if del is true, then the returned DateTime * should be deleted, if false, then it should not
DateTime *QoreFloatNode::getDateTimeRepresentation(bool &del) const
{
   del = true;
   return new DateTime((int64)f);
}

// assign date representation to a DateTime (no action for complex types = default implementation)
void QoreFloatNode::getDateTimeRepresentation(DateTime &dt) const
{
   dt.setDate((int64)f);
}

bool QoreFloatNode::getAsBool() const
{
   return (bool)f;
}

int QoreFloatNode::getAsInt() const
{
   return (int)f;
}

int64 QoreFloatNode::getAsBigInt() const
{
   return (int64)f;
}

double QoreFloatNode::getAsFloat() const
{
   return f;
}

// get string representation (for %n and %N), foff is for multi-line formatting offset, -1 = no line breaks
// the ExceptionSink is only needed for QoreObject where a method may be executed
// use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using these functions directly
// returns -1 for exception raised, 0 = OK
int QoreFloatNode::getAsString(QoreString &str, int foff, class ExceptionSink *xsink) const
{
   getStringRepresentation(str);
   return 0;
}

// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
QoreString *QoreFloatNode::getAsString(bool &del, int foff, class ExceptionSink *xsink) const
{
   return getStringRepresentation(del);
}

QoreNode *QoreFloatNode::realCopy() const
{
   return new QoreFloatNode(f);
}

// performs a lexical compare, return -1, 0, or 1 if the "this" value is less than, equal, or greater than
// the "val" passed
//DLLLOCAL virtual int compare(const QoreNode *val) const;
// the type passed must always be equal to the current type
bool QoreFloatNode::is_equal_soft(const QoreNode *v, ExceptionSink *xsink) const
{
   return v->getAsFloat() == f;
}

bool QoreFloatNode::is_equal_hard(const QoreNode *v, ExceptionSink *xsink) const
{
   const QoreFloatNode *fn = dynamic_cast<const QoreFloatNode *>(v);
   if (!fn)
      return false;

   return fn->f == f;
}

// returns the data type
const QoreType *QoreFloatNode::getType() const
{
   return NT_FLOAT;
}

// returns the type name as a c string
const char *QoreFloatNode::getTypeName() const
{
   return "float";
}
