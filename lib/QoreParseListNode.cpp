/* -*- indent-tabs-mode: nil -*- */
/*
    QoreParseListNode.cpp

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
#include "qore/intern/QoreParseListNode.h"
#include "qore/intern/qore_list_private.h"
#include "qore/intern/qore_program_private.h"

void QoreParseListNode::finalizeBlock(int sline, int eline) {
    QoreProgramLocation tl(sline, eline);
    if (tl.getFile() == loc->getFile()
        && tl.getSource() == loc->getSource()
        && (sline != loc->start_line || eline != loc->end_line)) {
        loc = qore_program_private::get(*getProgram())->getLocation(*loc, sline, eline);
    }
}

bool QoreParseListNode::parseInitIntern(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo) {
    bool needs_eval = false;

    // turn off "return value ignored" flag before performing parse init
    pflag &= ~PF_RETURN_VALUE_IGNORED;

    // initialize value type vector
    vtypes.resize(values.size());

    // try to find a common value type, if any
    bool vcommon = false;

    for (size_t i = 0; i < values.size(); ++i) {
        parse_init_value(values[i], oflag, pflag, lvids, vtypes[i]);

        //printd(5, "QoreParseListNode::parseInitIntern() this: %p %d: vcommon: %d vt: %p '%s' vtype: %p '%s'\n", this, i, vcommon, vtypes[i], QoreTypeInfo::getName(vtypes[i]), vtype, QoreTypeInfo::getName(vtype));

        if (!i) {
            if (vtypes[0] && vtypes[0] != anyTypeInfo) {
                vtype = vtypes[0];
                vcommon = true;
            }
        }
        else if (vcommon && !QoreTypeInfo::matchCommonType(vtype, vtypes[i]))
            vcommon = false;

        if (!needs_eval && values[i].needsEval()) {
            needs_eval = true;
        }
    }

    //printd(5, "QoreParseListNode::parseInitIntern() this: %p vcommon: %d vtype: %p '%s'\n", this, vcommon, vtype, QoreTypeInfo::getName(vtype));

    // issue #2791: when performing type folding, do not set to type "any" but rather use "auto"
    if (vtype && vtype != anyTypeInfo) {
        this->typeInfo = typeInfo = qore_get_complex_list_type(vtype);
    }
    else {
        this->typeInfo = listTypeInfo;
        // issue #2647: allow an empty list to be assigned to any complex list
        // it will get folded at runtime into the desired type in any case
        typeInfo = vtypes.empty() ? emptyListTypeInfo : listTypeInfo;
    }

    //printd(5, "QoreParseListNode::parseInitIntern() this: %p size: %d typeInfo: %p '%s'\n", this, size(), typeInfo, QoreTypeInfo::getName(typeInfo));

    return needs_eval;
}

AbstractQoreNode* QoreParseListNode::parseInitImpl(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo) {
    if (parseInitIntern(oflag, pflag, lvids, typeInfo))
        return this;

    // evaluate immediately
    SimpleRefHolder<QoreParseListNode> holder(this);
    ValueEvalRefHolder rv(this, nullptr);
    return rv.takeReferencedValue().takeNode();
}

QoreValue QoreParseListNode::evalValueImpl(bool& needs_deref, ExceptionSink* xsink) const {
    assert(needs_deref);
    ReferenceHolder<QoreListNode> l(new QoreListNode, xsink);
    qore_list_private* ll = qore_list_private::get(**l);
    ll->reserve(values.size());

    // issue #2106 we must calculate the runtime type again because lvalues can return NOTHING despite their declared type
    const QoreTypeInfo* vtype = nullptr;
    // try to find a common value type, if any
    bool vcommon = false;

    for (size_t i = 0; i < values.size(); ++i) {
        ValueEvalRefHolder v(values[i], xsink);
        if (xsink && *xsink)
            return QoreValue();

        const QoreTypeInfo* vt = v->getTypeInfo();

        if (!i) {
            vtype = vt;
            vcommon = true;
        }
        else if (vcommon && !QoreTypeInfo::matchCommonType(vtype, vt))
            vcommon = false;

        // issue #2791: ensure that type folding is performed at the source if necessary
        QoreValue val = v.takeReferencedValue();
        if (this->vtype != vt && !QoreTypeInfo::hasComplexType(this->vtype) && QoreTypeInfo::hasComplexType(vt)) {
            // this can never throw an exception; it's only used for type folding/stripping
            QoreTypeInfo::acceptAssignment(this->vtype, "<type folding>", val, xsink);
            // can be called with xsink == nullptr when called at parse time
            assert(!xsink || !*xsink);
        }

        ll->pushIntern(val);
    }

    ValueHolder rv(l.release(), xsink);

    // issue #2791: when performing type folding, do not set to type "any" but rather use "auto"
    if (!vtype || vtype == anyTypeInfo) {
        vtype = autoTypeInfo;
    }
    const QoreTypeInfo* ti = qore_get_complex_list_type(vtype);
    ll->complexTypeInfo = ti;

    //printd(5, "QoreParseListNode::evalValueImpl() this: %p size: %d typeInfo: %p '%s' (vtype: '%s')\n", this, size(), ti, QoreTypeInfo::getName(ti), QoreTypeInfo::getName(vtype));

    return rv.release();
}

int QoreParseListNode::initArgs(LocalVar* oflag, int pflag, type_vec_t& arg_types, QoreListNode*& args) {
    ReferenceHolder<> holder(this, nullptr);
    int lvids = 0;
    const QoreTypeInfo* ti = nullptr;
    parseInitIntern(oflag, pflag, lvids, ti);
    arg_types = std::move(vtypes);

    ReferenceHolder<QoreListNode> l(new QoreListNode(needs_eval()), nullptr);
    qore_list_private* ll = qore_list_private::get(**l);
    ll->complexTypeInfo = qore_get_complex_list_type(autoTypeInfo);
    ll->reserve(values.size());
    for (auto& i : values) {
        ll->pushIntern(i);
    }
    values.clear();
    args = l.release();

    return lvids;
}

int QoreParseListNode::getAsString(QoreString& str, int foff, ExceptionSink* xsink) const {
    str.sprintf("list: %d element%s", (int)values.size(), values.size() == 1 ? "" : "s");
    return 0;
}

QoreString* QoreParseListNode::getAsString(bool& del, int foff, ExceptionSink* xsink) const {
    QoreString* str = new QoreString();
    getAsString(*str, foff, xsink);
    del = true;
    return str;
}

