/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  FindSymbolsQuery.h

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

#ifndef _QLS_QUERIES_FINDSYMBOLSQUERY_H
#define _QLS_QUERIES_FINDSYMBOLSQUERY_H

#include <vector>

#include "ast/ASTName.h"
#include "ast/ASTSymbolInfo.h"

class ASTDeclaration;
class ASTExpression;
class ASTStatement;
class ASTTree;

class FindSymbolsQuery {
public:
    FindSymbolsQuery() = delete;
    FindSymbolsQuery(const FindSymbolsQuery& other) = delete;

    static std::vector<ASTSymbolInfo>* findSymbols(ASTTree* tree);

private:
    static void findSymbolsInDecl(std::vector<ASTSymbolInfo>* vec, ASTDeclaration* decl);
    static void findSymbolsInExpr(std::vector<ASTSymbolInfo>* vec, ASTExpression* expr);
    static void findSymbolsInName(std::vector<ASTSymbolInfo>* vec, ASTName& name);
    static void findSymbolsInName(std::vector<ASTSymbolInfo>* vec, ASTName* name);
    static void findSymbolsInStmt(std::vector<ASTSymbolInfo>* vec, ASTStatement* stmt);
};

#endif // _QLS_QUERIES_FINDSYMBOLSQUERY_H
