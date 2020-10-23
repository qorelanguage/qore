/* -*- indent-tabs-mode: nil -*- */
/*
    Function.cpp

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
#include "qore/intern/qore_program_private.h"
#include "qore/intern/qore_list_private.h"
#include "qore/intern/QoreParseListNode.h"
#include "qore/intern/StatementBlock.h"
#include "qore/intern/QoreListNodeEvalOptionalRefHolder.h"

#include <cassert>
#include <cctype>
#include <cmath>
#include <cstdio>

static void duplicateSignatureException(const char* cname, const char* name, const UserSignature* sig) {
    parseException(*sig->getParseLocation(), "DUPLICATE-SIGNATURE", "%s%s%s(%s) has already been declared", cname ? cname : "", cname ? "::" : "", name, sig->getSignatureText());
}

static void ambiguousDuplicateSignatureException(const char* cname, const char* name, const AbstractFunctionSignature* sig1, const UserSignature* sig2) {
    parseException(*sig2->getParseLocation(), "DUPLICATE-SIGNATURE", "%s%s%s(%s) matches already declared variant %s(%s)", cname ? cname : "", cname ? "::" : "", name, sig2->getSignatureText(), name, sig1->getSignatureText());
}

QoreFunction* IList::getFunction(const qore_class_private* class_ctx, const qore_class_private*& last_class,
    const_iterator aqfi, bool& internal_access, bool& stop) const {
    stop = internal_access && (*aqfi).access == Internal;

    QoreFunction* rv = (!last_class || ((*aqfi).access == Public) || stop
                        || (class_ctx && (*aqfi).access == Private)) ? (*aqfi).func : nullptr;

    if (rv) {
        const QoreClass* fc = rv->getClass();
        if (fc) {
            // get the function's class
            last_class = qore_class_private::get(*fc);
            if (last_class && class_ctx) {
                // set the internal access flag
                internal_access = last_class->equal(*class_ctx);
            }
        }
    }

    return rv;
}

bool AbstractFunctionSignature::operator==(const AbstractFunctionSignature& sig) const {
    if (num_param_types != sig.num_param_types || min_param_types != sig.min_param_types) {
        //printd(5, "AbstractFunctionSignature::operator==() pt: %d != %d || mpt %d != %d\n", num_param_types, sig.num_param_types, min_param_types, sig.min_param_types);
        return false;
    }

    // return types for abstract methods must be compatible if present
    if (sig.returnTypeInfo != nothingTypeInfo) {
        bool may_not_match = false;
        if (!QoreTypeInfo::parseAccepts(sig.returnTypeInfo, returnTypeInfo, may_not_match) || may_not_match) {
            //printd(5, "AbstractFunctionSignature::operator==() rt: %s is not compatible with %s (%p %p)\n", QoreTypeInfo::getName(returnTypeInfo), QoreTypeInfo::getName(sig.returnTypeInfo), returnTypeInfo, sig.returnTypeInfo);
            return false;
        }
    }

    for (unsigned i = 0; i < typeList.size(); ++i) {
        const QoreTypeInfo* ti = sig.typeList.size() <= i
            ? nullptr
            : sig.typeList[i];
        bool match;
        match = (QoreTypeInfo::runtimeTypeMatch(typeList[i], ti) >= QTI_NEAR);

        //printd(5, "AbstractFunctionSignature::operator==() param %d (%s =~ %s) %d\n", i, QoreTypeInfo::getName(typeList[i]), QoreTypeInfo::getName(ti), QoreTypeInfo::runtimeTypeMatch(typeList[i], ti));

        if (!match) {
            //printd(5, "AbstractFunctionSignature::operator==() param %d %s != %s\n", i, QoreTypeInfo::getName(typeList[i]), QoreTypeInfo::getName(sig.typeList[i]));
            return false;
        }
    }

    //printd(5, "AbstractFunctionSignature::operator==() '%s' == '%s' TRUE\n", str.c_str(), sig.str.c_str());
    return true;
}

int64 AbstractQoreFunctionVariant::getParseOptions(int64 po) const {
    return is_user ? getUserVariantBase()->getParseOptions(po) : po;
}

void AbstractQoreFunctionVariant::parseResolveUserSignature() {
    UserVariantBase* uvb = getUserVariantBase();
    if (uvb)
        uvb->getUserSignature()->resolve();
}

bool AbstractQoreFunctionVariant::hasBody() const {
    return is_user ? getUserVariantBase()->hasBody() : true;
}

LocalVar* AbstractQoreFunctionVariant::getSelfId() const {
    const UserVariantBase* uvb = getUserVariantBase();
    if (!uvb) {
        return nullptr;
    }
    return uvb->getUserSignature()->getSelfId();
}

static void do_call_name(QoreString &desc, const QoreFunction* func) {
    const char* class_name = func->className();
    if (class_name)
        desc.sprintf("%s::", class_name);
    desc.sprintf("%s(", func->getName());
}

static void add_args(QoreStringNode &desc, const QoreListNode* args) {
    if (!args || !args->size())
        return;

    for (unsigned i = 0; i < args->size(); ++i) {
        const QoreValue n = args->retrieveEntry(i);
        desc.concat(n.getFullTypeName());
        if (i != (args->size() - 1))
            desc.concat(", ");
    }
}

CodeEvaluationHelper::CodeEvaluationHelper(ExceptionSink* n_xsink, const QoreFunction* func,
    const AbstractQoreFunctionVariant*& variant, const char* n_name, const QoreListNode* args, QoreObject* self,
    const qore_class_private* n_qc, qore_call_t n_ct, bool is_copy, const qore_class_private* cctx)
    : ct(n_ct), name(n_name), xsink(n_xsink), qc(n_qc),
        loc(get_runtime_location()),
        tmp(n_xsink), returnTypeInfo((const QoreTypeInfo*)-1) {
    if (self && !self->isValid()) {
        assert(n_qc);
        xsink->raiseException("OBJECT-ALREADY-DELETED", "cannot call %s::%s() on an object that has already been deleted", qc->name.c_str(), func->getName());
        return;
    }

    setCallName(func);
    tmp.assignEval(args);
    if (*xsink) {
        return;
    }

    init(func, variant, is_copy, cctx, self);
}

CodeEvaluationHelper::CodeEvaluationHelper(ExceptionSink* n_xsink, const QoreFunction* func,
    const AbstractQoreFunctionVariant*& variant, const char* n_name, QoreListNode* args, QoreObject* self,
    const qore_class_private* n_qc, qore_call_t n_ct, bool is_copy, const qore_class_private* cctx)
    : ct(n_ct), name(n_name), xsink(n_xsink), qc(n_qc),
        loc(get_runtime_location()),
        tmp(n_xsink), returnTypeInfo((const QoreTypeInfo*)-1) {
    if (self && !self->isValid()) {
        assert(n_qc);
        xsink->raiseException("OBJECT-ALREADY-DELETED", "cannot call %s::%s() on an object that has already been deleted", qc->name.c_str(), func->getName());
        return;
    }

    setCallName(func);
    tmp.assignEval(args);
    if (*xsink) {
        return;
    }

    init(func, variant, is_copy, cctx, self);
}

CodeEvaluationHelper::~CodeEvaluationHelper() {
    if (restore_stack) {
        if (ct == CT_BUILTIN) {
            update_runtime_stack_location(stack_loc, old_runtime_loc);
        } else {
            update_runtime_stack_location(stack_loc);
        }
    }
    if (returnTypeInfo != (const QoreTypeInfo*)-1) {
        saveReturnTypeInfo(returnTypeInfo);
    }
}

void CodeEvaluationHelper::setCallName(const QoreFunction* func) {
    if (qc) {
        callName = qc->name.c_str();
        callName += "::";
    }
    callName += func->getName();
}

void CodeEvaluationHelper::init(const QoreFunction* func, const AbstractQoreFunctionVariant*& variant, bool is_copy,
    const qore_class_private* cctx, QoreObject* self) {
    printd(5, "CodeEvaluationHelper::init() this: %p '%s()' file: %s line: %d variant: %p cctx: %p (%s)\n", this, func->getName(),
        loc->getFile(), loc->start_line, variant, cctx, cctx ? cctx->name.c_str() : "n/a");

    if (!variant) {
        const qore_class_private* class_ctx = qc ? (cctx ? cctx : runtime_get_class()) : nullptr;
        if (class_ctx && !qore_class_private::runtimeCheckPrivateClassAccess(*qc->cls, class_ctx)) {
            class_ctx = nullptr;
        }

        variant = func->runtimeFindVariant(xsink, getArgs(), false, class_ctx);
        if (!variant) {
            assert(*xsink);
            return;
        }

        // check for accessible variants
        if (qc) {
            const MethodVariant* mv = reinterpret_cast<const MethodVariant*>(variant);
            ClassAccess va = mv->getAccess();
            if ((va > Public && !class_ctx) || (va == Internal && !qore_class_private::get(*mv->getClass())->equal(*qc))) {
                xsink->raiseException("METHOD-IS-PRIVATE", "%s::%s(%s) is not accessible in this context", mv->className(), func->getName(), mv->getSignature()->getSignatureText());
                return;
            }
        }
    }

    if (processDefaultArgs(func, variant, true, is_copy, self)) {
        return;
    }

    setCallType(variant->getCallType());
    setReturnTypeInfo(variant->getReturnTypeInfo());

    // add call to call stack; push builtin location on the stack if executing builtin c++ code
    if (ct == CT_BUILTIN) {
        stack_loc = update_get_runtime_stack_builtin_location(this, stmt, pgm, old_runtime_loc);
    } else {
        stack_loc = update_get_runtime_stack_location(this, stmt, pgm);
    }
    restore_stack = true;
}

int CodeEvaluationHelper::processDefaultArgs(const QoreFunction* func, const AbstractQoreFunctionVariant* variant,
    bool check_args, bool is_copy, QoreObject* self) {
    // get default argument list of variant
    AbstractFunctionSignature* sig = variant->getSignature();
    const arg_vec_t& defaultArgList = sig->getDefaultArgList();
    const type_vec_t& typeList = sig->getTypeList();

    unsigned max = QORE_MAX(defaultArgList.size(), typeList.size());
    for (unsigned i = 0; i < max; ++i) {
        if (i < defaultArgList.size() && defaultArgList[i] && (!tmp || tmp->retrieveEntry(i).isNothing())) {
            QoreValue& p = tmp.getEntryReference(i);

            // issue #3240: set self in case the default arg expression references a member of the current object
            // must be set only for evaluation, cannot be set when verifying types below in
            // QoreTypeInfo::acceptInputParam() as it will cause errors handling references related to the current
            // object - "self" is the object for the call but not necessarily the current "self"
            OptionalObjectOnlySubstitutionHelper self_helper(self);

            p = defaultArgList[i].eval(xsink);
            if (*xsink)
                return -1;

            // process default argument with accepting type's filter if necessary
            const QoreTypeInfo* paramTypeInfo = sig->getParamTypeInfo(i);
            if (QoreTypeInfo::mayRequireFilter(paramTypeInfo, p)) {
                QoreTypeInfo::acceptInputParam(paramTypeInfo, i, sig->getName(i), p, xsink);
                if (*xsink)
                    return -1;
            }
        } else if (i < typeList.size()) {
            QoreValue n;
            if (tmp)
                n = tmp->retrieveEntry(i);

            if (is_copy && !i && n.isNothing())
                continue;

            const QoreTypeInfo* paramTypeInfo = sig->getParamTypeInfo(i);
            if (!paramTypeInfo)
                continue;

            // issue #3184: do not create a NOTHING argument if none is needed
            if (!QoreTypeInfo::hasType(paramTypeInfo)
                || (tmp.size() < i && QoreTypeInfo::parseAcceptsReturns(paramTypeInfo, NT_NOTHING))) {
                continue;
            }
            // test for change or incompatibility
            if (check_args || QoreTypeInfo::mayRequireFilter(paramTypeInfo, n)) {
                QoreValue& p = tmp.getEntryReference(i);
                QoreTypeInfo::acceptInputParam(paramTypeInfo, i, sig->getName(i), p, xsink);
                if (*xsink)
                    return -1;
            }
        }
    }

    // check for excess args exception
    unsigned nargs = tmp.size();
    if (!nargs)
        return 0;
    unsigned nparams = sig->numParams();

    //printd(5, "processDefaultArgs() %s nargs: %d nparams: %d flags: %lld po: %d\n", func->getName(), nargs, nparams, variant->getFlags(), (bool)(getProgram()->getParseOptions64() & (PO_REQUIRE_TYPES | PO_STRICT_ARGS)));
    //if (nargs > nparams && (getProgram()->getParseOptions64() & (PO_REQUIRE_TYPES | PO_STRICT_ARGS))) {
    if (nargs > nparams) {
        // use the target program (if different than the current pgm) to check for argument errors
        const UserVariantBase* uvb = variant->getUserVariantBase();
        int64 po;
        if (uvb)
            po = uvb->pgm->getParseOptions64();
        else
            po = runtime_get_parse_options();

        if (po & (PO_REQUIRE_TYPES | PO_STRICT_ARGS)) {
            int64 flags = variant->getFlags();

            if (!(flags & QCF_USES_EXTRA_ARGS)) {
                for (unsigned i = nparams; i < nargs; ++i) {
                    //printd(5, "processDefaultArgs() %s arg %d nothing: %d\n", func->getName(), i, tmp->retrieveEntry(i).isNothing());
                    if (!tmp->retrieveEntry(i).isNothing()) {
                        QoreStringNode* desc = new QoreStringNode("call to ");
                        do_call_name(*desc, func);
                        if (nparams)
                            desc->concat(sig->getSignatureText());
                        desc->concat(") made as ");
                        do_call_name(*desc, func);
                        add_args(*desc, *tmp);
                        unsigned diff = nargs - nparams;
                        desc->sprintf(") with %d excess argument%s, which is an error when PO_REQUIRE_TYPES or PO_STRICT_ARGS is set", diff, diff == 1 ? "" : "s");
                        xsink->raiseException("CALL-WITH-TYPE-ERRORS", desc);
                        return -1;
                    }
                }
            }
        }
    }

    return 0;
}

void AbstractFunctionSignature::addDefaultArgument(QoreValue arg) {
    assert(arg);
    str.append(" = ");
    qore_type_t t = arg.getType();
    if (t == NT_BAREWORD) {
        str.append(arg.get<const BarewordNode>()->str);
        return;
    }
    if (t == NT_CONSTANT) {
        str.append(arg.get<const ScopedRefNode>()->scoped_ref->getIdentifier());
        return;
    }
    if (!arg.needsEval()) {
        QoreNodeAsStringHelper sh(arg, FMT_NONE, 0);
        str.append(sh->getBuffer());
        return;
    }
    str.append("<exp>");
}

UserSignature::UserSignature(int first_line, int last_line, QoreValue params, RetTypeInfo* retTypeInfo, int64 po) :
    AbstractFunctionSignature(retTypeInfo ? retTypeInfo->getTypeInfo() : nullptr),
    parseReturnTypeInfo(retTypeInfo ? retTypeInfo->takeParseTypeInfo() : nullptr),
    loc(qore_program_private::get(*getProgram())->getLocation(first_line, last_line)),
    lv(0), argvid(0), selfid(0), resolved(false) {

    bool needs_types = (bool)(po & (PO_REQUIRE_TYPES | PO_REQUIRE_PROTOTYPES));
    bool bare_refs = (bool)(po & PO_ALLOW_BARE_REFS);

    // assign no return type if return type declaration is missing and PO_REQUIRE_TYPES or PO_REQUIRE_PROTOTYPES is set
    if (!retTypeInfo && needs_types)
        returnTypeInfo = nothingTypeInfo;
    delete retTypeInfo;

    if (!params) {
        return;
    }

    ValueHolder param_holder(params, nullptr);

    if (params.getType() == NT_VARREF) {
        pushParam(params.get<VarRefNode>(), QoreValue(), needs_types);
        return;
    }

    if (params.getType() == NT_BAREWORD) {
        pushParam(params.get<BarewordNode>(), needs_types, bare_refs);
        return;
    }

    if (params.getType() == NT_OPERATOR) {
        pushParam(params.get<QoreOperatorNode>(), needs_types);
        return;
    }

    if (params.getType() != NT_PARSE_LIST) {
        param_error();
        return;
    }

    QoreParseListNode* l = params.get<QoreParseListNode>();

    parseTypeList.reserve(l->size());
    typeList.reserve(l->size());
    defaultArgList.reserve(l->size());

    for (unsigned i = 0; i < l->size(); ++i) {
        QoreValue n = l->get(i);
        qore_type_t t = n.getType();
        if (t == NT_OPERATOR)
            pushParam(n.get<QoreOperatorNode>(), needs_types);
        else if (t == NT_BAREWORD)
            pushParam(n.get<BarewordNode>(), needs_types, bare_refs);
        else if (t == NT_VARREF)
            pushParam(n.get<VarRefNode>(), QoreValue(), needs_types);
        else {
            if (!n.isNothing())
                param_error();
            break;
        }

        // add a comma to the signature string if it's not the last parameter
        if (i != (l->size() - 1))
            str.append(", ");
    }
}

void UserSignature::pushParam(QoreOperatorNode* t, bool needs_types) {
    QoreAssignmentOperatorNode* op = dynamic_cast<QoreAssignmentOperatorNode*>(t);
    if (!op) {
        parse_error(*loc, "invalid expression with the '%s' operator in parameter list; only simple assignments to default values are allowed", t->getTypeName());
        return;
    }

    QoreValue l = op->getLeft();
    if (l.getType() != NT_VARREF) {
        param_error();
        return;
    }
    VarRefNode* v = l.get<VarRefNode>();
    QoreValue defArg = op->swapRight(0);
    pushParam(v, defArg, needs_types);
}

void UserSignature::pushParam(BarewordNode* b, bool needs_types, bool bare_refs) {
   names.push_back(b->str);
   parseTypeList.push_back(0);
   typeList.push_back(0);
   str.append(NO_TYPE_INFO);
   str.append(" ");
   str.append(b->str);
   defaultArgList.push_back(QoreValue());

   if (needs_types)
      parse_error(*loc, "parameter '%s' declared without type information, but parse options require all declarations to have type information", b->str);

   if (!bare_refs)
      parse_error(*loc, "parameter '%s' declared without '$' prefix, but parse option 'allow-bare-defs' is not set", b->str);
   return;
}

void UserSignature::pushParam(VarRefNode* v, QoreValue defArg, bool needs_types) {
    // check for duplicate name
    for (name_vec_t::iterator i = names.begin(), e = names.end(); i != e; ++i)
        if (*i == v->getName())
            parse_error(*loc, "duplicate variable '%s' declared in parameter list", (*i).c_str());

    names.push_back(v->getName());

    bool is_decl = v->isDecl();
    if (needs_types && !is_decl)
        parse_error(*loc, "parameter '%s' declared without type information, but parse options require all declarations to have type information", v->getName());

    // see if this is a new object call
    if (v->has_effect()) {
        // here we make 4 virtual function calls when 2 would be enough, but no need to optimize for speed for an exception
        parse_error(*loc, "parameter '%s' may not be declared with implicit constructor syntax; instead use: '%s %s = new %s()'", v->getName(), v->parseGetTypeName(), v->getName(), v->parseGetTypeName());
    }

    if (is_decl) {
        VarRefDeclNode* vd = reinterpret_cast<VarRefDeclNode*>(v);
        QoreParseTypeInfo* pti = vd->takeParseTypeInfo();
        parseTypeList.push_back(pti);
        const QoreTypeInfo* ti = vd->getTypeInfo();
        typeList.push_back(ti);

        assert(!(pti && ti));

        if (pti || QoreTypeInfo::hasType(ti)) {
            ++num_param_types;
            // only increment min_param_types if there is no default argument
            if (!defArg)
                ++min_param_types;
        }

        // add type name to signature
        if (pti)
            QoreParseTypeInfo::concatName(pti, str);
        else
            QoreTypeInfo::concatName(ti, str);
    } else {
        parseTypeList.push_back(nullptr);
        typeList.push_back(nullptr);
        str.append(NO_TYPE_INFO);
    }

    str.append(" ");
    str.append(v->getName());

    defaultArgList.push_back(defArg);
    if (defArg)
        addDefaultArgument(defArg);

    if (v->explicitScope()) {
        if (v->getType() == VT_LOCAL)
            parse_error(*loc, "invalid local variable declaration in argument list; by default all variables declared in argument lists are local");
        else if (v->getType() == VT_GLOBAL)
            parse_error(*loc, "invalid global variable declaration in argument list; by default all variables declared in argument lists are local");
    }

    //printd(5, "UserSignature::UserSignature() %p '%s'\n", this, str.c_str());
}

void UserSignature::parseInitPushLocalVars(const QoreTypeInfo* classTypeInfo) {
    lv.reserve(parseTypeList.size());

    if (selfid) {
        push_local_var(selfid, loc);
    } else if (classTypeInfo) {
        selfid = push_local_var("self", loc, classTypeInfo, true, 1);
    }

    // push argv var on stack and save id
    argvid = push_local_var("argv", loc, listOrNothingTypeInfo, true, 1);
    printd(5, "UserSignature::parseInitPushLocalVars() this: %p (%s) argvid: %p selfid: %p\n", this, getSignatureText(), argvid, selfid);

    resolve();

    // init param ids and push local parameter vars on stack
    for (unsigned i = 0; i < typeList.size(); ++i) {
        // check for dups but do not check if the variables are referenced in the block
        // push args declared as type "*reference" as "any"; if no value passed, then they have no type restrictions
        // NOTE that when complex types are supported, the type restriction should be that of the reference's subtype
        lv.push_back(push_local_var(names[i].c_str(), loc,
            typeList[i] == referenceOrNothingTypeInfo ? anyTypeInfo : typeList[i], true, 1));
        printd(5, "UserSignature::parseInitPushLocalVars() registered local var %s (id: %p)\n", names[i].c_str(), lv[i]);
    }
}

void UserSignature::parseInitPopLocalVars() {
    // remove local variables from stack and unset the parse_assigned flag
    for (unsigned i = 0; i < typeList.size(); ++i) {
        pop_local_var(true);
    }

    // pop argv param off stack
    pop_local_var();

    // pop $self off stack if present
    if (selfid) {
        pop_local_var();
    }
}

void UserSignature::resolve() {
    if (resolved)
        return;

    resolved = true;

    if (!returnTypeInfo) {
        returnTypeInfo = QoreParseTypeInfo::resolveAndDelete(parseReturnTypeInfo, loc);
        parseReturnTypeInfo = nullptr;
    }
#ifdef DEBUG
    else assert(!parseReturnTypeInfo);
#endif

    // issue #3644: to fix recursive errors in signature resolution, first resolve types, then args
    bool has_def_args = true;
    for (unsigned i = 0; i < parseTypeList.size(); ++i) {
        if (parseTypeList[i]) {
            assert(!typeList[i]);
            typeList[i] = QoreParseTypeInfo::resolveAndDelete(parseTypeList[i], loc);
        }
        if (!has_def_args && defaultArgList[i]) {
            has_def_args = true;
        }
    }

    // now resolve default args
    if (has_def_args) {
        for (unsigned i = 0; i < parseTypeList.size(); ++i) {
            // initialize default arguments
            if (defaultArgList[i]) {
                int lvids = 0;
                const QoreTypeInfo* argTypeInfo = nullptr;
                parse_init_value(defaultArgList[i], selfid, 0, lvids, argTypeInfo);
                if (lvids) {
                    parse_error(*loc, "illegal local variable declaration in default value expression in parameter '%s'", names[i].c_str());
                    while (lvids--)
                        pop_local_var();
                }
                // check type compatibility
                if (!QoreTypeInfo::parseAccepts(typeList[i], argTypeInfo)) {
                    QoreStringNode* desc = new QoreStringNode;
                    desc->sprintf("parameter '%s' expects ", names[i].c_str());
                    QoreTypeInfo::getThisType(typeList[i], *desc);
                    desc->concat(", but the default value is ");
                    QoreTypeInfo::getThisType(argTypeInfo, *desc);
                    desc->concat(" instead");
                    qore_program_private::makeParseException(getProgram(), *loc, "PARSE-TYPE-ERROR", desc);
                }
            }
        }
    }
    parseTypeList.clear();
}

bool QoreFunction::existsVariant(const type_vec_t& paramTypeInfo) const {
    for (vlist_t::const_iterator i = vlist.begin(), e = vlist.end(); i != e; ++i) {
        AbstractFunctionSignature* sig = (*i)->getSignature();
        assert(sig);
        unsigned np = sig->numParams();
        if (np != paramTypeInfo.size())
            continue;
        if (!np)
            return true;
        bool ok = true;
        for (unsigned pi = 0; pi < np; ++pi) {
            if (!QoreTypeInfo::isInputIdentical(paramTypeInfo[pi], sig->getParamTypeInfo(pi))) {
                ok = false;
                break;
            }
        }
        if (ok)
            return true;
    }
    return false;
}

static QoreStringNode* getNoopError(const QoreFunction* func, const QoreFunction* aqf, const AbstractQoreFunctionVariant* variant) {
    QoreStringNode* desc = new QoreStringNode;
    do_call_name(*desc, aqf);
    desc->sprintf("%s) is a variant that returns a constant value when incorrect data types are passed to the function", variant->getSignature()->getSignatureText());
    const QoreTypeInfo* rti = variant->getReturnTypeInfo();
    if (QoreTypeInfo::hasType(rti) && !variant->numParams()) {
        desc->concat(" and always returns ");
        if (QoreTypeInfo::getUniqueReturnClass(rti) || func->className()) {
            QoreTypeInfo::getThisType(rti, *desc);
        } else {
            // get actual value and include in warning
            ExceptionSink xsink;
            CodeEvaluationHelper ceh(&xsink, func, variant, "noop-dummy");
            ValueHolder v(variant->evalFunction(func->getName(), ceh, 0), 0);
            //ReferenceHolder<AbstractQoreNode> v(variant->evalFunction(func->getName(), ceh, 0), 0);
            if (v->isNothing())
                desc->concat("NOTHING");
            else {
                QoreNodeAsStringHelper vs(*v, FMT_NONE, 0);
                desc->sprintf("the following value: %s (", vs->getBuffer());
                QoreTypeInfo::getThisType(rti, *desc);
                desc->concat(')');
            }
        }
    }
    return desc;
}

static bool skip_method_variant(const AbstractQoreFunctionVariant* v, const qore_class_private* class_ctx, bool internal_access) {
    assert(dynamic_cast<const MethodVariantBase*>(v));
    const MethodVariantBase* mvb = reinterpret_cast<const MethodVariantBase*>(v);
    ClassAccess va = mvb->getAccess();
    // skip if the variant is not accessible
    return ((!class_ctx && va > Public) || (va == Internal && !internal_access));
}

static AbstractQoreFunctionVariant* doSingleVariantTypeException(const QoreProgramLocation* loc, int pi, const char* class_name, const char* name, const char* sig, const QoreTypeInfo* proto, const QoreTypeInfo* arg) {
    QoreStringNode* desc = new QoreStringNode("argument ");
    desc->sprintf("%d to '", pi);
    if (class_name)
        desc->sprintf("%s::", class_name);
    desc->sprintf("%s(%s)' expects %s, but call supplies %s", name, sig, QoreTypeInfo::getName(proto), QoreTypeInfo::getName(arg));
    qore_program_private::makeParseException(getProgram(), *loc, "PARSE-TYPE-ERROR", desc);
    return nullptr;
}

static void do_call_str(QoreString &desc, const QoreFunction* func, const type_vec_t& argTypeInfo) {
   unsigned num_args = argTypeInfo.size();
   do_call_name(desc, func);
   if (num_args)
      for (unsigned i = 0; i < num_args; ++i) {
         desc.concat(QoreTypeInfo::getName(argTypeInfo[i]));
         if (i != (num_args - 1))
            desc.concat(", ");
      }
   desc.concat(')');
}

static void warn_excess_args(const QoreProgramLocation* loc, const QoreFunction* func, const type_vec_t& argTypeInfo, AbstractFunctionSignature* sig) {
   unsigned nargs = argTypeInfo.size();
   unsigned nparams = sig->numParams();

   QoreStringNode* desc = new QoreStringNode("call to ");
   desc->concat(func->className() ? "method " : "function ");
   do_call_name(*desc, func);
   if (nparams)
      desc->concat(sig->getSignatureText());
   desc->concat(") made as ");
   do_call_str(*desc, func, argTypeInfo);
   unsigned diff = nargs - nparams;
   desc->sprintf(" (with %d excess argument%s)", diff, diff == 1 ? "" : "s");
   // raise warning if require-types is not set
   //if (getProgram()->getParseOptions64() & (PO_REQUIRE_TYPES | PO_STRICT_ARGS)) {
   if (parse_get_parse_options() & (PO_REQUIRE_TYPES | PO_STRICT_ARGS)) {
      desc->concat("; this is an error when PO_REQUIRE_TYPES or PO_STRICT_ARGS is set");
      qore_program_private::makeParseException(getProgram(), *loc, "CALL-WITH-TYPE-ERRORS", desc);
   }
   else {
      // raise warning
      desc->concat("; excess arguments will be ignored; to disable this warning, use '%%disable-warning excess-args' in your code");
      qore_program_private::makeParseWarning(getProgram(), *loc, QP_WARN_EXCESS_ARGS, "EXCESS-ARGS", desc);
   }
}

static int check_extra_args(AbstractFunctionSignature* sig, const type_vec_t& argTypeInfo) {
   // either extra arguments are ignored (strict_args is false) or preclude the match
   // see if any of the extra arguments have a type
   for (unsigned pi = sig->numParams(); pi < argTypeInfo.size(); ++pi) {
      const QoreTypeInfo* a = argTypeInfo[pi];
      if (!QoreTypeInfo::parseAcceptsReturns(a, NT_NOTHING))
         return -1;
   }
   return 0;
}

QoreListNode* QoreFunction::runtimeGetCallVariants() const {
   ReferenceHolder<QoreListNode> rv(new QoreListNode(autoHashTypeInfo), nullptr);

   const char* class_name = className();
   int64 ppo = runtime_get_parse_options();

   printd(5, "QoreFunction::runtimeGetCallVariants() this: %p, class_name: %s\n", this, class_name);
   for (vlist_t::const_iterator i = vlist.begin(), e = vlist.end(); i != e; ++i) {
      // get code flags for the variant
      int64 vflags = (*i)->getFlags();

      // get parse options
      int64 po = (*i)->getParseOptions(ppo);
      // if we should ignore "noop" variants
      bool strict_args = po & (PO_REQUIRE_TYPES|PO_STRICT_ARGS);

      // ignore "runtime noop" variants if necessary
      if (strict_args && (vflags & QCF_RUNTIME_NOOP)) {
         printd(5, "QoreFunction::runtimeGetCallVariants() this: %p, skip runtime noop, vflags: %p\n", this, vflags);
         continue;
      }

      // check functionality flags to see if the variant is accessible
      int64 vfflags = (*i)->getFunctionality();
      if ((vfflags & po & ~PO_POSITIVE_OPTIONS)
          || ((vfflags & PO_POSITIVE_OPTIONS)
              && (((vfflags & PO_POSITIVE_OPTIONS) & po) != (vfflags & PO_POSITIVE_OPTIONS)))) {
         printd(5, "QoreFunction::runtimeGetCallVariants() this: %p, skip functionality, vfflags: %p\n", this, vfflags);
         continue;
      }

      ReferenceHolder<QoreHashNode> h(new QoreHashNode(autoTypeInfo), nullptr);

      SimpleRefHolder<QoreStringNode> desc(new QoreStringNode);

      AbstractFunctionSignature* sig = (*i)->getSignature();
      assert(sig);

      // add "desc" key
      QoreStringNodeMaker* sm = new QoreStringNodeMaker("%s%s%s(%s)",
            class_name ? class_name : "", class_name ? "::" : "", getName(),
            sig->getSignatureText());
      printd(5, "QoreFunction::runtimeGetCallVariants() this: %p, desc: %s, numparams: %d\n", this, sm->c_str(), sig->numParams());
      h->setKeyValue("desc", sm, nullptr);

      // add "params" key
      ReferenceHolder<QoreListNode> params(new QoreListNode(autoTypeInfo), nullptr);
      for (unsigned pi = 0; pi < sig->numParams(); ++pi) {
         QoreStringNode* s = new QoreStringNode(QoreTypeInfo::getName(sig->getParamTypeInfo(pi)));
         printd(5, "QoreFunction::runtimeGetCallVariants() this: %p, param %d: %s\n", this, pi, s->c_str());
         params->push(s, nullptr);
      }
      h->setKeyValue("params", params.release(), nullptr);

      rv->push(h.release(), nullptr);
   }

   return rv->empty() ? nullptr : rv.release();
}

// finds a variant at runtime
const AbstractQoreFunctionVariant* QoreFunction::runtimeFindVariant(ExceptionSink* xsink, const QoreListNode* args, bool only_user, const qore_class_private* class_ctx) const {
    // the lowest match length with the highest score wins
    int match_len = -1;
    int match = -1;
    const AbstractQoreFunctionVariant* variant = nullptr;
    //const AbstractQoreFunctionVariant* saved_variant = nullptr;

    //printd(5, "QoreFunction::runtimeFindVariant() this: %p %s%s%s() vlist: %d ilist: %d args: %p (%d) cctx: %p '%s'\n", this, className() ? className() : "", className() ? "::" : "", getName(), vlist.size(), ilist.size(), args, args ? args->size() : 0, class_ctx, class_ctx ? class_ctx->name.c_str() : "n/a");

    unsigned nargs = args ? args->size() : 0;

    const QoreFunction* aqf = nullptr;
    AbstractFunctionSignature* sig = nullptr;

    // parent class while iterating
    const qore_class_private* last_class = nullptr;
    bool internal_access = false;

    int64 ppo = runtime_get_parse_options();

    int cnt = 0;

    // iterate through inheritance list
    for (ilist_t::const_iterator aqfi = ilist.begin(), aqfe = ilist.end(); aqfi != aqfe; ++aqfi) {
        bool stop;
        aqf = ilist.getFunction(class_ctx, last_class, aqfi, internal_access, stop);
        if (!aqf) {
            break;
        }

        // issue #3070: skip abstract methods
        if (last_class && static_cast<const MethodFunctionBase*>(aqf)->isAbstract()) {
            continue;
        }

        //printd(5, "QoreFunction::runtimeFindVariant() this: %p %s%s%s(...) size: %d last_class: %p ctx: %p: %s\n", this, aqf->className() ? aqf->className() : "", className() ? "::" : "", getName(), ilist.size(), last_class, class_ctx, class_ctx ? class_ctx->name.c_str() : "n/a");

        for (vlist_t::const_iterator i = aqf->vlist.begin(), e = aqf->vlist.end(); i != e; ++i) {
            // skip checking variant if we are only looking for user variants and this variant is builtin
            if (only_user && !(*i)->isUser()) {
                continue;
            }

            // skip if the variant is not accessible or abstract
            if (last_class
                && (skip_method_variant(*i, class_ctx, internal_access)
                    || static_cast<const MethodVariantBase*>(*i)->isAbstract())) {
                    continue;
            }

            // get runtime parse options
            int64 po = (*i)->getParseOptions(ppo);
            // if we should ignore "noop" variants
            bool strict_args = po & (PO_REQUIRE_TYPES|PO_STRICT_ARGS);

            int64 vflags = (*i)->getFlags();

            // ignore "runtime noop" variants if necessary
            if (strict_args && (vflags & QCF_RUNTIME_NOOP))
                continue;

            // does the variant accept extra arguments?
            bool uses_extra_args = vflags & QCF_USES_EXTRA_ARGS;

            ++cnt;

            sig = (*i)->getSignature();
            assert(sig);

            //printd(5, "QoreFunction::runtimeFindVariant() this: %p %s(%s) args: %p (%d) class: %s class_ctx: %p '%s' nargs: %d nparams: %d\n", this, getName(), sig->getSignatureText(), args, args ? args->size() : 0, aqf->className() ? aqf->className() : "n/a", class_ctx, class_ctx ? class_ctx->name.c_str() : "n/a", nargs, sig->numParams());

            // issue 1507: ensure that calls with no arguments and no params are considered a perfect match
            if (!nargs && !sig->numParams()) {
                variant = *i;
                break;
            }

            // skip variants with signatures with fewer possible elements than the best match already
            if ((int)(sig->getParamTypes() * QTI_IDENT) < match)
                continue;

            int count = 0;
            bool ok = true;
            for (unsigned pi = 0; pi < sig->numParams(); ++pi) {
                const QoreTypeInfo* t = sig->getParamTypeInfo(pi);
                QoreValue n;
                if (args)
                    n = args->retrieveEntry(pi);

                //printd(5, "QoreFunction::runtimeFindVariant() this: %p %s(%s) i: %d param: %s arg: %s\n", this, getName(), sig->getSignatureText(), pi, QoreTypeInfo::getName(t), n.getFullTypeName());

                int rc;
                if (n.isNothing() && sig->hasDefaultArg(pi))
                    rc = QTI_IGNORE;
                else {
                    rc = QoreTypeInfo::runtimeAcceptsValue(t, n);
                    //printd(5, "QoreFunction::runtimeFindVariant() this: %p %s(%s) i: %d param: %s arg: %s rc: %d\n", this, getName(), sig->getSignatureText(), pi, QoreTypeInfo::getName(t), n.getFullTypeName(), rc);
                    if (rc == QTI_NOT_EQUAL) {
                        ok = false;
                        break;
                    }
                    // do not count default matches with non-existent arguments
                    if (pi >= nargs)
                        rc = QTI_IGNORE;
                }

                // only increment for actual type matches (t may be NULL)
                if (t && rc != QTI_IGNORE)
                    count += rc;
            }
            if (!ok)
                continue;

            // check for extra args
            if ((sig->numParams() < nargs) && strict_args && !uses_extra_args) {
                bool has_arg = false;
                for (unsigned pi = sig->numParams(); pi < nargs; ++pi) {
                    if (!args->retrieveEntry(pi).isNothing()) {
                        has_arg = true;
                        break;
                    }
                }
                if (has_arg)
                    continue;
            }

            //printd(5, "QoreFunction::runtimeFindVariant() count: %d match: %d match_len: %d np: %d v: %p\n", count, match, match_len, sig->numParams(), variant);

            if (count > match || (count == match && (match_len == -1 || (sig->numParams() < (unsigned)match_len)))) {
                match = count;
                variant = *i;

                match_len = sig->numParams();
            }
        }
        // issue 1229: stop searching the class hierarchy if a match found
        if (stop || variant)
            break;
    }
    /*
    if (saved_variant) {
        assert(!variant);
        variant = saved_variant;
    }
    */

    if (!variant && !only_user) {
        QoreStringNode* desc = new QoreStringNode("no variant matching '");
        const char* class_name = className();
        if (class_name)
            desc->sprintf("%s::", class_name);
        desc->sprintf("%s(", getName());
        add_args(*desc, args);
        desc->concat(")' can be found; ");
        if (!cnt) {
            desc->concat("no variants were accessible in this execution context");
        } else {
            desc->concat("the following variants were tested:");

            last_class = 0;
            internal_access = false;

            // add variants tested
            // iterate through inheritance list
            for (ilist_t::const_iterator aqfi = ilist.begin(), aqfe = ilist.end(); aqfi != aqfe; ++aqfi) {
                bool stop;
                aqf = ilist.getFunction(class_ctx, last_class, aqfi, internal_access, stop);
                if (!aqf)
                    break;
                class_name = aqf->className();

                for (vlist_t::const_iterator i = aqf->vlist.begin(), e = aqf->vlist.end(); i != e; ++i) {
                    // skip if the variant is not accessible or abstract
                    if (last_class
                        && (skip_method_variant(*i, class_ctx, internal_access)
                            || static_cast<const MethodVariantBase*>(*i)->isAbstract())) {
                            continue;
                    }
                    desc->concat("\n   ");
                    if (class_name)
                        desc->sprintf("%s::", class_name);
                    desc->sprintf("%s(%s)", getName(), (*i)->getSignature()->getSignatureText());
                }
                if (stop)
                    break;
            }
        }
        xsink->raiseException("RUNTIME-OVERLOAD-ERROR", desc);
    } else if (variant) {
        QoreProgram* pgm = getProgram();

        // pgm could be zero if called from a foreign thread with no current Program
        if (pgm) {
            // get runtime parse options
            int64 po = variant->getParseOptions(ppo);

            // check parse options
            int64 vflags = variant->getFunctionality();
            // check restrictive flags
            //printd(5, "QoreFunction::runtimeFindVariant() this: %p %s() returning %p %s(%s) vflags: " QLLD " po: " QLLD " neg: " QLLD "\n", this, getName(), variant, getName(), variant ? variant->getSignature()->getSignatureText() : "n/a", (vflags & po & ~PO_POSITIVE_OPTIONS));
            if ((vflags & po & ~PO_POSITIVE_OPTIONS) || ((vflags & PO_POSITIVE_OPTIONS) && (((vflags & PO_POSITIVE_OPTIONS) & po) != (vflags & PO_POSITIVE_OPTIONS)))) {
                //printd(5, "QoreFunction::runtimeFindVariant() this: %p %s(%s) getProgram(): %p getProgram()->getParseOptions64(): %x variant->getFunctionality(): %x\n", this, getName(), variant->getSignature()->getSignatureText(), getProgram(), getProgram()->getParseOptions64(), variant->getFunctionality());
                if (!only_user) {
                    const char* class_name = className();
                    xsink->raiseException("INVALID-FUNCTION-ACCESS", "parse options do not allow access to builtin %s '%s%s%s(%s)'", class_name ? "method" : "function", class_name ? class_name : "", class_name ? "::" : "", getName(), variant->getSignature()->getSignatureText());
                }
                return 0;
            }

            assert(!(po & (PO_REQUIRE_TYPES|PO_STRICT_ARGS)) || !(variant->getFlags() & QCF_RUNTIME_NOOP));
        }
    }

    //printd(5, "QoreFunction::runtimeFindVariant() this: %p %s() returning %p %s(%s) class: %s\n", this, getName(), variant, getName(), variant ? variant->getSignature()->getSignatureText() : "n/a", variant && aqf && aqf->className() ? aqf->className() : "n/a");

    return variant;
}

