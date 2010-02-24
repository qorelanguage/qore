/*
 CallReferenceNode.cpp
 
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
#include <qore/intern/QoreClassIntern.h>

#include <stdlib.h>
#include <string.h>

CallReferenceCallNode::CallReferenceCallNode(AbstractQoreNode *n_exp, QoreListNode *n_args) : ParseNode(NT_FUNCREFCALL), exp(n_exp), args(n_args) {
}

CallReferenceCallNode::~CallReferenceCallNode() {
   if (exp)
      exp->deref(0);
   if (args)
      args->deref(0);
}

// get string representation (for %n and %N), foff is for multi-line formatting offset, -1 = no line breaks
// the ExceptionSink is only needed for QoreObject where a method may be executed
// use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using these functions directly
// returns -1 for exception raised, 0 = OK
int CallReferenceCallNode::getAsString(QoreString &str, int foff, ExceptionSink *xsink) const {
   str.sprintf("call reference call (0x%08p)", this);
   return 0;
}

// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
QoreString *CallReferenceCallNode::getAsString(bool &del, int foff, ExceptionSink *xsink) const {
   del = true;
   QoreString *rv = new QoreString();
   getAsString(*rv, foff, xsink);
   return rv;
}

// returns the type name as a c string
const char *CallReferenceCallNode::getTypeName() const {
   return "call reference call";
}

// evalImpl(): return value requires a deref(xsink) if not 0
AbstractQoreNode *CallReferenceCallNode::evalImpl(ExceptionSink *xsink) const {
   ReferenceHolder<AbstractQoreNode> lv(exp->eval(xsink), xsink);
   if (*xsink)
      return 0;

   ResolvedCallReferenceNode *r = dynamic_cast<ResolvedCallReferenceNode *>(*lv);
   if (!r) {
      xsink->raiseException("REFERENCE-CALL-ERROR", "expression does not evaluate to a call reference (evaluated to type '%s')", lv ? lv->getTypeName() : "NOTHING"); 
      return 0;
   }
   return r->exec(args, xsink);
}

// evalImpl(): return value requires a deref(xsink) if not 0
AbstractQoreNode *CallReferenceCallNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const {
   needs_deref = true;
   return CallReferenceCallNode::evalImpl(xsink);
}

int64 CallReferenceCallNode::bigIntEvalImpl(ExceptionSink *xsink) const {
   ReferenceHolder<AbstractQoreNode> rv(CallReferenceCallNode::evalImpl(xsink), xsink);
   return rv ? rv->getAsBigInt() : 0;
}

int CallReferenceCallNode::integerEvalImpl(ExceptionSink *xsink) const {
   ReferenceHolder<AbstractQoreNode> rv(CallReferenceCallNode::evalImpl(xsink), xsink);
   return rv ? rv->getAsInt() : 0;
}

bool CallReferenceCallNode::boolEvalImpl(ExceptionSink *xsink) const {
   ReferenceHolder<AbstractQoreNode> rv(CallReferenceCallNode::evalImpl(xsink), xsink);
   return rv ? rv->getAsBool() : 0;
}

double CallReferenceCallNode::floatEvalImpl(ExceptionSink *xsink) const {
   ReferenceHolder<AbstractQoreNode> rv(CallReferenceCallNode::evalImpl(xsink), xsink);
   return rv ? rv->getAsFloat() : 0;
}

AbstractQoreNode *CallReferenceCallNode::parseInit(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
   pflag &= ~PF_REFERENCE_OK;

   const QoreTypeInfo *expTypeInfo = 0;
   if (exp) {
      exp = exp->parseInit(oflag, pflag, lvids, expTypeInfo);

      if (expTypeInfo->hasType() && !codeTypeInfo->parseEqual(expTypeInfo)) {
	 // raise parse exception only if parse exceptions are enabled
	 if (getProgram()->getParseExceptionSink()) {
	    QoreStringNode *desc = new QoreStringNode("invalid call; expression gives ");
	    expTypeInfo->getThisType(*desc);
	    desc->concat(", but a call reference or closure is required to make a call");
	    getProgram()->makeParseException("PARSE-TYPE-ERROR", desc);
	 }
      }
   }

   if (args) {
      bool needs_eval = args->needs_eval();
      
      ListIterator li(args);
      while (li.next()) {
	 AbstractQoreNode **n = li.getValuePtr();
	 if (*n) {
	    const QoreTypeInfo *argTypeInfo = 0;
	    if ((*n)->getType() == NT_REFERENCE)
	       (*n) = (*n)->parseInit(oflag, pflag | PF_REFERENCE_OK, lvids, argTypeInfo);
	    else
	       (*n) = (*n)->parseInit(oflag, pflag, lvids, argTypeInfo);
	    
	    if (!needs_eval && (*n)->needs_eval()) {
	       args->setNeedsEval();
	       needs_eval = true;
	    }
	 }
      }
   }
   
   return this;
}

AbstractCallReferenceNode::AbstractCallReferenceNode(bool n_needs_eval, qore_type_t n_type) : AbstractQoreNode(n_type, false, n_needs_eval) {
}

AbstractCallReferenceNode::AbstractCallReferenceNode(bool n_needs_eval, bool n_there_can_be_only_one, qore_type_t n_type) : AbstractQoreNode(n_type, false, n_needs_eval, n_there_can_be_only_one) {
}

AbstractCallReferenceNode::~AbstractCallReferenceNode() {
}

// parse types should never be copied
AbstractQoreNode *AbstractCallReferenceNode::realCopy() const {
   assert(false);
   return 0;
}

bool AbstractCallReferenceNode::is_equal_soft(const AbstractQoreNode *v, ExceptionSink *xsink) const {
   return is_equal_hard(v, xsink);
}

bool AbstractCallReferenceNode::is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const {
   assert(false);
   return false;
}

// get string representation (for %n and %N), foff is for multi-line formatting offset, -1 = no line breaks
// the ExceptionSink is only needed for QoreObject where a method may be executed
// use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using these functions directly
// returns -1 for exception raised, 0 = OK
int AbstractCallReferenceNode::getAsString(QoreString &str, int foff, ExceptionSink *xsink) const {
   str.sprintf("function reference (0x%08p)", this);
   return 0;
}

// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
QoreString *AbstractCallReferenceNode::getAsString(bool &del, int foff, ExceptionSink *xsink) const {
   del = true;
   QoreString *rv = new QoreString();
   getAsString(*rv, foff, xsink);
   return rv;
}

AbstractQoreNode *AbstractCallReferenceNode::evalImpl(ExceptionSink *xsink) const {
   assert(false);
   return 0;
}

AbstractQoreNode *AbstractCallReferenceNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const {
   assert(false);
   return 0;
}

int64 AbstractCallReferenceNode::bigIntEvalImpl(ExceptionSink *xsink) const {
   assert(false);
   return 0;
}

int AbstractCallReferenceNode::integerEvalImpl(ExceptionSink *xsink) const {
   assert(false);
   return 0;
}

bool AbstractCallReferenceNode::boolEvalImpl(ExceptionSink *xsink) const {
   assert(false);
   return false;
}

double AbstractCallReferenceNode::floatEvalImpl(ExceptionSink *xsink) const {
   assert(false);
   return 0.0;
}

// returns the type name as a c string
const char *AbstractCallReferenceNode::getTypeName() const {
   return getStaticTypeName();
}

ParseObjectMethodReferenceNode::ParseObjectMethodReferenceNode(AbstractQoreNode *n_exp, char *n_method) : exp(n_exp), method(n_method) {
}

ParseObjectMethodReferenceNode::~ParseObjectMethodReferenceNode() {
   if (exp)
      exp->deref(0);
   if (method)
      free(method);
}

// returns a RunTimeObjectMethodReference or NULL if there's an exception
AbstractQoreNode *ParseObjectMethodReferenceNode::evalImpl(ExceptionSink *xsink) const {
   // evaluate lvalue expression
   ReferenceHolder<AbstractQoreNode> lv(exp->eval(xsink), xsink);
   if (*xsink)
      return 0;

   QoreObject *o = dynamic_cast<QoreObject *>(*lv);
   if (!o) {
      xsink->raiseException("OBJECT-METHOD-REFERENCE-ERROR", "expression does not evaluate to an object");
      return 0;
   }
   return new RunTimeObjectMethodReferenceNode(o, method);
}

// evalImpl(): return value requires a deref(xsink) if not 0
AbstractQoreNode *ParseObjectMethodReferenceNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const {
   needs_deref = true;
   return ParseObjectMethodReferenceNode::evalImpl(xsink);
}

int64 ParseObjectMethodReferenceNode::bigIntEvalImpl(ExceptionSink *xsink) const {
   return 0;
}

int ParseObjectMethodReferenceNode::integerEvalImpl(ExceptionSink *xsink) const {
   return 0;
}

bool ParseObjectMethodReferenceNode::boolEvalImpl(ExceptionSink *xsink) const {
   return false;
}

double ParseObjectMethodReferenceNode::floatEvalImpl(ExceptionSink *xsink) const {
   return 0.0;
}

AbstractQoreNode *ParseObjectMethodReferenceNode::parseInit(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
   if (exp) {
      const QoreTypeInfo *argTypeInfo = 0;
      exp = exp->parseInit(oflag, pflag & ~PF_REFERENCE_OK, lvids, argTypeInfo);
   }
   return this;
}

ParseSelfMethodReferenceNode::ParseSelfMethodReferenceNode(char *n_method) : method(n_method) {
}

ParseSelfMethodReferenceNode::~ParseSelfMethodReferenceNode() {
   if (method)
      free(method);
}

// returns a RunTimeObjectMethodReference or NULL if there's an exception
AbstractQoreNode *ParseSelfMethodReferenceNode::evalImpl(ExceptionSink *xsink) const {
   return new RunTimeObjectMethodReferenceNode(getStackObject(), method);
}

// evalImpl(): return value requires a deref(xsink) if not 0
AbstractQoreNode *ParseSelfMethodReferenceNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const {
   needs_deref = true;
   return ParseSelfMethodReferenceNode::evalImpl(xsink);
}

int64 ParseSelfMethodReferenceNode::bigIntEvalImpl(ExceptionSink *xsink) const {
   return 0;
}

int ParseSelfMethodReferenceNode::integerEvalImpl(ExceptionSink *xsink) const {
   return 0;
}

bool ParseSelfMethodReferenceNode::boolEvalImpl(ExceptionSink *xsink) const {
   return false;
}

double ParseSelfMethodReferenceNode::floatEvalImpl(ExceptionSink *xsink) const {
   return 0.0;
}

AbstractQoreNode *ParseSelfMethodReferenceNode::parseInit(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
   if (!oflag)
      parse_error("reference to object member '%s' out of a class member function definition", method);
   return this;
}

ParseScopedSelfMethodReferenceNode::ParseScopedSelfMethodReferenceNode(class NamedScope *n_nscope) : nscope(n_nscope), method(0) {
}

ParseScopedSelfMethodReferenceNode::~ParseScopedSelfMethodReferenceNode() {
   delete nscope;
}

// returns a RunTimeObjectMethodReference or NULL if there's an exception
AbstractQoreNode *ParseScopedSelfMethodReferenceNode::evalImpl(ExceptionSink *xsink) const {
   return new RunTimeObjectScopedMethodReferenceNode(getStackObject(), method);
}

// evalImpl(): return value requires a deref(xsink) if not 0
AbstractQoreNode *ParseScopedSelfMethodReferenceNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const {
   needs_deref = true;
   return ParseScopedSelfMethodReferenceNode::evalImpl(xsink);
}

int64 ParseScopedSelfMethodReferenceNode::bigIntEvalImpl(ExceptionSink *xsink) const {
   return 0;
}

int ParseScopedSelfMethodReferenceNode::integerEvalImpl(ExceptionSink *xsink) const {
   return 0;
}

bool ParseScopedSelfMethodReferenceNode::boolEvalImpl(ExceptionSink *xsink) const {
   return false;
}

double ParseScopedSelfMethodReferenceNode::floatEvalImpl(ExceptionSink *xsink) const {
   return 0.0;
}

AbstractQoreNode *ParseScopedSelfMethodReferenceNode::parseInit(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
   if (!oflag)
      parse_error("reference to object member '%s' out of a class member function definition", method);
   else {
      method = getParseClass()->parseResolveSelfMethod(nscope);
      delete nscope;
      nscope = 0;
   }

   return this;
}

RunTimeObjectScopedMethodReferenceNode::RunTimeObjectScopedMethodReferenceNode(QoreObject *n_obj, const QoreMethod *n_method) : obj(n_obj), method(n_method) {
   printd(5, "RunTimeObjectScopedMethodReferenceNode::RunTimeObjectScopedMethodReferenceNode() this=%08p obj=%08p\n", this, obj);
   obj->tRef();
}

RunTimeObjectScopedMethodReferenceNode::~RunTimeObjectScopedMethodReferenceNode() {
   printd(5, "RunTimeObjectScopedMethodReferenceNode::~RunTimeObjectScopedMethodReferenceNode() this=%08p obj=%08p\n", this, obj);
   obj->tDeref();
}

AbstractQoreNode *RunTimeObjectScopedMethodReferenceNode::exec(const QoreListNode *args, ExceptionSink *xsink) const {
   return obj->evalMethod(*method, args, xsink);
}

QoreProgram *RunTimeObjectScopedMethodReferenceNode::getProgram() const {
   return obj->getProgram();
}

bool RunTimeObjectScopedMethodReferenceNode::is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const {
   const RunTimeObjectScopedMethodReferenceNode *vc = dynamic_cast<const RunTimeObjectScopedMethodReferenceNode *>(v);
   return vc && vc->obj == obj && vc->method == method;
}

RunTimeObjectMethodReferenceNode::RunTimeObjectMethodReferenceNode(QoreObject *n_obj, char *n_method) : obj(n_obj), method(strdup(n_method)) {
   printd(5, "RunTimeObjectMethodReferenceNode::RunTimeObjectMethodReferenceNode() this=%08p obj=%08p (method=%s)\n", this, obj, method);
   obj->tRef();
}

RunTimeObjectMethodReferenceNode::~RunTimeObjectMethodReferenceNode() {
   printd(5, "RunTimeObjectMethodReferenceNode::~RunTimeObjectMethodReferenceNode() this=%08p obj=%08p (method=%s)\n", this, obj, method);
   obj->tDeref();
   free(method);
}

AbstractQoreNode *RunTimeObjectMethodReferenceNode::exec(const QoreListNode *args, ExceptionSink *xsink) const {
   return obj->evalMethod(method, args, xsink);
}

QoreProgram *RunTimeObjectMethodReferenceNode::getProgram() const {
   return obj->getProgram();
}

bool RunTimeObjectMethodReferenceNode::is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const {
   const RunTimeObjectMethodReferenceNode *vc = dynamic_cast<const RunTimeObjectMethodReferenceNode *>(v);
   return vc && obj == vc->obj && !strcmp(vc->method, method);
}

UnresolvedCallReferenceNode::UnresolvedCallReferenceNode(char *n_str) : AbstractUnresolvedCallReferenceNode(false), str(n_str) {
}

UnresolvedCallReferenceNode::~UnresolvedCallReferenceNode() {
   free(str);
}

AbstractQoreNode *UnresolvedCallReferenceNode::parseInit(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
   return ::getProgram()->resolveCallReference(this);
}

/*
void UnresolvedCallReferenceNode::derefImpl(ExceptionSink *xsink) {
   assert(is_unique());
}
*/

