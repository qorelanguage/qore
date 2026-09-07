/* Copyright (C) 2026 Qore Technologies, s.r.o.
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */
#include <qore/Qore.h>

#ifdef NDEBUG
#undef NDEBUG
#endif
#include <cassert>
#include <cstdio>
#include <cstring>
#include <string>

static void check(const QoreEncoding* encoding, const char* first) {
    ExceptionSink xsink;
    std::string content = std::string("prefix:") + first + std::string(8192, ' ') + "!";
    SimpleRefHolder<QoreStringNode> source(new QoreStringNode(content, encoding));
    SimpleRefHolder<QoreStringNode> view(source->substr(7, &xsink));
    assert(!xsink && *view);

    // Use the QoreString API explicitly: QoreStringNode has separate view-aware overloads.
    const QoreString& base = **view;
    TempString head(base.substr(0, 1, &xsink));
    assert(!xsink && *head);
    assert(!strcmp(head->c_str(), first));
    TempString tail(base.substr(-1, &xsink));
    assert(!xsink && *tail);
    assert(!strcmp(tail->c_str(), "!"));
    TempString trimmed(base.substr(0, -1, &xsink));
    assert(!xsink && *trimmed);
    assert(trimmed->size() == strlen(first) + 8192);
    TempString outside(base.substr(10000, 1, &xsink));
    assert(!xsink && !*outside);
    TempString before(base.substr(-10000, &xsink));
    assert(!xsink && !*before);

    SimpleRefHolder<QoreStringNode> inside(source->substr(7, 8193, &xsink));
    assert(!xsink && *inside);
    TempString middle_head(static_cast<const QoreString&>(**inside).substr(0, 1, &xsink));
    assert(!xsink && *middle_head);
    assert(!strcmp(middle_head->c_str(), first));
    TempString middle_tail(static_cast<const QoreString&>(**inside).substr(-1, &xsink));
    assert(!xsink && *middle_tail);
    assert(!strcmp(middle_tail->c_str(), " "));
}

int main() {
    qore_init(QL_MIT);
    check(QCS_UTF8, "ž");
    check(QCS_ISO_8859_1, "x");
    qore_cleanup();
    puts("Passed substring-view C++ API checks");
    return 0;
}
