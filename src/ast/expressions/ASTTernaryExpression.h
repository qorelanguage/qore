/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  ASTTernaryExpression.h

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

#ifndef _QLS_AST_EXPRESSIONS_ASTTERNARYEXPRESSION_H
#define _QLS_AST_EXPRESSIONS_ASTTERNARYEXPRESSION_H

#include "ast/ASTExpression.h"

class ASTTernaryExpression : public ASTExpression {
public:
    //! Condition expression.
    ASTExpression::Ptr condition;

    //! Expression to return in case condition is true.
    ASTExpression::Ptr exprTrue;

    //! Expression to return in case condition is false.
    ASTExpression::Ptr exprFalse;

public:
    ASTTernaryExpression(ASTExpression* cond, ASTExpression* ifTrue, ASTExpression* ifFalse) :
        ASTExpression(),
        condition(cond),
        exprTrue(ifTrue),
        exprFalse(ifFalse)
    {
        loc.firstLine = cond->loc.firstLine;
        loc.firstCol = cond->loc.firstCol;
        loc.lastLine = ifFalse->loc.lastLine;
        loc.lastCol = ifFalse->loc.lastCol;
    }

    virtual Kind getKind() const override {
        return Kind::AEK_Ternary;
    }
};

#endif // _QLS_AST_EXPRESSIONS_ASTTERNARYEXPRESSION_H
