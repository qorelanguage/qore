/*
    FunctionCallNode.cpp

    Qore Programming Language

    Copyright (C) 2003 - 2020 Qore Technologies, s.r.o.

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
#include "qore/intern/QoreClassIntern.h"
#include "qore/intern/QoreNamespaceIntern.h"
#include "qore/intern/qore_program_private.h"

#include <vector>

// eval method against an object where the assumed qoreclass and method were saved at parse time
QoreValue AbstractMethodCallNode::exec(QoreObject* o, const char* c_str, const qore_class_private* ctx, ExceptionSink* xsink) const {
    /* the class and method saved at parse time are used here for this run-time
        optimization: the method pointer saved at parse time is used to execute the
        method directly if the object used at run-time is of the same class as
        either the method or the parse-time class.  Actually any class between the
        parse-time class and the method's class could be used, however I'd have to
        check and make sure that search would be quicker than the quick check
        implemented below on average
    */
    if (qc && method && (o->getClass() == qc || o->getClass() == method->getClass())) {
        //printd(5, "AbstractMethodCallNode::exec() using parse info for %s::%s() qc: %s (o: %s)\n", method->getClassName(), method->getName(), qc->getName(), o->getClass()->getName());
        assert(method);
        if (!o->isValid()) {
            if (variant)
                xsink->raiseException("OBJECT-ALREADY-DELETED", "cannot call %s::%s(%s) on an object that has already been deleted", qc->getName(), method->getName(), variant->getSignature()->getSignatureText());
            else
                xsink->raiseException("OBJECT-ALREADY-DELETED", "cannot call %s::%s() on an object that has already been deleted", qc->getName(), method->getName());
            return QoreValue();
        }

        return variant
            ? qore_method_private::evalNormalVariant(*method, xsink, o, reinterpret_cast<const QoreExternalMethodVariant*>(variant), args)
            : qore_method_private::eval(*method, xsink, o, args, ctx);
    }
    //printd(5, "AbstractMethodCallNode::exec() calling QoreObject::evalMethod() for %s::%s()\n", o->getClassName(), c_str);
    return qore_class_private::get(*o->getClass())->evalMethod(o, c_str, args, ctx, xsink);
}

const QoreTypeInfo* AbstractMethodCallNode::getTypeInfo() const {
    return variant
        ? variant->parseGetReturnTypeInfo()
        : (method
            ? qore_method_private::get(*method)->getFunction()->parseGetUniqueReturnTypeInfo()
            : nullptr);
}

static void invalid_access(const QoreProgramLocation* loc, QoreFunction* func) {
   // func will always be non-zero with builtin functions
   const char* class_name = func->className();
   parse_error(*loc, "parse options do not allow access to builtin %s '%s%s%s()'", class_name ? "method" : "function", class_name ? class_name : "", class_name ? "::" : "", func->getName());
}

static void warn_retval_ignored(const QoreProgramLocation* loc, QoreFunction* func, bool is_bg_call = false) {
    const char* class_name = func->className();
    qore_program_private::makeParseWarning(
        getProgram(),
        *loc,
        QP_WARN_RETURN_VALUE_IGNORED,
        "RETURN-VALUE-IGNORED",
        "%s %s %s%s%s() does not have any side effects and the return value is ignored; to disable this warning, use '%%disable-warning return-value-ignored' in your code",
            is_bg_call ? "background call to" : "call to",
            class_name ? "method" : "function",
            class_name ? class_name : "",
            class_name ? "::" : "",
            func->getName());
}

static void warn_only_may_throw_and_retval_ignored(const QoreProgramLocation* loc, QoreFunction* func, bool is_bg_call = false) {
    const char* class_name = func->className();
    qore_program_private::makeParseWarning(
        getProgram(),
        *loc,
        QP_WARN_RETURN_VALUE_IGNORED,
        "RETURN-VALUE-IGNORED",
        "%s %s %s%s%s() does not have any side effects except that it may throw an exception and the return value is ignored; to disable this warning, use '%%disable-warning return-value-ignored' in your code",
            is_bg_call ? "background call to" : "call to",
            class_name ? "method" : "function",
            class_name ? class_name : "",
            class_name ? "::" : "",
            func->getName());
}

