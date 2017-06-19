/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  GetNodesInfoQuery.h

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

#ifndef _QLS_QUERIES_GETNODESINFOQUERY_H
#define _QLS_QUERIES_GETNODESINFOQUERY_H

#include <vector>

#include "ast/ASTModifiers.h"
#include "ast/ASTName.h"
#include "ast/ASTOperator.h"

class ASTDeclaration;
class ASTExpression;
class ASTParseOption;
class ASTStatement;
class ASTTree;

class ExceptionSink;
class QoreHashNode;
class QoreListNode;
class QoreStringNode;

class GetNodesInfoQuery {
public:
    GetNodesInfoQuery() = delete;
    GetNodesInfoQuery(const GetNodesInfoQuery& other) = delete;

    //! Get info about nodes in the given tree.
    /**
        @param tree tree to search
        @return list of info about nodes
    */
    static QoreListNode* get(ASTTree* tree);

private:
    static QoreHashNode* getDeclaration(ASTDeclaration* decl, ExceptionSink* xsink);
    static QoreHashNode* getExpression(ASTExpression* expr, ExceptionSink* xsink);
    static QoreStringNode* getLocation(const ASTParseLocation& loc);
    static QoreStringNode* getModifiers(ASTModifiers& mods);
    static QoreHashNode* getName(ASTName& name, ExceptionSink* xsink);
    static QoreHashNode* getName(ASTName* name, ExceptionSink* xsink);
    static QoreStringNode* getOperator(ASTOperator& op);
    static QoreHashNode* getParseOption(ASTParseOption* po, ExceptionSink* xsink);
    static QoreHashNode* getStatement(ASTStatement* stmt, ExceptionSink* xsink);
};

#endif // _QLS_QUERIES_GETNODESINFOQUERY_H
