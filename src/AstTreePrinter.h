/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  AstTreePrinter.h

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

#ifndef _QLS_ASTTREEPRINTER_H
#define _QLS_ASTTREEPRINTER_H

#include <ostream>

#include "ast/ASTModifiers.h"
#include "ast/ASTName.h"
#include "ast/ASTOperator.h"

class ASTDeclaration;
class ASTExpression;
class ASTNode;
class ASTStatement;
class ASTTree;

class AstTreePrinter {
public:
    static void printTree(std::ostream& os, ASTTree* tree);

private:
    static void printModifiers(std::ostream& os, ASTModifiers mods, int indent);
    static void printDeclaration(std::ostream& os, ASTDeclaration* decl, int indent);
    static void printExpression(std::ostream& os, ASTExpression* expr, int indent);
    static void printName(std::ostream& os, ASTName& name, int indent, bool newline = true, const char* prefix = "name: ");
    static void printOperator(std::ostream& os, ASTOperator op, int indent, bool newline);
    static void printStatement(std::ostream& os, ASTStatement* stmt, int indent);
    static void printNode(std::ostream& os, ASTNode* node, int indent);
};

#endif // _QLS_ASTTREEPRINTER_H
