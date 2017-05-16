/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  AstParser.h

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

#ifndef _QLS_ASTPARSER_H
#define _QLS_ASTPARSER_H

#include <memory>
#include <ostream>
#include <string>
#include <vector>

#include "AstParseErrorLog.h"

#include "ast/ASTParseLocation.h"
#include "ast/ASTSymbolInfo.h"
#include "ast/ASTTree.h"

class AstParser : public AstParseErrorLog {
private:
    //! Parsed AST.
    std::unique_ptr<ASTTree> parsedTree;
    std::unique_ptr<std::vector<ASTSymbolInfo> > symbols;

public:
    AstParser() {}
    ~AstParser() {}

    int parseFile(const char* filename);
    int parseFile(std::string& filename);

    int parseString(const char* str);
    int parseString(std::string& str);

    void printTree(std::ostream& os);

    ASTSymbolInfo findHoverInfo(ast_loc_t line, ast_loc_t col);
    std::vector<ASTSymbolInfo>* findMatchingSymbols(const std::string& query);
    ASTNode* findNode(ast_loc_t line, ast_loc_t col);
    std::vector<ASTNode*>* findNodeAndParents(ast_loc_t line, ast_loc_t col);
    std::vector<ASTNode*>* findReferences(ast_loc_t line, ast_loc_t col, bool includeDecl);
    const std::vector<ASTSymbolInfo>* findSymbols();

    ASTTree* getTreePtr() { return parsedTree.get(); }
    ASTTree* releaseTree() { return parsedTree.release(); }
};

#endif // _QLS_ASTPARSER_H
