/*
    QoreMultiplicationOperatorNode.cpp

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

#include <qore/Qore.h>

QoreString QoreMultiplicationOperatorNode::multiplication_str("* operator expression");

QoreValue QoreMultiplicationOperatorNode::evalImpl(bool& needs_deref, ExceptionSink* xsink) const {
    ValueEvalRefHolder lh(left, xsink);
    if (*xsink)
        return QoreValue();
    ValueEvalRefHolder rh(right, xsink);
    if (*xsink)
        return QoreValue();

    qore_type_t lt = lh->getType();
    qore_type_t rt = rh->getType();

    if (lt == NT_NUMBER || rt == NT_NUMBER) {
        QoreNumberNodeHelper l(*lh);
        QoreNumberNodeHelper r(*rh);
        return l->doMultiply(**r);
    }

    if (lt == NT_FLOAT || rt == NT_FLOAT) {
        return lh->getAsFloat() * rh->getAsFloat();
    }

    if (lt == NT_INT || rt == NT_INT) {
        return lh->getAsBigInt() * rh->getAsBigInt();
    }

    return QoreValue();
}

void QoreMultiplicationOperatorNode::parseInitImpl(QoreValue& val, LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& parseTypeInfo) {
    // turn off "reference ok" and "return value ignored" flags
    pflag &= ~(PF_RETURN_VALUE_IGNORED);

    assert(!parseTypeInfo);

    const QoreTypeInfo* leftTypeInfo = 0, *rightTypeInfo = 0;

    parse_init_value(left, oflag, pflag, lvids, leftTypeInfo);
    parse_init_value(right, oflag, pflag, lvids, rightTypeInfo);

    // see if both arguments are constants, then eval immediately and substitute this node with the result
    if (right.isValue() && left.isValue()) {
        SimpleRefHolder<QoreMultiplicationOperatorNode> del(this);
        ParseExceptionSink xsink;
        ValueEvalRefHolder rv(this, *xsink);
        val = rv.takeReferencedValue();
        return;
    }

    // if either side is a float, then the return type is float (highest priority)
    if (QoreTypeInfo::isType(leftTypeInfo, NT_FLOAT) || QoreTypeInfo::isType(rightTypeInfo, NT_FLOAT))
        returnTypeInfo = floatTypeInfo;

    // otherwise only set return type if return types on both sides are known at parse time
    else if (QoreTypeInfo::hasType(leftTypeInfo) && QoreTypeInfo::hasType(rightTypeInfo)) {
        if (QoreTypeInfo::isType(leftTypeInfo, NT_INT) && QoreTypeInfo::isType(rightTypeInfo, NT_INT))
            returnTypeInfo = bigIntTypeInfo;
    }
}
