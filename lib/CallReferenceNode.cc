/*
 CallReferenceNode.cc
 
 Qore Programming Language
 
 Copyright 2003 - 2008 David Nichols
 
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

CallReferenceCallNode::CallReferenceCallNode(AbstractQoreNode *n_exp, QoreListNode *n_args) : ParseNode(NT_FUNCREFCALL), exp(n_exp), args(n_args)
{
}

CallReferenceCallNode::~CallReferenceCallNode()
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
int CallReferenceCallNode::getAsString(QoreString &str, int foff, ExceptionSink *xsink) const
{
   str.sprintf("call reference call (0x%08p)", this);
   return 0;
}

// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
QoreString *CallReferenceCallNode::getAsString(bool &del, int foff, ExceptionSink *xsink) const
{
   del = true;
   QoreString *rv = new QoreString();
   getAsString(*rv, foff, xsink);
   return rv;
}

// returns the type name as a c string
const char *CallReferenceCallNode::getTypeName() const
{
   return "call reference call";
}

// evalImpl(): return value requires a deref(xsink) if not 0
AbstractQoreNode *CallReferenceCallNode::evalImpl(ExceptionSink *xsink) const
{
   ReferenceHolder<AbstractQoreNode> lv(exp->eval(xsink), xsink);
   if (*xsink)
      return 0;

   ResolvedCallReferenceNode *r = dynamic_cast<ResolvedCallReferenceNode *>(*lv);
   if (!r)
   {
      xsink->raiseException("REFERENCE-CALL-ERROR", "expression does not evaluate to a call reference (evaluated to type '%s')", lv ? lv->getTypeName() : "NOTHING");
      return 0;
   }
   return r->exec(args, xsink);
}

// evalImpl(): return value requires a deref(xsink) if not 0
AbstractQoreNode *CallReferenceCallNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const
{
   needs_deref = true;
   return CallReferenceCallNode::evalImpl(xsink);
}

int64 CallReferenceCallNode::bigIntEvalImpl(ExceptionSink *xsink) const
{
   ReferenceHolder<AbstractQoreNode> rv(CallReferenceCallNode::evalImpl(xsink), xsink);
   return rv ? rv->getAsBigInt() : 0;
}

int CallReferenceCallNode::integerEvalImpl(ExceptionSink *xsink) const
{
   ReferenceHolder<AbstractQoreNode> rv(CallReferenceCallNode::evalImpl(xsink), xsink);
   return rv ? rv->getAsInt() : 0;
}

bool CallReferenceCallNode::boolEvalImpl(ExceptionSink *xsink) const
{
   ReferenceHolder<AbstractQoreNode> rv(CallReferenceCallNode::evalImpl(xsink), xsink);
   return rv ? rv->getAsBool() : 0;
}

double CallReferenceCallNode::floatEvalImpl(ExceptionSink *xsink) const
{
   ReferenceHolder<AbstractQoreNode> rv(CallReferenceCallNode::evalImpl(xsink), xsink);
   return rv ? rv->getAsFloat() : 0;
}

int CallReferenceCallNode::parseInit(LocalVar *oflag, int pflag)
{
   int lvids = process_node(&exp, oflag, pflag);
   lvids += process_list_node(&args, oflag, pflag);
   return lvids;
}

AbstractCallReferenceNode::AbstractCallReferenceNode(bool n_needs_eval) : AbstractQoreNode(NT_FUNCREF, false, n_needs_eval)
{
}

AbstractCallReferenceNode::AbstractCallReferenceNode(bool n_needs_eval, bool n_there_can_be_only_one) : AbstractQoreNode(NT_FUNCREF, false, n_needs_eval, n_there_can_be_only_one)
{
}

AbstractCallReferenceNode::~AbstractCallReferenceNode()
{
}

// parse types should never be copied
AbstractQoreNode *AbstractCallReferenceNode::realCopy() const
{
   assert(false);
   return 0;
}

bool AbstractCallReferenceNode::is_equal_soft(const AbstractQoreNode *v, ExceptionSink *xsink) const
{
   assert(false);
   return false;
}

bool AbstractCallReferenceNode::is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const
{
   assert(false);
   return false;
}

// get string representation (for %n and %N), foff is for multi-line formatting offset, -1 = no line breaks
// the ExceptionSink is only needed for QoreObject where a method may be executed
// use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using these functions directly
// returns -1 for exception raised, 0 = OK
int AbstractCallReferenceNode::getAsString(QoreString &str, int foff, ExceptionSink *xsink) const
{
   str.sprintf("function reference (0x%08p)", this);
   return 0;
}

// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
QoreString *AbstractCallReferenceNode::getAsString(bool &del, int foff, ExceptionSink *xsink) const
{
   del = true;
   QoreString *rv = new QoreString();
   getAsString(*rv, foff, xsink);
   return rv;
}

AbstractQoreNode *AbstractCallReferenceNode::evalImpl(ExceptionSink *xsink) const
{
   assert(false);
   return 0;
}

AbstractQoreNode *AbstractCallReferenceNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const
{
   assert(false);
   return 0;
}

int64 AbstractCallReferenceNode::bigIntEvalImpl(ExceptionSink *xsink) const
{
   assert(false);
   return 0;
}

int AbstractCallReferenceNode::integerEvalImpl(ExceptionSink *xsink) const
{
   assert(false);
   return 0;
}

bool AbstractCallReferenceNode::boolEvalImpl(ExceptionSink *xsink) const
{
   assert(false);
   return false;
}

double AbstractCallReferenceNode::floatEvalImpl(ExceptionSink *xsink) const
{
   assert(false);
   return 0.0;
}

// returns the type name as a c string
const char *AbstractCallReferenceNode::getTypeName() const
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

UnresolvedCallReferenceNode::UnresolvedCallReferenceNode(char *n_str) : AbstractCallReferenceNode(false, true), str(n_str)
{
}

UnresolvedCallReferenceNode::~UnresolvedCallReferenceNode()
{
   free(str);
}

AbstractCallReferenceNode *UnresolvedCallReferenceNode::resolve()
{
   return ::getProgram()->resolveCallReference(this);
}

void UnresolvedCallReferenceNode::deref()
{
   assert(is_unique());
   delete this;
}

/*
void UnresolvedCallReferenceNode::derefImpl(ExceptionSink *xsink)
{
   assert(is_unique());
}
*/

