/*
 QoreModulaEqualsOperatorNode.cpp
 
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

QoreString QoreModulaEqualsOperatorNode::op_str("%= operator expression");

AbstractQoreNode *QoreModulaEqualsOperatorNode::parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
   parseInitIntLValue(op_str.getBuffer(), oflag, pflag, lvids, typeInfo);
   return this;
}

int64 QoreModulaEqualsOperatorNode::bigIntEvalImpl(ExceptionSink *xsink) const {
   int64 val = right->bigIntEval(xsink);
   if (xsink->isEvent())
      return 0;

   // get ptr to current value (lvalue is locked for the scope of the LValueHelper object)
   LValueHelper v(left, xsink);
   if (!v)
      return 0;
   // do not try to execute %= 0 or a runtime exception will occur
   return val ? v.modulaEqualsBigInt(val, "<%= operator>") : v.assignBigInt(0, "<%= operator>");
}
