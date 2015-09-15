/*
  QoreMapOperatorNode.cpp

  Qore Programming Language

  Copyright (C) 2003 - 2015 David Nichols

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

#include <qore/intern/qore_program_private.h>

QoreString QoreMapOperatorNode::map_str("map operator expression");

// if del is true, then the returned QoreString * should be mapd, if false, then it must not be
QoreString *QoreMapOperatorNode::getAsString(bool &del, int foff, ExceptionSink *xsink) const {
   del = false;
   return &map_str;
}

int QoreMapOperatorNode::getAsString(QoreString &str, int foff, ExceptionSink *xsink) const {
   str.concat(&map_str);
   return 0;
}

AbstractQoreNode* QoreMapOperatorNode::parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
   assert(!typeInfo);

   pflag &= ~PF_RETURN_VALUE_IGNORED;

   // check iterated expression
   const QoreTypeInfo* expTypeInfo = 0;
   left = left->parseInit(oflag, pflag, lvids, expTypeInfo);

   // check iterator expression
   const QoreTypeInfo* iteratorTypeInfo = 0;
   right = right->parseInit(oflag, pflag, lvids, iteratorTypeInfo);

   // FIXME: if iterator is a list or an iterator, then the return type is a list, otherwise it's the return type of the iterated expression

   return this;
}

QoreValue QoreMapOperatorNode::evalValueImpl(bool& needs_deref, ExceptionSink* xsink) const {
   // conditionally evaluate argument expression
   ValueEvalRefHolder marg(right, xsink);
   if (*xsink)
      return QoreValue();

   qore_type_t t = marg->getType();
   if (t != NT_LIST) {
      if (t == NT_OBJECT) {
         AbstractIteratorHelper h(xsink, "map operator", const_cast<QoreObject*>(marg->get<const QoreObject>()));
         if (*xsink)
            return QoreValue();
         if (h)
            return mapIterator(h, xsink);
      }
      if (t == NT_NOTHING)
	 return QoreValue();

      // FIXME: inefficient
      ReferenceHolder<> argv_val(marg.getReferencedValue(), xsink);
      SingleArgvContextHelper argv_helper(*argv_val, xsink);
      ValueEvalRefHolder val(left, xsink);
      return *xsink ? QoreValue() : val.takeValue(needs_deref);
   }

   ReferenceHolder<QoreListNode> rv(ref_rv ? new QoreListNode : 0, xsink);
   ConstListIterator li(marg->get<const QoreListNode>());
   while (li.next()) {
      // set offset in thread-local data for "$#"
      ImplicitElementHelper eh(li.index());
      const AbstractQoreNode* elem = li.getValue();
      // check if value can be mapped
      SingleArgvContextHelper argv_helper(elem, xsink);

      ValueEvalRefHolder val(left, xsink);
      if (*xsink)
	 return QoreValue();
      if (ref_rv)
	 rv->push(val.getReferencedValue());
   }

   return rv.release();
}

QoreValue QoreMapOperatorNode::mapIterator(AbstractIteratorHelper& h, ExceptionSink* xsink) const {
   ReferenceHolder<QoreListNode> rv(ref_rv ? new QoreListNode : 0, xsink);

   qore_size_t i = 0;
   while (true) {
      bool b = h.next(xsink);
      if (*xsink)
         return QoreValue();
      if (!b)
         break;

      // set offset in thread-local data for "$#"
      ImplicitElementHelper eh(i++);

      // check if value can be mapped
      ValueHolder iv(h.getValue(xsink), xsink);
      //ReferenceHolder<> iv(h.getValue(xsink), xsink);
      if (*xsink)
         return QoreValue();
      SingleArgvContextHelper argv_helper(*iv, xsink);
      //printd(5, "op_map() e[0]=%p (%d %s)\n", e[0], e[0]->getType(), e[0]->getTypeName());
      ValueEvalRefHolder val(left, xsink);
      if (*xsink)
         return QoreValue();
      if (ref_rv)
	 rv->push(val.getReferencedValue());
   }

   return rv.release();
}
