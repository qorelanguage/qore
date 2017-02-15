/*
  QoreKeysOperatorNode.cpp

  Qore Programming Language

  Copyright (C) 2003 - 2016 Qore Technologies, s.r.o.

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
#include "qore/intern/QoreObjectIntern.h"

QoreString QoreKeysOperatorNode::keys_str("keys operator expression");

// if del is true, then the returned QoreString * should be keysd, if false, then it must not be
QoreString *QoreKeysOperatorNode::getAsString(bool &del, int foff, ExceptionSink *xsink) const {
   del = false;
   return &keys_str;
}

int QoreKeysOperatorNode::getAsString(QoreString &str, int foff, ExceptionSink *xsink) const {
   str.concat(&keys_str);
   return 0;
}

AbstractQoreNode* QoreKeysOperatorNode::parseInitImpl(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo) {
   assert(!typeInfo);

   pflag &= ~PF_RETURN_VALUE_IGNORED;

   const QoreTypeInfo *expTypeInfo = 0;
   exp = exp->parseInit(oflag, pflag, lvids, expTypeInfo);

   if (expTypeInfo->hasType()) {
      if (expTypeInfo->isType(NT_HASH) || expTypeInfo->isType(NT_OBJECT))
	 returnTypeInfo = listTypeInfo;
      else if (!hashTypeInfo->parseAccepts(expTypeInfo)
	       && !objectTypeInfo->parseAccepts(expTypeInfo)) {
	 QoreStringNode* edesc = new QoreStringNode("the expression with the 'keys' operator is ");
	 expTypeInfo->getThisType(*edesc);
	 edesc->concat(" and so this expression will always return NOTHING; the 'keys' operator can only return a value with hashes and objects");
	 qore_program_private::makeParseWarning(getProgram(), QP_WARN_INVALID_OPERATION, "INVALID-OPERATION", edesc);
	 returnTypeInfo = nothingTypeInfo;
      }
      else
	 returnTypeInfo = listOrNothingTypeInfo;
   }
   else
      returnTypeInfo = listOrNothingTypeInfo;

   if (exp && exp->is_value()) {
      ReferenceHolder<> holder(this, 0);
      qore_type_t t = get_node_type(exp);
      if (t == NT_HASH || t == NT_OBJECT) {
	 ValueEvalRefHolder rv(this, 0);
	 AbstractQoreNode* v = rv->isNothing() ? &Nothing : rv.getReferencedValue();
	 typeInfo = getTypeInfoForValue(v);
	 return v;
      }
      else {
	 typeInfo = nothingTypeInfo;
	 return &Nothing;
      }
   }

   typeInfo = returnTypeInfo;

   return this;
}

QoreValue QoreKeysOperatorNode::evalValueImpl(bool& needs_deref, ExceptionSink* xsink) const {
   FunctionalValueType value_type;
   std::unique_ptr<FunctionalOperatorInterface> f(getFunctionalIterator(value_type, xsink));
   if (*xsink || value_type != list || !ref_rv)
      return QoreValue();

   ReferenceHolder<QoreListNode> rv(new QoreListNode, xsink);

   while (true) {
      ValueOptionalRefHolder iv(xsink);
      if (f->getNext(iv, xsink))
	 break;

      if (*xsink)
	 return QoreValue();

      rv->push(iv.getReferencedValue());
   }

   return rv.release();
}

FunctionalOperatorInterface* QoreKeysOperatorNode::getFunctionalIteratorImpl(FunctionalValueType& value_type, ExceptionSink* xsink) const {
   ValueEvalRefHolder marg(exp, xsink);
   if (*xsink)
      return 0;

   qore_type_t t = marg->getType();
   if (t == NT_HASH) {
      value_type = list;
      return new QoreFunctionalKeysOperator(marg.takeReferencedNode<QoreHashNode>(), xsink);
   }
   if (t == NT_OBJECT) {
      value_type = list;
      return new QoreFunctionalKeysOperator(qore_object_private::get(*marg->get<QoreObject>())->getRuntimeMemberHash(xsink), xsink);
   }
   value_type = nothing;
   return 0;
}

bool QoreFunctionalKeysOperator::getNextImpl(ValueOptionalRefHolder& val, ExceptionSink* xsink) {
   if (!next())
      return true;

   val.setValue(new QoreStringNode(getKey()), true);
   return false;
}
