/*
    QoreAssignmentOperatorNode.cpp

    Qore Programming Language

    Copyright (C) 2003 - 2024 Qore Technologies, s.r.o.

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

int QoreAssignmentOperatorNode::parseInitIntern(QoreParseContext& parse_context, bool weak_assignment) {
    // turn off "reference ok" and "return value ignored" flags
    QoreParseContextFlagHelper fh(parse_context);
    fh.unsetFlags(PF_RETURN_VALUE_IGNORED);

    int err;
    {
        QoreParseContextFlagHelper fh0(parse_context);
        fh0.setFlags(PF_FOR_ASSIGNMENT);
        err = parse_init_value(left, parse_context);
    }
    // return type info is the same as the lvalue's typeinfo
    ti = parse_context.typeInfo;

    //printd(0, "QoreAssignmentOperatorNode::parseInitImpl() this: %p left: '%s' nt: %d ti: %p '%s'\n", this,
    //    left.getFullTypeName(), left.getType(), ti, QoreTypeInfo::getName(ti));
    if (!err && checkLValue(left, parse_context.pflag)) {
        err = -1;
    }

    // if "broken-int-assignments" is set, then set flag if applicable
    if ((ti == bigIntTypeInfo || ti == softBigIntTypeInfo)
        && (parse_context.pgm->getParseOptions64() & PO_BROKEN_INT_ASSIGNMENTS)) {
        broken_int = true;
    }

    parse_context.typeInfo = nullptr;
    if (parse_init_value(right, parse_context) && !err) {
        err = -1;
    }

    // check for illegal assignment to $self
    if (parse_context.oflag && check_self_assignment(loc, left, parse_context.oflag) && !err) {
        err = -1;
    }

    //printd(5, "QoreAssignmentOperatorNode::parseInitImpl() this: %p left: %s ti: %p '%s', right: %s ti: %s\n",
    //  this, get_type_name(left), ti, QoreTypeInfo::getName(ti), get_type_name(right),
    //  QoreTypeInfo::getName(parse_context.typeInfo));

    // issue #3337: make sure that the two varrefs are pointing to the same variable
    if (left.getType() == NT_VARREF && right.getType() == NT_VARREF
        && left.get<VarRefNode>()->parseEqualTo(*right.get<VarRefNode>())) {
        qore_program_private::makeParseException(parse_context.pgm, *loc, "PARSE-EXCEPTION",
            new QoreStringNodeMaker("illegal assignment of variable \"%s\" to itself",
                left.get<VarRefNode>()->getName()));
        if (!err) {
            err = -1;
        }
    }

    qore_type_result_e res;
    if (ti == autoTypeInfo) {
        if (parse_context.pflag & PF_FOR_ASSIGNMENT) {
            ident = true;
        }
        res = QTI_IDENT;
    } else if (QoreTypeInfo::hasType(ti)) {
        bool may_not_match = false;
        bool may_need_filter = false;
        qore_type_result_e max_result = QTI_NOT_EQUAL;
        // only set the initial assignment flag if the lvalue is a declaration
        bool initial_assignment = (left.getType() == NT_VARREF && left.get<VarRefNode>()->parseIsDecl());
        res = QoreTypeInfo::parseAccepts(ti, parse_context.typeInfo, may_not_match, may_need_filter, max_result,
            initial_assignment);
        // issue #2106 do not set the ident flag for any other type in case runtime types are more specific (complex)
        // than parse types and require filtering
        printd(5, "QoreAssignmentOperatorNode::parseInitImpl() '%s' <- '%s' res: %d may_not_match: %d " \
          "may_need_filter: %d ident: %d\n", QoreTypeInfo::getName(ti), QoreTypeInfo::getName(parse_context.typeInfo),
          res, may_not_match, may_need_filter, ident);
    } else {
        res = QTI_AMBIGUOUS;
    }

    if (parse_context.pgm->getParseExceptionSink() && !res) {
        QoreStringNode* edesc = new QoreStringNodeMaker("lvalue for %sassignment operator '%s' expects ",
            weak_assignment ? "weak " : "", weak_assignment ? ":=" : "=");
        QoreTypeInfo::getThisType(ti, *edesc);
        edesc->concat(", but right-hand side is ");
        QoreTypeInfo::getThisType(parse_context.typeInfo, *edesc);
        qore_program_private::makeParseException(parse_context.pgm, *loc, "PARSE-TYPE-ERROR", edesc);
        if (!err) {
            err = -1;
        }
    }

    parse_context.typeInfo = ti;
    return err;
}

QoreValue QoreAssignmentOperatorNode::evalIntern(ExceptionSink* xsink, bool& needs_deref,
        bool weak_assignment) const {
    /* assign new value, this value gets referenced with the
        eval(xsink) call, so there's no need to reference it again
        for the variable assignment - however it does need to be
        copied/referenced for the return value
    */
    ValueEvalOptimizedRefHolder new_value(right, xsink);
    if (*xsink)
        return QoreValue();

    if (broken_int) {
        // convert the value to an int unconditionally
        new_value.setValue(new_value->getAsBigInt());
        if (*xsink)
            return QoreValue();
    } else {
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