static void warn_deprecated(const QoreProgramLocation* loc, QoreFunction* func) {
   const char* class_name = func->className();
   qore_program_private::makeParseWarning(getProgram(), *loc, QP_WARN_DEPRECATED, "DEPRECATED", "call to deprecated %s %s%s%s(); to disable this warning, use '%%disable-warning deprecated' in your code", class_name ? "method" : "function", class_name ? class_name : "", class_name ? "::" : "", func->getName());
}

static void check_flags(const QoreProgramLocation* loc, QoreFunction* func, int64 flags, int64 pflag) {
    if (pflag & (PF_RETURN_VALUE_IGNORED | PF_BACKGROUND)) {
        bool is_bg_call = (pflag & PF_BACKGROUND);
        if ((flags & QCF_CONSTANT) == QCF_CONSTANT) {
            warn_retval_ignored(loc, func, is_bg_call);
        } else if (flags & QCF_RET_VALUE_ONLY && (pflag & PF_RETURN_VALUE_IGNORED)) {
            warn_only_may_throw_and_retval_ignored(loc, func, is_bg_call);
        }
    }
    if (flags & QCF_DEPRECATED) {
        warn_deprecated(loc, func);
    }
}

int FunctionCallBase::parseArgsVariant(const QoreProgramLocation* loc, LocalVar* oflag, int pflag, QoreFunction* func, qore_ns_private* ns, const QoreTypeInfo*& returnTypeInfo) {
    // number of local variables declared in arguments
    int lvids = 0;

    // number of arguments in call
    unsigned num_args = parse_args ? parse_args->size() : 0;

    // argument type list
    type_vec_t argTypeInfo;

    // initialize arguments and setup argument type list (argTypeInfo)
    if (num_args) {
        // issue #2993: do not initialize args with the "return value ignored" parse flag set
        lvids += parse_args->initArgs(oflag, pflag & ~(PF_RETURN_VALUE_IGNORED | PF_BACKGROUND), argTypeInfo, args);
        parse_args = nullptr;
    }

    //printd(5, "FunctionCallBase::parseArgsVariant() this: %p args: %p '%s'\n", this, args, args ? get_full_type_name(args) : "n/a");

    // resolves pending signatures unconditionally
    if (func) {
        func->resolvePendingSignatures();

        // initialize function or class immediately for possible error messages later (also in case of constant expressions for immediate evaluation)
        const QoreClass* qc = func->getClass();
        if (qc)
            qore_class_private::parseInit(*const_cast<QoreClass*>(qc));
        else
            func->parseInit(ns);

        const qore_class_private* class_ctx = qc ? parse_get_class_priv() : nullptr;
        if (class_ctx && !qore_class_private::parseCheckPrivateClassAccess(*qc, class_ctx))
            class_ctx = nullptr;

        // find variant
        variant = func->parseFindVariant(loc, argTypeInfo, class_ctx);

        QoreProgram* pgm = getProgram();

        //printd(5, "FunctionCallBase::parseArgsVariant() this: %p (%s::)%s ign: %d func: %p variant: %p rt: %s\n", this, func->className() ? func->className() : "", func->getName(), pflag & PF_RETURN_VALUE_IGNORED, func, variant, QoreTypeInfo::getName(func->parseGetUniqueReturnTypeInfo()));

        if (variant) {
            //printd(5, "FunctionCallBase::parseArgsVariant() this: %p (%s::)%s variant: %p f: %lld (%lld) (%lld) rt: %s\n", this, func->className() ? func->className() : "", func->getName(), variant, variant->getFunctionality(), variant->getFlags(), variant->getFlags() & QCF_RET_VALUE_ONLY, QoreTypeInfo::getName(variant->parseGetReturnTypeInfo()));
            if (qc) {
                assert(dynamic_cast<const MethodVariantBase*>(variant));
                const MethodVariantBase* mv = reinterpret_cast<const MethodVariantBase*>(variant);
                if (mv->isAbstract()) {
                    //printd(5, "FunctionCallBase::parseArgsVariant() found abstract %s::%s\n", qc->getName(), func->getName());
                    // issue #3387: set return type before clearing variant
                    returnTypeInfo = mv->parseGetReturnTypeInfo();
                    variant = nullptr;
                    func = nullptr;
                    return lvids;
                } else if (mv->isPrivate() && !qore_class_private::parseCheckPrivateClassAccess(*qc))
                    parse_error(*loc, "illegal call to private method variant %s::%s(%s)", qc->getName(), func->getName(), variant->getSignature()->getSignatureText());
            }
            if (variant) {
                int64 dflags = variant->getFunctionality();
                //printd(5, "FunctionCallBase::parseArgsVariant() this: %p (%s::)%s variant: %p dflags: " QLLD " fdflags: " QLLD "\n", this, func->className() ? func->className() : "", func->getName(), variant, dflags, func->parseGetUniqueFunctionality());
                if (dflags && qore_program_private::parseAddDomain(pgm, dflags))
                    invalid_access(loc, func);
                int64 flags = variant->getFlags();
                check_flags(loc, func, flags, pflag);
            }
        } else {
            //printd(5, "FunctionCallBase::parseArgsVariant() this: %p func: %p f: %lld (%lld) c: %lld (%lld)\n", this, func, func->parseGetUniqueFunctionality(), func->parseGetUniqueFunctionality() & parse_get_parse_options(), func->parseGetUniqueFlags(), func->parseGetUniqueFlags() & QCF_RET_VALUE_ONLY);

            int64 dflags = func->parseGetUniqueFunctionality();
            if (dflags && qore_program_private::parseAddDomain(pgm, dflags))
                invalid_access(loc, func);
            check_flags(loc, func, func->parseGetUniqueFlags(), pflag);
        }

        returnTypeInfo = variant ? variant->parseGetReturnTypeInfo() : func->parseGetUniqueReturnTypeInfo();

        //printd(5, "FunctionCallBase::parseArgsVariant() this: %p func: %s variant: %p pflag: %d pe: %d\n", this, func ? func->getName() : "n/a", variant, pflag, func ? func->empty() : -1);

        // if the function call is being made as a part of a constant expression and
        // there are uncommitted user variants in the function, then raise an error
        if ((pflag & PF_CONST_EXPRESSION) && !variant && !func->pendingEmpty()) {
            const char* name = func->getName();
            const char* cname = func->className();
            QoreStringNode* desc = new QoreStringNode("cannot ");
            if (cname && !strcmp(name, "constructor"))
                desc->sprintf("instantiate class %s", cname);
            else
                desc->sprintf("cannot call %s%s%s()", cname ? cname : "", cname ? "::" : "", name);

            desc->concat(" in an expression initializing a constant value at parse time when the function has uncommitted variants and the variant cannot be matched at parse time; to fix this error, add enough type information to the call to allow the variant to be resolved");

            parseException(*loc, "ILLEGAL-CALL", desc);
        }
    } else {
        returnTypeInfo = nullptr;
    }

    //printd(5, "FunctionCallBase::parseArgsVariant() this: %p func: %s variant: %p args: %p (%zd)\n", this, func ? func->getName() : "n/a", variant, args, args ? args->size() : 0);

    return lvids;
}