// finds a variant at runtime
const AbstractQoreFunctionVariant* QoreFunction::runtimeFindVariant(ExceptionSink* xsink, const type_vec_t& args, const qore_class_private* class_ctx) const {
    int match = -1;

    const AbstractQoreFunctionVariant* variant = nullptr;

    //printd(5, "QoreFunction::runtimeFindVariant() this: %p %s%s%s() vlist: %d ilist: %d args: %p (%d)\n", this, className() ? className() : "", className() ? "::" : "", getName(), vlist.size(), ilist.size(), args.size());

    const QoreFunction* aqf = nullptr;
    AbstractFunctionSignature* sig = nullptr;

    // parent class while iterating
    const qore_class_private* last_class = nullptr;
    bool internal_access = false;

    int64 ppo = runtime_get_parse_options();

    // iterate through inheritance list
    for (ilist_t::const_iterator aqfi = ilist.begin(), aqfe = ilist.end(); aqfi != aqfe; ++aqfi) {
        bool stop;
        aqf = ilist.getFunction(class_ctx, last_class, aqfi, internal_access, stop);
        if (!aqf) {
            break;
        }
        // issue #3070: skip abstract methods
        if (last_class && static_cast<const MethodFunctionBase*>(aqf)->isAbstract()) {
            continue;
        }

        //printd(5, "QoreFunction::runtimeFindVariant() this: %p %s%s%s(...) size: %d last_class: %p ctx: %p: %s\n", this, aqf->className() ? aqf->className() : "", className() ? "::" : "", getName(), ilist.size(), last_class, class_ctx, class_ctx ? class_ctx->name.c_str() : "n/a");

        for (vlist_t::const_iterator i = aqf->vlist.begin(), e = aqf->vlist.end(); i != e; ++i) {
            // skip if the variant is not accessible or abstract
            if (last_class
                && (skip_method_variant(*i, class_ctx, internal_access)
                    || static_cast<const MethodVariantBase*>(*i)->isAbstract())) {
                    continue;
            }

            // get runtime parse options
            int64 po = (*i)->getParseOptions(ppo);
            // if we should ignore "noop" variants
            bool strict_args = po & (PO_REQUIRE_TYPES|PO_STRICT_ARGS);

            int64 vflags = (*i)->getFlags();

            // ignore "runtime noop" variants if necessary
            if (strict_args && (vflags & QCF_RUNTIME_NOOP))
                continue;

            sig = (*i)->getSignature();
            assert(sig);

            //printd(5, "QoreFunction::runtimeFindVariant() this: %p %s(%s) args: %d class: %s class_ctx: %p '%s' nparams: %d\n", this, getName(), sig->getSignatureText(), args.size(), aqf->className() ? aqf->className() : "n/a", class_ctx, class_ctx ? class_ctx->name.c_str() : "n/a", sig->numParams());

            // issue 1507: ensure that calls with no arguments and no params are considered a perfect match
            if (args.empty() && !sig->numParams()) {
                variant = *i;
                break;
            }

            // skip variants with signatures a different number of arguments than provided
            if (sig->numParams() != args.size()) {
                continue;
            }

            int count = 0;
            bool ok = true;
            for (unsigned pi = 0; pi < sig->numParams(); ++pi) {
                const QoreTypeInfo* t = sig->getParamTypeInfo(pi);
                const QoreTypeInfo* a = args[pi];

                int rc = QoreTypeInfo::runtimeTypeMatch(t, a);
                //printd(5, "QoreFunction::runtimeFindVariant() '%s' %d ('%s' <=> %s'): rc: %d\n", sig->getSignatureText(), pi, QoreTypeInfo::getName(t), QoreTypeInfo::getName(a), rc);
                if (rc < QTI_WILDCARD) {
                    ok = false;
                    break;
                }
                count += rc;
            }
            if (!ok) {
                continue;
            }

            //printd(5, "QoreFunction::runtimeFindVariant() variant: %p count: %d match: %d (%s)\n", variant, count, match, sig->getSignatureText());
            if (count > match) {
                variant = *i;
                match = count;
            }
        }
        // issue 1229: stop searching the class hierarchy if a match found
        if (stop || variant)
            break;
    }
    return checkVariant(xsink, args, class_ctx, aqf, last_class, internal_access, ppo, variant);
}

