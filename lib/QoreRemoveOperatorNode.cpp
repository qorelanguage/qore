/*
 QoreRemoveOperatorNode.cpp
 
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

QoreString QoreRemoveOperatorNode::remove_str("remove operator expression");

// if del is true, then the returned QoreString * should be removed, if false, then it must not be
QoreString *QoreRemoveOperatorNode::getAsString(bool &del, int foff, ExceptionSink *xsink) const {
   del = false;
   return &remove_str;
}

int QoreRemoveOperatorNode::getAsString(QoreString &str, int foff, ExceptionSink *xsink) const {
   str.concat(&remove_str);
   return 0;
}

AbstractQoreNode *QoreRemoveOperatorNode::evalImpl(ExceptionSink *xsink) const {
   return remove_lvalue(exp, xsink);
}

AbstractQoreNode *QoreRemoveOperatorNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const {
   needs_deref = true;
   return remove_lvalue(exp, xsink);
}

AbstractQoreNode *QoreRemoveOperatorNode::parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
   if (exp) {
      exp = exp->parseInit(oflag, pflag & ~PF_REFERENCE_OK, lvids, typeInfo);
      if (exp && check_lvalue(exp))
         parse_error("the remove operator expects an lvalue as its operand, got '%s' instead", exp->getTypeName());
      returnTypeInfo = typeInfo;
   }
   return this;
}
