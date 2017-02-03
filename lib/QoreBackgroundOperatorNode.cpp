/*
  QoreBackgroundOperatorNode.cpp

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

QoreString QoreBackgroundOperatorNode::name("background operator expression");

QoreValue QoreBackgroundOperatorNode::evalValueImpl(bool& needs_deref, ExceptionSink* xsink) const {
   return do_op_background(exp, xsink);
}

AbstractQoreNode* QoreBackgroundOperatorNode::parseInitImpl(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo) {
   typeInfo = bigIntTypeInfo;

   if (pflag & PF_CONST_EXPRESSION)
      parseException("ILLEGAL-OPERATION", "the background operator may not be used in an expression initializing a constant value executed at parse time");

   // turn off "reference ok" and "return value ignored" flags
   pflag &= ~(PF_RETURN_VALUE_IGNORED);

   const QoreTypeInfo* expTypeInfo = 0;
   exp = exp->parseInit(oflag, pflag | PF_BACKGROUND, lvids, expTypeInfo);

   return this;
}
