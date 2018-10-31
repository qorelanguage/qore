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

#include <stdlib.h>

// check if "this" is valid in class member functions (cannot check "this" directly in g++ 4.9+ for example with optimization enabled)
static bool qore_check_this(const void* p) {
    assert(p);
    return p;
}

ExceptionSink::ExceptionSink() : priv(new qore_es_private) {
}

ExceptionSink::~ExceptionSink() {
    handleExceptions();
    delete priv;
}

void ExceptionSink::raiseThreadExit() {
    priv->thread_exit = true;
}

bool ExceptionSink::isEvent() const {
    return priv->head || priv->thread_exit;
}

bool ExceptionSink::isThreadExit() const {
    return priv->thread_exit;
}

bool ExceptionSink::isException() const {
    return priv->head;
}

// Intended as a alternative to isException():
// ExceptionSink xsink;
// if (xsink) { .. }
ExceptionSink::operator bool () const {
    assert(this);
    // FIXME: remove qore_check_this() in the next possible release of Qore
    return qore_check_this(this) && (priv->head || priv->thread_exit);
}

void ExceptionSink::overrideLocation(const QoreProgramLocation& loc) {
    QoreException *w = priv->head;
    while (w) {
        w->set(loc);
        w = w->next;
    }
}

QoreException *ExceptionSink::catchException() {
    QoreException *e = priv->head;
    priv->head = priv->tail = nullptr;
    return e;
}

QoreException *ExceptionSink::getException() {
    return priv->head;
}

void ExceptionSink::handleExceptions() {
    if (priv->head) {
        defaultExceptionHandler(priv->head);
        clear();
    }
    else
        priv->thread_exit = false;
}

void ExceptionSink::handleWarnings() {
    if (priv->head) {
        defaultWarningHandler(priv->head);
        clear();
    }
}

void ExceptionSink::clear() {
    priv->clearIntern();
    priv->head = priv->tail = nullptr;
    priv->thread_exit = false;
}

const QoreValue ExceptionSink::getExceptionErr() {
    return priv->head ? priv->head->err : QoreValue();
}

const QoreValue ExceptionSink::getExceptionDesc() {
    return priv->head ? priv->head->desc : QoreValue();
}

const QoreValue ExceptionSink::getExceptionArg() {
    return priv->head ? priv->head->arg : QoreValue();
}

int ExceptionSink::appendLastDescription(const char* fmt, ...) {
    if (!priv->head || priv->head->desc.getType() != NT_STRING) {
        return -1;
    }

    SimpleRefHolder<QoreStringNode> new_desc(new QoreStringNode);

    va_list args;

    while (true) {
        va_start(args, fmt);
        int rc = new_desc->vsprintf(fmt, args);
        va_end(args);
        if (!rc) {
            break;
        }
    }

    QoreStringNode* old_desc = priv->head->desc.get<QoreStringNode>();
    old_desc->concat(new_desc->c_str(), new_desc->size());
    return 0;
}

AbstractQoreNode* ExceptionSink::raiseException(const char *err, const char *fmt, ...) {
    QoreStringNode *desc = new QoreStringNode;

    va_list args;

    while (true) {
        va_start(args, fmt);
        int rc = desc->vsprintf(fmt, args);
        va_end(args);
        if (!rc)
        break;
    }
    printd(5, "ExceptionSink::raiseException(%s, %s)\n", err, desc->getBuffer());
    priv->insert(new QoreException(err, desc));
    return nullptr;
}

AbstractQoreNode* ExceptionSink::raiseErrnoException(const char *err, int en, QoreStringNode* desc) {
    // append strerror(en) to description
    desc->concat(": ");
    q_strerror(*desc, en);

    printd(5, "ExceptionSink::raiseException(%s, %s)\n", err, desc->getBuffer());
    priv->insert(new QoreException(err, desc, en));
    return nullptr;
}

AbstractQoreNode* ExceptionSink::raiseErrnoException(const char *err, int en, const char *fmt, ...) {
    QoreStringNode* desc = new QoreStringNode;

    va_list args;

    while (true) {
        va_start(args, fmt);
        int rc = desc->vsprintf(fmt, args);
        va_end(args);
        if (!rc)
        break;
    }

    return raiseErrnoException(err, en, desc);
}

// returns nullptr, takes ownership of the "desc" argument
AbstractQoreNode* ExceptionSink::raiseException(const char *err, QoreStringNode* desc) {
    printd(5, "ExceptionSink::raiseException(%s, %s)\n", err, desc->getBuffer());
    priv->insert(new QoreException(err, desc));
    return nullptr;
}

