/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
    QoreLogicalNotEqualsOperatorNode.h

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

#ifndef _QORE_QORELOGICALNOTEQUALSOPERATORNODE_H

#define _QORE_QORELOGICALNOTEQUALSOPERATORNODE_H

class QoreLogicalNotEqualsOperatorNode : public QoreLogicalEqualsOperatorNode {
public:
    DLLLOCAL QoreLogicalNotEqualsOperatorNode(const QoreProgramLocation* loc, QoreValue left, QoreValue right)
            : QoreLogicalEqualsOperatorNode(loc, left, right) {
    }

    // if del is true, then the returned QoreString * should be deleted, if false, then it must not be
    DLLLOCAL virtual QoreString *getAsString(bool& del, int foff, ExceptionSink* xsink) const {
        del = false;
        return &logical_not_equals_str;
    }

    DLLLOCAL virtual int getAsString(QoreString& str, int foff, ExceptionSink* xsink) const {
        str.concat(&logical_not_equals_str);
        return 0;
    }

    // returns the type name as a c string
    DLLLOCAL virtual const char* getTypeName() const {
        return logical_not_equals_str.getBuffer();
    }

    DLLLOCAL virtual QoreOperatorNode* copyBackground(ExceptionSink* xsink) const {
        return copyBackgroundExplicit<QoreLogicalNotEqualsOperatorNode>(xsink);
    }

    DLLLOCAL static bool softNotEqual(const QoreValue& l, const QoreValue& r, ExceptionSink* xsink) {
        return !QoreLogicalEqualsOperatorNode::softEqual(l, r, xsink);
    }

protected:
    DLLLOCAL static QoreString logical_not_equals_str;

    DLLLOCAL virtual QoreValue evalImpl(bool& needs_deref, ExceptionSink* xsink) const {
        QoreValue rv = QoreLogicalEqualsOperatorNode::evalImpl(needs_deref, xsink);
        if (*xsink)
            return QoreValue();
        return !rv.v.b;
    }

    DLLLOCAL virtual int parseInitImpl(QoreValue& val, QoreParseContext& parse_context) {
        int err = QoreLogicalEqualsOperatorNode::parseInitImpl(val, parse_context);
        // make sure to reverse sense of comparison if this expression was resolved to a constant boolean value
        if (val.type == QV_Bool) {
            val.v.b = !val.v.b;
        }
        return err;
    }
};

#endif