UserCallReferenceNode::UserCallReferenceNode(UserFunction *n_uf, QoreProgram *n_pgm) : uf(n_uf), pgm(n_pgm)
{
   //printd(5, "UserCallReferenceNode::UserCallReferenceNode() this=%08p (%s) calling QoreProgram::depRef() pgm=%08p\n", this, uf->getName(), pgm);
   pgm->depRef();
}

QoreProgram *UserCallReferenceNode::getProgram() const
{
   return pgm;
}

bool UserCallReferenceNode::derefImpl(ExceptionSink *xsink)
{
   //printd(5, "UserCallReferenceNode::deref() this=%08p pgm=%08p refs: %d -> %d\n", this, pgm, reference_count(), reference_count() - 1);
   //printd(5, "UserCallReferenceNode::deref() this=%08p calling QoreProgram::depDeref() pgm=%08p\n", this, pgm);
   pgm->depDeref(xsink);
   return true;
}

AbstractQoreNode *UserCallReferenceNode::exec(const QoreListNode *args, ExceptionSink *xsink) const
{
   ProgramContextHelper pch(pgm);
   return uf->eval(args, 0, xsink);
}

StaticUserCallReferenceNode::StaticUserCallReferenceNode(UserFunction *n_uf, QoreProgram *n_pgm) : ResolvedCallReferenceNode(true), uf(n_uf), pgm(n_pgm)
{
}

AbstractQoreNode *StaticUserCallReferenceNode::evalImpl(ExceptionSink *xsink) const
{
   return new UserCallReferenceNode(uf, pgm);
}

// evalImpl(): return value requires a deref(xsink) if not 0
AbstractQoreNode *StaticUserCallReferenceNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const
{
   needs_deref = true;
   return new UserCallReferenceNode(uf, pgm);
}

int64 StaticUserCallReferenceNode::bigIntEvalImpl(ExceptionSink *xsink) const
{
   return 0;
}

int StaticUserCallReferenceNode::integerEvalImpl(ExceptionSink *xsink) const
{
   return 0;
}

bool StaticUserCallReferenceNode::boolEvalImpl(ExceptionSink *xsink) const
{
   return false;
}

double StaticUserCallReferenceNode::floatEvalImpl(ExceptionSink *xsink) const
{
   return 0.0;
}

AbstractQoreNode *StaticUserCallReferenceNode::exec(const QoreListNode *args, ExceptionSink *xsink) const
{
   assert(false);
   return 0;
}

BuiltinCallReferenceNode::BuiltinCallReferenceNode(const BuiltinFunction *n_bf) : bf(n_bf)
{
}

AbstractQoreNode *BuiltinCallReferenceNode::exec(const QoreListNode *args, ExceptionSink *xsink) const
{
   return bf->eval(args, xsink);
}

ImportedCallReferenceNode::ImportedCallReferenceNode(ImportedFunctionCall *n_ifunc) : ifunc(n_ifunc)
{
}

AbstractQoreNode *ImportedCallReferenceNode::exec(const QoreListNode *args, ExceptionSink *xsink) const
{
   return ifunc->eval(args, xsink);
}

ImportedCallReferenceNode::~ImportedCallReferenceNode()
{
   delete ifunc;
}

QoreProgram *ImportedCallReferenceNode::getProgram() const
{
   return ifunc->pgm;
}

ResolvedCallReferenceNode::ResolvedCallReferenceNode(bool n_needs_eval) : AbstractCallReferenceNode(n_needs_eval)
{
}

QoreProgram *ResolvedCallReferenceNode::getProgram() const
{
   return 0;
}