// returns nullptr, takes ownership of the "desc" argument
AbstractQoreNode* ExceptionSink::raiseException(QoreStringNode *err, QoreStringNode *desc) {
    printd(5, "ExceptionSink::raiseException(%s, %s)\n", err->c_str(), desc->c_str());
    priv->insert(new QoreException(err, desc));
    return nullptr;
}

AbstractQoreNode* ExceptionSink::raiseExceptionArg(const char* err, QoreValue arg, QoreStringNode *desc) {
    printd(5, "ExceptionSink::raiseExceptionArg(%s, %s)\n", err, desc->getBuffer());
    QoreException* exc = new QoreException(err, desc);
    exc->arg = arg;
    priv->insert(exc);
    return nullptr;
}

AbstractQoreNode* ExceptionSink::raiseExceptionArg(const char* err, QoreValue arg, QoreStringNode *desc, const QoreCallStack& stack) {
    printd(5, "ExceptionSink::raiseExceptionArg(%s, %s, %p)\n", err, desc->getBuffer(), &stack);
    QoreException* exc = new QoreException(err, desc);
    exc->arg = arg;
    priv->insert(exc);
    priv->addStackInfo(stack);
    return nullptr;
}

AbstractQoreNode* ExceptionSink::raiseExceptionArg(const QoreProgramLocation& loc, const char* err, QoreValue arg, QoreStringNode *desc, const QoreCallStack& stack) {
    printd(5, "ExceptionSink::raiseExceptionArg(loc, %s, %s, %p)\n", err, desc->getBuffer(), &stack);
    priv->insert(new QoreException(loc, err, desc, arg));
    priv->addStackInfo(stack);
    return nullptr;
}

AbstractQoreNode* ExceptionSink::raiseExceptionArg(const char* err, QoreValue arg, const char* fmt, ...) {
    QoreStringNode *desc = new QoreStringNode;

    va_list args;

    while (true) {
        va_start(args, fmt);
        int rc = desc->vsprintf(fmt, args);
        va_end(args);
        if (!rc)
            break;
    }
    printd(5, "ExceptionSink::raiseExceptionArg(%s, %s)\n", err, desc->getBuffer());
    QoreException* exc = new QoreException(err, desc);
    exc->arg = arg;
    priv->insert(exc);
    return nullptr;
}

void ExceptionSink::raiseException(QoreException* e) {
    priv->insert(e);
}

void ExceptionSink::raiseException(const QoreListNode* n) {
    priv->insert(new QoreException(n));
}

void ExceptionSink::raiseException(const QoreProgramLocation& loc, const char* err, QoreValue arg, QoreValue desc) {
    printd(5, "ExceptionSink::raiseExceptionArg(%s, %s)\n", err, desc.getType() == NT_STRING ? desc.get<QoreStringNode>()->c_str() : desc.getTypeName());
    priv->insert(new QoreException(loc, err, desc, arg));
}

void ExceptionSink::raiseException(const QoreProgramLocation &loc, const char *err, QoreValue arg, const char *fmt, ...) {
    QoreStringNode *desc = new QoreStringNode;

    va_list args;

    while (true) {
        va_start(args, fmt);
        int rc = desc->vsprintf(fmt, args);
        va_end(args);
        if (!rc)
            break;
    }

    raiseException(loc, err, arg, desc);
}

void ExceptionSink::rethrow(QoreException *old) {
    priv->insert(old->rethrow());
}

void ExceptionSink::assimilate(ExceptionSink* xs) {
    assimilate(*xs);
    delete xs;
}

void ExceptionSink::assimilate(ExceptionSink& xs) {
    if (xs.priv->thread_exit) {
        priv->thread_exit = xs.priv->thread_exit;
        xs.priv->thread_exit = false;
    }
    if (xs.priv->tail) {
        if (priv->tail)
        priv->tail->next = xs.priv->head;
        else
        priv->head = xs.priv->head;
        priv->tail = xs.priv->tail;
    }
    xs.priv->head = xs.priv->tail = nullptr;
}

void ExceptionSink::outOfMemory() {
#ifdef QORE_OOM
    // get pre-allocated out of memory exception for this thread
    QoreException* ex = getOutOfMemoryException();
    // if it's already been used then return
    if (!ex)
        return;
    ex->set(QoreProgramLocation(RuntimeLocation));
    // there is no callstack in an out-of-memory exception
    // add exception to list
    priv->insert(ex);
#else
    printf("OUT OF MEMORY: aborting\n");
    _Exit(1);
#endif
}