QoreValue SelfFunctionCallNode::evalImpl(bool& needs_deref, ExceptionSink* xsink) const {
    QoreObject* self = runtime_get_stack_object();
    assert(self);

    //printd(5, "SelfFunctionCallNode::evalImpl() this: %p self: %p method: %p (%s) v: %d\n", this, self, method, ns.ostr, self->isValid());
    if (is_copy) {
        return self->getClass()->execCopy(self, xsink);
    }

    if (ns.size() == 1) {
        // must have a class context here
        assert(class_ctx || runtime_get_class());
        return exec(self, ns.ostr, class_ctx ? class_ctx : runtime_get_class(), xsink);
    }

    assert(method);

    return tmp_args
        ? qore_method_private::evalTmpArgs(*method, xsink, self, args)
        : qore_method_private::eval(*method, xsink, self, args);
}

void SelfFunctionCallNode::parseInitCall(QoreValue& val, LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& returnTypeInfo) {
    assert(!returnTypeInfo);
    // issue #3637: qc might be non-null while method is null in case of calls to implicit copy() methods, for example
    lvids += parseArgs(oflag, pflag, method ? qore_method_private::get(*method)->getFunction() : nullptr, nullptr, returnTypeInfo);
    // issue #2380 make sure to set the method correctly if resolved from a hierarchy
    if (variant)
        method = static_cast<const MethodVariantBase*>(variant)->method();
    if (method) {
        printd(5, "SelfFunctionCallNode::parseInitCall() this: %p resolved '%s' to %p\n", this, method->getName(), method);
    }
}

