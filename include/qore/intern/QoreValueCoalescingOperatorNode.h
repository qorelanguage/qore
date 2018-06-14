/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
    QoreValueCoalescingOperatorNode.h

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

#ifndef _QORE_QOREVALUECOALESCINGOPERATORNODE_H
#define _QORE_QOREVALUECOALESCINGOPERATORNODE_H

class QoreValueCoalescingOperatorNode : public QoreBinaryOperatorNode<QoreOperatorNode> {
protected:
    static QoreString value_coalescing_str;

    const QoreTypeInfo* typeInfo;

    DLLLOCAL virtual AbstractQoreNode *parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&returnTypeInfo);

    DLLLOCAL virtual QoreValue evalValueImpl(bool& needs_deref, ExceptionSink* xsink) const;

public:
    DLLLOCAL QoreValueCoalescingOperatorNode(const QoreProgramLocation* loc, QoreValue e0, QoreValue e1) : QoreBinaryOperatorNode<QoreOperatorNode>(loc, e0, e1), typeInfo(0) {
    }

    DLLLOCAL virtual QoreString *getAsString(bool &del, int foff, ExceptionSink *xsink) const;

    DLLLOCAL virtual int getAsString(QoreString &str, int foff, ExceptionSink *xsink) const;

    DLLLOCAL virtual const QoreTypeInfo *getTypeInfo() const {
        return typeInfo;
    }

    // returns the type name as a c string
    DLLLOCAL virtual const char* getTypeName() const {
        return value_coalescing_str.getBuffer();
    }

    DLLLOCAL virtual QoreOperatorNode* copyBackground(ExceptionSink* xsink) const {
        return copyBackgroundExplicit<QoreValueCoalescingOperatorNode>(xsink);
    }
};

#endif
