/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  FindNodeQuery.h

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

#ifndef _QLS_QUERIES_FINDNODEQUERY_H
#define _QLS_QUERIES_FINDNODEQUERY_H

#include "ast/ASTName.h"
#include "ast/ASTParseLocation.h"

class ASTDeclaration;
class ASTExpression;
class ASTNode;
class ASTParseOption;
class ASTStatement;
class ASTTree;

class FindNodeQuery {
public:
    FindNodeQuery() = delete;
    FindNodeQuery(const FindNodeQuery& other) = delete;

    static ASTNode* find(ASTTree* tree, ast_loc_t line, ast_loc_t col);

private:
    static ASTNode* inDeclaration(ASTDeclaration* decl, ast_loc_t line, ast_loc_t col);
    static ASTNode* inExpression(ASTExpression* expr, ast_loc_t line, ast_loc_t col);
    static ASTNode* inName(ASTName& name, ast_loc_t line, ast_loc_t col);
    static ASTNode* inName(ASTName* name, ast_loc_t line, ast_loc_t col);
    static ASTNode* inParseOption(ASTParseOption* po, ast_loc_t line, ast_loc_t col);
    static ASTNode* inStatement(ASTStatement* stmt, ast_loc_t line, ast_loc_t col);
};

#endif // _QLS_QUERIES_FINDNODEQUERY_H
