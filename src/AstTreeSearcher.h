/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  AstTreeSearcher.h

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

#ifndef _QLS_ASTTREESEARCHER_H
#define _QLS_ASTTREESEARCHER_H

#include <string>
#include <vector>

#include "ast/ASTModifiers.h"
#include "ast/ASTName.h"
#include "ast/ASTOperator.h"
#include "ast/ASTParseLocation.h"

class ASTDeclaration;
class ASTExpression;
class ASTNode;
class ASTParseOption;
class ASTStatement;
class ASTTree;

class AstTreeSearcher {
public:
    AstTreeSearcher() = delete;
    AstTreeSearcher(const AstTreeSearcher& other) = delete;

public:
    static ASTNode* findNode(ASTTree* tree, ast_loc_t line, ast_loc_t col);
    static std::vector<ASTNode*>* findNodeAndParents(ASTTree* tree, ast_loc_t line, ast_loc_t col);
    static std::vector<ASTNode*>* findReferences(ASTTree* tree, const std::string& name);

private:
    static ASTNode* findNodeInDeclaration(ASTDeclaration* decl, ast_loc_t line, ast_loc_t col);
    static ASTNode* findNodeInExpression(ASTExpression* expr, ast_loc_t line, ast_loc_t col);
    static ASTNode* findNodeInName(ASTName& name, ast_loc_t line, ast_loc_t col);
    static ASTNode* findNodeInName(ASTName* name, ast_loc_t line, ast_loc_t col);
    static ASTNode* findNodeInParseOption(ASTParseOption* po, ast_loc_t line, ast_loc_t col);
    static ASTNode* findNodeInStatement(ASTStatement* stmt, ast_loc_t line, ast_loc_t col);

    static std::vector<ASTNode*>* findNodeAndParentsInDecl(ASTDeclaration* decl, ast_loc_t line, ast_loc_t col);
    static std::vector<ASTNode*>* findNodeAndParentsInExpr(ASTExpression* expr, ast_loc_t line, ast_loc_t col);
    static std::vector<ASTNode*>* findNodeAndParentsInName(ASTName& name, ast_loc_t line, ast_loc_t col);
    static std::vector<ASTNode*>* findNodeAndParentsInName(ASTName* name, ast_loc_t line, ast_loc_t col);
    static std::vector<ASTNode*>* findNodeAndParentsInPO(ASTParseOption* po, ast_loc_t line, ast_loc_t col);
    static std::vector<ASTNode*>* findNodeAndParentsInStmt(ASTStatement* stmt, ast_loc_t line, ast_loc_t col);

    static void findReferencesInDecl(std::vector<ASTNode*>* vec, ASTDeclaration* decl, const std::string& name);
    static void findReferencesInExpr(std::vector<ASTNode*>* vec, ASTExpression* expr, const std::string& name);
    static void findReferencesInName(std::vector<ASTNode*>* vec, ASTName& n, const std::string& name);
    static void findReferencesInName(std::vector<ASTNode*>* vec, ASTName* n, const std::string& name);
    static void findReferencesInStmt(std::vector<ASTNode*>* vec, ASTStatement* stmt, const std::string& name);
};

#endif // _QLS_ASTTREESEARCHER_H
