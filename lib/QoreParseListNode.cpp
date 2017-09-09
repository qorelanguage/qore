/* -*- indent-tabs-mode: nil -*- */
/*
  QoreParseListNode.cpp

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

#include <qore/Qore.h>
#include "qore/intern/QoreParseListNode.h"
#include "qore/intern/qore_list_private.h"
#include "qore/intern/qore_program_private.h"

bool QoreParseListNode::parseInitIntern(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo) {
    bool needs_eval = false;

    if (lvec.size())
       loc.end_line = lvec[lvec.size() - 1].end_line;

    // turn off "return value ignored" flag before performing parse init
    pflag &= ~PF_RETURN_VALUE_IGNORED;

    // initialize value type vector
    vtypes.resize(values.size());

    // try to find a common value type, if any
    bool vcommon = false;

    for (size_t i = 0; i < values.size(); ++i) {
        values[i] = values[i]->parseInit(oflag, pflag, lvids, vtypes[i]);

        //printd(5, "QoreParseListNode::parseInitIntern() %d: vcommon: %d vt: %p '%s' vtype: %p '%s'\n", i, vcommon, vtypes[i], QoreTypeInfo::getName(vtypes[i]), vtype, QoreTypeInfo::getName(vtype));

        if (!i) {
            if (QoreTypeInfo::hasType(vtypes[i])) {
                vtype = vtypes[i];
                vcommon = true;
            }
        }
        else if (vcommon && !QoreTypeInfo::matchCommonType(vtype, vtypes[i]))
            vcommon = false;

        if (!needs_eval && values[i] && values[i]->needs_eval())
            needs_eval = true;
    }

    if (vtype && !QoreTypeInfo::hasType(vtype))
        vtype = nullptr;

    if (vtype) {
        this->typeInfo = typeInfo = qore_program_private::get(*getProgram())->getComplexListType(vtype);
    }
    else {
        this->typeInfo = typeInfo = listTypeInfo;
    }

    //printd(5, "QoreParseListNode::parseInitIntern() typeInfo: %p '%s'\n", typeInfo, QoreTypeInfo::getName(typeInfo));

    return needs_eval;
}

AbstractQoreNode* QoreParseListNode::parseInitImpl(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo) {
    if (parseInitIntern(oflag, pflag, lvids, typeInfo))
        return this;

    // evaluate immediately
    SimpleRefHolder<QoreParseListNode> holder(this);
    ValueEvalRefHolder rv(this, nullptr);
    return rv.getReferencedValue();
}

QoreValue QoreParseListNode::evalValueImpl(bool& needs_deref, ExceptionSink* xsink) const {
    assert(needs_deref);
    ReferenceHolder<QoreListNode> l(new QoreListNode, xsink);
    qore_list_private::get(**l)->reserve(values.size());

    // issue #2106 we must calculate the runtime type again because lvalues can return NOTHING despite their declared type
    const QoreTypeInfo* vtype = nullptr;
    // try to find a common value type, if any
    bool vcommon = false;

    for (size_t i = 0; i < values.size(); ++i) {
        QoreNodeEvalOptionalRefHolder v(values[i], xsink);
        if (xsink && *xsink)
            return QoreValue();

        AbstractQoreNode* val = v.getReferencedValue();
        l->push(val);

        if (!i) {
            vtype = getTypeInfoForValue(val);
            vcommon = true;
        }
        else if (vcommon && !QoreTypeInfo::matchCommonType(vtype, getTypeInfoForValue(val)))
            vcommon = false;
    }

    if (QoreTypeInfo::hasType(vtype))
       qore_list_private::get(**l)->complexTypeInfo = qore_program_private::get(*getProgram())->getComplexListType(vtype);

    return l.release();
}

int QoreParseListNode::initArgs(LocalVar* oflag, int pflag, type_vec_t& arg_types, QoreListNode*& args) {
    ReferenceHolder<> holder(this, nullptr);
    int lvids = 0;
    const QoreTypeInfo* ti = nullptr;
    parseInitIntern(oflag, pflag, lvids, ti);
    arg_types = std::move(vtypes);

    ReferenceHolder<QoreListNode> l(new QoreListNode, nullptr);
    qore_list_private::get(**l)->reserve(values.size());
    for (auto& i : values) {
        l->push(i);
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

