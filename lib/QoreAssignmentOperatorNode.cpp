/*
    QoreAssignmentOperatorNode.cpp

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

QoreString QoreAssignmentOperatorNode::op_str("assignment (=) operator expression");
QoreString QoreWeakAssignmentOperatorNode::op_str("weak assignment (:=) operator expression");

void QoreAssignmentOperatorNode::parseInitIntern(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo, bool weak_assignment) {
    // turn off "reference ok" and "return value ignored" flags
    pflag &= ~(PF_RETURN_VALUE_IGNORED);

    parse_init_value(left, oflag, pflag | PF_FOR_ASSIGNMENT, lvids, ti);
    // return type info is the same as the lvalue's typeinfo
    typeInfo = ti;

    //printd(5, "QoreAssignmentOperatorNode::parseInitImpl() this: %p left: %p '%s' nt: %d ti: %p '%s'\n", this, left, get_type_name(left), get_node_type(left), ti, QoreTypeInfo::getName(ti));
    checkLValue(left, pflag);

    // if "broken-int-assignments" is set, then set flag if applicable
    if ((ti == bigIntTypeInfo || ti == softBigIntTypeInfo)
        && (getProgram()->getParseOptions64() & PO_BROKEN_INT_ASSIGNMENTS))
        broken_int = true;

    const QoreTypeInfo* r = nullptr;
    parse_init_value(right, oflag, pflag, lvids, r);

    // check for illegal assignment to $self
    if (oflag)
        check_self_assignment(loc, left, oflag);

    //printd(5, "QoreAssignmentOperatorNode::parseInitImpl() this: %p left: %s ti: %p '%s', right: %s ti: %s\n", this, get_type_name(left), ti, QoreTypeInfo::getName(ti), get_type_name(right), QoreTypeInfo::getName(r));

    if (left.getType() == NT_VARREF && right.getType() == NT_VARREF
        && !strcmp(left.get<VarRefNode>()->getName(), right.get<VarRefNode>()->getName()))
        qore_program_private::makeParseException(getProgram(), *loc, "PARSE-EXCEPTION", new QoreStringNodeMaker("illegal assignment of variable \"%s\" to itself", left.get<VarRefNode>()->getName()));

    qore_type_result_e res;
    if (ti == autoTypeInfo) {
        if (pflag & PF_FOR_ASSIGNMENT) {
            ident = true;
        }
        res = QTI_IDENT;
    } else if (QoreTypeInfo::hasType(ti)) {
        bool may_not_match = false;
        bool may_need_filter = false;
        res = QoreTypeInfo::parseAccepts(ti, r, may_not_match, may_need_filter);
        // issue #2106 do not set the ident flag for any other type in case runtime types are more specific (complex) than parse types and require filtering
        //printd(5, "QoreAssignmentOperatorNode::parseInitImpl() '%s' <- '%s' res: %d may_not_match: %d may_need_filter: %d ident: %d\n", QoreTypeInfo::getName(ti), QoreTypeInfo::getName(r), res, may_not_match, may_need_filter, ident);
    } else {
        res = QTI_AMBIGUOUS;
    }

    if (getProgram()->getParseExceptionSink() && !res) {
        QoreStringNode* edesc = new QoreStringNodeMaker("lvalue for %sassignment operator '%s' expects ", weak_assignment ? "weak " : "", weak_assignment ? ":=" : "=");
        QoreTypeInfo::getThisType(ti, *edesc);
        edesc->concat(", but right-hand side is ");
        QoreTypeInfo::getThisType(r, *edesc);
        qore_program_private::makeParseException(getProgram(), *loc, "PARSE-TYPE-ERROR", edesc);
    }
}

QoreValue QoreAssignmentOperatorNode::evalIntern(ExceptionSink* xsink, bool& needs_deref, bool weak_assignment) const {
    /* assign new value, this value gets referenced with the
        eval(xsink) call, so there's no need to reference it again
        for the variable assignment - however it does need to be
        copied/referenced for the return value
    */
    ValueEvalRefHolder new_value(right, xsink);
    if (*xsink)
        return QoreValue();

    if (broken_int) {
        // convert the value to an int unconditionally
        new_value.setValue(new_value->getAsBigInt());
        if (*xsink)
            return QoreValue();
    }
    else {
        // we have to ensure that the value is referenced before the assignment in case the lvalue
        // is the same value, so it can be copied in the LValueHelper constructor
        new_value.ensureReferencedValue();
    }

    // get ptr to current value (lvalue is locked for the scope of the LValueHelper object)
    LValueHelper v(left, xsink);
    if (!v)
        return QoreValue();

    assert(!*xsink);

    // assign new value
    if (v.assign(new_value.takeReferencedValue(), "<lvalue>", !ident, weak_assignment))
        return QoreValue();

    // reference return value if necessary
    return ref_rv ? v.getReferencedValue() : QoreValue();
}
