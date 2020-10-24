/*
    QorePlusOperatorNode.cpp

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
#include "qore/intern/QoreObjectIntern.h"
#include "qore/intern/QoreHashNodeIntern.h"
#include "qore/intern/qore_list_private.h"

QoreString QorePlusOperatorNode::plus_str("+ operator expression");

QoreValue QorePlusOperatorNode::evalImpl(bool& needs_deref, ExceptionSink* xsink) const {
    ValueEvalRefHolder lh(left, xsink);
    if (*xsink)
        return QoreValue();
    ValueEvalRefHolder rh(right, xsink);
    if (*xsink)
        return QoreValue();

    qore_type_t lt = lh->getType();
    qore_type_t rt = rh->getType();

    if (lt == NT_LIST) {
        const QoreListNode* l = lh->get<const QoreListNode>();
        // issue #2791: perform type folding at the source
        if (rt == NT_LIST) {
            return qore_list_private::get(*l)->concatenate(rh->get<const QoreListNode>(), xsink);
        } else {
            return qore_list_private::get(*l)->concatenateElement(rh.takeReferencedValue(), xsink);
        }
    }

    if (rt == NT_LIST) {
        // issue #2791: perform type folding at the source
        return qore_list_private::get(*rh->get<const QoreListNode>())->prependElement(lh.takeReferencedValue(), xsink);
    }

    if (lt == NT_STRING) {
        QoreStringNodeHolder str(new QoreStringNode(*lh->get<const QoreStringNode>()));

        if (rt == NT_STRING)
            str->concat(rh->get<const QoreStringNode>(), xsink);
        else {
            QoreStringValueHelper r(*rh, str->getEncoding(), xsink);
            if (*xsink)
                return QoreValue();
            str->concat(*r, xsink);
        }
        return str.release();
    }

    if (rt == NT_STRING) {
        const QoreStringNode* r = rh->get<const QoreStringNode>();
        QoreStringNodeValueHelper strval(*lh, r->getEncoding(), xsink);
        if (*xsink)
            return QoreValue();
        SimpleRefHolder<QoreStringNode> str(strval->is_unique() && strval.is_temp() ? strval.getReferencedValue() : new QoreStringNode(**strval));
        assert(str->reference_count() == 1);

        QoreStringNode* rv = const_cast<QoreStringNode*>(*str);

        rv->concat(r, xsink);
        if (*xsink)
            return QoreValue();
        return str.release();
    }

    // issue #3157: try to handle timeout + date specially
    if (check_timeout_date_variant && lt == NT_INT && rt == NT_DATE) {
        int64 secs = lh->getAsBigInt() / 1000;
        int64 ms = lh->getAsBigInt() - (secs * 1000);
        DateTime l;
        l.setRelativeDateSeconds(secs, static_cast<int>(ms));
        return rh->get<DateTimeNode>()->add(l);
    }

    if (check_date_timeout_variant && lt == NT_DATE && rt == NT_INT) {
        int64 secs = rh->getAsBigInt() / 1000;
        int64 ms = rh->getAsBigInt() - (secs * 1000);
        DateTime r;
        r.setRelativeDateSeconds(secs, static_cast<int>(ms));
        return lh->get<DateTimeNode>()->add(r);
    }

    if (lt == NT_DATE || rt == NT_DATE) {
        DateTimeNodeValueHelper l(*lh);
        DateTimeValueHelper r(*rh);
        return l->add(*r);
    }

    if (lt == NT_NUMBER || rt == NT_NUMBER) {
        QoreNumberNodeHelper l(*lh);
        QoreNumberNodeHelper r(*rh);
        return l->doPlus(**r);
    }

    if (lt == NT_FLOAT || rt == NT_FLOAT) {
        return lh->getAsFloat() + rh->getAsFloat();
    }

    if (lt == NT_INT || rt == NT_INT) {
        return lh->getAsBigInt() + rh->getAsBigInt();
    }

    if (lt == NT_HASH) {
        const QoreHashNode* l = lh->get<const QoreHashNode>();
        if (rt == NT_HASH) {
            return qore_hash_private::get(*l)->plusEquals(rh->get<const QoreHashNode>(), xsink);
        }
        if (rt == NT_OBJECT) {
            QoreObject* r = rh->get<QoreObject>();
            ReferenceHolder<QoreHashNode> rv(qore_hash_private::get(*l)->copy(true), xsink);
            SafeDerefHelper sdh(xsink);
            qore_object_private::get(*r)->mergeDataToHash(*rv, sdh, xsink);
            if (*xsink)
                return QoreValue();

            return rv.release();
        }
        return l->refSelf();
    }

    if (lt == NT_OBJECT) {
        QoreObject* l = lh->get<QoreObject>();
        if (rt != NT_HASH)
            return l->refSelf();
        const QoreHashNode* r = rh->get<const QoreHashNode>();

        ReferenceHolder<QoreHashNode> h(qore_object_private::get(*l)->getRuntimeMemberHash(xsink), xsink);
        if (*xsink)
            return QoreValue();

        h->merge(r, xsink);
        if (*xsink)
            return QoreValue();

        return h.release();
    }

    if (rt == NT_HASH || rt == NT_OBJECT) {
        return rh.takeReferencedValue();
    }

    if (lt == NT_BINARY) {
        if (rt != NT_BINARY)
            return lh.takeReferencedValue();

        BinaryNode* rv = lh->get<const BinaryNode>()->copy();
        rv->append(rh->get<const BinaryNode>());
        return rv;
    }

    if (rt == NT_BINARY) {
        return rh.takeReferencedValue();
    }

    return QoreValue();
}

void QorePlusOperatorNode::parseInitImpl(QoreValue& val, LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& parseTypeInfo) {
    // turn off "reference ok" and "return value ignored" flags
    pflag &= ~(PF_RETURN_VALUE_IGNORED);

    assert(!parseTypeInfo);

    const QoreTypeInfo* leftTypeInfo = nullptr, *rightTypeInfo = nullptr;

    parse_init_value(left, oflag, pflag, lvids, leftTypeInfo);
    parse_init_value(right, oflag, pflag, lvids, rightTypeInfo);

    // see if both arguments are constants, then eval immediately and substitute this node with the result
    if (left.isValue() && right.isValue()) {
        SimpleRefHolder<QorePlusOperatorNode> del(this);
        ParseExceptionSink xsink;
        ValueEvalRefHolder rv(this, *xsink);
        val = rv.takeReferencedValue();
        parseTypeInfo = val.getTypeInfo();
        return;
    }

    // if either side is a list, then the return type is list (highest priority)
    bool is_list_left = QoreTypeInfo::isType(leftTypeInfo, NT_LIST);
    bool is_list_right = QoreTypeInfo::isType(rightTypeInfo, NT_LIST);
    if (is_list_left || is_list_right) {
        if (is_list_left && is_list_right) {
            returnTypeInfo = QoreTypeInfo::isOutputIdentical(leftTypeInfo, rightTypeInfo)
                ? leftTypeInfo
                : listTypeInfo;
        } else {
            returnTypeInfo = listTypeInfo;
        }
    }
    // otherwise only set return type if return types on both sides are known at parse time
    else if (QoreTypeInfo::hasType(leftTypeInfo) && QoreTypeInfo::hasType(rightTypeInfo)) {
        // issue #3157: try to handle timeout + date specially
        if (QoreTypeInfo::equal(leftTypeInfo, timeoutTypeInfo)
            && QoreTypeInfo::parseReturns(rightTypeInfo, NT_DATE)) {
            check_timeout_date_variant = true;
        } else if (QoreTypeInfo::equal(rightTypeInfo, timeoutTypeInfo)
            && QoreTypeInfo::parseReturns(leftTypeInfo, NT_DATE)) {
            check_date_timeout_variant = true;
        }

        if (QoreTypeInfo::isType(leftTypeInfo, NT_STRING) || QoreTypeInfo::isType(rightTypeInfo, NT_STRING)) {
            returnTypeInfo = stringTypeInfo;
        } else if (QoreTypeInfo::isType(leftTypeInfo, NT_DATE) || QoreTypeInfo::isType(rightTypeInfo, NT_DATE)) {
            returnTypeInfo = dateTypeInfo;
        } else if (QoreTypeInfo::isType(leftTypeInfo, NT_NUMBER) || QoreTypeInfo::isType(rightTypeInfo, NT_NUMBER)) {
            returnTypeInfo = numberTypeInfo;
        } else if (QoreTypeInfo::isType(leftTypeInfo, NT_FLOAT) || QoreTypeInfo::isType(rightTypeInfo, NT_FLOAT)) {
            returnTypeInfo = floatTypeInfo;
        } else if (QoreTypeInfo::isType(leftTypeInfo, NT_INT) || QoreTypeInfo::isType(rightTypeInfo, NT_INT)) {
            returnTypeInfo = bigIntTypeInfo;
        } else if (QoreTypeInfo::isType(leftTypeInfo, NT_HASH)) {
            if (QoreTypeInfo::isType(rightTypeInfo, NT_HASH)
                && QoreTypeInfo::isOutputIdentical(leftTypeInfo, rightTypeInfo)) {
                returnTypeInfo = leftTypeInfo;
            } else {
                returnTypeInfo = hashTypeInfo;
            }
        } else if (QoreTypeInfo::isType(leftTypeInfo, NT_OBJECT)) {
            returnTypeInfo = hashTypeInfo;
        } else if (QoreTypeInfo::isType(rightTypeInfo, NT_OBJECT)) {
            returnTypeInfo = objectTypeInfo;
        } else if (QoreTypeInfo::isType(leftTypeInfo, NT_BINARY) || QoreTypeInfo::isType(rightTypeInfo, NT_BINARY)) {
            returnTypeInfo = binaryTypeInfo;
        } else if (QoreTypeInfo::returnsSingle(leftTypeInfo) && QoreTypeInfo::returnsSingle(rightTypeInfo)) {
            // only return type nothing if both types are available and return a single type
            returnTypeInfo = nothingTypeInfo;
        }
    }

    if (returnTypeInfo) {
        parseTypeInfo = returnTypeInfo;
    }
}
