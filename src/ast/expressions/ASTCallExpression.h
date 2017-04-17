/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  ASTCallExpression.h

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

#ifndef _QLS_AST_EXPRESSIONS_ASTCALLEXPRESSION_H
#define _QLS_AST_EXPRESSIONS_ASTCALLEXPRESSION_H

#include <memory>

#include "ast/ASTExpression.h"

class ASTCallExpression : public ASTExpression {
public:
    //! Pointer type.
    using Ptr = std::unique_ptr<ASTCallExpression>;

public:
    //! Target function or variable.
    ASTExpression::Ptr target;

    //! Call arguments.
    ASTExpression::Ptr args;

public:
    ASTCallExpression(ASTExpression* targ, ASTExpression* arglist) :
        ASTExpression(),
        target(targ),
        args(arglist)
    {
        loc.firstLine = targ->loc.firstLine;
        loc.firstCol = targ->loc.firstCol;
        loc.lastLine = arglist->loc.lastLine;
        loc.lastCol = arglist->loc.lastCol;
    }

    virtual Kind getKind() const override {
        return Kind::AEK_Call;
    }
};

#endif // _QLS_AST_EXPRESSIONS_ASTCALLEXPRESSION_H