const AbstractQoreFunctionVariant* QoreFunction::checkVariant(ExceptionSink* xsink, const type_vec_t& args,
    const qore_class_private* class_ctx, const QoreFunction* aqf, const qore_class_private* last_class,
    bool internal_access, int64 ppo, const AbstractQoreFunctionVariant* variant) const {
    if (!variant) {
        QoreStringNode* desc = new QoreStringNode("no variant matching '");
        const char* class_name = className();
        do_call_str(*desc, this, args);
        desc->concat("' can be found; ");
        desc->concat("the following variants are defined:");

        last_class = nullptr;
        internal_access = false;

        // add variants tested
        // iterate through inheritance list
        for (ilist_t::const_iterator aqfi = ilist.begin(), aqfe = ilist.end(); aqfi != aqfe; ++aqfi) {
            bool stop;
            aqf = ilist.getFunction(class_ctx, last_class, aqfi, internal_access, stop);
            if (!aqf)
                break;
            class_name = aqf->className();

            for (vlist_t::const_iterator i = aqf->vlist.begin(), e = aqf->vlist.end(); i != e; ++i) {
                // skip if the variant is not accessible or abstract
                if (last_class
                    && (skip_method_variant(*i, class_ctx, internal_access)
                        || static_cast<const MethodVariantBase*>(*i)->isAbstract())) {
                        continue;
                }
                desc->concat("\n   ");
                if (class_name)
                desc->sprintf("%s::", class_name);
                desc->sprintf("%s(%s)", getName(), (*i)->getSignature()->getSignatureText());
            }
            if (stop)
                break;
        }
        xsink->raiseException("VARIANT-MATCH-ERROR", desc);
    } else if (variant) {
        // get runtime parse options
        int64 po = variant->getParseOptions(ppo);

        // check parse options
        int64 vflags = variant->getFunctionality();
        // check restrictive flags
        //printd(5, "QoreFunction::runtimeFindVariant() this: %p %s() returning %p %s(%s) vflags: " QLLD " po: " QLLD " neg: " QLLD "\n", this, getName(), variant, getName(), variant ? variant->getSignature()->getSignatureText() : "n/a", (vflags & po & ~PO_POSITIVE_OPTIONS));
        if ((vflags & po & ~PO_POSITIVE_OPTIONS) || ((vflags & PO_POSITIVE_OPTIONS) && (((vflags & PO_POSITIVE_OPTIONS) & po) != (vflags & PO_POSITIVE_OPTIONS)))) {
            //printd(5, "QoreFunction::runtimeFindVariant() this: %p %s(%s) getProgram(): %p getProgram()->getParseOptions64(): %x variant->getFunctionality(): %x\n", this, getName(), variant->getSignature()->getSignatureText(), getProgram(), getProgram()->getParseOptions64(), variant->getFunctionality());
            const char* class_name = className();
            xsink->raiseException("INVALID-FUNCTION-ACCESS", "parse options do not allow access to builtin %s '%s%s%s(%s)'", class_name ? "method" : "function", class_name ? class_name : "", class_name ? "::" : "", getName(), variant->getSignature()->getSignatureText());
            return nullptr;
        }

        assert(!(po & (PO_REQUIRE_TYPES|PO_STRICT_ARGS)) || !(variant->getFlags() & QCF_RUNTIME_NOOP));
    }

    //printd(5, "QoreFunction::checkVariant() this: %p %s() returning %p %s(%s) class: %s\n", this, getName(), variant, getName(), variant ? variant->getSignature()->getSignatureText() : "n/a", variant && aqf && aqf->className() ? aqf->className() : "n/a");

    return variant;
}

