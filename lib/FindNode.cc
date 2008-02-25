/*
 FindNode.cc
 
 Qore Programming Language
 
 Copyright (C) David Nichols 2003, 2004, 2005, 2006
 
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
#include <qore/intern/FindNode.h>

FindNode::FindNode(AbstractQoreNode *expr, AbstractQoreNode *find_expr, AbstractQoreNode *w) : ParseNode(NT_FIND)
{
   exp = expr;
   find_exp = find_expr;
   where = w;
}

FindNode::~FindNode()
{
   if (find_exp)
      find_exp->deref(0);
   if (exp)
      exp->deref(0);
   if (where)
      where->deref(0);
}

// get string representation (for %n and %N), foff is for multi-line formatting offset, -1 = no line breaks
// the ExceptionSink is only needed for QoreObject where a method may be executed
// use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using these functions directly
// returns -1 for exception raised, 0 = OK
int FindNode::getAsString(QoreString &qstr, int foff, ExceptionSink *xsink) const
{
   qstr.sprintf("find expression (0x%08p)", this);
   return 0;
}

// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
QoreString *FindNode::getAsString(bool &del, int foff, ExceptionSink *xsink) const
{
   del = true;
   QoreString *rv = new QoreString();
   getAsString(*rv, foff, xsink);
   return rv;
}

// returns the type name as a c string
const char *FindNode::getTypeName() const
{
   return "find expression";
}

// eval(): return value requires a deref(xsink)
AbstractQoreNode *FindNode::evalImpl(ExceptionSink *xsink) const
{
   ReferenceHolder<AbstractQoreNode> rv(xsink);
   ReferenceHolder<Context> context(new Context(0, xsink, find_exp), xsink);
   if (*xsink)
      return 0;
   
   QoreListNode *lrv = 0;
   for (context->pos = 0; context->pos < context->max_pos && !xsink->isEvent(); context->pos++)
   {
      printd(4, "FindNode::eval() checking %d/%d\n", context->pos, context->max_pos);
      bool b = context->check_condition(where, xsink);
      if (*xsink)
	 return 0;
      if (!b)
	 continue;

      printd(4, "FindNode::eval() GOT IT: %d\n", context->pos);
      AbstractQoreNode *result = exp->eval(xsink);
      if (*xsink)
	 return 0;
      if (rv)
      {
	 if (!lrv)
	 {
	    lrv = new QoreListNode();
	    lrv->push(rv.release());
	    lrv->push(result);
	    rv = lrv;
	 }
	 else
	    lrv->push(result);
      }
      else
	 rv = result;
   }

   return rv.release();
}

// evalImpl(): return value requires a deref(xsink) if not 0
AbstractQoreNode *FindNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const
{
   needs_deref = true;
   return FindNode::evalImpl(xsink);
}

int64 FindNode::bigIntEvalImpl(ExceptionSink *xsink) const
{
   ReferenceHolder<AbstractQoreNode> rv(FindNode::evalImpl(xsink), xsink);
   return rv ? rv->getAsBigInt() : 0;
}

int FindNode::integerEvalImpl(ExceptionSink *xsink) const
{
   ReferenceHolder<AbstractQoreNode> rv(FindNode::evalImpl(xsink), xsink);
   return rv ? rv->getAsInt() : 0;
}

bool FindNode::boolEvalImpl(ExceptionSink *xsink) const
{
   ReferenceHolder<AbstractQoreNode> rv(FindNode::evalImpl(xsink), xsink);
   return rv ? rv->getAsBool() : 0;
}

double FindNode::floatEvalImpl(ExceptionSink *xsink) const
{
   ReferenceHolder<AbstractQoreNode> rv(FindNode::evalImpl(xsink), xsink);
   return rv ? rv->getAsFloat() : 0;
}