AbstractQoreNode *LocalStaticMethodCallReferenceNode::evalImpl(ExceptionSink *xsink) const {
   return new StaticMethodCallReferenceNode(method, ::getProgram());
}

// evalImpl(): return value requires a deref(xsink) if not 0
AbstractQoreNode *LocalStaticMethodCallReferenceNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const {
   needs_deref = true;
   return new StaticMethodCallReferenceNode(method, ::getProgram());
}

AbstractQoreNode *LocalStaticMethodCallReferenceNode::exec(const QoreListNode *args, ExceptionSink *xsink) const {
   return method->eval(0, args, xsink);
}

bool LocalStaticMethodCallReferenceNode::is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const {   
   const LocalStaticMethodCallReferenceNode *vc = dynamic_cast<const LocalStaticMethodCallReferenceNode *>(v);
   //printd(0, "LocalStaticMethodCallReferenceNode::is_equal_hard() %p == %p (%p %s)\n", uf, vc ? vc->uf : 0, v, v ? v->getTypeName() : "n/a");
   return vc && method == vc->method;
}

StaticMethodCallReferenceNode::StaticMethodCallReferenceNode(const QoreMethod *n_method, QoreProgram *n_pgm) : LocalStaticMethodCallReferenceNode(n_method, false), pgm(n_pgm) {
   assert(pgm);
   //printd(5, "StaticMethodCallReferenceNode::StaticMethodCallReferenceNode() this=%p calling QoreProgram::depRef() pgm=%p\n", this, pgm);
   pgm->depRef();
}

