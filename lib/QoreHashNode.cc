/*
  QoreHashNode.cc

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols

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

QoreHashNode::QoreHashNode(bool ne) : AbstractQoreNode(NT_HASH), QoreHash(ne)
{
}

QoreHashNode::~QoreHashNode()
{
}

// FIXME: move QoreString * to first argument
// get string representation (for %n and %N), foff is for multi-line formatting offset, -1 = no line breaks
// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
// the ExceptionSink is only needed for QoreObject where a method may be executed
// use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using this function directly
QoreString *QoreHashNode::getAsString(bool &del, int foff, ExceptionSink *xsink) const
{
   return QoreHash::getAsString(del, foff, xsink);
}

int QoreHashNode::getAsString(QoreString &str, int foff, ExceptionSink *xsink) const
{
   return QoreHash::getAsString(str, foff, xsink);
}

bool QoreHashNode::needs_eval() const
{
   return QoreHash::needsEval();
}

AbstractQoreNode *QoreHashNode::realCopy() const
{
   return copy();
}

// performs a lexical compare, return -1, 0, or 1 if the "this" value is less than, equal, or greater than
// the "val" passed
//DLLLOCAL virtual int compare(const AbstractQoreNode *val) const;
// the type passed must always be equal to the current type
bool QoreHashNode::is_equal_soft(const AbstractQoreNode *v, ExceptionSink *xsink) const
{
   if (!v || v->type != NT_HASH)
      return false;

   return !compareSoft(reinterpret_cast<const QoreHashNode *>(v), xsink);
}

bool QoreHashNode::is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const
{
   if (!v || v->type != NT_HASH)
      return false;

   return !compareHard(reinterpret_cast<const QoreHashNode *>(v), xsink);
}

// returns the data type
const QoreType *QoreHashNode::getType() const
{
   return NT_HASH;
}

const char *QoreHashNode::getTypeName() const
{
   return "hash";
}

void QoreHashNode::deref(ExceptionSink *xsink)
{
   if (ROdereference()) {
      dereference(xsink);
      delete this;
   }
}

QoreHashNode *QoreHashNode::copy() const
{
   QoreHashNode *h = new QoreHashNode();

   copy_intern(h);
   return h;
}

AbstractQoreNode *QoreHashNode::eval(ExceptionSink *xsink) const
{
   if (!needs_eval())
      return refSelf();

   ReferenceHolder<QoreHashNode> h(new QoreHashNode(), xsink);

   if (!eval_intern(*h, xsink))
      return h.release();

   return 0;
}

AbstractQoreNode *QoreHashNode::eval(bool &needs_deref, ExceptionSink *xsink) const
{
   if (!needs_eval()) {
      needs_deref = false;
      return const_cast<QoreHashNode *>(this);
   }

   needs_deref = true;
   ReferenceHolder<QoreHashNode> h(new QoreHashNode(), xsink);

   if (!eval_intern(*h, xsink))
      return h.release();

   needs_deref = false;
   return 0;
}

bool QoreHashNode::is_value() const
{
   return !needs_eval();
}

QoreHashNode *QoreHashNode::hashRefSelf() const
{
   ref();
   return const_cast<QoreHashNode *>(this);
}
