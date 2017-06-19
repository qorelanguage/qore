/*
  QoreTrimOperatorNode.cpp

  Qore Programming Language

  Copyright (C) 2003 - 2017 Qore Technologies, s.r.o.

  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.

  Note that the Qore library is released under a choice of three open-source
  licenses: MIT (as above), LGPL 2+, or GPL 2+; see README-LICENSE for more
  information.
*/

#include <qore/Qore.h>
#include "qore/intern/qore_program_private.h"

QoreString QoreTrimOperatorNode::trim_str("trim operator expression");

// if del is true, then the returned QoreString*  should be trimd, if false, then it must not be
QoreString* QoreTrimOperatorNode::getAsString(bool& del, int foff, ExceptionSink* xsink) const {
   del = false;
   return &trim_str;
}

int QoreTrimOperatorNode::getAsString(QoreString& str, int foff, ExceptionSink* xsink) const {
   str.concat(&trim_str);
   return 0;
}

QoreValue QoreTrimOperatorNode::evalValueImpl(bool& needs_deref, ExceptionSink* xsink) const {
   // get ptr to current value (lvalue is locked for the scope of the LValueHelper object)
   LValueHelper val(exp, xsink);
   if (!val)
      return QoreValue();

   qore_type_t vtype = val.getType();
   if (vtype != NT_LIST && vtype != NT_STRING && vtype != NT_HASH)
      return QoreValue();

   // note that no exception can happen here
   val.ensureUnique();
   assert(!*xsink);

   if (vtype == NT_STRING) {
      QoreStringNode* vs = reinterpret_cast<QoreStringNode*>(val.getValue());
      if (vs->trim(xsink))
         return QoreValue();
   }
   else if (vtype == NT_LIST) {
      QoreListNode* l = reinterpret_cast<QoreListNode*>(val.getValue());
      ListIterator li(l);
      while (li.next()) {
         AbstractQoreNode** v = li.getValuePtr();
         if (*v && (*v)->getType() == NT_STRING) {
            // note that no exception can happen here
            ensure_unique(v, xsink);
            assert(!*xsink);
            QoreStringNode* vs = reinterpret_cast<QoreStringNode*>(*v);
            if (vs->trim(xsink))
               return QoreValue();
         }
      }
   }
   else { // is a hash
      QoreHashNode* vh = reinterpret_cast<QoreHashNode*>(val.getValue());
      HashIterator hi(vh);
      while (hi.next()) {
         AbstractQoreNode** v = hi.getValuePtr();
         if (*v && (*v)->getType() == NT_STRING) {
            // note that no exception can happen here
            assert(!*xsink);
            ensure_unique(v, xsink);
            QoreStringNode* vs = reinterpret_cast<QoreStringNode*>(*v);
            if (vs->trim(xsink))
               return QoreValue();
         }
      }
   }

   // reference for return value
   if (!ref_rv)
      return QoreValue();
   return val.getReferencedValue();
}

AbstractQoreNode* QoreTrimOperatorNode::parseInitImpl(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo) {
   assert(!typeInfo);
   if (!exp)
      return this;
   exp = exp->parseInit(oflag, pflag, lvids, typeInfo);
   if (exp)
      checkLValue(exp, pflag);

   if (QoreTypeInfo::hasType(typeInfo)
       && !QoreTypeInfo::parseAcceptsReturns(typeInfo, NT_STRING)
       && !QoreTypeInfo::parseAcceptsReturns(typeInfo, NT_LIST)
       && !QoreTypeInfo::parseAcceptsReturns(typeInfo, NT_HASH)) {
      QoreStringNode* desc = new QoreStringNode("the lvalue expression with the trim operator is ");
      QoreTypeInfo::getThisType(typeInfo, *desc);
      desc->sprintf(", therefore this operation will have no effect on the lvalue and will always return NOTHING; this operator only works on strings, lists, and hashes");
      qore_program_private::makeParseWarning(getProgram(), loc, QP_WARN_INVALID_OPERATION, "INVALID-OPERATION", desc);
   }
   returnTypeInfo = typeInfo;
   return this;
}