// called at parse time
void SelfFunctionCallNode::parseInitImpl(QoreValue& val, LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& returnTypeInfo) {
    assert(!returnTypeInfo);
    if (!oflag) {
        parse_error(*loc, "cannot call method '%s' outside of class code", getName());
        return;
    }

    class_ctx = qore_class_private::get(*QoreTypeInfo::getUniqueReturnClass(oflag->getTypeInfo()));

    if (!method) {
        printd(5, "SelfFunctionCallNode::parseInitImpl() this: %p resolving base class call '%s'\n", this, ns.ostr);

        // copy method calls will be recognized by name = 0
        if (ns.size() == 1) {
            if (!strcmp(ns.ostr, "copy")) {
                printd(5, "SelfFunctionCallNode::parseInitImpl() this: %p resolved to copy constructor\n", this);
                is_copy = true;
                if (args)
                    parse_error(*loc, "no arguments may be passed to copy methods (%lu argument%s given in " \
                        "call to %s::copy())", args->size(), args->size() == 1 ? "" : "s", class_ctx->name.c_str());
            } else {
                assert(!qc || qore_class_private::get(*qc) == class_ctx);
                // raises a parse exception if it fails
                method = const_cast<qore_class_private*>(class_ctx)->parseResolveSelfMethod(loc, ns.ostr, class_ctx);
                if (!method)
                    return;
            }
        } else {
            assert(!qc);
            // possible only if old-style is in effect
            qc = qore_root_ns_private::parseFindScopedClassWithMethod(loc, ns, true);
            // parse exception raised if !qc
            if (!qc)
                return;
            // raises a parse exception if it fails
            method = const_cast<qore_class_private*>(qore_class_private::get(*qc))->parseResolveSelfMethod(loc, ns.getIdentifier(), class_ctx);
            if (!method)
                return;
        }

        // issue #3070: make sure that abstract method calls are resolved at runtime
        if (method && qore_method_private::get(*method)->isAbstract()) {
            method = nullptr;
        }
    }

    // by here, if there are no errors, the class has been initialized
    parseInitCall(val, oflag, pflag, lvids, returnTypeInfo);
}

int SelfFunctionCallNode::getAsString(QoreString& str, int foff, ExceptionSink* xsink) const {
    str.sprintf("in-object method call (%p) to %s::%s()", this, method->getClass()->getName(), method->getName());
    return 0;
}

// if del is true, then the returned QoreString*  should be deleted, if false, then it must not be
QoreString* SelfFunctionCallNode::getAsString(bool& del, int foff, ExceptionSink* xsink) const {
    del = true;
    QoreString* rv = new QoreString;
    getAsString(*rv, foff, xsink);
    return rv;
}

AbstractQoreNode* SelfFunctionCallNode::makeReferenceNodeAndDeref() {
    AbstractQoreNode* rv;
    if (ns.size() == 1)
        rv = new ParseSelfMethodReferenceNode(loc, ns.takeName());
    else
        rv = new ParseScopedSelfMethodReferenceNode(loc, ns.copy());
    deref();
    return rv;
}

/* get string representation (for %n and %N), foff is for multi-line formatting offset, -1 = no line breaks
   the ExceptionSink is only needed for QoreObject where a method may be executed
   use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using these functions directly
   returns -1 for exception raised, 0 = OK
*/
int FunctionCallNode::getAsString(QoreString& str, int foff, ExceptionSink* xsink) const {
    str.sprintf("function call to '%s()' (%p)", getName(), this);
    return 0;
}

