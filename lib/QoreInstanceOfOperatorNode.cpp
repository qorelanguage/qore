/*
    QoreInstanceOfOperatorNode.cpp

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
#include "qore/intern/qore_number_private.h"
#include "qore/intern/qore_program_private.h"

QoreString QoreInstanceOfOperatorNode::InstanceOf_str("instanceof operator expression");

// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
QoreString *QoreInstanceOfOperatorNode::getAsString(bool& del, int foff, ExceptionSink* xsink) const {
    del = false;
    return &InstanceOf_str;
}

int QoreInstanceOfOperatorNode::getAsString(QoreString& str, int foff, ExceptionSink* xsink) const {
    str.concat(&InstanceOf_str);
    return 0;
}

QoreValue QoreInstanceOfOperatorNode::evalImpl(bool& needs_deref, ExceptionSink* xsink) const {
    assert(ti);

    ValueEvalRefHolder v(exp, xsink);
    if (*xsink) {
        return QoreValue();
    }

    // treat a weak reference as the target object
    if (v->getType() == NT_WEAKREF) {
        return QoreTypeInfo::runtimeAcceptsValue(ti, **v->get<const WeakReferenceNode>()) ? true : false;
    }

    return QoreTypeInfo::runtimeAcceptsValue(ti, *v) ? true : false;
}

void QoreInstanceOfOperatorNode::parseInitImpl(QoreValue& val, LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo) {
    // turn off "return value ignored" flags
    pflag &= ~(PF_RETURN_VALUE_IGNORED);

    typeInfo = boolTypeInfo;

    const QoreTypeInfo* lti = nullptr;
    parse_init_value(exp, oflag, pflag, lvids, lti);

    if (r) {
        ti = QoreParseTypeInfo::resolveAny(r, loc);
        delete r;
        r = nullptr;
    }
#ifdef DEBUG
    else
        assert(ti);
#endif

    //printd(5, "QoreInstanceOfOperatorNode::parseInitImpl() this: %p exp: %p lti: '%s'\n", this, exp, QoreTypeInfo::getName(lti));;
    // issue #4112: ensure that objects will be subject to runtime checks
    if (!QoreTypeInfo::parseAccepts(ti, lti)
        && (!QoreTypeInfo::parseAccepts(ti, objectTypeInfo) || !QoreTypeInfo::parseAccepts(lti, objectTypeInfo))) {
        QoreStringNode* edesc = new QoreStringNodeMaker("'%s instanceof %s' always returns False", QoreTypeInfo::getName(lti), QoreTypeInfo::getName(ti));
        qore_program_private::makeParseWarning(getProgram(), *loc, QP_WARN_INVALID_OPERATION, "INVALID-OPERATION", edesc);
    }

    // see the argument is a constant value, then eval immediately and substitute this node with the result
    if (exp.isValue()) {
        SimpleRefHolder<QoreInstanceOfOperatorNode> del(this);
        ParseExceptionSink xsink;
        ValueEvalRefHolder v(this, *xsink);
        assert(!**xsink);
        val = v.takeReferencedValue();
    }
}
