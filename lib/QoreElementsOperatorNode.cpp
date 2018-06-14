/*
    QoreElementsOperatorNode.cpp

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
#include "qore/intern/qore_number_private.h"
#include "qore/intern/qore_program_private.h"

QoreString QoreElementsOperatorNode::Elements_str("elements operator expression");

// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
QoreString *QoreElementsOperatorNode::getAsString(bool& del, int foff, ExceptionSink* xsink) const {
    del = false;
    return &Elements_str;
}

int QoreElementsOperatorNode::getAsString(QoreString& str, int foff, ExceptionSink* xsink) const {
    str.concat(&Elements_str);
    return 0;
}

QoreValue QoreElementsOperatorNode::evalValueImpl(bool& needs_deref, ExceptionSink* xsink) const {
    ValueEvalRefHolder v(exp, xsink);
    if (*xsink)
        return QoreValue();

    switch (v->getType()) {
        case NT_LIST: return v->get<const QoreListNode>()->size();
        // return the number of characters in a string (not bytes)
        case NT_STRING: return v->get<const QoreStringNode>()->length();
        case NT_HASH: return v->get<const QoreHashNode>()->size();
        case NT_OBJECT: return v->get<const QoreObject>()->size(xsink);
        case NT_BINARY: return v->get<const BinaryNode>()->size();
    }

    return 0;
}

AbstractQoreNode* QoreElementsOperatorNode::parseInitImpl(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo) {
    // turn off "return value ignored" flags
    pflag &= ~(PF_RETURN_VALUE_IGNORED);

    typeInfo = bigIntTypeInfo;

    const QoreTypeInfo* lti = 0;
    parse_init_value(exp, oflag, pflag, lvids, lti);

    if (QoreTypeInfo::hasType(lti)
        && !QoreTypeInfo::parseAccepts(listTypeInfo, lti)
        && !QoreTypeInfo::parseAccepts(hashTypeInfo, lti)
        && !QoreTypeInfo::parseAccepts(stringTypeInfo, lti)
        && !QoreTypeInfo::parseAccepts(binaryTypeInfo, lti)
        && !QoreTypeInfo::parseAccepts(objectTypeInfo, lti)) {
        QoreStringNode* edesc = new QoreStringNode("the argument given to the 'elements' operator is ");
        QoreTypeInfo::getThisType(lti, *edesc);
        edesc->concat(", so this expression will always return 0; the 'elements' operator can only return a value with lists, hashes, strings, binary objects, and objects");
        qore_program_private::makeParseWarning(getProgram(), *loc, QP_WARN_INVALID_OPERATION, "INVALID-OPERATION", edesc);
    }

    // see the argument is a constant value, then eval immediately and substitute this node with the result
    if (exp.isValue()) {
        SimpleRefHolder<QoreElementsOperatorNode> del(this);
        ParseExceptionSink xsink;
        ValueEvalRefHolder v(this, *xsink);
        assert(!**xsink);
        return v.takeReferencedValue().takeNode();
    }

    return this;
}
