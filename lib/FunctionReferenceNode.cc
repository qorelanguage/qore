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

// returns the type name as a c string
const char *FunctionReferenceCallNode::getTypeName() const
{
   return "call reference call";
}

// evalImpl(): return value requires a deref(xsink) if not 0
AbstractQoreNode *FunctionReferenceCallNode::evalImpl(ExceptionSink *xsink) const
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

// evalImpl(): return value requires a deref(xsink) if not 0
AbstractQoreNode *FunctionReferenceCallNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const
{
   needs_deref = true;
   return FunctionReferenceCallNode::evalImpl(xsink);
}

int64 FunctionReferenceCallNode::bigIntEvalImpl(ExceptionSink *xsink) const
{
   ReferenceHolder<AbstractQoreNode> rv(FunctionReferenceCallNode::evalImpl(xsink), xsink);
   return rv ? rv->getAsBigInt() : 0;
}

int FunctionReferenceCallNode::integerEvalImpl(ExceptionSink *xsink) const
{
   ReferenceHolder<AbstractQoreNode> rv(FunctionReferenceCallNode::evalImpl(xsink), xsink);
   return rv ? rv->getAsInt() : 0;
}

bool FunctionReferenceCallNode::boolEvalImpl(ExceptionSink *xsink) const
{
   ReferenceHolder<AbstractQoreNode> rv(FunctionReferenceCallNode::evalImpl(xsink), xsink);
   return rv ? rv->getAsBool() : 0;
}

double FunctionReferenceCallNode::floatEvalImpl(ExceptionSink *xsink) const
{
   ReferenceHolder<AbstractQoreNode> rv(FunctionReferenceCallNode::evalImpl(xsink), xsink);
   return rv ? rv->getAsFloat() : 0;
}

int FunctionReferenceCallNode::parseInit(LocalVar *oflag, int pflag)
{
   int lvids = process_node(&exp, oflag, pflag);
   lvids += process_list_node(&args, oflag, pflag);
   return lvids;
}

AbstractFunctionReferenceNode::AbstractFunctionReferenceNode(bool n_needs_eval) : AbstractQoreNode(NT_FUNCREF, false, n_needs_eval)
{
}

AbstractFunctionReferenceNode::AbstractFunctionReferenceNode(bool n_needs_eval, bool n_there_can_be_only_one) : AbstractQoreNode(NT_FUNCREF, false, n_needs_eval, n_there_can_be_only_one)
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

AbstractQoreNode *AbstractFunctionReferenceNode::evalImpl(ExceptionSink *xsink) const
{
   assert(false);
   return 0;
}

AbstractQoreNode *AbstractFunctionReferenceNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const
{
   assert(false);
   return 0;
}

int64 AbstractFunctionReferenceNode::bigIntEvalImpl(ExceptionSink *xsink) const
{
   assert(false);
   return 0;
}

int AbstractFunctionReferenceNode::integerEvalImpl(ExceptionSink *xsink) const
{
   assert(false);
   return 0;
}

bool AbstractFunctionReferenceNode::boolEvalImpl(ExceptionSink *xsink) const
{
   assert(false);
   return false;
}

double AbstractFunctionReferenceNode::floatEvalImpl(ExceptionSink *xsink) const
{
   assert(false);
   return 0.0;
}

// returns the type name as a c string
const char *AbstractFunctionReferenceNode::getTypeName() const
{
   return getStaticTypeName();
}

ParseObjectMethodReferenceNode::ParseObjectMethodReferenceNode(AbstractQoreNode *n_exp, char *n_method) : exp(n_exp), method(n_method)
{
}

ParseObjectMethodReferenceNode::~ParseObjectMethodReferenceNode()
{
   if (exp)
      exp->deref(0);
   if (method)
      free(method);
}

// returns a RunTimeObjectMethodReference or NULL if there's an exception
AbstractQoreNode *ParseObjectMethodReferenceNode::evalImpl(ExceptionSink *xsink) const
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

// evalImpl(): return value requires a deref(xsink) if not 0
AbstractQoreNode *ParseObjectMethodReferenceNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const
{
   needs_deref = true;
   return ParseObjectMethodReferenceNode::evalImpl(xsink);
}

int64 ParseObjectMethodReferenceNode::bigIntEvalImpl(ExceptionSink *xsink) const
{
   return 0;
}

int ParseObjectMethodReferenceNode::integerEvalImpl(ExceptionSink *xsink) const
{
   return 0;
}

bool ParseObjectMethodReferenceNode::boolEvalImpl(ExceptionSink *xsink) const
{
   return false;
}

double ParseObjectMethodReferenceNode::floatEvalImpl(ExceptionSink *xsink) const
{
   return 0.0;
}

int ParseObjectMethodReferenceNode::parseInit(LocalVar *oflag, int pflag)
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
AbstractQoreNode *ParseSelfMethodReferenceNode::evalImpl(ExceptionSink *xsink) const
{
   return new RunTimeObjectMethodReferenceNode(getStackObject(), method);
}

