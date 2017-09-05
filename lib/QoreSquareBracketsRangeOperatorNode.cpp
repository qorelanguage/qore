/*
  QoreSquareBracketsRangeOperatorNode.cpp

  Qore Programming Language

  Copyright (C) 2003 - 2017 Qore Technologies, sro

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

AbstractQoreNode* QoreSquareBracketsRangeOperatorNode::parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&returnTypeInfo) {
    pflag &= ~PF_RETURN_VALUE_IGNORED;
    assert(!typeInfo);
    assert(!returnTypeInfo);

    const QoreTypeInfo *typeInfo0 = 0, *typeInfo1 = 0, *typeInfo2 = 0;
    e[0] = e[0]->parseInit(oflag, pflag, lvids, typeInfo0);
    e[1] = e[1]->parseInit(oflag, pflag, lvids, typeInfo1);
    e[2] = e[2]->parseInit(oflag, pflag, lvids, typeInfo2);

    if (QoreTypeInfo::hasType(typeInfo0)) {
         if (QoreTypeInfo::isType(typeInfo0, NT_LIST))
             returnTypeInfo = listTypeInfo;
         else if (QoreTypeInfo::isType(typeInfo0, NT_STRING))
             returnTypeInfo = stringTypeInfo;
         else if (QoreTypeInfo::isType(typeInfo0, NT_BINARY))
             returnTypeInfo = binaryTypeInfo;
    }

    typeInfo = returnTypeInfo;
    return this;
}

QoreValue QoreSquareBracketsRangeOperatorNode::evalValueImpl(bool& needs_deref, ExceptionSink* xsink) const {
    ValueEvalRefHolder seq(e[0], xsink);
    if (*xsink)
        return QoreValue();

    qore_type_t seq_type = seq->getType();
    int64 start, stop, seq_size;
    bool empty = !getEffectiveRange(start, stop, seq_size, xsink);
    if (*xsink)
        return QoreValue();

    switch (seq_type) {
        case NT_LIST: {
            if (empty)
                return new QoreListNode();

            ReferenceHolder<QoreListNode> rv(new QoreListNode(), xsink);
            if (start < stop) {
                for (int64 i = start; i <= stop; i++) {
                    QoreValue entry = seq->get<const QoreListNode>()->get_referenced_entry(i);
                    rv->push(entry.getReferencedValue());
                }
            }
            else {
                for (int64 i = start; i >= stop; i--) {
                    QoreValue entry = seq->get<const QoreListNode>()->get_referenced_entry(i);
                    rv->push(entry.getReferencedValue());
                }
            }
            return rv.release();
        }
        case NT_STRING:
            if (empty)
                return new QoreStringNode();

            if (start < stop)
                return seq->get<const QoreStringNode>() -> substr(start, stop - start + 1, xsink);

            return seq->get<const QoreStringNode>() -> reverse() -> substr(seq_size - start - 1, start - stop + 1, xsink);

        case NT_BINARY: {
            if (empty)
                return new BinaryNode();

            int64 length = start < stop ? stop - start + 1 : start - stop + 1;
            void* ptr = malloc(length);
            if (start < stop)
                memcpy((unsigned char*)ptr, (unsigned char*)seq->get<const BinaryNode>()->getPtr() + start, length);
            else {
                for (int64 i = start; i >= stop; i--)
                    memcpy((unsigned char*)ptr + start - i, (unsigned char*)seq->get<const BinaryNode>()->getPtr() + i, 1);
            }
            return new BinaryNode(ptr, length);
        }
    }

    return QoreValue();
}

FunctionalOperatorInterface* QoreSquareBracketsRangeOperatorNode::getFunctionalIteratorImpl(FunctionalValueType& value_type, ExceptionSink* xsink) const {
    value_type = list;

    int64 start, stop, seq_size;
    if (getEffectiveRange(start, stop, seq_size, xsink))
        return new QoreFunctionalSquareBracketsRangeOperator(e[0], start, stop, xsink);
    else
        return 0;
}

bool QoreFunctionalSquareBracketsRangeOperator::getNextImpl(ValueOptionalRefHolder& val, ExceptionSink* xsink) {
    if (!next())
        return true;

    ValueEvalRefHolder seq(node, xsink);
    if (*xsink)
        return false;

    int64 i = getValue(xsink)->getAsBigInt();
    if (*xsink)
        return false;

    switch (seq->getType()) {
        case NT_LIST:
            val.setValue(seq->get<const QoreListNode>()->get_referenced_entry(i), true);
            break;
        case NT_STRING:
            val.setValue(seq->get<const QoreStringNode>()->substr(i, 1, xsink), true);
            break;
        case NT_BINARY: {
            const BinaryNode* b = seq->get<const BinaryNode>();
            if (i < 0 || (size_t)i >= b->size())
                val.setValue(new BinaryNode(), true);

            void* ptr = malloc(1);
            memcpy((unsigned char*)ptr, (unsigned char*)b->getPtr() + i, 1);
            val.setValue(new BinaryNode(ptr, 1), true);
        }
    }
    return false;
}

// returns true iff the range is nonempty
bool QoreSquareBracketsRangeOperatorNode::getEffectiveRange(int64& start, int64& stop, int64& seq_size, ExceptionSink* xsink) const {
    ValueEvalRefHolder seq(e[0], xsink);
    if (*xsink)
        return false;
    ValueEvalRefHolder start_index(e[1], xsink);
    if (*xsink)
        return false;
    ValueEvalRefHolder stop_index(e[2], xsink);
    if (*xsink)
        return false;

    qore_type_t seq_type = seq->getType();
    if (seq_type != NT_LIST && seq_type != NT_STRING && seq_type != NT_BINARY) {
        xsink->raiseException("ILLEGAL-EXPRESSION", "Index range can be applied only to lists, strings and binaries");
        return false;
    }

    switch (seq_type) {
        case NT_LIST:   seq_size = seq->get<const QoreListNode>()->size(); break;
        case NT_STRING: seq_size = seq->get<const QoreStringNode>()->size(); break;
        case NT_BINARY: seq_size = seq->get<const BinaryNode>()->size(); break;
    }

    bool no_start = start_index->getType() == NT_NOTHING,
         no_stop = stop_index->getType() == NT_NOTHING;
    start = no_start ? 0 : start_index->getAsBigInt();
    stop = no_stop ? seq_size - 1 : stop_index->getAsBigInt();

    if ((no_start && stop < 0) || (no_stop && start > seq_size - 1))
        return false;

    if (start < stop) {
        if (start > seq_size - 1 || stop < 0)
            return false;

        if (start < 0)
            start = 0;
        if (stop > seq_size - 1)
            stop = seq_size - 1;
    }
    else {
        if (stop > seq_size - 1 || start < 0)
            return false;

        if (stop < 0)
            stop = 0;
        if (start > seq_size - 1)
            start = seq_size - 1;
    }
    return true;
}