// finds a variant at runtime
const AbstractQoreFunctionVariant* QoreFunction::runtimeFindExactVariant(ExceptionSink* xsink, const type_vec_t& args, const qore_class_private* class_ctx) const {
    const AbstractQoreFunctionVariant* variant = nullptr;

    //printd(5, "QoreFunction::runtimeFindExactVariant() this: %p %s%s%s() vlist: %d ilist: %d args: %p (%d)\n", this, className() ? className() : "", className() ? "::" : "", getName(), vlist.size(), ilist.size(), args.size());

    const QoreFunction* aqf = nullptr;
    AbstractFunctionSignature* sig = nullptr;

    // parent class while iterating
    const qore_class_private* last_class = nullptr;
    bool internal_access = false;

    int64 ppo = runtime_get_parse_options();

    // iterate through inheritance list
    for (ilist_t::const_iterator aqfi = ilist.begin(), aqfe = ilist.end(); aqfi != aqfe; ++aqfi) {
        bool stop;
        aqf = ilist.getFunction(class_ctx, last_class, aqfi, internal_access, stop);
        if (!aqf) {
            break;
        }
        // issue #3070: skip abstract methods
        if (last_class && static_cast<const MethodFunctionBase*>(aqf)->isAbstract()) {
            continue;
        }

        //printd(5, "QoreFunction::runtimeFindExactVariant() this: %p %s%s%s(...) size: %d last_class: %p ctx: %p: %s\n", this, aqf->className() ? aqf->className() : "", className() ? "::" : "", getName(), ilist.size(), last_class, class_ctx, class_ctx ? class_ctx->name.c_str() : "n/a");

        for (vlist_t::const_iterator i = aqf->vlist.begin(), e = aqf->vlist.end(); i != e; ++i) {
            // skip if the variant is not accessible or abstract
            if (last_class
                && (skip_method_variant(*i, class_ctx, internal_access)
                    || static_cast<const MethodVariantBase*>(*i)->isAbstract())) {
                    continue;
            }

            // get runtime parse options
            int64 po = (*i)->getParseOptions(ppo);
            // if we should ignore "noop" variants
            bool strict_args = po & (PO_REQUIRE_TYPES|PO_STRICT_ARGS);

            int64 vflags = (*i)->getFlags();

            // ignore "runtime noop" variants if necessary
            if (strict_args && (vflags & QCF_RUNTIME_NOOP))
                continue;

            sig = (*i)->getSignature();
            assert(sig);

            //printd(5, "QoreFunction::runtimeFindExactVariant() this: %p %s(%s) args: %d class: %s class_ctx: %p '%s' nparams: %d\n", this, getName(), sig->getSignatureText(), args.size(), aqf->className() ? aqf->className() : "n/a", class_ctx, class_ctx ? class_ctx->name.c_str() : "n/a", sig->numParams());

            // issue 1507: ensure that calls with no arguments and no params are considered a perfect match
            if (args.empty() && !sig->numParams()) {
                variant = *i;
                break;
            }

            // skip variants with signatures a different number of arguments than provided
            if (sig->numParams() != args.size())
                continue;

            bool ok = true;
            for (unsigned pi = 0; pi < sig->numParams(); ++pi) {
                const QoreTypeInfo* t = sig->getParamTypeInfo(pi);
                const QoreTypeInfo* a = args[pi];
                if (t == a || (!t && a == anyTypeInfo))
                    continue;
                ok = false;
                break;
            }
            if (!ok)
                continue;

            variant = *i;
        }
        // issue 1229: stop searching the class hierarchy if a match found
        if (stop || variant)
            break;
    }
    return checkVariant(xsink, args, class_ctx, aqf, last_class, internal_access, ppo, variant);
}

