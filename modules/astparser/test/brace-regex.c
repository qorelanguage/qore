/* Copyright (C) 2026 Qore Technologies, s.r.o.
 * SPDX-License-Identifier: LGPL-2.1-or-later
 * Standalone grammar integration test, also usable under Valgrind without
 * loading the Qore runtime or its PCRE2 JIT dependency.
 */
#include <tree_sitter/api.h>

#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const TSLanguage* tree_sitter_qore(void);

static unsigned checks;

static TSNode find_node(TSNode node, const char* type) {
    if (!strcmp(ts_node_type(node), type)) {
        return node;
    }
    for (uint32_t i = 0; i < ts_node_named_child_count(node); ++i) {
        TSNode found = find_node(ts_node_named_child(node, i), type);
        if (!ts_node_is_null(found)) {
            return found;
        }
    }
    return (TSNode){0};
}

static void check(TSParser* parser, const char* source, const char* type, const char* token) {
    size_t length = strlen(source);
    assert(length <= UINT32_MAX);
    TSTree* tree = ts_parser_parse_string(parser, NULL, source, (uint32_t)length);
    assert(tree);
    TSNode root = ts_tree_root_node(tree);
    if (ts_node_has_error(root) != (type == NULL)) {
        fprintf(stderr, "Unexpected parse result: %s\n", source);
        abort();
    }
    if (type) {
        TSNode node = find_node(root, type);
        assert(!ts_node_is_null(node));
        uint32_t start = ts_node_start_byte(node);
        uint32_t end = ts_node_end_byte(node);
        assert(start <= end && end <= length);
        if (token) {
            assert(end - start == strlen(token));
            assert(!memcmp(source + start, token, end - start));
        }
    }
    ts_tree_delete(tree);
    ++checks;
}

int main(void) {
    TSParser* parser = ts_parser_new();
    assert(parser);
    bool language_set = ts_parser_set_language(parser, tree_sitter_qore());
    assert(language_set);

    check(parser, "value =~ m{^a{3}$}i;", "regex_literal", "m{^a{3}$}i");
    check(parser, "value =~ m{\\{abc\\}};", "regex_literal", "m{\\{abc\\}}");
    check(parser, "value =~ m{//abc};", "regex_literal", "m{//abc}");
    check(parser, "value =~ s{a/b} \n\t {žluťoučký // # /* */}g;", "regex_subst",
        "s{a/b} \n\t {žluťoučký // # /* */}g");
    check(parser, "value =~ tr{\\{\\}}{[]};", "regex_trans", "tr{\\{\\}}{[]}");
    check(parser, "value =~ x{(a{3})}g;", "regex_extract", "x{(a{3})}g");
    check(parser, "value !~ m{};", "regex_literal", "m{}");
    check(parser, "switch (value) { case m{abc}: break; }", "regex_literal", "m{abc}");
    check(parser, "value =~ /abc/;", "regex_literal", "/abc/");
    check(parser, "value =~ s/abc/new/;", "regex_subst", "s/abc/new/");
    check(parser, "value =~ tr/abc/xyz/;", "regex_trans", "tr/abc/xyz/");
    check(parser, "value =~ x/(abc)/;", "regex_extract", "x/(abc)/");

    const char* lookups[] = {"m{\"abc\"}", "s{\"abc\"}", "tr{\"abc\"}", "x{\"abc\"}"};
    for (unsigned i = 0; i < sizeof(lookups) / sizeof(*lookups); ++i) {
        char source[64];
        int size = snprintf(source, sizeof(source), "return %s;", lookups[i]);
        assert(size > 0 && (size_t)size < sizeof(source));
        check(parser, source, "index_expression", lookups[i]);
    }

    const char* invalid[] = {
        "value =~ m{", "value =~ m{abc\\", "value =~ m{{abc}",
        "value =~ s{abc}", "value =~ s{abc}{", "value =~ tr{abc}{xyz\\",
        "value =~ s{abc} /* comment */ {new};", "value =~ tr{abc} # comment\n{xyz};",
        "value =~ s{abc}\f{new};", "value =~ tr{abc}\v{xyz};",
        "value =~ m{abc}g;", "value =~ tr{abc}{xyz}g;", "value =~ x{abc}q;",
        "value !~ s{abc}{xyz};", "value !~ tr{abc}{xyz};", "value !~ x{abc};",
    };
    for (unsigned i = 0; i < sizeof(invalid) / sizeof(*invalid); ++i) {
        check(parser, invalid[i], NULL, NULL);
        check(parser, "value =~ s{abc}{};", "regex_subst", "s{abc}{}");
    }

    const char* prefix = "value =~ s{abc}{";
    size_t prefix_length = strlen(prefix);
    const size_t depth = 1024;
    const size_t body_length = 65536;
    size_t end = prefix_length + depth * 2 + body_length;
    char* large = malloc(end + 3);
    assert(large);
    memcpy(large, prefix, prefix_length);
    memset(large + prefix_length, '{', depth);
    memset(large + prefix_length + depth, 'x', body_length);
    memset(large + prefix_length + depth + body_length, '}', depth);
    memcpy(large + end, "};", 3);
    check(parser, large, "regex_subst", NULL);
    large[end] = '\0';
    check(parser, large, NULL, NULL);
    free(large);

    ts_parser_delete(parser);
    printf("Passed %u grammar integration checks\n", checks);
    return 0;
}
