/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreSquareBracketsRangeOperatorNode.h

  Qore Programming Language

  Copyright (C) 2003 - 2017 Qore Technologies, s.r.o.

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

#ifndef _QORE_QORESQUAREBRACKETSRANGEOPERATORNODE_H
#define _QORE_QORESQUAREBRACKETSRANGEOPERATORNODE_H

class QoreSquareBracketsRangeOperatorNode : public QoreNOperatorNodeBase<3> {
protected:
    const QoreTypeInfo* typeInfo;

    DLLLOCAL static QoreString op_str;

    DLLLOCAL virtual QoreValue evalValueImpl(bool& needs_deref, ExceptionSink* xsink) const;

    DLLLOCAL virtual AbstractQoreNode* parseInitImpl(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo);

    DLLLOCAL static QoreValue doSquareBrackets(QoreValue v0, QoreValue v1, QoreValue v2, ExceptionSink* xsink);

public:
    DLLLOCAL QoreSquareBracketsRangeOperatorNode(const QoreProgramLocation& loc, AbstractQoreNode* p0, AbstractQoreNode* p1, AbstractQoreNode* p2) :
        QoreNOperatorNodeBase<3>(loc, p0, p1 ? p1 : nothing(), p2 ? p2 : nothing()), typeInfo(0) {
    }

    DLLLOCAL virtual const QoreTypeInfo* getTypeInfo() const {
        return typeInfo;
    }

    DLLLOCAL virtual QoreString* getAsString(bool& del, int foff, ExceptionSink* xsink) const;
    DLLLOCAL virtual int getAsString(QoreString& str, int foff, ExceptionSink* xsink) const;

    DLLLOCAL virtual const char* getTypeName() const {
        return op_str.getBuffer();
    }

    DLLLOCAL virtual QoreOperatorNode* copyBackground(ExceptionSink* xsink) const {
        ReferenceHolder<> n_e0(copy_and_resolve_lvar_refs(e[0], xsink), xsink);
        if (*xsink)
            return 0;
        ReferenceHolder<> n_e1(copy_and_resolve_lvar_refs(e[1], xsink), xsink);
        if (*xsink)
            return 0;
        ReferenceHolder<> n_e2(copy_and_resolve_lvar_refs(e[2], xsink), xsink);
        if (*xsink)
            return 0;
        return new QoreSquareBracketsRangeOperatorNode(loc, n_e0.release(), n_e1.release(), n_e2.release());
    }
};

#endif // QORESQUAREBRACKETSRANGEOPERATORNODE_H
