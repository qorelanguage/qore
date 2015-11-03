/*
  FunctionCallNode.cc

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

// evalImpl(): return value requires a deref(xsink) if not 0
AbstractQoreNode *AbstractFunctionCallNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const
{
   needs_deref = true;
   return evalImpl(xsink);
}

int64 AbstractFunctionCallNode::bigIntEvalImpl(ExceptionSink *xsink) const
{
   ReferenceHolder<AbstractQoreNode> rv(evalImpl(xsink), xsink);
   return rv ? rv->getAsBigInt() : 0;
}

int AbstractFunctionCallNode::integerEvalImpl(ExceptionSink *xsink) const
{
   ReferenceHolder<AbstractQoreNode> rv(evalImpl(xsink), xsink);
   return rv ? rv->getAsInt() : 0;
}

bool AbstractFunctionCallNode::boolEvalImpl(ExceptionSink *xsink) const
{
   ReferenceHolder<AbstractQoreNode> rv(evalImpl(xsink), xsink);
   return rv ? rv->getAsBool() : 0;
}

double AbstractFunctionCallNode::floatEvalImpl(ExceptionSink *xsink) const
{
   ReferenceHolder<AbstractQoreNode> rv(evalImpl(xsink), xsink);
   return rv ? rv->getAsFloat() : 0;
}

FunctionCallNode::FunctionCallNode(const UserFunction *u, QoreListNode *a) : AbstractFunctionCallNode(NT_FUNCTION_CALL, a)
{
   ftype = FC_USER;
   f.ufunc = u;
}

FunctionCallNode::FunctionCallNode(const BuiltinFunction *b, QoreListNode *a) : AbstractFunctionCallNode(NT_FUNCTION_CALL, a)
{
   ftype = FC_BUILTIN;
   f.bfunc = b;
}

FunctionCallNode::FunctionCallNode(QoreListNode *a, char *name) : AbstractFunctionCallNode(NT_FUNCTION_CALL, a)
{
   printd(5, "FunctionCallNode::FunctionCallNode(a=%08p, name=%s) FC_SELF this=%08p\n", a, name, this);
   ftype = FC_SELF;
   f.sfunc = new SelfFunctionCall(name);
}

FunctionCallNode::FunctionCallNode(QoreListNode *a, class NamedScope *n) : AbstractFunctionCallNode(NT_FUNCTION_CALL, a)
{
   printd(5, "FunctionCallNode::FunctionCallNode(a=%08p, n=%s) FC_SELF this=%08p\n", a, n->ostr, this);
   ftype = FC_SELF;
   f.sfunc = new SelfFunctionCall(n);
}

FunctionCallNode::FunctionCallNode(const QoreMethod *m, QoreListNode *a) : AbstractFunctionCallNode(NT_FUNCTION_CALL, a)
{
   printd(5, "FunctionCallNode::FunctionCallNode(a=%08p, method=%08p %s) FC_SELF this=%08p\n", a, m, m->getName(), this);
   ftype = FC_SELF;
   f.sfunc = new SelfFunctionCall(m);
}

FunctionCallNode::FunctionCallNode(char *name, QoreListNode *a) : AbstractFunctionCallNode(NT_FUNCTION_CALL, a)
{
   ftype = FC_UNRESOLVED;
   f.c_str = name;
}

FunctionCallNode::FunctionCallNode(QoreProgram *p, const UserFunction *u, QoreListNode *a) : AbstractFunctionCallNode(NT_FUNCTION_CALL, a)
{
   ftype = FC_IMPORTED;
   f.ifunc = new ImportedFunctionCall(p, u);
}

FunctionCallNode::~FunctionCallNode()
{
   printd(5, "FunctionCallNode::~FunctionCallNode(): ftype=%d args=%08p (%s)\n",
	  ftype, args, (ftype == FC_UNRESOLVED && f.c_str) ? f.c_str : "(null)");

   switch (ftype)
   {
      case FC_USER:
      case FC_BUILTIN:
	 break;
      case FC_SELF:
	 delete f.sfunc;
	 break;
      case FC_UNRESOLVED:
	 if (f.c_str)
	    free(f.c_str);
	 break;
      case FC_IMPORTED:
	 delete f.ifunc;
	 break;
   }
}

char *FunctionCallNode::takeName()
{
   char *str = f.c_str;
   f.c_str = 0;
   return str;
}

// makes a "new" operator call from a function call
AbstractQoreNode *FunctionCallNode::parseMakeNewObject()
{
   ScopedObjectCallNode *rv = new ScopedObjectCallNode(new NamedScope(f.c_str), args);
   f.c_str = 0;
   args = 0;
   return rv;
}

bool FunctionCallNode::existsUserParam(int i) const
{
   if (ftype == FC_USER)
      return f.ufunc->params->num_params > i;
   if (ftype == FC_IMPORTED)
      return f.ifunc->func->params->num_params > i;
   return true;
}

int FunctionCallNode::getFunctionType() const
{
   return ftype;
}

const char *FunctionCallNode::getName() const
{
   switch (ftype)
   {
      case FC_USER:
	 return f.ufunc->getName();
      case FC_BUILTIN:
	 return f.bfunc->getName();
      case FC_SELF:
	 return f.sfunc->name;
      case FC_IMPORTED:
	 return f.ifunc->func->getName();
      case FC_UNRESOLVED:
	 return f.c_str ? f.c_str : "copy";
   }
   return 0;
}

// get string representation (for %n and %N), foff is for multi-line formatting offset, -1 = no line breaks
// the ExceptionSink is only needed for QoreObject where a method may be executed
// use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using these functions directly
// returns -1 for exception raised, 0 = OK
int FunctionCallNode::getAsString(QoreString &str, int foff, ExceptionSink *xsink) const
{
   str.sprintf("function call (0x%08p)", this);
   return 0;
}

// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
QoreString *FunctionCallNode::getAsString(bool &del, int foff, ExceptionSink *xsink) const
{
   del = true;
   QoreString *rv = new QoreString();
   getAsString(*rv, foff, xsink);
   return rv;
}

// returns the type name as a c string
const char *FunctionCallNode::getTypeName() const
{
   return "function call";
}

// eval(): return value requires a deref(xsink)
AbstractQoreNode *FunctionCallNode::evalImpl(ExceptionSink *xsink) const
{
   switch (ftype)
   {
      case FC_USER:
	 return f.ufunc->eval(args, 0, xsink);
      case FC_BUILTIN:
	 return f.bfunc->eval(args, xsink);
      case FC_SELF:
	 return f.sfunc->eval(args, xsink);
      case FC_IMPORTED:
	 return f.ifunc->eval(args, xsink);
   }

   assert(false);
   return 0;
}

int FunctionCallNode::parseInit(LocalVar *oflag, int pflag)
{
   if (ftype == FC_SELF) {
      if (!oflag)
	 parse_error("cannot call member function '%s' out of an object member function definition", f.sfunc->name);
      else
	 f.sfunc->resolve();
   }
   else if (getFunctionType() == FC_UNRESOLVED)
      getProgram()->resolveFunction(this);
   
   return parseArgs(oflag, pflag);   
}