// evalImpl(): return value requires a deref(xsink) if not 0
AbstractQoreNode *ParseSelfMethodReferenceNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const
{
   needs_deref = true;
   return ParseSelfMethodReferenceNode::evalImpl(xsink);
}

int64 ParseSelfMethodReferenceNode::bigIntEvalImpl(ExceptionSink *xsink) const
{
   return 0;
}

int ParseSelfMethodReferenceNode::integerEvalImpl(ExceptionSink *xsink) const
{
   return 0;
}

bool ParseSelfMethodReferenceNode::boolEvalImpl(ExceptionSink *xsink) const
{
   return false;
}

double ParseSelfMethodReferenceNode::floatEvalImpl(ExceptionSink *xsink) const
{
   return 0.0;
}

int ParseSelfMethodReferenceNode::parseInit(LocalVar *oflag, int pflag)
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
AbstractQoreNode *ParseScopedSelfMethodReferenceNode::evalImpl(ExceptionSink *xsink) const
{
   return new RunTimeObjectScopedMethodReferenceNode(getStackObject(), method);
}

// evalImpl(): return value requires a deref(xsink) if not 0
AbstractQoreNode *ParseScopedSelfMethodReferenceNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const
{
   needs_deref = true;
   return ParseScopedSelfMethodReferenceNode::evalImpl(xsink);
}

int64 ParseScopedSelfMethodReferenceNode::bigIntEvalImpl(ExceptionSink *xsink) const
{
   return 0;
}

int ParseScopedSelfMethodReferenceNode::integerEvalImpl(ExceptionSink *xsink) const
{
   return 0;
}

bool ParseScopedSelfMethodReferenceNode::boolEvalImpl(ExceptionSink *xsink) const
{
   return false;
}

double ParseScopedSelfMethodReferenceNode::floatEvalImpl(ExceptionSink *xsink) const
{
   return 0.0;
}

int ParseScopedSelfMethodReferenceNode::parseInit(LocalVar *oflag, int pflag)
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

UnresolvedFunctionReferenceNode::UnresolvedFunctionReferenceNode(char *n_str) : AbstractFunctionReferenceNode(false, true), str(n_str)
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

/*
void UnresolvedFunctionReferenceNode::derefImpl(ExceptionSink *xsink)
{
   assert(is_unique());
}
*/

UserFunctionReferenceNode::UserFunctionReferenceNode(UserFunction *n_uf, QoreProgram *n_pgm) : uf(n_uf), pgm(n_pgm)
{
   //printd(5, "UserFunctionReferenceNode::UserFunctionReferenceNode() this=%08p (%s) calling QoreProgram::depRef() pgm=%08p\n", this, uf->getName(), pgm);
   pgm->depRef();
}

QoreProgram *UserFunctionReferenceNode::getProgram() const
{
   return pgm;
}

bool UserFunctionReferenceNode::derefImpl(ExceptionSink *xsink)
{
   //printd(5, "UserFunctionReferenceNode::deref() this=%08p pgm=%08p refs: %d -> %d\n", this, pgm, reference_count(), reference_count() - 1);
   //printd(5, "UserFunctionReferenceNode::deref() this=%08p calling QoreProgram::depDeref() pgm=%08p\n", this, pgm);
   pgm->depDeref(xsink);
   return true;
}

AbstractQoreNode *UserFunctionReferenceNode::exec(const QoreListNode *args, ExceptionSink *xsink) const
{
   ProgramContextHelper pch(pgm);
   return uf->eval(args, 0, xsink);
}

StaticUserFunctionReferenceNode::StaticUserFunctionReferenceNode(UserFunction *n_uf, QoreProgram *n_pgm) : ResolvedFunctionReferenceNode(true), uf(n_uf), pgm(n_pgm)
{
}

AbstractQoreNode *StaticUserFunctionReferenceNode::evalImpl(ExceptionSink *xsink) const
{
   return new UserFunctionReferenceNode(uf, pgm);
}

// evalImpl(): return value requires a deref(xsink) if not 0
AbstractQoreNode *StaticUserFunctionReferenceNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const
{
   needs_deref = true;
   return new UserFunctionReferenceNode(uf, pgm);
}

int64 StaticUserFunctionReferenceNode::bigIntEvalImpl(ExceptionSink *xsink) const
{
   return 0;
}

int StaticUserFunctionReferenceNode::integerEvalImpl(ExceptionSink *xsink) const
{
   return 0;
}

bool StaticUserFunctionReferenceNode::boolEvalImpl(ExceptionSink *xsink) const
{
   return false;
}

double StaticUserFunctionReferenceNode::floatEvalImpl(ExceptionSink *xsink) const
{
   return 0.0;
}

/*
bool StaticUserFunctionReferenceNode::needs_eval() const
{
   return true;
}
*/

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

ResolvedFunctionReferenceNode::ResolvedFunctionReferenceNode(bool n_needs_eval) : AbstractFunctionReferenceNode(n_needs_eval)
{
}

QoreProgram *ResolvedFunctionReferenceNode::getProgram() const
{
   return 0;
}
