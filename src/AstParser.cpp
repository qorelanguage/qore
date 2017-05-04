/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  AstParser.cpp

  Qore AST Parser

  Copyright (C) 2017 Qore Technologies, s.r.o.

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
*/

#include "AstParser.h"

#include <iostream>

#include "ast/AST.h"
#include "AstTreePrinter.h"
#include "AstTreeSearcher.h"

typedef void *yyscan_t;
extern int yyparse(yyscan_t yyscanner, AstParseErrorLog* errorLog, ASTTree* parseTree);
extern struct yy_buffer_state* yy_scan_string(const char *, yyscan_t yyscanner);
extern struct yy_buffer_state* yy_create_buffer(FILE* file, int size, yyscan_t yyscanner);
extern void yy_delete_buffer(struct yy_buffer_state* buffer, yyscan_t yyscanner);
extern void yy_switch_to_buffer(struct yy_buffer_state* new_buffer, yyscan_t yyscanner);
extern int yylex_init(yyscan_t *yyscanner);
extern void yyset_in(FILE *in_str, yyscan_t yyscanner);
extern int yylex_destroy(yyscan_t yyscanner);
extern void yyset_lineno(int line_number, yyscan_t yyscanner);

//! Copied over from YY_BUF_SIZE from the generated flex scanner.
#define AST_BUF_SIZE 16384

int AstParser::parseFile(const char* filename) {
    // Prepare scanner.
    yyscan_t lexer;
    yylex_init(&lexer);

    // Open a file for reading.
    FILE* f = fopen(filename, "r");

    // Set up flex to scan the file.
    yy_buffer_state* buf = yy_create_buffer(f, AST_BUF_SIZE, lexer);
    yy_switch_to_buffer(buf, lexer);
    yyset_lineno(1, lexer);

    // Prepare an empty AST tree for holding the parsed tree.
    std::unique_ptr<ASTTree> tree(new ASTTree);

    // Parse.
    int rc = yyparse(lexer, this, tree.get());
    if (rc) {
        // ???
        return 1;
    }

    // Store the created tree;
    parsedTree = std::move(tree);

    //AstTreePrinter::printTree(std::cout, parsedTree.get());

    // Destroy buffer.
    yy_delete_buffer(buf, lexer);

    // Close the file.
    fclose(f);

    // Destroy scanner.
    yylex_destroy(lexer);
    return 0;
}

int AstParser::parseFile(std::string& filename) {
    return parseFile(filename.c_str());
}

int AstParser::parseString(const char* str) {
    // Prepare scanner.
    yyscan_t lexer;
    yylex_init(&lexer);

    // Set up flex to scan an in-memory string.
    yy_buffer_state* buf = yy_scan_string(str, lexer);
    yyset_lineno(1, lexer);

    // Prepare an empty AST tree for holding the parsed tree.
    std::unique_ptr<ASTTree> tree(new ASTTree);

    // Parse.
    int rc = yyparse(lexer, this, tree.get());
    if (rc) {
        // ???
        return 1;
    }

    // Store the created tree;
    parsedTree = std::move(tree);

    //AstTreePrinter::printTree(std::cout, parsedTree.get());

    // Destroy buffer.
    yy_delete_buffer(buf, lexer);

    // Destroy scanner.
    yylex_destroy(lexer);
    return 0;
}

int AstParser::parseString(std::string& str) {
    return parseString(str.c_str());
}

void AstParser::printTree(std::ostream& os) {
    AstTreePrinter::printTree(os, parsedTree.get());
}

ASTNode* AstParser::findNode(ast_loc_t line, ast_loc_t col) {
    if (!parsedTree) {
        return nullptr;
    }

    return AstTreeSearcher::findNode(parsedTree.get(), line, col);
}

std::vector<ASTNode*>* AstParser::findReferences(ast_loc_t line, ast_loc_t col, bool includeDecl) {
    ASTNode* node = AstTreeSearcher::findNode(parsedTree.get(), line, col);

    // Don't consider anything else other than names.
    if (!node || node->getNodeType() != ANT_Name)
        return nullptr;

    // Find the references.
    ASTName* name = static_cast<ASTName*>(node);
    std::unique_ptr<std::vector<ASTNode*> > vec(AstTreeSearcher::findReferences(parsedTree.get(), name->name));

    // Remove the initial declaration.
    if (vec && !includeDecl) {
        for (size_t i = 0, count = vec->size(); i < count; i++) {
            if (vec->at(i) == node) {
                vec->erase(vec->begin() + i);
                break;
            }
        }
    }
    return vec.release();
}

std::vector<ASTSymbolInfo>* AstParser::findSymbols() {
    return AstTreeSearcher::findSymbols(parsedTree.get());
}