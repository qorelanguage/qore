/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
    QoreCastOperatorNode.h

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

#ifndef _QORE_QORECASTOPERATORNODE_H

#define _QORE_QORECASTOPERATORNODE_H

class QoreParseCastOperatorNode : public QoreSingleExpressionOperatorNode<> {
friend class QoreCastOperatorNode;
public:
    DLLLOCAL QoreParseCastOperatorNode(const QoreProgramLocation* loc, QoreParseTypeInfo* pti, QoreValue exp)
        : QoreSingleExpressionOperatorNode<>(loc, exp), pti(pti) {
    }

    // type is unknown before resolution
    DLLLOCAL virtual const QoreTypeInfo* getTypeInfo() const {
        return anyTypeInfo;
    }

    DLLLOCAL virtual ~QoreParseCastOperatorNode() {
        delete pti;
    }

    DLLLOCAL virtual QoreString* getAsString(bool &del, int foff, ExceptionSink *xsink) const;

    DLLLOCAL virtual int getAsString(QoreString &str, int foff, ExceptionSink *xsink) const;

    // returns the type name as a c string
    DLLLOCAL virtual const char* getTypeName() const {
        return cast_str.getBuffer();
    }

    DLLLOCAL virtual QoreOperatorNode* copyBackground(ExceptionSink *xsink) const {
        assert(false);
        return nullptr;
    }

protected:
    DLLLOCAL static QoreString cast_str;
    QoreParseTypeInfo* pti;

    DLLLOCAL virtual QoreValue evalImpl(bool& needs_deref, ExceptionSink* xsink) const {
        assert(false);
        return QoreValue();
    }

    DLLLOCAL void parseInitImpl(QoreValue& val, LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo);
};

class QoreCastOperatorNode : public QoreSingleExpressionOperatorNode<> {
public:
    DLLLOCAL QoreCastOperatorNode(const QoreProgramLocation* loc, QoreValue exp)
        : QoreSingleExpressionOperatorNode<>(loc, exp) {
    }

    DLLLOCAL virtual ~QoreCastOperatorNode() = default;

    DLLLOCAL QoreString* getAsString(bool& del, int foff, ExceptionSink* xsink) const {
        del = false;
        return &QoreParseCastOperatorNode::cast_str;
    }

    DLLLOCAL virtual int getAsString(QoreString &str, int foff, ExceptionSink *xsink) const {
        str.concat(&QoreParseCastOperatorNode::cast_str);
        return 0;
    }

    // returns the type name as a c string
    DLLLOCAL virtual const char* getTypeName() const {
        return QoreParseCastOperatorNode::cast_str.getBuffer();
    }

protected:
    DLLLOCAL void parseInitImpl(QoreValue& val, LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo) {
    }
};

class QoreClassCastOperatorNode : public QoreCastOperatorNode {
public:
    DLLLOCAL QoreClassCastOperatorNode(const QoreProgramLocation* loc, const QoreClass* qc, QoreValue exp,
        bool or_nothing)
        : QoreCastOperatorNode(loc, exp), qc(qc), or_nothing(or_nothing) {
    }

    DLLLOCAL virtual ~QoreClassCastOperatorNode() = default;

    DLLLOCAL virtual const QoreTypeInfo* getTypeInfo() const {
        return qc ? qc->getTypeInfo() : objectTypeInfo;
    }

    DLLLOCAL virtual QoreOperatorNode* copyBackground(ExceptionSink *xsink) const {
        ValueHolder n_exp(copy_value_and_resolve_lvar_refs(exp, xsink), xsink);
        if (*xsink)
            return nullptr;
        return new QoreClassCastOperatorNode(loc, qc, n_exp.release(), or_nothing);
    }

protected:
    const QoreClass* qc;
    bool or_nothing;

    DLLLOCAL virtual QoreValue evalImpl(bool& needs_deref, ExceptionSink* xsink) const;
};

class QoreHashDeclCastOperatorNode : public QoreCastOperatorNode {
public:
    DLLLOCAL QoreHashDeclCastOperatorNode(const QoreProgramLocation* loc, const TypedHashDecl* hd, QoreValue exp,
        bool or_nothing)
        : QoreCastOperatorNode(loc, exp), hd(hd), or_nothing(or_nothing) {
    }

    DLLLOCAL virtual ~QoreHashDeclCastOperatorNode() = default;

    DLLLOCAL virtual const QoreTypeInfo* getTypeInfo() const {
        return hd ? hd->getTypeInfo() : hashTypeInfo;
    }

    DLLLOCAL virtual QoreOperatorNode* copyBackground(ExceptionSink *xsink) const {
        ValueHolder n_exp(copy_value_and_resolve_lvar_refs(exp, xsink), xsink);
        if (*xsink)
            return nullptr;
        return new QoreHashDeclCastOperatorNode(loc, hd, n_exp.release(), or_nothing);
    }

protected:
    const TypedHashDecl* hd;
    bool or_nothing;

    DLLLOCAL virtual QoreValue evalImpl(bool& needs_deref, ExceptionSink* xsink) const;
};

class QoreComplexHashCastOperatorNode : public QoreCastOperatorNode {
public:
    DLLLOCAL QoreComplexHashCastOperatorNode(const QoreProgramLocation* loc, const QoreTypeInfo* typeInfo,
        QoreValue exp, bool or_nothing)
        : QoreCastOperatorNode(loc, exp), typeInfo(typeInfo), or_nothing(or_nothing) {
    }

    DLLLOCAL virtual ~QoreComplexHashCastOperatorNode() = default;

    DLLLOCAL virtual const QoreTypeInfo* getTypeInfo() const {
        return typeInfo;
    }

    DLLLOCAL virtual QoreOperatorNode* copyBackground(ExceptionSink *xsink) const {
        ValueHolder n_exp(copy_value_and_resolve_lvar_refs(exp, xsink), xsink);
        if (*xsink)
            return nullptr;
        assert(typeInfo);
        return new QoreComplexHashCastOperatorNode(loc, typeInfo, n_exp.release(), or_nothing);
    }

protected:
    const QoreTypeInfo* typeInfo;
    bool or_nothing;

    DLLLOCAL virtual QoreValue evalImpl(bool& needs_deref, ExceptionSink* xsink) const;
};

class QoreComplexListCastOperatorNode : public QoreCastOperatorNode {
public:
    DLLLOCAL QoreComplexListCastOperatorNode(const QoreProgramLocation* loc, const QoreTypeInfo* typeInfo,
        QoreValue exp, bool or_nothing)
        : QoreCastOperatorNode(loc, exp), typeInfo(typeInfo), or_nothing(or_nothing) {
    }

    DLLLOCAL virtual ~QoreComplexListCastOperatorNode() = default;

    DLLLOCAL virtual const QoreTypeInfo* getTypeInfo() const {
        return typeInfo;
    }

    DLLLOCAL virtual QoreOperatorNode* copyBackground(ExceptionSink* xsink) const {
        ValueHolder n_exp(copy_value_and_resolve_lvar_refs(exp, xsink), xsink);
        if (*xsink)
            return nullptr;
        return new QoreComplexListCastOperatorNode(loc, typeInfo, n_exp.release(), or_nothing);
    }

protected:
    const QoreTypeInfo* typeInfo;
    bool or_nothing;

    DLLLOCAL virtual QoreValue evalImpl(bool& needs_deref, ExceptionSink* xsink) const;
};

#endif