bool StaticMethodCallReferenceNode::derefImpl(ExceptionSink *xsink) {
   //printd(5, "StaticMethodCallReferenceNode::deref() this=%p pgm=%p refs: %d -> %d\n", this, pgm, reference_count(), reference_count() - 1);
   pgm->depDeref(xsink);
   return true;
}

AbstractQoreNode *StaticMethodCallReferenceNode::exec(const QoreListNode *args, ExceptionSink *xsink) const {
   ProgramContextHelper pch(pgm, xsink);
   return method->eval(0, args, xsink);
}

UnresolvedStaticMethodCallReferenceNode::UnresolvedStaticMethodCallReferenceNode(NamedScope *n_scope) : AbstractUnresolvedCallReferenceNode(false), scope(n_scope) {
   //printd(0, "UnresolvedStaticMethodCallReferenceNode::UnresolvedStaticMethodCallReferenceNode(%s) this=%p\n", n_scope->ostr, this);
}

UnresolvedStaticMethodCallReferenceNode::~UnresolvedStaticMethodCallReferenceNode() {
   //printd(0, "UnresolvedStaticMethodCallReferenceNode::~UnresolvedStaticMethodCallReferenceNode() this=%p\n", this);
   delete scope;
}

AbstractQoreNode *UnresolvedStaticMethodCallReferenceNode::parseInit(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
   QoreClass *qc = getRootNS()->parseFindScopedClassWithMethod(scope);
   if (!qc)
      return this;
   
   const QoreMethod *qm = qc->parseFindStaticMethodTree(scope->getIdentifier());
   if (!qm) {
      parseException("INVALID-METHOD", "class '%s' has no static method '%s'", qc->getName(), scope->getIdentifier());
      return this;
   }

   assert(qm->isStatic());

   // check class capabilities against parse options
   if (qc->getDomain() & getProgram()->getParseOptions()) {
      parseException("class '%s' implements capabilities that are not allowed by current parse options", qc->getName());
      return this;
   }

   AbstractQoreNode *rv = new LocalStaticMethodCallReferenceNode(qm);
   deref();
   return rv;
}

