/*
    QorePostIncrementOperatorNode.cpp

    Qore Programming Language

    Copyright (C) 2003 - 2022 Qore Technologies, s.r.o.

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

QoreString QorePostIncrementOperatorNode::op_str("++ (post-increment) operator expression");

int QorePostIncrementOperatorNode::parseInitImpl(QoreValue& val, QoreParseContext& parse_context) {
    int err = parseInitIntern(op_str.c_str(), parse_context);

    // version for local var
    if ((parse_context.typeInfo == bigIntTypeInfo || parse_context.typeInfo == softBigIntTypeInfo)) {
        val = makeSpecialization<QoreIntPostIncrementOperatorNode>();
    }
    return err;
}

QoreValue QorePostIncrementOperatorNode::evalImpl(bool& needs_deref, ExceptionSink* xsink) const {
    // get ptr to current value (lvalue is locked for the scope of the LValueHelper object)
    LValueHelper n(exp, xsink);
    if (!n)
        return QoreValue();

    if (n.getType() == NT_NUMBER)
        return n.postIncrementNumber(ref_rv, "<-- (post) operator>");

    if (n.getType() == NT_FLOAT) {
        double f = n.postIncrementFloat("<++ (post) operator>");
        assert(!*xsink);
        return f;
    }

    return n.postIncrementBigInt("<++ (post) operator>");
}
