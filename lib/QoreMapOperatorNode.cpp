/*
  QoreMapOperatorNode.cpp
 
  Qore Programming Language
 
  Copyright (C) 2003 - 2014 David Nichols
  
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

AbstractQoreNode* QoreMapOperatorNode::evalImpl(ExceptionSink *xsink) const {
   return map(xsink);
}

AbstractQoreNode* QoreMapOperatorNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const {
   needs_deref = ref_rv;
   return map(xsink);
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

AbstractQoreNode* QoreMapOperatorNode::map(ExceptionSink* xsink) const {
   // conditionally evaluate argument
   QoreNodeEvalOptionalRefHolder arg(right, xsink);
   if (*xsink || is_nothing(*arg))
      return 0;

   if (arg->getType() != NT_LIST) {
      // check if it's an AbstractIterator object
      if (arg->getType() == NT_OBJECT) {
         AbstractIteratorHelper h(xsink, "map operator", const_cast<QoreObject*>(reinterpret_cast<const QoreObject*>(*arg)));
         if (*xsink)
            return 0;
         if (h)
            return mapIterator(h, xsink);
      }
      SingleArgvContextHelper argv_helper(*arg, xsink);
      return left->eval(xsink);
   }

   ReferenceHolder<QoreListNode> rv(ref_rv ? new QoreListNode() : 0, xsink);
   ConstListIterator li(reinterpret_cast<const QoreListNode*>(*arg));
   while (li.next()) {
      // set offset in thread-local data for "$#"
      ImplicitElementHelper eh(li.index());
      SingleArgvContextHelper argv_helper(li.getValue(), xsink);
      //printd(5, "op_map() left=%p (%d %s)\n", left, left->getType(), left->getTypeName());
      ReferenceHolder<AbstractQoreNode> val(left->eval(xsink), xsink);
      if (*xsink)
	 return 0;
      if (ref_rv)
	  rv->push(val.release());
   }

   return rv.release();
}

QoreListNode* QoreMapOperatorNode::mapIterator(AbstractIteratorHelper& h, ExceptionSink* xsink) const {
   ReferenceHolder<QoreListNode> rv(ref_rv ? new QoreListNode : 0, xsink);

   qore_size_t i = 0;
   // set offset in thread-local data for "$#"
   while (true) {
      bool b = h.next(xsink);
      if (*xsink)
         return 0;
      if (!b)
         return rv.release();

      ImplicitElementHelper eh(i++);

      ReferenceHolder<> iv(h.getValue(xsink), xsink);
      if (*xsink)
         return 0;
      SingleArgvContextHelper argv_helper(*iv, xsink);
      //printd(5, "op_map() left=%p (%d %s)\n", left, left->getType(), left->getTypeName());
      ReferenceHolder<AbstractQoreNode> val(left->eval(xsink), xsink);
      if (*xsink)
         return 0;
      if (ref_rv)
          rv->push(val.release());
   }
   
   return rv.release();
}
