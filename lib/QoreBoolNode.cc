/*
  QoreBoolNode.cc
  
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

QoreBoolNode::QoreBoolNode(bool n_b) : UniqueValueQoreNode(NT_BOOLEAN), b(n_b)
{
}

QoreBoolNode::~QoreBoolNode()
{
}

// get the value of the type in a string context (default implementation = del = false and returns NullString)
// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
// use the QoreStringValueHelper class (defined in QoreStringNode.h) instead of using this function directly
QoreString *QoreBoolNode::getStringRepresentation(bool &del) const
{
   del = true;
   return new QoreString(b);
}

// concatenate string representation to a QoreString (no action for complex types = default implementation)
void QoreBoolNode::getStringRepresentation(QoreString &str) const
{
   str.concat(b ? '1' : '0');
}

// if del is true, then the returned DateTime * should be deleted, if false, then it should not
class DateTime *QoreBoolNode::getDateTimeRepresentation(bool &del) const
{
   del = true;
   return new DateTime((int64)b);
}

// assign date representation to a DateTime (no action for complex types = default implementation)
void QoreBoolNode::getDateTimeRepresentation(DateTime &dt) const
{
   dt.setDate((int64)b);
}

bool QoreBoolNode::getAsBoolImpl() const
{
   return b;
}

int QoreBoolNode::getAsIntImpl() const
{
   return b;
}

int64 QoreBoolNode::getAsBigIntImpl() const
{
   return b;
}

double QoreBoolNode::getAsFloatImpl() const
{
   return b;
}

// get string representation (for %n and %N), foff is for multi-line formatting offset, -1 = no line breaks
// the ExceptionSink is only needed for QoreObject where a method may be executed
// use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using these functions directly
// returns -1 for exception raised, 0 = OK
int QoreBoolNode::getAsString(QoreString &str, int foff, ExceptionSink *xsink) const
{
   str.concat(b ? "True" : "False");
   return 0;
}

// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
QoreString *QoreBoolNode::getAsString(bool &del, int foff, ExceptionSink *xsink) const
{
   del = false;
   return b ? &TrueString : &FalseString;
}

// performs a lexical compare, return -1, 0, or 1 if the "this" value is less than, equal, or greater than
// the "val" passed
//DLLLOCAL int compare(const AbstractQoreNode *val) const;
// the type passed must always be equal to the current type
bool QoreBoolNode::is_equal_soft(const AbstractQoreNode *v, ExceptionSink *xsink) const
{
   return v->getAsBool() == b;
}

bool QoreBoolNode::is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const
{
   const QoreBoolNode *bn = dynamic_cast<const QoreBoolNode *>(v);
   if (!bn)
      return false;

   return b == bn->b;
}

// returns the type name as a c string
const char *QoreBoolNode::getTypeName() const
{
   return getStaticTypeName();
}

#ifdef DEBUG
static bool qore_bool_true_init = false;
static bool qore_bool_false_init = false;
#endif

QoreBoolTrueNode::QoreBoolTrueNode() : QoreBoolNode(true)
{
#ifdef DEBUG
   assert(!qore_bool_true_init);
   qore_bool_true_init = true;
#endif
}

QoreBoolFalseNode::QoreBoolFalseNode() : QoreBoolNode(false)
{
#ifdef DEBUG
   assert(!qore_bool_false_init);
   qore_bool_false_init = true;
#endif
}
