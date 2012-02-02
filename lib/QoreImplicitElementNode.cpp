/*
  QoreImplicitElementNode.cpp

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

AbstractQoreNode *QoreImplicitElementNode::evalImpl(ExceptionSink *xsink) const {
   return new QoreBigIntNode(get_implicit_element());
}

AbstractQoreNode *QoreImplicitElementNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const {
   int val = get_implicit_element();
   if (!val) {
      needs_deref = false;
      return Zero;
   }

   needs_deref = true;
   return new QoreBigIntNode(val);
}

int64 QoreImplicitElementNode::bigIntEvalImpl(ExceptionSink *xsink) const {
   return get_implicit_element();
}

int QoreImplicitElementNode::integerEvalImpl(ExceptionSink *xsink) const {
   return get_implicit_element();
}

bool QoreImplicitElementNode::boolEvalImpl(ExceptionSink *xsink) const {
   return get_implicit_element();
}

double QoreImplicitElementNode::floatEvalImpl(ExceptionSink *xsink) const {
   return get_implicit_element();
}

int QoreImplicitElementNode::getAsString(QoreString &str, int foff, ExceptionSink *xsink) const {
   str.concat("get implicit element offset");
   return 0;
}

QoreString *QoreImplicitElementNode::getAsString(bool &del, int foff, ExceptionSink *xsink) const {
   del = true;
   QoreString *rv = new QoreString;
   getAsString(*rv, foff, xsink);
   return rv;
}

const char *QoreImplicitElementNode::getTypeName() const {
   return getStaticTypeName();
}
