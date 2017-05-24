/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  ASTIfStatement.h

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

  Note that the Qore library is released under a choice of three open-source
  licenses: MIT (as above), LGPL 2+, or GPL 2+; see README-LICENSE for more
  information.
*/

#ifndef _QLS_AST_STATEMENTS_ASTIFSTATEMENT_H
#define _QLS_AST_STATEMENTS_ASTIFSTATEMENT_H

#include <memory>

#include "ast/ASTStatement.h"
#include "ast/ASTExpression.h"

class ASTIfStatement : public ASTStatement {
public:
    //! Pointer type.
    using Ptr = std::unique_ptr<ASTIfStatement>;

public:
    //! Condition.
    ASTExpression::Ptr condition;

    //! Code to execute when condition is true.
    ASTStatement::Ptr stmtThen;

    //! Code to execute when condition is false.
    ASTStatement::Ptr stmtElse;

public:
    ASTIfStatement(ASTExpression* cond, ASTStatement* sThen, ASTStatement* sElse) :
        ASTStatement(),
        condition(cond),
        stmtThen(sThen),
        stmtElse(sElse) {}

    virtual ~ASTIfStatement() {}

    virtual Kind getKind() const override {
        return Kind::ASK_If;
    }
};

#endif // _QLS_AST_STATEMENTS_ASTIFSTATEMENT_H
