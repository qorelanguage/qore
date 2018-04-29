/* -*- indent-tabs-mode: nil -*- */
/*
  QoreParseHashNode.cpp

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
#include "qore/intern/QoreParseHashNode.h"
#include "qore/intern/QoreHashNodeIntern.h"
#include "qore/intern/qore_program_private.h"

AbstractQoreNode* QoreParseHashNode::parseInitImpl(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo) {
    assert(keys.size() == values.size());
    bool needs_eval = false;

    // turn off "return value ignored" flag before performing parse init
    pflag &= ~PF_RETURN_VALUE_IGNORED;

    // initialize value type vector
    vtypes.resize(keys.size());

    // try to find a common value type, if any
    bool vcommon = false;

    for (size_t i = 0; i < keys.size(); ++i) {
        const QoreTypeInfo* argTypeInfo = 0;
        AbstractQoreNode* p = keys[i];
        keys[i] = keys[i]->parseInit(oflag, pflag, lvids, argTypeInfo);

        if (p != keys[i] && (!keys[i] || keys[i]->is_value())) {
            QoreStringValueHelper key(keys[i]);
            checkDup(lvec[i], key->getBuffer());
        }
        else if (!needs_eval && keys[i] && keys[i]->needs_eval())
            needs_eval = true;

        if (!QoreTypeInfo::canConvertToScalar(argTypeInfo)) {
            QoreStringMaker str("key number %ld (starting from 0) in the hash is ", i);
            argTypeInfo->doNonStringWarning(lvec[i], str.getBuffer());
        }

        argTypeInfo = nullptr;
        values[i] = values[i]->parseInit(oflag, pflag, lvids, vtypes[i]);

        if (!i) {
            if (vtypes[0] && vtypes[0] != anyTypeInfo) {
                vtype = vtypes[0];
                vcommon = true;
            }
        }
        else if (vcommon && !QoreTypeInfo::matchCommonType(vtype, vtypes[i]))
            vcommon = false;

        if (!needs_eval && values[i] && values[i]->needs_eval())
            needs_eval = true;
    }

    kmap.clear();

    // issue #2791: when performing type folding, do not set to type "any" but rather use "auto"
    if (vtype && vtype != anyTypeInfo) {
        this->typeInfo = typeInfo = qore_program_private::get(*getProgram())->getComplexHashType(vtype);
    }
    else {
        this->typeInfo = hashTypeInfo;
        // issue #2647: allow an empty hash to be assigned to any complex hash (but not hashdecls)
        // it will get folded at runtime into the desired type in any case
        typeInfo = vtypes.empty() ? emptyHashTypeInfo : hashTypeInfo;
    }

    if (needs_eval)
        return this;

    // evaluate immediately
    ReferenceHolder<QoreHashNode> h(new QoreHashNode(vtype), nullptr);
    qore_hash_private* ph = qore_hash_private::get(**h);
    for (size_t i = 0; i < keys.size(); ++i) {
        QoreStringValueHelper key(keys[i]);

        // issue #2791: ensure that type folding is performed at the source if necessary
        const QoreTypeInfo* vt = getTypeInfoForValue(values[i]);
        if (vtype != vt && !QoreTypeInfo::hasComplexType(vtype) && QoreTypeInfo::hasComplexType(vt)) {
            QoreValue val = values[i];
            // this can never throw an exception; it's only used for type folding/stripping
            QoreTypeInfo::acceptInputKey(vtype, key->c_str(), val, nullptr);
            values[i] = val.takeNode();
        }

        discard(ph->swapKeyValue(key->c_str(), values[i], nullptr), nullptr);
        values[i] = nullptr;
    }

    deref();
    return h.release();
}

QoreValue QoreParseHashNode::evalValueImpl(bool& needs_deref, ExceptionSink* xsink) const {
    assert(keys.size() == values.size());
    ReferenceHolder<QoreHashNode> h(new QoreHashNode, xsink);

    // issue #2106 we must calculate the runtime type again because lvalues can return NOTHING despite their declared type
    const QoreTypeInfo* vtype = nullptr;
    // try to find a common value type, if any
    bool vcommon = false;

    for (size_t i = 0; i < keys.size(); ++i) {
        ValueEvalRefHolder k(keys[i], xsink);
        if (xsink && *xsink)
            return QoreValue();

        ValueEvalRefHolder v(values[i], xsink);
        if (xsink && *xsink)
            return QoreValue();

        const QoreTypeInfo* vt = v->getTypeInfo();

        if (!i) {
            vtype = vt;
            vcommon = true;
        }
        else if (vcommon && !QoreTypeInfo::matchCommonType(vtype, vt)) {
            vcommon = false;
        }

        QoreStringValueHelper key(*k);

        // issue #2791: ensure that type folding is performed at the source if necessary
        QoreValue val = v.takeReferencedValue();
        //printd(5, "QoreParseHashNode::evalValueImpl() '%s' this->vtype: '%s' (c: %d) vt: '%s' (c: %d)\n", key->c_str(), QoreTypeInfo::getName(this->vtype), QoreTypeInfo::hasComplexType(this->vtype), QoreTypeInfo::getName(vt), QoreTypeInfo::hasComplexType(vt));
        if (this->vtype != vt && !QoreTypeInfo::hasComplexType(this->vtype) && QoreTypeInfo::hasComplexType(vt)) {
            // this can never throw an exception; it's only used for type folding/stripping
            QoreTypeInfo::acceptInputKey(this->vtype, key->c_str(), val, xsink);
        }

        h->setValueKeyValue(key->c_str(), val, xsink);
        if (xsink && *xsink) {
            return QoreValue();
        }
    }

    ValueHolder rv(h.release(), xsink);

    // issue #2791: when performing type folding, do not set to type "any" but rather use "auto"
    if (vtype && vtype != anyTypeInfo) {
        const QoreTypeInfo* ti = qore_program_private::get(*getProgram())->getComplexHashType(vtype);
        qore_hash_private::get(*rv->get<QoreHashNode>())->complexTypeInfo = ti;
    }

    return rv.release();
}
