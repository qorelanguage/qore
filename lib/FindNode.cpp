/*
  FindNode.cpp
 
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
#include <qore/intern/FindNode.h>

FindNode::FindNode(AbstractQoreNode *expr, AbstractQoreNode *find_expr, AbstractQoreNode *w) : ParseNode(NT_FIND) {
   exp = expr;
   find_exp = find_expr;
   where = w;
}

FindNode::~FindNode() {
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
int FindNode::getAsString(QoreString &qstr, int foff, ExceptionSink *xsink) const {
   qstr.sprintf("find expression (%p)", this);
   return 0;
}

// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
QoreString *FindNode::getAsString(bool &del, int foff, ExceptionSink *xsink) const {
   del = true;
   QoreString *rv = new QoreString();
   getAsString(*rv, foff, xsink);
   return rv;
}

// returns the type name as a c string
const char *FindNode::getTypeName() const {
   return "find expression";
}

// eval(): return value requires a deref(xsink)
AbstractQoreNode *FindNode::evalImpl(ExceptionSink *xsink) const {
   ReferenceHolder<AbstractQoreNode> rv(xsink);
   ReferenceHolder<Context> context(new Context(0, xsink, find_exp), xsink);
   if (*xsink)
      return 0;
   
   QoreListNode *lrv = 0;
   for (context->pos = 0; context->pos < context->max_pos && !xsink->isEvent(); context->pos++) {
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
      if (rv) {
	 if (!lrv) {
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
AbstractQoreNode *FindNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const {
   needs_deref = true;
   return FindNode::evalImpl(xsink);
}

int64 FindNode::bigIntEvalImpl(ExceptionSink *xsink) const {
   ReferenceHolder<AbstractQoreNode> rv(FindNode::evalImpl(xsink), xsink);
   return rv ? rv->getAsBigInt() : 0;
}

int FindNode::integerEvalImpl(ExceptionSink *xsink) const {
   ReferenceHolder<AbstractQoreNode> rv(FindNode::evalImpl(xsink), xsink);
   return rv ? rv->getAsInt() : 0;
}

bool FindNode::boolEvalImpl(ExceptionSink *xsink) const {
   ReferenceHolder<AbstractQoreNode> rv(FindNode::evalImpl(xsink), xsink);
   return rv ? rv->getAsBool() : 0;
}

double FindNode::floatEvalImpl(ExceptionSink *xsink) const {
   ReferenceHolder<AbstractQoreNode> rv(FindNode::evalImpl(xsink), xsink);
   return rv ? rv->getAsFloat() : 0;
}

AbstractQoreNode *FindNode::parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
   typeInfo = 0;

   push_cvar(0);
   const QoreTypeInfo *argTypeInfo = 0;
   if (find_exp)
      find_exp = find_exp->parseInit(oflag, pflag, lvids, argTypeInfo);
   if (where) {
      argTypeInfo = 0;
      where = where->parseInit(oflag, pflag, lvids, argTypeInfo);
   }
   if (exp) {
      argTypeInfo = 0;
      exp = exp->parseInit(oflag, pflag, lvids, argTypeInfo);
   }
   pop_cvar();
   return this;
}