// if del is true, then the returned QoreString*  should be deleted, if false, then it must not be
QoreString* FunctionCallNode::getAsString(bool& del, int foff, ExceptionSink* xsink) const {
    del = true;
    QoreString* rv = new QoreString;
    getAsString(*rv, foff, xsink);
    return rv;
}

// eval(): return value requires a deref(xsink)
QoreValue FunctionCallNode::evalImpl(bool& needs_deref, ExceptionSink* xsink) const {
    QoreFunction* func = fe->getFunction();
    //printd(5, "FunctionCallNode::evalImpl() this: %p '%s' tmp_args: %d args: %p '%s' (%zd)\n", this, func->getName(), tmp_args, args, args ? get_full_type_name(args) : "n/a", args ? args->size() : 0);
    return tmp_args
        ? func->evalFunctionTmpArgs(variant, args, pgm, xsink)
        : func->evalFunction(variant, args, pgm, xsink);
}

void FunctionCallNode::parseInitImpl(QoreValue& val, LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& returnTypeInfo) {
    assert(!returnTypeInfo);
    if (fe)
        return;
    //assert(!func);
    assert(c_str);

    bool abr = parse_check_parse_option(PO_ALLOW_BARE_REFS);

    // try to resolve bare reference if allowed
    if (abr) {
        // check for a local variable with the same name
        bool in_closure;
        LocalVar* id = find_local_var(c_str, in_closure);
        if (id) {
            VarRefNode* vrn = new VarRefNode(loc, takeName(), id, in_closure);
            val = new CallReferenceCallNode(loc, vrn, takeParseArgs());
            deref();
            parse_init_value(val, oflag, pflag, lvids, returnTypeInfo);
            return;
        }
    }

    // try to resolve a method call if we are parsing in an object context
    if (oflag) {
        const QoreClass* qc = QoreTypeInfo::getUniqueReturnClass(oflag->getTypeInfo());

        QoreValue n;
        if (abr && !qore_class_private::parseResolveInternalMemberAccess(qc, c_str, returnTypeInfo)) {
            n = new SelfVarrefNode(loc, takeName());
        } else {
            bool found;
            n = qore_class_private::parseFindConstantValue(const_cast<QoreClass*>(qc), c_str, returnTypeInfo, found, qore_class_private::get(*qc));
            if (found) {
                n.ref();
                //printd(5, "FunctionCallNode::parseInitImpl() this: %p n: %p (%d -> %d)\n", this, n, n->reference_count(), n->reference_count() + 1);
            } else {
                // check for class static var reference
                const QoreClass* oqc = nullptr;
                ClassAccess access;
                QoreVarInfo *vi = qore_class_private::parseFindStaticVar(qc, c_str, oqc, access);
                if (vi) {
                    assert(qc);
                    returnTypeInfo = vi->getTypeInfo();
                    n = new StaticClassVarRefNode(loc, c_str, *oqc, *vi);
                }
            }
        }

        if (!n.isNothing()) {
            val = new CallReferenceCallNode(loc, n, takeParseArgs());
            deref();
            parse_init_value(val, oflag, pflag, lvids, returnTypeInfo);
            return;
        }

        if (abr) {
            SelfFunctionCallNode* sfcn = nullptr;
            if (!strcmp(c_str, "copy")) {
                if (args) {
                    parse_error(*loc, "no arguments may be passed to copy methods (%lu argument%s given in " \
                        "call to %s::copy())", args->size(), args->size() == 1 ? "" : "s", qc->getName());
                    return;
                }
                sfcn = new SelfFunctionCallNode(loc, takeName(), 0);
            } else {
                const QoreMethod *m = qore_class_private::parseFindSelfMethod(const_cast<QoreClass*>(qc), c_str);
                if (m) {
                    if (!m->isStatic()) {
                        sfcn = new SelfFunctionCallNode(loc, takeName(), takeParseArgs(), m, qc, qore_class_private::get(*qc));
                    } else {
                        val = new StaticMethodCallNode(loc, m, takeParseArgs());
                        deref();
                        parse_init_value(val, oflag, pflag, lvids, returnTypeInfo);
                        return;
                    }
                }
            }
            if (sfcn) {
                val = sfcn;
                deref();
                sfcn->parseInitCall(val, oflag, pflag, lvids, returnTypeInfo);
                return;
            }
        }
    }

    parseInitCall(val, oflag, pflag, lvids, returnTypeInfo);
}

