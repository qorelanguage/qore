/*
 FunctionReferenceNode.cc
 
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

#include <stdlib.h>
#include <string.h>

FunctionReferenceCallNode::FunctionReferenceCallNode(AbstractQoreNode *n_exp, QoreListNode *n_args) : ParseNode(NT_FUNCREFCALL), exp(n_exp), args(n_args)
{
}

FunctionReferenceCallNode::~FunctionReferenceCallNode()
{
   if (exp)
      exp->deref(0);
   if (args)
      args->deref(0);
}

// get string representation (for %n and %N), foff is for multi-line formatting offset, -1 = no line breaks
// the ExceptionSink is only needed for QoreObject where a method may be executed
// use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using these functions directly
// returns -1 for exception raised, 0 = OK
int FunctionReferenceCallNode::getAsString(QoreString &str, int foff, ExceptionSink *xsink) const
{
   str.sprintf("call reference call (0x%08p)", this);
   return 0;
}

// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
QoreString *FunctionReferenceCallNode::getAsString(bool &del, int foff, ExceptionSink *xsink) const
{
   del = true;
   QoreString *rv = new QoreString();
   getAsString(*rv, foff, xsink);
   return rv;
}

// returns the data type
const QoreType *FunctionReferenceCallNode::getType() const
{
   return NT_FUNCREFCALL;
}

// returns the type name as a c string
const char *FunctionReferenceCallNode::getTypeName() const
{
   return "call reference call";
}

// eval(): return value requires a deref(xsink)
AbstractQoreNode *FunctionReferenceCallNode::eval(ExceptionSink *xsink) const
{
   ReferenceHolder<AbstractQoreNode> lv(exp->eval(xsink), xsink);
   if (*xsink)
      return 0;

   ResolvedFunctionReferenceNode *r = dynamic_cast<ResolvedFunctionReferenceNode *>(*lv);
   if (!r)
   {
      xsink->raiseException("REFERENCE-CALL-ERROR", "expression does not evaluate to a call reference (evaluated to type '%s')", lv ? lv->getTypeName() : "NOTHING");
      return 0;
   }
   return r->exec(args, xsink);
}

int FunctionReferenceCallNode::parseInit(lvh_t oflag, int pflag)
{
   int lvids = process_node(&exp, oflag, pflag);
   lvids += process_list_node(&args, oflag, pflag);
   return lvids;
}

AbstractFunctionReferenceNode::AbstractFunctionReferenceNode() : AbstractQoreNode(NT_FUNCREF)
{
}

AbstractFunctionReferenceNode::~AbstractFunctionReferenceNode()
{
}

// parse types should never be copied
AbstractQoreNode *AbstractFunctionReferenceNode::realCopy() const
{
   assert(false);
   return 0;
}

bool AbstractFunctionReferenceNode::is_equal_soft(const AbstractQoreNode *v, ExceptionSink *xsink) const
{
   assert(false);
   return false;
}

bool AbstractFunctionReferenceNode::is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const
{
   assert(false);
   return false;
}

bool AbstractFunctionReferenceNode::needs_eval() const
{
   return true;
}

bool AbstractFunctionReferenceNode::is_value() const
{
   return false;
}

// get string representation (for %n and %N), foff is for multi-line formatting offset, -1 = no line breaks
// the ExceptionSink is only needed for QoreObject where a method may be executed
// use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using these functions directly
// returns -1 for exception raised, 0 = OK
int AbstractFunctionReferenceNode::getAsString(QoreString &str, int foff, ExceptionSink *xsink) const
{
   str.sprintf("function reference (0x%08p)", this);
   return 0;
}

// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
QoreString *AbstractFunctionReferenceNode::getAsString(bool &del, int foff, ExceptionSink *xsink) const
{
   del = true;
   QoreString *rv = new QoreString();
   getAsString(*rv, foff, xsink);
   return rv;
}

// returns the data type
const QoreType *AbstractFunctionReferenceNode::getType() const
{
   return NT_FUNCREF;
}

// returns the type name as a c string
const char *AbstractFunctionReferenceNode::getTypeName() const
{
   return "function reference";
}

int64 AbstractFunctionReferenceNode::bigIntEval(ExceptionSink *xsink) const
{
   ReferenceHolder<AbstractQoreNode> rv(eval(xsink), xsink);
   return rv ? rv->getAsBigInt() : 0;
}

int AbstractFunctionReferenceNode::integerEval(ExceptionSink *xsink) const
{
   ReferenceHolder<AbstractQoreNode> rv(eval(xsink), xsink);
   return rv ? rv->getAsInt() : 0;
}

bool AbstractFunctionReferenceNode::boolEval(ExceptionSink *xsink) const
{
   ReferenceHolder<AbstractQoreNode> rv(eval(xsink), xsink);
   return rv ? rv->getAsBool() : 0;
}

double AbstractFunctionReferenceNode::floatEval(ExceptionSink *xsink) const
{
   ReferenceHolder<AbstractQoreNode> rv(eval(xsink), xsink);
   return rv ? rv->getAsFloat() : 0;
}

ParseObjectMethodReferenceNode::ParseObjectMethodReferenceNode(AbstractQoreNode *n_exp, char *n_method) : exp(n_exp), method(n_method)
{
}

ParseObjectMethodReferenceNode::~ParseObjectMethodReferenceNode()
{
   if (exp)
      exp->deref(NULL);
   if (method)
      free(method);
}

// returns a RunTimeObjectMethodReference or NULL if there's an exception
AbstractQoreNode *ParseObjectMethodReferenceNode::eval(ExceptionSink *xsink) const
{
   // evaluate lvalue expression
   ReferenceHolder<AbstractQoreNode> lv(exp->eval(xsink), xsink);
   if (*xsink)
      return 0;

   QoreObject *o = dynamic_cast<QoreObject *>(*lv);
   if (!o)
   {
      xsink->raiseException("OBJECT-METHOD-REFERENCE-ERROR", "expression does not evaluate to an object");
      return 0;
   }
   return new RunTimeObjectMethodReferenceNode(o, method);
}

int ParseObjectMethodReferenceNode::parseInit(lvh_t oflag, int pflag)
{
   return process_node(&exp, oflag, pflag);
}

ParseSelfMethodReferenceNode::ParseSelfMethodReferenceNode(char *n_method) : method(n_method)
{
}

ParseSelfMethodReferenceNode::~ParseSelfMethodReferenceNode()
{
   if (method)
      free(method);
}

// returns a RunTimeObjectMethodReference or NULL if there's an exception
AbstractQoreNode *ParseSelfMethodReferenceNode::eval(ExceptionSink *xsink) const
{
   return new RunTimeObjectMethodReferenceNode(getStackObject(), method);
}

int ParseSelfMethodReferenceNode::parseInit(lvh_t oflag, int pflag)
{
   if (!oflag)
      parse_error("reference to object member '%s' out of a class member function definition", method);
   return 0;
}

ParseScopedSelfMethodReferenceNode::ParseScopedSelfMethodReferenceNode(class NamedScope *n_nscope) : nscope(n_nscope), method(0)
{
}

ParseScopedSelfMethodReferenceNode::~ParseScopedSelfMethodReferenceNode()
{
   delete nscope;
}

// returns a RunTimeObjectMethodReference or NULL if there's an exception
AbstractQoreNode *ParseScopedSelfMethodReferenceNode::eval(ExceptionSink *xsink) const
{
   return new RunTimeObjectScopedMethodReferenceNode(getStackObject(), method);
}

int ParseScopedSelfMethodReferenceNode::parseInit(lvh_t oflag, int pflag)
{
   if (!oflag)
      parse_error("reference to object member '%s' out of a class member function definition", method);
   else
   {
      method = getParseClass()->resolveSelfMethod(nscope);
      delete nscope;
      nscope = 0;
   }

   return 0;
}

RunTimeObjectScopedMethodReferenceNode::RunTimeObjectScopedMethodReferenceNode(QoreObject *n_obj, const QoreMethod *n_method) : obj(n_obj), method(n_method)
{
   obj->tRef();
}

RunTimeObjectScopedMethodReferenceNode::~RunTimeObjectScopedMethodReferenceNode()
{
   obj->tDeref();
}

AbstractQoreNode *RunTimeObjectScopedMethodReferenceNode::exec(const QoreListNode *args, ExceptionSink *xsink) const
{
   return method->eval(obj, args, xsink);
}

QoreProgram *RunTimeObjectScopedMethodReferenceNode::getProgram() const
{
   return obj->getProgram();
}

RunTimeObjectMethodReferenceNode::RunTimeObjectMethodReferenceNode(QoreObject *n_obj, char *n_method) : obj(n_obj), method(strdup(n_method))
{
   //printd(5, "RunTimeObjectMethodReferenceNode::RunTimeObjectMethodReferenceNode() this=%08p obj=%08p (method=%s)\n", this, obj, method);
   obj->tRef();
}

RunTimeObjectMethodReferenceNode::~RunTimeObjectMethodReferenceNode()
{
   //printd(5, "RunTimeObjectMethodReferenceNode::~RunTimeObjectMethodReferenceNode() this=%08p obj=%08p (method=%s)\n", this, obj, method);
   obj->tDeref();
   free(method);
}

AbstractQoreNode *RunTimeObjectMethodReferenceNode::exec(const QoreListNode *args, ExceptionSink *xsink) const
{
   return obj->getClass()->evalMethod(obj, method, args, xsink);
}

QoreProgram *RunTimeObjectMethodReferenceNode::getProgram() const
{
   return obj->getProgram();
}

UnresolvedFunctionReferenceNode::UnresolvedFunctionReferenceNode(char *n_str) : str(n_str)
{
}

UnresolvedFunctionReferenceNode::~UnresolvedFunctionReferenceNode()
{
   free(str);
}

AbstractFunctionReferenceNode *UnresolvedFunctionReferenceNode::resolve()
{
   return ::getProgram()->resolveFunctionReference(this);
}

void UnresolvedFunctionReferenceNode::deref()
{
   assert(is_unique());
   delete this;
}

void UnresolvedFunctionReferenceNode::deref(ExceptionSink *xsink)
{
   assert(is_unique());
   delete this;
}

UserFunctionReferenceNode::UserFunctionReferenceNode(UserFunction *n_uf, QoreProgram *n_pgm) : uf(n_uf), pgm(n_pgm)
{
   //printd(5, "UserFunctionReferenceNode::UserFunctionReferenceNode() this=%08p (%s) calling QoreProgram::depRef() pgm=%08p\n", this, uf->getName(), pgm);
   pgm->depRef();
}

QoreProgram *UserFunctionReferenceNode::getProgram() const
{
   return pgm;
}

void UserFunctionReferenceNode::deref(ExceptionSink *xsink)
{
   //printd(5, "UserFunctionReferenceNode::deref() this=%08p pgm=%08p refs: %d -> %d\n", this, pgm, reference_count(), reference_count() - 1);
   if (ROdereference())
   {
      //printd(5, "UserFunctionReferenceNode::deref() this=%08p calling QoreProgram::depDeref() pgm=%08p\n", this, pgm);
      pgm->depDeref(xsink);
      delete this;	 
   }
}

AbstractQoreNode *UserFunctionReferenceNode::exec(const QoreListNode *args, ExceptionSink *xsink) const
{
   ProgramContextHelper pch(pgm);
   return uf->eval(args, 0, xsink);
}

StaticUserFunctionReferenceNode::StaticUserFunctionReferenceNode(UserFunction *n_uf, QoreProgram *n_pgm) : uf(n_uf), pgm(n_pgm)
{
}

AbstractQoreNode *StaticUserFunctionReferenceNode::eval(ExceptionSink *xsink) const
{
   return new UserFunctionReferenceNode(uf, pgm);
}

AbstractQoreNode *StaticUserFunctionReferenceNode::exec(const QoreListNode *args, ExceptionSink *xsink) const
{
   assert(false);
   return 0;
}

BuiltinFunctionReferenceNode::BuiltinFunctionReferenceNode(const BuiltinFunction *n_bf) : bf(n_bf)
{
}

AbstractQoreNode *BuiltinFunctionReferenceNode::exec(const QoreListNode *args, ExceptionSink *xsink) const
{
   return bf->eval(args, xsink);
}

ImportedFunctionReferenceNode::ImportedFunctionReferenceNode(ImportedFunctionCall *n_ifunc) : ifunc(n_ifunc)
{
}

AbstractQoreNode *ImportedFunctionReferenceNode::exec(const QoreListNode *args, ExceptionSink *xsink) const
{
   return ifunc->eval(args, xsink);
}

ImportedFunctionReferenceNode::~ImportedFunctionReferenceNode()
{
   delete ifunc;
}

QoreProgram *ImportedFunctionReferenceNode::getProgram() const
{
   return ifunc->pgm;
}
