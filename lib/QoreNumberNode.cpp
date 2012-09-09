/*
  QoreNumberNode.cpp
  
  Qore Programming Language

  Copyright 2003 - 2012 David Nichols

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

struct qore_number_private {
   DLLLOCAL qore_number_private() {
   }

   DLLLOCAL qore_number_private(double f) {
   }

   DLLLOCAL qore_number_private(int64 i) {
   }

   DLLLOCAL qore_number_private(const char* str) {
   }

   DLLLOCAL qore_number_private(const qore_number_private& old) {
   }
};

QoreNumberNode::QoreNumberNode(double f) : SimpleValueQoreNode(NT_NUMBER), priv(new qore_number_private(f)) {
}

QoreNumberNode::QoreNumberNode(int64 i) : SimpleValueQoreNode(NT_NUMBER), priv(new qore_number_private(i)) {
}

QoreNumberNode::QoreNumberNode(const char* str) : SimpleValueQoreNode(NT_NUMBER), priv(new qore_number_private(str)) {
}

QoreNumberNode::QoreNumberNode() : SimpleValueQoreNode(NT_NUMBER), priv(new qore_number_private) {
}

QoreNumberNode::QoreNumberNode(const QoreNumberNode& old) : SimpleValueQoreNode(old), priv(new qore_number_private(*old.priv)) {
}

QoreNumberNode::~QoreNumberNode() {
   delete priv;
}

// get the value of the type in a string context (default implementation = del = false and returns NullString)
// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
// use the QoreStringValueHelper class (defined in QoreStringNode.h) instead of using this function directly
QoreString *QoreNumberNode::getStringRepresentation(bool& del) const {
   del = true;
   return new QoreString("xxx");
}

// concatenate string representation to a QoreString (no action for complex types = default implementation)
void QoreNumberNode::getStringRepresentation(QoreString& str) const {
   str.concat("xxx");
}

// if del is true, then the returned DateTime * should be deleted, if false, then it should not
DateTime *QoreNumberNode::getDateTimeRepresentation(bool& del) const {
   del = true;
   return DateTime::makeAbsoluteLocal(currentTZ(), (int64)0, (int)0);
}

// assign date representation to a DateTime (no action for complex types = default implementation)
void QoreNumberNode::getDateTimeRepresentation(DateTime& dt) const {
   dt.setLocalDate(currentTZ(), (int64)0, (int)0);
}

bool QoreNumberNode::getAsBoolImpl() const {
   return (bool)false;
}

int QoreNumberNode::getAsIntImpl() const {
   return (int)0;
}

int64 QoreNumberNode::getAsBigIntImpl() const {
   return (int64)0;
}

double QoreNumberNode::getAsFloatImpl() const {
   return 0.0;
}

// get string representation (for %n and %N), foff is for multi-line formatting offset, -1 = no line breaks
// the ExceptionSink is only needed for QoreObject where a method may be executed
// use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using these functions directly
// returns -1 for exception raised, 0 = OK
int QoreNumberNode::getAsString(QoreString& str, int foff, ExceptionSink *xsink) const {
   getStringRepresentation(str);
   return 0;
}

// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
QoreString *QoreNumberNode::getAsString(bool& del, int foff, ExceptionSink *xsink) const {
   return getStringRepresentation(del);
}

AbstractQoreNode *QoreNumberNode::realCopy() const {
   return new QoreNumberNode(*this);
}

// performs a lexical compare, return -1, 0, or 1 if the "this" value is less than, equal, or greater than
// the "val" passed
//DLLLOCAL virtual int compare(const AbstractQoreNode *val) const;
// the type passed must always be equal to the current type
bool QoreNumberNode::is_equal_soft(const AbstractQoreNode *v, ExceptionSink *xsink) const {
   return false;
}

bool QoreNumberNode::is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const {
   const QoreNumberNode *fn = dynamic_cast<const QoreNumberNode *>(v);
   if (!fn)
      return false;

   return false;
}

// returns the type name as a c string
const char *QoreNumberNode::getTypeName() const {
   return getStaticTypeName();
}

AbstractQoreNode *QoreNumberNode::parseInit(LocalVar *oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo) {
   typeInfo = numberTypeInfo;
   return this;
}
