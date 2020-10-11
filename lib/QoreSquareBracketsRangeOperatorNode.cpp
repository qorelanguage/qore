/*
    QoreSquareBracketsRangeOperatorNode.cpp

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

#include <qore/Qore.h>
#include "qore/intern/qore_program_private.h"

QoreString QoreSquareBracketsRangeOperatorNode::op_str("x[m..n] operator expression");

QoreString *QoreSquareBracketsRangeOperatorNode::getAsString(bool &del, int foff, ExceptionSink *xsink) const {
    del = false;
    return &op_str;
}

int QoreSquareBracketsRangeOperatorNode::getAsString(QoreString &str, int foff, ExceptionSink *xsink) const {
    str.concat(&op_str);
    return 0;
}

void QoreSquareBracketsRangeOperatorNode::parseInitImpl(QoreValue& val, LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&returnTypeInfo) {
    pflag &= ~PF_RETURN_VALUE_IGNORED;
    assert(!typeInfo);
    assert(!returnTypeInfo);

    const QoreTypeInfo* typeInfo0 = nullptr, *typeInfo1 = nullptr, *typeInfo2 = nullptr;
    parse_init_value(e[0], oflag, pflag, lvids, typeInfo0);
    parse_init_value(e[1], oflag, pflag, lvids, typeInfo1);
    parse_init_value(e[2], oflag, pflag, lvids, typeInfo2);

    if (pflag & PF_FOR_ASSIGNMENT)
        parse_error(*loc, "the range operator cannot be used in the left-hand side of an assignment expression");

    if (QoreTypeInfo::hasType(typeInfo0)) {
        if (QoreTypeInfo::isType(typeInfo0, NT_LIST))
            returnTypeInfo = typeInfo0;
        else if (QoreTypeInfo::isType(typeInfo0, NT_STRING))
            returnTypeInfo = stringTypeInfo;
        else if (QoreTypeInfo::isType(typeInfo0, NT_BINARY))
            returnTypeInfo = binaryTypeInfo;
        else if (QoreTypeInfo::parseReturns(typeInfo0, NT_LIST))
            returnTypeInfo = get_or_nothing_type_check(typeInfo0);
        else if (QoreTypeInfo::parseReturns(typeInfo0, NT_STRING))
            returnTypeInfo = stringOrNothingTypeInfo;
        else if (QoreTypeInfo::parseReturns(typeInfo0, NT_BINARY))
            returnTypeInfo = binaryOrNothingTypeInfo;
        else {
            // raise an exception due to the invalid operand type
            parseException(*loc, "PARSE-TYPE-ERROR", "the operand for the range square brackets operator [m..n] is type '%s'; this operator only works with 'list', 'string', and 'binary'", QoreTypeInfo::getName(typeInfo0));
        }
    }
    // ensure that the range operands can be converted to an integer
    if (!QoreTypeInfo::isType(typeInfo1, NT_NOTHING) && !QoreTypeInfo::canConvertToScalar(typeInfo1))
        parseException(*loc, "PARSE-TYPE-ERROR", "the start expression of the 'range' operator (..) expression is type '%s', which does not evaluate to a numeric type, therefore will always evaluate to 0 at runtime", QoreTypeInfo::getName(typeInfo1));
    if (!QoreTypeInfo::isType(typeInfo2, NT_NOTHING) && !QoreTypeInfo::canConvertToScalar(typeInfo2))
        parseException(*loc, "PARSE-TYPE-ERROR", "the end expression of the 'range' operator (..) expression is type '%s', which does not evaluate to a numeric type, therefore will always evaluate to 0 at runtime", QoreTypeInfo::getName(typeInfo2));

    typeInfo = returnTypeInfo;
}

QoreValue QoreSquareBracketsRangeOperatorNode::evalImpl(bool& needs_deref, ExceptionSink* xsink) const {
    ValueEvalRefHolder seq(e[0], xsink);
    if (*xsink)
        return QoreValue();

    qore_type_t seq_type = seq->getType();
    int64 start, stop, seq_size;
    bool empty = !getEffectiveRange(*seq, start, stop, seq_size, xsink);
    if (*xsink)
        return QoreValue();

    switch (seq_type) {
        case NT_LIST: {
            const QoreListNode* l = seq->get<const QoreListNode>();
            ReferenceHolder<QoreListNode> rv(new QoreListNode(l->getValueTypeInfo()), xsink);
            if (start < stop) {
                for (int64 i = start; i <= stop; ++i) {
                    rv->push(l->getReferencedEntry(i), xsink);
                }
            }
            else {
                for (int64 i = start; i >= stop; --i) {
                    rv->push(l->getReferencedEntry(i), xsink);
                }
            }
            return rv.release();
        }
        case NT_STRING: {
            if (empty)
                return new QoreStringNode;

            if (start < stop)
                return seq->get<const QoreStringNode>()->substr(start, stop - start + 1, xsink);

            SimpleRefHolder<QoreStringNode> tmp(seq->get<const QoreStringNode>()->reverse());
            return tmp->substr(seq_size - start - 1, start - stop + 1, xsink);
        }
        case NT_BINARY: {
            if (empty)
                return new BinaryNode;

            int64 length = start < stop ? stop - start + 1 : start - stop + 1;
            SimpleRefHolder<BinaryNode> bin(new BinaryNode);
            if (start < stop)
                bin->append(((char*)seq->get<const BinaryNode>()->getPtr()) + start, length);
            else {
                bin->preallocate(length);
                for (int64 i = start; i >= stop; --i) {
                    char* p = (char*)bin->getPtr() + start - i;
                    *p = ((char*)seq->get<const BinaryNode>()->getPtr())[i];
                }
            }
            return bin.release();
        }
    }

    return QoreValue();
}

FunctionalOperatorInterface* QoreSquareBracketsRangeOperatorNode::getFunctionalIteratorImpl(FunctionalValueType& value_type, ExceptionSink* xsink) const {
    value_type = list;

    ValueEvalRefHolder seq(e[0], xsink);
    if (*xsink)
        return nullptr;

    if (seq->getType() == NT_LIST) {
        int64 start, stop, seq_size;
        if (getEffectiveRange(*seq, start, stop, seq_size, xsink)) {
            if (!(runtime_get_parse_options() & PO_BROKEN_RANGE)) {
                if (start <= stop) {
                    ++stop;
                } else {
                    --stop;
                }
            }
            return new QoreFunctionalSquareBracketsRangeOperator(seq, start, stop, xsink);
        }
    }

    bool needs_deref;
    ValueHolder res(evalImpl(needs_deref, xsink), xsink);

    if (*xsink)
        return nullptr;
    if (res->getType() == NT_LIST) {
        value_type = list;
        return new QoreFunctionalListOperator(true, res.release().get<QoreListNode>(), xsink);
    }
    value_type = single;
    return new QoreFunctionalSingleValueOperator(res.release(), xsink);
}

bool QoreFunctionalSquareBracketsRangeOperator::getNextImpl(ValueOptionalRefHolder& val, ExceptionSink* xsink) {
    if (!next())
        return true;

    int64 i;
    {
        ValueHolder val(getValue(xsink), xsink);
        if (*xsink)
            return false;
        i = val->getAsBigInt();
    }

    switch (seq->getType()) {
        case NT_LIST:
            val.setValue(seq->get<const QoreListNode>()->getReferencedEntry(i), true);
            break;
        case NT_STRING:
            val.setValue(seq->get<const QoreStringNode>()->substr(i, 1, xsink), true);
            break;
        case NT_BINARY: {
            const BinaryNode* b = seq->get<const BinaryNode>();
            if (i < 0 || (size_t)i >= b->size())
                val.setValue(new BinaryNode, true);
            else {
                BinaryNode* bin = new BinaryNode;
                val.setValue(bin, true);
                bin->append((unsigned char*)b->getPtr() + i, 1);
            }
        }
    }
    return false;
}

// returns true iff the range is nonempty
bool QoreSquareBracketsRangeOperatorNode::getEffectiveRange(const QoreValue& seq, int64& start, int64& stop, int64& seq_size, ExceptionSink* xsink) const {
    ValueEvalRefHolder start_index(e[1], xsink);
    if (*xsink)
        return false;
    ValueEvalRefHolder stop_index(e[2], xsink);
    if (*xsink)
        return false;

    return getEffectiveRange(seq, start, stop, seq_size, *start_index, *stop_index, xsink);
}

bool QoreSquareBracketsRangeOperatorNode::getEffectiveRange(const QoreValue& seq, int64& start, int64& stop, int64& seq_size, const QoreValue& start_index, const QoreValue& stop_index, ExceptionSink* xsink) {
    qore_type_t seq_type = seq.getType();
    if (seq_type != NT_LIST && seq_type != NT_STRING && seq_type != NT_BINARY) {
        xsink->raiseException("ILLEGAL-EXPRESSION", "Index range can be applied only to lists, strings and binaries");
        return false;
    }

    switch (seq_type) {
        case NT_LIST:   seq_size = seq.get<const QoreListNode>()->size(); break;
        case NT_STRING: seq_size = seq.get<const QoreStringNode>()->size(); break;
        case NT_BINARY: seq_size = seq.get<const BinaryNode>()->size(); break;
    }

    bool no_start = start_index.isNothing(),
         no_stop = stop_index.isNothing();
    start = no_start ? 0 : start_index.getAsBigInt();
    stop = no_stop ? seq_size - 1 : stop_index.getAsBigInt();

    if ((no_start && stop < 0) || (no_stop && start > seq_size - 1))
        return false;

    if (start < stop) {
        if (start > seq_size - 1 || stop < 0)
            return false;

        if (start < 0)
            start = 0;
        if (seq_type != NT_LIST && stop > seq_size - 1)
            stop = seq_size - 1;
    } else {
        if (stop > seq_size - 1 || start < 0)
            return false;

        if (stop < 0)
            stop = 0;
        if (seq_type != NT_LIST && start > seq_size - 1)
            start = seq_size - 1;
    }
    return true;
}