// finds a variant at parse time
const AbstractQoreFunctionVariant* QoreFunction::parseFindVariant(const QoreProgramLocation* loc, const type_vec_t& argTypeInfo, const qore_class_private* class_ctx) const {
    // the lowest match length with the highest score wins
    int match_len = -1;
    // the number of parameters * 2 matched to arguments (compatible but not perfect match = 1, perfect match = 2)
    int match = -1;
    // the number of possible matches at runtime (due to missing types at parse time); number of parameters
    int pmatch = -1;
    // the number of arguments matched perfectly in case of a tie score
    int nperfect = -1;
    // number of possible variants
    unsigned npv = 0;

    // pointer to the variant matched
    const AbstractQoreFunctionVariant* variant = nullptr;
    // pointer to the last possible variant matched
    const AbstractQoreFunctionVariant* pvariant = nullptr;
    unsigned num_args = argTypeInfo.size();

    //printd(5, "QoreFunction::parseFindVariant() this: %p %s() vlist: %d ilist: %d num_args: %d\n", this, getName(), vlist.size(), ilist.size(), num_args);

    QoreFunction* aqf = nullptr;

    // parent class while iterating
    const qore_class_private* last_class = nullptr;
    bool internal_access = false;

    int64 po = parse_get_parse_options();

    int cnt = 0;

    // do we need to match at runtime
    bool runtime_match = false;

    // iterate through inheritance list
    for (ilist_t::const_iterator aqfi = ilist.begin(), aqfe = ilist.end(); aqfi != aqfe; ++aqfi) {
        bool stop;
        aqf = ilist.getFunction(class_ctx, last_class, aqfi, internal_access, stop);
        if (!aqf)
            break;
        //printd(5, "QoreFunction::parseFindVariant() %p %s testing function %p\n", this, getName(), aqf);
        assert(!aqf->vlist.empty());

        // check committed list
        for (vlist_t::const_iterator i = aqf->vlist.begin(), e = aqf->vlist.end(); i != e; ++i) {
            // skip if the variant is not accessible
            if (last_class && skip_method_variant(*i, class_ctx, internal_access)) {
                continue;
            }
            AbstractFunctionSignature* sig = (*i)->getSignature();

            // get variant parse flags
            int64 vflags = (*i)->getFlags();

            // if we should ignore "noop" variants
            bool strict_args = (*i)->getParseOptions(po) & (PO_REQUIRE_TYPES|PO_STRICT_ARGS);

            // ignore "noop" variants if necessary
            if (strict_args && (vflags & (QCF_NOOP | QCF_RUNTIME_NOOP)))
                continue;

            // does the variant accept extra arguments?
            bool uses_extra_args = vflags & QCF_USES_EXTRA_ARGS;

            ++cnt;

            //printd(5, "QoreFunction::parseFindVariant() this: %p checking committed %s(%s) variant: %p sig->pt: %d sig->mpt: %d match: %d, args: %d\n", this, getName(), sig->getSignatureText(), variant, sig->getParamTypes(), sig->getMinParamTypes(), match, num_args);

            // issue 1507: ensure that calls with no arguments and no params are considered a perfect match
            if (!num_args && !sig->numParams()) {
                variant = *i;
                break;
            }

            // skip variants with signatures with fewer possible elements than the best match already
            if ((int)(sig->numParams() * QTI_IDENT) > match) {
                int variant_pmatch = 0;
                int count = 0;
                int variant_nperfect = 0;
                bool variant_runtime_match = false;
                bool ok = true;

                for (unsigned pi = 0; pi < sig->numParams(); ++pi) {
                    const QoreTypeInfo* t = sig->getParamTypeInfo(pi);
                    bool pos_has_arg = num_args && num_args > pi;
                    const QoreTypeInfo* a = pos_has_arg ? argTypeInfo[pi] : nullptr;
                    if (pos_has_arg) {
                        pos_has_arg = (bool)a;
                    }

                    //printd(5, "QoreFunction::parseFindVariant() %s(%s) committed pi: %d num_args: %d t: %s (has type: %d) a: %s (%p) t->parseAccepts(a): %d\n", getName(), sig->getSignatureText(), pi, num_args, QoreTypeInfo::getName(t), QoreTypeInfo::hasType(t), QoreTypeInfo::getName(a), a, QoreTypeInfo::parseAccepts(t, a));

                    int rc = QTI_UNASSIGNED;
                    if (QoreTypeInfo::hasType(t)) {
                        if (!QoreTypeInfo::hasType(a)) {
                            if (pi < num_args) {
                                // we are missing parse-time type information, we need to match at runtime
                                variant_runtime_match = true;
                                break;
                            } else if (sig->hasDefaultArg(pi))
                                rc = QTI_IGNORE;
                            else
                                a = nothingTypeInfo;
                        } else if (QoreTypeInfo::isType(a, NT_NOTHING) && sig->hasDefaultArg(pi))
                            rc = QTI_IDENT;
                    }

                    if (rc == QTI_UNASSIGNED) {
                        bool may_not_match = false;
                        rc = QoreTypeInfo::parseAccepts(t, a, may_not_match);
                        //printd(5, "QoreFunction::parseFindVariant() %s(%s) rc: %d may_not_match: %d\n", getName(), sig->getSignatureText(), rc, may_not_match);
                        // if we might not match, we need to match at runtime
                        if (may_not_match) {
                            variant_runtime_match = true;
                            continue;
                        }
                        if (rc == QTI_IDENT)
                            ++variant_nperfect;
                    }

                    if (rc == QTI_NOT_EQUAL) {
                        ok = false;
                        // raise a detailed parse exception immediately if there is only one variant
                        if (ilist.size() == 1 && aqf->vlist.singular() && getProgram()->getParseExceptionSink())
                            return doSingleVariantTypeException(loc, pi + 1, aqf->className(), getName(), sig->getSignatureText(), t, a);
                        break;
                    }
                    ++variant_pmatch;
                    if (rc != QTI_IGNORE && pos_has_arg)
                        count += rc;
                }

                // stop searching if we need to match at runtime
                if (variant_runtime_match) {
                    runtime_match = true;
                    if (variant)
                        variant = nullptr;
                    break;
                }

                //printd(5, "QoreFunction::parseFindVariant() this: %p tested %s(%s) ok: %d count: %d match: %d variant_pmatch: %d variant_nperfect: %d nperfect: %d variant_runtime_match: %d\n", this, getName(), sig->getSignatureText(), ok, count, match, variant_pmatch, variant_nperfect, nperfect, variant_runtime_match);
                if (!ok)
                    continue;

                // now check if additional args are present
                if ((sig->numParams() < num_args) && !uses_extra_args && strict_args && check_extra_args(sig, argTypeInfo))
                    continue;

                if (!npv)
                    pvariant = variant;
                else
                    pvariant = nullptr;

                ++npv;

                if (count > match || (count == match && (variant_nperfect > nperfect || (variant_nperfect == nperfect && (match_len == -1 || sig->numParams() < (unsigned)match_len))))) {
                    // if we could possibly match less than another variant
                    // then we have to match at runtime
                    if (variant_pmatch < pmatch) {
                        variant = nullptr;
                        runtime_match = true;
                        break;
                    } else {
                        // only set variant if it's the longest absolute match and the
                        // longest potential match
                        pmatch = variant_pmatch;
                        match = count;
                        nperfect = variant_nperfect;
                        match_len = sig->numParams();
                        //printd(5, "QoreFunction::parseFindVariant() assigning variant %p %s(%s)\n", *i, getName(), sig->getSignatureText());
                        variant = *i;
                    }
                } else if (variant_pmatch && variant_pmatch >= pmatch) {
                    // if we could possibly match less than another variant
                    // then we have to match at runtime
                    variant = nullptr;
                    pmatch = variant_pmatch;
                    match_len = -1;
                }
            }
        }

        // stop searching if we have to match at runtime
        if (runtime_match) {
            assert(!variant);
            break;
        }

        if (runtime_match) {
            if (variant)
                variant = nullptr;
            break;
        }
        // issue 1229: stop searching the class hierarchy if a match found
        if (stop || variant)
            break;
    }

    assert(!(runtime_match && variant));

    // if we only have one possible variant, then assign it, even it it's not a guaranteed match
    if (!variant && pvariant)
        variant = pvariant;
    else if (!variant && !runtime_match && pmatch == -1 && getProgram()->getParseExceptionSink()) {
        QoreStringNode* desc = new QoreStringNode("no variant matching '");
        do_call_str(*desc, this, argTypeInfo);
        desc->concat("' can be found; ");
        if (!cnt) {
            desc->concat("no variants were accessible in this context");
        }
        else {
            desc->concat("the following variants were tested:");

            last_class = 0;
            internal_access = false;

            // add variants tested
            // iterate through inheritance list
            for (ilist_t::const_iterator aqfi = ilist.begin(), aqfe = ilist.end(); aqfi != aqfe; ++aqfi) {
                bool stop;
                aqf = ilist.getFunction(class_ctx, last_class, aqfi, internal_access, stop);
                if (!aqf)
                break;
                const char* class_name = aqf->className();

                for (vlist_t::const_iterator i = aqf->vlist.begin(), e = aqf->vlist.end(); i != e; ++i) {
                    // skip if the variant is not accessible
                    if (last_class && skip_method_variant(*i, class_ctx, internal_access))
                        continue;

                    // if we should ignore "noop" variants
                    bool strict_args = (*i)->getParseOptions(po) & (PO_REQUIRE_TYPES|PO_STRICT_ARGS);

                    // ignore "noop" variants if necessary
                    if (strict_args && ((*i)->getFlags() & (QCF_NOOP | QCF_RUNTIME_NOOP)))
                        continue;

                    desc->concat("\n   ");
                    if (class_name)
                        desc->sprintf("%s::", class_name);
                    desc->sprintf("%s(%s)", getName(), (*i)->getSignature()->getSignatureText());
                }
                if (stop)
                break;
            }
        }
        qore_program_private::makeParseException(getProgram(), *loc, "PARSE-TYPE-ERROR", desc);
    } else if (variant) {
        int64 flags = variant->getFlags();
        if (flags & (QCF_NOOP | QCF_RUNTIME_NOOP)) {
            QoreStringNode* desc = getNoopError(this, aqf, variant);
            desc->concat("; to disable this warning, use '%disable-warning invalid-operation' in your code");
            qore_program_private::makeParseWarning(getProgram(), *loc, QP_WARN_CALL_WITH_TYPE_ERRORS, "CALL-WITH-TYPE-ERRORS", desc);
        }

        AbstractFunctionSignature* sig = variant->getSignature();
        if (!(flags & QCF_USES_EXTRA_ARGS) && num_args > sig->numParams())
            warn_excess_args(loc, this, argTypeInfo, sig);
    }

    /*
    {
        QoreString desc("(");
        for (int i = 0; i < argTypeInfo.size(); ++i)
            desc.sprintf("%s, ", QoreTypeInfo::getName(argTypeInfo[i]));
        if (desc.size() > 1)
            desc.terminate(desc.size() - 2);
        desc.concat(")");
        printd(0, "QoreFunction::parseFindVariant() this: %p %s%s%s() pmatch: %d call args: '%s' returning %p (class %s) %s(%s) flags: %lld runtime_match: %d\n", this, className() ? className() : "", className() ? "::" : "", getName(), pmatch, desc.c_str(), variant, variant && className() ? reinterpret_cast<const MethodVariant*>(variant)->getClass()->getName() : "n/a", getName(), variant ? variant->getSignature()->getSignatureText() : "n/a", variant ? variant->getFlags() : 0ll, runtime_match);
    }
    */

    //printd(5, "QoreFunction::parseFindVariant() this: %p %s%s%s() returning %p %s(%s) flags: %lld num_args: %d\n", this, className() ? className() : "", className() ? "::" : "", getName(), variant, getName(), variant ? variant->getSignature()->getSignatureText() : "n/a", variant ? variant->getFlags() : 0ll, num_args);

    return variant;
}

