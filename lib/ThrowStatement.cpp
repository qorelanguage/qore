/*
  ThrowStatement.cpp

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
#include "qore/intern/ThrowStatement.h"

int ThrowStatement::execImpl(QoreValue& return_value, ExceptionSink *xsink) {
   QoreNodeEvalOptionalRefHolder a(args, xsink);
   if (*xsink)
      return 0;

   assert(get_node_type(*a) == NT_LIST);

   xsink->raiseException(static_cast<const QoreListNode*>(*a));
   return 0;
}

int ThrowStatement::parseInitImpl(LocalVar *oflag, int pflag) {
   if (args) {
      int lvids = 0;

      // turn off top-level flag for statement vars
      pflag &= (~PF_TOP_LEVEL);

      const QoreTypeInfo* argTypeInfo = nullptr;
      args = args->parseInit(oflag, pflag, lvids, argTypeInfo);

      switch (get_node_type(args)) {
         case NT_LIST:
         case NT_PARSE_LIST:
            break;
         default: {
            //printd(5, "ThrowStatement::parseInitImpl() v: %p '%s' e: %d\n", args, get_type_name(args), args->needs_eval());
            QoreListNode* l = new QoreListNode(args->needs_eval());
            l->push(args);
            args = l;
            break;
         }
      }

      return lvids;
   }
   return 0;
}
