/*
 QoreDeleteOperatorNode.cpp
 
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

#include <qore/Qore.h>

QoreString QoreDeleteOperatorNode::delete_str("delete operator expression");

// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
QoreString *QoreDeleteOperatorNode::getAsString(bool &del, int foff, ExceptionSink *xsink) const {
   del = false;
   return &delete_str;
}

int QoreDeleteOperatorNode::getAsString(QoreString &str, int foff, ExceptionSink *xsink) const {
   str.concat(&delete_str);
   return 0;
}

AbstractQoreNode *QoreDeleteOperatorNode::evalImpl(ExceptionSink *xsink) const {
   delete_lvalue(exp, xsink);
   return 0;
}

AbstractQoreNode *QoreDeleteOperatorNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const {
   delete_lvalue(exp, xsink);
   return 0;
}

AbstractQoreNode *QoreDeleteOperatorNode::parseInit(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
   if (exp)
      exp = exp->parseInit(oflag, pflag & ~PF_REFERENCE_OK, lvids, typeInfo);
   typeInfo = nothingTypeInfo;
   return this;
}