// if the variant was identified at parse time, then variant will not be NULL, otherwise if NULL, then it is identified at run time
QoreValue QoreFunction::evalFunction(const AbstractQoreFunctionVariant* variant, const QoreListNode* args, QoreProgram *pgm, ExceptionSink* xsink) const {
    const char* fname = getName();

    // issue #3027: catch recursive references during parse initialization
    if (!parse_init_done) {
        SimpleRefHolder<QoreStringNode> desc(new QoreStringNode("recursive reference to "));
        const char* class_name = className();
        if (class_name) {
            desc->sprintf("method %s::", class_name);
        } else {
            desc->concat("function ");
        }
        desc->sprintf("%s(", fname);
        if (variant) {
            desc->concat(variant->getSignature()->getSignatureText());
        }
        desc->concat(") during parse initialization");
        xsink->raiseException("PARSE-EXCEPTION", desc.release());
        return QoreValue();
    }

    CodeEvaluationHelper ceh(xsink, this, variant, fname, args);
    if (*xsink) return QoreValue();
    // issue #3024: make the caller's call context available
    ProgramCallContextHelper pcch(pgm);

    return variant->evalFunction(fname, ceh, xsink);
}

// if the variant was identified at parse time, then variant will not be NULL, otherwise if NULL, then it is identified at run time
QoreValue QoreFunction::evalFunctionTmpArgs(const AbstractQoreFunctionVariant* variant, QoreListNode* args, QoreProgram *pgm, ExceptionSink* xsink) const {
    const char* fname = getName();
    CodeEvaluationHelper ceh(xsink, this, variant, fname, args);
    if (*xsink) return QoreValue();
    // issue #3024: make the caller's call context available
    ProgramCallContextHelper pcch(pgm);

    return variant->evalFunction(fname, ceh, xsink);
}

