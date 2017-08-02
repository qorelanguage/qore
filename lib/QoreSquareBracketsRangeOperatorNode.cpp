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

QoreString QoreSquareBracketsRangeOperatorNode::op_str("[m..n] operator expression");

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
    ValueEvalRefHolder h0(e[0], xsink);
    if (*xsink)
        return QoreValue();
    ValueEvalRefHolder h1(e[1], xsink);
    if (*xsink)
        return QoreValue();
    ValueEvalRefHolder h2(e[2], xsink);
    if (*xsink)
        return QoreValue();

    return doSquareBrackets(*h0, *h1, *h2, xsink);
}

QoreValue QoreSquareBracketsRangeOperatorNode::doSquareBrackets(QoreValue seq, QoreValue start_index, QoreValue stop_index, ExceptionSink* xsink) {
    qore_type_t seq_type = seq.getType();
    if (seq_type != NT_LIST && seq_type != NT_STRING && seq_type != NT_BINARY)
        return QoreValue();

    int64 start = (start_index.getType() == NT_NOTHING) ? 0 : start_index.getAsBigInt();
    int64 stop;
    int64 seq_size;
    switch (seq_type) {
        case NT_LIST:   seq_size = seq.get<const QoreListNode>()->size(); break;
        case NT_STRING: seq_size = seq.get<const QoreStringNode>()->size(); break;
        case NT_BINARY: seq_size = seq.get<const BinaryNode>()->size(); break;
    }

    if (stop_index.getType() == NT_NOTHING)
        stop = seq_size - 1;
    else
        stop = stop_index.getAsBigInt();

    bool empty = (start > seq_size - 1) || (stop < 0);
    if (!empty) {
        if (start < 0)
            start = 0;
        if (stop > seq_size - 1)
            stop = seq_size - 1;
        if (start > stop)
            empty = true;
    }

    switch (seq_type) {
        case NT_LIST: {
            QoreListNode* ret = new QoreListNode();
            if (!empty) {
                for (int64 i = start; i <= stop; i++) {
                    QoreValue entry = seq.get<const QoreListNode>()->get_referenced_entry(i);
                    ret->push(entry.getReferencedValue());
                }
            }
            return ret;
        }
        case NT_STRING:
            return empty ? new QoreStringNode() : seq.get<const QoreStringNode>() -> substr(start, stop - start + 1, xsink);
        case NT_BINARY: {
           if (empty)
               return new BinaryNode();

           int64 length = stop - start + 1;
           void* ptr = malloc(length);
           memcpy((unsigned char*)ptr, (unsigned char*)seq.get<const BinaryNode>()->getPtr() + start, length);
           return new BinaryNode(ptr, length);
        }
    }

    return QoreValue();
}
