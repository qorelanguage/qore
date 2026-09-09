/* Copyright (C) 2026 Qore Technologies, s.r.o.
 * SPDX-License-Identifier: MIT
 */
#include <qore/Qore.h>
#include <qore/intern/QoreJIT.h>

#include <cstdio>

static int check(const char* source) {
    ExceptionSink xsink;
    ExceptionSink warnings;
    QoreProgramHelper program(PO_MODERN | PO_ENABLE_DEBUG, xsink);
    program->setExecMode(QEM_JIT, true);
    program->parseFile(source, &xsink, &warnings);
    if (xsink || warnings) {
        xsink.handleExceptions();
        warnings.handleWarnings();
        return 1;
    }
    // The call graph has three levels: warmup, walker, and reference mutation.
    // Finish promotion at each level before the next fixed warmup pass.
    for (int phase = 0; phase < 3; ++phase) {
        ValueHolder warm(program->callFunction("warmRecursiveLocals", nullptr, &xsink), &xsink);
        if (xsink || warm->getAsBigInt() != 1) {
            xsink.handleExceptions();
            std::fputs("recursive-local warmup failed\n", stderr);
            return 1;
        }
        // The no-argument barrier finishes queued work; the per-Program teardown
        // overload would cancel queued work and cannot prove native execution.
        QoreJIT::instance().waitForBgCompileQueue();
    }
    for (int phase = 0; phase < 3; ++phase) {
        ValueHolder result(program->callFunction("checkRecursiveLocals", nullptr, &xsink), &xsink);
        if (xsink || result->getAsBigInt() != 1) {
            xsink.handleExceptions();
            std::fputs("recursive-local native checks failed\n", stderr);
            return 1;
        }
        QoreJIT::instance().waitForBgCompileQueue();
    }
    return 0;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::fputs("usage: recursive-closure-locals scenario.qr\n", stderr);
        return 2;
    }
    qore_init(QL_MIT, "UTF-8", true, QLO_DISABLE_SIGNAL_HANDLING);
    int status = check(argv[1]);
    qore_cleanup();
    if (!status) {
        std::puts("recursive-closure-locals: ok");
    }
    return status;
}
