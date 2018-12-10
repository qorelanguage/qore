/*
    QoreException.cpp

    Qore programming language exception handling support

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
#include "qore/intern/QoreHashNodeIntern.h"

#include <qore/safe_dslist>

#include <cassert>

#define Q_MAX_EXCEPTIONS 10

QoreExceptionBase::QoreExceptionBase(QoreValue n_err, QoreValue n_desc, QoreValue n_arg, qore_call_t n_type)
    : type(n_type), err(n_err), desc(n_desc), arg(n_arg) {
    // populate call stack
    const QoreStackLocation* w = get_runtime_stack_location();
    qore_call_t last_call_type;
    const char* last_call_name = nullptr;
    const QoreProgramLocation* last_loc = nullptr;
    while (w) {
        qore_call_t call_type = w->getCallType();
        const char* call_name = w->getCallName();
        const QoreProgramLocation& loc = w->getLocation();
        // only push if the location is not equal to the last one
        if (!last_loc || call_type != last_call_type || *last_loc != loc || strcmp(call_name, last_call_name)) {
            callStack->push(
                QoreException::getStackHash(call_type, nullptr, call_name, loc),
                nullptr
            );
        }
        last_call_type = call_type;
        last_call_name = call_name;
        last_loc = &loc;
        w = w->getNext();
    }
}

void QoreException::del(ExceptionSink* xsink) {
    if (callStack) {
        //printd(5, "QoreException::del() this: %p callStack: %p (r: %d)\n", this, callStack, callStack->reference_count());
        callStack->deref(xsink);
#ifdef DEBUG
        callStack = nullptr;
#endif
    }
    err.discard(xsink);
    desc.discard(xsink);
    arg.discard(xsink);
    if (next) {
        next->del(xsink);
    }

    delete this;
}

QoreHashNode* QoreException::makeExceptionObject() {
    QORE_TRACE("makeExceptionObject()");

    QoreHashNode* h = new QoreHashNode(hashdeclExceptionInfo, nullptr);
    auto ph = qore_hash_private::get(*h);

    ph->setKeyValueIntern("type", new QoreStringNode(type == CT_USER ? "User" : "System"));
    ph->setKeyValueIntern("file", new QoreStringNode(file));
    ph->setKeyValueIntern("line", start_line);
    ph->setKeyValueIntern("endline", end_line);
    ph->setKeyValueIntern("source", new QoreStringNode(source));
    ph->setKeyValueIntern("offset", offset);
    ph->setKeyValueIntern("callstack", callStack->refSelf());
    if (err) {
        ph->setKeyValueIntern("err", err.refSelf());
    }
    if (desc) {
        ph->setKeyValueIntern("desc", desc.refSelf());
    }
    if (arg) {
        ph->setKeyValueIntern("arg", arg.refSelf());
    }

    // add chained exceptions with this "chain reaction" call
    if (next)
        ph->setKeyValueIntern("next", next->makeExceptionObject());

    return h;
}

QoreHashNode *QoreException::makeExceptionObjectAndDelete(ExceptionSink *xsink) {
   QORE_TRACE("makeExceptionObjectAndDelete()");
   QoreHashNode* rv = makeExceptionObject();
   del(xsink);

   return rv;
}

void QoreException::addStackInfo(QoreHashNode* n) {
    //printd(5, "QoreException::addStackInfo() this: %p callStack: %p (r: %d) n: %p (nr: %d)\n", this, callStack, callStack->reference_count(), n, n->reference_count());
    callStack->push(n, nullptr);
}

const char* QoreException::getType(qore_call_t type) {
    switch (type) {
        case CT_USER:
            return "user";
        case CT_BUILTIN:
            return "builtin";
        case CT_RETHROW:
            return "rethrow";
        default:
            break;
    }
    assert(false);
    return 0;
}

// static function
QoreHashNode* QoreException::getStackHash(const QoreCallStackElement& cse) {
    QoreHashNode* h = new QoreHashNode;

    qore_hash_private* ph = qore_hash_private::get(*h);

    assert(!cse.code.empty());
    ph->setKeyValueIntern("function", new QoreStringNode(cse.code));
    ph->setKeyValueIntern("line",     cse.start_line);
    ph->setKeyValueIntern("endline",  cse.end_line);
    ph->setKeyValueIntern("file",     !cse.label.empty() ? new QoreStringNode(cse.label) : QoreValue());
    ph->setKeyValueIntern("source",   !cse.source.empty() ? new QoreStringNode(cse.source) : QoreValue());
    ph->setKeyValueIntern("offset",   cse.offset);
    ph->setKeyValueIntern("typecode", cse.type);
    ph->setKeyValueIntern("type",     new QoreStringNode(getType(cse.type)));

    return h;
}

// static function
QoreHashNode* QoreException::getStackHash(qore_call_t type, const char* class_name, const char* code,
    const QoreProgramLocation& loc) {
    QoreHashNode* h = new QoreHashNode;

    qore_hash_private* ph = qore_hash_private::get(*h);

    QoreStringNode *str = new QoreStringNode;
    if (class_name)
        str->sprintf("%s::", class_name);
    str->concat(code);

    //printd(5, "QoreException::getStackHash() %s at %s:%d-%d src: %s+%d\n", str->getBuffer(), loc.getFile() ? loc.getFile() : "n/a", loc.start_line, loc.end_line, loc.getSource() ? loc.getSource() : "n/a", loc.offset);

    ph->setKeyValueIntern("function", str);
    ph->setKeyValueIntern("line",     loc.start_line);
    ph->setKeyValueIntern("endline",  loc.end_line);
    ph->setKeyValueIntern("file",     loc.getFile() ? new QoreStringNode(loc.getFile()) : QoreValue());
    ph->setKeyValueIntern("source",   loc.getSource() ? new QoreStringNode(loc.getSource()) : QoreValue());
    ph->setKeyValueIntern("offset",   loc.offset);
    ph->setKeyValueIntern("typecode", type);
    ph->setKeyValueIntern("type",     new QoreStringNode(getType((qore_call_t)type)));

    return h;
}

DLLLOCAL ParseExceptionSink::~ParseExceptionSink() {
    if (xsink) {
        qore_program_private::addParseException(getProgram(), xsink);
    }
}
