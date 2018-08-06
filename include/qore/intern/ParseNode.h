/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
    ParseNode.h

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

#ifndef _QORE_PARSENODE_H

#define _QORE_PARSENODE_H

#include "qore/intern/WeakReferenceNode.h"

class ParseNode : public SimpleQoreNode {
public:
    const QoreProgramLocation* loc;

private:
    // not implemented
    ParseNode& operator=(const ParseNode&) = delete;

protected:
    //! if the node has an effect when evaluated (changes something)
    bool effect : 1;

    //! if the node has meaning as a top-level node
    bool effect_as_root : 1;

    //! if the return value is ignored
    bool ref_rv : 1;

    //! if the node has undergone "parse initialization"
    bool parse_init : 1;

    DLLLOCAL virtual void parseInitImpl(QoreValue& val, LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo) = 0;

    DLLLOCAL virtual const QoreTypeInfo* getTypeInfo() const = 0;

public:
    DLLLOCAL ParseNode(const QoreProgramLocation* loc, qore_type_t t, bool n_needs_eval = true) : SimpleQoreNode(t, false, n_needs_eval), loc(loc), effect(n_needs_eval), ref_rv(true), parse_init(false) {
        effect_as_root = effect;
    }
    DLLLOCAL ParseNode(const QoreProgramLocation* loc, qore_type_t t, bool n_needs_eval, bool n_effect) : SimpleQoreNode(t, false, n_needs_eval), loc(loc), effect(n_effect), ref_rv(true), parse_init(false) {
        effect_as_root = effect;
    }
    DLLLOCAL ParseNode(const ParseNode& old) : SimpleQoreNode(old.type, false, old.needs_eval_flag), loc(old.loc), effect(old.effect), ref_rv(old.ref_rv), parse_init(false) {
        effect_as_root = effect;
    }
    // parse types should never be copied
    DLLLOCAL virtual AbstractQoreNode* realCopy() const {
        assert(false);
        return 0;
    }
    DLLLOCAL virtual bool is_equal_soft(const AbstractQoreNode* v, ExceptionSink* xsink) const {
        assert(false);
        return false;
    }
    DLLLOCAL virtual bool is_equal_hard(const AbstractQoreNode* v, ExceptionSink* xsink) const {
        assert(false);
        return false;
    }
    DLLLOCAL void set_effect(bool n_effect) {
        effect = n_effect;
    }
    DLLLOCAL bool has_effect() const {
        return effect;
    }
    DLLLOCAL void set_effect_as_root(bool n_effect) {
        effect_as_root = n_effect;
    }
    DLLLOCAL bool has_effect_as_root() const {
        return effect_as_root;
    }
    DLLLOCAL void ignore_rv() {
        ref_rv = false;
    }
    DLLLOCAL bool need_rv() const {
        return ref_rv;
    }

    DLLLOCAL virtual void parseInit(QoreValue& val, LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo) {
        if (parse_init) {
            typeInfo = getTypeInfo();
            return;
        }
        parse_init = true;
        parseInitImpl(val, oflag, pflag, lvids, typeInfo);
    }
};

// these objects will never be copied or referenced therefore they can have
// public destructors - the deref() functions just call "delete this;"
class ParseNoEvalNode : public ParseNode {
private:
    // not implemented
    DLLLOCAL ParseNoEvalNode& operator=(const ParseNoEvalNode&);

    DLLLOCAL virtual void parseInitImpl(QoreValue& val, LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo) = 0;
    DLLLOCAL virtual const QoreTypeInfo* getTypeInfo() const = 0;

protected:
    DLLLOCAL virtual QoreValue evalImpl(bool& needs_deref, ExceptionSink* xsink) const {
        assert(false);
        return QoreValue();
    }

public:
    DLLLOCAL ParseNoEvalNode(const QoreProgramLocation* loc, qore_type_t t) : ParseNode(loc, t, false) {
    }

    DLLLOCAL ParseNoEvalNode(const ParseNoEvalNode& old) : ParseNode(old) {
    }
};

#endif
