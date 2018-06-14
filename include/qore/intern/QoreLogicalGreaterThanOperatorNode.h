/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
    QoreLogicalGreaterThanOperatorNode.h

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

#ifndef _QORE_QORELOGICALGREATERTHANOPERATORNODE_H

#define _QORE_QORELOGICALGREATERTHANOPERATORNODE_H

class QoreLogicalGreaterThanOperatorNode : public QoreBoolBinaryOperatorNode {
OP_COMMON

protected:
    // type of pointer to optimized versions depending on arguments found at parse-time
    typedef bool(QoreLogicalGreaterThanOperatorNode::*eval_t)(ExceptionSink *xsink) const;
    // pointer to optimized versions depending on arguments found at parse-time
    eval_t pfunc;

    DLLLOCAL virtual QoreValue evalValueImpl(bool& needs_deref, ExceptionSink *xsink) const;

    DLLLOCAL virtual AbstractQoreNode *parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
        return parseInitIntern(op_str.getBuffer(), oflag, pflag, lvids, typeInfo);
    }

    DLLLOCAL AbstractQoreNode *parseInitIntern(const char *name, LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo);

    DLLLOCAL bool floatGreaterThan(ExceptionSink *xsink) const;
    DLLLOCAL bool bigIntGreaterThan(ExceptionSink *xsink) const;

public:
    DLLLOCAL QoreLogicalGreaterThanOperatorNode(const QoreProgramLocation* loc, QoreValue left, QoreValue right) : QoreBoolBinaryOperatorNode(loc, left, right), pfunc(0) {
    }

    DLLLOCAL virtual QoreOperatorNode* copyBackground(ExceptionSink *xsink) const {
        return copyBackgroundExplicit<QoreLogicalGreaterThanOperatorNode>(xsink);
    }

    DLLLOCAL static bool doGreaterThan(QoreValue l, QoreValue r, ExceptionSink* xsink);
};

#endif
