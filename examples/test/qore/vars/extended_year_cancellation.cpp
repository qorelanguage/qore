/* Copyright (C) 2026 Qore Technologies, s.r.o.
 * SPDX-License-Identifier: MIT
 */
#include <qore/Qore.h>
#include <string>

#ifdef NDEBUG
#undef NDEBUG
#endif
#include <cassert>
#include <cstdio>

static void checkStrings() {
    ExceptionSink xsink;
    QoreProgramHelper program(QoreParseOptions{}, xsink);
    assert(!xsink);
    program->setTZ(nullptr);
    QoreProgramContextHelper context(*program);
    for (const char* year : {"-2147483648", "-0400", "-0001", "0000", "12345", "2147483647"}) {
        std::string lexical = std::string(year) + "-03-01T12:30:45Z";
        ReferenceHolder<DateTimeNode> value(new DateTimeNode(nullptr, lexical.c_str(), &xsink), &xsink);
        assert(!xsink);
        std::string expected = std::string(year) + "0301123045";
        QoreString constructed(*value);
        assert(constructed == expected.c_str());
        QoreString appended("prefix:");
        appended.concat(*value);
        assert(appended == ("prefix:" + expected).c_str());
        // This API uses the program's zone, explicitly fixed to UTC above.
        QoreString iso;
        iso.concatISO8601DateTime(*value);
        assert(iso == (std::string(year) + "0301T12:30:45").c_str());
    }
    assert(!xsink);
}

static void checkCancellation() {
    ExceptionSink xsink;
    std::string lexical(1000, '0');
    lexical += "12345-03-01T12:30:45.123456Z";
    assert(!qore_cancel_thread(q_gettid(), "extended year cancellation test"));
    {
        ReferenceHolder<DateTimeNode> value(new DateTimeNode(nullptr, lexical.c_str(), &xsink), &xsink);
        assert(xsink);
        assert(xsink.getExceptionErr().get<const QoreStringNode>()->equal("THREAD-CANCELLED"));
    }
    qore_clear_thread_cancel();
    xsink.clear();
    {
        ReferenceHolder<DateTimeNode> value(new DateTimeNode(nullptr, lexical.c_str(), &xsink), &xsink);
        assert(!xsink);
        qore_tm info;
        value->getInfo(info);
        assert(info.year == 12345 && info.month == 3 && info.day == 1 && info.us == 123456);
    }
    assert(!xsink);
    {
        ReferenceHolder<DateTimeNode> value(new DateTimeNode(nullptr, "2147483647-12-31T00:00:00Z", &xsink), &xsink);
        assert(!xsink);
        int64 year;
        int week, day;
        value->getISOWeek(year, week, day);
        assert(year == 2147483648LL && week == 1 && day == 2);
    }
    assert(!xsink);
}

int main() {
    qore_init(QL_MIT, "UTF-8", true, QLO_DISABLE_SIGNAL_HANDLING);
    checkCancellation();
    checkStrings();
    qore_cleanup();
    std::puts("extended-year-cancellation: ok");
}