LocalUserCallReferenceNode::LocalUserCallReferenceNode(const UserFunction *n_uf, bool n_needs_eval) : ResolvedCallReferenceNode(n_needs_eval), uf(n_uf) {
}

LocalUserCallReferenceNode::LocalUserCallReferenceNode(const UserFunction *n_uf) : ResolvedCallReferenceNode(true), uf(n_uf) {
}

AbstractQoreNode *LocalUserCallReferenceNode::evalImpl(ExceptionSink *xsink) const {
   return new UserCallReferenceNode(uf, ::getProgram());
}

// evalImpl(): return value requires a deref(xsink) if not 0
AbstractQoreNode *LocalUserCallReferenceNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const {
   needs_deref = true;
   return new UserCallReferenceNode(uf, ::getProgram());
}

AbstractQoreNode *LocalUserCallReferenceNode::exec(const QoreListNode *args, ExceptionSink *xsink) const {
   return uf->evalFunction(0, args, xsink);
}

bool LocalUserCallReferenceNode::is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const {   
   const LocalUserCallReferenceNode *vc = dynamic_cast<const LocalUserCallReferenceNode *>(v);
   //printd(0, "LocalUserCallReferenceNode::is_equal_hard() %p == %p (%p %s)\n", uf, vc ? vc->uf : 0, v, v ? v->getTypeName() : "n/a");
   return vc && uf == vc->uf;
}

