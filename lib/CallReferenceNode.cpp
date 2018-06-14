/* -*- indent-tabs-mode: nil -*- */
/*
  CallReferenceNode.cpp

  Qore Programming Language

  Copyright (C) 2003 - 2018 Qore Technologies, s.r.o.

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

#include "qore/Qore.h"
#include "qore/intern/QoreClassIntern.h"
#include "qore/intern/qore_program_private.h"
#include "qore/intern/QoreNamespaceIntern.h"
#include "qore/intern/QoreObjectIntern.h"

#include <stdlib.h>
#include <string.h>

CallReferenceCallNode::CallReferenceCallNode(const QoreProgramLocation* loc, AbstractQoreNode* n_exp, QoreParseListNode* n_args) : ParseNode(loc, NT_FUNCREFCALL), exp(n_exp), parse_args(n_args) {
}

CallReferenceCallNode::CallReferenceCallNode(const QoreProgramLocation* loc, AbstractQoreNode* n_exp, QoreListNode* n_args) : ParseNode(loc, NT_FUNCREFCALL), exp(n_exp), args(n_args) {
}

CallReferenceCallNode::~CallReferenceCallNode() {
   if (exp) {
      //printd(5, "CallReferenceCallNode::~CallReferenceCallNode() this: %p exp: %p '%s' type: %d refs: %d\n", this, exp, get_type_name(exp), get_node_type(exp), exp->reference_count());
      exp->deref(0);
   }
   if (parse_args)
      parse_args->deref();
   if (args)
      args->deref(0);
}

// get string representation (for %n and %N), foff is for multi-line formatting offset, -1 = no line breaks
// the ExceptionSink is only needed for QoreObject where a method may be executed
// use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using these functions directly
// returns -1 for exception raised, 0 = OK
int CallReferenceCallNode::getAsString(QoreString& str, int foff, ExceptionSink* xsink) const {
   str.sprintf("call reference call (%p)", this);
   return 0;
}

// if del is true, then the returned QoreString*  should be deleted, if false, then it must not be
QoreString* CallReferenceCallNode::getAsString(bool& del, int foff, ExceptionSink* xsink) const {
   del = true;
   QoreString* rv = new QoreString;
   getAsString(*rv, foff, xsink);
   return rv;
}

// returns the type name as a c string
const char* CallReferenceCallNode::getTypeName() const {
   return "call reference call";
}

// evalImpl(): return value requires a deref(xsink) if not 0
QoreValue CallReferenceCallNode::evalValueImpl(bool& needs_deref, ExceptionSink* xsink) const {
    ReferenceHolder<AbstractQoreNode> lv(exp->eval(xsink), xsink);
    if (*xsink)
        return QoreValue();

    ResolvedCallReferenceNode* r = dynamic_cast<ResolvedCallReferenceNode*>(*lv);
    if (!r) {
        xsink->raiseException(*loc, "REFERENCE-CALL-ERROR", QoreValue(), "expression does not evaluate to a call reference (evaluated to type '%s')", lv ? lv->getTypeName() : "NOTHING");
        return QoreValue();
    }
    return r->execValue(args, xsink);
}

AbstractQoreNode* CallReferenceCallNode::parseInitImpl(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo) {
   // call references calls can return any value
   typeInfo = 0;

   pflag &= ~(PF_RETURN_VALUE_IGNORED);

   const QoreTypeInfo* expTypeInfo = 0;
   if (exp) {
      exp = exp->parseInit(oflag, pflag, lvids, expTypeInfo);

      if (expTypeInfo && codeTypeInfo && QoreTypeInfo::hasType(expTypeInfo) && !QoreTypeInfo::parseAccepts(codeTypeInfo, expTypeInfo)) {
         // raise parse exception
         QoreStringNode* desc = new QoreStringNode("invalid call; expression gives ");
         QoreTypeInfo::getThisType(expTypeInfo, *desc);
         desc->concat(", but a call reference or closure is required to make a call");
         qore_program_private::makeParseException(getProgram(), *loc, "PARSE-TYPE-ERROR", desc);
      }
   }

   if (parse_args) {
      type_vec_t argTypeInfo;
      lvids += parse_args->initArgs(oflag, pflag, argTypeInfo, args);
      parse_args = nullptr;
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
AbstractQoreNode* AbstractCallReferenceNode::realCopy() const {
   assert(false);
   return 0;
}

bool AbstractCallReferenceNode::is_equal_soft(const AbstractQoreNode* v, ExceptionSink* xsink) const {
   return is_equal_hard(v, xsink);
}

bool AbstractCallReferenceNode::is_equal_hard(const AbstractQoreNode* v, ExceptionSink* xsink) const {
   assert(false);
   return false;
}

// get string representation (for %n and %N), foff is for multi-line formatting offset, -1 = no line breaks
// the ExceptionSink is only needed for QoreObject where a method may be executed
// use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using these functions directly
// returns -1 for exception raised, 0 = OK
int AbstractCallReferenceNode::getAsString(QoreString& str, int foff, ExceptionSink* xsink) const {
   str.sprintf("function reference (%p)", this);
   return 0;
}

// if del is true, then the returned QoreString*  should be deleted, if false, then it must not be
QoreString* AbstractCallReferenceNode::getAsString(bool& del, int foff, ExceptionSink* xsink) const {
   del = true;
   QoreString* rv = new QoreString;
   getAsString(*rv, foff, xsink);
   return rv;
}

AbstractQoreNode* AbstractCallReferenceNode::evalImpl(ExceptionSink* xsink) const {
   assert(false);
   return 0;
}

AbstractQoreNode* AbstractCallReferenceNode::evalImpl(bool& needs_deref, ExceptionSink* xsink) const {
   assert(false);
   return 0;
}

int64 AbstractCallReferenceNode::bigIntEvalImpl(ExceptionSink* xsink) const {
   assert(false);
   return 0;
}

int AbstractCallReferenceNode::integerEvalImpl(ExceptionSink* xsink) const {
   assert(false);
   return 0;
}

bool AbstractCallReferenceNode::boolEvalImpl(ExceptionSink* xsink) const {
   assert(false);
   return false;
}

double AbstractCallReferenceNode::floatEvalImpl(ExceptionSink* xsink) const {
   assert(false);
   return 0.0;
}

// returns the type name as a c string
const char* AbstractCallReferenceNode::getTypeName() const {
   return getStaticTypeName();
}

bool AbstractCallReferenceNode::getAsBoolImpl() const {
   // check if we should do perl-style boolean evaluation
   if (runtime_check_parse_option(PO_STRICT_BOOLEAN_EVAL))
      return false;
   return true;
}

ParseObjectMethodReferenceNode::ParseObjectMethodReferenceNode(const QoreProgramLocation* loc, AbstractQoreNode* n_exp, char* n_method) : AbstractParseObjectMethodReferenceNode(loc), exp(n_exp), method(n_method), qc(0), m(0) {
   free(n_method);
}

ParseObjectMethodReferenceNode::~ParseObjectMethodReferenceNode() {
   if (exp)
      exp->deref(0);
}

// returns a RunTimeObjectMethodReference or NULL if there's an exception
QoreValue ParseObjectMethodReferenceNode::evalValueImpl(bool& needs_deref, ExceptionSink* xsink) const {
    // evaluate lvalue expression
    ReferenceHolder<AbstractQoreNode> lv(exp->eval(xsink), xsink);
    if (*xsink)
        return QoreValue();

    QoreObject* o = (*lv) && (*lv)->getType() == NT_OBJECT ? reinterpret_cast<QoreObject*>(*lv) : 0;
    if (!o) {
        xsink->raiseException(*loc, "OBJECT-METHOD-REFERENCE-ERROR", QoreValue(), "expression does not evaluate to an object");
        return QoreValue();
    }

    //printd(5, "ParseObjectMethodReferenceNode::evalImpl() this: %p o: %p %s::%s() m: %p\n", this, o, o->getClassName(), method.c_str(), m);

    const QoreClass* oc = o->getClass();

    // find the method at runtime if necessary
    if (!m) {
        // serialize method resolution at runtime
        AutoLocker al(lck);
        // check m again inside the lock (this way we avoid the lock in the common case where the method has already been resolved)
        if (!m) {
            bool m_priv = false;
            m = oc->findMethod(method.c_str(), m_priv);
            if (!m) {
                m = oc->findStaticMethod(method.c_str(), m_priv);
                if (!m) {
                    xsink->raiseException(*loc, "OBJECT-METHOD-REFERENCE-ERROR", QoreValue(), "cannot resolve reference to %s::%s(): unknown method", o->getClassName(), method.c_str());
                    return QoreValue();
                }
            }

            if (m_priv && !qore_class_private::runtimeCheckPrivateClassAccess(*oc)) {
                if (m->isPrivate())
                    xsink->raiseException(*loc, "ILLEGAL-CALL-REFERENCE", QoreValue(), "cannot create a call reference to private %s::%s() from outside the class", o->getClassName(), method.c_str());
                else
                    xsink->raiseException(*loc, "ILLEGAL-CALL-REFERENCE", QoreValue(), "cannot create a call reference to %s::%s() because the parent class that implements the method (%s::%s()) is privately inherited", o->getClassName(), method.c_str(), m->getClass()->getName(), method.c_str());

                return QoreValue();
            }
        }
    }

    if (oc == m->getClass() || oc == qc)
        return new RunTimeResolvedMethodReferenceNode(loc, o, m);
    return new RunTimeObjectMethodReferenceNode(loc, o, method.c_str());
}

AbstractQoreNode* ParseObjectMethodReferenceNode::parseInitImpl(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo) {
   typeInfo = callReferenceTypeInfo;
   if (exp) {
      const QoreTypeInfo* argTypeInfo = 0;
      exp = exp->parseInit(oflag, pflag, lvids, argTypeInfo);

      if (QoreTypeInfo::hasType(argTypeInfo)) {
         if (objectTypeInfo && argTypeInfo && !QoreTypeInfo::parseAccepts(objectTypeInfo, argTypeInfo)) {
            // raise parse exception
            QoreStringNode* desc = new QoreStringNode("invalid call; object expression gives ");
            QoreTypeInfo::getThisType(argTypeInfo, *desc);
            desc->concat(", but should resolve to an object to make a call with this syntax");
            qore_program_private::makeParseException(getProgram(), *loc, "PARSE-TYPE-ERROR", desc);
         }
         else {
            const QoreClass* n_qc = QoreTypeInfo::getUniqueReturnClass(argTypeInfo);
            if (n_qc) {
               qore_class_private* class_ctx = oflag ? qore_class_private::get(*const_cast<QoreClass*>(QoreTypeInfo::getUniqueReturnClass(oflag->getTypeInfo()))) : parse_get_class_priv();
               if (class_ctx && !qore_class_private::parseCheckPrivateClassAccess(*n_qc, class_ctx))
                  class_ctx = 0;

               // class access is checked internally
               m = qore_class_private::get(*const_cast<QoreClass*>(n_qc))->parseFindAnyMethod(method.c_str(), class_ctx);
               if (m) {
                  if (!m->isStatic() && !strcmp(m->getName(), "copy"))
                     parseException(*loc, "PARSE-ERROR", "cannot take a call reference to copy method %s::%s()", n_qc->getName(), method.c_str());
                  qc = n_qc;
               }
               else
                  parseException(*loc, "PARSE-ERROR", "no method %s::%s() is accessible in this context", n_qc->getName(), method.c_str());
            }
         }
      }
   }
   return this;
}

// returns a RunTimeObjectMethodReferenceNode or NULL if there's an exception
QoreValue ParseSelfMethodReferenceNode::evalValueImpl(bool& needs_deref, ExceptionSink* xsink) const {
   QoreObject* o = runtime_get_stack_object();
   assert(o);

   // return class with method already found at parse time if known
   if (o->getClass() == meth->getClass())
      return new RunTimeResolvedMethodReferenceNode(loc, o, meth);

   return new RunTimeObjectMethodReferenceNode(loc, o, meth->getName());
}

AbstractQoreNode* ParseSelfMethodReferenceNode::parseInitImpl(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo) {
    typeInfo = callReferenceTypeInfo;
    if (!oflag)
        parse_error(*loc, "reference to object member '%s' when not in an object context", method.c_str());
    else {
        assert(!method.empty());
        qore_class_private* class_ctx = qore_class_private::get(*const_cast<QoreClass*>(QoreTypeInfo::getUniqueReturnClass(oflag->getTypeInfo())));
        meth = class_ctx->parseResolveSelfMethod(loc, method.c_str(), class_ctx);
        method.clear();
    }
    return this;
}

ParseScopedSelfMethodReferenceNode::ParseScopedSelfMethodReferenceNode(const QoreProgramLocation* loc, NamedScope* n_nscope) : AbstractParseObjectMethodReferenceNode(loc), nscope(n_nscope), method(0) {
}

ParseScopedSelfMethodReferenceNode::~ParseScopedSelfMethodReferenceNode() {
    delete nscope;
}

// returns a RunTimeObjectMethodReference or NULL if there's an exception
QoreValue ParseScopedSelfMethodReferenceNode::evalValueImpl(bool& needs_deref, ExceptionSink* xsink) const {
    return new RunTimeResolvedMethodReferenceNode(loc, runtime_get_stack_object(), method);
}

AbstractQoreNode* ParseScopedSelfMethodReferenceNode::parseInitImpl(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo) {
    typeInfo = callReferenceTypeInfo;
    if (!oflag)
        parse_error(*loc, "reference to object member '%s' when not in an object context", method);
    else {
        qore_class_private* class_ctx = qore_class_private::get(*const_cast<QoreClass*>(QoreTypeInfo::getUniqueReturnClass(oflag->getTypeInfo())));
        method = class_ctx->parseResolveSelfMethod(loc, nscope);
        delete nscope;
        nscope = 0;
    }

    return this;
}

RunTimeResolvedMethodReferenceNode::RunTimeResolvedMethodReferenceNode(const QoreProgramLocation* loc, QoreObject* n_obj, const QoreMethod* n_method) : ResolvedCallReferenceNodeIntern(loc), obj(n_obj), method(n_method), qc(runtime_get_class()) {
   printd(5, "RunTimeResolvedMethodReferenceNode::RunTimeResolvedMethodReferenceNode() this: %p obj: %p\n", this, obj);
   obj->tRef();
}

RunTimeResolvedMethodReferenceNode::~RunTimeResolvedMethodReferenceNode() {
   printd(5, "RunTimeResolvedMethodReferenceNode::~RunTimeResolvedMethodReferenceNode() this: %p obj: %p\n", this, obj);
   obj->tDeref();
}

QoreValue RunTimeResolvedMethodReferenceNode::execValue(const QoreListNode* args, ExceptionSink* xsink) const {
   //printd(5, "RunTimeResolvedMethodReferenceNode::execValue() cpgm: %p opgm: %p\n", getProgram(), obj->getProgram());
   // issue #2145: do not set the call reference class context before arguments are evaluted
   return qore_method_private::eval(*method, xsink, obj, args, qc);
}

QoreProgram* RunTimeResolvedMethodReferenceNode::getProgram() const {
   return obj->getProgram();
}

bool RunTimeResolvedMethodReferenceNode::is_equal_hard(const AbstractQoreNode* v, ExceptionSink* xsink) const {
   const RunTimeResolvedMethodReferenceNode* vc = dynamic_cast<const RunTimeResolvedMethodReferenceNode*>(v);
   return vc && vc->obj == obj && vc->method == method;
}

RunTimeObjectMethodReferenceNode::RunTimeObjectMethodReferenceNode(const QoreProgramLocation* loc, QoreObject* n_obj, const char* n_method) : ResolvedCallReferenceNodeIntern(loc), obj(n_obj), method(n_method), qc(runtime_get_class()) {
   printd(5, "RunTimeObjectMethodReferenceNode::RunTimeObjectMethodReferenceNode() this: %p obj: %p (method: %s qc: %p)\n", this, obj, n_method, qc);
   obj->tRef();
}

RunTimeObjectMethodReferenceNode::~RunTimeObjectMethodReferenceNode() {
   printd(5, "RunTimeObjectMethodReferenceNode::~RunTimeObjectMethodReferenceNode() this: %p obj: %p (method: %s qc: %p)\n", this, obj, method.c_str(), qc);
   obj->tDeref();
}

QoreValue RunTimeObjectMethodReferenceNode::execValue(const QoreListNode* args, ExceptionSink* xsink) const {
   //printd(5, "RunTimeObjectMethodReferenceNode::exec() this: %p obj: %p %s::%s() qc: %p (%s)\n", this, obj, obj->getClassName(), method.c_str(), qc, qc ? qc->name.c_str() : "n/a");
   OptionalClassObjSubstitutionHelper osh(qc);
   return obj->evalMethod(method.c_str(), args, xsink);
}

QoreProgram* RunTimeObjectMethodReferenceNode::getProgram() const {
   return obj->getProgram();
}

bool RunTimeObjectMethodReferenceNode::is_equal_hard(const AbstractQoreNode* v, ExceptionSink* xsink) const {
   const RunTimeObjectMethodReferenceNode* vc = dynamic_cast<const RunTimeObjectMethodReferenceNode*>(v);
   return vc && obj == vc->obj && method == vc->method;
}

UnresolvedProgramCallReferenceNode::UnresolvedProgramCallReferenceNode(const QoreProgramLocation* loc, char* n_str) : AbstractUnresolvedCallReferenceNode(loc, false), str(n_str) {
}

UnresolvedProgramCallReferenceNode::~UnresolvedProgramCallReferenceNode() {
   free(str);
}

AbstractQoreNode* UnresolvedProgramCallReferenceNode::parseInit(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo) {
   typeInfo = callReferenceTypeInfo;
   return qore_root_ns_private::parseResolveCallReference(this);
}

AbstractQoreNode* UnresolvedCallReferenceNode::parseInit(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo) {
   typeInfo = callReferenceTypeInfo;

   // try to resolve a method call if bare references are allowed
   // and we are parsing in an object context
   if (parse_check_parse_option(PO_ALLOW_BARE_REFS) && oflag) {
      const QoreClass* qc = QoreTypeInfo::getUniqueReturnClass(oflag->getTypeInfo());
      const QoreMethod* m = qore_class_private::parseFindSelfMethod(const_cast<QoreClass*>(qc), str);
      if (m) {
         ParseSelfMethodReferenceNode* rv = new ParseSelfMethodReferenceNode(loc, m);
         delete this;
         return rv;
      }
   }

   return qore_root_ns_private::parseResolveCallReference(this);
}

AbstractQoreNode* LocalStaticMethodCallReferenceNode::evalImpl(ExceptionSink* xsink) const {
   return new StaticMethodCallReferenceNode(loc, method, ::getProgram(), runtime_get_class());
}

// evalImpl(): return value requires a deref(xsink) if not 0
AbstractQoreNode* LocalStaticMethodCallReferenceNode::evalImpl(bool& needs_deref, ExceptionSink* xsink) const {
   needs_deref = true;
   return new StaticMethodCallReferenceNode(loc, method, ::getProgram(), runtime_get_class());
}

QoreValue LocalStaticMethodCallReferenceNode::execValue(const QoreListNode* args, ExceptionSink* xsink) const {
   return qore_method_private::eval(*method, xsink, 0, args);
}

bool LocalStaticMethodCallReferenceNode::is_equal_hard(const AbstractQoreNode* v, ExceptionSink* xsink) const {
   const LocalStaticMethodCallReferenceNode* vc = dynamic_cast<const LocalStaticMethodCallReferenceNode*>(v);
   //printd(5, "LocalStaticMethodCallReferenceNode::is_equal_hard() %p == %p (%p %s)\n", uf, vc ? vc->uf : 0, v, v ? v->getTypeName() : "n/a");
   return vc && method == vc->method;
}

AbstractQoreNode* LocalMethodCallReferenceNode::evalImpl(ExceptionSink* xsink) const {
   return new MethodCallReferenceNode(loc, method, ::getProgram());
}

// evalImpl(): return value requires a deref(xsink) if not 0
AbstractQoreNode* LocalMethodCallReferenceNode::evalImpl(bool& needs_deref, ExceptionSink* xsink) const {
   needs_deref = true;
   return new MethodCallReferenceNode(loc, method, ::getProgram());
}

QoreValue LocalMethodCallReferenceNode::execValue(const QoreListNode* args, ExceptionSink* xsink) const {
   return qore_method_private::eval(*method, xsink, runtime_get_stack_object(), args);
}

bool LocalMethodCallReferenceNode::is_equal_hard(const AbstractQoreNode* v, ExceptionSink* xsink) const {
   const LocalMethodCallReferenceNode* vc = dynamic_cast<const LocalMethodCallReferenceNode*>(v);
   //printd(5, "LocalMethodCallReferenceNode::is_equal_hard() %p == %p (%p %s)\n", uf, vc ? vc->uf : 0, v, v ? v->getTypeName() : "n/a");
   return vc && method == vc->method;
}

StaticMethodCallReferenceNode::StaticMethodCallReferenceNode(const QoreProgramLocation* loc, const QoreMethod* n_method, QoreProgram* n_pgm, const qore_class_private* n_class_ctx) : LocalStaticMethodCallReferenceNode(loc, n_method, false), pgm(n_pgm), class_ctx(n_class_ctx) {
   assert(pgm);
   //printd(5, "StaticMethodCallReferenceNode::StaticMethodCallReferenceNode() this: %p %s::%s()\n", this, n_method->getClass()->getName(), n_method->getName());
   //printd(5, "StaticMethodCallReferenceNode::StaticMethodCallReferenceNode() this: %p calling QoreProgram::depRef() pgm: %p\n", this, pgm);
   // make a weak reference to the Program - a strong reference (QoreProgram::ref()) could cause a recursive reference
   pgm->depRef();
}

bool StaticMethodCallReferenceNode::derefImpl(ExceptionSink* xsink) {
   //printd(5, "StaticMethodCallReferenceNode::deref() this: %p pgm: %p refs: %d -> %d\n", this, pgm, reference_count(), reference_count() - 1);
   pgm->depDeref();
#ifdef DEBUG
   pgm = 0;
#endif
   return true;
}

QoreValue StaticMethodCallReferenceNode::execValue(const QoreListNode* args, ExceptionSink* xsink) const {
   // set class ctx
   ObjectSubstitutionHelper osh(0, class_ctx);
   // do not set pgm context here before evaluating args
   return qore_method_private::eval(*method, xsink, 0, args);
}

MethodCallReferenceNode::MethodCallReferenceNode(const QoreProgramLocation* loc, const QoreMethod* n_method, QoreProgram* n_pgm) : LocalMethodCallReferenceNode(loc, n_method, false), obj(runtime_get_stack_object()) {
   assert(obj);
   //printd(5, "MethodCallReferenceNode::MethodCallReferenceNode() this: %p %s::%s()\n", this, n_method->getClass()->getName(), n_method->getName());
   //printd(5, "MethodCallReferenceNode::MethodCallReferenceNode() this: %p calling QoreProgram::depRef() pgm: %p\n", this, pgm);
   // make a weak reference to the object
   obj->tRef();
}

bool MethodCallReferenceNode::derefImpl(ExceptionSink* xsink) {
   //printd(5, "MethodCallReferenceNode::deref() this: %p pgm: %p refs: %d -> %d\n", this, pgm, reference_count(), reference_count() - 1);
   obj->tDeref();
   return true;
}

QoreValue MethodCallReferenceNode::execValue(const QoreListNode* args, ExceptionSink* xsink) const {
   return qore_method_private::eval(*method, xsink, obj, args);
}

UnresolvedStaticMethodCallReferenceNode::UnresolvedStaticMethodCallReferenceNode(const QoreProgramLocation* loc, NamedScope* n_scope) : AbstractUnresolvedCallReferenceNode(loc, false), scope(n_scope) {
   //printd(5, "UnresolvedStaticMethodCallReferenceNode::UnresolvedStaticMethodCallReferenceNode(%s) this: %p\n", n_scope->ostr, this);
}

UnresolvedStaticMethodCallReferenceNode::~UnresolvedStaticMethodCallReferenceNode() {
   //printd(5, "UnresolvedStaticMethodCallReferenceNode::~UnresolvedStaticMethodCallReferenceNode() this: %p\n", this);
   delete scope;
}

AbstractQoreNode* UnresolvedStaticMethodCallReferenceNode::parseInit(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo) {
   typeInfo = callReferenceTypeInfo;

   QoreClass* qc = qore_root_ns_private::parseFindScopedClassWithMethod(loc, *scope, false);
   if (!qc) {
      // see if this is a function call to a function defined in a namespace
      const QoreFunction* f = qore_root_ns_private::parseResolveFunction(*scope);
      if (f) {
         LocalFunctionCallReferenceNode* fr = new LocalFunctionCallReferenceNode(loc, f);
         deref();
         return fr->parseInit(oflag, pflag, lvids, typeInfo);
      }
      parse_error(*loc, "reference to undefined class '%s' in '%s()'", scope->get(scope->size() - 2), scope->ostr);
      return this;
   }

   qore_class_private* class_ctx = oflag ? qore_class_private::get(*const_cast<QoreClass*>(QoreTypeInfo::getUniqueReturnClass(oflag->getTypeInfo()))) : parse_get_class_priv();
   if (class_ctx && !qore_class_private::parseCheckPrivateClassAccess(*qc, class_ctx))
      class_ctx = 0;

   const QoreMethod* qm = 0;
   // try to find a pointer to a non-static method if parsing in the class's context
   // and bare references are enabled
   if (oflag && parse_check_parse_option(PO_ALLOW_BARE_REFS)) {
      qm = qore_class_private::get(*qc)->parseFindAnyMethodStaticFirst(scope->getIdentifier(), class_ctx);
      //assert(!qm || !qm->isStatic());

      if (qm) {
         if (!qm->isStatic() && !strcmp(qm->getName(), "copy")) {
            parseException(*loc, "INVALID-METHOD", "cannot take a reference to base class copy method %s::%s()", qc->getName(), scope->getIdentifier());
            return this;
         }
      }
      else {
         parseException(*loc, "INVALID-METHOD", "class '%s' has no accessible method '%s'", qc->getName(), scope->getIdentifier());
         return this;
      }
   }
   else {
      qm = qore_class_private::get(*qc)->parseFindStaticMethod(scope->getIdentifier(), class_ctx);

      if (!qm) {
         parseException(*loc, "INVALID-METHOD", "class '%s' has no accessible static method '%s'", qc->getName(), scope->getIdentifier());
         return this;
      }
   }

   // check class capabilities against parse options
   if (qore_program_private::parseAddDomain(getProgram(), qc->getDomain())) {
      parse_error(*loc, "class '%s' implements capabilities that are not allowed by current parse options", qc->getName());
      return this;
   }

   //printd(5, "UnresolvedStaticMethodCallReferenceNode::parseInit() got %s::%s() static: %d\n", qm->getClass()->getName(), qm->getName(), qm->isStatic());
   AbstractQoreNode* rv = qm->isStatic() ? new LocalStaticMethodCallReferenceNode(loc, qm) : new LocalMethodCallReferenceNode(loc, qm);
   deref();
   return rv;
}

LocalFunctionCallReferenceNode::LocalFunctionCallReferenceNode(const QoreProgramLocation* loc, const QoreFunction* n_uf, bool n_needs_eval) : ResolvedCallReferenceNodeIntern(loc, n_needs_eval), uf(n_uf) {
}

LocalFunctionCallReferenceNode::LocalFunctionCallReferenceNode(const QoreProgramLocation* loc, const QoreFunction* n_uf) : ResolvedCallReferenceNodeIntern(loc, true), uf(n_uf) {
}

AbstractQoreNode* LocalFunctionCallReferenceNode::evalImpl(ExceptionSink* xsink) const {
   return new FunctionCallReferenceNode(loc, uf, ::getProgram());
}

AbstractQoreNode* LocalFunctionCallReferenceNode::evalImpl(bool& needs_deref, ExceptionSink* xsink) const {
   needs_deref = true;
   return new FunctionCallReferenceNode(loc, uf, ::getProgram());
}

QoreValue LocalFunctionCallReferenceNode::execValue(const QoreListNode* args, ExceptionSink* xsink) const {
   return uf->evalFunction(0, args, 0, xsink);
}

bool LocalFunctionCallReferenceNode::is_equal_hard(const AbstractQoreNode* v, ExceptionSink* xsink) const {
   const LocalFunctionCallReferenceNode* vc = dynamic_cast<const LocalFunctionCallReferenceNode*>(v);
   //printd(5, "LocalFunctionCallReferenceNode::is_equal_hard() %p == %p (%p %s)\n", uf, vc ? vc->uf : 0, v, v ? v->getTypeName() : "n/a");
   return vc && uf == vc->uf;
}

bool FunctionCallReferenceNode::derefImpl(ExceptionSink* xsink) {
   //printd(5, "FunctionCallReferenceNode::deref() this: %p pgm: %p refs: %d -> %d\n", this, pgm, reference_count(), reference_count() - 1);
   pgm->depDeref();
   return true;
}

QoreValue FunctionCallReferenceNode::execValue(const QoreListNode* args, ExceptionSink* xsink) const {
   return uf->evalFunction(0, args, pgm, xsink);
}

ResolvedCallReferenceNode::ResolvedCallReferenceNode(bool n_needs_eval, qore_type_t n_type) : AbstractCallReferenceNode(n_needs_eval, n_type) {
}

QoreProgram* ResolvedCallReferenceNode::getProgram() const {
   return 0;
}
