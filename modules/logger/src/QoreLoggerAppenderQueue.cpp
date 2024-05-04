/* -*- mode: c++; indent-tabs-mode: nil -*- */
/** @file QoreLoggerAppenderQueue.cpp LoggerAppenderQueue class definition */
/*
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

#include "qore_logger.h"
#include "QoreLoggerAppenderQueue.h"
#include "QC_LoggerAppender.h"

//! Adds appender event
void QoreLoggerAppenderQueue::push(ExceptionSink* xsink, const QoreObject* appender, int64 type,
        const QoreValue params) {
    ReferenceHolder<QoreHashNode> h(new QoreHashNode(autoTypeInfo), nullptr);
    h->setKeyValue("appender", appender->refSelf(), nullptr);
    h->setKeyValue("type", type, nullptr);
    h->setKeyValue("params", params.refSelf(), nullptr);
    q.push(xsink, qobj, h.release());
}

void QoreLoggerAppenderQueue::process(int64 ms, ExceptionSink* xsink) {
    while (true) {
        ReferenceHolder<QoreHashNode> rec(getEvent(ms, xsink), xsink);
        if (!rec) {
            break;
        }
        QoreValue appender = rec->getKeyValue("appender", xsink);
        assert(!*xsink);
        assert(appender.getType() == NT_OBJECT);
        ReferenceHolder<QoreListNode> args(new QoreListNode(autoTypeInfo), xsink);
        args->push(rec->getKeyValue("type").refSelf(), xsink);
        args->push(rec->getKeyValue("params").refSelf(), xsink);
        appender.get<QoreObject>()->evalMethod("processEvent", *args, xsink).discard(xsink);
    }
}

QoreHashNode* QoreLoggerAppenderQueue::getEvent(int64 ms, ExceptionSink* xsink) {
    if (!q.size() && !ms) {
        return nullptr;
    }
    ValueHolder h(xsink);
    if (!ms) {
        h = q.shift(xsink, qobj, -1);
    } else if (ms > 0) {
        h = q.shift(xsink, qobj, (int)ms);
    } else {
        h = q.shift(xsink, qobj);
    }
    if (*xsink) {
        const QoreValue v = xsink->getExceptionErr();
        if (v.getType() == NT_STRING && *v.get<const QoreStringNode>() == "QUEUE-TIMEOUT") {
            xsink->clear();
        }
        return nullptr;
    }
    return h.releaseAs<QoreHashNode>();
}
