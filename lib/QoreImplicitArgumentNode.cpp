/*
  QoreImplicitArgumentNode.cpp

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

QoreImplicitArgumentNode::QoreImplicitArgumentNode(int n_offset) : ParseNode(NT_IMPLICIT_ARG), offset(n_offset)
{
   if (!offset)
      parse_error("implicit argument offsets must be greater than 0 (first implicit argument is $1)");
   else if (offset > 0)
      --offset;
}

QoreImplicitArgumentNode::~QoreImplicitArgumentNode()
{
}

const AbstractQoreNode *QoreImplicitArgumentNode::get() const
{
   const QoreListNode *argv = thread_get_implicit_args();
   if (!argv)
      return 0;
   //printd(5, "QoreImplicitArgumentNode::get() offset=%d v=%08p\n", offset, argv->retrieve_entry(offset));
   return argv->retrieve_entry(offset);
}

AbstractQoreNode *QoreImplicitArgumentNode::evalImpl(ExceptionSink *xsink) const
{
   if (offset == -1) {
      const QoreListNode *argv = thread_get_implicit_args();
      //printd(5, "QoreImplicitArgumentNode::evalImpl() offset=%d argv=%08p (%d)\n", offset, argv, argv ? argv->size() : -1);
      return argv ? argv->refSelf() : 0;
   }

   const AbstractQoreNode *v = get();
   //printd(5, "QoreImplicitArgumentNode::evalImpl() offset=%d v=%08p\n", offset, v);
   return v ? v->refSelf() : 0;
}

AbstractQoreNode *QoreImplicitArgumentNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const
{
   needs_deref = false;
   if (offset == -1)
      return const_cast<QoreListNode *>(thread_get_implicit_args());

   return const_cast<AbstractQoreNode *>(get());
}

int64 QoreImplicitArgumentNode::bigIntEvalImpl(ExceptionSink *xsink) const
{
   if (offset == -1)
      return 0;

   const AbstractQoreNode *v = get();
   return v ? v->getAsBigInt() : 0;
}

int QoreImplicitArgumentNode::integerEvalImpl(ExceptionSink *xsink) const
{
   if (offset == -1)
      return 0;

   const AbstractQoreNode *v = get();
   return v ? v->getAsInt() : 0;
}

bool QoreImplicitArgumentNode::boolEvalImpl(ExceptionSink *xsink) const
{
   if (offset == -1)
      return 0;

   const AbstractQoreNode *v = get();
   return v ? v->getAsBool() : 0;
}

double QoreImplicitArgumentNode::floatEvalImpl(ExceptionSink *xsink) const
{
   if (offset == -1)
      return 0;

   const AbstractQoreNode *v = get();
   return v ? v->getAsFloat() : 0;
}

int QoreImplicitArgumentNode::getAsString(QoreString &str, int foff, ExceptionSink *xsink) const
{
   str.concat("get implicit argument ");
   if (offset == -1)
      str.concat("list");
   else
      str.concat("%d", offset);
   return 0;
}

QoreString *QoreImplicitArgumentNode::getAsString(bool &del, int foff, ExceptionSink *xsink) const
{
   del = true;
   QoreString *rv = new QoreString();
   getAsString(*rv, foff, xsink);
   return rv;
}

const char *QoreImplicitArgumentNode::getTypeName() const
{
   return getStaticTypeName();
}
