/*
    QoreCastOperatorNode.cpp

    Qore Programming Language

    Copyright (C) 2003 - 2021 Qore Technologies, s.r.o.

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
#include "qore/intern/QoreNamespaceIntern.h"
#include "qore/intern/QoreClassIntern.h"
#include "qore/intern/typed_hash_decl_private.h"
#include "qore/intern/QoreHashNodeIntern.h"
#include "qore/intern/qore_list_private.h"

QoreString QoreParseCastOperatorNode::cast_str("cast operator expression");

// if del is true, then the returned QoreString* should be deleted, if false, then it must not be
QoreString* QoreParseCastOperatorNode::getAsString(bool& del, int foff, ExceptionSink* xsink) const {
    del = false;
    return &cast_str;
}

int QoreParseCastOperatorNode::getAsString(QoreString& str, int foff, ExceptionSink* xsink) const {
    str.concat(&cast_str);
    return 0;
}

int QoreParseCastOperatorNode::parseInitImpl(QoreValue& val, QoreParseContext& parse_context) {
    assert(!parse_context.typeInfo);

    int err = parse_init_value(exp, parse_context);
    //printd(5, "QoreParseCastOperatorNode::parseInitImp() this: %p exp: %s (err: %d)\n", this, exp.getFullTypeName(),
    //    err);

    const QoreTypeInfo* expTypeInfo = parse_context.typeInfo;

    // issue #3331: ignore nothing if it's an "or nothing" cast, or if broken-cast is in effect
    bool or_nothing = (pti->or_nothing || (getProgram()->getParseOptions64() & PO_BROKEN_CAST));
    if (!exp && or_nothing) {
        ReferenceHolder<> holder(this, nullptr);
        val = QoreValue();
        return 0;
    }

    // check special cases
    if (pti->cscope->size() == 1 && pti->subtypes.empty()) {
        const char* type_str = pti->cscope->ostr;
        // check special case of cast<object>(...)
        if (!strcmp(type_str, "object")) {
            // if the class is "object", then set qc = nullptr to use as a catch-all and generic "cast to object"
            if (QoreTypeInfo::parseReturns(expTypeInfo, NT_OBJECT) == QTI_NOT_EQUAL) {
                // issue #3331: ignore nothing if it's an "or nothing" cast, or if broken-cast is in effect
                if (!or_nothing || QoreTypeInfo::parseReturns(expTypeInfo, NT_NOTHING) == QTI_NOT_EQUAL) {
                    parse_error(*loc, "cast<object>(%s) is invalid; cannot cast from %s to object",
                        QoreTypeInfo::getName(expTypeInfo), QoreTypeInfo::getName(expTypeInfo));
                    err = -1;
                }
            }
            parse_context.typeInfo = objectTypeInfo;
            if (exp) {
                ReferenceHolder<> holder(this, nullptr);
                val = new QoreClassCastOperatorNode(loc, nullptr, takeExp(), or_nothing);
            }
            // parse exception already raised; current expression invalid
            return err;
        }
        // check special case of cast<hash>(...)
        if (!strcmp(type_str, "hash")) {
            if (QoreTypeInfo::parseReturns(expTypeInfo, NT_HASH) == QTI_NOT_EQUAL) {
                // issue #3331: ignore nothing if it's an "or nothing" cast, or if broken-cast is in effect
                if (!or_nothing || QoreTypeInfo::parseReturns(expTypeInfo, NT_NOTHING) == QTI_NOT_EQUAL) {
                    parse_error(*loc, "cast<hash>(%s) is invalid; cannot cast from %s to hash",
                        QoreTypeInfo::getName(expTypeInfo), QoreTypeInfo::getName(expTypeInfo));
                    err = -1;
                }
            }
            parse_context.typeInfo = hashTypeInfo;
            if (exp) {
                ReferenceHolder<> holder(this, nullptr);
                val = new QoreHashDeclCastOperatorNode(loc, nullptr, takeExp(), or_nothing);
            }
            // parse exception already raised; current expression invalid
            return err;
        }
        // check special case of cast<list>(...)
        if (!strcmp(type_str, "list")) {
            // check if expression can return a list
            if (QoreTypeInfo::parseReturns(expTypeInfo, NT_LIST) == QTI_NOT_EQUAL) {
                // issue #3331: ignore nothing if it's an "or nothing" cast, or if broken-cast is in effect
                if (!or_nothing || QoreTypeInfo::parseReturns(expTypeInfo, NT_NOTHING) == QTI_NOT_EQUAL) {
                    parse_error(*loc, "cast<list>(%s) is invalid; cannot cast from %s to list",
                        QoreTypeInfo::getName(expTypeInfo), QoreTypeInfo::getName(expTypeInfo));
                    err = -1;
                }
            }
            parse_context.typeInfo = listTypeInfo;
            if (exp) {
                ReferenceHolder<> holder(this, nullptr);
                val = new QoreComplexListCastOperatorNode(loc, nullptr, takeExp(), or_nothing);
            }
            // parse exception already raised; current expression invalid
            return err;
        }
    }

    parse_context.typeInfo = QoreParseTypeInfo::resolveAndDelete(pti, loc, err);
    pti = nullptr;

    {
        const QoreClass* qc = or_nothing
            ? QoreTypeInfo::getReturnClass(parse_context.typeInfo)
            : QoreTypeInfo::getUniqueReturnClass(parse_context.typeInfo);
        if (qc) {
            // issue #3331: ignore nothing if it's an "or nothing" cast, or if broken-cast is in effect
            // issue #4113: ensure that objects will be subject to runtime checks
            if ((QoreTypeInfo::parseReturns(expTypeInfo, qc) == QTI_NOT_EQUAL)
                && (!QoreTypeInfo::parseAccepts(expTypeInfo, objectTypeInfo))
                && (!or_nothing || QoreTypeInfo::parseReturns(expTypeInfo, NT_NOTHING) == QTI_NOT_EQUAL)) {
                parse_error(*loc, "cast<%s>(%s) is invalid; cannot cast from %s to %s",
                    QoreTypeInfo::getName(parse_context.typeInfo), QoreTypeInfo::getName(expTypeInfo),
                    QoreTypeInfo::getName(expTypeInfo), QoreTypeInfo::getName(parse_context.typeInfo));
                err = -1;
            } else {
                assert(exp);
                ReferenceHolder<> holder(this, nullptr);
                val = new QoreClassCastOperatorNode(loc, nullptr, takeExp(), or_nothing);
            }
            return err;
        }
    }

    {
        const TypedHashDecl* hd = or_nothing
            ? QoreTypeInfo::getTypedHash(parse_context.typeInfo)
            : QoreTypeInfo::getUniqueReturnHashDecl(parse_context.typeInfo);
        if (hd) {
            const_cast<typed_hash_decl_private*>(typed_hash_decl_private::get(*hd))->parseInit();

            bool runtime_check = false;
            typed_hash_decl_private::get(*hd)->parseCheckHashDeclInitialization(loc, expTypeInfo, exp,
                "cast<> operation", runtime_check, false);

            qore_type_result_e r = QoreTypeInfo::parseReturns(expTypeInfo, NT_HASH);
            if (r == QTI_NOT_EQUAL) {
                // issue #3331: ignore nothing if it's an "or nothing" cast, or if broken-cast is in effect
                if (!or_nothing || QoreTypeInfo::parseReturns(expTypeInfo, NT_NOTHING) == QTI_NOT_EQUAL) {
                    parse_error(*loc, "cast<%s>(%s) is invalid; cannot cast from %s to (hashdecl) %s",
                        QoreTypeInfo::getName(parse_context.typeInfo), QoreTypeInfo::getName(expTypeInfo),
                        QoreTypeInfo::getName(expTypeInfo), QoreTypeInfo::getName(parse_context.typeInfo));
                    return -1;
                }
            }

            parse_context.typeInfo = hd->getTypeInfo();
            if (exp) {
                ReferenceHolder<> holder(this, nullptr);
                val = new QoreHashDeclCastOperatorNode(loc, hd, takeExp(), or_nothing);
                return err;
            }
        }
    }

    {
        const QoreTypeInfo* ti = or_nothing
            ? QoreTypeInfo::getComplexHashValueType(parse_context.typeInfo)
            : QoreTypeInfo::getUniqueReturnComplexHash(parse_context.typeInfo);
        if (ti) {
            // check for cast<> compatibility
            qore_hash_private::parseCheckComplexHashInitialization(loc, ti, expTypeInfo, exp, "cast to", false);

            qore_type_result_e r = QoreTypeInfo::parseReturns(expTypeInfo, NT_HASH);
            if (r == QTI_NOT_EQUAL) {
                // issue #3331: ignore nothing if it's an "or nothing" cast, or if broken-cast is in effect
                if (!or_nothing || QoreTypeInfo::parseReturns(expTypeInfo, NT_NOTHING) == QTI_NOT_EQUAL) {
                    parse_error(*loc, "cast<%s>(%s) is invalid; cannot cast from %s to hash<string, %s>",
                        QoreTypeInfo::getName(parse_context.typeInfo), QoreTypeInfo::getName(expTypeInfo),
                        QoreTypeInfo::getName(expTypeInfo), QoreTypeInfo::getName(ti));
                    return -1;
                }
            }

            if (exp) {
                ReferenceHolder<> holder(this, nullptr);
                val = new QoreComplexHashCastOperatorNode(loc, parse_context.typeInfo, takeExp(), or_nothing);
                return err;
            }
        }
    }

    {
        const QoreTypeInfo* ti = or_nothing
            ? QoreTypeInfo::getComplexListValueType(parse_context.typeInfo)
            : QoreTypeInfo::getUniqueReturnComplexList(parse_context.typeInfo);
        if (ti) {
            // check for cast<> compatibility
            qore_list_private::parseCheckComplexListInitialization(loc, ti, expTypeInfo, exp, "cast to", false);

            // check arg type compatibility with list if the type is not a softlist
            if (!QoreTypeInfo::getUniqueReturnComplexSoftList(parse_context.typeInfo)) {
                qore_type_result_e r = QoreTypeInfo::parseReturns(expTypeInfo, NT_LIST);
                if (r == QTI_NOT_EQUAL) {
                    // issue #3331: ignore nothing if it's an "or nothing" cast, or if broken-cast is in effect
                    if (!or_nothing || QoreTypeInfo::parseReturns(expTypeInfo, NT_NOTHING) == QTI_NOT_EQUAL) {
                        parse_error(*loc, "cast<%s>(%s) is invalid; cannot cast from %s to %s",
                            QoreTypeInfo::getName(parse_context.typeInfo), QoreTypeInfo::getName(expTypeInfo),
                            QoreTypeInfo::getName(expTypeInfo), QoreTypeInfo::getName(parse_context.typeInfo));
                        return -1;
                    }
                }
            }

            if (exp) {
                ReferenceHolder<> holder(this, nullptr);
                val = new QoreComplexListCastOperatorNode(loc, parse_context.typeInfo, takeExp(), or_nothing);
                return err;
            }
        }
    }

    parse_error(*loc, "cannot cast<> to type '%s'", QoreTypeInfo::getName(parse_context.typeInfo));
    return -1;
}

QoreValue QoreClassCastOperatorNode::evalImpl(bool& needs_deref, ExceptionSink* xsink) const {
    ValueEvalRefHolder rv(exp, xsink);
    if (*xsink)
        return QoreValue();

    // issue #3331: ignore nothing if it's an "or nothing" cast, or if broken-cast is in effect
    if (rv->isNothing() && or_nothing) {
        return QoreValue();
    }

    if (rv->getType() != NT_OBJECT) {
        xsink->raiseException("RUNTIME-CAST-ERROR", "cannot cast from type '%s' to %s'%s'", rv->getTypeName(),
            qc ? "class " : "", qc ? qc->getName() : "object");
        return QoreValue();
    }

    const QoreObject* obj = rv->get<const QoreObject>();
    if (qc) {
        const QoreClass* oc = obj->getClass();
        bool priv;
        const QoreClass* tc = oc->getClass(*qc, priv);
        if (!tc) {
            xsink->raiseException("RUNTIME-CAST-ERROR", "cannot cast from class '%s' to class '%s'",
                obj->getClassName(), qc->getName());
            return QoreValue();
        }
        //printd(5, "QoreCastOperatorNode::evalImpl() %s -> %s priv: %d\n", oc->getName(), tc->getName(), priv);
        if (priv && !qore_class_private::runtimeCheckPrivateClassAccess(*tc)) {
            xsink->raiseException("RUNTIME-CAST-ERROR", "cannot cast from class '%s' to privately-accessible " \
                "class '%s' in this context", obj->getClassName(), qc->getName());
            return QoreValue();
        }
    }

    return rv.takeValue(needs_deref);
}

QoreValue QoreHashDeclCastOperatorNode::evalImpl(bool& needs_deref, ExceptionSink* xsink) const {
    ValueEvalRefHolder rv(exp, xsink);
    if (*xsink)
        return QoreValue();

    // issue #3331: ignore nothing if it's an "or nothing" cast, or if broken-cast is in effect
    if (rv->isNothing() && or_nothing) {
        return QoreValue();
    }

    if (rv->getType() != NT_HASH) {
        if (hd)
            xsink->raiseException("RUNTIME-CAST-ERROR", "cannot cast from type '%s' to hashdecl '%s'",
                rv->getTypeName(), hd->getName());
        else
            xsink->raiseException("RUNTIME-CAST-ERROR", "cannot cast from type '%s' to 'hash'", rv->getTypeName());
        return QoreValue();
    }

    const QoreHashNode* h = rv->get<const QoreHashNode>();

    const TypedHashDecl* vhd = h->getHashDecl();

    if (!hd) {
        if (!vhd && !h->getValueTypeInfo())
            return rv.takeValue(needs_deref);
        needs_deref = true;
        return qore_hash_private::getPlainHash(rv.takeReferencedNode<QoreHashNode>());
    }

    // if we already have the expected type, then there's nothing more to do
    if (vhd && typed_hash_decl_private::get(*vhd)->equal(*typed_hash_decl_private::get(*hd)))
        return rv.takeValue(needs_deref);

    // do the runtime cast
    return typed_hash_decl_private::get(*hd)->newHash(h, true, xsink);
}

QoreValue QoreComplexHashCastOperatorNode::evalImpl(bool& needs_deref, ExceptionSink* xsink) const {
    assert(needs_deref);
    ValueEvalRefHolder rv(exp, xsink);
    if (*xsink)
        return QoreValue();

    // issue #3331: ignore nothing if it's an "or nothing" cast, or if broken-cast is in effect
    if (rv->isNothing() && or_nothing) {
        return QoreValue();
    }

    if (rv->getType() != NT_HASH) {
        xsink->raiseException("RUNTIME-CAST-ERROR", "cannot cast from type '%s' to '%s'", rv->getTypeName(),
            QoreTypeInfo::getName(typeInfo));
        return QoreValue();
    }

    // do the runtime cast
    return qore_hash_private::newComplexHashFromHash(typeInfo, rv.takeReferencedNode<QoreHashNode>(), xsink);
}

QoreValue QoreComplexListCastOperatorNode::evalImpl(bool& needs_deref, ExceptionSink* xsink) const {
    assert(needs_deref);
    ValueEvalRefHolder rv(exp, xsink);
    if (*xsink)
        return QoreValue();

    // issue #3331: ignore nothing if it's an "or nothing" cast, or if broken-cast is in effect
    if (rv->isNothing() && or_nothing) {
        return QoreValue();
    }

    // check the value
    if ((!typeInfo || !QoreTypeInfo::getUniqueReturnComplexSoftList(typeInfo)) && (rv->getType() != NT_LIST)) {
        xsink->raiseException("RUNTIME-CAST-ERROR", "cannot cast from type '%s' to '%s'", rv->getTypeName(),
            typeInfo ? QoreTypeInfo::getName(typeInfo) : "list");
        return QoreValue();
    }

    // do the runtime cast
    if (!typeInfo) {
        return qore_list_private::getPlainList(rv.takeReferencedNode<QoreListNode>());
    }

    return qore_list_private::newComplexListFromValue(typeInfo, rv.takeReferencedValue(), xsink);
}
