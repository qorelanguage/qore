/*
  QoreImplicitArgumentNode.cc

  Qore Programming Language

  Copyright 2003 - 2009 David Nichols

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
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
