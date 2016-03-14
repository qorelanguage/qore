/*
  QoreNullCoalescingOperatorNode.cpp
 
  Qore Programming Language
 
  Copyright (C) 2003 - 2015 Qore Technologies, sro
  
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

#include <qore/intern/QoreNullCoalescingOperatorNode.h>
//#include <qore/intern/qore_program_private.h>

QoreString QoreNullCoalescingOperatorNode::null_coalescing_str("null coalescing operator");

// if del is true, then the returned QoreString * should be mapd, if false, then it must not be
QoreString *QoreNullCoalescingOperatorNode::getAsString(bool &del, int foff, ExceptionSink *xsink) const {
   del = false;
   return &null_coalescing_str;
}

int QoreNullCoalescingOperatorNode::getAsString(QoreString &str, int foff, ExceptionSink *xsink) const {
   str.concat(&null_coalescing_str);
   return 0;
}

AbstractQoreNode* QoreNullCoalescingOperatorNode::parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
   assert(!typeInfo);
   
   const QoreTypeInfo *leftTypeInfo = 0;
   e[0] = e[0]->parseInit(oflag, pflag, lvids, leftTypeInfo);
   
   leftTypeInfo = 0;
   e[1] = e[1]->parseInit(oflag, pflag, lvids, leftTypeInfo);

   return this;
}

QoreValue QoreNullCoalescingOperatorNode::evalValueImpl(bool& needs_deref, ExceptionSink* xsink) const {
   {
      ValueEvalRefHolder arg(e[0], xsink);
      if (*xsink)
         return QoreValue();
      
      if (!arg->isNullOrNothing())
         return arg.takeValue(needs_deref);
   }
   
   ValueEvalRefHolder arg(e[1], xsink);
   if (*xsink)
      return QoreValue();

   return arg.takeValue(needs_deref);
}