// finds a variant and checks variant capabilities against current
// program parse options
QoreValue QoreFunction::evalDynamic(const QoreListNode* args, ExceptionSink* xsink) const {
    const char* fname = getName();
    const AbstractQoreFunctionVariant* variant = 0;
    CodeEvaluationHelper ceh(xsink, this, variant, fname, args);
    if (*xsink) return QoreValue();

    return variant->evalFunction(fname, ceh, xsink);
}

void QoreFunction::addBuiltinVariant(AbstractQoreFunctionVariant* variant) {
    assert(variant->getCallType() == CT_BUILTIN);
#ifdef DEBUG
    // FIXME: this algorithm is no longer valid due to default arguments
    // does not detect ambiguous signatures
    AbstractFunctionSignature* sig = variant->getSignature();
    // check for duplicate parameter signatures
    for (vlist_t::iterator i = vlist.begin(), e = vlist.end(); i != e; ++i) {
        AbstractFunctionSignature* vs = (*i)->getSignature();
        unsigned tp = vs->numParams();
        if (tp != sig->numParams())
            continue;
        if (!tp) {
            printd(0, "BuiltinFunctionBase::addBuiltinVariant() this: %p %s(%s) added twice: %p, %p\n", this, getName(), sig->getSignatureText(), *i, variant);
            assert(false);
        }
        bool ok = false;
        for (unsigned pi = 0; pi < tp; ++pi) {
            if (vs->getParamTypeInfo(pi) != sig->getParamTypeInfo(pi)) {
                ok = true;
                break;
            }
        }
        if (!ok) {
            printd(0, "BuiltinFunctionBase::addBuiltinVariant() this: %p %s(%s) added twice: %p, %p\n", this, getName(), sig->getSignatureText(), *i, variant);
            assert(false);
        }
    }
#endif
    if (!has_builtin) {
        has_builtin = true;
    }
    if (all_priv) {
        all_priv = false;
    }
    if (!has_pub) {
        has_pub = true;
    }
    addVariant(variant);
}

UserVariantExecHelper::~UserVariantExecHelper() {
   if (!uvb)
      return;
   UserSignature* sig = uvb->getUserSignature();
   // uninstantiate local vars from param list
   for (unsigned i = 0; i < sig->numParams(); ++i) {
      //printd(5, "UserVariantExecHelper::~UserVariantExecHelper() this: %p %s %d/%d %p lv: %s (%s)\n", this, sig->getSignatureText(), i, sig->numParams(), sig->lv[i], sig->lv[i]->getName(), sig->lv[i]->getValueTypeName());
      sig->lv[i]->uninstantiate(xsink);
   }
}

UserVariantBase::UserVariantBase(StatementBlock *b, int n_sig_first_line, int n_sig_last_line, QoreValue params, RetTypeInfo* rv, bool synced)
    : signature(n_sig_first_line, n_sig_last_line, params, rv, b ? b->pwo.parse_options : parse_get_parse_options()), statements(b), gate(synced ? new VRMutex : nullptr),
        pgm(getProgram()), recheck(false), init(false) {
    //printd(5, "UserVariantBase::UserVariantBase() this: %p params: %p rv: %p b: %p synced: %d\n", params, rv, b, synced);
}

UserVariantBase::~UserVariantBase() {
    delete gate;
    delete statements;
}

int64 UserVariantBase::getParseOptions(int64 po) const {
    return statements ? statements->pwo.parse_options : po;
}

void UserVariantBase::parseInitPushLocalVars(const QoreTypeInfo* classTypeInfo) {
    signature.parseInitPushLocalVars(classTypeInfo);
}

void UserVariantBase::parseInitPopLocalVars() {
    signature.parseInitPopLocalVars();
}

// instantiates arguments and sets up the argv variable
int UserVariantBase::setupCall(CodeEvaluationHelper *ceh, ReferenceHolder<QoreListNode> &argv, ExceptionSink* xsink) const {
    QoreListNodeEvalOptionalRefHolder* args = ceh ? &ceh->getArgHolder() : nullptr;

    unsigned num_args = args ? args->size() : 0;
    // instantiate local vars from param list
    unsigned num_params = signature.numParams();

    for (unsigned i = 0; i < num_params; ++i) {
        QoreValue np;
        if (args && *args) {
            if (args->canEdit()) {
                signature.lv[i]->instantiate(qore_list_private::get(***args)->takeExists(i));
            } else {
                signature.lv[i]->instantiate((*args)->retrieveEntry(i).refSelf());
            }
            continue;
        }

        //printd(5, "UserVariantBase::setupCall() eval %d: instantiating param lvar %p ('%s') (exp nt: %d '%s')\n", i, signature.lv[i], signature.lv[i]->getName(), np.getType(), np.getTypeName());

        signature.lv[i]->instantiate(QoreValue());
    }

    // if there are more arguments than parameters
    printd(5, "UserVariantBase::setupCall() params: %d args: %d\n", num_params, num_args);

    if (num_params < num_args) {
        argv = new QoreListNode(autoTypeInfo);

        for (unsigned i = 0; i < (num_args - num_params); i++) {
            // here we try to take the reference from args if possible
            if (args->canEdit()) {
                argv->push(qore_list_private::get(***args)->takeExists(i + num_params), nullptr);
            } else {
                QoreValue n;
                if (args)
                    n = (*args)->retrieveEntry(i + num_params);
                //AbstractQoreNode* n = args ? const_cast<AbstractQoreNode*>(args->get_referenced_entry(i + num_params)) : 0;
                argv->push(n.refSelf(), nullptr);
            }
        }
    }

    return 0;
}

QoreValue UserVariantBase::evalIntern(ReferenceHolder<QoreListNode> &argv, QoreObject *self, ExceptionSink* xsink) const {
    QoreValue val;
    if (statements) {
        // self might be 0 if instantiated by a constructor call
        if (self && signature.selfid)
            signature.selfid->instantiateSelf(self);

        // instantiate argv and push id on stack
        signature.argvid->instantiate(argv ? argv->refSelf() : nullptr);

        {
            ArgvContextHelper argv_helper(argv.release(), xsink);

            // enter gate if necessary
            if (!gate || (gate->enter(xsink) >= 0)) {
                // execute function
                val = statements->exec(xsink);

                // exit gate if necessary
                if (gate) {
                    gate->exit();
                }
            }
        }

        // uninstantiate argv
        signature.argvid->uninstantiate(xsink);

        // if self then uninstantiate
        // self might be 0 if instantiated by a constructor call
        if (self && signature.selfid)
            signature.selfid->uninstantiateSelf();
    } else {
        argv = nullptr; // dereference argv now
    }

    // if return value is NOTHING; make sure it's valid; maybe there wasn't a return statement
    // only check if there isn't an active exception
    if (!*xsink && val.isNothing()) {
        const QoreTypeInfo* rt = signature.getReturnTypeInfo();

        // check return type
        QoreTypeInfo::acceptAssignment(rt, "<block return>", val, xsink, nullptr);
        // issue #3255: make sure any exception reflects the signature location
        if (*xsink) {
            xsink->overrideLocation(*signature.getParseLocation());
            xsink->appendLastDescription(": block missing return statement");
        }
    }

    return val;
}

// primary function for executing user code
QoreValue UserVariantBase::eval(const char* name, CodeEvaluationHelper* ceh, QoreObject *self, ExceptionSink* xsink, const qore_class_private* qc) const {
   QORE_TRACE("UserVariantBase::eval()");
   //printd(5, "UserVariantBase::eval() this: %p '%s()' args: %p (size: %d) self: %p class: %p '%s'\n", this, name, ceh ? ceh->getArgs() : 0, ceh && ceh->getArgs() ? ceh->getArgs()->size() : 0, self, qc, qc ? qc->name.c_str() : "n/a");

   assert(!self || (ceh ? ceh->getClass() : qc));

   // UserVariantExecHelper sets the Program thread context
   UserVariantExecHelper uveh(this, ceh, xsink);
   if (!uveh)
      return QoreValue();

   CodeContextHelper cch(xsink, CT_USER, name, self, qc ? qc : (ceh ? ceh->getClass() : nullptr));

   return evalIntern(uveh.getArgv(), self, xsink);
}

void UserVariantBase::parseCommit() {
    if (statements)
        statements->parseCommit(getProgram());
}

int QoreFunction::parseCheckDuplicateSignatureCommitted(UserSignature* sig) {
    const AbstractFunctionSignature* vs = 0;
    int rc = parseCompareResolvedSignature(vlist, sig, vs);
    if (rc == QTI_NOT_EQUAL)
        return 0;

    if (rc == QTI_AMBIGUOUS || rc == QTI_WILDCARD)
        ambiguousDuplicateSignatureException(className(), getName(), vs, sig);
    else
        duplicateSignatureException(className(), getName(), sig);
    return -1;
}

// returns 0 for OK, -1 for error
// this is called after types have been resolved and the types must be rechecked
int QoreFunction::parseCompareResolvedSignature(const VList& vlist, const AbstractFunctionSignature* sig, const AbstractFunctionSignature*& vs) {
   unsigned vp = sig->getParamTypes();

   // now check already-committed variants
   for (vlist_t::const_iterator i = vlist.begin(), e = vlist.end(); i != e; ++i) {
      vs = (*i)->getSignature();
      // get the minimum number of parameters with type information that need to match
      unsigned mp = vs->getMinParamTypes();
      // get number of parameters with type information
      unsigned tp = vs->getParamTypes();

      // shortcut: if the two variants have different numbers of parameters with type information, then they do not match
      if (vp < mp || vp > tp)
         continue;

      bool dup = true;
      bool ambiguous = false;
      unsigned max = QORE_MAX(tp, vp);
      for (unsigned pi = 0; pi < max; ++pi) {
         const QoreTypeInfo* variantTypeInfo = vs->getParamTypeInfo(pi);
         bool variantHasDefaultArg = vs->hasDefaultArg(pi);

         const QoreTypeInfo* typeInfo = sig->getParamTypeInfo(pi);
         assert(!sig->getParseParamTypeInfo(pi));
         bool thisHasDefaultArg = sig->hasDefaultArg(pi);

         // check for ambiguous matches
         if (typeInfo) {
            //printd(5, "QoreFunction::parseCompareResolvedSignature() this: sig: '%s' vti: %s ti: %s ident: %d\n", vs->getSignatureText(), QoreTypeInfo::getName(variantTypeInfo), QoreTypeInfo::getName(typeInfo), QoreTypeInfo::isInputIdentical(typeInfo, variantTypeInfo));

            if (!QoreTypeInfo::hasType(variantTypeInfo) && thisHasDefaultArg)
               ambiguous = true;
            else if (!QoreTypeInfo::isInputIdentical(typeInfo, variantTypeInfo)) {
               dup = false;
               break;
            }
         }
         else {
            if (QoreTypeInfo::hasType(variantTypeInfo) && variantHasDefaultArg)
               ambiguous = true;
            else if (!QoreTypeInfo::isInputIdentical(typeInfo, variantTypeInfo)) {
               dup = false;
               break;
            }
         }
      }
      if (dup)
         return ambiguous ? QTI_AMBIGUOUS : QTI_IDENT;
   }
   return QTI_NOT_EQUAL;
}