bool UserCallReferenceNode::derefImpl(ExceptionSink *xsink) {
   //printd(5, "UserCallReferenceNode::deref() this=%p pgm=%p refs: %d -> %d\n", this, pgm, reference_count(), reference_count() - 1);
   pgm->depDeref(xsink);
   return true;
}

AbstractQoreNode *UserCallReferenceNode::exec(const QoreListNode *args, ExceptionSink *xsink) const {
   ProgramContextHelper pch(pgm, xsink);
   return uf->evalFunction(0, args, xsink);
}

BuiltinCallReferenceNode::BuiltinCallReferenceNode(const BuiltinFunction *n_bf) : bf(n_bf) {
}

AbstractQoreNode *BuiltinCallReferenceNode::exec(const QoreListNode *args, ExceptionSink *xsink) const {
   return bf->evalFunction(0, args, xsink);
}

bool BuiltinCallReferenceNode::is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const {
   const BuiltinCallReferenceNode *vc = dynamic_cast<const BuiltinCallReferenceNode *>(v);
   return vc && vc->bf == bf;
}

ResolvedCallReferenceNode::ResolvedCallReferenceNode(bool n_needs_eval, qore_type_t n_type) : AbstractCallReferenceNode(n_needs_eval, n_type) {
}

QoreProgram *ResolvedCallReferenceNode::getProgram() const {
   return 0;
}
