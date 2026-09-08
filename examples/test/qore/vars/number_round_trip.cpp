/* Copyright (C) 2026 Qore Technologies, s.r.o.
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */
#include <qore/Qore.h>

#ifdef NDEBUG
#undef NDEBUG
#endif
#include <cassert>
#include <iostream>
#include <string>

// Driven by number_round_trip.py: the oracle uses Python integer fractions and exact
// rounding intervals, independently of MPFR's decimal conversion implementation.
static void check(const std::string& input, unsigned precision) {
    ExceptionSink xsink;
    ReferenceHolder<QoreNumberNode> value(new QoreNumberNode(input.c_str(), precision), &xsink);
    assert(value->getPrec() == precision);
    QoreString plain;
    QoreString scientific;
    assert(!value->toStringRoundTrip(plain, false, &xsink) && !xsink);
    assert(!value->toStringRoundTrip(scientific, true, &xsink) && !xsink);
    ReferenceHolder<QoreNumberNode> restored(new QoreNumberNode(plain.c_str(), precision), &xsink);
    ReferenceHolder<QoreNumberNode> restored_scientific(new QoreNumberNode(scientific.c_str(), precision), &xsink);
    assert(value->equals(**restored));
    assert(value->equals(**restored_scientific));
    QoreString destination("prefix:");
    assert(!value->toStringRoundTrip(destination, false, &xsink) && !xsink);
    assert(std::string(destination.c_str()) == "prefix:" + std::string(plain.c_str()));
    std::cout << plain.c_str() << '\t' << scientific.c_str() << '\n';
}

static void checkCancellation() {
    for (const char* input : {"0", "-0", "NaN", "INF", "-INF", "1.00000000000000000001", "1e1000000"}) {
        ExceptionSink xsink;
        ReferenceHolder<QoreNumberNode> value(new QoreNumberNode(input), &xsink);
        QoreString destination("unchanged");
        // The native API permits setting the current thread's cancellation state. There
        // is no timing race or polling: cancellation is pending before entry to the formatter.
        assert(!qore_cancel_thread(q_gettid(), "round-trip formatting test"));
        assert(value->toStringRoundTrip(destination, false, &xsink) == -1);
        assert(xsink);
        assert(xsink.getExceptionErr().get<const QoreStringNode>()->equal("THREAD-CANCELLED"));
        assert(destination.equal("unchanged"));
        qore_clear_thread_cancel();
        xsink.clear();
        QoreString restored;
        assert(!value->toStringRoundTrip(restored, true, &xsink) && !xsink);
        assert(!restored.empty());
    }
}

int main() {
    qore_init(QL_MIT);
    checkCancellation();
    unsigned precision;
    std::string input;
    unsigned count = 0;
    while (std::cin >> precision >> input) {
        assert(precision >= 128 && precision <= 8192);
        check(input, precision);
        ++count;
    }
    assert(std::cin.eof() && count);
    qore_cleanup();
    return 0;
}