void FunctionCallNode::parseInitCall(QoreValue& val, LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& returnTypeInfo) {
    assert(!fe);
    assert(c_str);
    assert(!returnTypeInfo);

    bool abr = parse_check_parse_option(PO_ALLOW_BARE_REFS);

    QoreValue n;

    // try to resolve a global var
    if (abr) {
        Var* v = qore_root_ns_private::parseFindGlobalVar(c_str);
        if (v)
            n = new GlobalVarRefNode(loc, takeName(), v);
    }

    bool found = !n.isNothing();

    // see if a constant can be resolved
    if (!found) {
        n = qore_root_ns_private::parseFindConstantValue(loc, c_str, returnTypeInfo, found, false);
        if (found) {
            n.ref();
        }
    }

    if (found) {
        val = new CallReferenceCallNode(loc, n, takeParseArgs());
        deref();
        parse_init_value(val, oflag, pflag, lvids, returnTypeInfo);
        return;
    }

    // resolves the function
    fe = qore_root_ns_private::parseResolveFunctionEntry(loc, c_str);
    free(c_str);
    c_str = nullptr;

    if (fe) {
        parseInitFinalizedCall(val, oflag, pflag, lvids, returnTypeInfo);
    }
}

void FunctionCallNode::parseInitFinalizedCall(QoreValue& val, LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& returnTypeInfo) {
    assert(!returnTypeInfo);
    assert(fe);
    lvids += parseArgs(oflag, pflag, fe->getFunction(), fe->getNamespace(), returnTypeInfo);
}

AbstractQoreNode* FunctionCallNode::makeReferenceNodeAndDerefImpl() {
    return new UnresolvedCallReferenceNode(loc, takeName());
}

AbstractQoreNode* ProgramFunctionCallNode::makeReferenceNodeAndDerefImpl() {
    return new UnresolvedProgramCallReferenceNode(loc, takeName());
}

void ScopedObjectCallNode::parseInitImpl(QoreValue& val, LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo) {
    assert(!typeInfo);
    if (name) {
        assert(!oc);
        // find object class
        if ((oc = qore_root_ns_private::parseFindScopedClass(loc, *name))) {
            // check if parse options allow access to this class
            int64 cflags = oc->getDomain();
            if (cflags && qore_program_private::parseAddDomain(getProgram(), cflags))
                parseException(*loc, "ILLEGAL-CLASS-INSTANTIATION", "parse options do not allow access to the '%s' class", oc->getName());
            // check if the class has pending changes and is used in a constant initialization expression
            if (pflag & PF_CONST_EXPRESSION && qore_class_private::parseHasPendingChanges(*oc))
                parseException(*loc, "ILLEGAL-CLASS-INSTANTIATION", "cannot instantiate '%s' class for assignment in a constant expression in the parse initialization phase when the class has uncommitted changes", oc->getName());
        }
        delete name;
        name = nullptr;
    }
#ifdef DEBUG
    else assert(oc);
#endif

    const QoreMethod* constructor = oc ? oc->parseGetConstructor() : nullptr;
    lvids += parseArgs(oflag, pflag, constructor ? qore_method_private::get(*constructor)->getFunction() : nullptr, nullptr, typeInfo);

    if (oc) {
        // parse init the class and check if we're trying to instantiate an abstract class
        qore_class_private::get(*const_cast<QoreClass*>(oc))->parseCheckAbstractNew(loc);

        // initialize class immediately, in case the class will be instantiated immediately after during parsing
        // to be assigned to a constant
        //qore_class_private::parseInit(*const_cast<QoreClass*>(oc));

        typeInfo = oc->getTypeInfo();
        desc.sprintf("new %s", oc->getName());
    }
    else
        typeInfo = nullptr;

    //printd(5, "ScopedObjectCallNode::parseInitImpl() this: %p constructor: %p variant: %p\n", this, constructor, variant);

    if (((constructor && (qore_method_private::getAccess(*constructor) > Public)) || (variant && CONMV_const(variant)->isPrivate())) && !qore_class_private::parseCheckPrivateClassAccess(*oc)) {
        if (variant)
            parse_error(*loc, "illegal external access to private constructor %s::constructor(%s)", oc->getName(), variant->getSignature()->getSignatureText());
        else
            parse_error(*loc, "illegal external access to private constructor of class %s", oc->getName());
    }

    //printd(5, "ScopedObjectCallNode::parseInitImpl() this: %p class: %s (%p) constructor: %p function: %p variant: %p\n", this, oc->getName(), oc, constructor, constructor ? qore_method_private::get(*constructor)->getFunction() : 0, variant);
}

