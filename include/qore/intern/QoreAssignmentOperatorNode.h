/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
    QoreAssignmentOperatorNode.h

    Qore Programming Language

    Copyright (C) 2003 - 2024 Qore Technologies, s.r.o.

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

#ifndef _QORE_QOREASSIGNMENTOPERATORNODE_H
#define _QORE_QOREASSIGNMENTOPERATORNODE_H

class QoreAssignmentOperatorNode : public QoreBinaryLValueOperatorNode {
OP_COMMON
public:
    DLLLOCAL QoreAssignmentOperatorNode(const QoreProgramLocation* loc, QoreValue left, QoreValue right) :
            QoreBinaryLValueOperatorNode(loc, left, right) {
    }

    DLLLOCAL virtual QoreOperatorNode* copyBackground(ExceptionSink* xsink) const {
        return copyBackgroundExplicit<QoreAssignmentOperatorNode>(xsink);
    }

protected:
    // to support "broken-int-assignments"
    bool broken_int = false;
    // flag for identical match with assignment types (lvalue & rvalue)
    bool ident = false;

    DLLLOCAL int parseInitIntern(QoreParseContext& parse_context, bool weak_assignment);

    DLLLOCAL QoreValue evalIntern(ExceptionSink* xsink, bool& needs_deref, bool weak_assignment) const;

    DLLLOCAL virtual int parseInitImpl(QoreValue& val, QoreParseContext& parse_context) {
        return parseInitIntern(parse_context, false);
    }

    DLLLOCAL virtual QoreValue evalImpl(bool& needs_deref, ExceptionSink* xsink) const {
        return evalIntern(xsink, needs_deref, false);
    }
};

class QoreWeakAssignmentOperatorNode : public QoreAssignmentOperatorNode {
OP_COMMON
public:
    DLLLOCAL QoreWeakAssignmentOperatorNode(const QoreProgramLocation* loc, QoreValue left, QoreValue right)
            : QoreAssignmentOperatorNode(loc, left, right) {
    }

    DLLLOCAL virtual QoreOperatorNode* copyBackground(ExceptionSink* xsink) const {
        return copyBackgroundExplicit<QoreWeakAssignmentOperatorNode>(xsink);
    }

protected:
    DLLLOCAL int parseInitImpl(QoreValue& val, QoreParseContext& parse_context) {
        return parseInitIntern(parse_context, true);
    }

    DLLLOCAL virtual QoreValue evalImpl(bool& needs_deref, ExceptionSink* xsink) const {
        return evalIntern(xsink, needs_deref, true);
    }
};

#endif
