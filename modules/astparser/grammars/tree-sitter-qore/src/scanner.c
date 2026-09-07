/* Copyright (C) 2026 Qore Technologies, s.r.o.
 * SPDX-License-Identifier: LGPL-2.1-or-later
 * Brace-delimited regex tokens follow lib/scanner.lpp.
 */
#include "tree_sitter/parser.h"

#include <stddef.h>
#include <stdint.h>

enum TokenType {
    BRACE_REGEX_MATCH,
    BRACE_REGEX_SUBST,
    BRACE_REGEX_TRANS,
    BRACE_REGEX_EXTRACT,
    TOKEN_COUNT,
};

static bool whitespace(int32_t c) {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

/* The grammar has already consumed the opening brace. No allocation or shared
 * state is needed; every successful iteration consumes source input.
 */
static bool body(TSLexer* lexer) {
    size_t depth = 1;
    while (!lexer->eof(lexer)) {
        int32_t c = lexer->lookahead;
        lexer->advance(lexer, false);
        if (c == '\\') {
            if (lexer->eof(lexer)) {
                return false;
            }
            lexer->advance(lexer, false);
        } else if (c == '{') {
            ++depth;
        } else if (c == '}') {
            if (!--depth) {
                return true;
            }
        }
    }
    return false;
}

void* tree_sitter_qore_external_scanner_create(void) {
    return NULL;
}

void tree_sitter_qore_external_scanner_destroy(void* payload) {
    (void)payload;
}

unsigned tree_sitter_qore_external_scanner_serialize(void* payload, char* buffer) {
    (void)payload;
    (void)buffer;
    return 0;
}

void tree_sitter_qore_external_scanner_deserialize(void* payload, const char* buffer, unsigned length) {
    (void)payload;
    (void)buffer;
    (void)length;
}

bool tree_sitter_qore_external_scanner_scan(void* payload, TSLexer* lexer, const bool* valid_symbols) {
    (void)payload;
    enum TokenType type = TOKEN_COUNT;
    for (unsigned i = 0; i < TOKEN_COUNT; ++i) {
        if (valid_symbols[i]) {
            if (type != TOKEN_COUNT) {
                // Tree-sitter enables every token during error recovery.
                return false;
            }
            type = (enum TokenType)i;
        }
    }
    if (type == TOKEN_COUNT || !body(lexer)) {
        return false;
    }
    if (type == BRACE_REGEX_SUBST || type == BRACE_REGEX_TRANS) {
        while (whitespace(lexer->lookahead)) {
            lexer->advance(lexer, false);
        }
        if (lexer->lookahead != '{') {
            return false;
        }
        lexer->advance(lexer, false);
        if (!body(lexer)) {
            return false;
        }
    }
    if (type != BRACE_REGEX_TRANS) {
        while (lexer->lookahead == 'i' || lexer->lookahead == 'm' || lexer->lookahead == 's'
                || lexer->lookahead == 'x' || lexer->lookahead == 'u' || lexer->lookahead == 'U'
                || (type != BRACE_REGEX_MATCH && lexer->lookahead == 'g')) {
            lexer->advance(lexer, false);
        }
    }
    lexer->mark_end(lexer);
    lexer->result_symbol = type;
    return true;
}