QoreValue ScopedObjectCallNode::evalImpl(bool& needs_deref, ExceptionSink* xsink) const {
    return qore_class_private::execConstructor(*oc, variant, args, xsink);
}

QoreValue MethodCallNode::exec(QoreObject* o, ExceptionSink* xsink) const {
    // issue #3596: do not use the context class if it's not compatible with "o"
    const qore_class_private* class_ctx = runtime_get_class();
    if (class_ctx && !qore_class_private::parseCheckPrivateClassAccess(*o->getClass(), class_ctx)) {
        class_ctx = nullptr;
    }
    return AbstractMethodCallNode::exec(o, c_str, class_ctx, xsink);
}

QoreValue MethodCallNode::execPseudo(const QoreValue n, ExceptionSink* xsink) const {
   //printd(5, "MethodCallNode::execPseudo() %s::%s() variant: %p\n", qc->getName(), method->getName(), variant);
   // if n is nothing make sure and use the "<nothing>" class with a dynamic method lookup
   if (n.isNothing() && qc != QC_PSEUDONOTHING)
      return qore_class_private::evalPseudoMethod(QC_PSEUDONOTHING, n, method->getName(), args, xsink);
   else
      return qore_class_private::evalPseudoMethod(qc, method, variant, n, args, xsink);
}

AbstractQoreNode* StaticMethodCallNode::makeReferenceNodeAndDeref() {
   if (args) {
      parse_error(*loc, "argument given to static method call reference");
      return this;
   }

   UnresolvedStaticMethodCallReferenceNode* rv = new UnresolvedStaticMethodCallReferenceNode(loc, takeScope());
   deref();
   return rv;
}