int QoreFunction::parseCheckDuplicateSignature(AbstractQoreFunctionVariant* variant) {
    UserSignature* sig = reinterpret_cast<UserSignature*>(variant->getSignature());

    // check for duplicate parameter signatures
    unsigned vnp = sig->numParams();
    unsigned vtp = sig->getParamTypes();
    unsigned vmp = sig->getMinParamTypes();

    // check all variants
    for (vlist_t::iterator i = vlist.begin(), e = vlist.end(); i != e; ++i) {
        UserSignature* vs = reinterpret_cast<UserSignature*>((*i)->getSignature());
        assert(!vs->resolved);
        // get the minimum number of parameters with type information that need to match
        unsigned mp = vs->getMinParamTypes();
        // get number of parameters with type information
        unsigned tp = vs->getParamTypes();

        //printd(5, "QoreFunction::parseCheckDuplicateSignature() adding %s(%s) checking %s(%s) vmp: %d vtp: %d mp: %d tp: %d\n", getName(), sig->getSignatureText(), getName(), vs->getSignatureText(), vmp, vtp, mp, tp);

        // shortcut: if the two variants have different numbers of parameters with type information, then they do not match
        if (vmp > tp || vtp < mp)
            continue;

        // the 2 signatures have the same number of parameters with type information
        if (!tp) {
            duplicateSignatureException(className(), getName(), vs);
            return -1;
        }

        unsigned np = vs->numParams();

        bool dup = true;
        bool ambiguous = false;
        bool recheck = false;
        unsigned max = QORE_MAX(np, vnp);
        for (unsigned pi = 0; pi < max; ++pi) {
            const QoreTypeInfo* variantTypeInfo = vs->getParamTypeInfo(pi);
            const QoreParseTypeInfo* variantParseTypeInfo = variantTypeInfo ? nullptr : vs->getParseParamTypeInfo(pi);
            bool variantHasDefaultArg = vs->hasDefaultArg(pi);

            const QoreTypeInfo* typeInfo = sig->getParamTypeInfo(pi);
            const QoreParseTypeInfo* parseTypeInfo = typeInfo ? nullptr : sig->getParseParamTypeInfo(pi);
            bool thisHasDefaultArg = sig->hasDefaultArg(pi);

            // FIXME: this is a horribly-complicated if/then/else structure
            //printd(5, "QoreFunction::parseCheckDuplicateSignature() ti: '%s' pti: '%s' vti: '%s' vpti: '%s' ident: %d\n", QoreTypeInfo::getName(typeInfo), QoreParseTypeInfo::getName(parseTypeInfo), QoreTypeInfo::getName(variantTypeInfo), QoreParseTypeInfo::getName(variantParseTypeInfo), QoreTypeInfo::isInputIdentical(typeInfo, variantTypeInfo));

            // check for ambiguous matches
            if (typeInfo || parseTypeInfo) {
                if (!QoreTypeInfo::hasType(variantTypeInfo) && !variantParseTypeInfo && thisHasDefaultArg)
                    ambiguous = true;
                else {
                    // check for real matches
                    if (typeInfo) {
                        if (variantTypeInfo) {
                            if (!QoreTypeInfo::isInputIdentical(typeInfo, variantTypeInfo)) {
                                dup = false;
                                break;
                            }
                        } else if (!QoreParseTypeInfo::parseStageOneIdenticalWithParsed(variantParseTypeInfo, typeInfo, recheck)) {
                            dup = false;
                            break;
                        }
                    } else {
                        if (variantTypeInfo) {
                            if (!QoreParseTypeInfo::parseStageOneIdenticalWithParsed(parseTypeInfo, variantTypeInfo, recheck)) {
                                dup = false;
                                break;
                            }
                        } else if (!QoreParseTypeInfo::parseStageOneIdentical(parseTypeInfo, variantParseTypeInfo, recheck)) {
                            dup = false;
                            break;
                        }
                    }
                }
            } else {
                if ((QoreTypeInfo::hasType(variantTypeInfo) || variantParseTypeInfo) && variantHasDefaultArg)
                    ambiguous = true;
                else if (variantTypeInfo) {
                    if (!QoreTypeInfo::isInputIdentical(typeInfo, variantTypeInfo)) {
                        dup = false;
                        break;
                    }
                } else if (!QoreParseTypeInfo::parseStageOneIdenticalWithParsed(variantParseTypeInfo, typeInfo, recheck)) {
                    dup = false;
                    break;
                }
            }
            //printd(5, "QoreFunction::parseCheckDuplicateSignature() %s(%s) == %s(%s) i: %d: %s <=> %s dup: %d\n", getName(), sig->getSignatureText(), getName(), vs->getSignatureText(), pi, QoreTypeInfo::getName(typeInfo), QoreTypeInfo::getName(variantTypeInfo), dup);
        }
        if (dup) {
            if (ambiguous)
                ambiguousDuplicateSignatureException(className(), getName(), vs, sig);
            else
                duplicateSignatureException(className(), getName(), sig);
            return -1;
        }
        if (recheck)
            variant->setRecheck();
    }

    return 0;
}

AbstractFunctionSignature* QoreFunction::parseGetUniqueSignature() const {
    if (vlist.singular()) {
        const UserVariantBase* uvb = first()->getUserVariantBase();
        if (uvb) {
            UserSignature* sig = uvb->getUserSignature();
            sig->resolve();
            return sig;
        }
        return first()->getSignature();
    }

    return nullptr;
}

void QoreFunction::resolvePendingSignatures() {
    if (!check_parse) {
        return;
    }

    const QoreTypeInfo* ti = nullptr;

    for (vlist_t::iterator i = vlist.begin(), e = vlist.end(); i != e; ++i) {
        UserVariantBase* uvb = (*i)->getUserVariantBase();
        if (!uvb) {
            continue;
        }

        UserSignature* sig = uvb->getUserSignature();
        sig->resolve();

        if (same_return_type) {
            const QoreTypeInfo* st = sig->getReturnTypeInfo();
            if (i != vlist.begin() && !QoreTypeInfo::isInputIdentical(st, ti))
                same_return_type = false;
            ti = st;
        }
    }
}

int QoreFunction::addPendingVariant(AbstractQoreFunctionVariant* variant) {
    if (!vlist.empty() && parse_init_done) {
        UserSignature* sig = reinterpret_cast<UserSignature*>(variant->getSignature());
        const char* cname = className();
        const char* name = getName();
        parse_error(*sig->getParseLocation(), "variant %s%s%s(%s) cannot be added to an existing function", cname ? cname : "", cname ? "::" : ""
, name, sig->getSignatureText());
        variant->deref();
        return -1;
    }

    parse_rt_done = false;
    parse_init_done = false;
    if (!check_parse) {
        check_parse = true;
    }

    // check for duplicate signature with existing variants
    if (parseCheckDuplicateSignature(variant)) {
        variant->deref();
        return -1;
    }

    vlist.push_back(variant);

    return 0;
}

void QoreFunction::parseCommit() {
    if (!check_parse) {
        return;
    }
    check_parse = false;

    parseCheckReturnType();

    for (vlist_t::iterator i = vlist.begin(), e = vlist.end(); i != e; ++i) {
        if ((*i)->isUser()) {
            if (!has_pub && (*i)->isModulePublic()) {
                has_pub = true;
                if (all_priv) {
                    all_priv = false;
                }
            }
            if (!has_user)
                has_user = true;
        }
        else if (!has_builtin) {
            has_builtin = true;
            if (all_priv) {
                all_priv = false;
            }
        }

        (*i)->parseCommit();
    }

    parse_rt_done = true;
    parse_init_done = true;
}

void QoreFunction::parseRollback() {
    // noop: object will be destroyed
    /*
    if (!same_return_type)
        same_return_type = true;

    parse_rt_done = true;
    parse_init_done = true;

    if (check_parse) {
        check_parse = false;
    }
    */
}

void QoreFunction::parseInit(qore_ns_private* ns) {
    if (parse_init_done || parse_init_in_progress) {
        return;
    }
    parse_init_in_progress = true;

    if (check_parse) {
        OptionalNamespaceParseContextHelper pch(ns);

        for (vlist_t::iterator i = vlist.begin(), e = vlist.end(); i != e; ++i) {
            (*i)->parseInit(this);
        }
    }
    parse_init_done = true;
}

QoreValue UserClosureFunction::evalClosure(const QoreClosureBase& closure_base, QoreProgram* pgm, const QoreListNode* args, QoreObject *self, const qore_class_private* class_ctx, ExceptionSink* xsink) const {
    // closures cannot be overloaded
    assert(vlist.singular());

    const AbstractQoreFunctionVariant* variant = first();

    // setup call, save runtime position
    // issue #1303: do not check for object validity here in the call, we already have a weak reference to the object,
    // so it will stay valid, if the closure code itself refers to the object, it will fail then if the object is invalid
    CodeEvaluationHelper ceh(xsink, this, variant, "<anonymous closure>", args, 0, class_ctx, CT_USER);
    if (*xsink)
        return QoreValue();

    ThreadSafeLocalVarRuntimeEnvironmentHelper ch(&closure_base);

    //printd(5, "UserClosureFunction::evalClosure() this: %p (%s) variant: %p args: %p self: %p\n", this, getName(), variant, args, self);
    return UCLOV_const(variant)->evalClosure(ceh, self, xsink);
}

void UserFunctionVariant::parseInit(QoreFunction* f) {
    signature.resolve();

    // resolve and push current return type on stack
    ParseCodeInfoHelper rtih(f->getName(), signature.getReturnTypeInfo());

    // set implicit argv arg type as unknown
    ParseImplicitArgTypeHelper pia(nullptr);

    // can (and must) be called even if statements is NULL
    statements->parseInit(this);

    // recheck types against committed types if necessary
    if (recheck)
        f->parseCheckDuplicateSignatureCommitted(&signature);
}

void UserClosureVariant::parseInit(QoreFunction* f) {
    UserClosureFunction* cf = static_cast<UserClosureFunction*>(f);

    signature.resolve();

    // resolve and push current return type on stack
    ParseCodeInfoHelper rtih(f->getName(), signature.getReturnTypeInfo());

    statements->parseInitClosure(this, cf);

    // only one variant is possible, no need to recheck types
}
