/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  AstTreeSearcher.cpp

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

#include "AstTreeSearcher.h"

#include <memory>

#include "ast/AST.h"
#include "queries/FindHoverInfoQuery.h"
#include "queries/FindMatchingSymbolsQuery.h"
#include "queries/FindNodeQuery.h"
#include "queries/FindNodeAndParentsQuery.h"
#include "queries/FindReferencesQuery.h"
#include "queries/FindSymbolsQuery.h"

ASTHoverInfo AstTreeSearcher::findHoverInfo(ASTTree* tree, ast_loc_t line, ast_loc_t col) {
    return std::move(FindHoverInfoQuery::find(tree, line, col));
}

std::vector<ASTSymbolInfo>* AstTreeSearcher::findMatchingSymbols(ASTTree* tree, const std::string& query) {
    return FindMatchingSymbolsQuery::find(tree, query);
}

std::vector<ASTSymbolInfo>* AstTreeSearcher::findMatchingSymbols(const std::vector<ASTSymbolInfo>* symbols, const std::string& query) {
    return FindMatchingSymbolsQuery::find(symbols, query);
}

ASTNode* AstTreeSearcher::findNode(ASTTree* tree, ast_loc_t line, ast_loc_t col) {
    return FindNodeQuery::find(tree, line, col);
}

std::vector<ASTNode*>* AstTreeSearcher::findNodeAndParents(ASTTree* tree, ast_loc_t line, ast_loc_t col) {
    return FindNodeAndParentsQuery::find(tree, line, col);
}

std::vector<ASTNode*>* AstTreeSearcher::findReferences(ASTTree* tree, const std::string& name) {
    return FindReferencesQuery::find(tree, name);
}

std::vector<ASTSymbolInfo>* AstTreeSearcher::findSymbols(ASTTree* tree) {
    return FindSymbolsQuery::find(tree);
}