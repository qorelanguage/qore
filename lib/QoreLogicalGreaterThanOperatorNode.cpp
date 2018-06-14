/*
    QoreLogicalGreaterThanOperatorNode.cpp

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

QoreString QoreLogicalGreaterThanOperatorNode::op_str("> operator expression");

QoreValue QoreLogicalGreaterThanOperatorNode::evalValueImpl(bool& needs_deref, ExceptionSink *xsink) const {
   if (pfunc)
      return (this->*pfunc)(xsink);

   ValueEvalRefHolder lh(left, xsink);
   if (*xsink)
      return QoreValue();
   ValueEvalRefHolder rh(right, xsink);
   if (*xsink)
      return QoreValue();

   return doGreaterThan(*lh, *rh, xsink);
}

AbstractQoreNode *QoreLogicalGreaterThanOperatorNode::parseInitIntern(const char *name, LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
    // turn off "reference ok" and "return value ignored" flags
    pflag &= ~(PF_RETURN_VALUE_IGNORED);

    typeInfo = boolTypeInfo;

    const QoreTypeInfo *lti = 0, *rti = 0;

    parse_init_value(left, oflag, pflag, lvids, lti);
    parse_init_value(right, oflag, pflag, lvids, rti);

    // see if both arguments are constants, then eval immediately and substitute this node with the result
    if (left.isValue() && right.isValue()) {
        SimpleRefHolder<QoreLogicalGreaterThanOperatorNode> del(this);
        ParseExceptionSink xsink;
        return get_bool_node(doGreaterThan(left, right, *xsink));
    }

    // check for optimizations based on type; but only if types are known on both sides, although the highest priority (float)
    // can be assigned if either side is a float
    if (!QoreTypeInfo::isType(lti, NT_NUMBER) && !QoreTypeInfo::isType(rti, NT_NUMBER)) {
        if (QoreTypeInfo::isType(lti, NT_FLOAT) || QoreTypeInfo::isType(rti, NT_FLOAT))
            pfunc = &QoreLogicalGreaterThanOperatorNode::floatGreaterThan;
        else if (QoreTypeInfo::hasType(lti) && QoreTypeInfo::hasType(rti)) {
            if (QoreTypeInfo::isType(lti, NT_INT)) {
                if (QoreTypeInfo::isType(rti, NT_INT))
                pfunc = &QoreLogicalGreaterThanOperatorNode::bigIntGreaterThan;
            }
            // FIXME: check for invalid operation here
        }
    }

    return this;
}

bool QoreLogicalGreaterThanOperatorNode::floatGreaterThan(ExceptionSink *xsink) const {
    ValueEvalRefHolder lh(left, xsink);
    if (*xsink) return false;
    ValueEvalRefHolder rh(right, xsink);
    if (*xsink) return false;

    return lh->getAsFloat() > rh->getAsFloat();
}

bool QoreLogicalGreaterThanOperatorNode::bigIntGreaterThan(ExceptionSink *xsink) const {
    ValueEvalRefHolder lh(left, xsink);
    if (*xsink) return false;
    ValueEvalRefHolder rh(right, xsink);
    if (*xsink) return false;

    return lh->getAsBigInt() > rh->getAsBigInt();
}

bool QoreLogicalGreaterThanOperatorNode::doGreaterThan(QoreValue lh, QoreValue rh, ExceptionSink* xsink) {
    qore_type_t lt = lh.getType();
    qore_type_t rt = rh.getType();

    if (lt == NT_NUMBER) {
        switch (rt) {
            case NT_NUMBER:
                return lh.get<const QoreNumberNode>()->greaterThan(*rh.get<const QoreNumberNode>());
            case NT_FLOAT:
                return lh.get<const QoreNumberNode>()->greaterThan(rh.getAsFloat());
            case NT_BOOLEAN:
            case NT_INT:
                return lh.get<const QoreNumberNode>()->greaterThan(rh.getAsBigInt());
            default: {
                ReferenceHolder<QoreNumberNode> rn(new QoreNumberNode(rh.getInternalNode()), xsink);
                return lh.get<const QoreNumberNode>()->greaterThan(**rn);
            }
        }
    }

    if (rt == NT_NUMBER) {
        assert(lt != NT_NUMBER);
        switch (lt) {
            case NT_FLOAT:
                return rh.get<const QoreNumberNode>()->lessThan(lh.getAsFloat());
            case NT_BOOLEAN:
            case NT_INT:
                return rh.get<const QoreNumberNode>()->lessThan(lh.getAsBigInt());
            default: {
                ReferenceHolder<QoreNumberNode> ln(new QoreNumberNode(lh.getInternalNode()), xsink);
                return rh.get<const QoreNumberNode>()->lessThan(**ln);
            }
        }
    }

    if (lt == NT_FLOAT || rt == NT_FLOAT)
        return lh.getAsFloat() > rh.getAsFloat();

    if (lt == NT_INT || rt == NT_INT)
        return lh.getAsBigInt() > rh.getAsBigInt();

    if (lt == NT_STRING || rt == NT_STRING) {
        QoreStringValueHelper ls(lh);
        QoreStringValueHelper rs(rh, ls->getEncoding(), xsink);
        if (*xsink)
            return false;
        return ls->compare(*rs) > 0;
    }

    if (lt == NT_DATE || rt == NT_DATE) {
        DateTimeValueHelper ld(lh);
        DateTimeValueHelper rd(rh);
        return DateTime::compareDates(*ld, *rd) > 0;
    }

    return lh.getAsFloat() > rh.getAsFloat();
}
