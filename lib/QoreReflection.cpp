/* -*- indent-tabs-mode: nil -*- */
/*
    QoreReflection.cpp

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
#include "qore/intern/Function.h"
#include "qore/intern/ConstantList.h"
#include "qore/intern/QoreNamespaceIntern.h"

const char* get_access_string(ClassAccess access) {
    switch (access) {
        case Public: return "public";
        case Private: return "private";
        case Internal: return "private:internal";
        default: break;
    }
    assert(false);
    return nullptr;
}

const QoreClass* QoreExternalVariant::getClass() const {
    return reinterpret_cast<const AbstractQoreFunctionVariant*>(this)->getClass();
}

const char* QoreExternalVariant::getSignatureText() const {
    return reinterpret_cast<const AbstractQoreFunctionVariant*>(this)->getSignature()->getSignatureText();
}

int64 QoreExternalVariant::getCodeFlags() const {
    return reinterpret_cast<const AbstractQoreFunctionVariant*>(this)->getFlags();
}

bool QoreExternalVariant::isModulePublic() const {
    const QoreClass* cls = getClass();
    return cls
        ? cls->isModulePublic()
        : reinterpret_cast<const AbstractQoreFunctionVariant*>(this)->isModulePublic();
}

bool QoreExternalVariant::isSynchronized() const {
    const UserVariantBase* uvb = reinterpret_cast<const AbstractQoreFunctionVariant*>(this)->getUserVariantBase();
    return uvb && uvb->isSynchronized();
}

bool QoreExternalVariant::isBuiltin() const {
    return !reinterpret_cast<const AbstractQoreFunctionVariant*>(this)->isUser();
}

bool QoreExternalVariant::hasBody() const {
    return reinterpret_cast<const AbstractQoreFunctionVariant*>(this)->hasBody();
}

int64 QoreExternalVariant::getDomain() const {
    int64 rc = reinterpret_cast<const AbstractQoreFunctionVariant*>(this)->getFunctionality();
    const QoreClass* cls = getClass();
    if (cls) {
        rc |= cls->getDomain64();
    }

    return rc;
}

unsigned QoreExternalVariant::numParams() const {
    return reinterpret_cast<const AbstractQoreFunctionVariant*>(this)->numParams();
}

const QoreTypeInfo* QoreExternalVariant::getReturnTypeInfo() const {
    return reinterpret_cast<const AbstractQoreFunctionVariant*>(this)->getReturnTypeInfo();
}

const type_vec_t& QoreExternalVariant::getParamTypeList() const {
    return reinterpret_cast<const AbstractQoreFunctionVariant*>(this)->getSignature()->getTypeList();
}

const arg_vec_t& QoreExternalVariant::getDefaultArgList() const {
    return reinterpret_cast<const AbstractQoreFunctionVariant*>(this)->getSignature()->getDefaultArgList();
}

const name_vec_t& QoreExternalVariant::getParamNames() const {
    return reinterpret_cast<const AbstractQoreFunctionVariant*>(this)->getSignature()->getParamNames();
}

const QoreExternalProgramLocation* QoreExternalVariant::getSourceLocation() const {
    const UserVariantBase* uvb = reinterpret_cast<const AbstractQoreFunctionVariant*>(this)->getUserVariantBase();
    return reinterpret_cast<const QoreExternalProgramLocation*>(uvb ? uvb->getUserSignature()->getParseLocation() : &loc_builtin);
}

const QoreMethod* QoreExternalMethodVariant::getMethod() const {
    return reinterpret_cast<const MethodVariantBase*>(this)->method();
}

bool QoreExternalMethodVariant::isAbstract() const {
    return reinterpret_cast<const MethodVariantBase*>(this)->isAbstract();
}

bool QoreExternalMethodVariant::isFinal() const {
    return reinterpret_cast<const MethodVariantBase*>(this)->isFinal();
}

bool QoreExternalMethodVariant::isStatic() const {
    return getMethod()->isStatic();
}

ClassAccess QoreExternalMethodVariant::getAccess() const {
    return reinterpret_cast<const MethodVariantBase*>(this)->getAccess();
}

const char* QoreExternalMethodVariant::getAccessString() const {
    return get_access_string(getAccess());
}

const QoreTypeInfo* QoreExternalMemberVarBase::getTypeInfo() const {
    return reinterpret_cast<const QoreMemberInfoBase*>(this)->getTypeInfo();
}

ClassAccess QoreExternalMemberVarBase::getAccess() const {
    return reinterpret_cast<const QoreMemberInfoBaseAccess*>(this)->getAccess();
}

const char* QoreExternalMemberVarBase::getAccessString() const {
    return get_access_string(getAccess());
}

QoreValue QoreExternalMemberVarBase::getDefaultValue(ExceptionSink* xsink) const {
    return reinterpret_cast<const QoreMemberInfoBase*>(this)->exp.eval(xsink);
}

const QoreExternalProgramLocation* QoreExternalMemberVarBase::getSourceLocation() const {
    const QoreProgramLocation* loc = reinterpret_cast<const QoreMemberInfoBase*>(this)->loc;
    return reinterpret_cast<const QoreExternalProgramLocation*>(loc ? loc : &loc_builtin);
}

QoreValue QoreExternalStaticMember::getValue() const {
    return reinterpret_cast<const QoreVarInfo*>(this)->getReferencedValue();
}

int QoreExternalStaticMember::setValue(const QoreValue val, ExceptionSink* xsink) const {
    LValueHelper lvh(xsink);
    const_cast<QoreVarInfo*>(reinterpret_cast<const QoreVarInfo*>(this))->getLValue(lvh);
    lvh.assign(val.refSelf(), "<set static class member value>");
    return *xsink ? -1 : 0;
}

QoreHashNode* QoreExternalProgramLocation::getHash() const {
    return get_source_location(reinterpret_cast<const QoreProgramLocation*>(this));
}

const char* QoreExternalConstant::getName() const {
    return reinterpret_cast<const ConstantEntry*>(this)->name.c_str();
}

bool QoreExternalConstant::isModulePublic() const {
    return reinterpret_cast<const ConstantEntry*>(this)->pub;
}

bool QoreExternalConstant::isBuiltin() const {
    return reinterpret_cast<const ConstantEntry*>(this)->builtin;
}

const QoreTypeInfo* QoreExternalConstant::getTypeInfo() const {
    return reinterpret_cast<const ConstantEntry*>(this)->typeInfo;
}

QoreValue QoreExternalConstant::getReferencedValue() const {
    return reinterpret_cast<const ConstantEntry*>(this)->val.refSelf();
}

const QoreExternalProgramLocation* QoreExternalConstant::getSourceLocation() const {
    const QoreProgramLocation* loc = reinterpret_cast<const ConstantEntry*>(this)->loc;
    return reinterpret_cast<const QoreExternalProgramLocation*>(loc ? loc : &loc_builtin);
}

ClassAccess QoreExternalConstant::getAccess() const {
    return reinterpret_cast<const ConstantEntry*>(this)->getAccess();
}