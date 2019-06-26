/*
    QoreHashMapSelectOperatorNode.cpp

    Qore Programming Language

    Copyright (C) 2003 - 2019 Qore Technologies, s.r.o.

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

#include "qore/intern/qore_program_private.h"
#include "qore/intern/QoreHashNodeIntern.h"

QoreString QoreHashMapSelectOperatorNode::map_str("map operator expression");

// if del is true, then the returned QoreString * should be mapd, if false, then it must not be
QoreString *QoreHashMapSelectOperatorNode::getAsString(bool& del, int foff, ExceptionSink* xsink) const {
    del = false;
    return &map_str;
}

int QoreHashMapSelectOperatorNode::getAsString(QoreString& str, int foff, ExceptionSink* xsink) const {
    str.concat(&map_str);
    return 0;
}

void QoreHashMapSelectOperatorNode::parseInitImpl(QoreValue& val, LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo) {
    assert(!typeInfo);

    pflag &= ~PF_RETURN_VALUE_IGNORED;

    // check iterator expression
    const QoreTypeInfo* iteratorTypeInfo = nullptr;
    parse_init_value(e[2], oflag, pflag, lvids, iteratorTypeInfo);

    const QoreTypeInfo* expTypeInfo = nullptr;
    const QoreTypeInfo* expTypeInfo2 = nullptr;
    const QoreTypeInfo* selectTypeInfo = nullptr;

    {
        // set implicit argv arg type
        ParseImplicitArgTypeHelper pia(QoreTypeInfo::getUniqueReturnComplexList(iteratorTypeInfo));

        // check key expression
        parse_init_value(e[0], oflag, pflag, lvids, expTypeInfo);
        // check value expression2
        parse_init_value(e[1], oflag, pflag, lvids, expTypeInfo2);
        // check select expression
        parse_init_value(e[3], oflag, pflag, lvids, selectTypeInfo);
    }

    typeInfo = QoreHashMapOperatorNode::setReturnTypeInfo(returnTypeInfo, expTypeInfo2, iteratorTypeInfo);
}

QoreHashNode* QoreHashMapSelectOperatorNode::getNewHash() const {
    const QoreTypeInfo* typeInfo = QoreTypeInfo::getUniqueReturnComplexHash(returnTypeInfo);
    return new QoreHashNode(typeInfo ? typeInfo : autoTypeInfo);
}

QoreValue QoreHashMapSelectOperatorNode::evalImpl(bool& needs_deref, ExceptionSink* xsink) const {
    ValueEvalRefHolder arg_lst(e[2], xsink);
    if (*xsink || arg_lst->isNothing())
        return QoreValue();

    // try to find a common value type, if any
    bool vcommon = false;
    const QoreTypeInfo* valueType = nullptr;

    qore_type_t arglst_type = arg_lst->getType();
    assert(arglst_type != NT_NOTHING);
    ReferenceHolder<QoreHashNode> ret_val(ref_rv ? getNewHash() : nullptr, xsink);
    if (NT_LIST != arglst_type) { // Single value
        // check if it's an AbstractIterator object
        if (NT_OBJECT == arglst_type) {
            AbstractIteratorHelper h(xsink, "hmap operator select",
                                    const_cast<QoreObject*>(arg_lst->get<const QoreObject>()));
            if (*xsink)
                return QoreValue();

            if (h)
                return mapIterator(h, xsink); // TODO!!
            // passed iterator
        }

        // check if value can be mapped
        SingleArgvContextHelper argv_helper(arg_lst.takeReferencedValue(), xsink);
        ValueEvalRefHolder result(e[3], xsink);
        if (*xsink || !result->getAsBool())
            return QoreValue();

        ValueEvalRefHolder arg_key(e[0], xsink);
        if (*xsink)
            return QoreValue();

        ValueEvalRefHolder arg_val(e[1], xsink);
        if (*xsink)
            return QoreValue();

        // we have to convert to a string in the default encoding to use a hash key
        QoreStringValueHelper str_util(*arg_key, QCS_DEFAULT, xsink);
        if (*xsink)
            return QoreValue();

        if (ref_rv) {
            const QoreTypeInfo* vtype = arg_val->getTypeInfo();
            //printd(5, "QoreHashMapSelectOperatorNode::evalImpl() i: %d vcommon: %d vtype: %p '%s' valueType: %p '%s'\n", li.index(), vcommon, vtype, QoreTypeInfo::getName(vtype), valueType, QoreTypeInfo::getName(valueType));
            if (vtype && vtype != anyTypeInfo) {
                valueType = vtype;
                vcommon = true;
            }

            // Insert key-Value pair to the hash
            ret_val->setKeyValue(str_util->c_str(), arg_val.takeReferencedValue(), xsink);
        }
    } else { // List of values
        ConstListIterator li(arg_lst->get<const QoreListNode>());
        while (li.next()) {
            // set offset in thread-local data for "$#"
            ImplicitElementHelper eh(li.index());
            SingleArgvContextHelper argv_helper(li.getReferencedValue(), xsink);

            ValueEvalRefHolder result(e[3], xsink);
            if (*xsink)
                return QoreValue();

            if (!result->getAsBool())
                continue;

            {
                ValueEvalRefHolder ekey(e[0], xsink);
                if (*xsink)
                    return QoreValue();

                // we have to convert to a string in the default encoding to use a hash key
                QoreStringValueHelper key(*ekey, QCS_DEFAULT, xsink);
                if (*xsink)
                    return QoreValue();

                ValueEvalRefHolder val(e[1], xsink);
                if (*xsink)
                    return QoreValue();

                if (ref_rv) {
                    const QoreTypeInfo* vtype = val->getTypeInfo();
                    //printd(5, "QoreHashMapSelectOperatorNode::evalImpl() i: %d vcommon: %d vtype: %p '%s' valueType: %p '%s'\n", li.index(), vcommon, vtype, QoreTypeInfo::getName(vtype), valueType, QoreTypeInfo::getName(valueType));
                    if (!li.index()) {
                        if (vtype && vtype != anyTypeInfo) {
                            valueType = vtype;
                            vcommon = true;
                        }
                    } else if (vcommon && !QoreTypeInfo::matchCommonType(valueType, vtype)) {
                        vcommon = false;
                    }

                    ret_val->setKeyValue(key->c_str(), val.takeReferencedValue(), xsink);
                }
            }
            // if there is an exception dereferencing one of the evaluted nodes above, then exit the loop
            if (*xsink)
                return QoreValue();
        }
    }
    if (*xsink || !ref_rv)
        return QoreValue();

    //printd(5, "QoreHashMapSelectOperatorNode::mapIterator() vcommon: %d valueType: %p '%s'\n", vcommon, valueType, QoreTypeInfo::getName(valueType));
    if (ref_rv && vcommon) {
        qore_hash_private::get(**ret_val)->complexTypeInfo = qore_get_complex_hash_type(valueType);
    }

    return ret_val.release();
}

QoreValue QoreHashMapSelectOperatorNode::mapIterator(AbstractIteratorHelper& h, ExceptionSink* xsink) const {
    ReferenceHolder<QoreHashNode> rv(ref_rv ? getNewHash() : nullptr, xsink);

    // try to find a common value type, if any
    bool vcommon = false;
    const QoreTypeInfo* valueType = nullptr;

    qore_size_t i = 0;
    // set offset in thread-local data for "$#"
    while (true) {
        bool has_next = h.next(xsink);
        if (*xsink)
            return QoreValue();
        if (!has_next)
            break;

        ImplicitElementHelper eh(i++);

        ValueHolder iv(h.getValue(xsink), xsink);
        if (*xsink)
            return QoreValue();

        // check if value can be mapped
        SingleArgvContextHelper argv_helper(iv.release(), xsink);

        ValueEvalRefHolder result(e[3], xsink);
        if (*xsink)
            return QoreValue();

        if (!result->getAsBool())
            continue;

        {
            ValueEvalRefHolder ekey(e[0], xsink);
            if (*xsink)
                return QoreValue();

            // we have to convert to a string in the default encoding to use a hash key
            QoreStringValueHelper key(*ekey, QCS_DEFAULT, xsink);
            if (*xsink)
                return QoreValue();

            ValueEvalRefHolder val(e[1], xsink);
            if (*xsink)
                return QoreValue();

            if (ref_rv) {
                const QoreTypeInfo* vtype = val->getTypeInfo();
                //printd(5, "QoreHashMapSelectOperatorNode::mapIterator() i: %d vcommon: %d vtype: %p '%s' valueType: %p '%s'\n", i, vcommon, vtype, QoreTypeInfo::getName(vtype), valueType, QoreTypeInfo::getName(valueType));
                if (i == 1) {
                    if (vtype && vtype != anyTypeInfo) {
                        valueType = vtype;
                        vcommon = true;
                    }
                } else if (vcommon && !QoreTypeInfo::matchCommonType(valueType, vtype)) {
                    vcommon = false;
                }

                rv->setKeyValue(key->c_str(), val.takeReferencedValue(), xsink);
            }
        }
        // if there is an exception dereferencing one of the evaluted nodes above, then exit the loop
        if (*xsink)
            return QoreValue();
    }

    //printd(5, "QoreHashMapSelectOperatorNode::mapIterator() vcommon: %d valueType: %p '%s'\n", vcommon, valueType, QoreTypeInfo::getName(valueType));
    if (ref_rv && vcommon) {
        qore_hash_private::get(**rv)->complexTypeInfo = qore_get_complex_hash_type(valueType);
    }

    return rv.release();
}
