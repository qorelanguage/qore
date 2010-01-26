/*
  FunctionCallNode.cc

  Qore Programming Language

  Copyright 2003 - 2010 David Nichols

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
AbstractQoreNode *AbstractFunctionCallNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const {
   needs_deref = true;
   return evalImpl(xsink);
}

int64 AbstractFunctionCallNode::bigIntEvalImpl(ExceptionSink *xsink) const {
   ReferenceHolder<AbstractQoreNode> rv(evalImpl(xsink), xsink);
   return rv ? rv->getAsBigInt() : 0;
}

int AbstractFunctionCallNode::integerEvalImpl(ExceptionSink *xsink) const {
   ReferenceHolder<AbstractQoreNode> rv(evalImpl(xsink), xsink);
   return rv ? rv->getAsInt() : 0;
}

bool AbstractFunctionCallNode::boolEvalImpl(ExceptionSink *xsink) const {
   ReferenceHolder<AbstractQoreNode> rv(evalImpl(xsink), xsink);
   return rv ? rv->getAsBool() : 0;
}

double AbstractFunctionCallNode::floatEvalImpl(ExceptionSink *xsink) const {
   ReferenceHolder<AbstractQoreNode> rv(evalImpl(xsink), xsink);
   return rv ? rv->getAsFloat() : 0;
}

AbstractQoreNode *SelfFunctionCallNode::evalImpl(ExceptionSink *xsink) const {
   QoreObject *self = getStackObject();
   
   //printd(0, "SelfFunctionCallNode::evalImpl() this=%p self=%p func=%p (name=%s ns=%s)\n", this, self, func, name ? name : "(null)", ns ? ns->ostr : "(null)");

   if (func)
      return self->evalMethod(*func, args, xsink);
   // otherwise exec copy method
   return self->getClass()->execCopy(self, xsink);
}

// called at parse time
AbstractQoreNode *SelfFunctionCallNode::parseInit(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&returnTypeInfo) {
   if (!oflag) {
      parse_error("cannot call member function '%s' out of an object member function definition", getName());
      return this;
   }

   assert(name || ns);

   ParamList *params = 0;
#ifdef DEBUG
   if (ns)
      printd(5, "SelfFunctionCallNode::parseInit() this=%p resolving base class call '%s'\n", this, ns->ostr);
   else 
      printd(5, "SelfFunctionCallNode::parseInit() this=%p resolving '%s'\n", this, name ? name : "(null)");
   assert(!func);
#endif
   if (name) {
      // copy method calls will be recognized by name = 0
      if (!strcmp(name, "copy")) {
	 free(name);
	 name = 0;
	 printd(5, "SelfFunctionCallNode::parseInit() this=%p resolved to copy constructor\n", this);
      }
      else
	 func = getParseClass()->resolveSelfMethod(name);
   }
   else
      func = getParseClass()->resolveSelfMethod(ns);

   if (func) {
      params = func->getParams();
      returnTypeInfo = func->getReturnTypeInfo();
      printd(5, "SelfFunctionCallNode::parseInit() this=%p resolved '%s' to %p\n", this, func->getName(), func);
      if (name) {
	 free(name);
	 name = 0;
      }
      else if (ns) {
	 delete ns;
	 ns = 0;
      }

   }

   lvids += parseArgs(oflag, pflag, params);
   return this;
}

int SelfFunctionCallNode::getAsString(QoreString &str, int foff, ExceptionSink *xsink) const {
   str.sprintf("in-object method call (0x%p) to %s::%s()", this, func->getClass()->getName(), func->getName());
   return 0;
}

// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
QoreString *SelfFunctionCallNode::getAsString(bool &del, int foff, ExceptionSink *xsink) const {
   del = true;
   QoreString *rv = new QoreString();
   getAsString(*rv, foff, xsink);
   return rv;
}

AbstractQoreNode *SelfFunctionCallNode::makeReferenceNodeAndDeref() {
   AbstractQoreNode *rv;
   if (name)
      rv = new ParseSelfMethodReferenceNode(takeName());
   else {
      assert(ns);
      rv = new ParseScopedSelfMethodReferenceNode(takeNScope());
   }
   deref();
   return rv;
}

FunctionCallNode::FunctionCallNode(const AbstractQoreFunction *af, QoreListNode *a) : AbstractFunctionCallNode(NT_FUNCTION_CALL, a) {
   ftype = FC_RESOLVED_GENERIC;
   f.func = af;
}

FunctionCallNode::FunctionCallNode(char *name, QoreListNode *a) : AbstractFunctionCallNode(NT_FUNCTION_CALL, a) {
   ftype = FC_UNRESOLVED;
   f.c_str = name;
}

FunctionCallNode::FunctionCallNode(QoreProgram *p, const UserFunction *u, QoreListNode *a) : AbstractFunctionCallNode(NT_FUNCTION_CALL, a) {
   ftype = FC_IMPORTED;
   f.ifunc = new ImportedFunctionCall(p, u);
}

FunctionCallNode::~FunctionCallNode() {
   printd(5, "FunctionCallNode::~FunctionCallNode(): ftype=%d args=%p (%s)\n",
	  ftype, args, (ftype == FC_UNRESOLVED && f.c_str) ? f.c_str : "(null)");

   switch (ftype) {
      case FC_RESOLVED_GENERIC:
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

char *FunctionCallNode::takeName() {
   char *str = f.c_str;
   f.c_str = 0;
   return str;
}

// makes a "new" operator call from a function call
AbstractQoreNode *FunctionCallNode::parseMakeNewObject() {
   ScopedObjectCallNode *rv = new ScopedObjectCallNode(new NamedScope(f.c_str), args);
   f.c_str = 0;
   args = 0;
   return rv;
}

bool FunctionCallNode::existsUserParam(unsigned i) const {
   if (ftype == FC_RESOLVED_GENERIC)
      return f.func->isUserCode() ? f.func->numParams() > i : true;
   if (ftype == FC_IMPORTED)
      return f.ifunc->func->params->numParams() > i;
   return true;
}

int FunctionCallNode::getFunctionType() const {
   return ftype;
}

const char *FunctionCallNode::getName() const {
   switch (ftype) {
      case FC_RESOLVED_GENERIC:
	 return f.func->getName();
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
int FunctionCallNode::getAsString(QoreString &str, int foff, ExceptionSink *xsink) const {
   str.sprintf("function call (0x%p)", this);
   return 0;
}

// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
QoreString *FunctionCallNode::getAsString(bool &del, int foff, ExceptionSink *xsink) const {
   del = true;
   QoreString *rv = new QoreString();
   getAsString(*rv, foff, xsink);
   return rv;
}

// returns the type name as a c string
const char *FunctionCallNode::getTypeName() const {
   return "function call";
}

// eval(): return value requires a deref(xsink)
AbstractQoreNode *FunctionCallNode::evalImpl(ExceptionSink *xsink) const {
   switch (ftype) {
      case FC_RESOLVED_GENERIC:
	 return f.func->evalFunction(args, xsink);
      case FC_IMPORTED:
	 return f.ifunc->eval(args, xsink);
   }

   assert(false);
   return 0;
}

AbstractQoreNode *FunctionCallNode::parseInit(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&returnTypeInfo) {
   ParamList *params = 0;
   switch (ftype) {
      case FC_UNRESOLVED:
	 getProgram()->resolveFunction(this, params, returnTypeInfo);
	 break;
      default: // should only be one of the above at parse time
	 assert(false);
   }
   
   lvids += parseArgs(oflag, pflag, params);
   return this;
}
