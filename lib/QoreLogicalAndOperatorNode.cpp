/*
    QoreLogicalAndOperatorNode.cpp

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

QoreString QoreLogicalAndOperatorNode::logical_and_str("logical and (&&) operator expression");

QoreValue QoreLogicalAndOperatorNode::evalValueImpl(bool& needs_deref, ExceptionSink* xsink) const {
    // if left side is 0, then do not evaluate right side (logical short circuiting)
    ValueEvalRefHolder lh(left, xsink);
    if (*xsink)
        return QoreValue();
    if (!lh->getAsBool()) {
        return false;
    }

    ValueEvalRefHolder rh(right, xsink);
    if (*xsink)
        return QoreValue();
    return rh->getAsBool();
}

AbstractQoreNode *QoreLogicalAndOperatorNode::parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
    pflag &= ~PF_RETURN_VALUE_IGNORED;

    typeInfo = boolTypeInfo;

    const QoreTypeInfo *lti = 0, *rti = 0;

    parse_init_value(left, oflag, pflag, lvids, lti);
    parse_init_value(right, oflag, pflag, lvids, rti);

    // see if both arguments are constants, then eval immediately and substitute this node with the result
    if (left.isValue() && right.isValue()) {
        SimpleRefHolder<QoreLogicalAndOperatorNode> del(this);
        ParseExceptionSink xsink;
        ValueEvalRefHolder v(this, *xsink);
        assert(!**xsink);
        return v.takeReferencedValue().takeNode();
    }

    return this;
}
