/*
    QoreListAssignmentOperatorNode.cpp

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

#include "qore/intern/qore_program_private.h"

QoreString QoreListAssignmentOperatorNode::op_str("list assignment operator expression");

void QoreListAssignmentOperatorNode::parseInitImpl(QoreValue& val, LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo) {
    // turn off "reference ok" and "return value ignored" flags
    pflag &= ~(PF_RETURN_VALUE_IGNORED);

    assert(left.getType() == NT_PARSE_LIST);
    QoreParseListNode* l = left.get<QoreParseListNode>();

    QoreParseListNodeParseInitHelper li(l, oflag, pflag | PF_FOR_ASSIGNMENT, lvids);
    QorePossibleListNodeParseInitHelper ri(right, oflag, pflag, lvids);

    const QoreTypeInfo* argInfo;
    while (li.next()) {
        ri.next();

        const QoreTypeInfo* prototypeInfo;
        QoreValue v = li.parseInit(prototypeInfo);

        if (check_lvalue(v))
            parse_error(*loc, "expecting lvalue in position %d of left-hand-side list in list assignment, got '%s' instead", li.index() + 1, v.getTypeName());
        else if ((pflag & PF_BACKGROUND) && v.getType() == NT_VARREF && v.get<const VarRefNode>()->getType() == VT_LOCAL)
            parse_error(*loc, "illegal local variable modification with the background operator in position %d of left-hand-side list in list assignment", li.index() + 1);

        // check for illegal assignment to $self
        if (oflag)
            check_self_assignment(loc, v, oflag);

        ri.parseInit(argInfo);

        if (QoreTypeInfo::hasType(prototypeInfo)) {
            if (!QoreTypeInfo::parseAccepts(prototypeInfo, argInfo)) {
                // raise an exception only if parse exceptions are not disabled
                if (getProgram()->getParseExceptionSink()) {
                    QoreStringNode* edesc = new QoreStringNode("lvalue for ListAssignment operator in position ");
                    edesc->sprintf("%d of list ListAssignment expects ", li.index() + 1);
                    QoreTypeInfo::getThisType(prototypeInfo, *edesc);
                    edesc->concat(", but right-hand side is ");
                    QoreTypeInfo::getThisType(argInfo, *edesc);
                    qore_program_private::makeParseException(getProgram(), *loc, "PARSE-TYPE-ERROR", edesc);
                }
            }
        }
    }

    while (ri.next()) {
        ri.parseInit(argInfo);
    }
}

QoreValue QoreListAssignmentOperatorNode::evalImpl(bool& needs_deref, ExceptionSink* xsink) const {
    assert(left.getType() == NT_PARSE_LIST);
    const QoreParseListNode* llv = left.get<const QoreParseListNode>();

    /* assign new value, this value gets referenced with the
        eval(xsink) call, so there's no need to reference it again
        for the variable assignment - however it does need to be
        copied/referenced for the return value
    */
    ValueEvalRefHolder new_value(right, xsink);
    if (*xsink)
        return QoreValue();

    const QoreListNode* nv = (new_value->getType() == NT_LIST ? new_value->get<QoreListNode>() : nullptr);

    // get values and save
    for (unsigned i = 0; i < llv->size(); i++) {
        QoreValue lv = llv->get(i);

        // get ptr to current value (lvalue is locked for the scope of the LValueHelper object)
        LValueHelper v(lv, xsink);
        if (!v)
            return QoreValue();

        // if there's only one value, then save it
        if (nv) { // assign to list position
            v.assign(nv->getReferencedEntry(i));
        }
        else {
            if (!i) {
                v.assign(new_value->refSelf());
            }
            else
                v.assign(QoreValue());
        }
        if (*xsink)
            return QoreValue();
    }

    return ref_rv ? new_value.takeValue(needs_deref) : QoreValue();
}
