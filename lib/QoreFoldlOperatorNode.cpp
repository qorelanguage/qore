/*
  QoreFoldlOperatorNode.cpp

  Qore Programming Language

  Copyright (C) 2003 - 2016 David Nichols

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

QoreString QoreFoldlOperatorNode::foldl_str("foldl operator expression");
QoreString QoreFoldrOperatorNode::foldr_str("foldr operator expression");

// if del is true, then the returned QoreString * should be foldld, if false, then it must not be
QoreString *QoreFoldlOperatorNode::getAsString(bool &del, int foff, ExceptionSink *xsink) const {
   del = false;
   return &foldl_str;
}

int QoreFoldlOperatorNode::getAsString(QoreString &str, int foff, ExceptionSink *xsink) const {
   str.concat(&foldl_str);
   return 0;
}

AbstractQoreNode* QoreFoldlOperatorNode::parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
   assert(!typeInfo);

   pflag &= ~PF_RETURN_VALUE_IGNORED;

   // check iterated expression
   const QoreTypeInfo* expTypeInfo = 0;
   left = left->parseInit(oflag, pflag, lvids, expTypeInfo);

   // check iterator expression
   const QoreTypeInfo* iteratorTypeInfo = 0;
   right = right->parseInit(oflag, pflag, lvids, iteratorTypeInfo);

   // FIXME: if "right" (iterator exp) is a list or an iterator, then the return type is expTypeInfo, otherwise it's the return type of the iterated expression

   return this;
}

QoreValue QoreFoldlOperatorNode::evalValueImpl(bool& needs_deref, ExceptionSink* xsink) const {
   // conditionally evaluate argument
   QoreNodeEvalOptionalRefHolder arg(right, xsink);
   if (!arg || *xsink)
      return QoreValue();

   // return the argument if there is no list
   qore_type_t t = arg->getType();
   if (t != NT_LIST) {
      if (t == NT_OBJECT) {
         AbstractIteratorHelper h(xsink, "foldl operator", const_cast<QoreObject*>(reinterpret_cast<const QoreObject*>(*arg)));
         if (*xsink)
            return QoreValue();
         if (h)
            return foldIterator(h, xsink);
      }
      return arg.getReferencedValue();
   }

   const QoreListNode* l = reinterpret_cast<const QoreListNode*>(*arg);

   // returns NOTHING if the list is empty
   if (!l->size())
      return QoreValue();

   ReferenceHolder<AbstractQoreNode> result(l->get_referenced_entry(0), xsink);

   // return the first element if the list only has one element
   if (l->size() == 1)
      return result.release();

   // skip the first element
   ConstListIterator li(l, 0);
   while (li.next()) {
      // set offset in thread-local data for "$#"
      ImplicitElementHelper eh(li.index());
      // create argument list
      QoreListNode* args = new QoreListNode;
      args->push(result.release());
      args->push(li.getReferencedValue());

      ArgvContextHelper argv_helper(args, xsink);

      result = left->eval(xsink);
      if (*xsink)
	 return QoreValue();
   }
   return result.release();
}

QoreValue QoreFoldlOperatorNode::foldIterator(AbstractIteratorHelper& h, ExceptionSink* xsink) const {
   // set offset in thread-local data for "$#"
   ImplicitElementHelper eh(-1);

   // first try to get first argument
   bool b = h.next(xsink);
   // if there is no first argument or an exception occurred, then return 0
   if (!b || *xsink)
      return QoreValue();

   // get first argument value
   ValueHolder result(h.getValue(xsink), xsink);
   if (*xsink)
      return QoreValue();

   while (true) {
      bool b = h.next(xsink);
      if (*xsink)
         return QoreValue();
      if (!b)
         break;

      // get next argument value
      ValueHolder arg(h.getValue(xsink), xsink);
      if (*xsink)
         return QoreValue();

      // create argument list for fold expression
      QoreListNode* args = new QoreListNode;
      args->push(result.getReferencedValue());
      args->push(arg.getReferencedValue());
      ArgvContextHelper argv_helper(args, xsink);
      result = left->eval(xsink);
      if (*xsink)
         return QoreValue();
   }

   return result.getReferencedValue();
}

// if del is true, then the returned QoreString * should be derefed, if false, then it must not be
QoreString* QoreFoldrOperatorNode::getAsString(bool& del, int foff, ExceptionSink* xsink) const {
   del = false;
   return &foldr_str;
}

int QoreFoldrOperatorNode::getAsString(QoreString& str, int foff, ExceptionSink* xsink) const {
   str.concat(&foldr_str);
   return 0;
}

QoreValue QoreFoldrOperatorNode::evalValueImpl(bool& needs_deref, ExceptionSink* xsink) const {
   // conditionally evaluate argument
   QoreNodeEvalOptionalRefHolder arg(right, xsink);
   if (!arg || *xsink)
      return QoreValue();

   // return the argument if there is no list
   qore_type_t t = arg->getType();
   if (t != NT_LIST) {
      if (t == NT_OBJECT) {
         AbstractIteratorHelper h(xsink, "foldr operator", const_cast<QoreObject*>(reinterpret_cast<const QoreObject*>(*arg)), false);
         if (*xsink)
            return QoreValue();
         if (h)
            return foldIterator(h, xsink);
      }
      return arg.getReferencedValue();
   }

   const QoreListNode* l = reinterpret_cast<const QoreListNode*>(*arg);

   // returns NOTHING if the list is empty
   if (!l->size())
      return QoreValue();

   ReferenceHolder<AbstractQoreNode> result(l->get_referenced_entry(l->size() - 1), xsink);

   // return the first element if the list only has one element
   if (l->size() == 1)
      return result.release();

   // skip the first element
   ConstListIterator li(l, l->size() - 1);
   while (li.prev()) {
      // set offset in thread-local data for "$#"
      ImplicitElementHelper eh(li.index());
      // create argument list
      QoreListNode* args = new QoreListNode;
      args->push(result.release());
      args->push(li.getReferencedValue());

      ArgvContextHelper argv_helper(args, xsink);

      result = left->eval(xsink);
      if (*xsink)
	 return QoreValue();
   }
   return result.release();
}
