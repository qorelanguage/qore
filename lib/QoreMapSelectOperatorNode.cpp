/*
    QoreMapSelectOperatorNode.cpp

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

#include <memory>

QoreString QoreMapSelectOperatorNode::map_str("map select operator expression");

// if del is true, then the returned QoreString * should be mapd, if false, then it must not be
QoreString *QoreMapSelectOperatorNode::getAsString(bool &del, int foff, ExceptionSink *xsink) const {
   del = false;
   return &map_str;
}

int QoreMapSelectOperatorNode::getAsString(QoreString &str, int foff, ExceptionSink *xsink) const {
   str.concat(&map_str);
   return 0;
}

AbstractQoreNode* QoreMapSelectOperatorNode::parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
    assert(!typeInfo);

    pflag &= ~PF_RETURN_VALUE_IGNORED;

    // check iterator expression
    const QoreTypeInfo* iteratorTypeInfo = nullptr;
    e[1] = e[1]->parseInit(oflag, pflag, lvids, iteratorTypeInfo);

    const QoreTypeInfo* expTypeInfo = nullptr;
    const QoreTypeInfo* selectTypeInfo = nullptr;
    {
        // set implicit argument type
        ParseImplicitArgTypeHelper pia(QoreTypeInfo::getUniqueReturnComplexList(iteratorTypeInfo));
        // check iterated expression
        e[0] = e[0]->parseInit(oflag, pflag, lvids, expTypeInfo);

        // check select expression
        e[2] = e[2]->parseInit(oflag, pflag, lvids, selectTypeInfo);
    }

    // use lazy evaluation if the iterator expression supports it
    iterator_func = dynamic_cast<FunctionalOperator*>(e[1]);

    bool is_list = false;
    // if iterator is a list or an iterator, then the return type is a list, otherwise it's the return type of the iterated expression
    if (QoreTypeInfo::hasType(iteratorTypeInfo)) {
        if (QoreTypeInfo::isType(iteratorTypeInfo, NT_NOTHING)) {
            qore_program_private::makeParseWarning(getProgram(), *loc, QP_WARN_INVALID_OPERATION, "INVALID-OPERATION", "the iterator expression with the map operator (the second expression) has no value (NOTHING) and therefore this expression will also return no value; update the expression to return a value or use '%%disable-warning invalid-operation' in your code to avoid seeing this warning in the future");
            typeInfo = nothingTypeInfo;
        }
        else if (QoreTypeInfo::isType(iteratorTypeInfo, NT_LIST)) {
            typeInfo = iteratorTypeInfo;
            is_list = true;
        }
        else {
            const QoreClass* qc = QoreTypeInfo::getUniqueReturnClass(iteratorTypeInfo);
            if (qc && qore_class_private::parseCheckCompatibleClass(qc, QC_ABSTRACTITERATOR))
                typeInfo = autoListTypeInfo;
            else if ((QoreTypeInfo::parseReturns(iteratorTypeInfo, NT_LIST) == QTI_NOT_EQUAL)
                && (QoreTypeInfo::parseReturns(iteratorTypeInfo, QC_ABSTRACTITERATOR) == QTI_NOT_EQUAL)) {

                typeInfo = iteratorTypeInfo;
            }
        }
    }

    if (!QoreTypeInfo::hasType(expTypeInfo)) {
        expTypeInfo = autoTypeInfo;
    }

    //printd(5, "QoreMapSelectOperatorNode::parseInitImpl() r: '%s' e: '%s' i: '%s' arg: '%s' t: '%s'\n", QoreTypeInfo::getName(returnTypeInfo), QoreTypeInfo::getName(expTypeInfo), QoreTypeInfo::getName(iteratorTypeInfo), QoreTypeInfo::getName(QoreTypeInfo::getUniqueReturnComplexList(iteratorTypeInfo)), QoreTypeInfo::getName(typeInfo));
    if (is_list) {
        typeInfo = QoreMapOperatorNode::setReturnTypeInfo(returnTypeInfo, expTypeInfo, iteratorTypeInfo);
    }

    return this;
}

QoreValue QoreMapSelectOperatorNode::evalValueImpl(bool &needs_deref, ExceptionSink *xsink) const {
    FunctionalValueType value_type;
    std::unique_ptr<FunctionalOperatorInterface> f(getFunctionalIterator(value_type, xsink));
    if (*xsink || value_type == nothing)
        return QoreValue();

    ReferenceHolder<QoreListNode> rv(ref_rv && (value_type != single) ? new QoreListNode(f->getValueType()) : nullptr, xsink);

    while (true) {
        ValueOptionalRefHolder iv(xsink);
        if (f->getNext(iv, xsink)) {
            break;
        }

        if (*xsink) {
            return QoreValue();
        }

        if (value_type == single) {
            return ref_rv ? iv.takeReferencedValue() : QoreValue();
        }

        if (ref_rv) {
            rv->push(iv.takeReferencedValue(), nullptr);
        }
    }

    return rv.release();
}

FunctionalOperatorInterface* QoreMapSelectOperatorNode::getFunctionalIteratorImpl(FunctionalValueType& value_type, ExceptionSink* xsink) const {
    if (iterator_func) {
        std::unique_ptr<FunctionalOperatorInterface> f(iterator_func->getFunctionalIterator(value_type, xsink));
        if (*xsink || value_type == nothing)
            return 0;
        return new QoreFunctionalMapSelectOperator(this, f.release());
    }

    ValueEvalRefHolder marg(e[1], xsink);
    if (*xsink)
        return 0;

    qore_type_t t = marg->getType();
    if (t != NT_LIST) {
        if (t == NT_OBJECT) {
            AbstractIteratorHelper h(xsink, "map operator", const_cast<QoreObject*>(marg->get<const QoreObject>()));
            if (*xsink)
                return 0;
            if (h) {
                bool temp = marg.isTemp();
                marg.clearTemp();
                value_type = list;
                return new QoreFunctionalMapSelectIteratorOperator(this, temp, h, xsink);
            }
        }
        if (t == NT_NOTHING) {
            value_type = nothing;
            return 0;
        }

        value_type = single;
        return new QoreFunctionalMapSelectSingleValueOperator(this, marg.getReferencedValue(), xsink);
    }

    value_type = list;
    return new QoreFunctionalMapSelectListOperator(this, marg.takeReferencedNode<QoreListNode>(), xsink);
}

bool QoreFunctionalMapSelectListOperator::getNextImpl(ValueOptionalRefHolder& val, ExceptionSink* xsink) {
   while (true) {
      if (!next())
         return true;

      // set offset in thread-local data for "$#"
      ImplicitElementHelper eh(index());
      SingleArgvContextHelper argv_helper(getReferencedValue(), xsink);

      // check if value can be mapped
      ValueEvalRefHolder result(map->e[2], xsink);
      if (*xsink)
         return false;
      if (!result->getAsBool())
         continue;

      ValueEvalRefHolder tval(map->e[0], xsink);
      if (!*xsink) {
         tval.sanitize();
         tval.ensureReferencedValue();
         val.takeValueFrom(tval);
      }
      break;
   }
   return false;
}

bool QoreFunctionalMapSelectSingleValueOperator::getNextImpl(ValueOptionalRefHolder& val, ExceptionSink* xsink) {
   if (done)
      return true;

   done = true;

   SingleArgvContextHelper argv_helper(v, xsink);
   v.clear();

   // check if value can be mapped
   ValueEvalRefHolder result(map->e[2], xsink);
   if (*xsink)
      return false;
   if (!result->getAsBool())
      return true;

   ValueEvalRefHolder tval(map->e[0], xsink);
   if (!*xsink) {
      tval.sanitize();
      tval.ensureReferencedValue();
      val.takeValueFrom(tval);
   }
   return false;
}

bool QoreFunctionalMapSelectIteratorOperator::getNextImpl(ValueOptionalRefHolder& val, ExceptionSink* xsink) {
   while (true) {
      bool b = h.next(xsink);
      if (!b)
         return true;
      if (*xsink)
         return false;

      // set offset in thread-local data for "$#"
      ImplicitElementHelper eh(index++);

      // get current value
      ValueHolder iv(h.getValue(xsink), xsink);
      if (*xsink)
         return false;
      SingleArgvContextHelper argv_helper(iv.release(), xsink);

      // check if value can be mapped
      ValueEvalRefHolder result(map->e[2], xsink);
      if (*xsink)
         return false;
      if (!result->getAsBool())
         continue;

      ValueEvalRefHolder tval(map->e[0], xsink);
      if (!*xsink) {
         tval.sanitize();
         tval.ensureReferencedValue();
         val.takeValueFrom(tval);
      }
      break;
   }

   return false;
}

bool QoreFunctionalMapSelectOperator::getNextImpl(ValueOptionalRefHolder& val, ExceptionSink* xsink) {
   while (true) {
      // get current value
      ValueOptionalRefHolder iv(xsink);
      if (f->getNext(iv, xsink))
         return true;
      if (*xsink)
         return false;

      ImplicitElementHelper eh(index++);
      SingleArgvContextHelper argv_helper(iv.takeReferencedValue(), xsink);

      // check if value can be mapped
      ValueEvalRefHolder result(map->e[2], xsink);
      if (*xsink)
         return false;
      if (!result->getAsBool())
         continue;

      ValueEvalRefHolder tval(map->e[0], xsink);
      if (!*xsink) {
         tval.sanitize();
         tval.ensureReferencedValue();
         val.takeValueFrom(tval);
      }
      break;
   }
   return false;
}