void StaticMethodCallNode::parseInitImpl(QoreValue& val, LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo) {
    if (!method) {
        assert(!typeInfo);
        bool abr = parse_check_parse_option(PO_ALLOW_BARE_REFS);

        QoreClass* qc = qore_root_ns_private::parseFindScopedClassWithMethod(loc, *scope, false);

        const QoreClass* pc = oflag && abr ? QoreTypeInfo::getUniqueReturnClass(oflag->getTypeInfo()) : nullptr;

        qore_class_private* class_ctx = oflag
            ? qore_class_private::get(*const_cast<QoreClass*>(pc ? pc : QoreTypeInfo::getUniqueReturnClass(oflag->getTypeInfo())))
            : parse_get_class_priv();

        // see if this is a call to a base class method if bare refs are allowed
        // and we're parsing in a class context and the class found is in the
        // current class parse context
        if (qc) {
            if (class_ctx && !qore_class_private::parseCheckPrivateClassAccess(*qc, class_ctx))
                class_ctx = nullptr;
            if (pc && class_ctx) {
                // checks access already
                method = qore_class_private::get(*qc)->parseFindAnyMethodStaticFirst(scope->getIdentifier(), class_ctx);
                if (method && !method->isStatic() && !strcmp(method->getName(), "copy")) {
                    parseException(*loc, "INVALID-METHOD", "cannot explicitly call base class %s::%s() copy method", qc->getName(), scope->getIdentifier());
                    return;
                }
                //printd(5, "StaticMethodCallNode::parseInitImpl() '%s' pc: %s qc: %smethod: %p\n", scope->ostr, pc->getName(), qc->getName(), method);
            }
            else
                method = qore_class_private::get(*qc)->parseFindStaticMethod(scope->getIdentifier(), class_ctx);
        }

        //printd(5, "StaticMethodCallNode::parseInitImpl() %s qc: %p '%s' method: %p '%s()'\n", scope->ostr, qc, qc ? qc->getName() : "n/a", method, scope->getIdentifier());

        // see if a constant can be resolved
        if (!method) {
            {
                // see if this is a function call to a function defined in a namespace
                const FunctionEntry* f = qore_root_ns_private::parseResolveFunctionEntry(*scope);
                if (f) {
                    FunctionCallNode* fcn = new FunctionCallNode(loc, f, takeParseArgs());
                    val = fcn;
                    deref();
                    fcn->parseInitFinalizedCall(val, oflag, pflag, lvids, typeInfo);
                    return;
                }
            }

            /*
            ValueHolder n(nullptr);

            if (abr) {
                Var* v = qore_root_ns_private::parseFindGlobalVar(*scope);
                if (v)
                    n = new GlobalVarRefNode(loc, strdup(scope->getIdentifier()), v);
            }

            bool found = false;
            if (n->isNothing()) {
                n = qore_root_ns_private::parseFindReferencedConstantValue(loc, *scope, typeInfo, found, false);
            }
            */

            bool found = false;
            QoreValue n = qore_root_ns_private::parseFindReferencedConstantValue(loc, *scope, typeInfo, found, false);

            if (found) {
                val = new CallReferenceCallNode(loc, n, takeParseArgs());
                deref();
                parse_init_value(val, oflag, pflag, lvids, typeInfo);
                return;
            }
            else {
                assert(!n);
            }

            parse_error(*loc, "cannot resolve call '%s()' to any reachable and callable object", scope->ostr);
            return;
        }

        // check class capabilities against parse options
        if (qore_program_private::parseAddDomain(getProgram(), qc->getDomain())) {
            parseException(*loc, "INVALID-METHOD", "class '%s' implements capabilities that are not allowed by current parse options", qc->getName());
            return;
        }

        if (!method->isStatic()) {
            SelfFunctionCallNode* sfcn = new SelfFunctionCallNode(loc, scope->takeName(), takeParseArgs(), method, qc, class_ctx);
            val = sfcn;
            deref();
            parse_init_value(val, oflag, pflag, lvids, typeInfo);
            return;
        }

        delete scope;
        scope = nullptr;

        /*
        // need to get the current contextual class when parsing in case we're in a static method for example
        if (!pc)
            pc = parse_get_class();
        */
    }
    else {
        assert(!scope);
        // check class capabilities against parse options
        if (qore_program_private::parseAddDomain(getProgram(), method->getClass()->getDomain())) {
            parseException(*loc, "INVALID-METHOD", "class '%s' implements capabilities that are not allowed by current parse options", method->getClass()->getName());
        }
    }

    assert(method->isStatic());

    lvids += parseArgs(oflag, pflag, qore_method_private::get(*method)->getFunction(), nullptr, typeInfo);
    // issue #2380 make sure to set the method correctly if resolved from a hierarchy
    if (variant)
        method = static_cast<const MethodVariantBase*>(variant)->method();
}

QoreValue StaticMethodCallNode::evalImpl(bool& needs_deref, ExceptionSink* xsink) const {
    // FIXME: implement rv as QoreValue
    return tmp_args
        ? qore_method_private::evalTmpArgs(*method, xsink, nullptr, args)
        : qore_method_private::eval(*method, xsink, nullptr, args);
}

const QoreTypeInfo* StaticMethodCallNode::getTypeInfo() const {
    return variant
        ? variant->parseGetReturnTypeInfo()
        : (method
            ? qore_method_private::get(*method)->getFunction()->parseGetUniqueReturnTypeInfo()
            : 0);
}
