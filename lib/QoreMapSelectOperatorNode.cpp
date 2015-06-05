/*
  QoreMapSelectOperatorNode.cpp
 
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

QoreString QoreMapSelectOperatorNode::map_str("map select operator expression");

// if del is true, then the returned QoreString * should be mapd, if false, then it must not be
QoreString *QoreMapSelectOperatorNode::getAsString(bool &del, int foff, ExceptionSink *xsink) const {
   del = false;
   return &map_str;
}

int QoreMapSelectOperatorNode::getAsString(QoreString &str, int foff, ExceptionSink *xsink) const {
   str.concat(&map_str);
   return 0;
}

AbstractQoreNode* QoreMapSelectOperatorNode::parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
   assert(!typeInfo);
   
   pflag &= ~PF_RETURN_VALUE_IGNORED;

   // check iterated expression
   const QoreTypeInfo* expTypeInfo = 0;
   e[0] = e[0]->parseInit(oflag, pflag, lvids, expTypeInfo);

   // check iterator expression
   const QoreTypeInfo* iteratorTypeInfo = 0;
   e[1] = e[1]->parseInit(oflag, pflag, lvids, iteratorTypeInfo);

   // check select expression
   const QoreTypeInfo* selectTypeInfo = 0;
   e[2] = e[2]->parseInit(oflag, pflag, lvids, selectTypeInfo);

   // FIXME: if iterator is a list or an iterator, then the return type is a list, otherwise it's the return type of the iterated expression or NOTHING in case the select experssion evalutes to False

   return this;
}

QoreValue QoreMapSelectOperatorNode::evalValueImpl(bool &needs_deref, ExceptionSink *xsink) const {
   // conditionally evaluate argument expression
   ValueEvalRefHolder marg(e[1], xsink);
   if (*xsink)
      return QoreValue();

   qore_type_t t = marg->getType();
   if (t != NT_LIST) {
      if (t == NT_OBJECT) {
         AbstractIteratorHelper h(xsink, "map operator", marg->get<QoreObject>());
         if (*xsink)
            return QoreValue();
         if (h)
            return mapSelectIterator(h, xsink);
      }
      if (t == NT_NOTHING)
	 return QoreValue();

      ReferenceHolder<> argv_val(marg.getReferencedValue(), xsink);
      SingleArgvContextHelper argv_helper(*argv_val, xsink);

      // check if value can be mapped
      ValueEvalRefHolder result(e[2], xsink);
      if (*xsink || !result->getAsBool())
	 return QoreValue();

      ValueEvalRefHolder val(e[0], xsink);
      if (*xsink)
	return QoreValue();
      return val.takeValue(needs_deref);       
   }

   ReferenceHolder<QoreListNode> rv(ref_rv ? new QoreListNode : 0, xsink);
   ConstListIterator li(marg->get<const QoreListNode>());
   while (li.next()) {
      // set offset in thread-local data for "$#"
      ImplicitElementHelper eh(li.index());
      const AbstractQoreNode* elem = li.getValue();
      // check if value can be mapped
      SingleArgvContextHelper argv_helper(elem, xsink);
      ValueEvalRefHolder result(e[2], xsink);
      if (*xsink)
         return QoreValue();
      if (!result->getAsBool())
         continue;

      ValueEvalRefHolder val(e[0], xsink);
      if (*xsink)
	 return QoreValue();
      if (ref_rv)
	 rv->push(val.getReferencedValue());
   }
   return rv.release();
}

QoreValue QoreMapSelectOperatorNode::mapSelectIterator(AbstractIteratorHelper& h, ExceptionSink* xsink) const {
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
      ReferenceHolder<> iv(h.getValue(xsink), xsink);
      if (*xsink)
         return QoreValue();
      SingleArgvContextHelper argv_helper(*iv, xsink);
      ValueEvalRefHolder result(e[2], xsink);
      if (*xsink)
         return QoreValue();
      if (!result->getAsBool())
         continue;

      //printd(5, "op_map() e[0]=%p (%d %s)\n", e[0], e[0]->getType(), e[0]->getTypeName());
      ValueEvalRefHolder val(e[0], xsink);
      if (*xsink)
         return QoreValue();
      if (ref_rv)
	 rv->push(val.getReferencedValue());
   }
   
   return rv.release();
}
